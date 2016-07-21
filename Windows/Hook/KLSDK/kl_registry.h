#ifndef _KL_REGISTRY_H
#define _KL_REGISTRY_H

#include "kl_object.h"
#include "kldef.h"
#include "klstatus.h"

class CKl_Reg: public CKl_Object
{
private:
#ifndef ISWIN9X
	PWCHAR		m_Base_KeyPath;
    HANDLE		m_hKey;
#else
	HKEY		m_hBase_KeyRoot;
	PCHAR		m_ansiBase_KeyPath;
	HKEY		m_hKey;
#endif

#ifdef ISWIN9X
	KLSTATUS	QualifyRegRoot(PCHAR ansiKeyPath, PCHAR *p_ansiSubKey_Path, PHKEY p_hKeyRoot);
#endif
public: 
    KLSTATUS	OpenKey(PWCHAR KeyPath);
    void		CloseKey();
    KLSTATUS    Delete();
    KLSTATUS	CreateSubKey(PWCHAR SubKey);
    KLSTATUS	QueryValue(PWCHAR ValueName, PULONG pType, PVOID Data, PULONG pDataSize);
    KLSTATUS	SetValue(PWCHAR ValueName, ULONG Type, PVOID Data, ULONG DataSize);
    KLSTATUS	EnumValues(ULONG EnumIndex, PWCHAR ValueName, PULONG pValueName_Len);


    CKl_Reg();
    virtual ~CKl_Reg();
};

#endif