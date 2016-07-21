/*!
 * (C) 2003 Kaspersky Lab 
 * 
 * \file	prssconst.h
 * \author	Andrew Kazachkov
 * \date	31.03.2003 18:44:51
 * \brief	
 * 
 */

#ifndef __PRSSCONST_H__sOEwHBhrHf4XQ56duq8m41
#define __PRSSCONST_H__sOEwHBhrHf4XQ56duq8m41

namespace KLPRSS
{

    #define KLPRSS_TASKS_POLICY		L"KLPRSS_TASKS_POLICY"


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
