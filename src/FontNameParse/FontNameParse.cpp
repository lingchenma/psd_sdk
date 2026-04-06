// the main include that always needs to be included in every translation unit that uses the PSD library
#include "../Psd/Psd.h"

// for convenience reasons, we directly include the platform header from the PSD library.
// we could have just included <Windows.h> as well, but that is unnecessarily big, and triggers lots of warnings.
//#include "../Psd/PsdPlatform.h"
#include <windows.h>
#include <string>
#include <sstream>

#include "../Psd/PsdMallocAllocator.h"
#include "../Psd/PsdNativeFile.h"

#include "../Psd/PsdDocument.h"
#include "../Psd/PsdColorMode.h"
#include "../Psd/PsdLayer.h"
#include "../Psd/PsdLayerMaskSection.h"
#include "../Psd/PsdExportEngineData.h"
#include "../Psd/PsdParseDocument.h"
#include "../Psd/PsdParseLayerMaskSection.h"
#include "../Psd/PsdParseImageResourcesSection.h"
#include "../Psd/PsdEncodeEngineData.h"
#include "../Psd/PsdExport.h"

using namespace psd;

//https://blog.csdn.net/yu57955/article/details/107294000
int setClipbar(std::wstring fontName) 
{
	const wchar_t* data = fontName.c_str();
	int contentSize = ((int)wcslen(data) + 1) * sizeof(wchar_t);
	HGLOBAL hMemory; LPTSTR lpMemory;
	if (!OpenClipboard(NULL)) return 0;/* 打开剪切板 */
	if (!EmptyClipboard()) return 0; /* 清空剪切板 */
	if (!(hMemory = GlobalAlloc(GMEM_MOVEABLE, contentSize))) return 0;/* 对剪切板分配内存 */
	if (!(lpMemory = (LPTSTR)GlobalLock(hMemory))) return 0;/* 锁定内存区域 */
	memcpy_s(lpMemory, contentSize, data, contentSize);    /* 复制数据到内存区域 */
	GlobalUnlock(hMemory);                                 /* 解除内存锁定 */
	if (!SetClipboardData(CF_UNICODETEXT, hMemory)) return 0;      /* 设置剪切板数据 */
	CloseClipboard();/* 关闭剪切板 */
	return 1;
}

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
std::wstring ParseFontName(const std::wstring srcPath)
{
	std::wstring fontName = L"";
	MallocAllocator allocator;
	NativeFile file(&allocator);

	// try opening the file. if it fails, bail out.
	if (!file.OpenRead(srcPath.c_str()))
	{
		MessageBox(NULL, L"Cannot open file.\n", L"Error", MB_OK);
	}

	// create a new document that can be used for extracting different sections from the PSD.
	// additionally, the document stores information like width, height, bits per pixel, etc.
	Document* document = CreateDocument(&file, &allocator);
	if (!document)
	{
		MessageBox(NULL, L"Cannot create document.\n", L"Error", MB_OK);
		file.Close();
	}

	// the sample only supports RGB colormode
	if (document->colorMode != colorMode::RGB)
	{
		MessageBox(NULL, L"Document is not in RGB color mode.\n", L"Error", MB_OK);
		DestroyDocument(document, &allocator);
		file.Close();
	}

	// extract image resources section.
	// this gives access to the ICC profile, EXIF data and XMP metadata.
	{
		ImageResourcesSection* imageResourcesSection = ParseImageResourcesSection(document, &file, &allocator);
		DestroyImageResourcesSection(imageResourcesSection, &allocator);
	}

	// extract all layers and masks.
	LayerMaskSection* layerMaskSection = ParseLayerMaskSection(document, &file, &allocator);
	if (layerMaskSection)
	{
		// extract all layers one by one. this should be done in parallel for maximum efficiency.
		for (unsigned int i = 0; i < layerMaskSection->layerCount; ++i)
		{
			Layer* layer = &layerMaskSection->layers[i];
			if (layer->ObjectType = (uint32_t)layerType::LayerObjectType::Text)
			{
				fontName = std::wstring((wchar_t*)layer->fontName);
				break;
			}
		}

		DestroyLayerMaskSection(layerMaskSection, &allocator);
	}
	// don't forget to destroy the document, and close the file.
	DestroyDocument(document, &allocator);
	file.Close();

	return fontName;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance, LPSTR lpCmdLine, int nShowCmd)
{
    LPWSTR* szArgList;
    int argCount;

    szArgList = CommandLineToArgvW(GetCommandLine(), &argCount);
    if (szArgList == NULL)
    {
        MessageBox(NULL, L"Unable to parse command line", L"Error", MB_OK);
        return 10;
    }

#if _DEBUG
	std::wstring path = L"../../bin/TextLayer.psd";
#else
	if (argCount < 2)
	{
		MessageBox(NULL, L"请拖入Psd文件", L"Error", MB_OK);
		return 10;
	}

	std::wstring path = szArgList[1];
#endif


    LocalFree(szArgList);

	std::wstring fontName = ParseFontName(path);
	if (fontName.empty())
	{
		MessageBox(NULL, L"unknow error", L"Error", MB_OK);
		return 1;
	}
    setClipbar(fontName);
	std::wstring msg = std::wstring(L"字体名\"") + fontName + L"\"已写入剪切板";
	MessageBox(NULL, msg.c_str(), L"成功", MB_OK);
    return 0;
}