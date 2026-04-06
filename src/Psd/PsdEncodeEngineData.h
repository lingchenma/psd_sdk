#pragma once
#include "PsdExportEngineData.h"
#include "PsdAllocator.h"

#include <xstring>
PSD_NAMESPACE_BEGIN
//根据LayerTextData生成EngineData结构
uint8_t* EncodeEngineData(LayerTextData& data, Allocator* allocator, uint64_t& length);
void GetFontAndColorDict(char* data, uint64_t len, float32_t& fontSize, float32_t* color, std::wstring& fontName);
PSD_NAMESPACE_END