/*!
 * (C) 2002 Kaspersky Lab 
 * 
 * \file	helpersi.h
 * \author	Andrew Kazachkov
 * \date	21.10.2002 18:25:53
 * \brief	
 * 
 */
/*KLCSAK_PUBLIC_HEADER*/

#ifndef _KLPRSS_HELPERSI_H_
#define _KLPRSS_HELPERSI_H_

#include "settingsstorage.h"
#include "prssconst.h"

/*KLCSAK_BEGIN_PRIVATE*/
#include "ss_bulkmode.h"
#include "store.h"
/*KLCSAK_END_PRIVATE*/

bool KLPRSS_HasArrayItemPolicyFmt(KLPAR::Value* pVal);

/*KLCSAK_BEGIN_PRIVATE*/
namespace KLPRSS
{
    void Profiles_Initialize();
    void Profiles_Deinitialize();    

    // returned memory must be freed with 'free'
    ss_format_t* GetDefaultFormat();
    KLCSKCA_DECL void SetDefaultFormat(const ss_format_t* pFormat);
    //throws if NULL or unsupported version
    void CheckFormat(const ss_format_t* pFormat);
    ss_format_t* DupFormat(const ss_format_t* pFormat);

    KLCSKCA_DECL void SetStoreServerId(KLPRSS::Storage* pStorage, const std::wstring& wstrId);
    KLCSKCA_DECL bool GetStoreServerId(KLPRSS::Storage* pStorage, std::wstring& wstrId);
    KLCSKCA_DECL void SetStoreServerId(KLPAR::Params* pVerData, const std::wstring& wstrId);
    bool GetStoreServerId(KLPAR::Params* pVerData, std::wstring& wstrId);
    
    KLCSKCA_DECL void SetStoreClientId(KLPRSS::Storage* pStorage, const std::wstring& wstrId);
    KLCSKCA_DECL bool GetStoreClientId(KLPRSS::Storage* pStorage, std::wstring& wstrId);

    KLCSKCA_DECL void SetStoreChanged(KLPRSS::Storage* pStorage, bool bChanged);
    KLCSKCA_DECL bool GetStoreChanged(KLPRSS::Storage* pStorage);

    KLCSKCA_DECL void SetStoreNew(KLPRSS::Storage* pStorage, bool bNew);
    KLCSKCA_DECL bool GetStoreNew(KLPRSS::Storage* pStorage);
    void SetStoreNew(KLPAR::Params* pVerData, bool bNew);
    bool GetStoreNew(KLPAR::Params* pVerData);
    
    std::wstring& CalcDataHash(KLPAR::ValuePtr pVal, std::wstring& wstrHash);

    std::wstring& CalcParamsHash(KLPAR::ParamsPtr pData, std::wstring& wstrHash);
    
	KLCSKCA_DECL void ForceCreateEntry(
						KLPRSS::Storage*		pStorage,
                        const wchar_t*          szwProduct,
                        const wchar_t*          szwVersion,
                        const wchar_t*          szwSection);

	    
    KLCSKCA_DECL void SS_OnSaving(Storage* pStorage);

/*!
  \brief	Загружает секцию KLPRSS_PRODUCT_CORE/KLPRSS_VERSION_INDEPENDENT/
            KLPRSS_VERSION_INFO и прописывает qwVersion.

  \param	KLPRSS::Storage* pStorage
  \param	AVP_qword qwVersion
  \return	void 
*/
	//void WriteStoreVersion(KLPRSS::Storage* pStorage, AVP_qword qwVersion);    
    //void WriteStoreVersion(KLPAR::Params* pVerData, AVP_qword qwVersion);
    //AVP_qword RetrieveStoreVersion(KLPAR::Params* pVerData);
    //KLCSKCA_DECL AVP_qword RetrieveStoreVersion(KLPRSS::Storage* pStorage);
    //KLCSKCA_DECL AVP_qword IncrementVersion(KLPRSS::Storage* pStorage);

    /*!
      \brief	Loads section KLPRSS_PRODUCT_CORE/KLPRSS_VERSION_INDEPENDENT/
            KLPRSS_VERSION_INFO and writes new ss id

      \param	pStorage
      \param	wstrId
    */
    KLCSKCA_DECL void WriteStoreID(KLPRSS::Storage* pStorage, const std::wstring& wstrId);

    /*!
      \brief	reads new ss id

      \param	pVerData
      \param	wstrId
      \return	false if no id
    */
    KLCSKCA_DECL bool RetrieveStoreID(KLPAR::Params* pVerData, std::wstring& wstrId);

    /*!
      \brief	Loads section KLPRSS_PRODUCT_CORE/KLPRSS_VERSION_INDEPENDENT/
                KLPRSS_VERSION_INFO and reads ss id

      \param	pStorage
      \param	wstrId
      \return	false if no id
    */
    KLCSKCA_DECL bool RetrieveStoreID(KLPRSS::Storage* pStorage, std::wstring& wstrId);

    /*!
      \brief	RemoveInstance удяляет запись о запущенном экземпляре из хранилища настроек

      \param	wstrLocation [in]		- расположение хранилища настроек
      \param	wstrProduct [in]		- название продукиа
      \param	wstrVersion [in]		- версия продукта
      \param	wstrComponent [in]		- имя компонента
      \param	wstrInstanceName [in]	- имя экземпляра
      \return	false, если записи уже не существует
    */
	KLCSKCA_DECL bool RemoveInstance(
				const std::wstring&         wstrLocation,
				const std::wstring&         wstrProduct,
				const std::wstring&         wstrVersion,
				const std::wstring&         wstrComponent,
				const std::wstring&         wstrInstanceName,
                KLPRSS::SettingsStorage*    pBase = NULL);
};

KLCSKCA_DECL void KLPRSS_GetInstalledProducts2(
			KLPRSS::SettingsStorage*						pStorage,
			std::vector<KLPRSS::product_version_t>&	vecProducts);

KLCSKCA_DECL void KLPRSS_GetInstalledComponents2(
					KLPRSS::SettingsStorage*	pStorage,
					const std::wstring&			wstrProductName,
					const std::wstring&			wstrProductVersion,
					std::vector<std::wstring>&	vecComponents);

KLCSKCA_DECL void KLPRSS_ExpandType(const std::wstring& wstrType, KLPAR::Params** ppOutData);

KLCSKCA_DECL void KLPRSS_UnexpandType(KLPAR::Params* pData, std::wstring& wstrType);

KLCSKCA_DECL void KLPRSS_ResolveNames(KLPAR::Params* pInData, KLPAR::Params** ppOutData);

KLCSKCA_DECL std::wstring KLPRSS_TypeToFileSystemPath(const std::wstring& wstrType);

KLCSKCA_DECL void KLPRSS_ResolveValue(
                         const std::wstring&    wstrValue,
                         KLPAR::Params*         pInData,
                         std::wstring&          wstrResult);


/*!
  \brief	Returns (and creates) path to the folder where to store files
            specific for current user

  \return	path
*/
KLCSKCA_DECL std::wstring KLPRSS_MakeUserDataPath();

KLCSKCA_DECL bool KLPRSS_IsSSTypeProductSpecific(const wchar_t* szwType);
KLCSKCA_DECL bool KLPRSS_IsSSTypeGlobal(const wchar_t* szwType);

void KLCSKCA_DECL KLPRSS_GetStoreChangedCached(
        const wchar_t*          szwPath, 
        bool&                   bStoreChanged,
        long                    lTimeout);

/*KLCSAK_END_PRIVATE*/

KLCSKCA_DECL void KLPRSS_GetMandatoryAndDefault(
                            KLPAR::Params*  pInData,
                            KLPAR::Params** ppMandatory,
                            KLPAR::Params** ppDefault);

/*KLCSAK_BEGIN_PRIVATE*/
KLCSKCA_DECL std::wstring KLPRSS_MakeTypeG(const wchar_t* szwTypeName);

KLCSKCA_DECL std::wstring KLPRSS_MakeTypeP(
                                        const wchar_t*      szwTypeName,
                                        const std::wstring& wstrProduct,
                                        const std::wstring& wstrVersion);

KLCSKCA_DECL bool KLPRSS_RemoveSettingsStorage(const std::wstring& wstrLocation, long lTimeout);


/*!
  \brief	KLPRSS_CopySettingsStorage
            \param	wstrExistingSs - file system path for source Ss. It
                    must exist.
            \param	wstrNewSs - file system path for destination Ss
            \param  bFailIfExist - if wstrNewSs already exists and
                    bFailIfExist is true function will throw exception 
                    STDE_EXIST, if wstrNewSs already exists and bFailIfExist
                    is false existing Ss will be overwritten.
*/
KLCSKCA_DECL void KLPRSS_CopySettingsStorage(
                        const std::wstring& wstrExistingSs,
                        const std::wstring& wstrNewSs,
                        bool                bFailIfExist,
                        long                lTimeout);

namespace KLPRSS
{
    /*!
      \brief	Данный колбэк вызывается в ответ на обновление каждого SS.
                Следует как можно скорее возвращать управление из данного колбэка,
                т.к. суммарное время нахождения в колбэке определяет время
                блокировки исходного хранилища. Поэтому все долговременные
                операции следует выполнять асинхронно.

      \param	pContext величина, определяемая пользователем
      \param	nIndex индекс пути ss в массиве pszwDstPaths (см. KLPRSS_DistributePolicy)
      \param	bResult true,если операция прошла успешно
      \param	pError указатель на исключение в случае ошибки
      \exception функция не должна выбрасывать исключения
    */
    typedef void (DistributePolicyCallback)(void* pContext, int nIndex, bool bResult, KLERR::Error* pError);
};

const std::wstring& KLPRSS_GetSettingsStorageLocationI(
                               KLPRSS::SS_TYPE          nType,
                               KLPRSS::SS_OPEN_TYPE     nOpenType);



/*!
  \brief	KLPRSS_DistributePolicy
  \param in pszwSrcPath путь к SS в файловой системе
  \param in lSrcTimeout таймаут доступа к исходному ss в мс
  \param in ppszwSections завершающийся нулём массив указателей на "имена
            секций", подлежащих обработке. "Имя секции" представляет собой
            массив из трёх указателей: имя продукта, версия продукта,
            собственно имя секции.
  \param in pszwDstPaths завершающийся нулём массив указателей на пути ss-
            назначений
  \param in lDstTimeout таймаут доступа к ss назначения в мс
  \param in pContext величина, определяемая пользователем.
  \param in pCallback указатель на функцию. Функция вызывается столько раз,
            сколько ss в pszwDstPaths.
*/
KLCSKCA_DECL void KLPRSS_DistributePolicy(
                            const wchar_t*                      pszwSrcPath,
                            long                                lSrcTimeout,
                            const wchar_t***                    ppszwSections,
                            const wchar_t**                     pszwDstPaths,
                            long                                lDstTimeout,
                            void*                               pContext,
                            KLPRSS::DistributePolicyCallback*   pCallback,
                            bool                                bFailIfSectionNotFound,
                            long                                lSrcPolicyId = -1L,
                            long                                lSrcPolicyGroupId = -1L);



/*!
  \brief	KLPRSS_ResetParentalLocks
  \param in szwSsFsPath - путь к хранилищу в файловой системе
  \param in lSsTimeout - таймаут доступа к хранилищу
  \param in pSections - список секций, поле m_dwFlags структуры 
                    section_name_t д.б. равно 0. Если указатель pSections
                    равен NULL, то производится обработка всех секций.
*/
KLCSKCA_DECL void KLPRSS_ResetParentalLocks(
                            const wchar_t*      szwSsFsPath,
                            long                lSsTimeout,
                            KLPRSS::sections_t* pSections = NULL);



/*!
  \brief	Checks whether nagent is in a roaming mode
            for internal use only, subject to change.

  \return	true if in roaming mode
*/
KLCSKCA_DECL bool KLPRSS_GetRoamingMode();


/*!
  \brief	Sets romaing mode
            for internal use only, subject to change

  \param	bMode true if in roaming mode
*/
KLCSKCA_DECL void KLPRSS_SetRoamingMode(bool bMode);

//! for internal use only, subject to change
KLCSKCA_DECL bool KLPRSS_GetRoamingSsExistence(
                        const KLPRSS::product_version_t& product);

//! for internal use only, subject to change
KLCSKCA_DECL void KLPRSS_ReinitRoamingSsExistence();

//! for internal use only, subject to change
KLCSKCA_DECL void KLPRSS_ReinitRoamingSsExistenceForProduct(
                        const KLPRSS::product_version_t& product);

//! for internal use only, subject to change
KLCSKCA_DECL std::wstring KLPRSS_GetFsPolicyPath(
                    const std::wstring& wstrProduct, 
                    const std::wstring& wstrVersion);

namespace KLPRSS
{

    KLCSKCA_DECL void PrepareForSmartWrite(
                            KLPAR::Params*  pDst,
                            KLPAR::Params*  pDef,
                            KLPAR::Params*  pMnd,
                            KLPAR::Params*  pSrc,
                            KLPAR::Params** ppResult);


    void AdjustTaskParams(
                        const std::wstring&         wstrType,
                        const std::wstring&         wstrProductName,
                        const std::wstring&         wstrProductVersion,
                        const std::wstring&         wstrSsLocation,
                        KLPRSS::SettingsStorage*    pSsProxy,
                        KLPAR::Params*              pTaskParams,
                        KLPAR::Params**             ppResult,
                        long                        lTimeout);

/*!
  \brief	Applies task policy for task params
  \param    wstrType [in] - task type name
  \param    wstrProductName [in] - tasks storage product name
  \param    wstrProductVersion [in] - tasks storage product version
  \param    pTaskParams [in] - task parameters without policy alpplied
  \param    ppResult    [out] - result
  \param    lTimeout    [in] - timeout for ss requests
*/
    void SmartReadTaskParams(
                        const std::wstring& wstrType,
                        const std::wstring& wstrProductName,
                        const std::wstring& wstrProductVersion,
                        KLPAR::Params*      pTaskParams,
                        KLPAR::Params**     ppResult,
                        long                lTimeout = KLSTD_INFINITE);


/*!
  \brief	Writes tasks settings using task policy
  \param    wstrProductName [in] - wstrProductName
  \param    wstrProductVersion [in] - wstrProductVersion
  \param    wstrType [in] - task type name
  \param    wstrTsProduct [in] - tasks storage product name
  \param    wstrTsVersion [in] - tasks storage product version
  \param    wstrTsSection [in] - tasks storage section name
  \param    pTs [in] - 
  \param    pTaskParams [in] - task parameters without policy alpplied
  \param    lTimeout    [in] - timeout for ss requests
*/
    void SmartWriteTaskParams(
                            const std::wstring&         wstrProductName,
                            const std::wstring&         wstrProductVersion,
                            const std::wstring&         wstrType,
                            const std::wstring&         wstrTsProduct,
                            const std::wstring&         wstrTsVersion,
                            const std::wstring&         wstrTsSection,
                            KLPRSS::SettingsStorage*    pTs,
                            KLPAR::Params*              pTaskParams,
                            long                        lTimeout);
};
/*KLCSAK_END_PRIVATE*/
#endif // _KLPRSS_HELPERSI_H_
