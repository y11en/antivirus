#pragma once

#include <windows.h>
#include <string>
#include <map>
#include <version/ver_product.h>

typedef std::string string;

void   RemoveLastSlash(string &strPath);
string GetFileFormPath(const string &strPath);
string LoadResourceString(UINT nStringID);

#define LoadRS LoadResourceString

//////////////////////////////////////////////////////////////////////////
// CTraceManager

// -------------------------------------------------------------------------------------------------
// 6 version defines
#define VER_COMPANY_REGISTRY_PATH_6			"Software\\KasperskyLab"
#define VER_PRODUCT_REGISTRY_ROOT_6			VER_COMPANY_REGISTRY_PATH_6
#define VER_PRODUCT_REGISTRY_NAME_6			"AVP6"
#define VER_PRODUCT_REGISTRY_PATH_6			VER_PRODUCT_REGISTRY_ROOT_6 "\\" VER_PRODUCT_REGISTRY_NAME_6

#define VER_PRODUCT_APPDATA_NAME_6            "AVP6"
// -------------------------------------------------------------------------------------------------

#define AVP6_TRACE_SERVER      "beta.kaspersky-labs.com" // test srf - 183038161

class CTraceManager
{
public:
	CTraceManager();
	~CTraceManager();

public:
	bool   Init();

	string GetProductLocation();
	string GetProductVersion();
	
	void   EnableTrace(bool bEnable = true);
	bool   IsTraceEnabled();
	bool   IsAnyTracesExists();
	void   SetLevel(UINT nLevel);
	UINT   GetLevel();
	void   SetSRF(UINT nSRF);
	UINT   GetSRF();
	bool   Send2KL();
	bool   ZipTraces();
	string GetZippedTraces();
	bool   LogRegistry(LPCSTR strFilePrefix, LPCSTR strRegPath, string &strResultFile);

	UINT   GetComponentCount();
	string GetComponentName(UINT index);
	void   SetComponentLevel(UINT index, UINT nLevel);
	UINT   GetComponentLevel(UINT index);

	bool   IsDumpEnabled() const;
	void   TryToCreateDump();

protected:
	void   DeleteZippedTraces();

protected:
	typedef std::map<DWORD, DWORD> TPidToLevel;
	TPidToLevel m_PidToLevel;
	string  m_strTraceLocation;
	string  m_strAvpExeLocation;
	string  m_strProductLocation;
	string  m_strProductVersion;
	string  m_strZippedLogs;
	BOOL    m_bTraceEnabled;
	DWORD   m_nTraceLevel;
	UINT    m_nSRF;
	BOOL	m_bIsDumpEnabled;
};
