/*!
 * (C) 2003 Kaspersky Lab 
 * 
 * \file	valuesimpl.h
 * \author	Andrew Kazachkov
 * \date	13.09.2004 10:02:25
 * \brief	
 * 
 */

#ifndef __KLVALUESIMPL_H__
#define __KLVALUESIMPL_H__

namespace KLPAR
{
    void InitValues();
    void DeinitValues();

    extern KLSTD::CAutoPtr<ValuesFactory> g_pFactory;
};

#endif //__KLVALUESIMPL_H__
