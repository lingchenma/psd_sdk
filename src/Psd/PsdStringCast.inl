namespace util
{
	// ---------------------------------------------------------------------------------------------------------------------
	// ---------------------------------------------------------------------------------------------------------------------

	PSD_INLINE std::string Wstring2string(const std::wstring& str)
	{
		typedef std::codecvt_byname<wchar_t, char, std::mbstate_t> F;
		static std::wstring_convert<F> strCnv(new F(".936"));
		return strCnv.to_bytes(str);
	}

	PSD_INLINE std::wstring String2wstring(const std::string& str)
	{
		typedef std::codecvt_byname<wchar_t, char, std::mbstate_t> F;
		static std::wstring_convert<F> strCnv(new F(".936"));
		return strCnv.from_bytes(str);
	}
}
