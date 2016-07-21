// RootSink.h: interface for the CKav6RootSink classes.
//
//////////////////////////////////////////////////////////////////////

#pragma once

#ifndef __ROOTSINK_H__
#define __ROOTSINK_H__


struct cCryptoHelper;

class CKav6RootSink :
	public CItemSinkT<CRootItem, CRootSink>,
	public cGuiGlobals
{
public:
	typedef CItemSinkT<CRootItem, CRootSink> TBase;

	tDWORD       ExpandEnvironmentInt(LPCSTR strName);
	cStringObj   ExpandEnvironmentString(LPCSTR strName);
	bool         ExpandEnvironmentString(cStringObj& strName);

protected:
	tERROR _ShowWindow(CSinkCreateCtx& ctx, bool bModal, bool bSingleton = true);
	
	SINK_MAP_DECLARE();
	cNode * CreateOperator(const char *name, cNode **args, cAlloc *al);

	bool IsTooltipTest();
	bool IsGuiControllerFull();
	bool GetCustomObjectInfo(CObjectInfo& pObject);
	void GetLocalizationId(tString& strLocId);
	void GetHelpStorageId(cStrObj& strHelpStorageId);

	virtual cCryptoHelper* GetCryptoHelper(bool& bSimple) { return NULL; }
	virtual bool CryptDecrypt(cStringObj& str, bool bCrypt);
	virtual bool CheckPasswords(cStringObj& strOldCrypted, cStringObj& strNew);
	virtual bool CheckCondition(CItemBase * pParent, LPCSTR strCondition);
	virtual bool GetAppInfoByPid(tDWORD nPID, CObjectInfo * pObjInfo);
	virtual void OnCreate();
	virtual bool OnAddDataSection(CFieldsBinder *pBinding, LPCSTR strSect);
};

//////////////////////////////////////////////////////////////////////
// CStatisticsSink

class CStatisticsSink : public CReportSink
{
public:
	CStatisticsSink() : m_pReport(NULL){}
	void OnInitProps(CItemProps& pProps);
	void OnDestroy();

	tERROR GetRecordCount( tDWORD* pdwRecords );
	tERROR GetRecord( tDWORD* result, tDWORD dwIndex, tVOID* pBuffer, tDWORD dwSize, tDATETIME* pdtTimestamp );

protected:
	cReport* m_pReport;
};


#endif //__ROOTSINK_H__
