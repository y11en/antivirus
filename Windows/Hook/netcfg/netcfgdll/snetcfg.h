//+---------------------------------------------------------------------------
//
//  Microsoft Windows
//  Copyright (C) Microsoft Corporation, 1997.
//
//  File:       S N E T C F G . H
//
//  Contents:   Sample code that demonstrates how to:
//              - find out if a component is installed
//              - install a net component
//              - install an OEM net component
//              - uninstall a net component
//              - enumerate net components
//              - enumerate net adapters using Setup API
//              - enumerate binding paths of a component
//
//  Notes:
//
//  Author:     kumarp 26-March-98
//
//----------------------------------------------------------------------------

#pragma once
#include "defs.h"

enum NetClass
{
    NC_NetAdapter=0,
    NC_NetProtocol,
    NC_NetService,
    NC_NetClient,
    NC_Unknown
};

extern "C" HRESULT NETCFGEXPORT HrInstallNetComponent(IN PCWSTR szComponentId,
                                                      IN enum NetClass nc,
                                                      IN PCWSTR szSrcDir, 
													  IN PWSTR szCatFullPath,
													  IN PWSTR szCatName,
													  IN PCWSTR szwUniqueID);

extern "C" HRESULT NETCFGEXPORT HrUninstallNetComponent(IN PCWSTR szComponentId,
														IN PWSTR szCatFullPath,
														IN PWSTR szCatName,
														IN PCWSTR szwUniqueID,
                                                        IN PCWSTR szServiceName);



