/*!
 * (C) 2003 Kaspersky Lab 
 * 
 * \file	valenum.h
 * \author	Andrew Kazachkov
 * \date	04.12.2003 11:57:51
 * \brief	
 * 
 */
/*KLCSAK_PUBLIC_HEADER*/

#ifndef __KLPAR_VALENUM_H__
#define __KLPAR_VALENUM_H__

#include <std/par/params.h>

namespace KLPAR
{
    class KLSTD_NOVTABLE EnumValuesCallback : public KLSTD::KLBaseQI
    {
    public:
        virtual void OnEnumeratedValue(const wchar_t* szwName, KLPAR::Value*) = 0;
    };

    class KLSTD_NOVTABLE EnumValues : public KLSTD::KLBaseQI
    {
    public:
        virtual void EnumerateContents(EnumValuesCallback* pCallback) = 0;
    };
};

#endif //__KLPAR_VALENUM_H__
