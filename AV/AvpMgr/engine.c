// AVP Prague stamp begin( Interface header,  )
// -------- ANSI C Code Generator 2.0 --------
// -------- Wednesday,  12 January 2005,  11:44 --------
// -------------------------------------------
// Copyright © Kaspersky Labs 1996-2004.
// -------------------------------------------
// Project     -- Kaspersky Anti-Virus
// Sub project -- AV Engine
// Purpose     -- Менеджер антивирусной обработки (Anti-Virus Processing Manager).//eng:Anti-Virus Processing Manager.
// Author      -- Graf
// File Name   -- engine.c
// Additional info
//    Модуль AvpMgr обеспечиает антивирусную функциональность верхнего уровня для приложений.
//    //eng:
//    AvpMgr plugin - Anti-Virus Processing Manager - represents AV Engine to Applications.
//
// -------------------------------------------
// AVP Prague stamp end



// AVP Prague stamp begin( Private definitions,  )
#define Engine_PRIVATE_DEFINITION
// AVP Prague stamp end



// AVP Prague stamp begin( Includes for interface,  )
#include <Prague/prague.h>
#include <Prague/plugin/p_win32_nfio.h>
#include <AV/plugin/p_avpmgr.h>
// AVP Prague stamp end

#include <string.h>

#include "static.h"
#include <ProductCore/plugin/p_icheckersa.h>
#include <ProductCore/plugin/p_ichecker2.h>
#include <Extract/plugin/p_btimages.h>
#include "../../ProductCore/ISLite/iCheckerStreams/plugin_ichstrms.h"
#include "../../Extract/Wrappers/Dmap/plugin_dmap.h"
#include "../../Extract/Wrappers/Dmap/dmap.h"

#include <AV/iface/i_engine.h>
#include <Prague/iface/i_reg.h>
#include <Prague/iface/i_converter.h>
#include <AV/iface/i_hashct.h>
#include <Extract/iface/i_minarc.h>
#include <Prague/iface/i_buffer.h>
#include <Prague/iface/i_csect.h>
#include "../AVP1/engine.h"

#define  IMPEX_IMPORT_LIB
#include <Prague/iface/e_loader.h> 
#include <Prague/iface/e_ktrace.h> 
#include <Prague/iface/e_ktime.h>
#include <Extract/plugin/p_windiskio.h> 
#include <ProductCore/plugin/p_ichecker2.h>
#include <Prague/plugin/p_win32_nfio.h>
#include <Prague/pr_oid.h>
#include <AV/plugin/p_avlib.h> 

#include "../../Extract/Arc/MiniArchiver/Rar/rar.h"

#define  IMPEX_EXPORT_LIB
#include <AV/plugin/p_avpmgr.h>

//==============================================================
#ifdef KLAVPMGR
#include <AV/plugin/klavpmgr.h>
#endif // KLAVPMGR

tERROR KLAV_AVPMGR_ProcessSubEngine (hOBJECT avpmgr_this, tPID pid, hOBJECT object);

//==============================================================

#define DEFAULT_STACK_PROBE 0x00040000 //64K*4

#define AVPMGR_HASH_DATA_DETECTED          0xAD000000
#define AVPMGR_HASH_DATA_DETECTED_MODIFIED 0xAD000001
#define AVPMGR_HASH_DATA_DETECTED_VOLUME   0xAD000003
#define AVPMGR_HASH_DATA_DETECTED_DELETED  0xAD000004

#define VIRUS_NAME_SIZE 0x100

typedef struct tag_HASH_DATA
{
	tDWORD type;
	tDWORD detect_type;
	tDWORD detect_state;
	tDWORD detect_counter;
	tDWORD detect_certanity;
	tDWORD detect_disinfectability;
	tDWORD detect_danger;
	tDWORD detect_behaviour;
	tCHAR  detect_name[VIRUS_NAME_SIZE];
	hIO    io;
} tHASH_DATA;

#ifdef _DEBUG
#define SET_DEBUG_TRACE(x) //CALL_SYS_TraceLevelSet((hOBJECT)(x), tlsALL_OBJECTS, prtNOT_IMPORTANT, prtMIN_TRACE_LEVEL)
#else
#define SET_DEBUG_TRACE(x)
#endif

#ifdef _PRAGUE_TRACE_
static tDWORD dwTraceLevel = prtERROR;
#endif

typedef struct tag_ACTION
{
	tDWORD type;
	tDWORD cls;
#ifdef _PRAGUE_TRACE_
	tCHAR* name;
#endif
} tACTION;

typedef struct tag_ACTION_SetOfActions
{
	tDWORD type;
	tDWORD cls;
#ifdef _PRAGUE_TRACE_
	tCHAR* name;
#endif
	tDWORD actions_count;
	tDWORD* actions;
} tACTION_SetOfActions;

typedef struct tag_ACTION_ProcessOSonIO
{
	tDWORD type;
	tDWORD cls;
#ifdef _PRAGUE_TRACE_
	tCHAR* name;
#endif
	tDWORD pid;
	tDWORD child_origin;
	tDWORD mimi_pid;
	tBOOL multivolume;
} tACTION_ProcessOSonIO;

typedef struct tag_ACTION_ConvertIO
{
	tDWORD type;
	tDWORD cls;
#ifdef _PRAGUE_TRACE_
	tCHAR* name;
#endif
	tDWORD pid;
	tDWORD child_origin;
	tDWORD converter_pid;
} tACTION_ConvertIO;

typedef struct tag_ACTION_SwitchByProperty
{
	tDWORD type;
	tDWORD cls;
#ifdef _PRAGUE_TRACE_
	tCHAR* name;
#endif
	tDWORD propid;
	tDWORD actions_count;
	tDWORD* value;
	tDWORD* actions;
	tDWORD default_action;
} tACTION_SwitchByProperty;

typedef struct tag_ACTION_SubengineProcess
{
	tDWORD type;
	tDWORD cls;
#ifdef _PRAGUE_TRACE_
	tCHAR* name;
#endif
	hENGINE engine;
} tACTION_SubengineProcess;


// AVP Prague stamp begin( Interface comment,  )
// Engine interface implementation
// Short comments -- Базовый интерфейс обработки объектов.//eng:Generic interface for object processing.
//    Интерфейс определяет основные правила и методы обработки сложных объектов.
//    Служит для реализации как абстрактного антивирусного ядра, так и подключаемых сканеров.
// AVP Prague stamp end



#define PASSWORD_TABLE_SIZE 16

// AVP Prague stamp begin( Data structure,  )
// Interface Engine. Inner data structure

typedef struct tag_data 
{
	tDWORD    DB_Count;      // --
	tDATETIME DB_Time;       // --
	tSTRING   DB_Name;       // --
	tDWORD    DB_State;      // --
	tDWORD    EngineCount;     // --
	tPID*     EnginePID;    // --
	hENGINE*  EngineHandle; // --
	tDATETIME DB_LoadTime;     // --
	tSTRING   DB_LoadName;     // --
	tACTION** Actions;      // --
	tDWORD    ActionsCount; // --
	tDWORD    StackProbe;   // --
	tDWORD    Entry_OBJECT;  // --
	tDWORD    Entry_ARCHIVE; // --
	tDWORD    Entry_IO;      // --
	tDWORD    dwBaseTime;    // --
	hOBJECT   hIChecker;     // --
	tDWORD    dwQuarantineDays; // --
	hOBJECT   hICheckerStream;  // --
	tDWORD    dwLongQuarantineDays; // --
	tDWORD    dwIcheckerInitNumber; // --
	hOBJECT   hBTImages;            // --
	tBYTE             DB_Verdicts;                        // --
	hSTRING           PasswordTable[PASSWORD_TABLE_SIZE]; // --
	hCRITICAL_SECTION PasswordTableCS;                    // --
	tDWORD            PasswordTableIndex;                 // --
	tDWORD            PasswordTableIndexLast;             // --
	hENGINE           hKLAVPMGR;                          // --
//	hHASH             hHash;                               // --
} data;

// AVP Prague stamp end



// AVP Prague stamp begin( Object declaration,  )

typedef struct tag_hi_Engine 
{
	const iEngineVtbl* vtbl; // pointer to the "Engine" virtual table
	const iSYSTEMVTBL* sys;  // pointer to the "SYSTEM" virtual table
	data*              data; // pointer to the "Engine" data structure
} *hi_Engine;

// AVP Prague stamp end



// AVP Prague stamp begin( Internal method table prototypes,  )
tERROR pr_call Engine_ObjectInitDone( hi_Engine _this );
tERROR pr_call Engine_ObjectPreClose( hi_Engine _this );
tERROR pr_call Engine_MsgReceive( hi_Engine _this, tDWORD msg_cls_id, tDWORD msg_id, hOBJECT send_point, hOBJECT ctx, hOBJECT receive_point, tVOID* par_buf, tDWORD* par_buf_len );
// AVP Prague stamp end



tERROR UnloadConfiguration( hi_Engine _this );

tERROR EngineAction( hi_Engine _this, hOBJECT obj, tDWORD action_id);

tERROR EngineActionSetOfActions( hi_Engine _this, hOBJECT obj, tACTION_SetOfActions* action);
tERROR EngineActionProcessOSonIO( hi_Engine _this, hOBJECT obj, tACTION_ProcessOSonIO* action);
tERROR EngineActionConvertIO( hi_Engine _this, hOBJECT obj, tACTION_ConvertIO* action);
tERROR EngineActionSwitchByProperty( hi_Engine _this, hOBJECT obj, tACTION_SwitchByProperty* action);
tERROR EngineActionSubengineProcess( hi_Engine _this, hOBJECT obj, tACTION_SubengineProcess* action);

// AVP Prague stamp begin( Internal method table,  )
static iINTERNAL i_Engine_vtbl = 
{
	(tIntFnRecognize)         NULL,
	(tIntFnObjectNew)         NULL,
	(tIntFnObjectInit)        NULL,
	(tIntFnObjectInitDone)    Engine_ObjectInitDone,
	(tIntFnObjectCheck)       NULL,
	(tIntFnObjectPreClose)    Engine_ObjectPreClose,
	(tIntFnObjectClose)       NULL,
	(tIntFnChildNew)          NULL,
	(tIntFnChildInitDone)     NULL,
	(tIntFnChildClose)        NULL,
	(tIntFnMsgReceive)        Engine_MsgReceive,
	(tIntFnIFaceTransfer)     NULL
};
// AVP Prague stamp end



// AVP Prague stamp begin( Interface function forward declarations,  )
typedef   tERROR (pr_call *fnpEngine_Process)           ( hi_Engine _this, hOBJECT obj );   // -- Обработать объект.//eng:Process object.;
typedef   tERROR (pr_call *fnpEngine_GetAssociatedInfo) ( hi_Engine _this, hOBJECT obj, tDWORD info_id, tDWORD param, tPTR buffer, tDWORD size, tDWORD* out_size ); // -- Получить ассоциированные данные.//eng:Get associated data.;
typedef   tERROR (pr_call *fnpEngine_GetExtendedInfo)   ( hi_Engine _this, tDWORD info_id, tDWORD param, tPTR buffer, tDWORD size, tDWORD* out_size ); // -- Получить дополнительные свойства.//eng:Get extended information.;
typedef   tERROR (pr_call *fnpEngine_LoadConfiguration) ( hi_Engine _this, hREGISTRY reg, tRegKey key ); // -- Загрузить конфигурацию.;
// AVP Prague stamp end



// AVP Prague stamp begin( Interface declaration,  )
struct iEngineVtbl 
{
	fnpEngine_Process            Process;
	fnpEngine_GetAssociatedInfo  GetAssociatedInfo;
	fnpEngine_GetExtendedInfo    GetExtendedInfo;
	fnpEngine_LoadConfiguration  LoadConfiguration;
}; // "Engine" external virtual table prototype
// AVP Prague stamp end



// AVP Prague stamp begin( External method table prototypes,  )
tERROR pr_call Engine_Process( hi_Engine _this, hOBJECT obj );
tERROR pr_call Engine_GetAssociatedInfo( hi_Engine _this, hOBJECT obj, tDWORD info_id, tDWORD param, tPTR buffer, tDWORD size, tDWORD* out_size );
tERROR pr_call Engine_GetExtendedInfo( hi_Engine _this, tDWORD info_id, tDWORD param, tPTR buffer, tDWORD size, tDWORD* out_size );
tERROR pr_call Engine_LoadConfiguration( hi_Engine _this, hREGISTRY reg, tRegKey key );
// AVP Prague stamp end



// AVP Prague stamp begin( External method table,  )
static iEngineVtbl e_Engine_vtbl = 
{
	(fnpEngine_Process)       Engine_Process,
	(fnpEngine_GetAssociatedInfo) Engine_GetAssociatedInfo,
	(fnpEngine_GetExtendedInfo) Engine_GetExtendedInfo,
	(fnpEngine_LoadConfiguration) Engine_LoadConfiguration
};
// AVP Prague stamp end



// AVP Prague stamp begin( Forwarded property methods declarations,  )
tERROR pr_call Engine_PROP_SetIChecker( hi_Engine _this, tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );
// AVP Prague stamp end



// AVP Prague stamp begin( Global property variable declaration,  )
// Interface "Engine". Static(shared) property table variables
const tVERSION EngineVersion = 1; // must be READ_ONLY at runtime
// AVP Prague stamp end



// AVP Prague stamp begin( Interface version,  )
#define Engine_VERSION ((tVERSION)1)
// AVP Prague stamp end



// AVP Prague stamp begin( Header includes,  )
#include <Prague/prague.h>
#include <Prague/iface/i_root.h>
#include <Prague/iface/i_os.h>
#include <Prague/iface/i_objptr.h>
#include <Prague/iface/i_io.h>
#include <Prague/iface/i_string.h>
#include <Prague/iface/i_reg.h>

#include <AV/plugin/p_avpmgr.h>
// AVP Prague stamp end



// AVP Prague stamp begin( Property table,  )
mPROPERTY_TABLE(Engine_PropTable)
	mSHARED_PROPERTY( pgINTERFACE_VERSION, Engine_VERSION )
	mSHARED_PROPERTY_PTR( pgINTERFACE_COMMENT, "AV Processing Manager", 22 )
	mSHARED_PROPERTY_PTR( pgENGINE_VERSION, EngineVersion, sizeof(EngineVersion) )
	mLOCAL_PROPERTY_BUF( pgENGINE_DB_NAME, data, DB_Name, cPROP_BUFFER_READ )
	mLOCAL_PROPERTY_BUF( pgENGINE_DB_COUNT, data, DB_Count, cPROP_BUFFER_READ )
	mLOCAL_PROPERTY_BUF( pgENGINE_DB_TIME, data, DB_Time, cPROP_BUFFER_READ )
	mLOCAL_PROPERTY_BUF( pgENGINE_DB_STATE, data, DB_State, cPROP_BUFFER_READ )
	mLOCAL_PROPERTY_BUF( pgENGINE_DB_LOAD_NAME, data, DB_LoadName, cPROP_BUFFER_READ )
	mLOCAL_PROPERTY_BUF( pgENGINE_DB_LOAD_TIME, data, DB_LoadTime, cPROP_BUFFER_READ )
	mLOCAL_PROPERTY_BUF( pgENGINE_SUBENGINE_COUNT, data, EngineCount, cPROP_BUFFER_READ )
	mLOCAL_PROPERTY_BUF( pgENGINE_SUBENGINE_TABLE, data, EngineHandle, cPROP_BUFFER_READ )
	mLOCAL_PROPERTY_BUF( plENGINE_STACK_PROBE, data, StackProbe, cPROP_BUFFER_READ_WRITE )
	mLOCAL_PROPERTY( plENGINE_ICHECKER, data, hIChecker, cPROP_BUFFER_READ_WRITE, NULL, Engine_PROP_SetIChecker )
	mLOCAL_PROPERTY_BUF( plENGINE_DB_TIME_DWORD, data, dwBaseTime, cPROP_BUFFER_READ )
	mLOCAL_PROPERTY_BUF( plENGINE_QUARANTINE_DAYS_DWORD, data, dwQuarantineDays, cPROP_BUFFER_READ_WRITE )
	mLOCAL_PROPERTY_BUF( plENGINE_LONG_QUARANTINE_DAYS_DWORD, data, dwLongQuarantineDays, cPROP_BUFFER_READ_WRITE )
	mLOCAL_PROPERTY_BUF( plENGINE_ICHECKER_STREAM, data, hICheckerStream, cPROP_BUFFER_READ_WRITE )
	mLOCAL_PROPERTY_BUF( plENGINE_ICHECKER_INIT_NUMBER, data, dwIcheckerInitNumber, cPROP_BUFFER_READ )
mPROPERTY_TABLE_END(Engine_PropTable)
// AVP Prague stamp end



// AVP Prague stamp begin( Registration call,  )
// Interface "Engine". Register function
tERROR pr_call Engine_Register( hROOT root ) 
{
	tERROR error;

	PR_TRACE_A0( root, "Enter \"Engine::Register\" method" );

	error = CALL_Root_RegisterIFace(
		root,                                   // root object
		IID_ENGINE,                             // interface identifier
		PID_AVPMGR,                             // plugin identifier
		0x00000001,                             // subtype identifier
		Engine_VERSION,                         // interface version
		VID_GRAF,                               // interface developer
		&i_Engine_vtbl,                         // internal(kernel called) function table
		(sizeof(i_Engine_vtbl)/sizeof(tPTR)),   // internal function table size
		&e_Engine_vtbl,                         // external function table
		(sizeof(e_Engine_vtbl)/sizeof(tPTR)),   // external function table size
		Engine_PropTable,                       // property table
		mPROPERTY_TABLE_SIZE(Engine_PropTable), // property table size
		sizeof(data),                           // memory size
		0                                       // interface flags
	);

	#ifdef _PRAGUE_TRACE_
		if ( PR_FAIL(error) )
			PR_TRACE( (root,prtDANGER,"Engine(IID_ENGINE) registered [%terr]",error) );
	#endif // _PRAGUE_TRACE_

	PR_TRACE_A1( root, "Leave \"Engine::Register\" method, ret %terr", error );
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Caution !!!,  )
// You have to implement propetry: pgENGINE_DB_VERDICTS
// AVP Prague stamp end



// AVP Prague stamp begin( Property method implementation, SetIChecker )
// Interface "Engine". Method "Set" for property(s):
//  -- ENGINE_ICHECKER
tERROR pr_call Engine_PROP_SetIChecker( hi_Engine _this, tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size )
{
	tERROR error = errOK;
	PR_TRACE_A0( _this, "Enter *SET* method Engine_PROP_SetIChecker for property plENGINE_ICHECKER" );

	if(_this->data->hIChecker)
	{
		error = CALL_SYS_PropertySetDWord(_this->data->hIChecker, plDB_USER_DATA_SIZE, 4*sizeof(tDWORD));
		if (PR_FAIL(error))
			PR_TRACE(( _this, prtIMPORTANT, "Engine_SetIChecker: plDB_USER_DATA_SIZE failed %terr", error));

		if(PR_SUCC(error))
		{
			error = CALL_SYS_PropertySetDWord(_this->data->hIChecker, plDB_USER_VERSION, 0x00000003);
			if (PR_FAIL(error))
				PR_TRACE(( _this, prtIMPORTANT, "Engine_SetIChecker: plDB_USER_VERSION failed %terr", error));
		}
			
		if(PR_SUCC(error))
		{
			error=CALL_SYS_ObjectCreateDone(_this->data->hIChecker);
			if (PR_FAIL(error))
				PR_TRACE(( _this, prtIMPORTANT, "Engine_SetIChecker: CreateDone failed %terr", error));
		}
		
		if(PR_SUCC(error))
		{
			error = CALL_IChecker_FlushStatusDB((hICHECKER)(_this->data->hIChecker));
			if (PR_FAIL(error))
				PR_TRACE(( _this, prtIMPORTANT, "Engine_SetIChecker: FlushStatusDB failed %terr", error));
		}
		
		if(PR_FAIL(error))
			_this->data->hIChecker = NULL;

	}
		
	PR_TRACE_A2( _this, "Leave *SET* method Engine_PROP_SetIChecker for property plENGINE_ICHECKER, ret tDWORD = %u(size), error = 0x%08x", *out_size, error );
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Internal (kernel called) interface method implementation, ObjectInitDone )
// Extended method comment
//  Notification that all necessary properties have been set and object must go to operation state
// Result comment
//  Returns value differ from errOK if object cannot function properly
// Parameters are:
tERROR pr_call Engine_ObjectInitDone( hi_Engine _this )
{
	tERROR error = errOK;
	hHEAP hHeap;
	PR_TRACE_A0( _this, "Enter \"Engine::ObjectInitDone\" method" );

	if(_this->data->StackProbe==0) _this->data->StackProbe=DEFAULT_STACK_PROBE;

	if(PR_SUCC(error))
		error=CALL_SYS_ObjectCreateQuick(_this, &_this->data->PasswordTableCS, IID_CRITICAL_SECTION, PID_LOADER, SUBTYPE_ANY);
	if(PR_SUCC(error))
		error=CALL_SYS_ObjectCreateQuick(_this, &hHeap, IID_HEAP, PID_LOADER, SUBTYPE_ANY);
	if(PR_SUCC(error))
		error=CALL_SYS_PropertySetDWord(_this, pgOBJECT_HEAP, (tDWORD)hHeap);

	PR_TRACE_A1( _this, "Leave \"Engine::ObjectInitDone\" method, ret %terr", error );
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Internal (kernel called) interface method implementation, ObjectPreClose )
// Extended method comment
//  Kernel warns object it is going to close all children
// Behaviour comment
//  Object takes all necessary "before preclosing" actions
// Parameters are:
tERROR pr_call Engine_ObjectPreClose( hi_Engine _this )
{
	tERROR error = errOK;
	PR_TRACE_A0( _this, "Enter \"Engine::ObjectPreClose\" method" );

	error=UnloadConfiguration(_this);

	PR_TRACE_A1( _this, "Leave \"Engine::ObjectPreClose\" method, ret %terr", error );
	return error;
}
// AVP Prague stamp end



#define COPY_BUFFER_SIZE 0x4000
tERROR CopyIOContent(hIO dest_io,hIO src_io)
{
	tBYTE buf[COPY_BUFFER_SIZE];
	tQWORD written=0;
	tDWORD size=0;
	tQWORD qwSize=0;
	tERROR error=CALL_IO_GetSize(src_io, &qwSize,IO_SIZE_TYPE_EXPLICIT);

	if(PID_NATIVE_FIO == OBJ_PID(dest_io))
	{
		tQWORD qwSizeDest;
		if(PR_SUCC(CALL_IO_GetSize(dest_io, &qwSizeDest,IO_SIZE_TYPE_EXPLICIT)))
		{
			if( qwSize > qwSizeDest )
			{
				tQWORD free_space;
				if(PR_SUCC(PrGetDiskFreeSpace((hOBJECT)dest_io,&free_space)))
				{
					if( qwSizeDest + free_space < qwSize )
						error = errOUT_OF_SPACE;
				}
			}
		}
	}

	while(PR_SUCC(error))
	{
		error=CALL_IO_SeekRead(src_io,&size,written,buf,COPY_BUFFER_SIZE);
		if(size==0)
			break;
		if(PR_SUCC(error))
		{
			error=CALL_IO_SeekWrite(dest_io,&size,written,buf,size);
			written+=size;
		}
		if(PR_SUCC(error))
			error = CALL_SYS_SendMsg(src_io,pmc_PROCESSING,pm_PROCESSING_YIELD,0,0,0);
	}

	if(written==qwSize)
		error=errOK;
	else if(PR_SUCC(error))
		error=errNOT_OK;

	if(PR_SUCC(error)) 
		error=CALL_IO_SetSize(dest_io,written);

	if(PR_SUCC(error)) 
		error=CALL_IO_Flush(dest_io);
	return error;
}


tERROR IncrementInfected(hOBJECT obj, tDWORD count)
{
	hOBJECT obj_main=CALL_SYS_PropertyGetObj(obj,propid_object_main);
	if(count==0) //rollback if error
	{
		count=CALL_SYS_PropertyGetDWord(obj,propid_detect_counter)
			-CALL_SYS_PropertyGetDWord(obj,propid_detect_state);
	}
	if(count)
	{
		tDWORD 	detect_type=CALL_SYS_PropertyGetDWord(obj,propid_detect_type);
		tDWORD 	detect_certanity=CALL_SYS_PropertyGetDWord(obj,propid_detect_certanity);
		tDWORD 	detect_disinfectability=CALL_SYS_PropertyGetDWord(obj,propid_detect_disinfectability);
		tDWORD 	detect_danger=CALL_SYS_PropertyGetDWord(obj,propid_detect_danger);
		tDWORD 	detect_behaviour=CALL_SYS_PropertyGetDWord(obj,propid_detect_behaviour);
		tCHAR   detect_name[VIRUS_NAME_SIZE];
		strcpy_s(detect_name,countof(detect_name),"{BAD_VIRUS_NAME}");
		CALL_SYS_PropertyGetStr(obj,0,propid_detect_name,detect_name,VIRUS_NAME_SIZE,cCP_ANSI);
		
		PR_TRACE((obj, prtIMPORTANT, "AVPMGR\tDetected: %s", detect_name));

		while(obj)
		{
			tDWORD 	_type=CALL_SYS_PropertyGetDWord(obj,propid_detect_type);
			tDWORD 	_certanity=CALL_SYS_PropertyGetDWord(obj,propid_detect_certanity);
			tDWORD 	_disinfectability=CALL_SYS_PropertyGetDWord(obj,propid_detect_disinfectability);
			tDWORD 	_danger=CALL_SYS_PropertyGetDWord(obj,propid_detect_danger);
			tDWORD 	_behaviour=CALL_SYS_PropertyGetDWord(obj,propid_detect_behaviour);

			tDWORD 	infected=CALL_SYS_PropertyGetDWord(obj,propid_detect_state);
			CALL_SYS_PropertySetDWord(obj,propid_detect_state, infected + count);

			infected=CALL_SYS_PropertyGetDWord(obj,propid_detect_counter);
			CALL_SYS_PropertySetDWord(obj,propid_detect_counter,infected + count);

			CALL_SYS_PropertySetStr(obj,0,propid_detect_name,detect_name,(tDWORD)strlen(detect_name)+1,cCP_ANSI);

			if( infected )
			{
				if( _disinfectability != detect_disinfectability )
					CALL_SYS_PropertySetDWord(obj,propid_detect_disinfectability,ENGINE_DETECT_DISINFECTABILITY_UNKNOWN);

				if( _danger > detect_danger )
				{
					CALL_SYS_PropertySetDWord(obj,propid_detect_type,detect_type);
					CALL_SYS_PropertySetDWord(obj,propid_detect_certanity,detect_certanity);
					CALL_SYS_PropertySetDWord(obj,propid_detect_danger, detect_danger);
					CALL_SYS_PropertySetDWord(obj,propid_detect_behaviour, detect_behaviour);
				}
				else if(_danger == detect_danger)
				{
					if( _certanity > detect_certanity ) //means previous was less sure
						CALL_SYS_PropertySetDWord(obj,propid_detect_certanity,detect_certanity);
				}
			}
			else
			{
				CALL_SYS_PropertySetDWord(obj,propid_detect_type,detect_type);
				CALL_SYS_PropertySetDWord(obj,propid_detect_certanity,detect_certanity);
				CALL_SYS_PropertySetDWord(obj,propid_detect_disinfectability,detect_disinfectability);
				CALL_SYS_PropertySetDWord(obj,propid_detect_danger, detect_danger);
				CALL_SYS_PropertySetDWord(obj,propid_detect_behaviour, detect_behaviour);
			}


			if(obj == obj_main)
				break;
			if(IID_ROOT == OBJ_IID(obj))
				break;
			obj=CALL_SYS_ParentGet(obj,IID_ANY);
		}
	}
	return errOK;
}
tERROR DecrementInfected(hOBJECT obj, tDWORD count, tBOOL both_counters)
{
	if(count==0)
		count =CALL_SYS_PropertyGetDWord(obj,propid_detect_state);
	if(count==0) 
		return errOK;

	while(obj){
		tDWORD infected=CALL_SYS_PropertyGetDWord(obj,propid_detect_state);
		if(infected==0) 
			break;
		if(infected<count)
			infected=count;
		CALL_SYS_PropertySetDWord(obj,propid_detect_state, infected - count);
		if(both_counters)
		{
			infected=CALL_SYS_PropertyGetDWord(obj,propid_detect_counter);
			if(infected==0) 
				break;
			if(infected<count) 
				infected=count;
			CALL_SYS_PropertySetDWord(obj,propid_detect_counter, infected - count);
		}
		if(IID_ROOT == OBJ_IID(obj))
			break;
		obj=CALL_SYS_ParentGet(obj,IID_ANY);
	}
	return errOK;
}

tERROR HC_UpdateData(  hHASHCONTAINER HashCont, tQWORD qwHash, tHASH_DATA* p_Data, tBOOL p_bDeleteHash)
{
	tHASH_DATA* pData=0;
	tDWORD size=sizeof(pData);
	tERROR error=errOK;

	if(!qwHash)
		return errOK;

	if(PR_SUCC(CALL_HashContainer_GetHash(HashCont,qwHash,(tPTR*)&pData)))
		CALL_HashContainer_DeleteHash(HashCont,qwHash);

	if(p_bDeleteHash)
	{
		if(pData)
			error=CALL_SYS_ObjHeapFree(HashCont,pData);
	}
	else
	{
		if(p_Data->detect_counter){
			if(!pData)
			{
				error=CALL_SYS_ObjHeapAlloc(HashCont,(tPTR*)&pData,sizeof(tHASH_DATA));
				if(PR_SUCC(error))
					memcpy(pData,p_Data,sizeof(tHASH_DATA));
			}
			else if(p_Data->io)
					memcpy(pData,p_Data,sizeof(tHASH_DATA));
		}
		else if(pData)
		{
			CALL_SYS_ObjHeapFree(HashCont,pData);
			pData=0;
		}

		if(PR_SUCC(error))
			error=CALL_HashContainer_SetHash(HashCont,qwHash,pData);
	}

	return error;
}


tERROR HC_UpdateAllData( hOBJECT p_obj )
{
	tERROR error=errOK;
	tQWORD qwHash=CALL_SYS_PropertyGetQWord(p_obj,propid_os_hash);
	
	if(qwHash)
	{
		hHASHCONTAINER HashCont=(hHASHCONTAINER)CALL_SYS_PropertyGetObj(p_obj, propid_hash_container);
		if( HashCont)
		{
			hLIST hash_list;
			tHASH_DATA Data;
			tBOOL b_delete_hash=0;
			tERROR err=CALL_SYS_PropertyGetDWord(p_obj, propid_processing_error);
			if(PR_FAIL(err))
				b_delete_hash=1;
			if(err==errENGINE_OS_VOLUME_SET_INCOMPLETE)
				b_delete_hash=0;

			memset(&Data, 0, sizeof(Data));
			Data.detect_counter=CALL_SYS_PropertyGetDWord(p_obj,propid_detect_counter);
			if(Data.detect_counter)
			{
				b_delete_hash=0;
				Data.detect_state = CALL_SYS_PropertyGetDWord(p_obj,propid_detect_state);
				Data.detect_type = CALL_SYS_PropertyGetDWord(p_obj,propid_detect_type);
				Data.detect_certanity = CALL_SYS_PropertyGetDWord(p_obj,propid_detect_certanity);
				Data.detect_danger = CALL_SYS_PropertyGetDWord(p_obj,propid_detect_danger);
				Data.detect_behaviour = CALL_SYS_PropertyGetDWord(p_obj,propid_detect_behaviour);
				Data.detect_disinfectability = CALL_SYS_PropertyGetDWord(p_obj,propid_detect_disinfectability);

				CALL_SYS_PropertyGetStr(p_obj,0,propid_detect_name,Data.detect_name,sizeof(Data.detect_name),cCP_ANSI);
				
				Data.type = AVPMGR_HASH_DATA_DETECTED;
				if( Data.detect_state!=Data.detect_counter )
				{
					hIO io;
					if(PR_SUCC(CALL_SYS_ObjectCreateQuick( HashCont, &io, IID_IO, PID_TMPFILE, 0 )))
					{
						if(PR_SUCC(CopyIOContent(io,(hIO)p_obj)))
						{
							Data.type = AVPMGR_HASH_DATA_DETECTED_MODIFIED;
							Data.io=io;
						}
						else 
						{
							CALL_SYS_ObjectClose(io);
						}
					}
				}
			}

			error=HC_UpdateData(HashCont,qwHash,&Data,b_delete_hash);

			hash_list=(hLIST)CALL_SYS_PropertyGetObj(p_obj,propid_hash_temp_list);

			if( hash_list )
			{
				tLISTNODE node=0;
				CALL_SYS_PropertyDelete(p_obj,propid_hash_temp_list);
				if(Data.detect_counter)
				{
					if( CALL_SYS_PropertyGetBool(p_obj,pgOBJECT_DELETE_ON_CLOSE) )
						Data.type = AVPMGR_HASH_DATA_DETECTED_DELETED;
					else
						Data.type = AVPMGR_HASH_DATA_DETECTED_VOLUME;
					Data.io=0;
				}
				while(PR_SUCC(CALL_List_First(hash_list,&node)) && node)
				{
					tQWORD l_qwHash;
					if(PR_SUCC(error=CALL_List_DataGet(hash_list,0,node,&l_qwHash,sizeof(l_qwHash))))
					{
						if(l_qwHash != qwHash)
							error=HC_UpdateData(HashCont,l_qwHash,&Data,b_delete_hash);
					}
					CALL_List_Remove(hash_list,node);
				}
				CALL_SYS_ObjectClose(hash_list);
			}
		}
	}
	return error;
}
 
tERROR HC_AddHash( hOBJECT p_obj, tQWORD qwHash )
{
	tERROR error=errOK;
	if(qwHash)
	{
		hOBJECT parent_io=CALL_SYS_PropertyGetObj(p_obj,propid_object_parent_io);

		if(parent_io)
		{
			hLIST hash_list=(hLIST)CALL_SYS_PropertyGetObj(parent_io,propid_hash_temp_list);
			if(!hash_list){
				error=CALL_SYS_ObjectCreateQuick(parent_io, &hash_list, IID_LIST, PID_ANY, 0);
				if(PR_SUCC(error))
					error=CALL_SYS_PropertySetObj(parent_io,propid_hash_temp_list,(hOBJECT)hash_list);
			}
			if(hash_list){
				error=CALL_List_Add(hash_list,0,&qwHash,sizeof(qwHash),0,cLIST_LAST);
			}
		}
	}
	return error;
}

tERROR CALL_SYS_PropertyFree(hOBJECT obj,tPROPID prop)  {
	hOBJECT obj_to_close=CALL_SYS_PropertyGetObj(obj, prop);
	if(obj_to_close && PR_SUCC(CALL_SYS_ObjectValid(obj,obj_to_close))){
		CALL_SYS_ObjectClose(obj_to_close);
	}
	return CALL_SYS_PropertyDelete(obj, prop );
}

tERROR CreateBtImages( hi_Engine _this, hBTIMAGES* p_BTImages )
{
	hBTIMAGES hBTImages=0;
	tERROR error=CALL_SYS_ObjectCreate( _this, &hBTImages, IID_BTIMAGES, PID_BTIMAGES, 0 );
	if(PR_SUCC(error))
	{
		hSTRING str;
		if(PR_SUCC(CALL_SYS_ObjectCreateQuick(_this, &str, IID_STRING,PID_ANY,SUBTYPE_ANY )))
		{
			CALL_String_AddFromBuff(str,0,"%Data%\\BTImages.dat",0,cCP_ANSI,0);
			CALL_SYS_SendMsg(_this,pmc_PRODUCT_ENVIRONMENT, pm_EXPAND_ENVIRONMENT_STRING, (hOBJECT)str,0,0);
			CALL_String_ExportToProp(str,0,cSTRING_WHOLE,(hOBJECT)hBTImages,pgRepositoryName);
			CALL_SYS_ObjectClose(str);
		}
		if(PR_FAIL(error=CALL_SYS_ObjectCreateDone(hBTImages)))
			CALL_SYS_ObjectClose(hBTImages);
		else
			*p_BTImages=hBTImages;
	}
	return error;
}


// AVP Prague stamp begin( Internal (kernel called) interface method implementation, MsgReceive )
// Extended method comment
//  Receives message sent to the object or to the one of the object parents as broadcast
// Parameters are:
// "msg_cls_id"    : Message class identifier
// "msg_id"        : Message identifier
// "obj"           : Object as a reason of message
// "ctx"           : Context of the object processing
// "receive_point" : Point of distributing message (tree top for RegisterMsgHandler call
// "par_buf"       : Buffer of the parameters
// "par_buf_len"   : Lenght of the buffer of the parameters
tERROR pr_call Engine_MsgReceive( hi_Engine _this, tDWORD msg_cls_id, tDWORD msg_id, hOBJECT send_point, hOBJECT ctx, hOBJECT receive_point, tVOID* par_buf, tDWORD* par_buf_len )
{
	tERROR error = errOK;
	PR_TRACE_A0( _this, "Enter \"Engine::MsgReceive\" method" );

	switch(msg_cls_id){

	case pmc_OS_VOLUME:
		switch( msg_id ) {
		case pm_OS_VOLUME_HASH:
			if(par_buf)
			{
				tQWORD qwHash=*(tQWORD*)par_buf;
				if( NULL != CALL_SYS_PropertyGetObj(send_point,propid_object_executable_parent))
					qwHash ^= (tQWORD)1;

				error=HC_AddHash(send_point,qwHash);
			}
			else
				error=errPARAMETER_INVALID;
			break;
		case pm_OS_VOLUME_SET_INCOMPLETE:
			{
				hOBJECT parent_io=CALL_SYS_PropertyGetObj(send_point,propid_object_parent_io);
				if(parent_io)
					CALL_SYS_PropertySetDWord(parent_io, propid_processing_error, errENGINE_OS_VOLUME_SET_INCOMPLETE);
			}
			break;
		
		default:
			break;
		}			
		break;

	case pmc_IO:
		switch( msg_id ) {
		case pm_IO_DELETE_ON_CLOSE_SUCCEED:
			DecrementInfected(send_point,0,cFALSE);
			break;

		case pm_IO_DELETE_ON_CLOSE_FAILED:
			break;
		
		case pm_IO_PASSWORD_EXE_HEURISTIC:
			{
				CALL_SYS_PropertySetDWord( send_point, propid_object_readonly_error,  errOBJECT_PASSWORD_PROTECTED);
				CALL_SYS_PropertySetObj(   send_point, propid_object_readonly_object, (hOBJECT)send_point);
			}
			break;

		case pm_IO_PASSWORD_REQUEST:
			PR_TRACE((_this, prtIMPORTANT, "AVPMGR\tPassword request processing"));
			if(_this->data->PasswordTableCS && OBJ_IID(ctx)==IID_STRING)
			{
				tBOOL auto_passw;
				tDWORD index;

				auto_passw=CALL_SYS_PropertyGetBool(send_point,propid_auto_password_bool);
				if( !auto_passw )
					break;
				index=CALL_SYS_PropertyGetDWord(send_point,propid_password_node);
				if( index == -1 )
					break;


				if(PR_SUCC(CALL_CriticalSection_Enter(_this->data->PasswordTableCS,SHARE_LEVEL_READ)))
				{
					if(_this->data->PasswordTable[index])
					{
                        CALL_String_ExportToProp(_this->data->PasswordTable[index],0,cSTRING_WHOLE,CALL_SYS_ParentGet(send_point,IID_ANY),propid_password_str);
                        CALL_String_ExportToProp(_this->data->PasswordTable[index],0,cSTRING_WHOLE,send_point,propid_password_str);

						if(PR_SUCC(CALL_String_ImportFromStr((hSTRING)ctx,0,_this->data->PasswordTable[index],cSTRING_WHOLE)))
						{
							error=errOK_DECIDED;
							if(++index == PASSWORD_TABLE_SIZE)
								index=-1;
							CALL_SYS_PropertySetDWord(send_point,propid_password_node,index);
						}
					}
					CALL_CriticalSection_Leave(_this->data->PasswordTableCS,NULL);
				}

				if( error!=errOK_DECIDED)
				{
					error=errOK;
					CALL_SYS_PropertySetDWord(send_point,propid_password_node,-1);
					CALL_SYS_SendMsg(send_point,pmc_ENGINE_IO,pm_ENGINE_IO_MAKE_VIRTUAL_NAME,_this,0,0);
				}
			}
			break;

		default:
			break;
		}			
		break;
		
	case pmc_ENGINE_IO:

		switch(msg_id){
		case pm_ENGINE_IO_MAKE_VIRTUAL_NAME:
			{
				hOBJECT parent;
				hSTRING str;
				tDWORD len=0;

				if( 0==par_buf && CALL_SYS_PropertySize( send_point, propid_virtual_name))
					break;
				
				error=CALL_SYS_ObjectCreateQuick(send_point, &str,IID_STRING,PID_ANY,SUBTYPE_ANY);
				if ( PR_FAIL(error)) return error;

				parent=send_point;
				while( ( parent=CALL_SYS_ParentGet(parent,IID_ANY) ) != 0){
					if(IID_ROOT==OBJ_IID(parent))
						break;
					if(PR_SUCC(CALL_String_ImportFromProp( str, 0, parent, propid_virtual_name )))
						break;
					continue;
				}
				CALL_String_Length( str, &len );

				if((len && parent && IID_OBJPTR==OBJ_IID(parent)) || IID_OS==OBJ_IID(send_point))
				{
				}
				else
				{
					if( len )	CALL_String_AddFromBuff( str, 0, par_buf? "//": "/", 2, cCP_ANSI, 0 );
					
					if(PR_SUCC(CALL_SYS_ObjectCheck(_this,(hOBJECT)par_buf,IID_STRING,PID_ANY,SUBTYPE_ANY,cFALSE)))
						error=CALL_String_AddFromStr( str, &len, (hSTRING)par_buf, cSTRING_WHOLE );
					else if(par_buf)
						error=CALL_String_AddFromBuff( str, &len, par_buf, (tDWORD)strlen(par_buf)+1, cCP_ANSI, 0 );
					else
						error=CALL_String_AddFromProp( str, &len, send_point, pgOBJECT_FULL_NAME );
					if(PR_FAIL(error))
						error=CALL_String_AddFromProp( str, &len, send_point, pgOBJECT_NAME );
					if(PR_FAIL(error))
						CALL_String_AddFromBuff( str, 0, "???", 11, cCP_ANSI, 0 );
					
					CALL_String_TrimBuff(str,"\\/",3,cCP_ANSI,fSTRING_RIGHT);
				}

				error= CALL_String_ExportToProp( str, &len, cSTRING_WHOLE, send_point, propid_virtual_name);
				CALL_SYS_ObjectClose(str);
			}
			break;

		case pm_ENGINE_IO_PROCESSING_ERROR:
			CALL_SYS_PropertySetDWord(receive_point, propid_processing_error, *(tERROR*)par_buf);
			break;

		case pm_ENGINE_IO_CHANGE_ACCESS_MODE:
			//moved to disinfect request
			break;
			
		case pm_ENGINE_IO_DETECT:
			{
				hOBJECT int_par;
				IncrementInfected(send_point,1);
				CALL_SYS_PropertySetObj(send_point,propid_object_infected_parent,send_point);
				int_par=send_point;
				while( int_par=CALL_SYS_PropertyGetObj(int_par,propid_integral_parent_io ))
					CALL_SYS_PropertySetObj(int_par,propid_object_infected_parent,send_point);
			}
			break;

		case pm_ENGINE_IO_DISINFECTED:
			DecrementInfected(send_point,1,cFALSE);
// 6.0 handle this in AVS
//			if(CALL_SYS_PropertyGetObj(send_point,propid_integral_parent_io))
//				error=errOK_DECIDED;
			break;

		case pm_ENGINE_IO_BEGIN:
			if(0 == CALL_SYS_PropertyGetDWord(send_point,propid_creator_engine_pid))
			{
				if((tDWORD)ctx != (tDWORD)_this)
				{
					CALL_SYS_PropertySetDWord(send_point,propid_creator_engine_pid,(tDWORD)ctx);
					error=CALL_SYS_SendMsg(send_point,pmc_ENGINE_IO,pm_ENGINE_IO_BEGIN,_this,0,0);
				}
			}
			break;
		case pm_ENGINE_IO_DONE:
			if(CALL_SYS_PropertyGetBool(send_point,pgOBJECT_DELETE_ON_CLOSE))
			{
				hOBJECT deletable_parent=0;
				tERROR l_error=Engine_SetWriteAccess(send_point,&deletable_parent);
				if( !deletable_parent )
				{
					tDWORD l_size=sizeof(l_error);
					CALL_SYS_PropertySetBool(send_point,pgOBJECT_DELETE_ON_CLOSE,0);
					CALL_SYS_SendMsg(send_point,pmc_IO,pm_IO_DELETE_ON_CLOSE_FAILED,_this,&l_error,&l_size);
				}
			}
			
			if((tDWORD)ctx == CALL_SYS_PropertyGetDWord(send_point,propid_creator_engine_pid))
			{
				if((tDWORD)ctx != (tDWORD)_this)
				{
					error=CALL_SYS_SendMsg(send_point,pmc_ENGINE_IO,pm_ENGINE_IO_DONE,_this,0,0);
					CALL_SYS_PropertySetDWord(send_point,propid_creator_engine_pid,0);
				}
			}

			if(0==CALL_SYS_PropertyGetDWord(send_point,propid_detect_counter))
			{
				switch(CALL_SYS_PropertyGetDWord(send_point,pgOBJECT_ORIGIN))
				{
				case OID_LOGICAL_DRIVE:
				case OID_PHYSICAL_DISK:
					{
						hBTIMAGES hBTImages;
						if(PR_SUCC(CreateBtImages(_this,&hBTImages)))
						{
							CALL_BtImages_Save(hBTImages,send_point);
							CALL_SYS_ObjectClose(hBTImages);
						}
					}
				default:
					break;
				}	
			}
			break;
			
		default:
			break;
		}
		break;
		
		case pmc_AVPMGR_INTERNAL:
			
			switch(msg_id){
				
			case pm_AVPMGR_INTERNAL_IO_HASH_FRAME_CHECK:
				{
					hHASHCONTAINER HashCont=(hHASHCONTAINER)CALL_SYS_PropertyGetObj(send_point, propid_hash_container);
					tQWORD qwHash=*(tQWORD*)par_buf;
					tHASH_DATA* pData=0;
					tDWORD size=sizeof(pData);
					
					if( NULL != CALL_SYS_PropertyGetObj(send_point,propid_object_executable_parent))
						qwHash ^= (tQWORD)1;

					if(HashCont && PR_SUCC(error=CALL_HashContainer_GetHash(HashCont,qwHash,(tPTR*)&pData)))
					{
						if(PR_SUCC(error=CALL_SYS_SendMsg(send_point,pmc_ENGINE_IO,pm_ENGINE_IO_HASH_FRAME_FOUND, _this, &pData, &size))){
							if(pData==0)  // was clean
							{
								CALL_SYS_SendMsg(send_point,pmc_ENGINE_IO,pm_ENGINE_IO_HC_CLEAN, _this, 0, 0);
								error=errENGINE_ACTION_SKIP;  //means no processing
								break;
							}
							else  // was infected
							{
								hOBJECT deletable_parent=0;
								CALL_SYS_PropertySetDWord(send_point,propid_detect_type,pData->detect_type);
								CALL_SYS_PropertySetDWord(send_point,propid_detect_certanity, pData->detect_certanity);
								CALL_SYS_PropertySetDWord(send_point,propid_detect_danger, pData->detect_danger);
								CALL_SYS_PropertySetDWord(send_point,propid_detect_behaviour, pData->detect_behaviour);
								CALL_SYS_PropertySetDWord(send_point,propid_detect_disinfectability, pData->detect_disinfectability);
								CALL_SYS_PropertySetStr(send_point,0,propid_detect_name,pData->detect_name,(tDWORD)strlen(pData->detect_name)+1,cCP_ANSI);

								IncrementInfected(send_point,pData->detect_counter);
									
								error=CALL_SYS_SendMsg(send_point,pmc_ENGINE_IO,pm_ENGINE_IO_HC_DETECT, _this, pData->io, 0);
								if(!(ENGINE_ACTION_CLASS_OBJECT_MODIFY & CALL_SYS_PropertyGetDWord(send_point,propid_action_class_mask)))
								{
									error=errENGINE_ACTION_SKIP;
									break;
								}

								if(error!=errOK_DECIDED)
								{
									if(error==errENGINE_HC_NO_OVERWRITE)
										pData->type=AVPMGR_HASH_DATA_DETECTED;
									else
									{
										error=errENGINE_ACTION_SKIP;
										break;
									}
								}

								switch(pData->type)
								{
								case AVPMGR_HASH_DATA_DETECTED_MODIFIED:
									error=Engine_SetWriteAccess(send_point,&deletable_parent);
									if(PR_SUCC(error))
										error=CopyIOContent((hIO)send_point,pData->io);
									if(PR_SUCC(error))
									{
										DecrementInfected(send_point, pData->detect_counter - pData->detect_state,cFALSE);
										CALL_SYS_SendMsg(send_point,pmc_ENGINE_IO,pm_ENGINE_IO_HC_OVERWRITED, _this, 0, 0);
									}
									else
										CALL_SYS_SendMsg(send_point,pmc_ENGINE_IO,pm_ENGINE_IO_HC_OVERWRITE_FAILED, _this, 0, 0);
									error=errENGINE_ACTION_SKIP;
									break;
									
								case AVPMGR_HASH_DATA_DETECTED_VOLUME:
									CALL_SYS_PropertySetDWord( send_point, propid_object_readonly_error,  errOBJECT_WRITE_NOT_SUPPORTED);
									CALL_SYS_PropertySetObj(   send_point, propid_object_readonly_object, send_point);
//									CALL_SYS_SendMsg(send_point,pmc_ENGINE_IO,pm_ENGINE_IO_HC_OVERWRITE_FAILED, _this, 0, 0);
									error=errENGINE_ACTION_SKIP;
									break;
								
								case AVPMGR_HASH_DATA_DETECTED_DELETED:
									Engine_SetWriteAccess(send_point,&deletable_parent);
									if( send_point == deletable_parent )
									{
										CALL_SYS_PropertySetBool(send_point,pgOBJECT_DELETE_ON_CLOSE,cTRUE);
										CALL_SYS_SendMsg(send_point,pmc_ENGINE_IO,pm_ENGINE_IO_HC_MARKED_FOR_DELETE, _this, 0, 0);
									}
									else
										CALL_SYS_SendMsg(send_point,pmc_ENGINE_IO,pm_ENGINE_IO_HC_OVERWRITE_FAILED, _this, 0, 0);
									error=errENGINE_ACTION_SKIP;
									break;

								case AVPMGR_HASH_DATA_DETECTED:
								default:
									if( PID_TMPFILE != OBJ_PID(send_point) )
									{
										error=Engine_SetWriteAccess(send_point,&deletable_parent);
										if(PR_SUCC(error))
										{
											DecrementInfected(send_point, pData->detect_counter, cTRUE);
											break;  //means process again - probably can be disinfected
										}
									}
									CALL_SYS_SendMsg(send_point,pmc_ENGINE_IO,pm_ENGINE_IO_HC_OVERWRITE_FAILED, _this, 0, 0);
									error=errENGINE_ACTION_SKIP;
									break;
								}
							}
						}
					}

					if(error != errENGINE_ACTION_SKIP)
					{
						CALL_SYS_SendMsg(send_point,pmc_ENGINE_IO,pm_ENGINE_IO_HASH_FRAME_BEGIN,_this,0,0);
						error=CALL_SYS_PropertySetQWord(send_point,propid_os_hash,qwHash);
					}
				}
				break;

		case pm_AVPMGR_INTERNAL_IO_HASH_FRAME_LEAVE:
			if(PR_SUCC(CALL_SYS_SendMsg(send_point,pmc_ENGINE_IO,pm_ENGINE_IO_HASH_FRAME_LEAVE, _this, 0, 0)))
				HC_UpdateAllData(send_point);
			break;

		case pm_AVPMGR_INTERNAL_IO_PROCESS:
#ifdef _PRAGUE_TRACE_
			CALL_SYS_TraceLevelSet(send_point, tlsTHIS_OBJECT, dwTraceLevel, prtMIN_TRACE_LEVEL);
#endif
			CALL_SYS_PropertyFree  (send_point, propid_postprocess_list );
			CALL_SYS_PropertyFree  (send_point, propid_os_offset_list);
			CALL_SYS_PropertyDelete(send_point, propid_object_skip );
			
			CALL_SYS_PropertyDelete(send_point, propid_hash_temp_list );
			CALL_SYS_PropertyDelete(send_point, propid_detect_state );
			CALL_SYS_PropertyDelete(send_point, propid_detect_counter );
			CALL_SYS_PropertyDelete(send_point, propid_detect_type );
			CALL_SYS_PropertyDelete(send_point, propid_detect_certanity );
			CALL_SYS_PropertyDelete(send_point, propid_detect_danger );
			CALL_SYS_PropertyDelete(send_point, propid_detect_behaviour );
			CALL_SYS_PropertyDelete(send_point, propid_detect_disinfectability );
			CALL_SYS_PropertyDelete(send_point, propid_detect_name );
			CALL_SYS_PropertyDelete(send_point, propid_object_infected_parent );

			CALL_SYS_PropertyDelete(send_point, propid_os_offset );
			CALL_SYS_PropertyDelete(send_point, propid_os_hash );
			CALL_SYS_PropertyDelete(send_point, propid_processing_error );
			CALL_SYS_PropertyDelete(send_point, propid_password_node);
			CALL_SYS_PropertyDelete(send_point, propid_object_os_passed );


			error=EngineAction( _this, send_point, _this->data->Entry_IO );
			break;

		case pm_AVPMGR_INTERNAL_IO_REGISTER_FOR_POSTPROCESS:
			{
				hLIST postprocess_list=(hLIST)CALL_SYS_PropertyGetObj(send_point,propid_postprocess_list);
				if(!postprocess_list){
					error=CALL_SYS_ObjectCreateQuick(send_point, &postprocess_list, IID_LIST, PID_ANY, 0);
					if(PR_SUCC(error)) error=CALL_SYS_PropertySetObj(send_point,propid_postprocess_list,(hOBJECT)postprocess_list);
				}
#ifdef _PRAGUE_TRACE_
				CALL_SYS_TraceLevelSet(ctx, tlsTHIS_OBJECT, dwTraceLevel, prtMIN_TRACE_LEVEL);
#endif
				if(postprocess_list) error=CALL_List_Add(postprocess_list,0,&ctx,sizeof(hOBJECT),0,cLIST_LAST);
				else
					error=errOBJECT_NOT_FOUND;
				if (PR_SUCC(error))
					PR_TRACE((_this, prtIMPORTANT, "Object [%08X] registered for postprocessing on [%08X]", ctx, send_point));
				else
					PR_TRACE((_this, prtERROR, "Failed to register for postprocess object [%08X], %terr", ctx, error));
			}
			break;
			
		case pm_AVPMGR_INTERNAL_IO_OS_OFFSET_FLUSH:{
				hOBJECT offset_list=CALL_SYS_PropertyGetObj(send_point,propid_os_offset_list);
				if(offset_list){
					error=CALL_SYS_PropertySetObj(send_point,propid_os_offset_list,0);
					CALL_SYS_ObjectClose(offset_list);
				}
			}
			break;
			
		case pm_AVPMGR_INTERNAL_IO_OS_OFFSET_REGISTER:
			{
				tQWORD par_off=0;
				hLIST offset_list=(hLIST)CALL_SYS_PropertyGetObj(send_point,propid_os_offset_list);

				if(par_buf){
					if(par_buf_len){
						switch(*par_buf_len){
						case sizeof(tQWORD):
							par_off=*(tQWORD*)par_buf;
							break;
						case sizeof(tDWORD):
							par_off=*(tDWORD*)par_buf;
							break;
						default:
							break;
						}
					}
					else par_off=(tDWORD)par_buf;
				}
				else{
					par_off=CALL_SYS_PropertyGetQWord( send_point, propid_os_offset);
					CALL_SYS_PropertyDelete( send_point, propid_os_offset);					
				}

				if(!offset_list){
					error=CALL_SYS_ObjectCreateQuick(send_point, &offset_list, IID_LIST, PID_ANY, 0);
					if(PR_SUCC(error)) error=CALL_SYS_PropertySetObj(send_point,propid_os_offset_list,(hOBJECT)offset_list);
				}

				if(offset_list) error=CALL_List_Add(offset_list,0,&par_off,sizeof(par_off),0,cLIST_LAST);
				
				if(par_off)
				{
					hOBJECT integral_parent_io=CALL_SYS_PropertyGetObj(send_point,propid_integral_parent_io);
					if(integral_parent_io){
						tQWORD os_size=0;
						tQWORD parent_size=0;
						CALL_IO_GetSize((hIO)integral_parent_io,&parent_size,IO_SIZE_TYPE_EXPLICIT);
						CALL_IO_GetSize((hIO)send_point,&os_size,IO_SIZE_TYPE_EXPLICIT);
						os_size -= par_off;
						if(parent_size>os_size){
							tQWORD parent_off = parent_size - os_size;
							tDWORD size;
							tQWORD parent_qw=0;
							tQWORD par_qw=0;
							CALL_IO_SeekRead((hIO)send_point,&size,par_off,&par_qw,sizeof(par_qw));
							CALL_IO_SeekRead((hIO)integral_parent_io,&size,parent_off,&parent_qw,sizeof(parent_qw));
							if( par_qw == parent_qw )
							{
								size=sizeof(parent_off);
								CALL_SYS_SendMsg(integral_parent_io,pmc_AVPMGR_INTERNAL,pm_AVPMGR_INTERNAL_IO_OS_OFFSET_REGISTER,_this,&parent_off,&size);
							}
						}
					}
				}
			}
			break;
			
		case pm_AVPMGR_INTERNAL_IO_PROCESS_ARCHIVE:
			{
				tQWORD par_off=0;
				if(par_buf){
					// get offset from parameters
					if(par_buf_len){
						switch(*par_buf_len){
						case sizeof(tQWORD):
							par_off=*(tQWORD*)par_buf;
							break;
						case sizeof(tDWORD):
							par_off=*(tDWORD*)par_buf;
							break;
						default:
							break;
						}
					}
					else par_off=(tDWORD)par_buf;
				}

				if( CALL_SYS_PropertyGetDWord(send_point,propid_object_infected_parent) )
				{
					//infected - should not be parsed. - stop
					if( par_buf_len )
						*par_buf_len=(tDWORD)-1;
					break;
				}

				if(PR_SUCC(error=CALL_SYS_PropertySetQWord( send_point, propid_os_offset, par_off)))
				{
					error=EngineAction( _this, send_point, _this->data->Entry_ARCHIVE );

					if(par_buf_len && 0==CALL_SYS_PropertySize( send_point, propid_os_offset))
					{
						//object system found and processed. - stop
						*par_buf_len=(tDWORD)-1;
		    			CALL_SYS_PropertySetBool(send_point, propid_object_os_passed, cTRUE );
					}
				}
			}			
			break;
		case pm_AVPMGR_INTERNAL_IO_SECTOR_OVERWRITE_REQUEST:
			{
				tDWORD cls=ENGINE_ACTION_CLASS_OBJECT_MODIFY;
				tDWORD len=sizeof(cls);
				
				if(0==(cls & CALL_SYS_PropertyGetDWord(send_point,propid_action_class_mask))){
					error=errENGINE_ACTION_SKIP;
					break;
				}
				if(PR_FAIL(error=CALL_SYS_PropertyGetDWord(send_point, propid_processing_error))){
					break;
				}
				if(CALL_SYS_PropertyGetDWord(send_point,pgOBJECT_ORIGIN)==OID_LOGICAL_DRIVE)
				{
					if(CALL_SYS_PropertyGetDWord(send_point,propid_boot_BytesPerSector))
						break; //floppy image - overwrite always

					if(CALL_SYS_PropertyGetBool(send_point,plIsPartition))
					{
						CALL_SYS_SendMsg(send_point,pmc_ENGINE_IO,pm_ENGINE_IO_DISINFECTION_IMPOSSIBLE,ctx,&error,&len);
						error=errENGINE_ACTION_SKIP;
					}
					break;
				}
				if(PR_FAIL(error=CALL_SYS_SendMsg(send_point,pmc_ENGINE_ACTION,ENGINE_ACTION_TYPE_SECTOR_OVERWRITE,ctx,&cls,&len))){
					break;
				}
			}
			break;
		case pm_AVPMGR_INTERNAL_IO_DISINFECT_REQUEST:
			{
				tDWORD cls=ENGINE_ACTION_CLASS_OBJECT_MODIFY;
				tDWORD len=sizeof(tDWORD);
				hOBJECT deletable_parent=0;
				tDWORD disinfectability=0;

				if(0==(cls & CALL_SYS_PropertyGetDWord(send_point,propid_action_class_mask))){
					error=errENGINE_ACTION_SKIP;
					break;
				}
				if(PR_FAIL(error=CALL_SYS_SendMsg(send_point,pmc_ENGINE_ACTION,pm_ENGINE_ACTION_DISINFECT,ctx,&cls,&len)))
					break;
				
				if(error!=errOK_DECIDED)
				{
					CALL_Engine_GetAssociatedInfo((hi_Engine)ctx,send_point,ENGINE_DETECT_DISINFECTABILITY,0,&disinfectability,sizeof(tDWORD),0);
					switch(disinfectability)
					{
					case ENGINE_DETECT_DISINFECTABILITY_CLEAN:
						error=errENGINE_ACTION_SKIP;
						break;
						
					case ENGINE_DETECT_DISINFECTABILITY_YES_BY_DELETE:
					case ENGINE_DETECT_DISINFECTABILITY_NO:
						switch(CALL_SYS_PropertyGetDWord(send_point,pgOBJECT_ORIGIN))
						{
						case OID_LOGICAL_DRIVE:
						case OID_PHYSICAL_DISK:
							error=Engine_SetWriteAccess(send_point,&deletable_parent);
							if(PR_FAIL(error))
								CALL_SYS_SendMsg(send_point,pmc_ENGINE_IO,pm_ENGINE_IO_DISINFECTION_IMPOSSIBLE,ctx,&error,&len);
							break;
						default:
							CALL_SYS_SendMsg(send_point,pmc_ENGINE_IO,pm_ENGINE_IO_DISINFECTION_IMPOSSIBLE,ctx,&error,&len);
							error=errENGINE_ACTION_SKIP;
							break;
						}
						break;
						
						case ENGINE_DETECT_DISINFECTABILITY_UNKNOWN:
						case ENGINE_DETECT_DISINFECTABILITY_YES:
						default:
							error=Engine_SetWriteAccess(send_point,&deletable_parent);
							if(PR_FAIL(error))
								CALL_SYS_SendMsg(send_point,pmc_ENGINE_IO,pm_ENGINE_IO_DISINFECTION_IMPOSSIBLE,ctx,&error,&len);
							break;
					}
				}

				if(PR_SUCC(error))
				{
					tDWORD origin=CALL_SYS_PropertyGetDWord(send_point,pgOBJECT_ORIGIN);
					switch(origin)
					{
					case OID_LOGICAL_DRIVE:
					case OID_PHYSICAL_DISK:
						{
							hBTIMAGES hBTImages;
							if(PR_SUCC(CreateBtImages(_this,&hBTImages)))
							{
								error=CALL_BtImages_Restore(hBTImages,send_point,BTIMAGES_LAST);
								CALL_SYS_ObjectClose(hBTImages);
							}
							else
								error=errENGINE_ACTION_SKIP;
						}
						if(PR_SUCC(error))
						{
							CALL_SYS_SendMsg(send_point,pmc_ENGINE_IO,pm_ENGINE_IO_DISINFECTED,_this,0,0);
							DecrementInfected(send_point,0,cFALSE);
							error=errENGINE_IO_MODIFIED;
						}
						else
							error=errOK;
						break;
					default:
						break;
					}	
				}
			}
			break;

		case pm_AVPMGR_INTERNAL_IO_REGISTER_PASSWORD:
			if(_this->data->PasswordTableCS)
			{
				if(PR_SUCC(CALL_CriticalSection_Enter(_this->data->PasswordTableCS,SHARE_LEVEL_WRITE)))
				{
					hSTRING str=_this->data->PasswordTable[_this->data->PasswordTableIndex];
					if(!str)
					{
						error=CALL_SYS_ObjectCreateQuick(_this->data->PasswordTableCS, &str, IID_STRING,PID_ANY,SUBTYPE_ANY );
						if(PR_SUCC(error))
							_this->data->PasswordTable[_this->data->PasswordTableIndex]=str;
					}
					if(PR_SUCC(error))
					{
						if(ctx)
							error=CALL_String_ImportFromStr(str,0,(hSTRING)ctx,cSTRING_WHOLE);
						else if(par_buf)
							error=CALL_String_ImportFromBuff(str,0,par_buf,(tDWORD)strlen(par_buf),cCP_ANSI,cSTRING_Z);
						else
							error=errPARAMETER_INVALID;

						if(PR_SUCC(error))
						{
							tDWORD i=0;
							for(i=0;i<PASSWORD_TABLE_SIZE;i++)
							{
								if(i ==  _this->data->PasswordTableIndex)
									continue;
								if(0 == _this->data->PasswordTable[i])
									break;

								if(PR_SUCC(CALL_String_Compare( str, cSTRING_WHOLE, _this->data->PasswordTable[i], cSTRING_WHOLE, 0 )))
								{
									_this->data->PasswordTableIndexLast=i;
									error=errNOT_OK;
									break;
								}
							}

							if(PR_SUCC(error))
							{
								_this->data->PasswordTableIndexLast=_this->data->PasswordTableIndex;
								if(++_this->data->PasswordTableIndex == PASSWORD_TABLE_SIZE)
									_this->data->PasswordTableIndex=0;
							}
						}
					}
					CALL_CriticalSection_Leave(_this->data->PasswordTableCS,NULL);
				}
			}
			break;

			
		default:
			break;
		}
		
		if(PR_SUCC(error)){
			error=errOK_DECIDED;
		}
		
		break;
		
//==============================================================
#ifdef KLAVPMGR
	case pmc_KLAVPMGR_REQUEST:
		switch(msg_id) {
		case pm_KLAVPMGR_REQUEST_PROCESS_ACTION:
		{
			KLAVPMGR_ACTION_REQUEST *req = (KLAVPMGR_ACTION_REQUEST *)par_buf;
			switch (req->type)
			{
			case ENGINE_ACTION_TYPE_SUBENGINE_PROCESS:
				error = KLAV_AVPMGR_ProcessSubEngine ((hOBJECT)_this, req->pid, send_point);
				break;
			case ENGINE_ACTION_TYPE_PROCESS_OS_ON_IO:
				if(CALL_SYS_PropertyGetDWord(send_point,propid_object_infected_parent) == 0)
				{
					tACTION_ProcessOSonIO action;
					action.type = ENGINE_ACTION_TYPE_PROCESS_OS_ON_IO;
					action.cls = req->cls;
				#ifdef _PRAGUE_TRACE_
					action.name = req->name;
				#endif
					action.pid = req->pid;
					action.child_origin = req->child_origin;
					action.mimi_pid = req->mini_pid;
					action.multivolume = (req->flags & KLAVPMGR_F_MULTIVOLUME) == 0 ? 0 : 1;

					error = EngineActionProcessOSonIO (_this, send_point, &action);
				} break;
			case ENGINE_ACTION_TYPE_CONVERT_IO: {
					tACTION_ConvertIO action;
					action.type = ENGINE_ACTION_TYPE_CONVERT_IO;
					action.cls = req->cls;
				#ifdef _PRAGUE_TRACE_
					action.name = req->name;
				#endif
					action.pid = req->pid;
					action.child_origin = 0;
					action.converter_pid = req->mini_pid;

					error = EngineActionConvertIO (_this, send_point, &action);
				} break;
			default:
				error = errPARAMETER_INVALID;
				break;
			};
		}
		break;
	}
#endif // KLAVPMGR
//==============================================================
	default:
		break;
	}
	
	PR_TRACE_A1( _this, "Leave \"Engine::MsgReceive\" method, ret %terr", error );
	return error;
}
// AVP Prague stamp end

tERROR HandleProcessingError( hi_Engine _this, hOBJECT obj, tERROR error)
{
	if(PR_FAIL(error)){
		tDWORD l;
		tERROR handled_error=errOK;
		
		if(OBJ_IID(obj)!=IID_OBJPTR)
			handled_error=CALL_SYS_PropertyGetDWord(obj, propid_handled_error);

		switch(error)
		{
		case errENGINE_OS_VOLUME_SET_INCOMPLETE: 
		case errENGINE_ACTION_SKIP: 
			//internal. it is ok.
			error=errOK;
			break;
		case errENGINE_OS_STOPPED:
			//internal. leave it as is.
			break;
		case errENGINE_IO_MODIFIED:
		case errOPERATION_CANCELED:
			//internal. leave it as is.
			break;
		case warnOBJECT_DATA_CORRUPTED :
		case errOBJECT_DATA_CORRUPTED:
			if(handled_error == error)
				error=errOK;
			else
			{
				CALL_SYS_PropertySetDWord(obj, propid_handled_error, error);
				error=CALL_SYS_SendMsg(obj,pmc_ENGINE_IO,pm_ENGINE_IO_CORRUPTED,_this,0,0);
			}
			break;
		case errOBJECT_PASSWORD_PROTECTED:
			if(handled_error == error)
				error=errOK;
			else
			{
				hOBJECT obj_main=CALL_SYS_PropertyGetObj(obj, propid_object_main);
				if(obj_main && obj_main!=obj)
				{
					if(PR_SUCC(CALL_SYS_PropertyGetDWord(obj_main, propid_processing_error)))
					{
						CALL_SYS_PropertySetDWord(obj_main, propid_processing_error, error);
						CALL_SYS_PropertySetDWord(obj_main, propid_handled_error, error);
					}
				}
				obj_main=CALL_SYS_ParentGet(obj,IID_OS);
				if(obj_main)
				{
					if(PR_SUCC(CALL_SYS_PropertyGetDWord(obj_main, propid_processing_error)))
						CALL_SYS_PropertySetDWord(obj_main, propid_processing_error, error);
				}
				
				CALL_SYS_PropertySetDWord(obj, propid_processing_error, error);
				CALL_SYS_PropertySetDWord(obj, propid_handled_error, error);
				error=CALL_SYS_SendMsg(obj,pmc_ENGINE_IO,pm_ENGINE_IO_PASSWORD_PROTECTED,_this,0,0);
			}
			break;
		default:
			if(handled_error == error)
				error=errOK;
			else
			{
				hOBJECT obj_main=CALL_SYS_PropertyGetObj(obj, propid_object_main);
				if(obj_main && obj_main!=obj)
				{
					if(PR_SUCC(CALL_SYS_PropertyGetDWord(obj_main, propid_processing_error)))
					{
						CALL_SYS_PropertySetDWord(obj_main, propid_processing_error, error);
						CALL_SYS_PropertySetDWord(obj_main, propid_handled_error, error);
					}
				}
				CALL_SYS_PropertySetDWord(obj, propid_processing_error, error);
				CALL_SYS_PropertySetDWord(obj, propid_handled_error, error);
				CALL_SYS_SendMsg(obj,pmc_ENGINE_IO,pm_ENGINE_IO_MAKE_VIRTUAL_NAME,_this,0,0);
				l=sizeof(error);
				error=CALL_SYS_SendMsg(obj,pmc_ENGINE_IO ,pm_ENGINE_IO_PROCESSING_ERROR,_this,&error,&l);
			}
			break;
		}
	}
	return error;
}


#define cREOPEN_MAGIC 0xAD7777AD

typedef struct ReopenRecord
{
	tDWORD    size;
	tPROPID   propid;
	tIID      iid;
	tPID      pid;
	tCODEPAGE cp;
	tBOOL     multivolume;
	tBYTE     data[];
}
tReopenRecord;

typedef struct ReopenHeader
{
	tDWORD    size;
	tDWORD    count;
	tDWORD    magic;
}
tReopenHeader;

typedef struct ReopenNode
{
	tDWORD          user_data;
	tBOOL           leaf;
	tQWORD          next_offset;
	tQWORD          child_offset;
	tReopenRecord   record;
}
tReopenNode;


tERROR AddObjectReopenData(hOBJECT obj, tReopenHeader** rh ){
	tERROR error=errOK;
	tPROPID prop=pgOBJECT_REOPEN_DATA;
	tDWORD size=0;

	error=CALL_SYS_PropertyGetDWord(obj,propid_object_readonly_error);
	if(PR_FAIL(error))
		return error;

	if(OBJ_PID(obj)==PID_TMPFILE)
	{
		switch(CALL_SYS_PropertyGetDWord(obj,pgOBJECT_ORIGIN))
		{
		case OID_AVP3_DOS_MEMORY:
			prop=pgOBJECT_ORIGIN;
			break;
		default:
			return errOBJECT_INCOMPATIBLE;
		}
		
	}

	size=CALL_SYS_PropertySize(obj,prop);
	if(!size &&	PR_SUCC(CALL_SYS_ObjectCheck( obj,obj,IID_IO,0,0,0 ))){
		prop=pgOBJECT_FULL_NAME;
		size=CALL_SYS_PropertySize(obj,prop);
	}
	if(!size)
		return errOBJECT_INCOMPATIBLE;
	
	if(PR_SUCC(error)){
		if(PR_SUCC(error=CALL_SYS_ObjHeapRealloc(obj, (tPTR*)rh, *(tPTR*)rh, (*rh)->size + size + sizeof(tReopenRecord))))
		{
			tReopenRecord* rr=(tReopenRecord*)((tBYTE*)*rh + (*rh)->size);
			(*rh)->count++;
			(*rh)->size += size + sizeof(tReopenRecord);
			rr->size  = size;
			rr->propid= prop; 
			rr->cp    = 0;
			rr->iid   = CALL_SYS_PropertyGetDWord(obj,pgINTERFACE_ID); 
			rr->pid   = CALL_SYS_PropertyGetDWord(obj,pgPLUGIN_ID); 

			// залипон
			#define PID_MKAVIO  ((tPID)(61015))
			if(rr->pid == PID_MKAVIO)
				rr->pid = PID_WIN32_NFIO;

			if(prop==pgOBJECT_FULL_NAME)
			{
				rr->cp= CALL_SYS_PropertyGetDWord(obj,pgOBJECT_FULL_NAME_CP); 
				if(rr->cp==0)
					rr->cp=cCP_ANSI;
				error=CALL_SYS_PropertyGetStr(obj,0,prop,rr->data,size,rr->cp);
			}
			else
				error=CALL_SYS_PropertyGet(obj,0,prop,rr->data,size);

			if(PR_SUCC(CALL_SYS_ObjectCheck( obj,obj,IID_OS,0,0,0 )))
				rr->multivolume=CALL_SYS_PropertyGetBool(obj, pgMULTIVOL_AS_SINGLE_SET);
		}
	}
	return error;
}

tERROR ObjectReopen(hOBJECT parent, hOBJECT* obj, tReopenRecord* rr, tPTR rr_data)
{
	tERROR error=errOK;
	
	if(rr->propid==pgOBJECT_FULL_NAME && PR_SUCC(CALL_SYS_ObjectCheck( parent,parent,IID_OS,0,0,0 )))
	{
		hSTRING str;
		if(PR_SUCC(error=CALL_SYS_ObjectCreateQuick(parent, &str, IID_STRING,PID_ANY,SUBTYPE_ANY )))
		{
			if(PR_SUCC(error=CALL_String_ImportFromBuff(str,0,rr_data,rr->size,rr->cp,0)))
				error=CALL_OS_IOCreate((hOS)parent,(hIO*)obj,(hOBJECT)str,fACCESS_RW,fOMODE_OPEN_IF_EXIST);
			CALL_SYS_ObjectClose(str);
		}
	}
	else
	{
		if(PR_SUCC(error=CALL_SYS_ObjectCreate( parent, obj, rr->iid, rr->pid, 0 ))) 
		{
			if ((rr->pid == PID_WIN32_NFIO) && (IID_IO == rr->iid))
			{
				CALL_SYS_PropertySetDWord(*obj, pgOBJECT_ACCESS_MODE, fACCESS_READ | fACCESS_FORCE_READ | fACCESS_FORCE_OPEN);
				CALL_SYS_PropertySetDWord(*obj, pgOBJECT_OPEN_MODE, fOMODE_OPEN_IF_EXIST | fOMODE_OPEN_IF_EXECUTABLE);
				PR_TRACE((g_root, prtIMPORTANT, "force access on reopen"));
			}

			if(rr->multivolume)
					CALL_SYS_PropertySetBool(*obj, pgMULTIVOL_AS_SINGLE_SET, cTRUE);

			if(rr->cp)
				error=CALL_SYS_PropertySetStr(*obj,0,rr->propid,rr_data,rr->size,rr->cp);
			else
				error=CALL_SYS_PropertySet(*obj,0,rr->propid,rr_data,rr->size);

			
			if(PR_FAIL(error) || PR_FAIL(error=CALL_SYS_ObjectCreateDone( *obj)))
			{
				HandleProcessingError(NULL, (hOBJECT)*obj, error);
				CALL_SYS_ObjectClose(*obj);
				*obj=0;
				return error;
			}
		}
	}
	HandleProcessingError(NULL, (hOBJECT)parent, error);
	return error;
}

tERROR IO_ObjectReopen( hOBJECT obj, hIO io, tQWORD offset, hOBJECT* obj_new, tReopenNode* rn)
{
	tERROR error=errOK;
	tDWORD size;

	error = CALL_IO_SeekRead(io,&size,offset,rn,sizeof(tReopenNode));
	if(size != sizeof(tReopenNode))
		error=errPARAMETER_INVALID;
	if(PR_SUCC(error))
	{
		tPTR r_data;
		error=CALL_SYS_ObjHeapAlloc(obj,&r_data,rn->record.size);
		if(PR_SUCC(error))
		{
			error = CALL_IO_SeekRead(io,&size,offset+sizeof(tReopenNode),r_data,rn->record.size);
			if(PR_SUCC(error))
				if(PR_FAIL(ObjectReopen(obj,obj_new,&rn->record,r_data)))
					*obj_new = NULL;
				else if(rn->user_data) 
					CALL_SYS_PropertySetDWord(*obj_new,propid_reopen_user_data,rn->user_data);

			CALL_SYS_ObjHeapFree(obj,r_data);
		}
	}
	return error;
}

tERROR EngineActionProcessOS( hi_Engine _this, hOBJECT obj, tACTION_ProcessOSonIO* action, hOS p_osystem);

tERROR ProcessChild( hi_Engine _this, hOBJECT obj, tBOOL b_makeheap)
{
	tERROR error=errOBJECT_CANNOT_BE_INITIALIZED;
	hIO reopen_io=(hIO)CALL_SYS_PropertyGetObj(obj,propid_reopen_data_io);
	tQWORD offset=CALL_SYS_PropertyGetQWord(obj,propid_reopen_data_offset);
	tDWORD size=0;
	
	if(!reopen_io) //this is root
	{
		tDWORD magic=0;
		tDWORD size=0;
		reopen_io=(hIO)obj;
		error = CALL_IO_SeekRead((hIO)obj,&size,0,&magic,sizeof(magic));
		if( magic != cREOPEN_MAGIC )
			error=errPARAMETER_INVALID;
		else
			offset=size;
	}
	else{
		tReopenNode rn;
		error = CALL_IO_SeekRead(reopen_io,&size,offset,&rn,sizeof(tReopenNode));
		if(size != sizeof(tReopenNode))
			error=errPARAMETER_INVALID;
		if(PR_SUCC(error))
			offset=rn.child_offset;
	}

	while( offset )
	{
		tERROR  err=errOK;
		tReopenNode rn;
		hOBJECT obj_new;

		if(PR_SUCC(err=IO_ObjectReopen(obj,reopen_io,offset,&obj_new,&rn)))
		{
			if( obj_new )
			{
				CALL_SYS_PropertySetObj(obj_new,propid_reopen_data_io,(hOBJECT)reopen_io);
				CALL_SYS_PropertySetQWord(obj_new,propid_reopen_data_offset,offset);

				if(PR_SUCC(CALL_SYS_ObjectCheck( obj,obj_new,IID_OS,0,0,0 )))
				{
					err=EngineActionProcessOS( _this, obj, 0, (hOS)obj_new );
					//closed in EngineActionProcessOS
					//CALL_SYS_ObjectClose(obj_new);
				}
				else
				{
					tBOOL b_io=PR_SUCC(CALL_SYS_ObjectCheck( obj,obj_new,IID_IO,0,0,0 ));
					if(b_io)
					{
						if(b_makeheap)
						{
							if(!CALL_SYS_PropertyGetObj(obj_new,pgOBJECT_HEAP))
							{
								hOBJECT hHeap;
								error=CALL_SYS_ObjectCreateQuick(obj_new, &hHeap, IID_HEAP, PID_ANY, SUBTYPE_ANY);
								if(PR_SUCC(error))
									CALL_SYS_PropertySetObj(obj_new, pgOBJECT_HEAP, hHeap);
							}
						}

						CALL_SYS_PropertySetObj(obj_new,propid_object_parent_io,obj_new);
						CALL_SYS_SendMsg(obj_new,pmc_ENGINE_IO,pm_ENGINE_IO_MAKE_VIRTUAL_NAME,_this,0,0);
						err=CALL_SYS_SendMsg(obj_new,pmc_ENGINE_IO,pm_ENGINE_IO_NAME_CHECK,0,0,0);
						if(PR_SUCC(err)) err=CheckStackAvail(_this->data->StackProbe);
						if(PR_SUCC(err)) err=CALL_SYS_SendMsg(obj_new,pmc_ENGINE_IO,pm_ENGINE_IO_BEGIN,_this,0,0);
					}
					if(PR_SUCC(err))
					{
						if(rn.leaf)
							err=EngineAction( _this, obj_new, _this->data->Entry_OBJECT );
						else
							err=ProcessChild( _this, obj_new, 0);

						if(b_io)
							CALL_SYS_SendMsg(obj_new,pmc_ENGINE_IO,pm_ENGINE_IO_DONE,_this,0,0);
					}
					CALL_SYS_ObjectClose(obj_new);
				}
			}
			if( err!=errOPERATION_CANCELED )
				offset=rn.next_offset;
			else
				offset=0;
		}
		else
			offset=0;

		if(PR_SUCC(error))
			error=err;
	}
	return error;
}


tERROR AddNodeReopenRecord( hIO obj, tQWORD offset, tReopenRecord* rr, tDWORD count, tDWORD user_data)
{
	tERROR error = errOK;
	tReopenNode rn;
	tDWORD size;
	while(PR_SUCC(error = CALL_IO_SeekRead(obj,&size,offset,&rn,sizeof(rn))))
	{
		if(size != sizeof(rn)) //create root
		{
			memset(&rn,0,sizeof(tReopenNode));
			rn.user_data=user_data;
			memcpy(&rn.record, rr, sizeof(tReopenRecord));
			error=CALL_IO_SeekWrite(obj,&size,offset,&rn,sizeof(rn));
			if(PR_SUCC(error))
				error=CALL_IO_SeekWrite(obj,&size,offset+sizeof(tReopenNode),rr->data,rr->size);
			break;
		}

		if( !memcmp( &rn.record,rr,sizeof(tReopenRecord)))
		{
			tPTR ptr;
			tBOOL on=0;
			error=CALL_SYS_ObjHeapAlloc(obj,&ptr,rn.record.size);
			if(PR_SUCC(error))
			{
				error=CALL_IO_SeekRead(obj,&size,offset+sizeof(tReopenNode),ptr,rn.record.size);
				if(PR_SUCC(error))
					if( !memcmp( ptr,rr->data,rr->size))
						on=1;
				CALL_SYS_ObjHeapFree(obj,ptr);
			}
			if( on || PR_FAIL(error))
				break;
		}

		if(rn.next_offset)
			offset=rn.next_offset;
		else
		{
			error=CALL_IO_GetSize(obj,&rn.next_offset,IO_SIZE_TYPE_EXPLICIT);
			if(PR_SUCC(error))
				error=CALL_IO_SeekWrite(obj,&size,offset,&rn,sizeof(rn));
			if(PR_SUCC(error))
			{
				offset=rn.next_offset;
				memset(&rn,0,sizeof(tReopenNode));
				rn.user_data=user_data;
				memcpy(&rn.record, rr, sizeof(tReopenRecord));
				error=CALL_IO_SeekWrite(obj,&size,offset,&rn,sizeof(rn));
				if(PR_SUCC(error))
					error=CALL_IO_SeekWrite(obj,&size,offset + sizeof(tReopenNode),rr->data,rr->size);
			}
			break;
		}
	}
	
	if(PR_SUCC(error))
	{
		if(--count)
		{
			rr=(tReopenRecord*)(((tBYTE*)rr)+sizeof(tReopenRecord)+rr->size );
			if(rn.child_offset)
				error=AddNodeReopenRecord( obj, rn.child_offset, rr, count, user_data);
			else
			{
				error=CALL_IO_GetSize(obj,&rn.child_offset,IO_SIZE_TYPE_EXPLICIT);
				if(PR_SUCC(error))
					error=CALL_IO_SeekWrite(obj,&size,offset,&rn,sizeof(rn));
				if(PR_SUCC(error))
				{
					offset=rn.child_offset;
					memset(&rn,0,sizeof(tReopenNode));
					rn.user_data=user_data;
					memcpy(&rn.record, rr, sizeof(tReopenRecord));
					error=CALL_IO_SeekWrite(obj,&size,offset,&rn,sizeof(rn));
					if(PR_SUCC(error))
						error=CALL_IO_SeekWrite(obj,&size,offset + sizeof(tReopenNode),rr->data,rr->size);
					if(PR_SUCC(error))
						error=AddNodeReopenRecord( obj, offset, rr, count, user_data);
				}
			}
		}
		else
		{
			rn.leaf=cTRUE;
			rn.user_data=user_data;
			error=CALL_IO_SeekWrite(obj,&size,offset,&rn,sizeof(rn));
		}
	}
	return error;
}
 

tERROR ObjectReopenRecourse( hi_Engine _this, hOBJECT parent, tReopenRecord* rr, tDWORD level)
{
	tERROR error = errOK;
	hOBJECT obj;
	if(PR_SUCC(error=ObjectReopen(parent, &obj, rr, rr->data))){
		tBOOL b_os=PR_SUCC(CALL_SYS_ObjectCheck( parent,obj,IID_OS,0,0,0 ));
		if(b_os){
			hSTRING str;
			if(PR_SUCC(CALL_SYS_ObjectCreateQuick(obj, &str, IID_STRING,PID_ANY,SUBTYPE_ANY ))){
				if(PR_SUCC(CALL_String_ImportFromProp(str,0,obj,pgINTERFACE_COMMENT)))
					CALL_String_ExportToProp(str,0,cSTRING_WHOLE,parent,propid_npENGINE_OBJECT_OBTAINED_TRANSFORMER_NAME);
				CALL_SYS_ObjectClose(str);
			}
			CALL_SYS_SendMsg(parent,pmc_ENGINE_IO,pm_ENGINE_IO_OS_ENTER,_this,0,0);
		}else if(PR_SUCC(CALL_SYS_ObjectCheck( parent,obj,IID_IO,0,0,0 ))){
			CALL_SYS_SendMsg(obj,pmc_ENGINE_IO,pm_ENGINE_IO_MAKE_VIRTUAL_NAME,_this,0,0);
		}

		error=errNOT_OK;

		if(--level)
			error=ObjectReopenRecourse( _this, obj, (tReopenRecord*)((tBYTE*)rr + rr->size + sizeof(tReopenRecord)), level);
		
		if(PR_FAIL(error))
			error=EngineAction( _this, obj, _this->data->Entry_OBJECT );

		if(b_os){
			tDWORD l=sizeof(error);
			CALL_SYS_SendMsg(parent,pmc_ENGINE_IO,pm_ENGINE_IO_OS_LEAVE,_this,&error,&l);
			CALL_SYS_SendMsg(parent,pmc_ENGINE_IO,pm_ENGINE_IO_OS_PRECLOSE,obj,0,0);
		}
		
		error=CALL_SYS_ObjectClose( obj );
	}
	return error;
}

tERROR Do_Process( hi_Engine _this, hOBJECT obj )
{
	tERROR error = errOK;
	tDWORD action_class;
	hHASHCONTAINER HashCont=0;
	hOBJECT session_obj;
	tMsgHandlerDescr hdl[] = {
		{ (hOBJECT)_this, rmhDECIDER,  pmc_IO,                   IID_ANY, PID_ANY, IID_ANY, PID_ANY },
		{ (hOBJECT)_this, rmhDECIDER,  pmc_AVPMGR_INTERNAL,      IID_ANY, PID_ANY, IID_ANY, PID_ANY },
		{ (hOBJECT)_this, rmhLISTENER, pmc_OS_VOLUME,            IID_ANY, PID_ANY, IID_ANY, PID_ANY },
//==============================================================
#ifdef KLAVPMGR
		{ (hOBJECT)_this, rmhDECIDER,  pmc_KLAVPMGR_REQUEST,     IID_ANY, PID_ANY, IID_ANY, PID_ANY },
#endif
//==============================================================
	};
	
//	if( !(_this->data->DB_State & cENGINE_DB_STATE_FUNCTIONAL))
//		return errENGINE_CONFIGURATION_NOT_LOADED;
	
	error = CALL_SYS_RegisterMsgHandlerList( obj, hdl, countof(hdl) );
	if(PR_FAIL(error)) return error;

	CALL_SYS_PropertyFree  (obj, propid_postprocess_list );
	CALL_SYS_PropertyFree  (obj, propid_os_offset_list);
	CALL_SYS_PropertyDelete(obj, propid_object_skip );
	CALL_SYS_PropertyDelete(obj, propid_os_offset );
	CALL_SYS_PropertyDelete(obj, propid_os_hash );
	CALL_SYS_PropertyDelete(obj, propid_object_readonly_error );
	CALL_SYS_PropertyDelete(obj, propid_object_readonly_object );
	CALL_SYS_PropertyDelete(obj, propid_object_executable_parent );
	CALL_SYS_PropertyDelete(obj, propid_processing_error );
	CALL_SYS_PropertyFree  (obj, propid_password_list);
	CALL_SYS_PropertyDelete(obj, propid_password_node);
	CALL_SYS_PropertyDelete(obj, propid_hash_temp_list );
	CALL_SYS_PropertyDelete(obj, propid_detect_state );
	CALL_SYS_PropertyDelete(obj, propid_detect_counter );
	CALL_SYS_PropertyDelete(obj, propid_detect_type );
	CALL_SYS_PropertyDelete(obj, propid_detect_certanity );
	CALL_SYS_PropertyDelete(obj, propid_detect_danger );
	CALL_SYS_PropertyDelete(obj, propid_detect_behaviour );
	CALL_SYS_PropertyDelete(obj, propid_detect_disinfectability );
	CALL_SYS_PropertyDelete(obj, propid_detect_name );
	CALL_SYS_PropertyDelete(obj, propid_auto_password_bool);
	CALL_SYS_PropertyDelete(obj, propid_object_os_passed );


	
	action_class=CALL_SYS_PropertyGetDWord(obj,propid_action_class_mask);
	if(!action_class){
//		action_class=~ENGINE_ACTION_CLASS_OBJECT_MODIFY;
		action_class=-1;
		CALL_SYS_PropertySetDWord(obj,propid_action_class_mask,action_class);
	}

	CALL_SYS_PropertySetObj( obj, propid_object_main, obj);
	
	session_obj=CALL_SYS_PropertyGetObj( obj, propid_object_session	);
	if(session_obj == 0)
		session_obj=obj;
	else if(!CALL_SYS_PropertyGetObj(session_obj,pgOBJECT_HEAP)){
		hOBJECT hHeap;
		error=CALL_SYS_ObjectCreateQuick(session_obj, &hHeap, IID_HEAP, PID_ANY, SUBTYPE_ANY);
		if(PR_SUCC(error)) CALL_SYS_PropertySetObj(session_obj, pgOBJECT_HEAP, hHeap);
	}
		
	if(!CALL_SYS_PropertyGetObj(obj,pgOBJECT_HEAP)){
		hOBJECT hHeap;
		error=CALL_SYS_ObjectCreateQuick(obj, &hHeap, IID_HEAP, PID_ANY, SUBTYPE_ANY);
		if(PR_SUCC(error)) CALL_SYS_PropertySetObj(obj, pgOBJECT_HEAP, hHeap);
	}
				
	if(PR_SUCC(error)){
		HashCont=(hHASHCONTAINER)CALL_SYS_PropertyGetObj(session_obj, propid_hash_container);
		if(!HashCont){
			if(PR_SUCC(error)) error=CALL_SYS_ObjectCreateQuick(session_obj, &HashCont, IID_HASHCONTAINER, PID_ANY, SUBTYPE_ANY);
			if(PR_SUCC(error)) error=CALL_SYS_PropertySetObj(session_obj, propid_hash_container, (hOBJECT)HashCont);
		}
		if(PR_SUCC(error)) error=CALL_SYS_PropertySetObj(obj, propid_hash_container, (hOBJECT)HashCont);
	}

	if(PR_SUCC(error)) {
		tBOOL done=0;
		if(	IID_BUFFER==OBJ_IID(obj) && OID_REOPEN_DATA == CALL_SYS_PropertyGetDWord(obj,pgOBJECT_ORIGIN))
		{
			tReopenHeader* rh=0;
			if(PR_SUCC(error=CALL_Buffer_Lock((hBUFFER)obj,(tPTR*)&rh)))
			{
				error= ObjectReopenRecourse( _this, obj, (tReopenRecord*)(rh+1), rh->count);
				done=1;
				CALL_Buffer_Unlock((hBUFFER)obj);
			}
		}
		else if(IID_IO==OBJ_IID(obj)  && OID_REOPEN_DATA == CALL_SYS_PropertyGetDWord(obj,pgOBJECT_ORIGIN))
		{
			error = ProcessChild( _this, obj, cTRUE);
			done=1;
		}

		if(!done)
			error=EngineAction( _this, obj, _this->data->Entry_OBJECT );
	}

	if(PR_SUCC(error)){
		error=CALL_SYS_PropertyGetDWord(obj,propid_processing_error);
	}
	
	CALL_SYS_UnregisterMsgHandlerList( obj, hdl, countof(hdl) );
	
	return error;
}


// AVP Prague stamp begin( External (user called) interface method implementation, Process )
// Extended method comment
//  Processes object - scan, modify or change configuration.
// Behaviour comment
//  Should be reentrant, multithread safe.
//  Processing messages interesting for application and other plugins should be send from processing object with engine as context.
//
//  These error codes shoud be used by application in message handler for stopping:
//  errOPERATION_CANCELED
//  errOPERATION_COMPLETE
// Result comment
//  If Process return PR_FAIL all further processing shold be stopped.
//
// Parameters are:
tERROR pr_call Engine_Process( hi_Engine _this, hOBJECT obj )
{
	tERROR error = errOK;
	tDWORD count = sizeof(error);
//#define RESTRICT_TRACE
#if defined(_PRAGUE_TRACE_) && defined(RESTRICT_TRACE)
	tDWORD dwIOTraceLevel;
#endif

	tMsgHandlerDescr hdl[] = {
		{ (hOBJECT)_this, rmhDECIDER,  pmc_ENGINE_IO,            IID_ANY, PID_ANY, IID_ANY, PID_ANY },
	};
	
	PR_TRACE_A0( _this, "Enter \"Engine::Process\" method" );
	CALL_SYS_SendMsg(_this,pmc_ENGINE_PROCESS, pm_ENGINE_PROCESS_BEGIN,_this,&error,&count);

#if defined(_PRAGUE_TRACE_) && defined(RESTRICT_TRACE)
	CALL_SYS_TraceLevelGet(obj, tlsTHIS_OBJECT, &dwIOTraceLevel, NULL);
	CALL_SYS_TraceLevelSet(obj, tlsTHIS_OBJECT, dwTraceLevel, prtMIN_TRACE_LEVEL);
#endif
	error = CALL_SYS_RegisterMsgHandlerList( obj, hdl, countof(hdl) );
	if(PR_SUCC(error)){
			
		error=Do_Process( _this, obj );

		CALL_SYS_UnregisterMsgHandlerList( obj, hdl, countof(hdl) );
	}

#if defined(_PRAGUE_TRACE_) && defined(RESTRICT_TRACE)
	CALL_SYS_TraceLevelSet(obj, tlsTHIS_OBJECT, dwTraceLevel, prtMIN_TRACE_LEVEL);
#endif

	CALL_SYS_SendMsg(_this,pmc_ENGINE_PROCESS, pm_ENGINE_PROCESS_DONE,_this,&error,&count);
	PR_TRACE_A1( _this, "Leave \"Engine::Process\" method, ret %terr", error );
	return error;
}
// AVP Prague stamp end



	
// AVP Prague stamp begin( External (user called) interface method implementation, GetAssociatedInfo )
// Extended method comment
//  Returns runtime information about processing object.
//  Very similar to PropertyGet system function.
//  This function is to be called in MsgReceive handler.
//
// Behaviour comment
//  Should be reentrant, multithread safe.
//
// Result comment
//  Returns errNOT_FOUND if info absent or not exist.
// Parameters are:
tERROR pr_call Engine_GetAssociatedInfo( hi_Engine _this, hOBJECT obj, tDWORD info_id, tDWORD param, tPTR buffer, tDWORD size, tDWORD* out_size )
{
	tERROR error = errNOT_FOUND;
	PR_TRACE_A0( _this, "Enter \"Engine::GetAssociatedInfo\" method" );
	
	switch(info_id){
	case ENGINE_REOPEN_DATA:
		{
			tReopenHeader* rh=0;
			hOBJECT object=obj;
			hLIST hList;
			if(PR_SUCC(error = CALL_SYS_ObjectCreateQuick(_this, &hList, IID_LIST, PID_ANY, SUBTYPE_ANY)))
			{
				tLISTNODE node=0;
				hOBJECT _obj;
				if(_obj=CALL_SYS_PropertyGetObj(object,propid_object_executable_parent))
					object=_obj;
				while(object && PR_SUCC(error)){
					_obj=CALL_SYS_PropertyGetObj(object,propid_integral_parent_io);
					if(_obj==NULL)
					{
						error=CALL_List_Add(hList, NULL, &object, sizeof(hOBJECT), 0, cLIST_LAST);
						_obj=CALL_SYS_PropertyGetObj(object,pgOBJECT_BASED_ON);
					}
					if(_obj==object)
						error=errOBJECT_NOT_FOUND;
					else
						object=_obj;
				}

				if(PR_SUCC(error))
				{
					error=CALL_SYS_ObjHeapAlloc(obj,(tPTR*)&rh,sizeof(tReopenHeader));
					if(PR_SUCC(error))
					{
						rh->magic=cREOPEN_MAGIC;
						rh->size+=sizeof(tReopenHeader);
					}
				}

				while(PR_SUCC(error) && PR_SUCC(CALL_List_Last(hList,&node)) && node){
					if(PR_SUCC(error=CALL_List_DataGet(hList,0,node,&object,sizeof(object)))){
						error=AddObjectReopenData(object, &rh);
						CALL_List_Remove(hList,node);
					}
				}
				CALL_SYS_ObjectClose( hList );
			}

			if(error==errOBJECT_INCOMPATIBLE)
				error=errOK;

			if(PR_SUCC(error))
			{
				if(rh==0 || rh->size==0 || rh->size==sizeof(tReopenHeader))
					error=errNOT_FOUND;
				else
				{
					if(out_size)
						*out_size=rh->size;
					if(buffer==0)
						error=errOK;
					else if(rh->size > size)
						error=errBUFFER_TOO_SMALL;
					else
						memcpy(buffer,rh,rh->size);
				}
			}
			if(rh)
				CALL_SYS_ObjHeapFree(obj,rh);
		}

		break;
	case ENGINE_DETECT_DANGER:
		error=CALL_SYS_PropertyGet(obj,out_size,propid_detect_danger,buffer,size);
		break;
	case ENGINE_DETECT_BEHAVIOUR:
		error=CALL_SYS_PropertyGet(obj,out_size,propid_detect_behaviour,buffer,size);
		break;
	case ENGINE_DETECT_CERTANITY:
		error=CALL_SYS_PropertyGet(obj,out_size,propid_detect_certanity,buffer,size);
		break;
	case ENGINE_DETECT_DISINFECTABILITY:
		error=CALL_SYS_PropertyGet(obj,out_size,propid_detect_disinfectability,buffer,size);
		break;
	case ENGINE_DETECT_TYPE:
		error=CALL_SYS_PropertyGet(obj,out_size,propid_detect_type,buffer,size);
		break;
	case ENGINE_DETECT_NAME:
		error=CALL_SYS_PropertyGetStr(obj, out_size, propid_detect_name, buffer, size, param);
		break;
	case ENGINE_OBTAINED_OS_NAME:
		error= CALL_SYS_PropertyGetStr(obj, out_size, propid_npENGINE_OBJECT_OBTAINED_TRANSFORMER_NAME, buffer, size, param);
		break;
	default:
		break;
	}
	
	PR_TRACE_A1( _this, "Leave \"Engine::GetAssociatedInfo\" method, ret %terr", error );
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, GetExtendedInfo )
// Extended method comment
//  Returns information about engine abilities.
//  Very similar to PropertyGet system function, but has paramerer.
//
// Behaviour comment
//  Should be reentrant, multithread safe.
//
// Result comment
//  Returns errNOT_FOUND if info absent or not exist.
// Parameters are:
tERROR pr_call Engine_GetExtendedInfo( hi_Engine _this, tDWORD info_id, tDWORD param, tPTR buffer, tDWORD size, tDWORD* out_size )
{
	tERROR error =  errNOT_FOUND;
	PR_TRACE_A0( _this, "Enter \"Engine::GetExtendedInfo\" method" );

	// Place your code here

	PR_TRACE_A1( _this, "Leave \"Engine::GetExtendedInfo\" method, ret %terr", error );
	return error;
}
// AVP Prague stamp end



/* Defined but never used. Commented out.*/
/* static tERROR RegGetStringIdxValue( hREGISTRY reg, tRegKey key, tSTRING name, tDWORD ind, tSTRING out_buffer, tDWORD size ) */
/* { */
/* 	tTYPE_ID type; */
/* 	tCHAR buff[100]; */
/* 	pr_sprintf(buff, 100, "%s%d",name,ind); */
/* 	type=tid_STRING; */
/* 	return CALL_Registry_GetValue(reg,0,key,buff,&type,out_buffer,size); */
/* } */

/* Defined but never used. Commented out.*/
/* static tERROR RegGetDwordIdxValue( hREGISTRY reg, tRegKey key, tSTRING name, tDWORD ind, tDWORD* value ) */
/* { */
/* 	tTYPE_ID type; */
/* 	tCHAR buff[100]; */
/* 	pr_sprintf(buff, 100, "%s%d",name,ind); */
/* 	type=tid_DWORD; */
/* 	return CALL_Registry_GetValue(reg,0,key,buff,&type,value,sizeof(tDWORD)); */
/* } */

tERROR UnloadConfiguration( hi_Engine _this )
{
	tERROR error = errOK;
	_this->data->DB_State=0;
	_this->data->DB_Count=0;

//==============================================================
#ifdef KLAVPMGR
	if (_this->data->hKLAVPMGR != 0)
	{
		CALL_SYS_ObjectClose (_this->data->hKLAVPMGR);
		_this->data->hKLAVPMGR = 0;
	}
#endif // KLAVPMGR
//==============================================================

	while(_this->data->EngineCount){
		_this->data->EngineCount--;
		if(_this->data->EngineHandle[_this->data->EngineCount]) // if NOT Disabled
		{
			CALL_SYS_ObjectClose(_this->data->EngineHandle[_this->data->EngineCount]);
		}
	}

	if(_this->data->Actions){
		tDWORD i=_this->data->ActionsCount;
		while(i--){
			if(_this->data->Actions[i]){
				switch(_this->data->Actions[i]->type){
				default:
				case ENGINE_ACTION_TYPE_PROCESS_OS_ON_IO:
				case ENGINE_ACTION_TYPE_CONVERT_IO:
				case ENGINE_ACTION_TYPE_SUBENGINE_PROCESS:
				case ENGINE_ACTION_TYPE_NO_ACTION:
					break;
				case ENGINE_ACTION_TYPE_SET_OF_ACTIONS:
					{
						tACTION_SetOfActions* Action=((tACTION_SetOfActions*)_this->data->Actions[i]);
						if(Action->actions){
							CALL_SYS_ObjHeapFree(_this,Action->actions);
							Action->actions=0;
						}
					}
					break;
				case ENGINE_ACTION_TYPE_SWITCH_BY_PROPERTY:
					{
						tACTION_SwitchByProperty* Action=((tACTION_SwitchByProperty*)_this->data->Actions[i]);
						if(Action->actions){
							CALL_SYS_ObjHeapFree(_this,Action->actions);
							Action->actions=0;
						}
						if(Action->value){
							CALL_SYS_ObjHeapFree(_this,Action->value);
							Action->value=0;
						}
					}
					break;
				}
#ifdef _PRAGUE_TRACE_
				if(_this->data->Actions[i]->name)
					CALL_SYS_ObjHeapFree(_this,_this->data->Actions[i]->name);
#endif
				CALL_SYS_ObjHeapFree(_this,_this->data->Actions[i]);
				_this->data->Actions[i]=0;
			}
		}
		CALL_SYS_ObjHeapFree(_this,_this->data->Actions);
		_this->data->Actions=0;
	}

	if(_this->data->EnginePID){
		CALL_SYS_ObjHeapFree(_this,_this->data->EnginePID);
		_this->data->EnginePID=0;
	}
	if(_this->data->EngineHandle){
		CALL_SYS_ObjHeapFree(_this,_this->data->EngineHandle);
		_this->data->EngineHandle=0;
	}
	if(_this->data->hIChecker){
		error=CALL_IChecker_FlushStatusDB( (hICHECKER)(_this->data->hIChecker) );
	}

	CALL_SYS_SendMsg(_this,pmc_ENGINE_LOAD, pm_ENGINE_LOAD_CONFIGURATION_UNLOADED,0,0,0);
	
	return error;
}

typedef struct tag_NAMES_IDs_LIST_ITEM {
	tDWORD dwID;
	tCHAR  szName[0x100];
} NAMES_IDs_LIST_ITEM;

tDWORD iGetRealListItemSize(NAMES_IDs_LIST_ITEM* pListItem)
{
	return (tDWORD)((tBYTE*)&(pListItem->szName) - (tBYTE*)pListItem + strlen(pListItem->szName) + 1);
}

tERROR iGetNamesIDsList(hi_Engine _this, hREGISTRY hReg, tRegKey Key, hLIST* phNamesIDsList)
{
	tERROR error = errOK;
	hLIST hList = NULL;
	tDWORD dwCount = 0;
	tRegKey key2;

	if (PR_FAIL(error = CALL_SYS_ObjectCreateQuick(_this, &hList, IID_LIST, PID_ANY, SUBTYPE_ANY)))
		PR_TRACE((_this, prtERROR, "ObjectCreateQuick(IID_LIST) failed with %terr", error));

	if (PR_SUCC(error) && PR_SUCC(error=CALL_Registry_OpenKey(hReg, &key2, Key, "Actions", cFALSE)))
	{
		if (PR_FAIL(error = CALL_Registry_GetKeyCount(hReg, &dwCount, key2)))
			PR_TRACE((_this, prtERROR, "Registry_GetKeyCount failed with %terr", error));
		else
		if (dwCount == 0)
			PR_TRACE((_this, prtERROR, "KeyCount==0"));
		else
		{
			tDWORD i;
			NAMES_IDs_LIST_ITEM sListItem;

			for (i=0; i<dwCount; i++)
			{
				sListItem.dwID = i;
				if (PR_FAIL(error = CALL_Registry_GetKeyNameByIndex(hReg, NULL, key2, i, sListItem.szName, sizeof(sListItem.szName), cFALSE)))
				{
					PR_TRACE((_this, prtERROR, "CALL_Registry_GetKeyNameByIndex(%d) failed with %terr", i, error));
					break;
				}
				if (PR_FAIL(error = CALL_List_Add(hList, NULL, &sListItem, iGetRealListItemSize(&sListItem), 0, cLIST_LAST)))
				{
					PR_TRACE((_this, prtERROR, "List_Add failed with %terr", error));
					break;
				}
			}
		}
	}

	if (PR_SUCC(error) && PR_SUCC(error=CALL_Registry_OpenKey(hReg, &key2, Key, "Constants", cFALSE)))
	{
		if (PR_FAIL(error = CALL_Registry_GetValueCount(hReg, &dwCount, key2)))
			PR_TRACE((_this, prtERROR, "Registry_GetValueCount failed with %terr", error));
		else
		if (dwCount == 0)
			PR_TRACE((_this, prtERROR, "ValueCount==0"));
		else
		{
			tDWORD i;
			NAMES_IDs_LIST_ITEM sListItem;

			for (i=0; i<dwCount; i++)
			{
				tTYPE_ID type = tid_DWORD;
				if (PR_FAIL(error = CALL_Registry_GetValueNameByIndex(hReg, NULL, key2, i, &sListItem.szName[0], sizeof(sListItem.szName))))
				{
					PR_TRACE((_this, prtERROR, "Registry_GetValueNameByIndex(%d) failed with %terr", i, error));
					break;
				}
				if (PR_FAIL(error = CALL_Registry_GetValue(hReg, NULL, key2, &sListItem.szName[0], &type, &sListItem.dwID, sizeof(sListItem.dwID))))
				{
					PR_TRACE((_this, prtERROR, "Registry_GetValueNameByIndex(%d) failed with %terr", i, error));
					break;
				}
				if (PR_FAIL(error = CALL_List_Add(hList, NULL, &sListItem, iGetRealListItemSize(&sListItem), 0, cLIST_LAST)))
				{
					PR_TRACE((_this, prtERROR, "List_Add failed with %terr", error));
					break;
				}
			}
		}
	}

	if (PR_FAIL(error) && hList)
	{
		CALL_SYS_ObjectClose(hList);
		hList = NULL;
	}

	if (phNamesIDsList)
		*phNamesIDsList = hList;
	
	return error;
}

tERROR iGetIDFromName(hLIST hNamesIDsList, tCHAR* szName, tDWORD* pdwID)
{
	tERROR error;
	tLISTNODE node;
	tDWORD dwID;
	
	if (PR_FAIL(error = CALL_List_First(hNamesIDsList, &node)))
		PR_TRACE((hNamesIDsList, prtERROR, "List_First failed with %terr", error));
	else
	{
		NAMES_IDs_LIST_ITEM sListItem;
		do 
		{
			if (PR_FAIL(error = CALL_List_DataGet(hNamesIDsList, NULL, node, &sListItem, sizeof(sListItem))))
			{
				PR_TRACE((hNamesIDsList, prtERROR, "List_DataGet failed with %terr", error));
				break;
			}
			else
			{
				if (strcmp(sListItem.szName, szName) == 0)
				{
					dwID = sListItem.dwID;
					break;
				}
			}
		} while (PR_SUCC(error = CALL_List_Next(hNamesIDsList, &node, node)));
	}

	if (pdwID)
	{
		if (PR_FAIL(error))
			dwID = 0;
		*pdwID = dwID;
	}

	if (PR_FAIL(error))
		PR_TRACE((hNamesIDsList, prtERROR, "Cannot find ID for name: %s", szName));

	return error;
}

#define op_DELIMITER ':'
#define op_OR '|'
#define op_COMMENT '/'
#define op_EOL 0

tERROR iParseNames(hLIST hNamesIDsList, tCHAR* szNames, tDWORD* pdwIDs, tDWORD dwMaxIDsCount, tDWORD* pdwIDsCount)
{
	tERROR error;
	tCHAR* pName = szNames;
	tCHAR* pNextName;
	tCHAR* pTmp;
	tDWORD dwIDsCount = 0;
	tCHAR op;
	tDWORD dwID;

	// zero result array
	memset(pdwIDs, 0, dwMaxIDsCount*sizeof(tDWORD));

	do 
	{
		// find next name
		pNextName = pName-1;

		do {
			pNextName++;
			op = *pNextName;
		} while ( !(op==op_EOL || op==op_OR || op==op_DELIMITER || (op==op_COMMENT && *(pNextName)==op_COMMENT)) );

		// trim name
		while (*pName == ' ')
			pName++;
		pTmp = pNextName-1;
		while (pTmp > pName && *pTmp==' ')
		{
			*pTmp = 0;
			pTmp--;
		}

		if (op != op_EOL)
		{
			*pNextName = 0;
			pNextName++;
		}
	

		if (PR_FAIL(error = iGetIDFromName(hNamesIDsList, pName, &dwID)))
			break;

		pdwIDs[dwIDsCount] |= dwID;

		if (op != op_OR)
			dwIDsCount++;

		if (op==op_EOL || op==op_COMMENT)
			break;

		if (dwIDsCount == dwMaxIDsCount)
		{
			error = errBUFFER_TOO_SMALL;
			break;
		}

		pName = pNextName;
	} while (*pName);

	if (pdwIDsCount)
	{
		if (PR_FAIL(error))
			dwIDsCount = 0;
		*pdwIDsCount = dwIDsCount;
	}
	
	return error;
}

#ifdef _DEBUG
	tCHAR szIDsFromRegValue[0x100];
#endif

tERROR iGetIDsFromRegValue(hLIST hNamesIDsList, hREGISTRY hReg, tRegKey Key, tSTRING szValueName, tDWORD* pdwIDs, tDWORD dwMaxIDsCount, tDWORD* pdwIDsCount)
{
	tERROR error;
#ifndef _DEBUG
	tCHAR szIDsFromRegValue[0x100];
#endif
	tTYPE_ID type;
		
	type = tid_CHAR;
	if (PR_SUCC(error = CALL_Registry_GetValue(hReg,0,Key,szValueName,&type,&szIDsFromRegValue,sizeof(szIDsFromRegValue))))
		error = iParseNames(hNamesIDsList, szIDsFromRegValue, pdwIDs, dwMaxIDsCount, pdwIDsCount);

	return error;
}

tERROR iGetIDsFromRegValueIdx(hLIST hNamesIDsList, hREGISTRY hReg, tRegKey Key, tSTRING szValueName, tDWORD dwIndex, tDWORD* pdwIDs, tDWORD dwMaxIDsCount, tDWORD* pdwIDsCount)
{
	tERROR error;
	tCHAR buff[100];
	pr_sprintf(buff, sizeof(buff), "%s%d",szValueName,dwIndex);
	error = iGetIDsFromRegValue(hNamesIDsList, hReg, Key, buff, pdwIDs, dwMaxIDsCount, pdwIDsCount);
	return error;
}

tERROR iSetActionName(hi_Engine _this, tACTION* pAction, tCHAR* szActionName)
{
	tERROR error = errOK;
#ifdef _PRAGUE_TRACE_
	tSIZE_T nameSz = (tDWORD)strlen(szActionName)+1;
	if (PR_SUCC(error = CALL_SYS_ObjHeapAlloc(_this, &pAction->name, nameSz)))
	{
		strcpy_s(pAction->name, nameSz, szActionName);
	}
#endif // _PRAGUE_TRACE_
	return error;
}

// AVP Prague stamp begin( External (user called) interface method implementation, LoadConfiguration )
// Behaviour comment
//    Состояние можно узнать из свойства ENGINE_DB_STATE
//
// Result comment
//    Возвращает PR_SUCC если конфигурация успешно загружена
//
// Parameters are:
tERROR pr_call Engine_LoadConfiguration( hi_Engine _this, hREGISTRY reg, tRegKey key )
{
	tERROR error = errOK;
	tTYPE_ID type;
	tDWORD l=0;
	char name[0x200];
	tRegKey engines_key;
	tRegKey entries_key;
	hLIST hNamesIDsList=NULL;
	tDWORD Version=0;
	PR_TRACE_A0( _this, "Enter Engine::LoadConfiguration method" );
	

	if(PR_FAIL(error=UnloadConfiguration(_this)))
	{
		return error;
	}

	if (PR_FAIL(error = CALL_Registry_OpenKey(reg, &key, key, "Version2", cFALSE)))
	{
		PR_TRACE((_this, prtERROR, "Registry_OpenKey(\"Version2\") failed with %terr", error));
		return errENGINE_BAD_REGISTRY_CONFIGURATION;
	}
	
	type=tid_DWORD;
	CALL_Registry_GetValue(reg,0,key,"Version",&type,&Version,sizeof(Version));
	if( Version < ENGINE_CONFIGURATION_REQUIRED_VERSION ){
		CALL_SYS_SendMsg(_this,pmc_ENGINE_LOAD, pm_ENGINE_LOAD_CONFIGURATION_OUT_OF_DATE,reg,&Version,0);
	}
		
	type=tid_DWORD;
	CALL_Registry_GetValue(reg,0,key,"SFDB Init Number",&type,&_this->data->dwIcheckerInitNumber,sizeof(_this->data->dwIcheckerInitNumber));

	CALL_Registry_GetValue(reg,0,key,"SFDB Quarantine Days",&type,&_this->data->dwQuarantineDays,sizeof(_this->data->dwQuarantineDays));
	if(_this->data->dwQuarantineDays==0)
		_this->data->dwQuarantineDays=14;

	CALL_Registry_GetValue(reg,0,key,"SFDB Long Quarantine Days",&type,&_this->data->dwLongQuarantineDays,sizeof(_this->data->dwLongQuarantineDays));
	if(_this->data->dwLongQuarantineDays==0)
		_this->data->dwLongQuarantineDays=365;
	
	if (PR_FAIL(error=iGetNamesIDsList(_this, reg, key, &hNamesIDsList)))
	{
		PR_TRACE((_this, prtERROR, "iGetNamesIDsList failed with %terr", error));
		return errENGINE_BAD_ACTION_CONFIGURATION;
	}
	
	if (PR_FAIL(error = CALL_Registry_OpenKey(reg,&engines_key,key,"Engines",cFALSE)))
	{
		PR_TRACE((_this, prtERROR, "Registry_OpenKey(\"Engines\") failed with %terr", error));
		return errENGINE_BAD_ACTION_CONFIGURATION;
	}
	else
	{
#ifdef _PRAGUE_TRACE_
		type = tid_DWORD;
		if (PR_SUCC(CALL_Registry_GetValue(reg, NULL, engines_key, "TraceLevel", &type, &dwTraceLevel, sizeof(dwTraceLevel))))
			CALL_SYS_TraceLevelSet(_this, tlsTHIS_OBJECT | tlsAND_NEW, dwTraceLevel, prtMIN_TRACE_LEVEL);
#endif
		while( PR_SUCC(error) && PR_SUCC( CALL_Registry_GetKeyNameByIndex(reg,NULL,engines_key,l++,name, sizeof(name),cFALSE))){
			tRegKey engine_key;
			if(PR_SUCC(error=CALL_Registry_OpenKey(reg,&engine_key,engines_key,name,cFALSE)))
			{
				tPID pid;
				if(PR_SUCC(error=iGetIDsFromRegValue(hNamesIDsList, reg, engine_key, "PID", (tDWORD*)&pid, 1, NULL)))
				{
					if(PR_SUCC(error=CALL_SYS_ObjHeapRealloc(_this,(tPTR)&_this->data->EnginePID,_this->data->EnginePID,(_this->data->EngineCount+1)*sizeof(tPID)))
						&& PR_SUCC(error=CALL_SYS_ObjHeapRealloc(_this,(tPTR)&_this->data->EngineHandle,_this->data->EngineHandle,(_this->data->EngineCount+1)*sizeof(hOBJECT))))
					{
						tBOOL disabled=cFALSE;
						hENGINE engine=NULL;
						
						type=tid_DWORD;
						CALL_Registry_GetValue(reg,NULL,engine_key,"Disabled",&type,&disabled, sizeof(disabled));
						
						if(!disabled)
						{
							if(PR_SUCC(error=CALL_SYS_ObjectCreateQuick(_this, &engine, IID_ENGINE, pid, 0)))
							{
								error=CALL_Engine_LoadConfiguration((hi_Engine)engine,reg,engine_key);
								if(PR_SUCC(error))
								{
									tDT time;
									_this->data->DB_Verdicts=CALL_SYS_PropertyGetByte(engine,pgENGINE_DB_VERDICTS);
									_this->data->DB_State |= CALL_SYS_PropertyGetDWord(engine,pgENGINE_DB_STATE);
									_this->data->DB_Count += CALL_SYS_PropertyGetDWord(engine,pgENGINE_DB_COUNT);
									CALL_SYS_PropertyGet(engine,0,pgENGINE_DB_TIME,&time,sizeof(tDATETIME));
									if( *(tQWORD*)time > *(tQWORD*)(_this->data->DB_Time)){
										tQWORD minutes;
										*(tQWORD*)(_this->data->DB_Time)=*(tQWORD*)time;
										if(PR_SUCC(DTIntervalGet(&time, 0, &minutes, LONG_LONG_CONST(60000000000))))
											_this->data->dwBaseTime=(tDWORD)minutes;
										
									}
								}
							}
						}
						
						_this->data->EnginePID[_this->data->EngineCount]=pid;
						_this->data->EngineHandle[_this->data->EngineCount]=engine;
						_this->data->EngineCount++;
						SET_DEBUG_TRACE(engine);
					}
				}
				CALL_Registry_CloseKey(reg,engine_key);
			}
		}
		CALL_Registry_CloseKey(reg, engines_key);
	}

//==============================================================
#ifdef KLAVPMGR
	if (PR_SUCC (error))
	{
		tRegKey klavpmgr_key;

		if (PR_SUCC (CALL_Registry_OpenKey(reg, &klavpmgr_key, key, "KLAVPMGR", cFALSE)))
		{
			// initialize KLAVA engine
			error = CALL_SYS_ObjectCreateQuick (_this, &(_this->data->hKLAVPMGR), IID_ENGINE, PID_KLAVCORE, SUBTYPE_ANY);
			if (PR_SUCC (error))
			{
				error = CALL_Engine_LoadConfiguration ((hi_Engine)_this->data->hKLAVPMGR, reg, klavpmgr_key);
			}

			_this->data->Entry_OBJECT = KLAVPMGR_ENTRY_OBJECT;
			_this->data->Entry_ARCHIVE = KLAVPMGR_ENTRY_OS;
			_this->data->Entry_IO = KLAVPMGR_ENTRY_IO;

			goto skip_old_action_processor;
		}
	}
#endif // KLAVPMGR
//==============================================================

	if(PR_SUCC(error)){
		if (PR_FAIL(error = CALL_Registry_OpenKey(reg, &entries_key, key, "Entries", cFALSE)))
		{
			PR_TRACE((_this, prtERROR, "Registry_OpenKey(\"Entries\") failed with %terr", error));
		}
		else
		{
			error = iGetIDsFromRegValue(hNamesIDsList, reg, entries_key, "ENTRY_OBJECT", &_this->data->Entry_OBJECT, 1, NULL);
			if (PR_SUCC(error)) error = iGetIDsFromRegValue(hNamesIDsList, reg, entries_key, "ENTRY_IO", &_this->data->Entry_IO, 1, NULL);
			if (PR_SUCC(error)) error = iGetIDsFromRegValue(hNamesIDsList, reg, entries_key, "ENTRY_ARCHIVE", &_this->data->Entry_ARCHIVE, 1, NULL);
			CALL_Registry_CloseKey(reg, entries_key);
		}
	}

	if(PR_SUCC(error)){
		tRegKey actions_key;
		tTYPE_ID type;
		type=tid_DWORD;
		if(PR_SUCC(error=CALL_Registry_OpenKey(reg,&actions_key,key,"Actions",cFALSE))
		&& PR_SUCC(error=CALL_Registry_GetKeyCount(reg,&_this->data->ActionsCount,actions_key))
		&& _this->data->ActionsCount
		&& PR_SUCC(error=CALL_SYS_ObjHeapAlloc( _this, (tPTR*)&_this->data->Actions, sizeof(tPTR)*_this->data->ActionsCount))
		){
			tRegKey act_key;
			tDWORD i=0;
			tDWORD dwActionTypeParams[2];
			tDWORD dwActionTypeParamsCount;
			while( PR_SUCC(error) && PR_SUCC(CALL_Registry_OpenKeyByIndex(reg,&act_key,actions_key,i++,cFALSE))){
				tDWORD engine_dependance_pid;
				tDWORD action_id;
				tDWORD action_type=0;
				type=tid_CHAR;
				if (PR_SUCC(iGetIDsFromRegValue(hNamesIDsList, reg, act_key, "Type", &dwActionTypeParams[0], countof(dwActionTypeParams), &dwActionTypeParamsCount)))
					action_type = dwActionTypeParams[0];
				if (PR_SUCC(iGetIDsFromRegValue(hNamesIDsList, reg, act_key, "Engine Dependance", &engine_dependance_pid, 1, NULL))){
					tDWORD i=_this->data->EngineCount;
					while (i--){
						if(_this->data->EnginePID[i]==engine_dependance_pid){
							if(NULL==_this->data->EngineHandle[i]){
								action_type=ENGINE_ACTION_TYPE_NO_ACTION;
							}
						}
					}
				}
				if(PR_SUCC(error)) error=CALL_Registry_GetKeyName(reg,0,act_key,name,sizeof(name),cFALSE);
				if(PR_SUCC(error)) error=iGetIDFromName(hNamesIDsList, name, &action_id);
				if(PR_SUCC(error)){

					switch(action_type){
					default:
					case ENGINE_ACTION_TYPE_NO_ACTION:
						break;
					case ENGINE_ACTION_TYPE_SET_OF_ACTIONS:
						if(PR_SUCC(error=CALL_SYS_ObjHeapAlloc( _this, (tPTR*)&_this->data->Actions[action_id], sizeof(tACTION_SetOfActions))))
						{
							tACTION_SetOfActions* Action= (tACTION_SetOfActions*)_this->data->Actions[action_id];
							iSetActionName(_this, (tACTION*)Action, name);
							Action->type=action_type;
							type=tid_DWORD;
							iGetIDsFromRegValue(hNamesIDsList, reg,act_key,"Class", &Action->cls, 1, NULL);
							if(!Action->cls)
								Action->cls=-1;
							{
								tDWORD value;
								tDWORD l=0;
								while( PR_SUCC(error) && PR_SUCC(iGetIDsFromRegValueIdx(hNamesIDsList,reg,act_key,"Action",l,&value,1,NULL))){
									if(PR_SUCC(error=CALL_SYS_ObjHeapRealloc( _this, (tPTR*)&Action->actions,Action->actions, sizeof(tDWORD)*(l+1)))){
										Action->actions[l]=value;
										Action->actions_count++;
									}
									l++;
								}
							}
						}
						break;
					case ENGINE_ACTION_TYPE_PROCESS_OS_ON_IO:
						if(PR_SUCC(error=CALL_SYS_ObjHeapAlloc( _this, (tPTR*)&_this->data->Actions[action_id], sizeof(tACTION_ProcessOSonIO))))
						{
							tACTION_ProcessOSonIO* Action= (tACTION_ProcessOSonIO*)_this->data->Actions[action_id];
							iSetActionName(_this, (tACTION*)Action, name);
							Action->type=action_type;
							iGetIDsFromRegValue(hNamesIDsList, reg,act_key,"Class", &Action->cls, 1, NULL);
							if(!Action->cls)
								Action->cls=-1;
							iGetIDsFromRegValue(hNamesIDsList, reg,act_key,"Child Origin", &Action->child_origin, 1, NULL);
							iGetIDsFromRegValue(hNamesIDsList, reg,act_key,"Mini PID", &Action->mimi_pid, 1, NULL);
							type=tid_BOOL;
							CALL_Registry_GetValue(reg,0,act_key,"Multivolume",&type,&Action->multivolume,sizeof(Action->multivolume));
							error=iGetIDsFromRegValue(hNamesIDsList, reg,act_key,"PID", &Action->pid, 1, NULL);
						}
						break;
					case ENGINE_ACTION_TYPE_CONVERT_IO:
						if(PR_SUCC(error=CALL_SYS_ObjHeapAlloc( _this, (tPTR*)&_this->data->Actions[action_id], sizeof(tACTION_ConvertIO))))
						{
							tACTION_ConvertIO* Action= (tACTION_ConvertIO*)_this->data->Actions[action_id];
							iSetActionName(_this, (tACTION*)Action, name);
							Action->type=action_type;
							iGetIDsFromRegValue(hNamesIDsList, reg,act_key,"Class", &Action->cls, 1, NULL);
							if(!Action->cls)
								Action->cls=-1;
							iGetIDsFromRegValue(hNamesIDsList, reg,act_key,"Child Origin", &Action->child_origin, 1, NULL);
							iGetIDsFromRegValue(hNamesIDsList, reg,act_key,"Converter PID", &Action->converter_pid, 1, NULL);
							iGetIDsFromRegValue(hNamesIDsList, reg,act_key,"PID", &Action->pid, 1, NULL);
							if (Action->converter_pid==0 && Action->pid==0)
								error = errENGINE_BAD_ACTION_CONFIGURATION;
						}
						break;
					case ENGINE_ACTION_TYPE_SWITCH_BY_PROPERTY:
						if(PR_SUCC(error=CALL_SYS_ObjHeapAlloc( _this, (tPTR*)&_this->data->Actions[action_id], sizeof(tACTION_SwitchByProperty))))
						{
							tACTION_SwitchByProperty* Action= (tACTION_SwitchByProperty*)_this->data->Actions[action_id];
							iSetActionName(_this, (tACTION*)Action, name);
							if (dwActionTypeParamsCount < 2)
							{
								PR_TRACE((_this, prtERROR, "%s:ENGINE_ACTION_TYPE_SWITCH_BY_PROPERTY - parameter not specified (PROPID)", name));
								error = errENGINE_BAD_ACTION_CONFIGURATION;
								break;
							}
							Action->type=action_type;
							type=tid_DWORD;
							iGetIDsFromRegValue(hNamesIDsList, reg,act_key,"Class", &Action->cls, 1, NULL);
							if(!Action->cls)
								Action->cls=-1;
							iGetIDsFromRegValue(hNamesIDsList, reg,act_key,"Default", &Action->default_action, 1, NULL);
							Action->propid = dwActionTypeParams[1];
							{
								tDWORD dwValueParams[2];
								tDWORD dwValueParamsCount;
								tDWORD l=0;
								while( PR_SUCC(error) && PR_SUCC(iGetIDsFromRegValueIdx(hNamesIDsList,reg,act_key,"Case",l,&dwValueParams[0], countof(dwValueParams), &dwValueParamsCount))){
									if (dwValueParamsCount < 2){
										PR_TRACE((_this, prtERROR, "%s:Action%d - parameter not specified (Action)", name, l));
										return error;
									}
									if(PR_SUCC(error=CALL_SYS_ObjHeapRealloc( _this, (tPTR*)&Action->actions,Action->actions, sizeof(tDWORD)*(l+1)))
									&& PR_SUCC(error=CALL_SYS_ObjHeapRealloc( _this, (tPTR*)&Action->value,Action->value, sizeof(tDWORD)*(l+1)))
									){
										Action->value[l] = dwValueParams[0];
										Action->actions[l] = dwValueParams[1];
										Action->actions_count++;
									}
									l++;
								}
							}
						}
						break;
					case ENGINE_ACTION_TYPE_SUBENGINE_PROCESS:
						if(PR_SUCC(error=CALL_SYS_ObjHeapAlloc( _this, (tPTR*)&_this->data->Actions[action_id], sizeof(tACTION_SubengineProcess))))
						{
							tPID pid;
							tACTION_SubengineProcess* Action= (tACTION_SubengineProcess*)_this->data->Actions[action_id];
							iSetActionName(_this, (tACTION*)Action, name);
							if (dwActionTypeParamsCount < 2)
							{
								PR_TRACE((_this, prtERROR, "%s:ENGINE_ACTION_TYPE_SUBENGINE_PROCESS - parameter not specified (Engine PID)", name));
								error = errENGINE_BAD_ACTION_CONFIGURATION;
								break;
							}
							Action->type=action_type;
							type=tid_DWORD;
							iGetIDsFromRegValue(hNamesIDsList, reg,act_key,"Class", &Action->cls, 1, NULL);
							if(!Action->cls)
								Action->cls=-1;
							pid = dwActionTypeParams[1];
							{
								tDWORD i=_this->data->EngineCount;
								error=errOBJECT_NOT_FOUND;
								while (i--){
									if(_this->data->EnginePID[i]==pid){
										Action->engine=_this->data->EngineHandle[i];
										error=errOK;
										i=0;
									}
								}
								if(error==errOBJECT_NOT_FOUND){
									i=0;
									iGetIDsFromRegValue(hNamesIDsList,reg,act_key,"Flags",&i,1,NULL);
									if(i & ENGINE_ACTION_FLAG_NOT_REQUIRED){
										error=errOK;
										Action->type=ENGINE_ACTION_TYPE_NO_ACTION;
									}
								}
							}
						}
						break;
					}
				}
				CALL_Registry_CloseKey(reg,act_key);
			}
			CALL_Registry_CloseKey(reg,actions_key);
		}
	}
	
//==============================================================
#ifdef KLAVPMGR
	skip_old_action_processor: ;
#endif // KLAVPMGR
//==============================================================

	CALL_SYS_ObjectClose(hNamesIDsList);
	
	if(PR_SUCC(error)){
		CALL_SYS_SendMsg(_this,pmc_ENGINE_LOAD, pm_ENGINE_LOAD_CONFIGURATION_LOADED,reg,0,0);
	}else{
		tDWORD l=sizeof(error);
		CALL_SYS_SendMsg(_this,pmc_ENGINE_LOAD, pm_ENGINE_LOAD_CONFIGURATION_ERROR,reg,&error,&l);
		PR_TRACE((_this, prtERROR, "Engine_LoadConfiguration failed with %terr", error));
		UnloadConfiguration(_this);
	}
	PR_TRACE_A1( _this, "Leave Engine::LoadConfiguration method, ret %terr", error );
	return error;
}
// AVP Prague stamp end



//
tERROR ProcessObjptr( hi_Engine _this, hOS os, hObjPtr obj, tACTION_ProcessOSonIO* action, hSTRING transformer_name)
{
	tERROR error=errOK;
	tERROR err=errOK;
	
	while(PR_SUCC(error)){
		hIO io;
		error=CALL_ObjPtr_Next(obj);
		if(PR_FAIL(error))
			break;

		CALL_SYS_PropertyDelete(obj,propid_virtual_name);

		CALL_SYS_SendMsg(obj,pmc_ENGINE_IO,pm_ENGINE_IO_MAKE_VIRTUAL_NAME,_this,0,0);
		error=CALL_SYS_SendMsg(obj, pmc_ENGINE_IO, pm_ENGINE_IO_NAME_CHECK,_this,0,0);
		if(PR_FAIL(error)){
			if(error==errENGINE_ACTION_SKIP)
				continue;
			break;
		}
		error=CALL_ObjPtr_IOCreate( obj, &io, NULL, fOMODE_OPEN_IF_EXIST, fACCESS_READ );
		
		if(PR_SUCC(error) && !io)
			error=errOBJECT_CANNOT_BE_INITIALIZED;
		
		if(PR_FAIL(error))
		{
			if(CALL_SYS_PropertyGetBool( obj, pgIS_FOLDER)){
				error=errOK;
			}
			error=HandleProcessingError(_this, (hOBJECT)obj, error);
		}
		else
		{
			if(transformer_name)
				CALL_String_ExportToProp(transformer_name,0,cSTRING_WHOLE,(hOBJECT)io,propid_npENGINE_OBJECT_TRANSFORMER_NAME);

			if(OID_PASSWORDED_EXE == CALL_SYS_PropertyGetDWord(io, pgOBJECT_ORIGIN))
			{
				CALL_SYS_PropertySetDWord( io, propid_object_readonly_error,  errOBJECT_PASSWORD_PROTECTED);
				CALL_SYS_PropertySetObj(   io, propid_object_readonly_object, (hOBJECT)io);
			}
			
			if( action->child_origin)
				CALL_SYS_PropertySetDWord(io,pgOBJECT_ORIGIN,action->child_origin);
			
			if(PR_SUCC(error)) error=EngineAction( _this, (hOBJECT)io, _this->data->Entry_IO );

			err=CALL_SYS_ObjectClose(io);
			if(PR_FAIL(err))
				IncrementInfected((hOBJECT)obj,0);
			if(PR_SUCC(error))error=err;
		}
		
		if(CALL_SYS_PropertyGetBool(obj,propid_object_skip))
		{
			error=errENGINE_ACTION_SKIP;
			break;
		}

		if(CALL_SYS_PropertyGetBool( obj, pgIS_FOLDER) )
		{
			hObjPtr optr;
			if(PR_SUCC(err=CALL_ObjPtr_Clone( obj, &optr)))
			{
				if(PR_SUCC(err=CALL_ObjPtr_StepDown( optr )))
				{
					err=ProcessObjptr( _this, os, optr, action, transformer_name );
				}
				CALL_SYS_ObjectClose( optr );
			}
			else
				err=HandleProcessingError(_this, (hOBJECT)obj, err);

			if(PR_SUCC(error))error=err;
		}
	}
	if(error==errNOT_FOUND)	error=errOK;
	if(error==errEND_OF_THE_LIST)	error=errOK;
	error=HandleProcessingError(_this, (hOBJECT)obj, error);
	return error;
}


tERROR EngineActionProcessOS( hi_Engine _this, hOBJECT obj, tACTION_ProcessOSonIO* action, hOS p_osystem)
{
	tERROR error=errOK;
	hOS osystem=p_osystem;
	tERROR rec_error=errOK;
	tBOOL b_multivolume=cTRUE;
	tDWORD dwSize;

	if( !p_osystem )
	{
		rec_error=CALL_SYS_Recognize(obj,IID_OS,action->pid,action->mimi_pid);
	}
	
	if(PR_SUCC(rec_error))
	{
		dwSize=sizeof(b_multivolume);
		error=rec_error=CALL_SYS_SendMsg( obj,pmc_ENGINE_IO,pm_ENGINE_IO_OS_RECOGNIZE, 0, &b_multivolume, &dwSize );
	}
		
	if(PR_SUCC(rec_error))
	{
		if( !p_osystem )
			rec_error=CALL_SYS_ObjectCreate( obj, &osystem, IID_OS, action->pid, 0 );

		if ( PR_SUCC(rec_error)) {
			tERROR err;
			tDWORD l=sizeof(error);
			tQWORD qwHash=0;
			
			if(action && action->multivolume && b_multivolume)
				CALL_SYS_PropertySetBool(osystem, pgMULTIVOL_AS_SINGLE_SET, cTRUE);
			

			if((qwHash = CALL_SYS_PropertyGetQWord(osystem, pgOBJECT_HASH)) != 0){
				dwSize=sizeof(qwHash);
				
				if(errENGINE_ACTION_SKIP == CALL_SYS_SendMsg(obj, pmc_AVPMGR_INTERNAL, pm_AVPMGR_INTERNAL_IO_HASH_FRAME_CHECK, _this, &qwHash, &dwSize))
				{
					rec_error=errENGINE_ACTION_SKIP;
					qwHash=0;
					CALL_SYS_SendMsg(obj,pmc_AVPMGR_INTERNAL,pm_AVPMGR_INTERNAL_IO_OS_OFFSET_REGISTER,_this,0,0);
				}
			}
			
			if( !p_osystem )
				if( PR_SUCC(rec_error) )
					rec_error = CALL_SYS_ObjectCreateDone( osystem );

			if( PR_SUCC(rec_error)) {
				//OS created
				hObjPtr optr;
				tDWORD  l_action_class;
				tDWORD  cls;
				PR_TRACE(( _this, prtNOTIFY, "AVPMGR\tOS created: %tpid on object %08X", OBJ_PID(osystem), obj ));

				if(PR_SUCC(CALL_SYS_ObjectCheck( obj,obj,IID_IO,0,0,0 )))
				{
					tQWORD size=0;
					CALL_IO_GetSize((hIO)obj,&size,IO_SIZE_TYPE_EXPLICIT);
					if( size > 0x200000 )
						CALL_SYS_PropertyDelete(osystem,propid_auto_password_bool);
				}

				l_action_class=CALL_SYS_PropertyGetDWord(obj,propid_action_class_mask_current);
				cls=CALL_SYS_PropertyGetDWord(obj,propid_action_class_mask);
				if(	l_action_class & (ENGINE_ACTION_CLASS_MAIL_DATABASE|ENGINE_ACTION_CLASS_MAIL_PLAIN))
				{
					cls|=ENGINE_ACTION_CLASS_MAIL_PLAIN;
					CALL_SYS_PropertySetDWord(osystem,propid_action_class_mask,cls);
					CALL_SYS_PropertySetBool(osystem,propid_auto_password_bool,cTRUE);
				}

				if( action  && (action->mimi_pid==PID_RAR) )
					CALL_SYS_PropertyDelete(osystem,propid_auto_password_bool);
				
				if( PR_FAIL(CALL_SYS_SendMsg(obj,pmc_ENGINE_IO,pm_ENGINE_IO_OS_CHECK,osystem,0,0))){
					PR_TRACE(( _this, prtNOTIFY, "AVPMGR\t- OS Processing skipped by user" ));
				}else 
				if( PR_FAIL(rec_error=CALL_SYS_ObjectCreateQuick(osystem,&optr,IID_OBJPTR,CALL_SYS_PropertyGetDWord(osystem,pgPLUGIN_ID),0))){
					PR_TRACE(( _this, prtERROR, "AVPMGR\tERROR: Can't crate ObjEnum on OS" ));
				}else{
					//Object Enumerator created
					hSTRING str=0;
					tDWORD size=sizeof(l_action_class);

					if(PR_SUCC(CALL_SYS_ObjectCreateQuick(obj, &str, IID_STRING,PID_ANY,SUBTYPE_ANY ))){
						if(PR_SUCC(CALL_String_ImportFromProp(str,0,(hOBJECT)osystem,pgINTERFACE_COMMENT)))
							CALL_String_ExportToProp(str,0,cSTRING_WHOLE,obj,propid_npENGINE_OBJECT_OBTAINED_TRANSFORMER_NAME);
						else
							str=0;
					}

					CALL_SYS_SendMsg(obj,pmc_ENGINE_IO,pm_ENGINE_IO_OS_ENTER,_this,&l_action_class,&size);
					
					if( p_osystem )
						error=ProcessChild( _this, (hOBJECT)osystem, cFALSE);
					else
						error=ProcessObjptr( _this, osystem, optr, action, str );
					
					if(errENGINE_OS_STOPPED==error)
						error=errOK;
					
					if(str)
						CALL_SYS_ObjectClose(str);
					CALL_SYS_SendMsg(obj,pmc_ENGINE_IO,pm_ENGINE_IO_OS_LEAVE,_this,&error,&l);

					err=CALL_SYS_ObjectClose( optr );
					if(PR_SUCC(error))error=err;
				}
	
				CALL_SYS_SendMsg(obj,pmc_AVPMGR_INTERNAL,pm_AVPMGR_INTERNAL_IO_OS_OFFSET_REGISTER,_this,0,0);
			}
			
			CALL_SYS_SendMsg(obj,pmc_ENGINE_IO,pm_ENGINE_IO_OS_PRECLOSE,osystem,0,0);
			PR_TRACE(( _this, prtNOTIFY, "AVPMGR\tOS closed : %tpid on object %08X", OBJ_PID(osystem), obj ));
			err=CALL_SYS_ObjectClose( osystem );
			if(PR_FAIL(err))
			{
				tDWORD size=sizeof(err);
				CALL_SYS_SendMsg(obj,pmc_ENGINE_IO,pm_ENGINE_IO_OS_CLOSE_ERROR,0,(tPTR)&err,&size);
				IncrementInfected(obj,0);
			}
			if(PR_SUCC(error))
				error=err;
			
			if(qwHash)
				CALL_SYS_SendMsg(obj, pmc_AVPMGR_INTERNAL, pm_AVPMGR_INTERNAL_IO_HASH_FRAME_LEAVE, _this, 0, 0);
		}
	}
	
	switch(rec_error)
	{
	case errOBJECT_PASSWORD_PROTECTED:
		error=errOBJECT_PASSWORD_PROTECTED;
		break;
	case warnOBJECT_DATA_CORRUPTED :
	case errOBJECT_DATA_CORRUPTED:
		error=errOBJECT_DATA_CORRUPTED;
		break;
	default:		
		break;
	}

	return error;
}

tERROR CheckObjectOSOffset(hOBJECT obj, tQWORD par_off)
{
	tERROR error=errOK;
	tLISTNODE node=0;
	hLIST offset_list=(hLIST)CALL_SYS_PropertyGetObj(obj,propid_os_offset_list);
	if(offset_list){
		error=CALL_List_First(offset_list,&node);
		while(node && PR_SUCC(error)){
			tQWORD off=(tQWORD)-1;
			if(PR_SUCC(error)) error=CALL_List_DataGet(offset_list,0,node,&off,sizeof(off));
			if(PR_SUCC(error)) if(par_off==off){
				CALL_SYS_PropertyDelete( obj, propid_os_offset);					
				return errENGINE_ACTION_SKIP;
			}
			if(PR_SUCC(error)) error=CALL_List_Next(offset_list,&node,node);
		}
	}
	if(error==errEND_OF_THE_LIST)error=errOK;
	return error;	
}
	

tERROR EngineActionProcessOSonIO( hi_Engine _this, hOBJECT obj, tACTION_ProcessOSonIO* action)
{
	tERROR error=errOK;
	hOBJECT dmap_io=0;
	tQWORD par_off=CALL_SYS_PropertyGetQWord( obj, propid_os_offset );
	
	error=CheckObjectOSOffset(obj, par_off);
	if(PR_FAIL(error))
		return error;

	if(par_off==0){
		error=EngineActionProcessOS( _this, obj, action, 0 );
	}
	else{
		tQWORD size=0;
		error=CALL_IO_GetSize((hIO)obj,&size,IO_SIZE_TYPE_EXPLICIT);
		if(par_off>=size) //can be already truncated
		{
			CALL_SYS_PropertyDelete( obj, propid_os_offset);					
			return errOK; 
		}
		else
			size-=par_off;

		error=CALL_SYS_ObjectCreate(obj, &dmap_io,  IID_IO, DMAP_ID,0);
		if(PR_SUCC(error)){
			if(PR_SUCC(error)) error=CALL_SYS_PropertySetDWord( dmap_io, ppMAP_AREA_ORIGIN, (tDWORD)obj);
			if(PR_SUCC(error)) error=CALL_SYS_PropertySetQWord( dmap_io, ppMAP_AREA_START, par_off);
			if(PR_SUCC(error)) error=CALL_SYS_PropertySetQWord( dmap_io, ppMAP_AREA_SIZE, size);
			if(PR_SUCC(error)) error=CALL_SYS_PropertySetDWord( dmap_io, pgOBJECT_ACCESS_MODE , fACCESS_NO_CHANGE_MODE | fACCESS_READ);
			if(PR_SUCC(error)) error=CALL_SYS_ObjectCreateDone( dmap_io );
			if(PR_SUCC(error))
			{
				if(PR_SUCC(CALL_SYS_PropertyGetDWord(obj,propid_object_readonly_error)))
				{
					CALL_SYS_PropertySetDWord( dmap_io, propid_object_readonly_error,  errOBJECT_WRITE_NOT_SUPPORTED);
					CALL_SYS_PropertySetObj(   dmap_io, propid_object_readonly_object, obj);
				}
				CALL_SYS_PropertySetQWord( dmap_io, propid_os_offset, 0);
				CALL_SYS_PropertySetObj(   dmap_io, propid_integral_parent_io, CALL_SYS_PropertyGetObj(obj, propid_integral_parent_io));
				CALL_SYS_PropertySetObj(   dmap_io, pgOBJECT_BASED_ON,obj);
			}

			if(PR_SUCC(error)){
				tDWORD l=1;
				CALL_SYS_SendMsg(dmap_io,pmc_ENGINE_IO,pm_ENGINE_IO_MAKE_VIRTUAL_NAME,_this,"",&l);
				
				error=EngineActionProcessOS( _this, dmap_io, action, 0 );

				if( 0==CALL_SYS_PropertySize( dmap_io, propid_os_offset))
					CALL_SYS_SendMsg(obj,pmc_AVPMGR_INTERNAL,pm_AVPMGR_INTERNAL_IO_OS_OFFSET_REGISTER,_this,0,0);
			}

			if(CALL_SYS_PropertyGetBool(dmap_io,pgOBJECT_DELETE_ON_CLOSE))
				CALL_SYS_PropertySetBool(obj,pgOBJECT_DELETE_ON_CLOSE,cTRUE);

			CALL_SYS_ObjectClose(dmap_io);
		}
		return error;
	}

	return error;
}

tERROR EngineActionProcessSubIO( hi_Engine _this, hOBJECT obj, hIO io, tDWORD origin, hOBJECT converter)
{
	tERROR error=errOK;
	
	if(origin)	error=CALL_SYS_PropertySetDWord(io,pgOBJECT_ORIGIN,origin);
	
	if ( PR_SUCC(error)){
		tDWORD l=sizeof(error);
		hSTRING str;
		if(PR_SUCC(CALL_SYS_ObjectCreateQuick(obj, &str, IID_STRING,PID_ANY,SUBTYPE_ANY ))){
			if(PR_SUCC(CALL_String_ImportFromProp(str,0,converter,pgINTERFACE_COMMENT))){
				CALL_String_ExportToProp(str,0,cSTRING_WHOLE,obj,propid_npENGINE_OBJECT_OBTAINED_TRANSFORMER_NAME);
				CALL_String_ExportToProp(str,0,cSTRING_WHOLE,(hOBJECT)io,propid_npENGINE_OBJECT_TRANSFORMER_NAME);
			}
			CALL_SYS_ObjectClose(str);
		}

		CALL_SYS_PropertySetObj(   io, pgOBJECT_BASED_ON,obj);
		if(PR_SUCC(CALL_SYS_PropertyGetDWord(obj,propid_object_readonly_error)))
		{
			CALL_SYS_PropertySetDWord( io, propid_object_readonly_error,  errOBJECT_WRITE_NOT_SUPPORTED);
			CALL_SYS_PropertySetObj(   io, propid_object_readonly_object, obj);
		}
	
		CALL_SYS_SendMsg(obj,pmc_ENGINE_IO,pm_ENGINE_IO_SUB_IO_ENTER,_this,0,0);
		error=EngineAction( _this, (hOBJECT)io, _this->data->Entry_OBJECT );
		CALL_SYS_SendMsg(obj,pmc_ENGINE_IO,pm_ENGINE_IO_SUB_IO_LEAVE,_this,&error,&l);
	}
	return error;
}

tERROR EngineActionConvertIO( hi_Engine _this, hOBJECT obj, tACTION_ConvertIO* action)
{
	tERROR error=errOK;
	tERROR _err;
	hIO io=0;
	
	if(action->converter_pid == 0){
		PR_TRACE((_this, prtNOTIFY, "Converting by creating IID_IO PID=%d", action->pid));
		if( PR_SUCC(_err = CALL_SYS_ObjectCreateQuick( obj, &io, IID_IO, action->pid, 0 ))) 
		{
			error=EngineActionProcessSubIO( _this, obj,io, action->child_origin, (hOBJECT)io);
			PR_TRACE((_this, prtNOTIFY, "Converting by creating IID_IO PID=%d succeeded with %terr, EngineActionProcessSubIO returned %terr", _err, error));
		}
		else
		{
			PR_TRACE((_this, prtNOTIFY, "Converting by creating IID_IO PID=%d failed", _err));
		}
	}
	else if(action->pid == 0)
	{
		hCONVERTER converter;
		PR_TRACE((_this, prtNOTIFY, "Converting by creating IID_CONVERTER PID=%d", action->converter_pid));
		if ( PR_SUCC(_err = CALL_SYS_ObjectCreateQuick( obj, &converter, IID_CONVERTER, action->converter_pid, 0 ))) 
		{
			PR_TRACE((_this, prtNOTIFY, "Creating converter PID=%d succeeded with %terr", action->converter_pid, _err));
			if ( PR_SUCC(_err = CALL_Converter_Convert( converter, (hOBJECT*)&io, obj )) ) 
			{
				error=EngineActionProcessSubIO( _this, obj,io, action->child_origin, (hOBJECT)converter);
				PR_TRACE((_this, prtNOTIFY, "Converter::Convert PID=%d succeeded with %terr, EngineActionProcessSubIO returned %terr", action->converter_pid, _err, error));
			}
			else
			{
				PR_TRACE((_this, prtNOTIFY, "Converter::Convert PID=%d failed with %terr", action->converter_pid, _err));
			}
			CALL_SYS_ObjectClose(converter);
		}
		else
		{
			PR_TRACE((_this, prtERROR, "Creating converter PID=%d failed with %terr", action->converter_pid, _err));
		}
	}else{
		PR_TRACE((_this, prtERROR, "Converting by both io PID and converter PID not supported"));
		error=errNOT_IMPLEMENTED;
	}
	
	if(io)
		CALL_SYS_ObjectClose( io );
	return error;
}

tERROR ProcessRegisteredIOs(hi_Engine _this, hOBJECT obj)
{
	tERROR error=errOK;
	tLISTNODE node=0;
	hLIST postprocess_list=(hLIST)CALL_SYS_PropertyGetObj(obj,propid_postprocess_list);
	if(postprocess_list){
		PR_TRACE((_this, prtNOTIFY, "+ Postprocess IOs processing..."));
		while(PR_SUCC(error) && PR_SUCC(CALL_List_First(postprocess_list,&node)) && node){
			hIO io=0;
			error=CALL_List_DataGet(postprocess_list,0,node,&io,sizeof(io));
			if(PR_SUCC(error)){
				tDWORD l=sizeof(error);
				PR_TRACE((_this, prtIMPORTANT, "Performing postprocessing for object [%08X] (parent [%08X])", io, obj));
				CALL_SYS_SendMsg(obj,pmc_ENGINE_IO,pm_ENGINE_IO_SUB_IO_ENTER,_this,0,0);
				error=EngineAction( _this, (hOBJECT)io, _this->data->Entry_OBJECT );
				CALL_SYS_SendMsg(obj,pmc_ENGINE_IO,pm_ENGINE_IO_SUB_IO_LEAVE,_this,&error,&l);
				CALL_SYS_ObjectClose( io );
				CALL_List_Remove(postprocess_list,node);
			}
		}
		PR_TRACE((_this, prtNOTIFY, "- Postprocess IOs processing finished."));
	}
	return error;
}

tERROR EngineActionSwitchByProperty( hi_Engine _this, hOBJECT obj, tACTION_SwitchByProperty* action)
{
	tERROR error=errOK;
	tDWORD i;
	tBOOL  action_founded=0;
	tDWORD action_to_perform=0;
	tDWORD value;
	value=CALL_SYS_PropertyGetDWord(obj,action->propid);

	if(action->propid == pgOBJECT_ORIGIN)
	{
		switch(value){
		case OID_MAIL_MSG_BODY:
		case OID_MAIL_MSG_ATTACH:
		case OID_MAIL_MSG_MIME:
			CALL_SYS_PropertySetBool(obj,propid_auto_password_bool,cTRUE);
			break;
		default:
			break;
		}
	}

	for(i=0;i<action->actions_count;i++){
		if(value == action->value[i]){
			action_to_perform=action->actions[i];
def:
			action_founded=1;
			error=EngineAction(_this,obj,action_to_perform);
			break; //exit from loop
		}
	}
	if (!action_founded)
	{
		if( action->default_action ){
			PR_TRACE((_this, prtNOTIFY, "Performing default action in switch of %s", action->name));
			action_to_perform = action->default_action;
			goto def;
		}
		else
			PR_TRACE((_this, prtDANGER, "No action found in %s for object [%08X, %tiid, %tpid, %toid]", action->name, obj, OBJ_IID(obj), OBJ_PID(obj), CALL_SYS_PropertyGetDWord(obj, pgOBJECT_ORIGIN)));
	}
	
	return error;
}

//==============================================================
#ifdef KLAVPMGR

tERROR KlavEngineAction( hi_Engine _this, hOBJECT obj, tDWORD action_id)
{
	tERROR error = errOK;
	tDWORD l;
	tSTRING action_name = 0;

	error = CALL_SYS_SendMsg (obj,pmc_PROCESSING,pm_PROCESSING_YIELD,_this,0,0);
	if (PR_FAIL (error)) return error;

	if (CALL_SYS_PropertyGetBool (obj,propid_object_skip))
	{
		error = errENGINE_ACTION_SKIP;
	}

	if (PR_SUCC (error))
	{
		switch (action_id)
		{
		case KLAVPMGR_ENTRY_OBJECT:
			action_name = "ENGINE_ACTION_OBJECT";
			break;
		case KLAVPMGR_ENTRY_OS:
			action_name = "ENGINE_ACTION_OBJECT_SYSTEM";
			break;
		case KLAVPMGR_ENTRY_IO:
			action_name = "ENGINE_ACTION_IO";
			break;
		default:
			error = errUNEXPECTED;
			break;
		}
	}

	if (PR_SUCC (error))
	{
		tDWORD attempt = 0;
		tBOOL  b_enter_io = cFALSE;
		tBOOL  b_io = cFALSE;

		if (PR_SUCC (CALL_SYS_ObjectCheck(obj,obj,IID_IO,PID_ANY,SUBTYPE_ANY,cTRUE)))
		{
			b_io=cTRUE;
			if(0 == CALL_SYS_PropertyGetDWord(obj,propid_creator_engine_pid))
			{
				b_enter_io=cTRUE;
				CALL_SYS_PropertySetDWord(obj,propid_creator_engine_pid,(tDWORD)_this);
			}
		}

		if( b_enter_io )
		{
			CALL_SYS_PropertySetObj(obj,propid_object_parent_io,obj);
			error=CALL_SYS_SendMsg(obj,pmc_ENGINE_IO,pm_ENGINE_IO_MAKE_VIRTUAL_NAME,_this,0,0);
			if(PR_SUCC(error)) error=CALL_SYS_SendMsg(obj,pmc_ENGINE_IO,pm_ENGINE_IO_NAME_CHECK,0,0,0);
			if(PR_SUCC(error)) error=CheckStackAvail(_this->data->StackProbe);
			if(PR_SUCC(error)) error=CALL_SYS_SendMsg(obj,pmc_ENGINE_IO,pm_ENGINE_IO_BEGIN,_this,0,0);
		}
again:
		if(PR_SUCC(error))
		{
			// process action (TODO: cCP_UTF8)
			CALL_SYS_PropertySetStr (obj, 0, propid_klavpmgr_action_name, action_name, 0, cCP_ANSI);

			error = CALL_Engine_Process ((hi_Engine)_this->data->hKLAVPMGR, obj);

			if( b_io )
			{
				if(PR_SUCC(error))
					error=CALL_SYS_PropertyGetDWord(obj, propid_processing_error);
				
				if(PR_SUCC(error))
					error=ProcessRegisteredIOs(_this,obj);

				if(OID_AVP3_DOS_MEMORY == CALL_SYS_PropertyGetDWord(obj,pgOBJECT_ORIGIN))
					error=errOK;
			
				if(PR_FAIL(error))
				{
					hOBJECT integral_parent_io;
					switch(error){
					case errENGINE_IO_MODIFIED:
						if(CALL_SYS_PropertyGetBool(obj,pgOBJECT_DELETE_ON_CLOSE))
						{
							error=errOK;
							break;
						}
						CALL_SYS_PropertyDelete(obj, propid_processing_error);
						CALL_SYS_PropertyDelete(obj, propid_object_infected_parent);
						CALL_SYS_SendMsg(obj,pmc_AVPMGR_INTERNAL,pm_AVPMGR_INTERNAL_IO_OS_OFFSET_FLUSH,_this,0,0);
						error=CALL_SYS_SendMsg(obj,pmc_ENGINE_IO ,pm_ENGINE_IO_WAS_MODIFIED,_this,0,0);
						
						integral_parent_io=CALL_SYS_PropertyGetObj(obj,propid_integral_parent_io);
						if(integral_parent_io){
							hOBJECT _integral_parent_io;
							while((_integral_parent_io=CALL_SYS_PropertyGetObj(integral_parent_io,propid_integral_parent_io)) != 0){
								integral_parent_io=_integral_parent_io;
							}
							error=CopyIOContent((hIO)integral_parent_io,(hIO)obj);
							if(PR_SUCC(error))
							{
								CALL_SYS_PropertySetDWord(integral_parent_io, propid_processing_error,errENGINE_IO_MODIFIED);
// 6.0 handle this in AVS
//								if(0==CALL_SYS_PropertyGetObj(integral_parent_io,propid_integral_parent_io))
//									CALL_SYS_SendMsg(integral_parent_io,pmc_ENGINE_IO,pm_ENGINE_IO_DISINFECTED,_this,0,0);
							}
							else
							{
								tDWORD l=sizeof(error);
								IncrementInfected(obj,0);
								CALL_SYS_SendMsg(integral_parent_io,pmc_ENGINE_IO,pm_ENGINE_IO_DISINFECTION_FAILED,_this,&error,&l);
							}
						}
						if(PR_SUCC(error)) {
							if(attempt++ < 5)
								goto again;
						}
						break;
					default:
						break;
					}
				}
				l = sizeof(error);
			}
		}

		if( b_enter_io )
		{
			CALL_SYS_SendMsg(obj,pmc_ENGINE_IO,pm_ENGINE_IO_DONE,_this,&error,&l);
			CALL_SYS_PropertySetDWord(obj,propid_creator_engine_pid,0);
		}
	}

	error = HandleProcessingError(_this, obj, error);
	return error;
}

tERROR KLAV_AVPMGR_ProcessSubEngine (hOBJECT avpmgr_this, tPID pid, hOBJECT object)
{
	hi_Engine _this = (hi_Engine)avpmgr_this;
	tERROR error = errOK;
	tDWORD cnt = _this->data->EngineCount;
	tDWORD i;
	hENGINE engine = 0;

	for (i = 0; i < cnt; ++i)
	{
		if (_this->data->EnginePID [i] == pid)
		{
			engine = _this->data->EngineHandle [i];
			break;
		}
	}

	if (i == cnt)
	{
		PR_TRACE(( _this, prtERROR, "AVPMGR\t- Sub-engine not found (PID:%d)" , pid ));
		return errUNEXPECTED;
	}

	if (engine == 0)
	{
		PR_TRACE(( _this, prtNOTIFY, "AVPMGR\t- Sub-engine disabled (PID:%d)" , pid ));
	}
	else
	{
		error = CALL_Engine_Process ((hi_Engine)engine, object);
		error = HandleProcessingError (_this, object, error);
	}

	return error;
}

//tERROR KLAV_AVPMGR_ConvertIO (

#endif // KLAVPMGR
//==============================================================

tERROR EngineAction( hi_Engine _this, hOBJECT obj, tDWORD action_id)
{
	tERROR error=errOK;
	tACTION* Action=0;
	tDWORD l;

//==============================================================
#ifdef KLAVPMGR
	if (_this->data->hKLAVPMGR != 0)
	{
		return KlavEngineAction (_this, obj, action_id);
	}
#endif // KLAVPMGR
//==============================================================

	Action=_this->data->Actions[action_id];

	if(Action && PR_SUCC(error = CALL_SYS_SendMsg(obj,pmc_PROCESSING,pm_PROCESSING_YIELD,_this,0,0)))
	{
		tDWORD action_class_mask=CALL_SYS_PropertyGetDWord(obj,propid_action_class_mask);

		if(CALL_SYS_PropertyGetBool(obj,propid_object_skip))
		{
			error=errENGINE_ACTION_SKIP;
		}
		else if(!(Action->cls & action_class_mask))
		{
			PR_TRACE(( _this, prtNOTIFY, "AVPMGR\t- %s - Action skipped by class mask." , Action->name ));
			error=errENGINE_ACTION_SKIP;
		}
		else
		{
			CALL_SYS_PropertySetDWord(obj,propid_action_class_mask_current,Action->cls);
		}

#ifdef FULL_pmc_ENGINE_ACTION
		if(PR_SUCC(error))
		{
			l=sizeof(Action->cls);
			error = CALL_SYS_SendMsg(obj,pmc_ENGINE_ACTION,Action->type,_this,&Action->cls,&l);
#ifdef _PRAGUE_TRACE_
			{
				tCHAR szObjName[0x100];
				if (PR_FAIL(CALL_SYS_PropertyGetStr(obj, NULL, propid_virtual_name, szObjName, sizeof(szObjName), cCP_ANSI)))
					szObjName[0] = 0;
				if(PR_FAIL(error))
					PR_TRACE(( _this, prtNOTIFY, "AVPMGR\t- %s - Action skipped by application for [%08X] %s", Action->name, obj, szObjName ));
				else
					PR_TRACE(( _this, prtNOTIFY, "AVPMGR\t+ %s - Perform Action for [%08X] %s.", Action->name, obj, szObjName));
			}
#endif
		}
#endif

		if(PR_SUCC(error))
		{
			tDWORD attempt=0;
			tBOOL  b_enter_io=cFALSE;
			tBOOL  b_io = cFALSE;


			if(PR_SUCC(CALL_SYS_ObjectCheck(obj,obj,IID_IO,PID_ANY,SUBTYPE_ANY,cTRUE)))
			{
				b_io=cTRUE;
				if(0 == CALL_SYS_PropertyGetDWord(obj,propid_creator_engine_pid))
				{
					b_enter_io=cTRUE;
					CALL_SYS_PropertySetDWord(obj,propid_creator_engine_pid,(tDWORD)_this);
				}
			}

			if( b_enter_io )
			{
				CALL_SYS_PropertySetObj(obj,propid_object_parent_io,obj);
				error=CALL_SYS_SendMsg(obj,pmc_ENGINE_IO,pm_ENGINE_IO_MAKE_VIRTUAL_NAME,_this,0,0);
				if(PR_SUCC(error)) error=CALL_SYS_SendMsg(obj,pmc_ENGINE_IO,pm_ENGINE_IO_NAME_CHECK,0,0,0);
				if(PR_SUCC(error)) error=CheckStackAvail(_this->data->StackProbe);
				if(PR_SUCC(error)) error=CALL_SYS_SendMsg(obj,pmc_ENGINE_IO,pm_ENGINE_IO_BEGIN,_this,0,0);
				if(PR_SUCC(error) && NULL==CALL_SYS_PropertyGetObj( obj,propid_object_executable_parent)) 
				{
					tDWORD l_result=0;
					AVLIB_IsExecutableFormatLite((hIO)obj,&l_result);
					if(l_result & cAVLIB_EXECUTABLE_FORMAT)
					{
						CALL_SYS_PropertySetObj( obj,propid_object_executable_parent,obj);
						if(0==(action_class_mask & ENGINE_ACTION_CLASS_ARCHIVE_SFX)
						&& 0!=(action_class_mask & ENGINE_ACTION_CLASS_ARCHIVE))
							CALL_SYS_PropertySetDWord(obj,propid_action_class_mask,action_class_mask & ~ENGINE_ACTION_CLASS_ARCHIVE);
					}
				}
			}
again:
			if(PR_SUCC(error))
			{
				switch(Action->type){
				default:
				case ENGINE_ACTION_TYPE_NO_ACTION:
					break;
				case ENGINE_ACTION_TYPE_SET_OF_ACTIONS:
					{
						tDWORD i;
						for(i=0;i<((tACTION_SetOfActions*)Action)->actions_count;i++){
							error=EngineAction(_this,obj,((tACTION_SetOfActions*)Action)->actions[i]);
							if(PR_FAIL(error))
								break;
						}
					}
					break;
					
				case ENGINE_ACTION_TYPE_PROCESS_OS_ON_IO:
					if( CALL_SYS_PropertyGetDWord(obj,propid_object_infected_parent) )
						break;
					if( CALL_SYS_PropertyGetBool(obj,propid_object_os_passed) )
						break;
					error=EngineActionProcessOSonIO( _this, obj, (tACTION_ProcessOSonIO*)Action );
					break;
				case ENGINE_ACTION_TYPE_CONVERT_IO:
					error=EngineActionConvertIO( _this, obj, (tACTION_ConvertIO*)Action );
					break;
				case ENGINE_ACTION_TYPE_SWITCH_BY_PROPERTY:
					error=EngineActionSwitchByProperty( _this, obj, (tACTION_SwitchByProperty*)Action );
					break;
				case ENGINE_ACTION_TYPE_SUBENGINE_PROCESS:
					if(((tACTION_SubengineProcess*)Action)->engine) // if NOT Disabled
					{
						error=CALL_Engine_Process((hi_Engine)(((tACTION_SubengineProcess*)Action)->engine),obj);
					}
					break;
				}

				if( b_io ){
					if(PR_SUCC(error))
						error=CALL_SYS_PropertyGetDWord(obj, propid_processing_error);
					
					if(PR_SUCC(error))
						error=ProcessRegisteredIOs(_this,obj);

					if(OID_AVP3_DOS_MEMORY == CALL_SYS_PropertyGetDWord(obj,pgOBJECT_ORIGIN))
						error=errOK;
				
					if(PR_FAIL(error)){
						hOBJECT integral_parent_io;
						switch(error){
						case errENGINE_IO_MODIFIED:
							if(CALL_SYS_PropertyGetBool(obj,pgOBJECT_DELETE_ON_CLOSE))
							{
								error=errOK;
								break;
							}
							CALL_SYS_PropertyDelete(obj, propid_processing_error);
							CALL_SYS_PropertyDelete(obj, propid_object_infected_parent);
							CALL_SYS_SendMsg(obj,pmc_AVPMGR_INTERNAL,pm_AVPMGR_INTERNAL_IO_OS_OFFSET_FLUSH,_this,0,0);
							error=CALL_SYS_SendMsg(obj,pmc_ENGINE_IO ,pm_ENGINE_IO_WAS_MODIFIED,_this,0,0);
							
							integral_parent_io=CALL_SYS_PropertyGetObj(obj,propid_integral_parent_io);
							if(integral_parent_io){
								hOBJECT _integral_parent_io;
								while((_integral_parent_io=CALL_SYS_PropertyGetObj(integral_parent_io,propid_integral_parent_io)) != 0){
									integral_parent_io=_integral_parent_io;
								}
								error=CopyIOContent((hIO)integral_parent_io,(hIO)obj);
								if(PR_SUCC(error))
								{
									CALL_SYS_PropertySetDWord(integral_parent_io, propid_processing_error,errENGINE_IO_MODIFIED);
// 6.0 handle this in AVS
//									if(0==CALL_SYS_PropertyGetObj(integral_parent_io,propid_integral_parent_io))
//										CALL_SYS_SendMsg(integral_parent_io,pmc_ENGINE_IO,pm_ENGINE_IO_DISINFECTED,_this,0,0);
								}
								else
								{
									tDWORD l=sizeof(error);
									IncrementInfected(obj,0);
									CALL_SYS_SendMsg(integral_parent_io,pmc_ENGINE_IO,pm_ENGINE_IO_DISINFECTION_FAILED,_this,&error,&l);
								}
							}
							if(PR_SUCC(error)) {
								if(attempt++ < 5)
									goto again;
							}
							break;
						default:
							break;
						}
					}
					l=sizeof(error);
				}
			}
			if( b_enter_io )
			{
				CALL_SYS_SendMsg(obj,pmc_ENGINE_IO,pm_ENGINE_IO_DONE,_this,&error,&l);
				CALL_SYS_PropertySetDWord(obj,propid_creator_engine_pid,0);
			}
		}
		error=HandleProcessingError(_this, obj, error);

//		PR_TRACE(( _this, (PR_FAIL(error) ? prtERROR : prtNOTIFY), "- AVPMGR: %s - Action finished for [%08X]: %terr" , Action->name,  obj, error ));
	}
	return error;
}




// AVP Prague stamp begin( Plugin export method implementation, Engine_SetWriteAccess )
// Parameters are:
tERROR Engine_SetWriteAccess( hOBJECT p_obj, hOBJECT* p_deletable_parent ) 
{
	tERROR error=errOK;
	hOBJECT deletable_object=0;
	hOBJECT parent=0;
	
	if(PR_SUCC(CALL_SYS_ObjectCheck(p_obj,p_obj,IID_IO,PID_ANY,SUBTYPE_ANY,cTRUE)))
	{
		tDWORD mode=CALL_SYS_PropertyGetDWord(p_obj,pgOBJECT_ACCESS_MODE);
		tDWORD change_mode = fACCESS_WRITE;
		if(PID_NATIVE_FIO == OBJ_PID(p_obj))
			change_mode|=fACCESS_CHECK_WRITE;

		if((mode & change_mode) != change_mode)
		{
			mode|=change_mode;
			error=CALL_SYS_PropertySetDWord(p_obj, pgOBJECT_ACCESS_MODE, mode);
		}

		if(PR_SUCC(error) && (change_mode & fACCESS_CHECK_WRITE) )
		{
			tQWORD free_space;
			if(PR_SUCC(PrGetDiskFreeSpace(p_obj,&free_space)))
			{
				tQWORD qwSize;
				if(PR_SUCC(CALL_IO_GetSize((hIO)p_obj, &qwSize, IO_SIZE_TYPE_EXPLICIT)))
				{
					if( qwSize > free_space )
						error = errOUT_OF_SPACE;
				}
			}
		}

		if(PR_SUCC(CALL_SYS_PropertyGetDWord(p_obj,propid_object_readonly_error)))
		{
			if(fAVAIL_DELETE_ON_CLOSE & CALL_SYS_PropertyGetDWord(p_obj, pgOBJECT_AVAILABILITY))
				deletable_object=p_obj;
		}
		
		if(PR_SUCC(error))
		{
			if(error==warnSET_PARENT_ACCESS_MODE_NOT_NEEDED)
				return errOK;
			
			if( PID_WINDISKIO == OBJ_PID(p_obj))
			{
				if (cMEDIA_TYPE_FIXED == CALL_SYS_PropertyGetDWord((hIO)p_obj, plMediaType))
				{
					// skip write access check for fixed media
					PR_TRACE(( g_root, prtIMPORTANT, "AVPMGR\tskip write access check" ));
					deletable_object = 0;
				}
				else
				{
					tDWORD count=0;

					tBYTE  b;
					tQWORD qwSize;
					CALL_IO_GetSize((hIO)p_obj, &qwSize, IO_SIZE_TYPE_EXPLICIT);
					if( qwSize )
						error=CALL_IO_SeekRead((hIO)p_obj,&count,0,&b,1);
					if(PR_SUCC(error) && count)
					{
						error=CALL_IO_SeekWrite((hIO)p_obj,&count,0,&b,1);
						if (PR_SUCC(error))
							error = CALL_IO_Flush((hIO)p_obj);
					}
					// VIK 21.05.04
					if(PR_FAIL(error))
						deletable_object=0;
				}
			}
		}
	}
	if(PR_SUCC(error))
		error=CALL_SYS_PropertyGetDWord(p_obj,propid_object_readonly_error);
	
	parent=CALL_SYS_ParentGet(p_obj,IID_ANY);
	if(parent)
	{
		switch(OBJ_IID(parent))
		{
		case IID_ROOT:
			break;
		case IID_BUFFER:
		case IID_IO:
			if(	OID_REOPEN_DATA == CALL_SYS_PropertyGetDWord(parent,pgOBJECT_ORIGIN))
				break;
			// goto default!
		default:
			{
				tERROR err=Engine_SetWriteAccess( parent, &deletable_object );
				if(PR_SUCC(error))
					error=err;
			}
			break;
		}
	}

	CALL_SYS_PropertySetDWord(p_obj,propid_object_set_write_access_error,error);
	
	if(PR_SUCC(error))
	{
		if(p_deletable_parent && !*p_deletable_parent) 
			*p_deletable_parent=deletable_object;
	}
	else
	{
		if(deletable_object)
			CALL_SYS_PropertySetObj(p_obj,propid_object_readonly_object,deletable_object);
		else
			CALL_SYS_PropertySetObj(p_obj,propid_object_readonly_object,p_obj);

		if(error==errACCESS_DENIED)
			deletable_object=0;

		if(p_deletable_parent)
				*p_deletable_parent=deletable_object; //force deletable_parent
	}
	
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Plugin export method implementation, Engine_AddReopenData )
// Parameters are:
tERROR Engine_AddReopenData( hIO p_io, tPTR p_reopen_data, tDWORD p_user_data ) 
{
	tERROR error=errOK;
	tReopenHeader* rh = (tReopenHeader*)p_reopen_data;
	tDWORD magic=0;
	tDWORD size=0;
	
	if(rh->magic!=cREOPEN_MAGIC)
		return errPARAMETER_INVALID;

	if(rh->size<=sizeof(tReopenHeader))
		return errPARAMETER_INVALID;

	error = CALL_IO_SeekRead(p_io,&size,0,&magic,sizeof(magic));
	if( magic != cREOPEN_MAGIC )
	{
		magic=cREOPEN_MAGIC;
		error=CALL_IO_SetSize(p_io,0);
		if(PR_SUCC(error))
			error=CALL_IO_SeekWrite(p_io,&size,0,&magic,sizeof(magic));
		if(PR_SUCC(error))
			error=CALL_SYS_PropertySetDWord(p_io,pgOBJECT_ORIGIN,OID_REOPEN_DATA);
	}
	if(PR_SUCC(error))
		error=AddNodeReopenRecord( p_io, size, (tReopenRecord*)(rh+1), rh->count, p_user_data);
	
	return error;
}
// AVP Prague stamp end



#define SECURE_PERIOD_PART 10
								// 1/10 => after 1  days   2-4    hours (rescan every 2 hours)
								// 1/10 => after 10 days   24-36  hours (rescan every 1-2 days)
								//         after 30 days   3-4.5  days  (rescan every 3-4 days)
								//         after 3  months 9-13.5 days  (rescan every 1-2 weeks)
								//         after 1  year   36-54  days  (rescan every 1-2 months)

// all parameters are prague time rounded to minutes
// returns PR_SUCC if processing not needed.
tERROR IsSecureToSkip(hENGINE _this, tDWORD tFirstScanDBTime, tDWORD tLastScanDBTime, tDWORD tCurrentDBTime, tDWORD tMandatoryScanPeriod)
{
	tDWORD tSecurePeriod;
	tDWORD tNextScan;

	tSecurePeriod = tLastScanDBTime - tFirstScanDBTime; // period of time(minutes) file was scanned and treated as clean

	if(tMandatoryScanPeriod==0)
	{
		if( !tLastScanDBTime )
			return errNOT_OK;

		if( tSecurePeriod < 24*60 )
			return warnFALSE;

		PR_TRACE(( _this, prtNOTIFY, "AVPMGR\tISTS T(3)" ));
		return errOK; //skip, don't use rescan heuristics for tMandatoryScanPeriod==0
	}

	if (tCurrentDBTime <= tLastScanDBTime)
	{
		PR_TRACE(( _this, prtNOTIFY, "AVPMGR\tISTS T(1)" ));
		return errOK; //already scaned with tis database
	}

	if (tSecurePeriod < tMandatoryScanPeriod)
	{
		PR_TRACE(( _this, prtNOTIFY, "AVPMGR\tISTS F(2)" ));
		return errNOT_OK; //should be scanned with each update while in minimum secure period
	}

	// calculate next scan time
	tNextScan = tLastScanDBTime + tSecurePeriod/SECURE_PERIOD_PART + ((tSecurePeriod/SECURE_PERIOD_PART/2)*(tWORD)PrGetTickCount())/0x10000;
	if (tCurrentDBTime > tNextScan) 
	{
		PR_TRACE(( _this, prtNOTIFY, "AVPMGR\tISTS F(4)" ));
		return errNOT_OK; // need to be rescanned
	}
	// don't need to rescan this time
	PR_TRACE(( _this, prtNOTIFY, "AVPMGR\tISTS T(5)" ));
	return errOK;
}



// AVP Prague stamp begin( Plugin export method implementation, Engine_ICGetData )
// Parameters are:
tERROR Engine_ICGetData( hENGINE p_this, hOBJECT p_obj, tPID p_pid, tPTR* p_icdata_ptr, tCHAR* p_hash_init_buf, tDWORD p_hash_init_size ) 
{
	tERROR error=errOK;
	hICHECKER ic=NULL;
	hi_Engine _this = (hi_Engine)p_this;
	tDWORD count = sizeof(error);
	IC_DATA* icdata;

	switch(CALL_SYS_PropertyGetDWord(p_obj,pgOBJECT_ORIGIN)){
	case OID_AVP3_BOOT:
	case OID_AVP3_MBR:
	case OID_AVP3_DOS_MEMORY:
		return errOBJECT_NOT_FOUND;
	default:
		break;
	}
	switch(p_pid){
		case PID_ICHSTRMS:
		case PID_ICHECKERSA:
			ic=(hICHECKER)_this->data->hICheckerStream;
			break;
		case PID_ICHECKER2:
			ic=(hICHECKER)_this->data->hIChecker;
			break;
		default:
		return errPARAMETER_INVALID;
	}
	
	if(!ic)
		return errOBJECT_NOT_FOUND;

	if(PR_SUCC(error))
	{
		error=CALL_SYS_ObjHeapAlloc(p_obj,(tPTR*)&icdata,sizeof(IC_DATA));
		if(PR_SUCC(error))
		{
			if(_this->data->dwIcheckerInitNumber){
				if(p_hash_init_size)
					p_hash_init_buf[0]^=(tCHAR)_this->data->dwIcheckerInitNumber;
				else{
					p_hash_init_buf = (tCHAR*)&_this->data->dwIcheckerInitNumber;
					p_hash_init_size=sizeof(_this->data->dwIcheckerInitNumber);
				}
			}
			error=CALL_IChecker_GetStatus(ic, 0, (hIO)p_obj, p_hash_init_buf, p_hash_init_size, (void*)icdata, sizeof(IC_DATA));
			if(PR_SUCC(error) && p_icdata_ptr)
			{
				*p_icdata_ptr=icdata;
				if(icdata->dwAVBTimeLastCheck < icdata->dwAVBTimeFirstCheck)
					icdata->dwAVBTimeFirstCheck = icdata->dwAVBTimeLastCheck;
			}
			else
				CALL_SYS_ObjHeapFree(p_obj, icdata);
		}
	}

	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Plugin export method implementation, Engine_ICFreeData )
// Parameters are:
tERROR Engine_ICFreeData( hENGINE p_this, tPTR p_icdata ) 
{
	return CALL_SYS_ObjHeapFree((hOBJECT)p_this, p_icdata);
}
// AVP Prague stamp end



// AVP Prague stamp begin( Plugin export method implementation, Engine_ICCheckData )
// Parameters are:
tERROR Engine_ICCheckData( hENGINE p_this, tPTR p_icdata, tQWORD p_bit_compare_data, tDWORD p_mandatory_scan_days ) 
{
	hi_Engine _this = (hi_Engine)p_this;
	IC_DATA* icdata=(IC_DATA*)p_icdata;
	
	if(!p_icdata)
	{
		PR_TRACE(( p_this, prtNOTIFY, "AVPMGR\tEngine_ICCheckData errPARAMETER_INVALID (NULL)"));
		return errPARAMETER_INVALID;
	}

	p_bit_compare_data|=((tQWORD)_this->data->DB_Verdicts)<<56;

	if(	icdata->dwAVBTimeLastCheck && !(p_bit_compare_data & ~icdata->qwLevel) )
	{
		tDWORD tMandatoryScanPeriod=24*60; //in minutes
		
		if(p_mandatory_scan_days == ENGINE_ICHECK_USE_DATABASE_DAYS)
			tMandatoryScanPeriod*=_this->data->dwQuarantineDays/2;
		else
			tMandatoryScanPeriod*=p_mandatory_scan_days;

		return IsSecureToSkip(p_this, icdata->dwAVBTimeFirstCheck, icdata->dwAVBTimeLastCheck, _this->data->dwBaseTime, tMandatoryScanPeriod);
	}
	else
	{
		PR_TRACE(( p_this, prtNOTIFY, "AVPMGR\tEngine_ICCheckData errNOT_OK %08X %I64X %I64X", icdata->dwAVBTimeLastCheck, p_bit_compare_data, icdata->qwLevel ));
	}
	return errNOT_OK;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Plugin export method implementation, Engine_ICUpdate )
// Parameters are:
tERROR Engine_ICUpdate( hENGINE p_this, hOBJECT p_obj, tPID p_pid, tPTR p_icdata, tQWORD p_bit_compare_data, tBOOL p_delete_status ) 
{
	tERROR error=errOK;
	tDWORD count = sizeof(error);
	hi_Engine _this = (hi_Engine)p_this;
	IC_DATA* icdata=(IC_DATA*)p_icdata;
	hICHECKER ic=NULL;

	if(!p_delete_status)
		error=CALL_SYS_PropertyGetDWord(p_obj, propid_processing_error);

	if(PR_SUCC(error))
	{
		switch(p_pid){
			case PID_ICHSTRMS:
			case PID_ICHECKERSA:
				ic=(hICHECKER)_this->data->hICheckerStream;
				break;
			case PID_ICHECKER2:
				ic=(hICHECKER)_this->data->hIChecker;
				break;
			default:
			return errPARAMETER_INVALID;
		}

		if(p_delete_status)
			error=CALL_IChecker_DeleteStatus(ic, (hIO)p_obj);
		else
		{
			p_bit_compare_data|=((tQWORD)_this->data->DB_Verdicts)<<56;
			icdata->qwLevel=p_bit_compare_data;
			icdata->dwAVBTimeLastCheck = _this->data->dwBaseTime;
			if(icdata->dwAVBTimeFirstCheck == 0)
				icdata->dwAVBTimeFirstCheck = _this->data->dwBaseTime;

			if( p_bit_compare_data & IC_BIT_RANGE_MSTRUSTED )
			{
				icdata->dwAVBTimeFirstCheck = _this->data->dwBaseTime - 60*24*365; // - 1 year
				icdata->qwLevel=(tQWORD)-1;
			}
			error=CALL_IChecker_UpdateStatus(ic, (hIO)p_obj,  (void*)icdata, sizeof(IC_DATA));
		}
	}
	return error;
}
// AVP Prague stamp end


