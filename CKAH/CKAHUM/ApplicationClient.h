#if !defined(AFX_APPLICATIONCLIENT_H__E7FA7B96_878D_4B96_B056_7C2EFAA88B1D__INCLUDED_)
#define AFX_APPLICATIONCLIENT_H__E7FA7B96_878D_4B96_B056_7C2EFAA88B1D__INCLUDED_

#include <CKAH/ckahum.h>

class CApplicationClient : public CGuardClient
{
public:
	CApplicationClient ();
	virtual ~CApplicationClient ();

	bool Enable (bool bEnable);
	void SignalStop ();

	void SetApplicationNotifyCallback (CKAHFW::APPLICATIONRULENOTIFYCALLBACK ApplicationNotifyCallback,
											LPVOID lpCallbackParam)
	{
		m_fnApplicationNotifyCallback = ApplicationNotifyCallback;
		m_lpCallbackParam = lpCallbackParam;
	}

	void SetChecksumNotifyCallback (CKAHFW::CHECKSUMNOTIFYCALLBACK ChecksumCallback,
											LPVOID lpChecksumCallbackContext)
	{
		m_fnChecksumCallback = ChecksumCallback;
		m_lpChecksumCallbackContext = lpChecksumCallbackContext;
	}

	enum FilterState
	{
		fNotFound,
		fBlock,
		fAllow
	};

	FilterState GetBlockAllowAllFiltersState ();
	FilterState GetDefaultAnswerFiltersState ();

	BOOL RemoveDefaultAnswerFilters ();

	bool AddBlockFilters (bool bIsBlock);
	bool AddBlockDefaultFilters (bool bIsBlock);
	bool AddAskUserDefaultFilters ();

protected:

	bool Create (HANDLE hShutdownEvent);
	void StopClient ();

//	virtual ULONG AddFilter(const CGuardFilter &filter);

	virtual VERDICT OnEvent (PVOID pMessage);

	ULONG AddDefaultFilter (CGuardFilter &flt);
//	ULONG GetLastUserFilterID (DWORD Mj, OUT bool &bIsFirst);

	bool FindFilterGetFlags (ULONG nParam, ULONG *pFlags);

	bool ChangeChecksum(const UUID &uuid, const UCHAR checksum[FW_HASH_SIZE]);

	CKAHFW::APPLICATIONRULENOTIFYCALLBACK m_fnApplicationNotifyCallback;
	LPVOID m_lpCallbackParam;

	CKAHFW::CHECKSUMNOTIFYCALLBACK m_fnChecksumCallback;
	LPVOID m_lpChecksumCallbackContext;
};

#endif // !defined(AFX_APPLICATIONCLIENT_H__E7FA7B96_878D_4B96_B056_7C2EFAA88B1D__INCLUDED_)
