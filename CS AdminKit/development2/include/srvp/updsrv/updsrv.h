/*!
 * (C) 2003 Kaspersky Lab 
 * 
 * \file	updsrv.h
 * \author	Andrew Kazachkov
 * \date	28.05.2003 13:12:36
 * \brief	
 * 
 */

#ifndef __KL_UPDSRV_H__96D3468A_D243_4f26_8112_0D29586BA03E
#define __KL_UPDSRV_H__96D3468A_D243_4f26_8112_0D29586BA03E

#include <std/base/klbase.h>
#include <std/par/params.h>
//#include <srvp/updsrv/updatercompconstants.h>

namespace KLUPDSRV
{
    struct BundleOptions
    {
        std::wstring wstrBundleId;  //< id пакета
        //bool bOrderToLoad;             //< если true, то при следующей загрузке пакет будет загружен, если false, то не будет загружен.
        bool bAutoInstall;          //< если true, то пакет будет автомтически инсталлирован на всех машинах. Пакет реально может быть еще не загружен в папку ретрансляции.
    };

    struct GeneralOptions
    {
        bool bAutoInstallAVDB;
        bool bAutoInstallAllPatches;
        bool bAutoInstallAllUrgentUpdates;

        GeneralOptions() :
            bAutoInstallAVDB( false ),
            bAutoInstallAllPatches( false ),
            bAutoInstallAllUrgentUpdates( false ) {};
    };

    class KLSTD_NOVTABLE Updates : public KLSTD::KLBase
    {
    public:
        /*!
          \brief	GetUpdatesInfo - возвращает список ретрансляции

          \param	pFilter Если NULL, то будет возвращена вся информация по
                    всем продуктам. Может содержать список ID пакетов, для
                    которых надо возвратить данные:
                        c_spUpdate_Bundles //ARRAY_T
                            c_spUpdate_BundleId //STRING_T

          \param	bIncludeDependantsInfo - если true, то в результат будут включены
                        значения c_spUpdate_BundleDependencies
                    
          \param	ppData - результат
                        c_spUpdate_Bundles //ARRAY_T
                            c_spUpdate_BundleId //STRING_T
                            c_spUpdate_BundleName //STRING_T
                            c_spUpdate_BundleApp //STRING_T
                            c_spUpdate_BundleComponentList //ARRAY_T
                            c_spUpdate_BundleComponent //STRING_T
                            c_spUpdate_BundleToVersion //STRING_T
                            c_spUpdate_BundleComment //STRING_T
                            c_spUpdate_BundleStatus //STRING_T
                            c_spUpdate_BundleSize //LONG_T!!!!!!!!!!!!!!!!!!!!!!!
							c_spUpdate_BundleRecordsCount // LONG_T - только для ComponentidAVS
                            c_spUpdate_BundleType //STRING_T
                            c_spUpdate_BundleDate //DATE_TIME_T
                            c_spUpdate_BundleDownloadDate //DATE_TIME_T
                            c_spUpdate_BundlePostponedToLoad //BOOL_T
                            c_spUpdate_BundleFromVersions //PARAMS_T - содержит пары version / fake bool value
                                <Version> // BOOL_T
                                <Version> // BOOL_T
                                <Version> // BOOL_T
                            [c_spUpdate_BundleDependencies] //ARRAY_T - присутствует только если bIncludeDependantsInfo = true, 
                                        // содержит Params такой же структуры, в массиве верхенго уровня c_spUpdate_Bundles,
                                        // но без элемента c_spUpdate_BundleDependencies (дабы не плодить дерево).
                                        // Т.е. присутствуют элементы c_spUpdate_BundleId, c_spUpdate_BundleName, и т.п.
                                        // Порядок важен, первым идет пакет, непосредственно после которого 
                                        // должен ставиться данный пакет.
                                        // ВАЖНО!!!! Элемент c_spUpdate_BundleDependencies может отсутствовать, либо
                                        // какие-либо из его элеменитов могут быть NULL!!!!

            Если значение c_spUpdate_BundleComponent равно ComponentidAVS и значение c_spUpdate_BundleType равно SS_KEY_Desc,
                то этот пакет описывает антивирусные базы. Такой пакет всегда присутствует в единственном
                экземпляре.

            Если у пакета c_spUpdate_BundleStatus==SS_KEY_PostponedAvailable, это означает, что этот пакет еще
                не загружен с сервера ЛК. В таком случае для этого пакета имеет смысл вызов
                функции SetBundleOptions() - можно указать, что при следующем запуске Updater'а данный пакет
                нужно скачать с сервера ЛК. Либо при помощи той же функции можно снять подобно указание. Если
                же у пакета какой-то другой статус, то вызов этой функции смысла не имеет.
        */
        virtual void GetUpdatesInfo(
            KLPAR::Params* pFilter, bool bIncludeDependantsInfo, KLPAR::Params** ppData ) = 0;
        
        /*!
          \brief	SetBundleOptions - устанавливает опции для пакетов обновления.

          \param	vectOrders - массив с указаниями для различных пакетов. Значение bOrderToLoad 
                имеет значение только для пакетов, у которых статус c_spUpdate_BundleStatus == SS_KEY_PostponedAvailable 
                (т.е. доступен к загрузке, но еще не загружен). Если статус другой, то значение bOrderToLoad при выхове этой функции
                для соотвествующего пакета игнорируется.
        */
        virtual void SetBundleOptions( const std::vector<BundleOptions> & vectOptions ) = 0;

        virtual void SetGeneralOptions( const GeneralOptions & generalOptions ) = 0;
        virtual GeneralOptions GetGeneralOptions() = 0;

    };

	/*
	enum EGetUpdatesTypesInfoParams{
		AvailableUpdateComps = 1,
		AvailableUpdateApps = 2, 
		InstalledComps = 4,
		InstalledApps = 8
	};
	*/

    class KLSTD_NOVTABLE Updates2 : public KLSTD::KLBaseQI{
    public:
		virtual void GetUpdates(Updates** ppUpdates) = 0;

		virtual void GetAvailableUpdatesInfo(
			const wchar_t* wszLocalization,
			KLPAR::Params** ppAvailableUpdateComps,
			KLPAR::Params** ppAvailableUpdateApps) = 0;

		virtual void GetRequiedUpdatesInfo(
			KLPAR::Params** ppInstalledComps,
			KLPAR::Params** ppInstalledApps) = 0;
	};
	
	enum AntiSpamProductType{
		AS_UNQNOWN,
		AS_AntiSpamPersonal_1_0,
		AS_AntiSpamPersonal_1_1,
		AS_SecuritySMTPGateway_5_5,
		AS_SecurityForExchange2003,
		AS_KasperskyAntiSpam_3_0
	};

};

KLCSSRVP_DECL void KLUPDSRV_CreateUpdatesProxy(
							const std::wstring&		wstrLocalComponentName,
							const std::wstring&		wstrConnName,
                            KLUPDSRV::Updates**     ppUpdates,
                            KLSTD::KLBase*          pParent=NULL);

KLCSSRVP_DECL KLUPDSRV::AntiSpamProductType KLUPDSRV_GetAntiSpamProductFromAppId(const std::wstring& wstrAppId);

KLCSSRVP_DECL void KLUPDSRV_GetAppIdsForAntiSpamProductType(KLUPDSRV::AntiSpamProductType type, std::vector<std::wstring>& appIds);

KLCSSRVP_DECL void KLUPDSRV_GetAllAntiSpamProducts(std::vector<KLUPDSRV::AntiSpamProductType>& vectAntiSpamPrds);

#endif //__KL_UPDSRV_H__96D3468A_D243_4f26_8112_0D29586BA03E

// Local Variables:
// mode: C++
// End:
