// DataProtection.h: interface for the KLADMSRV::DataProtection 
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DATAPROTECTION_H__99034450_AC1B_4C67_8373_83D5CC4EC30F__INCLUDED_)
#define AFX_DATAPROTECTION_H__99034450_AC1B_4C67_8373_83D5CC4EC30F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <std/base/klstd.h>

namespace KLPlugin {

class KLSTD_NOVTABLE CDataProtection : public KLSTD::KLBase
{
public:
    virtual void ProtectData(const void* pData,
							 size_t nData,
							 KLSTD::MemoryChunk** ppChunk) = 0;
};

typedef KLSTD::CAutoPtr<CDataProtection> CDataProtectionPtr;

};

#endif // !defined(AFX_DATAPROTECTION_H__99034450_AC1B_4C67_8373_83D5CC4EC30F__INCLUDED_)
