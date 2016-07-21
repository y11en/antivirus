// AVP Prague stamp begin( Interface header,  )
// -------- ANSI C++ Code Generator 1.0 --------
// -------- Tuesday,  23 October 2007,  20:44 --------
// -------------------------------------------
// Copyright © Kaspersky Lab 1996-2006.
// -------------------------------------------
// Project     -- Not defined
// Sub project -- Not defined
// Purpose     -- Not defined
// Author      -- Guschin
// File Name   -- basesverifier.cpp
// -------------------------------------------
// AVP Prague stamp end



// AVP Prague stamp begin( Interface version,  )
#define BasesVerifier_VERSION ((tVERSION)1)
// AVP Prague stamp end



// AVP Prague stamp begin( Includes for interface,  )
#include "basesverifier.h"
#include "../comdefs.h"
#include "../PragueCallbacksFake.h"
#include "../../../../SharedCode/PragueHelpers/PragueLog.h"
#include "../../include/journal_iface.h"
#include "../../include/DownloadProgress.h"

#include "../../helper/indexFileXMLParser.h"
// AVP Prague stamp end



// AVP Prague stamp begin( Global property variable declaration,  )
// Interface "BasesVerifier". Static(shared) property table variables
// AVP Prague stamp end

bool ParseComponentsList(const char* szComponents, KLUPD::ComponentIdentefiers& result)
{
    result.clear();

    const std::vector<KLUPD::NoCaseString> componentsToUpdate = 
        KLUPD::StringParser::splitString(KLUPD::NoCaseString().fromAscii(szComponents), KLUPD::NoCaseString().fromAscii(";,"));

    for(KLUPD::ComponentIdentefiers::const_iterator iter = componentsToUpdate.begin(); iter != componentsToUpdate.end(); ++iter)
        if( !iter->empty() )
            result.push_back(*iter);

    return true;
}

bool ParseDateTime(const char* szDateTime, tDATETIME& dtResult )
{
    char year[5], month[3], day[3], hour[3], minute[3];
    if( sscanf(szDateTime, "%2s%2s%4s %2s%2s", &day, &month, &year, &hour, &minute) != 5 )
        return false;

    cDateTime().SetUTC(atoi(year), atoi(month), atoi(day), atoi(hour), atoi(minute), 0).CopyTo(dtResult);
    return true;
}

// AVP Prague stamp begin( External (user called) interface method implementation, VerifyBases )
// Parameters are:
tERROR CBaseVerifier::VerifyBases( hSTRING p_components, hSTRING p_blackListPath, tDATETIME* p_basesDate )
{
	tERROR error = errOK;
	PR_TRACE_FUNC_FRAME( "BasesVerifier::VerifyBases method" );

    if(p_components == 0 || p_blackListPath == 0 || p_basesDate == 0)
        return errPARAMETER_INVALID;


    PragueCallbacksFake callbacks;
    KLUPD::EmptyDownloadProgress pragueDownloadProgress;
    KLUPD::EmptyJournal journal;
    PRAGUE_HELPERS::PragueLog log("DBVERIF", static_cast<tTRACE_LEVEL>(349));
    KLUPD::Updater updater(pragueDownloadProgress, callbacks, journal, &log);

    char szComponents[1024];
    p_components->ExportToBuff(NULL, cSTRING_WHOLE, szComponents, 1024, cCP_ANSI, cSTRING_Z);
    if(!ParseComponentsList(szComponents, callbacks.m_config.m_componentsToUpdate))
        return errNOT_OK;

    KLUPD::FileVector files;
    if(!updater.EnumerateLocalFiles(files, false))
        error = errNOT_OK;

    KLUPD::NoCaseString strUpdateDate = updater.GetUpdateDate(files);

    KLUPD::NoCaseString strBlstPath;
    for( KLUPD::FileVector::iterator iter = files.begin(); iter != files.end(); ++iter )
    {
        if( iter->isBlackList() )
        {
            strBlstPath = iter->m_localPath.m_value + iter->m_filename.m_value;
            break;
        }
    }

    if( strBlstPath.empty() || strUpdateDate.empty() )
        error = errNOT_OK;

    ParseDateTime(strUpdateDate.toAscii().c_str(), *p_basesDate);
    p_blackListPath->ImportFromBuff( NULL,
                                    (tPTR) strBlstPath.toWideChar(),
                                    strBlstPath.size()*sizeof(KLUPD::NoCaseStringImplementation::value_type),
                                    cCP_UNICODE,
                                    cSTRING_Z);

	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( C++ class regitration,  )
// Interface "BasesVerifier". Register function
tERROR CBaseVerifier::Register( hROOT root ) 
{
	tERROR error;
// AVP Prague stamp end



// AVP Prague stamp begin( Internal method table,  )
mINTERNAL_TABLE_BEGIN(BasesVerifier)
mINTERNAL_TABLE_END(BasesVerifier)
// AVP Prague stamp end



// AVP Prague stamp begin( External method table,  )
mEXTERNAL_TABLE_BEGIN(BasesVerifier)
	mEXTERNAL_METHOD(BasesVerifier, VerifyBases)
mEXTERNAL_TABLE_END(BasesVerifier)
// AVP Prague stamp end



// AVP Prague stamp begin( Registration call,  )

	PR_TRACE_FUNC_FRAME_( *root, "BasesVerifier::Register method", &error );

	error = CALL_Root_RegisterIFace(
		root,                                   // root object
		IID_BASESVERIFIER,                      // interface identifier
		PID_UPDATER2005,                        // plugin identifier
		0x00000000,                             // subtype identifier
		BasesVerifier_VERSION,                  // interface version
		VID_GUSCHIN,                            // interface developer
		&i_BasesVerifier_vtbl,                  // internal(kernel called) function table
		(sizeof(i_BasesVerifier_vtbl)/sizeof(tPTR)),// internal function table size
		&e_BasesVerifier_vtbl,                  // external function table
		(sizeof(e_BasesVerifier_vtbl)/sizeof(tPTR)),// external function table size
		NULL,                                   // property table
		0,                                      // property table size
		sizeof(CBaseVerifier)-sizeof(cObjImpl), // memory size
		0                                       // interface flags
	);

	#ifdef _PRAGUE_TRACE_
		if ( PR_FAIL(error) )
			PR_TRACE( (root,prtDANGER,"BasesVerifier(IID_BASESVERIFIER) registered [%terr]",error) );
	#endif // _PRAGUE_TRACE_
// AVP Prague stamp end



// AVP Prague stamp begin( C++ class regitration end,  )
	return error;
}

tERROR pr_call BasesVerifier_Register( hROOT root ) { return CBaseVerifier::Register(root); }
// AVP Prague stamp end



// AVP Prague stamp begin( Caution !!!,  )
// You have to implement propetry: pgINTERFACE_VERSION
// You have to implement propetry: pgINTERFACE_COMMENT
// AVP Prague stamp end
