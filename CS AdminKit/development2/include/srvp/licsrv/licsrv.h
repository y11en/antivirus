#ifndef __KL__LICSRV_H__
#define __KL__LICSRV_H__

#include <nagent/naginst/nagent_const.h>
#include <srvp/evp/notificationproperties.h>
#include <srvp/csp/klcsp.h>

namespace KLLICSRV
{
	/*!
		Типы ключей
	*/
	enum LicKeyType{
		KEY_COMMERCIAL	= 1,
		KEY_BETA		= 2,
		KEY_TRIAL		= 3,
		KEY_TEST		= 4,
		KEY_OEM			= 5
	};

    /*!
        На сервере имеется таблица со следующими столбцами. С помощью
        SrvLicences::GetKeysInfo() можно запрашивать значения из этой таблицы.
    */    
    const wchar_t c_szwIK_Serial[]=L"KLLICSRV_IK_SERIAL";               //STRING_T
    const wchar_t c_szwNK_Serial[]=L"KLLICSRV_NK_SERIAL";               //STRING_T
    const wchar_t c_szwIK_AppId[]=L"KLLICSRV_IK_APPID";                 //INT_T
    const wchar_t c_szwNK_AppId[]=L"KLLICSRV_NK_APPID";                 //INT_T
    const wchar_t c_szwIK_LicCount[]=L"KLLICSRV_IK_LICCOUNT";           //INT_T
    const wchar_t c_szwNK_LicCount[]=L"KLLICSRV_NK_LICCOUNT";           //INT_T
    const wchar_t c_szwKeyInstallTime[]=L"KLLICSRV_INSTALLATION_TIME";  //DATE_TIME_T
    const wchar_t c_szwHostName[]=L"KLLICSRV_HOST_NAME";                //STRING_T
    const wchar_t c_szwHostDisplName[]=L"KLLICSRV_HOST_DISPL_NAME";     //STRING_T
    const wchar_t c_szwHostDomain[]=L"KLLICSRV_HOST_DOMAIN";            //STRING_T
    const wchar_t c_szwHost_Group[]=L"KLLICSRV_HOST_GROUP";             //INT_T
    const wchar_t c_szwHost_GroupName[]=L"KLLICSRV_HOST_GROUPNAME";     //STRING_T
    const wchar_t c_szwProductName[]=L"KLLICSRV_PRODUCT_NAME";          //STRING_T
    const wchar_t c_szwProductVersion[]=L"KLLICSRV_PRODUCT_VERSION";    //STRING_T
    const wchar_t c_szwKeyExpirationTime[]=L"KLLICSRV_EXPIRATION_TIME"; //DATE_TIME_T
    const wchar_t c_szwIK_Type[]=L"KLLICSRV_IK_TYPE";					//INT_T
    const wchar_t c_szwNK_Type[]=L"KLLICSRV_NK_TYPE";					//INT_T
    const wchar_t c_szwIK_ProdSuiteId[]=L"KLLICSRV_IK_PRODSUIE_ID";     //INT_T
    const wchar_t c_szwNK_ProdSuiteId[]=L"KLLICSRV_NK_PRODSUIE_ID";     //INT_T
	const wchar_t c_szwIK_MajVer[]=L"KLLICSRV_IK_MAJ_VER";			    //INT_T
	const wchar_t c_szwNK_MajVer[]=L"KLLICSRV_NK_MAJ_VER";			    //INT_T
	const wchar_t c_szwIK_LicPeriod[]=L"KLLICSRV_IK_LIC_PERIOD";			//INT_T
	const wchar_t c_szwNK_LicPeriod[]=L"KLLICSRV_NK_LIC_PERIOD";			//INT_T
    const wchar_t c_szwIKProductName[]=L"KLLICSRV_IK_PRODUCT_NAME";         //STRING_T
    const wchar_t c_szwNKProductName[]=L"KLLICSRV_NK_PRODUCT_NAME";         //STRING_T
    
    typedef KLCSP::host_id_t host_id_t;
    
    class KLSTD_NOVTABLE SrvLicences
        :   public KLSTD::KLBaseQI
    {
    public:
        virtual void InstallKeyForGroup(
                            const std::wstring&             wstrKeyFile,                            
                            const std::wstring&             wstrProductName,
                            const std::wstring&             wstrVersion,
                            bool                            bIsCurrentKey,
                            long                            idGroup,
                            KLEVP::notification_descr_t*    pNotifications,
                            int                             nNotifications) = 0;
        
        virtual void InstallKeyForHosts(
                            const std::wstring&             wstrKeyFile,                            
                            const std::wstring&             wstrProductName,
                            const std::wstring&             wstrVersion,
                            bool                            bIsCurrentKey,
                            host_id_t*                      pHosts,
                            int                             nHosts,
                            KLEVP::notification_descr_t*    pNotifications,
                            long                            nNotifications) = 0;

        virtual void GetKeysInfo(
                            const wchar_t**     ppFields,
                            int                 nFields,
                            const std::wstring& wstrFilter,
                            long                lTimeout,
                            KLCSP::ForwardIterator**   ppIterator) = 0;

        virtual long GetKeyInfo(
							const std::vector<std::wstring>& vect_fields,
							const std::vector<KLCSP::field_order_t>& vect_fields_to_order,
                            const std::wstring& wstrSerial,
                            long                lTimeout,
                            KLCSP::ChunkAccessor**   ppAccessor) = 0;

        virtual void DeleteKeyInfo(const std::wstring& wstrSerial) = 0;
    };
};

KLCSSRVP_DECL void KLLICSRV_CreateLicSrvProxy(
						const wchar_t*  szwAddress,
						KLLICSRV::SrvLicences** ppHosts,
						const unsigned short* pPorts=NULL,
                        bool            bUseSsl = true);

KLCSSRVP_DECL void KLLICSRV_CreateLicSrvLocalProxy(KLLICSRV::SrvLicences** ppHosts);

#endif //__KL__LICSRV_H__

// Local Variables:
// mode: C++
// End:
