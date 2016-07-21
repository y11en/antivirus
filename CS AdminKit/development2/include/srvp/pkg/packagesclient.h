/*!
 * (C) 2002 Kaspersky Lab 
 * 
 * \file	PackagesClient.h
 * \author	Mikhail Karmazine
 * \date	15:43 20.11.2002
 * \brief	
 * 
 */

#ifndef __PACKAGESCLIENT_H__70BE08BB_4599_4d3e_9BD2_4E06583605B5_
#define __PACKAGESCLIENT_H__70BE08BB_4599_4d3e_9BD2_4E06583605B5_

#include <std/base/kldefs.h>
#include <srvp/pkg/packages.h>
#include <kca/prss/settingsstorage.h>

/*!
  \brief —оздает proxy-объект класса KLPKG::Packages

    \param wstrURL      [in] адрес soap-сервера
    \param ppPackages   [out] ”казатель на объект класса KLPKG::Packages
*/

KLCSSRVP_DECL void KLPKG_CreatePackagesProxy(
                        const wchar_t*      szwAddress,
                        KLPKG::Packages**   pp,
                        const unsigned short* pPorts = NULL,
                        bool            bUseSsl = true);
#endif //__PACKAGESCLIENT_H__70BE08BB_4599_4d3e_9BD2_4E06583605B5_
