#ifndef __RDB_HELPER_94A124_H_
#define __RDB_HELPER_94A124_H_

#include "rdb_types.h"
#include <Abstract/al_file.h>

class cReportDBHelper {
public:
	virtual bool Init(const wchar_t* sFolder, bool bClear) = 0;

	virtual tDateTime GetSystemTime() = 0; // as Windows FILETIME
	virtual tDateTime GetLocalTime() = 0; // as Windows FILETIME
	virtual tERROR GetApplicationId(tNativePid PID, tAppId* pAppId) = 0;
	virtual tERROR OpenFile(wchar_t* nName, uint32 nSegment, bool bCreate, bool bWriter, alFile** ppFile) = 0;
	virtual tERROR DeleteFile(wchar_t* nName, uint32 nSegment) = 0;
	virtual void* Alloc(size_t size) = 0;
	virtual void Free(void* pmem) = 0;
	virtual bool ConvertWC2UTF8(const wchar_t* wcString, size_t cbWideChars, char* pUtf8String, size_t cbUtf8BufferSize, size_t* pcbUtf8Size) = 0;
	virtual bool ConvertUTF82WC(const char* pUtf8String, size_t cbUtf8Size, wchar_t* wcString, size_t cbWCBufferSize, size_t* pcbWCSize) = 0;
	virtual unsigned short ConvertWC2UPR(unsigned short wc) 
	{ 
		if (wc >= 'a' && wc <= 'z')
			return (wc & ~0x20);
		return wc;
	};
};

class cRDBHelperAutoMem {
public:
	cRDBHelperAutoMem(cReportDBHelper* pHelper, void** pptr) : 
		m_pHelper(pHelper), 
		m_pptr(pptr)
	{
	}
	~cRDBHelperAutoMem()
	{
		if (m_pptr && *m_pptr)
		{
			m_pHelper->Free(*m_pptr);
			*m_pptr = NULL;
		}
	}

private:
	cReportDBHelper* m_pHelper;
	void** m_pptr;
};

#endif // __RDB_HELPER_94A124_H_