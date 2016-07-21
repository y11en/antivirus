/*!
 * (C) 2003 Kaspersky Lab 
 * 
 * \file	locevents.h
 * \author	Andrew Kazachkov
 * \date	09.11.2004 17:49:29
 * \brief	
 * 
 */

#ifndef __KL_LOCEVENTS_H__
#define __KL_LOCEVENTS_H__

#include <std/base/klbase.h>
#include <std/par/params.h>
#include <common/fmtmsg.h>

namespace KLLOC
{
    KLCSKCA_DECL void* GetLocLibrary();
    
    KLCSKCA_DECL void LoadLocLibrary(const std::wstring& wstrPath);
    

    KLCSKCA_DECL void UnloadLocLibrary();
    
    KLCSKCA_DECL std::wstring LoadLocString(int id, const wchar_t* szwDefault);
    
    KLCSKCA_DECL std::wstring FormatLocMessage(
                            int             idTemplate, 
                            const wchar_t*  szwDefTemplate,
                            const wchar_t** pArgs);
    
    KLCSKCA_DECL std::wstring FormatLocMessageV(
                    int             idTemplate, 
                    const wchar_t*  szwDefTemplate,
                    const wchar_t*  pArg1,
                    const wchar_t*  pArg2 = NULL,
                    const wchar_t*  pArg3 = NULL,
                    const wchar_t*  pArg4 = NULL,
                    const wchar_t*  pArg5 = NULL,
                    const wchar_t*  pArg6 = NULL,
                    const wchar_t*  pArg7 = NULL,
                    const wchar_t*  pArg8 = NULL,
                    const wchar_t*  pArg9 = NULL,
                    const wchar_t*  pArg10 = NULL);

    KLCSKCA_DECL void PrepareEventBody(
                    const wchar_t*          szwEventName,
                    int                     idEventDisplayName,
                    const wchar_t*          szwDefEventDisplayName,
                    int                     idFormatString,
                    const wchar_t*          szwDefFormatString,
                    long                    lSeverity,
                    KLPAR::Params**         ppEventBody,
                    const wchar_t*          szwPar1 = NULL,
                    const wchar_t*          szwPar2 = NULL,
                    const wchar_t*          szwPar3 = NULL,
                    const wchar_t*          szwPar4 = NULL,
                    const wchar_t*          szwPar5 = NULL,
                    const wchar_t*          szwPar6 = NULL);
};

#endif //__KL_LOCEVENTS_H__
