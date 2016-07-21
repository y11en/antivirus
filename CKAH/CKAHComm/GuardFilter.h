// GuardFilter.h: interface for the CGuardFilter class.
//
//////////////////////////////////////////////////////////////////////

#if !defined (AFX_GUARDFILTER_H__C698C235_4C4C_4ED6_87BA_16E95E73FDF8__INCLUDED_)
#define AFX_GUARDFILTER_H__C698C235_4C4C_4ED6_87BA_16E95E73FDF8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <list>
#include <Hook/avpgcom.h>
#include <Hook/Hookspec.h>
#include <Hook/fssync.h>

typedef	std::list<PFILTER_PARAM> PARAMLIST;
typedef PARAMLIST::iterator ppPARAM;

class CGuardFilter 
{
public:
	CKAHCOMMEXPORT CGuardFilter ();
	CKAHCOMMEXPORT CGuardFilter (DWORD HookID, DWORD FunctionMj, DWORD FunctionMi, DWORD dwFlags);
	CKAHCOMMEXPORT virtual ~CGuardFilter ();

	CKAHCOMMEXPORT CGuardFilter (const CGuardFilter& filter);
	CKAHCOMMEXPORT CGuardFilter (const PVOID pFilter);
		
	
	CKAHCOMMEXPORT CGuardFilter& operator= (const CGuardFilter& filter);

	CKAHCOMMEXPORT ppPARAM AddParam (PFILTER_PARAM pParam);
	CKAHCOMMEXPORT ppPARAM AddParam (ULONG uParamID, FLT_PARAM_OPERATION ParamFlt, ULONG uParamSize, LPVOID pParamValue, DWORD dwParamFlags = _FILTER_PARAM_FLAG_MUSTEXIST);

	CKAHCOMMEXPORT PFILTER_PARAM FindParam (ULONG uParamID) const;

	CKAHCOMMEXPORT bool RemoveParam (PARAMLIST::iterator iter);

	CKAHCOMMEXPORT PFILTER_TRANSMIT AllocateFilter () const;

public:
	CKAHCOMMEXPORT void Disable ();
	CKAHCOMMEXPORT void Enable ();
    CKAHCOMMEXPORT ULONG GetGroupID ();
    CKAHCOMMEXPORT void SetGroupID (ULONG GroupID);
	CKAHCOMMEXPORT BOOL IsBufferLocked () const;
	CKAHCOMMEXPORT const PARAMLIST& GetParamList () const;

	CKAHCOMMEXPORT void RemoveAllParams ();

	CKAHCOMMEXPORT void ReleaseInternalBuffer ();
	CKAHCOMMEXPORT PFILTER_TRANSMIT GetInternalBuffer (DWORD dwBufSize = 1024);
	CKAHCOMMEXPORT int GetParamCount () const;
	CKAHCOMMEXPORT size_t GetFilterSize () const;

	ULONG	m_HookID;					// 
	ULONG	m_FunctionMj;				// 
	ULONG	m_FunctionMi;				// 
	PROCESSING_TYPE m_ProcessingType;	// see PROCESSING_TYPE
	ULONG	m_Flags;					// see FLT_A_???
	ULONG	m_Timeout;					// timeout (request window life time)
	__int64 m_Expiration;				// filter time life
	std::string	m_szProcName;			// Wildcard "*" possible
	DWORD	m_FilterID;
    ULONG   m_GroupID;                  // filer group id (0 means no group)

protected:
	ppPARAM AddParamInternal (ULONG uParamID, FLT_PARAM_OPERATION ParamFlt, ULONG uParamSize, LPVOID pParamValue, DWORD dwParamFlags = _FILTER_PARAM_FLAG_MUSTEXIST);
	PFILTER_TRANSMIT m_BufferAlloc;
	PARAMLIST	m_lstParams;

private:
	int AddParamsFromFilterTransmit (PFILTER_TRANSMIT pFlt);
	ppPARAM AddParamInternal (PFILTER_PARAM pParam);
	
};

#endif // !defined (AFX_GUARDFILTER_H__C698C235_4C4C_4ED6_87BA_16E95E73FDF8__INCLUDED_)
