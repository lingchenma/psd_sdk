// Copyright 2011-2020, Molecular Matters GmbH <office@molecular-matters.com>
// See LICENSE.txt for licensing details (2-clause BSD License: https://opensource.org/licenses/BSD-2-Clause)

#pragma once

#include "../Psd/Psdstdint.h"


namespace tgaExporter
{
	FILE* CreateFile(const wchar_t* filename);
	/// Assumes 8-bit single-channel data.
	void SaveMonochrome(const wchar_t* filename, unsigned int width, unsigned int height, const uint8_t* data);

	/// Assumes 8-bit RGBA data, but ignores alpha (32-bit data is assumed for performance reasons).
	void SaveRGB(const wchar_t* filename, unsigned int width, unsigned int height, const uint8_t* data);

	/// Assumes 8-bit RGBA data.
	void SaveRGBA(const wchar_t* filename, unsigned int width, unsigned int height, const uint8_t* data);

	void SaveSvPngRGBA(const wchar_t* filename, unsigned int width, unsigned int height, const uint8_t* data);

	void SaveSvPngRGB(const wchar_t* filename, unsigned int width, unsigned int height, const uint8_t* data);

	void SavelibPngRGBA(const wchar_t* filename, unsigned int width, unsigned int height, const uint8_t* data);
	void SavelibPngRGBA(const wchar_t* filename, unsigned int width, unsigned int height, const uint16_t* data);
	void SavelibPngRGBA(const wchar_t* filename, unsigned int width, unsigned int height, const float32_t* data);

	void SavelibPngRGB(const wchar_t* filename, unsigned int width, unsigned int height, const uint8_t* data);
}
