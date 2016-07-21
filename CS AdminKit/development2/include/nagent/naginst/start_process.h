/*!
 * (C) 2003 Kaspersky Lab 
 * 
 * \file	start_process.h
 * \author	Andrew Kazachkov
 * \date	22.08.2003 13:15:45
 * \brief	
 * 
 */


#ifndef __KLNAG_START_PROCESS_H__
#define __KLNAG_START_PROCESS_H__

namespace KLNAG
{
    void StartProcess(
                    const wchar_t* szwCommandLine,
                    const wchar_t* szwCurrentDirectory,
					bool bCkeckSignature = true);

    bool StartService_(const wchar_t*  szwServiceName, bool bCheckSign);

    bool StopProcess(int pid);

    bool StopService(const wchar_t*  szwServiceName);
    
    void SetAutoStart_RegRun(
                            bool bAutoStart,
                            const wchar_t* szwVarName,
                            const wchar_t* szwCmdLine);

    void SetAutoStart_Service(
                            bool bAutoStart,
                            const wchar_t* szwServiceName);
};

#endif //__KLNAG_START_PROCESS_H__
