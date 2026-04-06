#include "PsdPch.h"
#include "PsdEncodeEngineData.h"
#include "PsdPlatform.h"
#include "PsdSyncFileUtil.h"
#include "PsdMemoryUtil.h"

#include <string>
#include <vector>
#include <map>


PSD_NAMESPACE_BEGIN
#pragma region WriteEngineDataBytes

static uint32_t indent = 0;
static uint32_t length = 0;
static uint64_t offset = 0;
static uint8_t* buffer = nullptr;

template <typename T>
inline void WriteToBuffer(Allocator* allocator, const T& data)
{
	size_t size = sizeof(T);
	if (size + offset > length)
	{
		uint8_t* newBuffer = memoryUtil::AllocateArray<uint8_t>(allocator, length * 2);
		memcpy(newBuffer, buffer, offset);
		memoryUtil::FreeArray(allocator, buffer);
		buffer = newBuffer;
		length *= 2;
	}
	memcpy(buffer + offset, &data, size);
	offset += size;
}

//浮点数转字符串
std::string serializeNumber(float64_t value)
{
	static std::string zeroStr = std::string("0.0");
	if (value == 0)
	{
		return zeroStr;
	}

	uint64_t value1 = static_cast<uint32_t>(value);
	uint64_t value2 = static_cast<uint32_t>(round((value - value1) * 1000));
	if (value1 == 0 && value2 == 0)
	{
		return zeroStr;
	}

	if (value2 == 1000)
	{
		value2 = 0;
		value1 += 1;
	}

	size_t zeroNum = 0;
	if (value2 > 100)
	{
		zeroNum = 0;
	}
	else if (value2 > 10)
	{
		zeroNum = 1;
	}
	else
	{
		zeroNum = 2;
	}

	std::string mid(zeroNum, '0');

	if (value2 != 0)
	{
		while (value2 % 10 == 0)
		{
			value2 = value2 / 10;
		}
	}

	if (value1 == 0)
	{
		return std::string(".") + mid + std::to_string(value2);
	}

	if (value2 == 0)
	{
		return std::to_string(value1) + std::string(".0");
	}

	return std::to_string(value1) + std::string(".") + mid + std::to_string(value2);
}

//整数转字符串
std::string serializeNumber(uint64_t value)
{
	return std::to_string(value);
}

//写入缩进
void writeIndent(Allocator* allocator)
{
	for (uint32_t i = 0; i < indent; ++i)
	{
		WriteToBuffer(allocator, (uint8_t)9);//'\t'
	}
}

//写入字符串
void writeString(Allocator* allocator, const std::string& str)
{
	for (size_t i = 0; i < str.length(); ++i)
	{
		WriteToBuffer(allocator, str[i]);
	}
}

//写入Unicode字符的字节
void writeStringByte(Allocator* allocator, uint8_t value)
{
	if (value == 40 || value == 41 || value == 92) // '(' ')' '\'
	{ 
		WriteToBuffer(allocator, (uint8_t)92);
	}

	WriteToBuffer(allocator, value);
}

//写入前缀
void writePrefix(Allocator* allocator, bool inProperty)
{
	if (inProperty)
	{
		writeString(allocator, " ");
	}
	else
	{
		writeIndent(allocator);
	}
}

//写入浮点数
void writeValue(Allocator* allocator, float64_t value, bool inProperty)
{
	writePrefix(allocator, inProperty);
	writeString(allocator, serializeNumber(value));
}

//写入整数
void writeValue(Allocator* allocator, uint64_t value, bool inProperty)
{
	writePrefix(allocator, inProperty);
	writeString(allocator, serializeNumber(value));
}

//写入bool类型
void writeValue(Allocator* allocator, bool value, bool inProperty)
{
	writePrefix(allocator, inProperty);
	writeString(allocator, value ? "true" : "false");
}

//写入Unicode字符串
void writeValue(Allocator* allocator, std::wstring value, bool inProperty)
{
	writePrefix(allocator, inProperty);
	
	writeString(allocator, "(");
	WriteToBuffer(allocator, (uint8_t)0xfe);
	WriteToBuffer(allocator, (uint8_t)0xff);

	for (size_t i = 0; i < value.length(); i++) 
	{
		wchar_t code = value[i];
		writeStringByte(allocator, (code >> 8) & 0xff);
		writeStringByte(allocator, code & 0xff);
	}

	writeString(allocator, ")");
}

//写入整数数组
void writeValue(Allocator* allocator, uint64_t value[], uint64_t len, bool inProperty)
{
	writePrefix(allocator, inProperty);
	writeString(allocator, "[");

	for (uint64_t i = 0; i < len; ++i)
	{
		writeString(allocator, " ");
		writeString(allocator, serializeNumber(value[i]));
	}

	writeString(allocator, " ]");
}

//写入浮点数组
void writeValue(Allocator* allocator, float64_t value[], uint64_t len, bool inProperty)
{
	writePrefix(allocator, inProperty);
	writeString(allocator, "[");

	for (uint64_t i = 0; i < len; ++i)
	{
		writeString(allocator, " ");
		writeString(allocator, serializeNumber(value[i]));
	}

	writeString(allocator, " ]");
}

//开始写入Object数组
void begineWriteObjectArray(Allocator* allocator, bool inProperty, bool isNotEmpty = true)
{
	writePrefix(allocator, inProperty);
	writeString(allocator, "[");
	if (isNotEmpty)
	{
		writeString(allocator, "\n");
	}
	else
	{
		writeString(allocator, " ");
	}
}

//结束写入Object数组
void endWriteObjectArray(Allocator* allocator, bool isNotEmpty = true)
{
	if (isNotEmpty)
	{
		writeIndent(allocator);
	}
	writeString(allocator, "]");
}

//开始写入Object
void begineWriteObject(Allocator* allocator, bool inProperty)
{
	if (inProperty) writeString(allocator, "\n");

	writeIndent(allocator);
	writeString(allocator, "<<");

	writeString(allocator, "\n");

	indent++;
}

//结束写入Object
void endWriteObject(Allocator* allocator)
{
	indent--;
	writeIndent(allocator);
	writeString(allocator, ">>");
}

//开始写入Object的属性
void begineWriteProperty(Allocator* allocator, const std::string& key)
{
	writeIndent(allocator);
	writeString(allocator, "/");
	writeString(allocator, key);
}

//结束写入Object的属性
void endWriteProperty(Allocator* allocator)
{
	writeString(allocator, "\n");
}

//由于C++不支持反射，所以这里写入的Object类型需要手动写多个函数!

//写入PhotoshopNode结构
void writeValue(Allocator* allocator, PhotoshopNode& photoshopNode, bool inProperty, bool notInArray = true)
{
	begineWriteObject(allocator, inProperty && notInArray);

	//ShapeType
	begineWriteProperty(allocator, "ShapeType");
	writeValue(allocator, photoshopNode.ShapeType, inProperty);
	endWriteProperty(allocator);

	//PointBase
	begineWriteProperty(allocator, "PointBase");
	writeValue(allocator, photoshopNode.PointBase, 2, inProperty);
	endWriteProperty(allocator);

	//Base
	begineWriteProperty(allocator, "Base");

	begineWriteObject(allocator, inProperty);
	//Base.ShapeType
	begineWriteProperty(allocator, "ShapeType");
	writeValue(allocator, photoshopNode.Base.ShapeType, inProperty);
	endWriteProperty(allocator);
	//Base.TransformPoint0
	begineWriteProperty(allocator, "TransformPoint0");
	writeValue(allocator, photoshopNode.Base.TransformPoint0, 2, inProperty);
	endWriteProperty(allocator);
	//Base.TransformPoint1
	begineWriteProperty(allocator, "TransformPoint1");
	writeValue(allocator, photoshopNode.Base.TransformPoint1, 2, inProperty);
	endWriteProperty(allocator);
	//Base.TransformPoint2
	begineWriteProperty(allocator, "TransformPoint2");
	writeValue(allocator, photoshopNode.Base.TransformPoint2, 2, inProperty);
	endWriteProperty(allocator);

	endWriteObject(allocator);

	endWriteProperty(allocator);


	endWriteObject(allocator);
}

//写入ParagraphProperties结构
//isDefault 默认值形式，不会写入值
void writeValue(Allocator* allocator, ParagraphProperties& paragraphProperties, bool inProperty, bool notInArray = true)
{
	begineWriteObject(allocator, inProperty && notInArray);

	if (!paragraphProperties.isDefault)
	{
		//Justification
		begineWriteProperty(allocator, "Justification");
		writeValue(allocator, paragraphProperties.Justification, inProperty);
		endWriteProperty(allocator);

		//FirstLineIndent
		begineWriteProperty(allocator, "FirstLineIndent");
		writeValue(allocator, paragraphProperties.FirstLineIndent, inProperty);
		endWriteProperty(allocator);

		//StartIndent
		begineWriteProperty(allocator, "StartIndent");
		writeValue(allocator, paragraphProperties.StartIndent, inProperty);
		endWriteProperty(allocator);

		//EndIndent
		begineWriteProperty(allocator, "EndIndent");
		writeValue(allocator, paragraphProperties.EndIndent, inProperty);
		endWriteProperty(allocator);

		//SpaceBefore
		begineWriteProperty(allocator, "SpaceBefore");
		writeValue(allocator, paragraphProperties.SpaceBefore, inProperty);
		endWriteProperty(allocator);

		//SpaceAfter
		begineWriteProperty(allocator, "SpaceAfter");
		writeValue(allocator, paragraphProperties.SpaceAfter, inProperty);
		endWriteProperty(allocator);

		//AutoHyphenate
		begineWriteProperty(allocator, "AutoHyphenate");
		writeValue(allocator, paragraphProperties.AutoHyphenate, inProperty);
		endWriteProperty(allocator);

		//HyphenatedWordSize
		begineWriteProperty(allocator, "HyphenatedWordSize");
		writeValue(allocator, paragraphProperties.HyphenatedWordSize, inProperty);
		endWriteProperty(allocator);

		//PreHyphen
		begineWriteProperty(allocator, "PreHyphen");
		writeValue(allocator, paragraphProperties.PreHyphen, inProperty);
		endWriteProperty(allocator);

		//PostHyphen
		begineWriteProperty(allocator, "PostHyphen");
		writeValue(allocator, paragraphProperties.PostHyphen, inProperty);
		endWriteProperty(allocator);

		//ConsecutiveHyphens
		begineWriteProperty(allocator, "ConsecutiveHyphens");
		writeValue(allocator, paragraphProperties.ConsecutiveHyphens, inProperty);
		endWriteProperty(allocator);

		//Zone
		begineWriteProperty(allocator, "Zone");
		writeValue(allocator, paragraphProperties.Zone, inProperty);
		endWriteProperty(allocator);

		//WordSpacing
		begineWriteProperty(allocator, "WordSpacing");
		writeValue(allocator, paragraphProperties.WordSpacing, 3, inProperty);
		endWriteProperty(allocator);

		//LetterSpacing
		begineWriteProperty(allocator, "LetterSpacing");
		writeValue(allocator, paragraphProperties.LetterSpacing, 3, inProperty);
		endWriteProperty(allocator);

		//GlyphSpacing
		begineWriteProperty(allocator, "GlyphSpacing");
		writeValue(allocator, paragraphProperties.GlyphSpacing, 3, inProperty);
		endWriteProperty(allocator);

		//AutoLeading
		begineWriteProperty(allocator, "AutoLeading");
		writeValue(allocator, paragraphProperties.AutoLeading, inProperty);
		endWriteProperty(allocator);

		//LeadingType
		begineWriteProperty(allocator, "LeadingType");
		writeValue(allocator, paragraphProperties.LeadingType, inProperty);
		endWriteProperty(allocator);

		//Hanging
		begineWriteProperty(allocator, "Hanging");
		writeValue(allocator, paragraphProperties.Hanging, inProperty);
		endWriteProperty(allocator);

		//Burasagari
		begineWriteProperty(allocator, "Burasagari");
		writeValue(allocator, paragraphProperties.Burasagari, inProperty);
		endWriteProperty(allocator);

		//KinsokuOrder
		begineWriteProperty(allocator, "KinsokuOrder");
		writeValue(allocator, paragraphProperties.KinsokuOrder, inProperty);
		endWriteProperty(allocator);

		//EveryLineComposer
		begineWriteProperty(allocator, "EveryLineComposer");
		writeValue(allocator, paragraphProperties.EveryLineComposer, inProperty);
		endWriteProperty(allocator);
	}

	endWriteObject(allocator);
}

//写入TypeValues结构
void writeValue(Allocator* allocator, TypeValues& typeValues, bool inProperty, bool notInArray = true)
{
	begineWriteObject(allocator, inProperty && notInArray);

	//Type
	begineWriteProperty(allocator, "Type");
	writeValue(allocator, typeValues.Type, inProperty);
	endWriteProperty(allocator);

	//Values
	begineWriteProperty(allocator, "Values");
	writeValue(allocator, typeValues.Values, 4, inProperty);
	endWriteProperty(allocator);

	endWriteObject(allocator);
}

//写入StyleSheetData结构
void writeValue(Allocator* allocator, StyleSheetData& styleSheetData, bool inProperty, bool notInArray = true)
{
	begineWriteObject(allocator, inProperty && notInArray);

	if (!styleSheetData.isDefault)
	{
		if (styleSheetData.isFew)
		{
			//Font
			begineWriteProperty(allocator, "Font");
			writeValue(allocator, styleSheetData.Font, inProperty);
			endWriteProperty(allocator);

			//FontSize
			begineWriteProperty(allocator, "FontSize");
			writeValue(allocator, styleSheetData.FontSize, inProperty);
			endWriteProperty(allocator);

			//AutoKerning
			begineWriteProperty(allocator, "AutoKerning");
			writeValue(allocator, styleSheetData.AutoKerning, inProperty);
			endWriteProperty(allocator);

			//Kerning
			begineWriteProperty(allocator, "Kerning");
			writeValue(allocator, styleSheetData.Kerning, inProperty);
			endWriteProperty(allocator);

			//FillColor
			begineWriteProperty(allocator, "FillColor");
			writeValue(allocator, styleSheetData.FillColor, inProperty);
			endWriteProperty(allocator);
		}
		else
		{
			//Font
			begineWriteProperty(allocator, "Font");
			writeValue(allocator, styleSheetData.Font, inProperty);
			endWriteProperty(allocator);

			//FontSize
			begineWriteProperty(allocator, "FontSize");
			writeValue(allocator, styleSheetData.FontSize, inProperty);
			endWriteProperty(allocator);

			//FauxBold
			begineWriteProperty(allocator, "FauxBold");
			writeValue(allocator, styleSheetData.FauxBold, inProperty);
			endWriteProperty(allocator);

			//FauxItalic
			begineWriteProperty(allocator, "FauxItalic");
			writeValue(allocator, styleSheetData.FauxItalic, inProperty);
			endWriteProperty(allocator);

			//AutoLeading
			begineWriteProperty(allocator, "AutoLeading");
			writeValue(allocator, styleSheetData.AutoLeading, inProperty);
			endWriteProperty(allocator);

			//Leading
			begineWriteProperty(allocator, "Leading");
			writeValue(allocator, styleSheetData.Leading, inProperty);
			endWriteProperty(allocator);

			//HorizontalScale
			begineWriteProperty(allocator, "HorizontalScale");
			writeValue(allocator, styleSheetData.HorizontalScale, inProperty);
			endWriteProperty(allocator);

			//VerticalScale
			begineWriteProperty(allocator, "VerticalScale");
			writeValue(allocator, styleSheetData.VerticalScale, inProperty);
			endWriteProperty(allocator);

			//Tracking
			begineWriteProperty(allocator, "Tracking");
			writeValue(allocator, styleSheetData.Tracking, inProperty);
			endWriteProperty(allocator);

			//AutoKerning
			begineWriteProperty(allocator, "AutoKerning");
			writeValue(allocator, styleSheetData.AutoKerning, inProperty);
			endWriteProperty(allocator);

			//Kerning
			begineWriteProperty(allocator, "Kerning");
			writeValue(allocator, styleSheetData.Kerning, inProperty);
			endWriteProperty(allocator);

			//BaselineShift
			begineWriteProperty(allocator, "BaselineShift");
			writeValue(allocator, styleSheetData.BaselineShift, inProperty);
			endWriteProperty(allocator);

			//FontCaps
			begineWriteProperty(allocator, "FontCaps");
			writeValue(allocator, styleSheetData.FontCaps, inProperty);
			endWriteProperty(allocator);

			//FontBaseline
			begineWriteProperty(allocator, "FontBaseline");
			writeValue(allocator, styleSheetData.FontBaseline, inProperty);
			endWriteProperty(allocator);

			//Underline
			begineWriteProperty(allocator, "Underline");
			writeValue(allocator, styleSheetData.Underline, inProperty);
			endWriteProperty(allocator);

			//Strikethrough
			begineWriteProperty(allocator, "Strikethrough");
			writeValue(allocator, styleSheetData.Strikethrough, inProperty);
			endWriteProperty(allocator);

			//Ligatures
			begineWriteProperty(allocator, "Ligatures");
			writeValue(allocator, styleSheetData.Ligatures, inProperty);
			endWriteProperty(allocator);

			//DLigatures
			begineWriteProperty(allocator, "DLigatures");
			writeValue(allocator, styleSheetData.DLigatures, inProperty);
			endWriteProperty(allocator);

			//BaselineDirection
			begineWriteProperty(allocator, "BaselineDirection");
			writeValue(allocator, styleSheetData.BaselineDirection, inProperty);
			endWriteProperty(allocator);

			//Tsume
			begineWriteProperty(allocator, "Tsume");
			writeValue(allocator, styleSheetData.Tsume, inProperty);
			endWriteProperty(allocator);

			//StyleRunAlignment
			begineWriteProperty(allocator, "StyleRunAlignment");
			writeValue(allocator, styleSheetData.StyleRunAlignment, inProperty);
			endWriteProperty(allocator);

			//Language
			begineWriteProperty(allocator, "Language");
			writeValue(allocator, styleSheetData.Language, inProperty);
			endWriteProperty(allocator);

			//NoBreak
			begineWriteProperty(allocator, "NoBreak");
			writeValue(allocator, styleSheetData.NoBreak, inProperty);
			endWriteProperty(allocator);

			//FillColor
			begineWriteProperty(allocator, "FillColor");
			writeValue(allocator, styleSheetData.FillColor, inProperty);
			endWriteProperty(allocator);

			//StrokeColor
			begineWriteProperty(allocator, "StrokeColor");
			writeValue(allocator, styleSheetData.StrokeColor, inProperty);
			endWriteProperty(allocator);

			//FillFlag
			begineWriteProperty(allocator, "FillFlag");
			writeValue(allocator, styleSheetData.FillFlag, inProperty);
			endWriteProperty(allocator);

			//StrokeFlag
			begineWriteProperty(allocator, "StrokeFlag");
			writeValue(allocator, styleSheetData.StrokeFlag, inProperty);
			endWriteProperty(allocator);

			//FillFirst
			begineWriteProperty(allocator, "FillFirst");
			writeValue(allocator, styleSheetData.FillFirst, inProperty);
			endWriteProperty(allocator);

			//AutoKerning
			begineWriteProperty(allocator, "YUnderline");
			writeValue(allocator, styleSheetData.YUnderline, inProperty);
			endWriteProperty(allocator);

			//OutlineWidth
			begineWriteProperty(allocator, "OutlineWidth");
			writeValue(allocator, styleSheetData.OutlineWidth, inProperty);
			endWriteProperty(allocator);

			//CharacterDirection
			begineWriteProperty(allocator, "CharacterDirection");
			writeValue(allocator, styleSheetData.CharacterDirection, inProperty);
			endWriteProperty(allocator);

			//HindiNumbers
			begineWriteProperty(allocator, "HindiNumbers");
			writeValue(allocator, styleSheetData.HindiNumbers, inProperty);
			endWriteProperty(allocator);

			//Kashida
			begineWriteProperty(allocator, "Kashida");
			writeValue(allocator, styleSheetData.Kashida, inProperty);
			endWriteProperty(allocator);

			//DiacriticPos
			begineWriteProperty(allocator, "DiacriticPos");
			writeValue(allocator, styleSheetData.DiacriticPos, inProperty);
			endWriteProperty(allocator);
		}
	}
	endWriteObject(allocator);
}

//写入ParagraphSheet结构
void writeValue(Allocator* allocator, ParagraphSheet& paragraphSheet, bool inProperty, bool notInArray = true)
{
	begineWriteObject(allocator, inProperty && notInArray);

	if (paragraphSheet.isDefault)
	{
		//DefaultStyleSheet
		begineWriteProperty(allocator, "DefaultStyleSheet");
		writeValue(allocator, paragraphSheet.DefaultStyleSheet, inProperty);
		endWriteProperty(allocator);

		//Properties
		begineWriteProperty(allocator, "Properties");
		//这里写入的是一个空的数据结构
		begineWriteObject(allocator, inProperty);
		endWriteObject(allocator);
		endWriteProperty(allocator);
	}
	else
	{
		//Name
		//begineWriteProperty(allocator, "Name");
		//writeValue(allocator, std::wstring(paragraphSheet.Name), inProperty);
		//endWriteProperty(allocator);

		//DefaultStyleSheet
		begineWriteProperty(allocator, "DefaultStyleSheet");
		writeValue(allocator, paragraphSheet.DefaultStyleSheet, inProperty);
		endWriteProperty(allocator);

		//Properties
		begineWriteProperty(allocator, "Properties");
		writeValue(allocator, paragraphSheet.Properties, inProperty);
		endWriteProperty(allocator);
	}

	endWriteObject(allocator);
}

//写入Adjustments结构
void writeValue(Allocator* allocator, Adjustments& adjustments, bool inProperty, bool notInArray = true)
{
	begineWriteObject(allocator, inProperty && notInArray);

	//Axis
	begineWriteProperty(allocator, "Axis");
	writeValue(allocator, adjustments.Axis, 3,inProperty);
	endWriteProperty(allocator);

	//XY
	begineWriteProperty(allocator, "XY");
	writeValue(allocator, adjustments.XY, 2, inProperty);
	endWriteProperty(allocator);

	endWriteObject(allocator);
}

//写入FontSet结构
void writeValue(Allocator* allocator, FontSet& fontSet, bool inProperty, bool notInArray = true)
{
	begineWriteObject(allocator, inProperty && notInArray);

	//Name
	begineWriteProperty(allocator, "Name");
	writeValue(allocator, std::wstring(fontSet.Name), inProperty);
	endWriteProperty(allocator);

	//Script
	begineWriteProperty(allocator, "Script");
	writeValue(allocator, fontSet.Script, inProperty);
	endWriteProperty(allocator);

	//FontType
	begineWriteProperty(allocator, "FontType");
	writeValue(allocator, fontSet.FontType, inProperty);
	endWriteProperty(allocator);

	//Synthetic
	begineWriteProperty(allocator, "Synthetic");
	writeValue(allocator, fontSet.Synthetic, inProperty);
	endWriteProperty(allocator);

	endWriteObject(allocator);
}

//写入ParagraphRun结构
void writeValue(Allocator* allocator, ParagraphRun& paragraphRun, bool inProperty, bool notInArray = true)
{
	begineWriteObject(allocator, inProperty && notInArray);

	//ParagraphSheet
	begineWriteProperty(allocator, "ParagraphSheet");
	writeValue(allocator, paragraphRun.ParagraphSheet, inProperty);
	endWriteProperty(allocator);

	//Adjustments
	begineWriteProperty(allocator, "Adjustments");
	writeValue(allocator, paragraphRun.Adjustments, inProperty);
	endWriteProperty(allocator);

	endWriteObject(allocator);
}

//写入StyleRun结构
void writeValue(Allocator* allocator, StyleRun& styleRun, bool inProperty, bool notInArray = true)
{
	begineWriteObject(allocator, inProperty && notInArray);

	if (styleRun.isDefault)
	{
		//StyleSheet
		begineWriteProperty(allocator, "StyleSheet");

		//StyleSheet.StyleSheetData
		begineWriteObject(allocator, inProperty);

		begineWriteProperty(allocator, "StyleSheetData");
		//这里写入的是一个空的数据结构
		begineWriteObject(allocator, inProperty);
		endWriteObject(allocator);
		endWriteProperty(allocator);

		endWriteObject(allocator);

		endWriteProperty(allocator);
	}
	else
	{
		//StyleSheet
		begineWriteProperty(allocator, "StyleSheet");

		//StyleSheet.StyleSheetData
		begineWriteObject(allocator, inProperty);

		begineWriteProperty(allocator, "StyleSheetData");
		writeValue(allocator, styleRun.StyleSheet.StyleSheetData, inProperty);
		endWriteProperty(allocator);

		endWriteObject(allocator);

		endWriteProperty(allocator);
	}

	endWriteObject(allocator);
}

//写入ResourceDict结构
void writeValue(Allocator* allocator, ResourceDict& resourceDict, bool inProperty, bool notInArray = true)
{
	begineWriteObject(allocator, inProperty && notInArray);

	//KinsokuSet
	begineWriteProperty(allocator, "KinsokuSet");
	begineWriteObjectArray(allocator, inProperty);
	for (int i = 0; i < 2; ++i)
	{
		begineWriteObject(allocator, false);
		//Name
		begineWriteProperty(allocator, "Name");
		writeValue(allocator, std::wstring(resourceDict.KinsokuSet[i].Name), inProperty);
		endWriteProperty(allocator);

		//NoStart
		begineWriteProperty(allocator, "NoStart");
		writeValue(allocator, std::wstring(resourceDict.KinsokuSet[i].NoStart), inProperty);
		endWriteProperty(allocator);

		//NoEnd
		begineWriteProperty(allocator, "NoEnd");
		writeValue(allocator, std::wstring(resourceDict.KinsokuSet[i].NoEnd), inProperty);
		endWriteProperty(allocator);

		//Keep
		begineWriteProperty(allocator, "Keep");
		writeValue(allocator, std::wstring(resourceDict.KinsokuSet[i].Keep), inProperty);
		endWriteProperty(allocator);

		//Hanging
		begineWriteProperty(allocator, "Hanging");
		writeValue(allocator, std::wstring(resourceDict.KinsokuSet[i].Hanging), inProperty);
		endWriteProperty(allocator);

		endWriteObject(allocator);

		writeString(allocator,"\n");
	}
	endWriteObjectArray(allocator);
	endWriteProperty(allocator);

	//MojiKumiSet
	begineWriteProperty(allocator, "MojiKumiSet");
	begineWriteObjectArray(allocator, inProperty);
	for (int i = 0; i < 4; ++i)
	{
		begineWriteObject(allocator, false);
		//InternalName
		begineWriteProperty(allocator, "InternalName");
		writeValue(allocator, std::wstring(resourceDict.MojiKumiSet[i].InternalName), inProperty);
		endWriteProperty(allocator);

		endWriteObject(allocator);
		writeString(allocator, "\n");
	}
	endWriteObjectArray(allocator);
	endWriteProperty(allocator);

	//TheNormalStyleSheet
	begineWriteProperty(allocator, "TheNormalStyleSheet");
	writeValue(allocator, resourceDict.TheNormalStyleSheet, inProperty);
	endWriteProperty(allocator);

	//TheNormalParagraphSheet
	begineWriteProperty(allocator, "TheNormalParagraphSheet");
	writeValue(allocator, resourceDict.TheNormalParagraphSheet, inProperty);
	endWriteProperty(allocator);

	//ParagraphSheetSet
	begineWriteProperty(allocator, "ParagraphSheetSet");
	begineWriteObjectArray(allocator, inProperty);
	writeValue(allocator, resourceDict.ParagraphSheetSet[0], inProperty, false);
	writeString(allocator, "\n");
	endWriteObjectArray(allocator);
	endWriteProperty(allocator);

	//StyleSheetSet
	begineWriteProperty(allocator, "StyleSheetSet");
	begineWriteObjectArray(allocator, inProperty);
	begineWriteObject(allocator, false);
	//StyleSheetSet.Name
	begineWriteProperty(allocator, "Name");
	writeValue(allocator, std::wstring(resourceDict.StyleSheetSet[0].Name), inProperty);
	endWriteProperty(allocator);
	//StyleSheetSet.StyleSheetData
	begineWriteProperty(allocator, "StyleSheetData");
	writeValue(allocator, resourceDict.StyleSheetSet[0].StyleSheetData, inProperty);
	endWriteProperty(allocator);
	endWriteObject(allocator);
	writeString(allocator, "\n");
	endWriteObjectArray(allocator);
	endWriteProperty(allocator);

	//FontSet
	begineWriteProperty(allocator, "FontSet");
	begineWriteObjectArray(allocator, inProperty);
	for (int i = 0; i < 2; ++i)
	{
		writeValue(allocator, resourceDict.FontSet[i], inProperty, false);
		writeString(allocator, "\n");
	}
	endWriteObjectArray(allocator);
	endWriteProperty(allocator);

	//SuperscriptSize
	begineWriteProperty(allocator, "SuperscriptSize");
	writeValue(allocator, resourceDict.SuperscriptSize, inProperty);
	endWriteProperty(allocator);

	//SuperscriptPosition
	begineWriteProperty(allocator, "SuperscriptPosition");
	writeValue(allocator, resourceDict.SuperscriptPosition, inProperty);
	endWriteProperty(allocator);

	//SubscriptSize
	begineWriteProperty(allocator, "SubscriptSize");
	writeValue(allocator, resourceDict.SubscriptSize, inProperty);
	endWriteProperty(allocator);

	//SubscriptPosition
	begineWriteProperty(allocator, "SubscriptPosition");
	writeValue(allocator, resourceDict.SubscriptPosition, inProperty);
	endWriteProperty(allocator);

	//SmallCapSize
	begineWriteProperty(allocator, "SmallCapSize");
	writeValue(allocator, resourceDict.SmallCapSize, inProperty);
	endWriteProperty(allocator);

	endWriteObject(allocator);
}

//写入EngineData结构
void writeValue(Allocator* allocator, EngineData& engineData, bool inProperty)
{
	writeString(allocator, "\n\n");
	begineWriteObject(allocator, false);

	//EngineDict
	begineWriteProperty(allocator, "EngineDict");
	begineWriteObject(allocator, inProperty);
		//EngineDict.Editor
		begineWriteProperty(allocator, "Editor");
		begineWriteObject(allocator, inProperty);
			//EngineDict.Editor.Text
			begineWriteProperty(allocator, "Text");
			writeValue(allocator, std::wstring(engineData.EngineDict.Editor.Text), inProperty);
			endWriteProperty(allocator);
		endWriteObject(allocator);
		endWriteProperty(allocator);
		//EngineDict.ParagraphRun
		begineWriteProperty(allocator, "ParagraphRun");
		begineWriteObject(allocator, inProperty);
			//EngineDict.ParagraphRun.DefaultRunData
			begineWriteProperty(allocator, "DefaultRunData");
			writeValue(allocator, engineData.EngineDict.ParagraphRun.DefaultRunData, inProperty);
			endWriteProperty(allocator);
			//EngineDict.ParagraphRun.RunArray
			begineWriteProperty(allocator, "RunArray");
			begineWriteObjectArray(allocator, inProperty);
			for (size_t i = 0; i < engineData.EngineDict.ParagraphRun.RunArray.size(); ++i)
			{
				writeValue(allocator, engineData.EngineDict.ParagraphRun.RunArray[i], inProperty, false);
				writeString(allocator, "\n");
			}
			endWriteObjectArray(allocator);
			endWriteProperty(allocator);
			//EngineDict.ParagraphRun.RunLengthArray
			begineWriteProperty(allocator, "RunLengthArray");
			writeValue(allocator, &(engineData.EngineDict.ParagraphRun.RunLengthArray[0]), engineData.EngineDict.ParagraphRun.RunLengthArray.size(), inProperty);
			endWriteProperty(allocator);
			//EngineDict.ParagraphRun.IsJoinable
			begineWriteProperty(allocator, "IsJoinable");
			writeValue(allocator, engineData.EngineDict.ParagraphRun.IsJoinable, inProperty);
			endWriteProperty(allocator);
		endWriteObject(allocator);
		endWriteProperty(allocator);
		//EngineDict.StyleRun
		begineWriteProperty(allocator, "StyleRun");
		begineWriteObject(allocator, inProperty);
			//EngineDict.StyleRun.DefaultRunData
			begineWriteProperty(allocator, "DefaultRunData");
			writeValue(allocator, engineData.EngineDict.StyleRun.DefaultRunData, inProperty);
			endWriteProperty(allocator);
			//EngineDict.StyleRun.RunArray
			begineWriteProperty(allocator, "RunArray");
			begineWriteObjectArray(allocator, inProperty);
			for (size_t i = 0; i < engineData.EngineDict.StyleRun.RunArray.size(); ++i)
			{
				writeValue(allocator, engineData.EngineDict.StyleRun.RunArray[i], inProperty, false);
				writeString(allocator, "\n");
			}
			endWriteObjectArray(allocator);
			endWriteProperty(allocator);
			//EngineDict.StyleRun.RunLengthArray
			begineWriteProperty(allocator, "RunLengthArray");
			writeValue(allocator, &(engineData.EngineDict.StyleRun.RunLengthArray[0]), engineData.EngineDict.StyleRun.RunArray.size(), inProperty);
			endWriteProperty(allocator);
			//EngineDict.StyleRun.IsJoinable
			begineWriteProperty(allocator, "IsJoinable");
			writeValue(allocator, engineData.EngineDict.StyleRun.IsJoinable, inProperty);
			endWriteProperty(allocator);
		endWriteObject(allocator);
		endWriteProperty(allocator);
		//EngineDict.GridInfo
		begineWriteProperty(allocator, "GridInfo");
		begineWriteObject(allocator, inProperty);
			//EngineDict.GridInfo.GridIsOn
			begineWriteProperty(allocator, "GridIsOn");
			writeValue(allocator, engineData.EngineDict.GridInfo.GridIsOn, inProperty);
			endWriteProperty(allocator);
			//EngineDict.GridInfo.ShowGrid
			begineWriteProperty(allocator, "ShowGrid");
			writeValue(allocator, engineData.EngineDict.GridInfo.ShowGrid, inProperty);
			endWriteProperty(allocator);
			//EngineDict.GridInfo.GridSize
			begineWriteProperty(allocator, "GridSize");
			writeValue(allocator, engineData.EngineDict.GridInfo.GridSize, inProperty);
			endWriteProperty(allocator);
			//EngineDict.GridInfo.GridLeading
			begineWriteProperty(allocator, "GridLeading");
			writeValue(allocator, engineData.EngineDict.GridInfo.GridLeading, inProperty);
			endWriteProperty(allocator);
			//EngineDict.GridInfo.GridColor
			begineWriteProperty(allocator, "GridColor");
			writeValue(allocator, engineData.EngineDict.GridInfo.GridColor, inProperty);
			endWriteProperty(allocator);
			//EngineDict.GridInfo.GridLeadingFillColor
			begineWriteProperty(allocator, "GridLeadingFillColor");
			writeValue(allocator, engineData.EngineDict.GridInfo.GridLeadingFillColor, inProperty);
			endWriteProperty(allocator);
			//EngineDict.GridInfo.AlignLineHeightToGridFlags
			begineWriteProperty(allocator, "AlignLineHeightToGridFlags");
			writeValue(allocator, engineData.EngineDict.GridInfo.AlignLineHeightToGridFlags, inProperty);
			endWriteProperty(allocator);
		endWriteObject(allocator);
		endWriteProperty(allocator);
		//EngineDict.AntiAlias
		begineWriteProperty(allocator, "AntiAlias");
		writeValue(allocator, engineData.EngineDict.AntiAlias, inProperty);
		endWriteProperty(allocator);
		//EngineDict.UseFractionalGlyphWidths
		begineWriteProperty(allocator, "UseFractionalGlyphWidths");
		writeValue(allocator, engineData.EngineDict.UseFractionalGlyphWidths, inProperty);
		endWriteProperty(allocator);
		//EngineDict.Rendered
		begineWriteProperty(allocator, "Rendered");
		begineWriteObject(allocator, inProperty);
			//EngineDict.Rendered.Version
			begineWriteProperty(allocator, "Version");
			writeValue(allocator, engineData.EngineDict.Rendered.Version, inProperty);
			endWriteProperty(allocator);
			//EngineDict.Rendered.Shapes
			begineWriteProperty(allocator, "Shapes");
			begineWriteObject(allocator, inProperty);
				//EngineDict.Rendered.Shapes.WritingDirection
				begineWriteProperty(allocator, "WritingDirection");
				writeValue(allocator, engineData.EngineDict.Rendered.Shapes.WritingDirection, inProperty);
				endWriteProperty(allocator);
				//EngineDict.Rendered.Shapes.Children
				begineWriteProperty(allocator, "Children");
				begineWriteObjectArray(allocator, inProperty);
				begineWriteObject(allocator, false);
					//EngineDict.Rendered.Shapes.Children[0].ShapeType
					begineWriteProperty(allocator, "ShapeType");
					writeValue(allocator, engineData.EngineDict.Rendered.Shapes.Children[0].ShapeType, inProperty);
					endWriteProperty(allocator);
					//EngineDict.Rendered.Shapes.Children[0].Procession
					begineWriteProperty(allocator, "Procession");
					writeValue(allocator, engineData.EngineDict.Rendered.Shapes.Children[0].Procession, inProperty);
					endWriteProperty(allocator);
					//EngineDict.Rendered.Shapes.Children[0].Lines
					begineWriteProperty(allocator, "Lines");
					begineWriteObject(allocator, inProperty);
						//EngineDict.Rendered.Shapes.Children[0].Lines.WritingDirection
						begineWriteProperty(allocator, "WritingDirection");
						writeValue(allocator, engineData.EngineDict.Rendered.Shapes.Children[0].Lines.WritingDirection, inProperty);
						endWriteProperty(allocator);
						//EngineDict.Rendered.Shapes.Children[0].Lines.WritingDirection
						begineWriteProperty(allocator, "Children");
						begineWriteObjectArray(allocator, inProperty, false);
						//这里数组是个空的,
						endWriteObjectArray(allocator, false);
						endWriteProperty(allocator);
					endWriteObject(allocator);
					endWriteProperty(allocator);
					//EngineDict.Rendered.Shapes.Children[0].Cookie
					begineWriteProperty(allocator, "Cookie");
					begineWriteObject(allocator, inProperty);
						//EngineDict.Rendered.Shapes.Children[0].Cookie.Photoshop
						begineWriteProperty(allocator, "Photoshop");
						writeValue(allocator, engineData.EngineDict.Rendered.Shapes.Children[0].Cookie.Photoshop, inProperty);
						endWriteProperty(allocator);
					endWriteObject(allocator);
					endWriteProperty(allocator);
				endWriteObject(allocator);
				writeString(allocator, "\n");
				endWriteObjectArray(allocator);
				endWriteProperty(allocator);
			endWriteObject(allocator);
			endWriteProperty(allocator);
		endWriteObject(allocator);
		endWriteProperty(allocator);
	endWriteObject(allocator);
	endWriteProperty(allocator);
	//ResourceDict
	begineWriteProperty(allocator, "ResourceDict");
	writeValue(allocator, engineData.resourceDict, inProperty);
	endWriteProperty(allocator);
	//DocumentResources
	begineWriteProperty(allocator, "DocumentResources");
	writeValue(allocator, engineData.DocumentResources, inProperty);
	endWriteProperty(allocator);

	endWriteObject(allocator);
}

#pragma endregion

#pragma region ReadEngineDataBytes
//https://github.com/firstplacelabs/py-psd-engineData/blob/master/engineData.py
//只解析了字体颜色和字体大小
std::string readProp(char* data, uint64_t& off)
{
	std::string prop = "";
	while (true)
	{
		char c = data[off++];
		if (c == ' ' || c == '\n' || c == '\r' || c == '\t')
		{
			break;
		}
		prop += c;
	}
	return prop;
}

std::string readFontSize(char* data, uint64_t& off)
{
	std::string fontSize = "";

	while (true)
	{
		char c = data[off++];
		if (c == ' ' || c == '\n' || c == '\r' || c == '\t')
		{
			break;
		}
		fontSize += c;
	}
	
	return fontSize;
}

std::string readARGB(char* data, uint64_t& off)
{
	std::string argb = "";
	char c = data[off++];
	while (c != '[')
	{
		c = data[off++];
	}

	c = data[off++];
	while (true)
	{
		c = data[off++];
		if (c == ']')
		{
			break;
		}
		argb += c;
	}
	return argb;
}

std::wstring readText(char* data, uint64_t& off)
{
	std::wstring text = L"";

	char c = data[off++];
	//skip to start of string
	while (c != '(')
	{
		c = data[off++];
	}

	char sig1 = data[off++];
	char sig2 = data[off++];

	if (sig1 != 0xfe || sig2 != 0xff)
	{
		
		//ERROR("readText sig error")
	}

	while (true)
	{
		char c1 = data[off++];
		if (c1 == ')')
		{
			return text;
		}
		char c2 = data[off++];
		if (c2 == '\\')
		{
			c2 = data[off++];
		}
		else if(c2 == '13')
		{
			text += '\n';
		}
		else
		{
			wchar_t val = c1 << 8 | c2;
			text += val;
		}
	}
	return text;
}

void GetFontAndColorDict(char* data, uint64_t len, float32_t& fontSize, float32_t* color, std::wstring& fontName)
{
	uint64_t off = 0;
	std::map<std::string, std::string> propDict;
	//propDict["FontSet"] = "";
	propDict["FontSize"] = "";
	propDict["A"] = "";
	propDict["R"] = "";
	propDict["G"] = "";
	propDict["B"] = "";

	fontName = L"";

	char c;
	while (true)
	{
		if (off == len)
		{
			break;
		}

		c = data[off++];

		if (c == '/')
		{
			std::string prop = readProp(data, off);
			if (prop == "FontSize" && propDict["FontSize"] == "")
			{
				propDict["FontSize"] = readFontSize(data, off);
			}

			if (prop == "Values" && propDict["R"] == "")
			{
				std::string argb = readARGB(data, off);

				size_t pos = argb.find(' ');
				propDict["A"] = argb.substr(0, pos);
				argb.erase(0, pos + 1);

				pos = argb.find(' ');
				propDict["R"] = argb.substr(0, pos);
				argb.erase(0, pos + 1);

				pos = argb.find(' ');
				propDict["G"] = argb.substr(0, pos);
				argb.erase(0, pos + 1);

				pos = argb.find(' ');
				propDict["B"] = argb.substr(0, pos);
				argb.erase(0, pos + 1);
			}

			if (prop == "FontSet" && fontName == L"")
			{
				fontName = readText(data, off);
				int a = 10;
				++a;
			}
		}
	}

	fontSize = static_cast<float>(atof(propDict["FontSize"].c_str()));
	color[0] = static_cast<float>(atof(propDict["R"].c_str()));
	color[1] = static_cast<float>(atof(propDict["G"].c_str()));
	color[2] = static_cast<float>(atof(propDict["B"].c_str()));
	//color[0] = atof(propDict["A"].c_str());
}
#pragma endregion


static const Font defaultFont
{
	(wchar_t*)L"MyriadPro-Regular",
	0,
	0,
	0,
};

static ParagraphProperties defaultParagraphStyle
{
	 Justification::left,
	 0,
	 0,
	 0,
	 0,
	 0,
	 true,
	 6,
	 2,
	 2,
	 8,
	 36,
	 {0.8, 1, 1.33} ,
	 {0, 0, 0} ,
	 {1, 1, 1} ,
	 1.2,
	 0,
	 false,
	 false,
	 0,
	 false,

	 false,
};

static StyleSheetData defaultStyle
{
	0,
	32,
	false,
	false,
	true,
	0,
	1,
	1,
	0,
	true,
	0,
	0,
	0,
	0,
	false,
	false,
	true,
	false,
	2,
	0,
	2,
	0,
	false,
	{ 1, {1,0, 0, 0 }},
	{ 1, {1,0, 0, 0 }},
	true,
	false,
	true,
	1,
	1,
	0,
	false,
	1,
	2,

	false,
	false,
};

static StyleSheetData defaulResourcetStyle = defaultStyle;

uint8_t* EncodeEngineData(LayerTextData& data, Allocator* allocator, uint64_t& len)
{
	ParagraphSheet paragraphSheet
	{
		L"Basic Paragraph",
		0,
		defaultParagraphStyle,
		false,
	};

	Adjustments adjustments
	{
		{1,0,1},
		{0,0}
	};

	FontSet fontSet
	{
		L"AdobeInvisFont",
		0,
		0,
		0,
	};

	paragraphSheet.isDefault = false;
	//defaulResourcetStyle.FillColor.Values[1] = 0;
	//defaulResourcetStyle.StrokeColor.Values[1] = 0;

	ParagraphSheet resourceParagraphSheet
	{
		L"Normal RGB",
		0,
		defaultParagraphStyle,
		false,
	};
	ResourceDict resourceDict
	{
		{
			{
				L"PhotoshopKinsokuHard",
				L"、。，．・：；？！ー―’”）〕］｝〉》」』】ヽヾゝゞ々ぁぃぅぇぉっゃゅょゎァィゥェォッャュョヮヵヶ゛゜?!)]},.:;℃℉¢％‰",
				L"‘“（〔［｛〈《「『【([{￥＄£＠§〒＃",
				L"―‥",
				L"、。.,",
			},
			{
				L"PhotoshopKinsokuSoft",
				L"、。，．・：；？！’”）〕］｝〉》」』】ヽヾゝゞ々",
				L"‘“（〔［｛〈《「『【",
				L"―‥",
				L"、。.,",
			}
		},
		{
			L"Photoshop6MojiKumiSet1",
			L"Photoshop6MojiKumiSet2",
			L"Photoshop6MojiKumiSet3",
			L"Photoshop6MojiKumiSet4",
		},
		0,
		0,
		{resourceParagraphSheet},
		{
			{
				L"Normal RGB",
				defaulResourcetStyle,
			}
		},
		{
			{
				L"MicrosoftYaHei",
				0,
				0,
				0,
			},
			{
				L"MyriadPro-Regular",
				0,
				0,
				0,
			},
		},
		0.583,
		0.333,
		0.583,
		0.333,
		0.7,
	};

	ParagraphRun DefaultRunData
	{
		paragraphSheet,
		adjustments
	};
	DefaultRunData.ParagraphSheet.isDefault = true;
	ParagraphRun RunArray
	{
		paragraphSheet,
		adjustments
	};
	RunArray.ParagraphSheet.isDefault = false;

	//设置字体
	if (data.font != nullptr)
	{
		resourceDict.FontSet[0].Name = data.font;
	}
	//设置字体大小
	defaultStyle.FontSize = data.style.FontSize;
	StyleRun DefaultRunData2
	{
		{defaultStyle},
		false,
	};
	DefaultRunData2.isDefault = true;

	//TODO 下面改成动态计算
	StyleRun RunArray2
	{
		{defaultStyle},
		false,
	};
	RunArray2.StyleSheet.StyleSheetData.isDefault = false;
	RunArray2.StyleSheet.StyleSheetData.isFew = true;
	//设置颜色
	memcpy(&(RunArray2.StyleSheet.StyleSheetData.FillColor.Values[1]), data.styleRuns[0].style.FillColor, 3 * sizeof(float64_t));
	StyleRun RunArray3
	{
		{defaultStyle},
		false,
	};
	RunArray3.StyleSheet.StyleSheetData.isDefault = false;
	RunArray3.StyleSheet.StyleSheetData.isFew = true;
	RunArray3.StyleSheet.StyleSheetData.AutoKerning = false;
	//设置颜色
	memcpy(&(RunArray3.StyleSheet.StyleSheetData.FillColor.Values[1]), data.styleRuns[1].style.FillColor, 3 * sizeof(float64_t));
	PhotoshopNode Photoshop
	{
		0,
		{0, 0},
		{0, {1, 0}, {0, 1}, {0,0}}
	};

	EngineData engineData
	{
		{
			{L"Test text2"},
			{
				DefaultRunData,
				{},
				{},
				1
			},
			{
				DefaultRunData2,
				//TODO 这里改成动态计算
				{},
				{},
				1
			},
			{
				false,
				false,
				18,
				22,
				{
					1,
					{0,0,0,1}
				},
				{
					1,
					{0,0,0,1}
				},
				false
			},
			4,
			true,
			{
				1,
				{
					data.orientation == vertical ? 2u : 0,
					{
						0,
						data.orientation == vertical ? 1u : 0,
						{
							data.orientation == vertical ? 2u : 0,
							{}
						},
						{
							Photoshop
						}
					}
				}
			}
		},
		resourceDict,
		resourceDict,

	};

	//深拷贝？
	std::wstring fixedText = std::wstring(data.text) + L"\r";
	engineData.EngineDict.Editor.Text = const_cast <wchar_t*>(fixedText.c_str());

	//计算换行
	uint64_t textLength = wcslen(engineData.EngineDict.Editor.Text);
	for(uint64_t i = 0, last = 0; i < textLength; ++i)
	{
		if (engineData.EngineDict.Editor.Text[i] == 13) // \r
		{
			engineData.EngineDict.ParagraphRun.RunLengthArray.push_back(i - last + 1);
			engineData.EngineDict.ParagraphRun.RunArray.push_back(RunArray);

			engineData.EngineDict.StyleRun.RunLengthArray.push_back(i - last + 1);
			engineData.EngineDict.StyleRun.RunArray.push_back(RunArray2);

			last = i + 1;
		}
	}

	length = 10 * 1024;
	offset = 0;
	buffer = memoryUtil::AllocateArray<uint8_t>(allocator ,length);

	writeValue(allocator, engineData, true);
	len = offset;

	return buffer;
}
PSD_NAMESPACE_END