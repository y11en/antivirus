struct CHARSET
{
	enum eChs {eChsWindows = 0, eChsUtf, eChsKoi8, eChsOem, eChsISO8859, eChsXMac};

	eChs	nType;
	tDWORD	nSubType;
	tDWORD	nCodepage;
};

void _ChsetDec_Init();
void _ChsetDec_Deinit();
void _ChsetDec_Parse(const tCHAR * strCharset, CHARSET& chs);
void _ChsetDec_Str2Utf16(std::wstring& dst, const CHARSET& chs, const tCHAR * src, tDWORD nSrcLen);
