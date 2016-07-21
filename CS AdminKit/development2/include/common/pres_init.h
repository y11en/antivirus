/*!
 * (C) 2003 Kaspersky Lab 
 * 
 * \file	pres_init.h
 * \author	Andrew Kazachkov
 * \date	25.09.2003 16:09:48
 * \brief	
 * 
 */

#ifndef __KLPRES_INIT_H__
#define __KLPRES_INIT_H__

namespace KLPRES
{
    void InitLocalPresServer(
        const KLPRCI::ComponentId&  idComponent,
        bool                        bActiveMode=false);

    void DeinitLocalPresServer(
        const std::wstring& wstrProduct,
        const std::wstring& wstrVersion,
        bool                bWaitEventQueueEmpty);
};
#endif //__KLPRES_INIT_H__