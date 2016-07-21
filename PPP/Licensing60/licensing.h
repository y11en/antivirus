// AVP Prague stamp begin( Interface header,  )
// -------- ANSI C++ Code Generator 1.0 --------
// -------- Monday,  16 April 2007,  18:02 --------
// -------------------------------------------
// Copyright © Kaspersky Lab 1996-2006.
// -------------------------------------------
// Project     -- Not defined
// Sub project -- Not defined
// Purpose     -- Not defined
// Author      -- Guschin
// File Name   -- licensing.cpp
// -------------------------------------------
// AVP Prague stamp end



// AVP Prague stamp begin( Header ifndef,  )
#if !defined( __licensing_cpp__a68eda17_caf4_46ae_b8cc_93b76d393288 )
#define __licensing_cpp__a68eda17_caf4_46ae_b8cc_93b76d393288
// AVP Prague stamp end



// AVP Prague stamp begin( Header includes,  )
#include <prague.h>
#include <iface/i_root.h>
#include <pr_vtbl.h>
#include <plugin/p_licensing60.h>
// AVP Prague stamp end



#include <iface/i_io.h>
#include <iface/i_mutex.h>
#include <iface/i_event.h>
#include <iface/i_csect.h>
#include <iface/i_taskmanager.h>
#include <plugin/p_cryptohelper.h>

#include <structs/s_licensing.h>

#include "cmndefs.h"
#include "licensing_policy.h"

#include "Activation.h"
#include <memory>



#define IMPEX_IMPORT_LIB
    #include <iface/e_loader.h>



#define _LP(mbr)    LicensingPolicy::##mbr
#define _ILP(mbr)   LicensingPolicy::ILicensingPolicy::##mbr
#define _CLP(mbr)   LicensingPolicy::CLicensingPolicy::##mbr



// AVP Prague stamp begin( C++ class declaration,  )
struct pr_novtable CLicensing60 : public cLicensing , protected IActivationCallback
{
private:
// Internal function declarations
	tERROR pr_call ObjectInit();
	tERROR pr_call ObjectInitDone();
	tERROR pr_call ObjectPreClose();
	tERROR pr_call MsgReceive( tDWORD p_msg_cls_id, tDWORD p_msg_id, hOBJECT p_send_point, hOBJECT p_ctx, hOBJECT p_receive_point, tVOID* p_par_buf, tDWORD* p_par_buf_len );

// Property function declarations

public:
// External function declarations
	tERROR pr_call GetActiveKeyInfo( cSerializable* p_pResult );
	tERROR pr_call GetInstalledKeysInfo( cSerializable* p_pResult );
	tERROR pr_call RevokeActiveKey();
	tERROR pr_call RevokeKey( cSerializable* p_pKeySerialNumber );
	tERROR pr_call CheckKeyFile( tWSTRING p_szKeyFileName, tDWORD p_dwKeyAddMode, cSerializable* p_pResult );
	tERROR pr_call CheckActiveKey( cSerializable* p_pResult );
	tERROR pr_call AddKey( tWSTRING p_szFileName, tDWORD p_dwKeyAddMode );
	tERROR pr_call AddOnlineKey( cSerializable* p_pUserData, tDWORD p_dwKeyAddMode, cSerializable* p_pResult, tDWORD* p_pdwOLASrvErr );
	tERROR pr_call StopOnlineLoadKey();
	tERROR pr_call ClearTrialLimitDate();
	tERROR pr_call GetInfo( cSerializable* p_pInfo );
	tERROR pr_call AddOLAKey( cSerializable* p_pUserData, tDWORD p_dwKeyAddMode, hSTRING p_pResForm, cSerializable* p_pResult, tDWORD* p_pdwOLAErrCode, tDWORD* p_pdwOLAErrCodeSpecific );
	tERROR pr_call SubmitOLAForm( cSerializable* p_pForm, tDWORD p_dwKeyAddMode, hSTRING p_pResForm, cSerializable* p_pResult, tDWORD* p_pdwOLAErrCode, tDWORD* p_pdwOLAErrCodeSpecific );
	tERROR pr_call StopOLA();
	tERROR pr_call GetCustomerCredentials( hSTRING p_strCustomerId, hSTRING p_strCustomerPassword );

// Data declaration
	hIO     m_hStorage;        // --
	hSTRING m_hBasesPath;      // --
	tBOOL   m_blUsesBlacklist; // --
	iTaskManager* m_hTM;             // --
// AVP Prague stamp end

private:
    tERROR ReinitLicensing();

    tERROR GetError(HRESULT p_hError) ;

    tERROR ConvertKeySerialNumber_L(_LP(key_serial_number_t)& p_rStruct, 
                                    cKeySerialNumber* p_pSer) ;

    tERROR ConvertLicensedObject_L(_LP(dword_t) p_dwKey, 
                                   _LP(dword_t) p_dwValue, 
                                   cLicensedObject* p_pSer) ;

    tERROR ConvertComponentsLevel_L(_LP(dword_t) p_dwKey, 
                                    _LP(dword_t) p_dwValue, 
                                    cComponentsLevel* p_pSer) ;

    tERROR ConvertKeyInfo_L(_LP(key_info_t)& p_rStruct, 
                            cKeyInfo* p_pSer) ;

    tERROR ConvertDT_L(_LP(date_t)& p_rStruct, 
                       tDT* p_pDT) ;

    tERROR ConvertCheckInfo_L(_ILP(check_info_t)& p_rCheckStruct, 
                              _LP(installed_key_info_t)* p_rInstallStruct,
                              cCheckInfo* p_pSer) ;

    tERROR ConvertCheckInfoList_L(_ILP(check_info_list_t)& p_rCheckStruct, 
                                  _CLP(installed_key_info_list_t)& p_rInstallStruct, 
                                  cCheckInfoList* p_pSer) ;

    tERROR ConvertKeySerialNumber_P(cSerializable* p_pSer, 
                                    _LP(key_serial_number_t)& p_rStruct) ;

    _ILP(key_add_mode_t) ConvertKeyAddMode_P(tDWORD p_dwValue) ;

    bool IsKeyForProduct(cKeyInfo * pInfo) ;

    bool GetInstallKeyInfo(_ILP(check_info_t)& p_rCheckStruct, 
                           _LP(installed_key_info_t)& p_rInstallStruct) ;

    tDWORD UpdateKeyAddMode(tDWORD p_dwKeyAddMode) ;

	tERROR ProcessActivationResult(
		tDWORD p_dwKeyAddMode,
		hSTRING p_pResForm,
		cSerializable* p_pResult,
		tDWORD* p_pdwOLAErrCode,
		tDWORD* p_pdwOLAErrCodeSpecific);

	void UpdateCustomerCredentials(const cStringObj& strCustomerId, const cStringObj& strPassword);

	void OnSendingRequest();
	void OnRequestSent(size_t nBytes);
	void OnRequestComplete();
	bool GetProxyCredentials(char* user, size_t user_size, char* password, size_t password_size);

	tERROR _AddOLAKey( cSerializable* p_pUserData, tDWORD p_dwKeyAddMode, hSTRING p_pResForm, cSerializable* p_pResult, tDWORD* p_pdwOLAErrCode, tDWORD* p_pdwOLAErrCodeSpecific );
	tERROR _SubmitOLAForm( cSerializable* p_pForm, tDWORD p_dwKeyAddMode, hSTRING p_pResForm, cSerializable* p_pResult, tDWORD* p_pdwOLAErrCode, tDWORD* p_pdwOLAErrCodeSpecific );
	tERROR _StopOLA();
	tERROR _GetCustomerCredentials( hSTRING strCustomerId, hSTRING strCustomerPassword );

private:

	cStrObj GetTrialCode();

	static int writeLicData(const char* s, 
                            unsigned int size, 
                            void* context) ;

    static int readLicData(char** s, 
                           unsigned int* size, 
                           void* context) ;

    cRegistry * _OpenLicDataReg(bool bModify, 
                                hOBJECT regRoot = NULL) ;

    bool _CheckLicDataReg(hOBJECT regRoot = NULL) ;

    int _ReadLicDataReg(char** s, 
                        unsigned int* size, 
                        hOBJECT regRoot = NULL) ;

    int _WriteLicDataReg(const char* s, 
                         unsigned int size, 
                         hOBJECT regRoot = NULL) ;

private:
    _LP(CLicensingPolicy)*         m_licPolicy ;
    ILicenseStorage                m_licenseStorage ;
    hMUTEX                           m_hSync ;
    hCRITICAL_SECTION              m_hCSOnline ;
    _LP(string_t)                  m_strBlackListFile ;
    _app_info_t*                   m_appInfo ;
    tINT                           m_nProdId ;
    cStrObj                        m_strListStg ;
	tBOOL                          m_blGetPassForProxyFromUser;
	cAuthInfo                      m_auth_info ;
	std::auto_ptr<CActivation>     m_pActivation;

// AVP Prague stamp begin( C++ class declaration end,  )
public:
	mDECLARE_INITIALIZATION(CLicensing60)
};
// AVP Prague stamp end



// AVP Prague stamp begin( Header endif,  )
#endif // licensing_cpp
// AVP Prague stamp end



