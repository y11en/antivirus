/*!
 * (C) 2003 Kaspersky Lab 
 * 
 * \file	del_path.cpp
 * \author	Andrew Kazachkov
 * \date	26.04.2005 10:29:45
 * \brief	
 * 
 */

#include <std/base/klstd.h>
#include <std/err/error.h>
#include <std/err/errlocids.h>
#include <std/err/klerrors.h>

#ifdef _WIN32
    #include <std/win32/klos_win32v50.h>
    #include <std/win32/klos_win32_errors.h>
#else
    #include <boost/filesystem/operations.hpp>
    //#include <boost/filesystem/path.hpp>
    #include <boost/filesystem/exception.hpp>
#endif

#include <avp/klids.h>
//#include <std/stdi.h>

#include <string>
#include <list>

#define KLCS_MODULENAME L"KLSTD"

namespace KLSTD
{
    void LocalizeIoError(
                KLERR::Error*       pError,
                int                 nCode,
                const wchar_t*      szwString3,
                const wchar_t*      szwString4 = NULL,
                const wchar_t*      szwString5 = NULL);
};

#ifdef _WIN32

namespace
{
    typedef std::basic_string<TCHAR> tstring;

    struct delpath_data_t
        :   public KLSTD::KLBaseImpl<KLSTD::KLBase>
    {
        delpath_data_t()
            :   m_hFind(INVALID_HANDLE_VALUE)
        {
            KLSTD_ZEROSTRUCT(m_fd);
        };
        virtual ~delpath_data_t()
        {
            if(m_hFind != INVALID_HANDLE_VALUE)
            {
                FindClose(m_hFind);
                m_hFind = INVALID_HANDLE_VALUE;
            };
        };
        HANDLE          m_hFind;
        tstring         m_strRootPath;
        WIN32_FIND_DATA m_fd;
    private:
        delpath_data_t(const delpath_data_t&);
    };


    typedef KLSTD::CAutoPtr<delpath_data_t>             DelPathDataPtr;
    typedef std::list< KLSTD::KLAdapt<DelPathDataPtr> > lst_delpath_data_t;
    typedef lst_delpath_data_t::iterator                it_lst_delpath_data_t;
    typedef lst_delpath_data_t::const_iterator          cit_lst_delpath_data_t;

    tstring Append(const tstring& strPath, const tstring& strAdd)
    {
        if(strPath.empty())
            return strAdd;
        tstring strResult;
        strResult.reserve(strPath.size() + strAdd.size() +1);
        strResult = strPath;
        TCHAR ch = strPath[strPath.size() - 1];
        if( ch != _T('/') && ch != _T('\\') )
            strResult += _T("\\");
        strResult += strAdd;
        return strResult;
    };
};

namespace KLSTD
{
    bool DeletePath(LPCTSTR szPath)
    {
        szPath = KLSTD::FixNullString(szPath);
        KLERR_TRY
            DWORD dwAttr = GetFileAttributes(szPath);
            if(-1 == dwAttr)
            {
                DWORD dwError = GetLastError();
                if( ERROR_FILE_NOT_FOUND == dwError ||
                    ERROR_PATH_NOT_FOUND == dwError )
                {
                    return false;
                };
                KLSTD_RGCHK(dwError);
            };
            KLSTD_ASSERT_THROW((dwAttr & FILE_ATTRIBUTE_REPARSE_POINT) == 0);
            if(dwAttr & FILE_ATTRIBUTE_READONLY)
            {
                KLSTD_LECHK(SetFileAttributes(
                    szPath, 
                    dwAttr & ~FILE_ATTRIBUTE_READONLY));
            };
            if(!(dwAttr & FILE_ATTRIBUTE_DIRECTORY))
            {
                KLSTD_LECHK(DeleteFile(szPath));
            }
            else
            {
                lst_delpath_data_t lstData;
                {
                    DelPathDataPtr pData;
                    pData.Attach(new delpath_data_t);
                    pData->m_strRootPath = szPath;
                    lstData.push_back(pData);
                };
                while(!lstData.empty())
                {
                    DelPathDataPtr pData = lstData.back();
                    if( INVALID_HANDLE_VALUE ==  pData->m_hFind)
                    {
                        pData->m_hFind = FindFirstFile( 
                                        Append(pData->m_strRootPath, _T("*")).c_str(), 
                                        &pData->m_fd);
                        KLSTD_LECHK(pData->m_hFind != INVALID_HANDLE_VALUE);
                    }
                    else
                    {
                        BOOL bResult = FindNextFile(pData->m_hFind, &pData->m_fd);
                        if(!bResult)
                        {
                            DWORD dwError = GetLastError();
                            if(ERROR_NO_MORE_FILES == dwError)
                            {
                                tstring strPath = pData->m_strRootPath;                        
                                lstData.pop_back();
                                pData = NULL;
                                KLSTD_LECHK(RemoveDirectory(strPath.c_str()));
                            }
                            else
                            {
                                KLSTD_RGCHK(dwError);
                            };
                        };
                    };
                    if( !pData ||
                        _tcscmp(pData->m_fd.cFileName, _T(".")) == 0 || 
                        _tcscmp(pData->m_fd.cFileName, _T("..")) == 0)
                    {
                        continue;
                    };
                    ;
                    const tstring& strFullName = Append(pData->m_strRootPath, pData->m_fd.cFileName);
                    KLSTD_ASSERT_THROW(
                            (pData->m_fd.dwFileAttributes & 
                            FILE_ATTRIBUTE_REPARSE_POINT) == 0);
                    if(pData->m_fd.dwFileAttributes & FILE_ATTRIBUTE_READONLY)
                    {
                        KLSTD_LECHK(SetFileAttributes(
                                    strFullName.c_str(), 
                                    pData->m_fd.dwFileAttributes & ~FILE_ATTRIBUTE_READONLY));
                    };
                    if(pData->m_fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
                    {
                        DelPathDataPtr pData;
                        pData.Attach(new delpath_data_t);
                        pData->m_strRootPath = strFullName;
                        lstData.push_back(pData);
                    }
                    else
                    {
                        KLSTD_LECHK(DeleteFile(strFullName.c_str()));
                    };
                };
            };
        KLERR_CATCH(pError)
            KLERR_SAY_FAILURE(3, pError);
            LocalizeIoError(pError, STDEL_DIR_DELETE_ERROR, KLSTD_T2CW2(szPath));
            KLERR_RETHROW();// bug fixed !!!
        KLERR_ENDTRY
        return true;
    };
};
#endif // _WIN32

KLCSC_DECL bool KLSTD_DeletePath(const wchar_t* szwPath)
{
    KLSTD_CHK(szPath, szwPath && szwPath[0]);

    KLSTD_TRACE1(1, L"KLSTD_DeletePath('%ls')\n", szwPath);
#ifdef _WIN32
    return KLSTD::DeletePath((LPCTSTR)KLSTD_W2CT2(szwPath));
#else
    bool bResult = true;    
    KLERR_TRY
        try
        {
            KLSTD_W2CA2 astrPath(szwPath);
            if(boost::filesystem::exists((const char*)astrPath))
                boost::filesystem::remove_all((const char*)astrPath);
            else
                bResult = false;
        }
        catch(const boost::filesystem::filesystem_error& error)
        {
            if(error.native_error())
                KLSTD_THROW_LASTERROR_CODE2(error.native_error());
            else
                throw;
        };
    KLERR_CATCH(pError)
        KLERR_SAY_FAILURE(3, pError);
        LocalizeIoError(pError, KLSTD::STDEL_DIR_DELETE_ERROR, szwPath);
        KLERR_RETHROW();
    KLERR_ENDTRY
    return bResult;
#endif
};

