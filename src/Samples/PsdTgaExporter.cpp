// Copyright 2011-2020, Molecular Matters GmbH <office@molecular-matters.com>
// See LICENSE.txt for licensing details (2-clause BSD License: https://opensource.org/licenses/BSD-2-Clause)
#include "PsdTgaExporter.h"

#include "../Psd/PsdPlatform.h"
#include "PsdDebug.h"
#include "../libpng-libpng16/include/png.h"
#include "../svpng/svpng.inc"

#include <stdlib.h>
#include<iostream>
#include <io.h>
#if defined(__APPLE__)
#include <codecvt>
#include <locale>

#endif

#if defined( __linux)
#include <cwchar>
#include <cstring>
#include <cstdlib>

#endif

namespace
{
	struct TgaType
	{
		enum Enum
		{
			BGR_UNCOMPRESSED = 2,			// TGA file contains BGR triplets of color data.
			MONO_UNCOMPRESSED = 3			// TGA file contains grayscale values.
		};
	};


#pragma pack(push, 1)
	struct TgaHeader
	{
		uint8_t idLength;
		uint8_t paletteType;
		uint8_t type;
		uint16_t paletteOffset;
		uint16_t paletteLength;
		uint8_t bitsPerPaletteEntry;
		uint16_t originX;
		uint16_t originY;
		uint16_t width;
		uint16_t height;
		uint8_t bitsPerPixel;
		uint8_t attributes;
	};
#pragma pack(pop)

	struct FRetInfo {

		FRetInfo()
		{
			IsSusscc = false;
			fp = NULL;
			png_ptr = NULL;
			info_ptr = NULL;
		}
		operator bool()const {
			if (fp == NULL) {
				return false;
			}
			return true;
		}
		FILE* fp;
		png_structp png_ptr;
		png_infop info_ptr;
		bool IsSusscc;
		std::string errorstr;

	};
	// ---------------------------------------------------------------------------------------------------------------------
	// ---------------------------------------------------------------------------------------------------------------------
	static TgaHeader CreateHeader(unsigned int width, unsigned int height, TgaType::Enum type, uint8_t bitsPerPixel)
	{
		TgaHeader header;
		header.idLength = 0u;
		header.paletteType = 0u;
		header.type = static_cast<uint8_t>(type);
		header.paletteOffset = 0u;
		header.paletteLength = 0u;
		header.bitsPerPaletteEntry = 0u;
		header.originX = 0u;
		header.originY = 0u;
		header.width = static_cast<uint16_t>(width);
		header.height = static_cast<uint16_t>(height);
		header.bitsPerPixel = bitsPerPixel;
		header.attributes = 0x20u;

		return header;
	}

	struct FRetInfo* InitSaveInfoFile(const wchar_t* filename, unsigned int width, unsigned int height)
	{
		struct FRetInfo* RetValue = new FRetInfo();

#undef CreateFile
		FILE* fp = tgaExporter::CreateFile(filename);
		
		if (fp == NULL)
		{
			return RetValue;
		}
		RetValue->fp = fp;
		png_structp png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
		if (!png_ptr) {
			// 处理 png 结构创建失败的情况
			fclose(fp);
			RetValue->fp = NULL;
			return RetValue;
		}
		RetValue->png_ptr = png_ptr;
		png_infop info_ptr = png_create_info_struct(png_ptr);
		if (!info_ptr) {
			// 处理 png 信息结构创建失败的情况
			png_destroy_write_struct(&png_ptr, (png_infopp)NULL);
			fclose(fp);
			
			return RetValue;
		}
		RetValue->info_ptr = info_ptr;
		// 设置错误处理回调（如果需要的话）
		if (setjmp(png_jmpbuf(png_ptr))) {
			// 处理发生错误的情况
			png_destroy_write_struct(&png_ptr, &info_ptr);
			fclose(fp);
			return RetValue;
		}

		// 初始化写入操作
		png_init_io(png_ptr, fp);
		RetValue->IsSusscc = true;
		return RetValue;
	}
}


namespace tgaExporter
{
	// ---------------------------------------------------------------------------------------------------------------------
	// ---------------------------------------------------------------------------------------------------------------------
	FILE* CreateFile(const wchar_t* filename)
	{
		FILE* file = nullptr;
#if PSD_USE_MSVC
		const errno_t success = _wfopen_s(&file, filename, L"wb");
        if (success != 0)
#elif defined(__APPLE__)
        std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>,wchar_t> convert;
        std::string s = convert.to_bytes(filename);
        char const *cs = s.c_str();
        file = fopen(cs, "wb");
        if (file == NULL)
#elif defined(__linux)
		//In Linux
		char *buffer;
		size_t n = std::wcslen(filename) * 4 + 1;
		buffer = static_cast<char*>(std::malloc(n));
		std::memset(buffer,0,n);
		std::wcstombs(buffer,filename,n);
		file = fopen(buffer,"wb");
		std::free(buffer);
		if (file == NULL)
#endif
		{
			OutputDebugStringA("Cannot create file for writing.");
			return nullptr;
		}

		return file;
	}


	// ---------------------------------------------------------------------------------------------------------------------
	// ---------------------------------------------------------------------------------------------------------------------
	void SaveMonochrome(const wchar_t* filename, unsigned int width, unsigned int height, const uint8_t* data)
	{
		FILE* file = CreateFile(filename);
		if (!file)
		{
			return;
		}

		const TgaHeader& header = CreateHeader(width, height, TgaType::MONO_UNCOMPRESSED, 8u);
		fwrite(&header, sizeof(TgaHeader), 1u, file);
		fwrite(data, sizeof(char), width*height, file);
		fclose(file);
	}


	// ---------------------------------------------------------------------------------------------------------------------
	// ---------------------------------------------------------------------------------------------------------------------
	void SaveRGB(const wchar_t* filename, unsigned int width, unsigned int height, const uint8_t* data)
	{
		FILE* file = CreateFile(filename);
		if (!file)
		{
			return;
		}

		const TgaHeader& header = CreateHeader(width, height, TgaType::BGR_UNCOMPRESSED, 24u);
		fwrite(&header, sizeof(TgaHeader), 1u, file);

		uint8_t* colors = new uint8_t[width*height*3u];
		for (unsigned int i=0u; i<height; ++i)
		{
			for (unsigned int j=0u; j<width; ++j)
			{
				const uint8_t r = data[(i*width + j) * 4u + 0u];
				const uint8_t g = data[(i*width + j) * 4u + 1u];
				const uint8_t b = data[(i*width + j) * 4u + 2u];

				colors[(i*width + j) * 3u + 2u] = r;
				colors[(i*width + j) * 3u + 1u] = g;
				colors[(i*width + j) * 3u + 0u] = b;
			}
		}

		fwrite(colors, sizeof(uint8_t)*3u, width*height, file);
		delete[] colors;
		fclose(file);
	}


	// ---------------------------------------------------------------------------------------------------------------------
	// ---------------------------------------------------------------------------------------------------------------------
	void SaveRGBA(const wchar_t* filename, unsigned int width, unsigned int height, const uint8_t* data)
	{
		FILE* file = CreateFile(filename);
		if (!file)
		{
			return;
		}

		const TgaHeader& header = CreateHeader(width, height, TgaType::BGR_UNCOMPRESSED, 32u);
		fwrite(&header, sizeof(TgaHeader), 1u, file);

		uint8_t* colors = new uint8_t[width*height*4u];
		for (unsigned int i=0u; i < height; ++i)
		{
			for (unsigned int j=0u; j < width; ++j)
			{
				const uint8_t r = data[(i*width + j) * 4u + 0u];
				const uint8_t g = data[(i*width + j) * 4u + 1u];
				const uint8_t b = data[(i*width + j) * 4u + 2u];
				const uint8_t a = data[(i*width + j) * 4u + 3u];

				colors[(i*width + j) * 4u + 2u] = r;
				colors[(i*width + j) * 4u + 1u] = g;
				colors[(i*width + j) * 4u + 0u] = b;
				colors[(i*width + j) * 4u + 3u] = a;
			}
		}

		fwrite(colors, sizeof(uint8_t)*4u, width*height, file);
		delete[] colors;
		fclose(file);
	}

	
	// ---------------------------------------------------------------------------------------------------------------------
	// ---------------------------------------------------------------------------------------------------------------------
	void SaveSvPngRGBA(const wchar_t* filename, unsigned int width, unsigned int height, const uint8_t* data)
	{
		FILE* file = CreateFile(filename);
		if (!file)
		{
			return;
		}
		//const TgaHeader& header = CreateHeader(width, height, TgaType::BGR_UNCOMPRESSED, 32u);
		//fwrite(&header, sizeof(TgaHeader), 1u, file);
		svpng(file, width, height, data, 1);
		fclose(file);
	}

	void SaveSvPngRGB(const wchar_t* filename, unsigned int width, unsigned int height, const uint8_t* data)
	{
		FILE* file = CreateFile(filename);
		if (!file)
		{
			return;
		}
		//const TgaHeader& header = CreateHeader(width, height, TgaType::BGR_UNCOMPRESSED, 32u);
		//fwrite(&header, sizeof(TgaHeader), 1u, file);
		svpng(file, width, height, data, 0);
		fclose(file);
	}
	
	void SavelibPngRGBA(const wchar_t* filename, unsigned int width, unsigned int height, const uint8_t* data)
	{
		struct FRetInfo* retvalu = InitSaveInfoFile(filename, width, height);

		const int bit_depth = 8;
		int color_type = PNG_COLOR_TYPE_RGBA;

		png_set_IHDR(retvalu->png_ptr, retvalu->info_ptr, width, height, bit_depth, color_type, PNG_INTERLACE_NONE,
			PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);

		png_bytep* row_pointers = (png_bytep*)malloc(height * sizeof(png_bytep));
		
		for (unsigned int y = 0; y < height; y++) {
			
			row_pointers[y] = const_cast<png_bytep>(data+y *width*4); // RGBA 图片数据（每个像素 4 字节）
			
		}
		
		png_set_rows(retvalu->png_ptr, retvalu->info_ptr, row_pointers);
		
		png_write_png(retvalu->png_ptr, retvalu->info_ptr, PNG_TRANSFORM_IDENTITY, NULL);
		free(row_pointers);
		png_destroy_write_struct(&retvalu->png_ptr, &retvalu->info_ptr);
		fclose(retvalu->fp);
		delete retvalu;
	}
	
	void SavelibPngRGBA(const wchar_t* filename, unsigned int width, unsigned int height, const uint16_t* data)
	{
		
		struct FRetInfo* retvalu = InitSaveInfoFile(filename, width, height);
		const int bit_depth = 8;
		int color_type = PNG_COLOR_TYPE_RGBA;

		png_set_IHDR(retvalu->png_ptr, retvalu->info_ptr, width, height, bit_depth, color_type, PNG_INTERLACE_NONE,
			PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);

		png_uint_16p* row_pointers = (png_uint_16p*)malloc(height * sizeof(png_uint_16p));
		/*for (int i; i < width; i++) {

		}*/
		for (unsigned int y = 0; y < height; y++) {

			row_pointers[y] = const_cast<png_uint_16p>(data + y * width * 4); // RGBA 图片数据（每个像素 4 字节）

		}

		png_get_hIST(retvalu->png_ptr, retvalu->info_ptr, row_pointers);

		png_write_png(retvalu->png_ptr, retvalu->info_ptr, PNG_TRANSFORM_IDENTITY, NULL);
		free(row_pointers);
		png_destroy_write_struct(&retvalu->png_ptr, &retvalu->info_ptr);
		fclose(retvalu->fp);

	}
	// 
	void SavelibPngRGBA(const wchar_t* filename, unsigned int width, unsigned int height, const float32_t* data)
	{
		struct FRetInfo* retvalu = InitSaveInfoFile(filename, width, height);
		const int bit_depth = 8;
		int color_type = PNG_COLOR_TYPE_RGBA;

		png_set_IHDR(retvalu->png_ptr, retvalu->info_ptr, width, height, bit_depth, color_type, PNG_INTERLACE_NONE,
			PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);

		png_uint_32p* row_pointers = (png_uint_32p*)malloc(height * sizeof(png_uint_32p));
		/*for (int i; i < width; i++) {

		}*/
		for (unsigned int y = 0; y < height; y++) {

			// row_pointers[y] = const_cast<png_uint_32p>(data + y * width * 4); // RGBA 图片数据（每个像素 4 字节）

		}
		free(row_pointers);
		/*png_get_hIST(retvalu.png_ptr, retvalu.info_ptr, row_pointers);

		png_write_png(retvalu.png_ptr, retvalu.info_ptr, PNG_TRANSFORM_IDENTITY, NULL);

		png_destroy_write_struct(&retvalu.png_ptr, &retvalu.info_ptr);
		fclose(retvalu.fp);*/
	}
	void SavelibPngRGB(const wchar_t* filename, unsigned int width, unsigned int height, const uint8_t* data)
	{
		FILE* fp = CreateFile(filename);
		if (!fp)
		{
			return;
		}
		png_structp png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
		if (!png_ptr) {
			// 处理 png 结构创建失败的情况
			fclose(fp);
			return;
		}

		png_infop info_ptr = png_create_info_struct(png_ptr);
		if (!info_ptr) {
			// 处理 png 信息结构创建失败的情况
			png_destroy_write_struct(&png_ptr, (png_infopp)NULL);
			fclose(fp);
			return;
		}

		// 设置错误处理回调（如果需要的话）
		if (setjmp(png_jmpbuf(png_ptr))) {
			// 处理发生错误的情况
			png_destroy_write_struct(&png_ptr, &info_ptr);
			fclose(fp);
			return;
		}

		// 初始化写入操作
		png_init_io(png_ptr, fp);



		int bit_depth = 8;
		int color_type = PNG_COLOR_TYPE_RGB;

		png_set_IHDR(png_ptr, info_ptr, width, height, bit_depth, color_type, PNG_INTERLACE_NONE,
			PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);

		png_bytep* row_pointers = (png_bytep*)malloc(height * sizeof(png_bytep));
		
		for (unsigned int y = 0; y < height; y++) {

			row_pointers[y] = const_cast<png_bytep>(data + y * width * 4); // RGBA 图片数据（每个像素 4 字节）

		}

		png_set_rows(png_ptr, info_ptr, row_pointers);

		png_write_png(png_ptr, info_ptr, PNG_TRANSFORM_IDENTITY, NULL);
		free(row_pointers);
		png_destroy_write_struct(&png_ptr, &info_ptr);
		fclose(fp);
	}

	void SaveMaskPNG(const wchar_t* filename, unsigned int width, unsigned int height, const uint8_t* mask)
	{
		FRetInfo* ret = InitSaveInfoFile(filename, width, height);

		png_set_IHDR(ret->png_ptr, ret->info_ptr,
			width, height,
			8,                     // 8-bit
			PNG_COLOR_TYPE_GRAY,  // 单通道灰度
			PNG_INTERLACE_NONE,
			PNG_COMPRESSION_TYPE_DEFAULT,
			PNG_FILTER_TYPE_DEFAULT);

		png_bytep* rows = (png_bytep*)malloc(height * sizeof(png_bytep));

		for (unsigned int y = 0; y < height; y++)
			rows[y] = (png_bytep)(mask + y * width);

		png_set_rows(ret->png_ptr, ret->info_ptr, rows);
		png_write_png(ret->png_ptr, ret->info_ptr, PNG_TRANSFORM_IDENTITY, NULL);

		free(rows);
		png_destroy_write_struct(&ret->png_ptr, &ret->info_ptr);
		fclose(ret->fp);
		delete ret;
	}
}
