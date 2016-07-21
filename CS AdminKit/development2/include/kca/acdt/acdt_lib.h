#ifndef __ACDT_LIB_H__
#define __ACDT_LIB_H__

#include <std/base/klstd.h>
#include <std/par/params.h>
#include <std/par/par_conv.h>
#include <kca/acdt/acdt_const.h>

namespace KLACDTAPI
{
    struct prodinfo_t
    {
        const wchar_t*  m_szwID;
        const wchar_t*  m_szwProduct;
        const wchar_t*  m_szwVersion;
        const wchar_t*  m_szwUpgradeCode;
        const wchar_t*  m_szwDisplayName;
        const wchar_t*  m_szwServiceName;
    };

    struct full_prdinf_t
        :   prodinfo_t
    {
        std::wstring    m_wstrProductCode;
    };

    full_prdinf_t FindProduct(const wchar_t* szwProduct);

    void DecodeFilePath(const wchar_t* szwFile, std::wstring* pwstrDN, AVP_qword* pqwSize);

    std::wstring DecodeFilePath2(const wchar_t* szwFile);

    std::wstring EncodeFilePath(const wchar_t* szwFile, const wchar_t* szwDN, AVP_qword qwSize);

    std::wstring EncodeFilePath2(const wchar_t* szwFilePath);
    
    void PackCommonData(const wchar_t* szwDstFile, bool bCompress);

    void CompressFolder(
        const wchar_t*          szwFsFolder,
        const wchar_t*          szwFolderInCab,
        const wchar_t*          szwDstFile, 
        bool                    bRecursive,
        bool                    bCompress);

    void CompressFile(
        const wchar_t*          szwFsFile,
        const wchar_t*          szwFolderInCab,
        const wchar_t*          szwDstFile, 
        bool                    bCompress);


    void DecompressFolder(
        const wchar_t*          szwCabFile,
        const wchar_t*          szwDstFsFolder);

    /*!
      \brief	Returns product info

      \param	    pData
      \param	    szwProduct
      \return	    product info
      \exception    throws exception if not found and bExceptIfNotFound is true
    */
    KLPAR::ParamsPtr GetProductInfo(
                        KLPAR::ParamsPtr    pData, 
                        const wchar_t*      szwProduct, 
                        bool                bExceptIfNotFound = true);


    /*!
      \brief	Returns product info

      \param	pData
      \param	szwProduct
      \param	szwValue
      \return	Value
      \exception    returns NULL if not found

        Example: 
            std::wstring wstrTraceFile = KLPAR::ParVal(
                        GetProductVal(pData, wstrProduct, c_szwStateTraceFile), 
                        L"");
    */
    KLSTD::CAutoPtr<KLPAR::Value> GetProductVal(
                        KLPAR::ParamsPtr    pData, 
                        const wchar_t*      szwProduct, 
                        const wchar_t*      szwValue,
                        bool                bExceptIfNotFound = true);

    std::wstring GetSavedEventLog(
                        KLPAR::ParamsPtr    pData, 
                        const wchar_t*      szwEventLog, 
                        bool                bExceptIfNotFound = true);


    long GetNumber(const wchar_t* szwString);

    /*!
      \brief	Make file name suitable for remote usage

      \param	wstrName 
      \return	
    */
    std::wstring MakeRemoteFileName(const wchar_t* szwName);

    /*!
      \brief	GetProductsState

      \return	KLPAR::ParamsPtr 
    */
    KLPAR::ParamsPtr GetProductsState();

    /*
    void GetProductPars(
            const wchar_t*              szwProduct, 
            std::wstring*               pwstrDisplayName,
            std::wstring*               pwstrBuild,
            std::wstring*               pwstrVersion,
            bool*                       pbIsRunning,
            std::wstring*               pwstrTraceFile,
            int*                        pnTraceLevel,
            std::wstring*               pwstrDiagLog,
            std::wstring*               pwstrDiagTrace,
            AVP_dword*                  pdwProps,
            int*                        pnTraceMin,
            int*                        pnTraceMax);
    */
    /*!
      \brief	SetTracingLevel

      \param	const wchar_t* szwProduct
      \param	int nLevel
      \return	void 
    */
    void SetTracingLevel(const wchar_t* szwProduct, int nLevel);

    /*!
      \brief	SendProductAction

      \param	const wchar_t* szwProduct
      \param	KLACDT_PRODUCT_ACTION nCode
      \return	void 
    */
    void SendProductAction(const wchar_t* szwProduct, KLACDT_PRODUCT_ACTION nCode, long Timeout);

    /*!
      \brief	ExecuteNagChk

      \return	void 
    */
    void ExecuteDiagChk(const wchar_t* szwProduct, long Timeout);

    void SaveEventLog(const wchar_t* szwEL, const wchar_t* szwFileName);

    void DoRunProcess(
                const wchar_t*  szwCommandLine, 
                unsigned&       nExitCode,
                const wchar_t*  szwCurrentDir,            
                long            lTimeout,
                const void*     lpEnvironment = NULL,
                const wchar_t*  szwStdOut = NULL);

    std::wstring Int2Str(int x);

    int Str2Int(const wchar_t* szwX);
    
    std::wstring DoExpandEnvStrings(const wchar_t* szwX);

    std::wstring GetNagentInstallFolder();
};

#endif //__ACDT_LIB_H__
