// AVP Prague stamp begin( Interface header,  )
// -------- ANSI C++ Code Generator 1.0 --------
// -------- Friday,  30 March 2007,  14:52 --------
// -------------------------------------------
// Copyright © Kaspersky Lab 1996-2006.
// -------------------------------------------
// Project     -- Updater
// Sub project --
// Purpose     -- interface implementation for products 7 line
// Author      -- Voronkov
// File Name   -- updaterconfigurator.cpp
// -------------------------------------------
// AVP Prague stamp end
// AVP Prague stamp begin( Interface version,  )
#define UpdaterConfigurator_VERSION ((tVERSION)1)
// AVP Prague stamp end
// 4786 'identifier' : identifier was truncated to 'number' characters in the debug information
#include "../comdefs.h"
// AVP Prague stamp begin( Includes for interface,  )
#include "updaterconfigurator.h"
// AVP Prague stamp end
// AVP Prague stamp begin( Global property variable declaration,  )
// Interface "UpdaterConfigurator". Static(shared) property table variables
// AVP Prague stamp end
// AVP Prague stamp begin( Internal (kernel called) interface method implementation, ObjectInitDone )
tERROR UpdaterConfiguratorImplementation::ObjectInitDone()
{
    memset(&m_updateDate, 0, sizeof(tDATETIME));
    m_updateOnlyBlackList = cFALSE;
    m_updateResultNeedReboot = cFALSE;
    m_updateScheduleTimeoutOnSuccess = 0;
    m_updateScheduleTimeoutOnFailure = 0;
    m_rollbackAvailable = cFALSE;
    m_retranslateOnlyListedComponents = cTRUE;
    m_rollbackMode = cFALSE;
    m_interactionWithUserEnabled = cTRUE;
    return errOK;
}
// AVP Prague stamp end
// AVP Prague stamp begin( Property method implementation, getUpdateDate )
// Interface "UpdaterConfigurator". Method "Get" for property(s):
//  -- updateDate
tERROR UpdaterConfiguratorImplementation::getUpdateDate( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size )
{
    *out_size = sizeof(tDATETIME);
    if(!buffer || !size)
        return errOK;

    if(size < sizeof(tDATETIME))
        return errBUFFER_TOO_SMALL;

    memcpy(buffer, &m_updateDate, sizeof(tDATETIME));
    return errOK;
}
// AVP Prague stamp end
// AVP Prague stamp begin( Property method implementation, setUpdateDate )
// Interface "UpdaterConfigurator". Method "Set" for property(s):
//  -- updateDate
tERROR UpdaterConfiguratorImplementation::setUpdateDate( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size )
{
    *out_size = sizeof(tDATETIME);
    if(size < sizeof(tDATETIME))
        return errBUFFER_TOO_SMALL;

    memcpy(&m_updateDate, buffer, sizeof(tDATETIME));
    return errOK;
}
// AVP Prague stamp end
// AVP Prague stamp begin( Property method implementation, getUpdateOnlyBlackList )
// Interface "UpdaterConfigurator". Method "Get" for property(s):
//  -- updateOnlyBlackList
tERROR UpdaterConfiguratorImplementation::getUpdateOnlyBlackList( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size )
{
    *out_size = sizeof(tBOOL);
    if(!buffer || !size)
        return errOK;

    if(size < sizeof(tBOOL))
        return errBUFFER_TOO_SMALL;

    *(reinterpret_cast<tBOOL *>(buffer)) = m_updateOnlyBlackList;
    return errOK;
}
// AVP Prague stamp end
// AVP Prague stamp begin( Property method implementation, setUpdateOnlyBlackList )
// Interface "UpdaterConfigurator". Method "Set" for property(s):
//  -- updateOnlyBlackList
tERROR UpdaterConfiguratorImplementation::setUpdateOnlyBlackList( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size )
{
    *out_size = sizeof(tBOOL);
    if(size < sizeof(tBOOL))
        return errBUFFER_TOO_SMALL;

    m_updateOnlyBlackList = *(reinterpret_cast<tBOOL *>(buffer));
    return errOK;
}
// AVP Prague stamp end
// AVP Prague stamp begin( Property method implementation, getComponentsToUpdate )
// Interface "UpdaterConfigurator". Method "Get" for property(s):
//  -- componentsToUpdate
tERROR UpdaterConfiguratorImplementation::getComponentsToUpdate( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size )
{
    *out_size = m_componentsToUpdate.size();
    if(!buffer || !size)
        return errOK;

    if(!m_componentsToUpdate.empty())
    {
        if(size < m_componentsToUpdate.size())
            return errBUFFER_TOO_SMALL;
        memcpy_s(buffer, size, &m_componentsToUpdate[0], m_componentsToUpdate.size());
    }
    return errOK;
}
// AVP Prague stamp end
// AVP Prague stamp begin( Property method implementation, setComponentsToUpdate )
// Interface "UpdaterConfigurator". Method "Set" for property(s):
//  -- componentsToUpdate
tERROR UpdaterConfiguratorImplementation::setComponentsToUpdate( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size )
{
    *out_size = size;
    m_componentsToUpdate.assign(buffer, buffer + size);
    return errOK;
}
// AVP Prague stamp end
// AVP Prague stamp begin( Property method implementation, getUpdateResultFixNames )
// Interface "UpdaterConfigurator". Method "Get" for property(s):
//  -- updateResultFixNames
tERROR UpdaterConfiguratorImplementation::getUpdateResultFixNames( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size )
{
    *out_size = m_updateResultFixNames.size();
    if(!buffer || !size)
        return errOK;

    if(!m_updateResultFixNames.empty())
    {
        if(size < m_updateResultFixNames.size())
            return errBUFFER_TOO_SMALL;
        memcpy_s(buffer, size, &m_updateResultFixNames[0], m_updateResultFixNames.size());
    }
    return errOK;
}
// AVP Prague stamp end
// AVP Prague stamp begin( Property method implementation, setUpdateResultFixNames )
// Interface "UpdaterConfigurator". Method "Set" for property(s):
//  -- updateResultFixNames
tERROR UpdaterConfiguratorImplementation::setUpdateResultFixNames( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size )
{
    *out_size = size;
    m_updateResultFixNames.assign(buffer, buffer + size);
    return errOK;
}
// AVP Prague stamp end
// AVP Prague stamp begin( Property method implementation, getUpdateResultNeedReboot )
// Interface "UpdaterConfigurator". Method "Get" for property(s):
//  -- updateResultNeedReboot
tERROR UpdaterConfiguratorImplementation::getUpdateResultNeedReboot( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size )
{
    *out_size = sizeof(tBOOL);
    if(!buffer || !size)
        return errOK;

    if(size < sizeof(tBOOL))
        return errBUFFER_TOO_SMALL;

    *(reinterpret_cast<tBOOL *>(buffer)) = m_updateResultNeedReboot;
    return errOK;
}
// AVP Prague stamp end
// AVP Prague stamp begin( Property method implementation, setUpdateResultNeedReboot )
// Interface "UpdaterConfigurator". Method "Set" for property(s):
//  -- updateResultNeedReboot
tERROR UpdaterConfiguratorImplementation::setUpdateResultNeedReboot( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size )
{
    *out_size = sizeof(tBOOL);
    if(size < sizeof(tBOOL))
        return errBUFFER_TOO_SMALL;

    m_updateResultNeedReboot = *(reinterpret_cast<tBOOL *>(buffer));
    return errOK;
}
// AVP Prague stamp end
// AVP Prague stamp begin( Property method implementation, getUpdateScheduleTimeoutOnSuccess )
// Interface "UpdaterConfigurator". Method "Get" for property(s):
//  -- updateScheduleTimeoutOnSuccess
tERROR UpdaterConfiguratorImplementation::getUpdateScheduleTimeoutOnSuccess( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size )
{
    *out_size = sizeof(tDWORD);
    if(!buffer || !size)
        return errOK;

    if(size < sizeof(tDWORD))
        return errBUFFER_TOO_SMALL;

    *(reinterpret_cast<tDWORD *>(buffer)) = m_updateScheduleTimeoutOnSuccess;
    return errOK;
}
// AVP Prague stamp end
// AVP Prague stamp begin( Property method implementation, setUpdateScheduleTimeoutOnSuccess )
// Interface "UpdaterConfigurator". Method "Set" for property(s):
//  -- updateScheduleTimeoutOnSuccess
tERROR UpdaterConfiguratorImplementation::setUpdateScheduleTimeoutOnSuccess( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size )
{
    *out_size = sizeof(tDWORD);
    if(size < sizeof(tDWORD))
        return errBUFFER_TOO_SMALL;

    m_updateScheduleTimeoutOnSuccess = *(reinterpret_cast<tDWORD *>(buffer));
    return errOK;
}
// AVP Prague stamp end
// AVP Prague stamp begin( Property method implementation, getUpdateScheduleTimeoutOnFailure )
// Interface "UpdaterConfigurator". Method "Get" for property(s):
//  -- updateScheduleTimeoutOnFailure
tERROR UpdaterConfiguratorImplementation::getUpdateScheduleTimeoutOnFailure( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size )
{
    *out_size = sizeof(tBOOL);
    if(!buffer || !size)
        return errOK;

    if(size < sizeof(tBOOL))
        return errBUFFER_TOO_SMALL;

    *(reinterpret_cast<tBOOL *>(buffer)) = m_updateScheduleTimeoutOnFailure;
    return errOK;
}
// AVP Prague stamp end
// AVP Prague stamp begin( Property method implementation, setUpdateScheduleTimeoutOnFailure )
// Interface "UpdaterConfigurator". Method "Set" for property(s):
//  -- updateScheduleTimeoutOnFailure
tERROR UpdaterConfiguratorImplementation::setUpdateScheduleTimeoutOnFailure( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size )
{
    *out_size = sizeof(tBOOL);
    if(size < sizeof(tBOOL))
        return errBUFFER_TOO_SMALL;

    m_updateScheduleTimeoutOnFailure = *(reinterpret_cast<tBOOL *>(buffer));
    return errOK;
}
// AVP Prague stamp end
// AVP Prague stamp begin( Property method implementation, getRollbackAvailability )
// Interface "UpdaterConfigurator". Method "Get" for property(s):
//  -- rollbackAvailable
tERROR UpdaterConfiguratorImplementation::getRollbackAvailability( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size )
{
    *out_size = sizeof(tBOOL);
    if(!buffer || !size)
        return errOK;

    if(size < sizeof(tBOOL))
        return errBUFFER_TOO_SMALL;

    *(reinterpret_cast<tBOOL *>(buffer)) = m_rollbackAvailable;
    return errOK;
}
// AVP Prague stamp end
// AVP Prague stamp begin( Property method implementation, setRollbackAvailability )
// Interface "UpdaterConfigurator". Method "Set" for property(s):
//  -- rollbackAvailable
tERROR UpdaterConfiguratorImplementation::setRollbackAvailability( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size )
{
    *out_size = sizeof(tBOOL);
    if(size < sizeof(tBOOL))
        return errBUFFER_TOO_SMALL;

    m_rollbackAvailable = *(reinterpret_cast<tBOOL *>(buffer));
    return errOK;
}
// AVP Prague stamp end
// AVP Prague stamp begin( Property method implementation, getRetranslateOnlyListedComponents )
// Interface "UpdaterConfigurator". Method "Get" for property(s):
//  -- retranslateOnlyListedComponents
tERROR UpdaterConfiguratorImplementation::getRetranslateOnlyListedComponents( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size )
{
    *out_size = sizeof(tBOOL);
    if(!buffer || !size)
        return errOK;

    if(size < sizeof(tBOOL))
        return errBUFFER_TOO_SMALL;

    *(reinterpret_cast<tBOOL *>(buffer)) = m_retranslateOnlyListedComponents;
    return errOK;
}
// AVP Prague stamp end
// AVP Prague stamp begin( Property method implementation, setRetranslateOnlyListedComponents )
// Interface "UpdaterConfigurator". Method "Set" for property(s):
//  -- retranslateOnlyListedComponents
tERROR UpdaterConfiguratorImplementation::setRetranslateOnlyListedComponents( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size )
{
    *out_size = sizeof(tBOOL);
    if(size < sizeof(tBOOL))
        return errBUFFER_TOO_SMALL;

    m_retranslateOnlyListedComponents = *(reinterpret_cast<tBOOL *>(buffer));
    return errOK;
}
// AVP Prague stamp end
// AVP Prague stamp begin( Property method implementation, getComponentsToRetranslate )
// Interface "UpdaterConfigurator". Method "Get" for property(s):
//  -- componentsToRetranslate
tERROR UpdaterConfiguratorImplementation::getComponentsToRetranslate( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size )
{
    *out_size = m_componentsToRetranslate.size();
    if(!buffer || !size)
        return errOK;

    if(!m_componentsToRetranslate.empty())
    {
        if(size < m_componentsToRetranslate.size())
            return errBUFFER_TOO_SMALL;
        memcpy_s(buffer, size, &m_componentsToRetranslate[0], m_componentsToRetranslate.size());
    }
    return errOK;
}
// AVP Prague stamp end
// AVP Prague stamp begin( Property method implementation, setComponentsToRetranslate )
// Interface "UpdaterConfigurator". Method "Set" for property(s):
//  -- componentsToRetranslate
tERROR UpdaterConfiguratorImplementation::setComponentsToRetranslate( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size )
{
    *out_size = size;
    m_componentsToRetranslate.assign(buffer, buffer + size);
    return errOK;
}
// AVP Prague stamp end
// AVP Prague stamp begin( Property method implementation, getRollbackMode )
// Interface "UpdaterConfigurator". Method "Get" for property(s):
//  -- rollbackMode
tERROR UpdaterConfiguratorImplementation::getRollbackMode( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size )
{
    *out_size = sizeof(tBOOL);
    if(!buffer || !size)
        return errOK;

    if(size < sizeof(tBOOL))
        return errBUFFER_TOO_SMALL;

    *(reinterpret_cast<tBOOL *>(buffer)) = m_rollbackMode;
    return errOK;
}
// AVP Prague stamp end
// AVP Prague stamp begin( Property method implementation, setRollbackMode )
// Interface "UpdaterConfigurator". Method "Set" for property(s):
//  -- rollbackMode
tERROR UpdaterConfiguratorImplementation::setRollbackMode( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size )
{
    *out_size = sizeof(tBOOL);
    if(size < sizeof(tBOOL))
        return errBUFFER_TOO_SMALL;

    m_rollbackMode = *(reinterpret_cast<tBOOL *>(buffer));
    return errOK;
}
// AVP Prague stamp end
// AVP Prague stamp begin( Property method implementation, getInteractionWithUserEnabled )
// Interface "UpdaterConfigurator". Method "Get" for property(s):
//  -- interactionWithUserEnabled
tERROR UpdaterConfiguratorImplementation::getInteractionWithUserEnabled( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size )
{
    *out_size = sizeof(tBOOL);
    if(!buffer || !size)
        return errOK;

    if(size < sizeof(tBOOL))
        return errBUFFER_TOO_SMALL;

    *(reinterpret_cast<tBOOL *>(buffer)) = m_interactionWithUserEnabled;
    return errOK;
}
// AVP Prague stamp end
// AVP Prague stamp begin( Property method implementation, setInteractionWithUserEnabled )
// Interface "UpdaterConfigurator". Method "Set" for property(s):
//  -- interactionWithUserEnabled
tERROR UpdaterConfiguratorImplementation::setInteractionWithUserEnabled( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size )
{
    *out_size = sizeof(tBOOL);
    if(size < sizeof(tBOOL))
        return errBUFFER_TOO_SMALL;

    m_interactionWithUserEnabled = *(reinterpret_cast<tBOOL *>(buffer));
    return errOK;
}
// AVP Prague stamp end
// AVP Prague stamp begin( C++ class regitration,  )
// Interface "UpdaterConfigurator". Register function
tERROR UpdaterConfiguratorImplementation::Register( hROOT root ) 
{
    tERROR error;
// AVP Prague stamp end
// AVP Prague stamp begin( Property table,  )
mpPROPERTY_TABLE(UpdaterConfigurator_PropTable)
    mpLOCAL_PROPERTY_FN( pgupdateDate, FN_CUST(getUpdateDate), FN_CUST(setUpdateDate) )
    mpLOCAL_PROPERTY_FN( pgupdateOnlyBlackList, FN_CUST(getUpdateOnlyBlackList), FN_CUST(setUpdateOnlyBlackList) )
    mpLOCAL_PROPERTY_FN( pgcomponentsToUpdate, FN_CUST(getComponentsToUpdate), FN_CUST(setComponentsToUpdate) )
    mpLOCAL_PROPERTY_FN( pgupdateResultFixNames, FN_CUST(getUpdateResultFixNames), FN_CUST(setUpdateResultFixNames) )
    mpLOCAL_PROPERTY_FN( pgupdateResultNeedReboot, FN_CUST(getUpdateResultNeedReboot), FN_CUST(setUpdateResultNeedReboot) )
    mpLOCAL_PROPERTY_FN( pgupdateScheduleTimeoutOnSuccess, FN_CUST(getUpdateScheduleTimeoutOnSuccess), FN_CUST(setUpdateScheduleTimeoutOnSuccess) )
    mpLOCAL_PROPERTY_FN( pgupdateScheduleTimeoutOnFailure, FN_CUST(getUpdateScheduleTimeoutOnFailure), FN_CUST(setUpdateScheduleTimeoutOnFailure) )
    mpLOCAL_PROPERTY_FN( pgrollbackAvailable, FN_CUST(getRollbackAvailability), FN_CUST(setRollbackAvailability) )
    mpLOCAL_PROPERTY_FN( pgretranslateOnlyListedComponents, FN_CUST(getRetranslateOnlyListedComponents), FN_CUST(setRetranslateOnlyListedComponents) )
    mpLOCAL_PROPERTY_FN( pgcomponentsToRetranslate, FN_CUST(getComponentsToRetranslate), FN_CUST(setComponentsToRetranslate) )
    mpLOCAL_PROPERTY_FN( pgrollbackMode, FN_CUST(getRollbackMode), FN_CUST(setRollbackMode) )
    mpLOCAL_PROPERTY_FN( pginteractionWithUserEnabled, FN_CUST(getInteractionWithUserEnabled), FN_CUST(setInteractionWithUserEnabled) )
mpPROPERTY_TABLE_END(UpdaterConfigurator_PropTable)
// AVP Prague stamp end
// AVP Prague stamp begin( Internal method table,  )
mINTERNAL_TABLE_BEGIN(UpdaterConfigurator)
    mINTERNAL_METHOD(ObjectInitDone)
mINTERNAL_TABLE_END(UpdaterConfigurator)
// AVP Prague stamp end
// AVP Prague stamp begin( External method table,  )
mEXTERNAL_TABLE_BEGIN(UpdaterConfigurator)
mEXTERNAL_TABLE_END(UpdaterConfigurator)
// AVP Prague stamp end
// AVP Prague stamp begin( Registration call,  )
    error = CALL_Root_RegisterIFace(
        root,                                   // root object
        IID_UPDATERCONFIGURATOR,                // interface identifier
        PID_UPDATERCONFIGURATORIMPLEMENTATION,  // plugin identifier
        0x00000000,                             // subtype identifier
        UpdaterConfigurator_VERSION,            // interface version
        VID_VORONKOV,                           // interface developer
        &i_UpdaterConfigurator_vtbl,            // internal(kernel called) function table
        (sizeof(i_UpdaterConfigurator_vtbl)/sizeof(tPTR)),// internal function table size
        NULL,                                   // external function table
        0,                                      // external function table size
        UpdaterConfigurator_PropTable,          // property table
        mPROPERTY_TABLE_SIZE(UpdaterConfigurator_PropTable),// property table size
        sizeof(UpdaterConfiguratorImplementation)-sizeof(cObjImpl),// memory size
        0                                       // interface flags
    );

    if(PR_SUCC(error))
    {
        PR_TRACE((root, prtDANGER, "Plugin registered: IID_UPDATERCONFIGURATOR"));
    }
    else
        PR_TRACE((root, prtDANGER, "Failed to register plugin IID_UPDATERCONFIGURATOR [%terr]", error));
// AVP Prague stamp end
// AVP Prague stamp begin( C++ class regitration end,  )
    return error;
}

tERROR pr_call UpdaterConfigurator_Register( hROOT root ) { return UpdaterConfiguratorImplementation::Register(root); }
// AVP Prague stamp end
// AVP Prague stamp begin( Caution !!!,  )
// You have to implement propetry: pgINTERFACE_VERSION
// You have to implement propetry: pgINTERFACE_COMMENT
// AVP Prague stamp end
