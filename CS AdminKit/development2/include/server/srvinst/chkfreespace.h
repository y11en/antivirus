/*!
 * (C) 2003 Kaspersky Lab 
 * 
 * \file	chkfreespace.h
 * \author	Andrew Kazachkov
 * \date	14.04.2004 14:30:11
 * \brief	
 * 
 */

#ifndef __CHKFREESPACE_H__
#define __CHKFREESPACE_H__

namespace KLSRV
{

    struct path_to_check_t
    {
        path_to_check_t()
            :   bIsDir(false)
        {;};
        path_to_check_t(const std::wstring& name, bool is_dir)
            :   wstrPath(name)
            ,   bIsDir(is_dir)
        {;};
        path_to_check_t(const path_to_check_t& x)
            :   wstrPath(x.wstrPath)
            ,   bIsDir(x.bIsDir)
        {;};

        std::wstring    wstrPath;
        bool            bIsDir;
    };

    class KLSTD_NOVTABLE DiskAndDbSpaceChecker: public KLSTD::KLBase
    {
    public:
        virtual void Create(
                    KLSRV::SrvData*     pData,
                    const std::vector<KLSRV::path_to_check_t>&
                                        vecPaths) = 0;

        KLSTD_NOTHROW virtual void Close()  throw() = 0;

        KLSTD_NOTHROW virtual void ReportSharedFolderUnavailable(
                                            KLERR::Error*   pError,
                                            const wchar_t*  szwFolderName,
                                            const wchar_t*  szwPath) throw() = 0;

        KLSTD_NOTHROW virtual void ReportSQLServerUnavailable(
                                            KLERR::Error*   pError) throw() = 0;

        KLSTD_NOTHROW virtual void ReportDataBaseIsFull(
                                            KLERR::Error*   pError,    
                                            const wchar_t*  szwInstance = NULL,
                                            const wchar_t*  szwDatabase = NULL) throw() = 0;

    };

};

void KLSRV_CreateDiskAndDbSpaceChecker(
            KLSRV::ServerHelpers*                       pServerHelpers,
            KLSTD::Reporter*                            pReporter,
            const std::wstring&                         wstrDbName,
            const std::wstring&                         wstrInstanceName,
            KLSRV::DiskAndDbSpaceChecker**              ppDiskAndDbSpaceChecker);

#endif //__CHKFREESPACE_H__