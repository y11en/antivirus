/*!
 * (C) 2003 Kaspersky Lab 
 * 
 * \file	prssconst.h
 * \author	Andrew Kazachkov
 * \date	31.03.2003 18:44:51
 * \brief	
 * 
 */
/*KLCSAK_PUBLIC_HEADER*/

#ifndef __PRSSCONST_H__sOEwHBhrHf4XQ56duq8m41
#define __PRSSCONST_H__sOEwHBhrHf4XQ56duq8m41
/*KLCSAK_BEGIN_PRIVATE*/
#ifdef _WIN32
# include <tchar.h>
# include <avpregid.h>
#endif

#define KLPRSS_SSF_DIRECT   0
#define KLPRSS_SSF_MACHINE  1
#define KLPRSS_SSF_POLICY   2
#define KLPRSS_SSF_USER     4

#define KLPRSS_CF_USE_CACHE 64         //use caching


/*KLCSAK_END_PRIVATE*/

namespace KLPRSS
{
/*KLCSAK_BEGIN_PRIVATE*/
	#define KLPRSS_VERSION_INFO		L"KLPRSS_info"
//! variables names
	const wchar_t c_wstrSyncVersion[]=L"KLPRSS_SS_VERSION";  //LONG_T
    const wchar_t c_wstrSsServerId[]=L"KLPRSS_SS_SERVER_ID"; //STRING_T
    const wchar_t c_wstrSsClientId[]=L"KLPRSS_SS_CLIENT_ID"; //STRING_T
    const wchar_t c_wstrSsId[]=L"KLPRSS_SS_ID";              //STRING_T
    const wchar_t c_wstrSsIsNew[]=L"KLPRSS_SS_NEW";          //BOOL_T
    const wchar_t c_wstrSsChanged[]=L"KLPRSS_SS_CHANGED";    //BOOL_T

/*KLCSAK_END_PRIVATE*/

    #define KLPRSS_TASKS_POLICY		L"KLPRSS_TASKS_POLICY"

/*KLCSAK_BEGIN_PRIVATE*/
//! ss attributes names
	const wchar_t c_wstrData[]=L"dat";
	const wchar_t c_wstrChanged[]=L"chg";
    const wchar_t c_wstrNew[]=L"new";
	const wchar_t c_wstrHash[]=L"hash";
    
    const wchar_t c_szwSsFmtType[] = L"type"; //INT_T

    typedef enum
    {
        SFT_DEFAULT = 0,
        SFT_SEMIPACKED = 1

    }
    store_fmt_type_t;

//! files, directories
	const wchar_t c_szwFilePref_Backup[]=L"~";
    const wchar_t c_szwFilePref_Backup2[]=L"~2";    // write-cash protection copy
	const wchar_t c_szwFileExt_Data[]=L".dat";
	const wchar_t c_szwFileExt_Sem[]=L".lck";

    const wchar_t c_szwDirUserData[]=L"E80F56F4-6363-4672-957D-11029D864FE5";
    
    const wchar_t c_szwUserData_FileName[]=L"SS_USER";

    //Policies

#ifdef _WIN32
    const TCHAR c_szRegKey_Components[]=AVP_REGKEY_ROOT _SEP_ AVP_REGKEY_COMPONENTS _SEP_;
#endif

//! Settings storage path structure
    const wchar_t c_chwSSP_Marker=L'|';
    const wchar_t c_szwSSP_Marker[]=L"|";

    //! not used ?
    const wchar_t c_szwSSP_User[]=   L"ss_user";
    //! not used ?
    const wchar_t c_szwSSP_Host[]=   L"ss_host";

    //! part of policy sync name
    const wchar_t c_szwSSP_Policy[]= L"ss_policy";
    //! part of roaming policy sync name
    const wchar_t c_szwSSP_PolicyRoaming[]= L"ss_policy_rm";
    
    //! sync type name
    const wchar_t c_szwSSP_Type[]=   L"ss_type";

    //! sync product name
    const wchar_t c_szwSSP_Product[]=L"ss_product";

    //! sync product version
    const wchar_t c_szwSSP_Version[]=L"ss_version";

    //! relative path (used for local tasks)
    const wchar_t c_szwSSP_RelPath[]=L"ss_relpath";

    //!policy id
    const wchar_t c_szwSSP_PolicyId[]=L"ss_policy_id";

    const wchar_t c_szwSSP_GroupTaskSrvPath[]=L"ss_grp_tsk_path";

	const wchar_t c_szwSSP_GrpTSRelPath[]=L"ss_grp_ts_relpath";

    #define KLPRSS_TEST_SS_BASE  (KLSTD_StGetDefDirW() + L"test" + KLSTD::SLASH)
    
//! SS types

    //Global (not product-specific) storage
    #define KLPRSS_GT_PREFIX L"SS_"
    
    const wchar_t c_szwSST_HostSS[]=KLPRSS_GT_PREFIX     L"SETTINGS";    //< machine's global SS
    const wchar_t c_szwSST_CUserSS[]=KLPRSS_GT_PREFIX    L"USER";        //< current user specific SS
    const wchar_t c_szwSST_ProdinfoSS[]=KLPRSS_GT_PREFIX L"PRODINFO";    //< product info SS
    const wchar_t c_szwSST_RuntimeSS[]=KLPRSS_GT_PREFIX  L"RUNTIME";     //< run time info
    const wchar_t c_szwSST_TestGS[]=KLPRSS_GT_PREFIX  L"TEST_GS";

    const wchar_t c_szwSST_LicKeys[]=KLPRSS_GT_PREFIX  L"CACHE1";        //< lickeys special storage
    
    
    //Product specific storage
    #define KLPRSS_PT_PREFIX L"SSP_"        

    const wchar_t c_szwSST_HostSP[]=KLPRSS_PT_PREFIX     L"POLICY";      //< product SS policy
    const wchar_t c_szwSST_HostSPR[]=KLPRSS_PT_PREFIX    L"POLICY_RM";   //< product SS roaming policy
    const wchar_t c_szwSST_TasksSS[]=KLPRSS_PT_PREFIX    L"TASKS";       //< tasks storage
	const wchar_t c_szwSST_PrivateSS[]=KLPRSS_PT_PREFIX  L"SETTINGS";    //< private storage
	const wchar_t c_szwSST_SrvTasksSS[]=KLPRSS_PT_PREFIX  L"SRV_TASKS";   //< server tasks storage
    
    //!ss synthetic types
    const wchar_t c_szwSST_LocalMachine[]=KLPRSS_GT_PREFIX L"LOCAL_MACHINE";
    const wchar_t c_szwSST_CurrentUser[]=KLPRSS_GT_PREFIX L"CURRENT_USER";
    const wchar_t c_szwSST_Smart[]=KLPRSS_GT_PREFIX L"SMART";

    /*
        example
        |ss_type="|SS_TASKS"; ss_product="1093"; ss_version="1.0.0.0"
    */


    //! Protected storage file name. not used unde win32: windows already has its own protected storage

    const wchar_t c_szwProtectedStgFileName[] = L"prtstg";

    //! Protected storage section name. 
    const wchar_t c_szwProtectedStgProduct[] = L"KLPRSS_PRTSTG_PRODUCT";

    //! Protected storage section name. 
    const wchar_t c_szwProtectedStgVersion[] = L"1.0";

    //! Protected storage section name. 
    const wchar_t c_szwProtectedStgSection[] = L"KLPRSS_PRTSTG_KEYS";

/*KLCSAK_END_PRIVATE*/

    // notification parameters sent from server to network agent
    const wchar_t c_szwSPS_EVPNotifications[]=L"KLPRSS_EVPNotifications";//PARAMS_T

/*! Policy structure
    
    Структура данных:
        <container1_name>
         +---<var1_name>=value1 //type1
         +---<var2_name>=value2 //type2


    Соответствующая ей структура политики:
        <container1_name>                              //PARAMS
         +---<c_szwSPS_Value>                          //PARAMS_T
         |          +---<var1_name>                    //PARAMS_T
         |               +---<c_szwSPS_Value>=value1   //type1
         |               +---c_szwSPS_Mandatory        //BOOL_T
         |               +---c_szwSPS_Locked           //BOOL_T
         |               +---c_szwSPS_LockedPolicyName //STRING_T
         |               +---c_szwSPS_LockedGroupName  //STRING_T
         |          +---<var2_name>                    //PARAMS_T
         |               +---<c_szwSPS_Value>=value2   //type2
         |               +---c_szwSPS_Mandatory        //BOOL_T
         |               +---c_szwSPS_Locked           //BOOL_T
         |               +---c_szwSPS_LockedPolicyName //STRING_T
         |               +---c_szwSPS_LockedGroupName  //STRING_T
         +---c_szwSPS_Mandatory                        //BOOL_T
         +---c_szwSPS_Locked                           //BOOL_T
         +---c_szwSPS_LockedPolicyName                 //STRING_T
         +---c_szwSPS_LockedGroupName                  //STRING_T
         +---c_szwSPS_EVPNotifications                 //PARAMS_T

*/

//Mandatory attributes
    // value
    const wchar_t c_szwSPS_Value[]=L"KLPRSS_Val";        //
    // is value mandatory
    const wchar_t c_szwSPS_Mandatory[]=L"KLPRSS_Mnd";    //BOOL_T
//Optional attributes
    // is value locked ?
    const wchar_t c_szwSPS_Locked[]=L"KLPRSS_ValLck";    //BOOL_T
    // policy name (if value is locked)
    const wchar_t c_szwSPS_LockedPolicyName[]=L"KLPRSS_ValLckPolicy";//STRING_T
    // group name (if value is locked)
    const wchar_t c_szwSPS_LockedGroupName[]=L"KLPRSS_ValLckGroup";//STRING_T
    // policy id (if value is locked)
    const wchar_t c_szwSPS_LockedPolicyId[] = L"KLPRSS_ValLckPolId"; //INT_T
    // group id (if value is locked)
    const wchar_t c_szwSPS_LockedGroupId[] = L"KLPRSS_ValLckGrpId"; //INT_T

    /*
        example
        |ss_type="|SS_TASKS"; ss_product="1093"; ss_version="1.0.0.0"
    */    
};

#endif //__PRSSCONST_H__sOEwHBhrHf4XQ56duq8m41

// Local Variables:
// mode: C++
// End:
