#ifndef __NAG_UTILS_H__
#define __NAG_UTILS_H__


namespace KLNAGUT
{
    const long c_lDefIoTimeout = 30*1000;
};

#ifdef __unix

#include <std/base/klstd.h>
#include <signal.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/file.h>
#include <sys/stat.h>
#include <fcntl.h>

namespace KLNAGUT
{
    const wchar_t c_szwReloadFlagFileName[ ] = L"klnagent.reload";

    const wchar_t c_szwPidFile[ ] = L"/var/run/klnagent.pid";

    const wchar_t c_szwLocalPidFileName[ ] = L"klnagent.pid";

    std::wstring GetPidFileName();

    class CPidFile
    {
    public:
        CPidFile();

        ~CPidFile();

        //! creates file locks it exclusively and writes pid here
        void Create(const wchar_t* szwPath);    

        //! closes and deletes file
        void Destroy();

        //! opens file, checks whether it is locked and reads it
        pid_t Read(const wchar_t* szwPath);
    protected:
        int             m_nFile;
        std::wstring    m_wstrFile;
        bool            m_bMustDelete;
    };
    
    /*!
      \brief	reloads nagent

      \return	false if nagent is not running
    */
    bool Nagent_ReloadNagent();

    /*!
      \brief	Reloads connectors

      \return	false if nagent is not running
    */
    bool Nagent_ReloadConnectors();

    /*!
      \brief	Stop nagent daemon
    */
    void Nagent_StopDaemon();
};
#endif

#endif //__NAG_UTILS_H__
