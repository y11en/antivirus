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
// AVP Prague stamp begin( Header ifndef,  )
#if !defined( __updaterconfigurator_cpp__00e067d2_191d_4457_aa51_06600bac9e79 )
#define __updaterconfigurator_cpp__00e067d2_191d_4457_aa51_06600bac9e79
// AVP Prague stamp end
// AVP Prague stamp begin( Header includes,  )
#include <Prague/prague.h>
#include <Prague/iface/i_root.h>
#include <Prague/pr_vtbl.h>
// AVP Prague stamp end
#include <vector>
#include "p_updaterconfiguratorimplementation.h"
// AVP Prague stamp begin( C++ class declaration,  )
struct pr_novtable UpdaterConfiguratorImplementation : public cUpdaterConfigurator 
{
private:
// Internal function declarations
	tERROR pr_call ObjectInitDone();

// Property function declarations
	tERROR pr_call getUpdateDate( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );
	tERROR pr_call setUpdateDate( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );
	tERROR pr_call getUpdateOnlyBlackList( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );
	tERROR pr_call setUpdateOnlyBlackList( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );
	tERROR pr_call getComponentsToUpdate( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );
	tERROR pr_call setComponentsToUpdate( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );
	tERROR pr_call getUpdateResultFixNames( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );
	tERROR pr_call setUpdateResultFixNames( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );
	tERROR pr_call getUpdateResultNeedReboot( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );
	tERROR pr_call setUpdateResultNeedReboot( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );
	tERROR pr_call getUpdateScheduleTimeoutOnSuccess( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );
	tERROR pr_call setUpdateScheduleTimeoutOnSuccess( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );
	tERROR pr_call getUpdateScheduleTimeoutOnFailure( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );
	tERROR pr_call setUpdateScheduleTimeoutOnFailure( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );
	tERROR pr_call getRollbackAvailability( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );
	tERROR pr_call setRollbackAvailability( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );
	tERROR pr_call getRetranslateOnlyListedComponents( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );
	tERROR pr_call setRetranslateOnlyListedComponents( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );
	tERROR pr_call getComponentsToRetranslate( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );
	tERROR pr_call setComponentsToRetranslate( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );
	tERROR pr_call getRollbackMode( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );
	tERROR pr_call setRollbackMode( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );
	tERROR pr_call getInteractionWithUserEnabled( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );
	tERROR pr_call setInteractionWithUserEnabled( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );

public:
// External function declarations

// Data declaration
// AVP Prague stamp end
    tDATETIME m_updateDate;
    tBOOL m_updateOnlyBlackList;
    // not std::string type, but vector is used for convenience
    std::vector<char> m_componentsToUpdate;
    tBOOL m_retranslateOnlyListedComponents;
    std::vector<char> m_componentsToRetranslate;
    std::vector<char> m_updateResultFixNames;
    tBOOL m_updateResultNeedReboot;
    tDWORD m_updateScheduleTimeoutOnSuccess;
    tDWORD m_updateScheduleTimeoutOnFailure;
    tBOOL m_rollbackAvailable;
    tBOOL m_rollbackMode;
    tBOOL m_interactionWithUserEnabled;
// AVP Prague stamp begin( C++ class declaration end,  )
public:
    mDECLARE_INITIALIZATION(UpdaterConfiguratorImplementation)
};
// AVP Prague stamp end
// AVP Prague stamp begin( Header endif,  )
#endif // updaterconfigurator_cpp
// AVP Prague stamp end
