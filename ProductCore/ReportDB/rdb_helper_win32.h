#ifndef __RDB_HELPER_WIN32_
#define __RDB_HELPER_WIN32_

#include "rdb_helper.h"

class cReportDBHelperWin32 : public cReportDBHelper
{
public:
	cReportDBHelperWin32();
	~cReportDBHelperWin32();

	bool Init(const wchar_t* sFolder, bool bClear);
	tDateTime GetSystemTime(); // as Windows FILETIME
	tDateTime GetLocalTime();
	tERROR GetApplicationId(tNativePid PID, tAppId* pAppId);
	tERROR OpenFile(wchar_t* nName, uint32 nSegment, bool bCreate, bool bWriter, alFile** ppFile);
	tERROR DeleteFile(wchar_t* nName, uint32 nSegment);
	void* Alloc(size_t size);
	void Free(void* pmem);
	bool ConvertWC2UTF8(const wchar_t* wcString, size_t cbWideChars, char* pUtf8String, size_t cbUtf8BufferSize, size_t* pcbUtf8Size);
	bool ConvertUTF82WC(const char* pUtf8String, size_t cbUtf8Size, wchar_t* wcString, size_t cbWCBufferSize, size_t* pcbWCSize);
	unsigned short ConvertWC2UPR(unsigned short wc);

private:
	wchar_t* m_sFolder;
};

#endif // __RDB_HELPER_WIN32_
