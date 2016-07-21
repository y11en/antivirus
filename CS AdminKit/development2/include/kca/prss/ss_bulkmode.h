/*!
 * (C) 2003 Kaspersky Lab 
 * 
 * \file	ss_bulkmode.h
 * \author	Andrew Kazachkov
 * \date	23.10.2003 12:09:37
 * \brief	internal interfaces
 * 
 */

#ifndef __KL_SS_BULKMODE_H__

namespace KLPRSS
{
    struct section_name_t
    {
        int         m_nProduct,
                    m_nVersion,
                    m_nSection;
        AVP_dword   m_dwFlags;
    };

    struct sections_t
    {
        std::vector<section_name_t> m_vecNames;
        std::vector<std::wstring>   m_vecProducts;
        std::vector<std::wstring>   m_vecVersions;
        std::vector<std::wstring>   m_vecSections;
    };

    class KLSTD_NOVTABLE SsBulkMode: public KLSTD::KLBaseQI
    {
    public:
        /*!
            Получить имена всех секций за одну транзакцию
        */
        virtual void EnumAllWSections(sections_t& sections) = 0;

        /*!
            Прочитать несколько секций за одну транзакцию
        */
        virtual void ReadSections(
                const sections_t&   sections,
                KLPAR::ArrayValue** ppData) = 0;

        /*!
            Записать несколько секций за одну транзакцию
        */
        virtual void WriteSections(
                const sections_t&   sections,
                KLPAR::ArrayValue*  pData) = 0;
    };
    

    typedef enum
    {
        BMSS_Update = KLSTD::CF_OPEN_EXISTING,  //same as SettingsStorage::Update()
        BMSS_Add = KLSTD::CF_CREATE_NEW,        //same as SettingsStorage::Add()
        BMSS_Replace = KLSTD::CF_OPEN_ALWAYS,   //same as SettingsStorage::Replace()
        BMSS_Delete = KLSTD::CF_CLEAR,          //same as SettingsStorage::Delete()
        BMSS_Clear = KLSTD::CF_CREATE_ALWAYS    //same as SettingsStorage::Clear()
    }
    SsBulkMode_WriteMode;


    /*!
      \brief	SsBulkMode_FastWriteSection

      \param	pSs         IN pointer to Ss acquired by KLPRSS_CreateSettingStorageDirect*
      \param	szwProduct  IN product name, see kca/prss/settingsstorage.h for limitations
      \param	szwVersion  IN product version, see kca/prss/settingsstorage.h for limitations
      \param	szwSection  IN section name, see kca/prss/settingsstorage.h for limitations
      \param	nMode       IN write mode, one of BMSS_*
      \param	pSection    IN pointer to section to write
    */

    KLCSKCA_DECL void SsBulkMode_FastWriteSection(
                        KLSTD::KLBaseQI*        pSs, 
                        const wchar_t*          szwProduct, 
                        const wchar_t*          szwVersion,
                        const wchar_t*          szwSection,
                        SsBulkMode_WriteMode    nMode,
                        KLPAR::Params*          pSection);
};

#define __KL_SS_BULKMODE_H__

#endif //__KL_SS_BULKMODE_H__
