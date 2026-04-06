// Copyright 2011-2020, Molecular Matters GmbH <office@molecular-matters.com>
// See LICENSE.txt for licensing details (2-clause BSD License: https://opensource.org/licenses/BSD-2-Clause)

#pragma once
#include <clocale>	/// std::setlocale
#include <cstdlib>	/// std::wcstombs, std::mbstowcs

#pragma warning(disable:4996)


PSD_NAMESPACE_BEGIN

namespace util
{
	//PSD_INLINE std::string Wstring2string(const std::wstring& str)
	//{
	//	typedef std::codecvt_byname<wchar_t, char, std::mbstate_t> F;
	//	static std::wstring_convert<F> strCnv(new F(".936"));
	//	return strCnv.to_bytes(str);
	//}

	//PSD_INLINE std::wstring String2wstring(const std::string& str)
	//{
	//	typedef std::codecvt_byname<wchar_t, char, std::mbstate_t> F;
	//	static std::wstring_convert<F> strCnv(new F(".936"));
	//	return strCnv.from_bytes(str);
	//}

	/*! std::wstring转std::string */
	PSD_INLINE std::string Wstring2string(const std::wstring& wstr)
	{
		std::setlocale(LC_CTYPE, "");
		const wchar_t* wcstr = wstr.c_str();
		std::size_t wcstrlen = std::wcstombs(nullptr, wcstr, 0) + 1;
		char* cstr = new char[wcstrlen];
		std::memset(cstr, 0, wcstrlen);
		std::wcstombs(cstr, wcstr, wcstrlen);
		std::string str(cstr);
		delete[]cstr;
		return str;
	}

	/*! std::string转std::wstring */
	PSD_INLINE std::wstring String2wstring(const std::string& str)
	{
		std::setlocale(LC_CTYPE, "");
		const char* cstr = str.c_str();
		std::size_t cstrlen = std::mbstowcs(nullptr, cstr, 0) + 1;
		wchar_t* wcstr = new wchar_t[cstrlen];
		std::wmemset(wcstr, 0, cstrlen);
		std::mbstowcs(wcstr, cstr, cstrlen);
		std::wstring wstr(wcstr);
		delete[]wcstr;
		return wstr;
	}
}



PSD_NAMESPACE_END
