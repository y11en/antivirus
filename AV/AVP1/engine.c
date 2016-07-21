// AVP Prague stamp begin( Interface header,  )
// -------- Wednesday,  05 February 2003,  16:09 --------
// -------------------------------------------
// Copyright (c) Kaspersky Labs. 1996-2002.
// -------------------------------------------
// Project     -- Kaspersky Anti-Virus
// Sub project -- AV Engine
// Purpose     -- Anti-Virus Plugin Engine 3.0
// Author      -- Graf
// File Name   -- engine.c
// AVP Prague stamp end



// AVP Prague stamp begin( Private definitions,  )
#define Engine_PRIVATE_DEFINITION
// AVP Prague stamp end



// AVP Prague stamp begin( Includes for interface,  )
#include "engine.h"
#include <Prague/prague.h>
// AVP Prague stamp end

#include "static.h"

#if defined (_WIN32)
#include <windows.h>
#endif

#include <Prague/pr_oid.h>
#include <Prague/iface/i_reg.h> 
#include <Prague/plugin/p_win32_nfio.h> 

#include <AV/iface/i_avpg.h>
#include <AV/iface/i_hashct.h>
#include <AV/plugin/p_avpmgr.h>

#include <Extract/plugin/p_windiskio.h>

#include "../../Extract/Wrappers/Dmap/plugin_dmap.h"
#include "../../Prague/IniLib/IniLib.h"

#define AVP_SCANOBJ_USE_NAMED_UNION
#include "ScanAPI/BaseAPI.h"
#include "ScanAPI/Avp_msg.h"
#include "ScanAPI/RetFlags.h"
#include "DupMem.h"
#include "../../Prague/TempFile2/plugin_temporaryio2.h"

#if defined (__tim_environment__)
	#include <../basework/workarea.h>
#else	
	#include "../../AVP32/BaseWork/Workarea.h"
#endif	

#include <stdio.h>

#define IMPEX_IMPORT_LIB
#include <Prague/iface/e_ktime.h>
#include <Prague/iface/e_loader.h> 

#include <AV/plugin/p_l_llio.h> 

#include <stdio.h>
#include <string.h>
#include <Stuff/CalcSum.h>
#include <AVPVndID.h>
#include <Serialize/KLDTSer.h>

#ifdef _WIN32
#ifdef _DEBUG
#undef _TRY_FRAMES_
#else//_RELEASE
#define _TRY_FRAMES_
#endif
#endif

#ifdef _TRY_FRAMES_
#define _TRY_        __try
#define _FINALLY_    __finally
#define _EXCEPT_(x)  __except(x)
#define _EXCEPTION_CODE_ _exception_code()
#define _EXCEPTION_FILTER_ ExceptionFilter(_EXCEPTION_CODE_, (((struct _EXCEPTION_POINTERS *)_exception_info())))

int ExceptionFilter(unsigned int code, struct _EXCEPTION_POINTERS *ep)
{
	PR_TRACE((g_root, prtALWAYS_REPORTED_MSG, "\nAVP1     EXCEPTION 0x%8X at address 0x%X\n",
		code, ep->ExceptionRecord->ExceptionAddress));

	return EXCEPTION_EXECUTE_HANDLER;
}

#else
#define _TRY_
#define _FINALLY_
#define _EXCEPT_(x)  if(0)
#define _EXCEPTION_CODE_ -1
#define _EXCEPTION_FILTER_ EXCEPTION_EXECUTE_HANDLER
#endif

#define DB_NAME_STACK_LEN 0x200


extern BOOL LoadIO();
extern void* SaveWorkArea(void* wa);
extern void  RestoreWorkArea(void* wa, void* s);

#if defined (__unix__)
typedef enum _MEDIA_TYPE {
    Unknown,                // Format is unknown
    F5_1Pt2_512,            // 5.25", 1.2MB,  512 bytes/sector
    F3_1Pt44_512,           // 3.5",  1.44MB, 512 bytes/sector
    F3_2Pt88_512,           // 3.5",  2.88MB, 512 bytes/sector
    F3_20Pt8_512,           // 3.5",  20.8MB, 512 bytes/sector
    F3_720_512,             // 3.5",  720KB,  512 bytes/sector
    F5_360_512,             // 5.25", 360KB,  512 bytes/sector
    F5_320_512,             // 5.25", 320KB,  512 bytes/sector
    F5_320_1024,            // 5.25", 320KB,  1024 bytes/sector
    F5_180_512,             // 5.25", 180KB,  512 bytes/sector
    F5_160_512,             // 5.25", 160KB,  512 bytes/sector
    RemovableMedia,         // Removable media other than floppy
    FixedMedia,             // Fixed hard disk media
    F3_120M_512,            // 3.5", 120M Floppy
    F3_640_512,             // 3.5" ,  640KB,  512 bytes/sector
    F5_640_512,             // 5.25",  640KB,  512 bytes/sector
    F5_720_512,             // 5.25",  720KB,  512 bytes/sector
    F3_1Pt2_512,            // 3.5" ,  1.2Mb,  512 bytes/sector
    F3_1Pt23_1024,          // 3.5" ,  1.23Mb, 1024 bytes/sector
    F5_1Pt23_1024,          // 5.25",  1.23MB, 1024 bytes/sector
    F3_128Mb_512,           // 3.5" MO 128Mb   512 bytes/sector
    F3_230Mb_512,           // 3.5" MO 230Mb   512 bytes/sector
    F8_256_128              // 8",     256KB,  128 bytes/sector
} MEDIA_TYPE, *PMEDIA_TYPE;
#endif

// AVP Prague stamp begin( Interface comment,  )
// Short comments -- Generic interface for object processing
// Short comments -- Generic interface for object processing
// Short comments -- Базовый интерфейс обработки объектов.//eng:Generic interface for object processing.
//    Интерфейс определяет основные правила и методы обработки сложных объектов.
//    Служит для реализации как абстрактного антивирусного ядра, так и подключаемых сканеров.
// Engine interface implementation
// Short comments -- Anti-Virus Plugin 3.0
// AVP Prague stamp end



// AVP Prague stamp begin( Data structure,  )
// Interface Engine. Inner data structure


//hardcoded section - temporary

typedef struct tag_sVerdict
{
	tCHAR*        prefix;
	tDWORD        type;
	tDWORD        danger;
	tDWORD        behaviour;
}sVerdict; 
sVerdict VerdictTable[]=
{
	{ "trojan",           ENGINE_DETECT_TYPE_VIRWARE,   ENGINE_DETECT_DANGER_HIGH, 0 },
	{ "worm",             ENGINE_DETECT_TYPE_VIRWARE,   ENGINE_DETECT_DANGER_HIGH, 0 },
	{ "backdoor",         ENGINE_DETECT_TYPE_VIRWARE,   ENGINE_DETECT_DANGER_HIGH, 0 },
	{ "constructor",      ENGINE_DETECT_TYPE_MALWARE,   ENGINE_DETECT_DANGER_MEDIUM, 0 },
	{ "dos",              ENGINE_DETECT_TYPE_MALWARE,   ENGINE_DETECT_DANGER_MEDIUM, 0 },
	{ "ddos",             ENGINE_DETECT_TYPE_TROJWARE,  ENGINE_DETECT_DANGER_HIGH, 0 },
	{ "exploit",          ENGINE_DETECT_TYPE_MALWARE,   ENGINE_DETECT_DANGER_MEDIUM, 0 },
	{ "flooder",          ENGINE_DETECT_TYPE_MALWARE,   ENGINE_DETECT_DANGER_MEDIUM, 0 },
	{ "hacktool",         ENGINE_DETECT_TYPE_MALWARE,   ENGINE_DETECT_DANGER_MEDIUM, 0 },
	{ "hoax",             ENGINE_DETECT_TYPE_MALWARE,   ENGINE_DETECT_DANGER_LOW, 0 },
	{ "nuker",            ENGINE_DETECT_TYPE_MALWARE,   ENGINE_DETECT_DANGER_MEDIUM, 0 },
	{ "polycryptor",      ENGINE_DETECT_TYPE_MALWARE,   ENGINE_DETECT_DANGER_MEDIUM, 0 },
	{ "polyengine",       ENGINE_DETECT_TYPE_MALWARE,   ENGINE_DETECT_DANGER_MEDIUM, 0 },
	{ "spamtool",         ENGINE_DETECT_TYPE_MALWARE,   ENGINE_DETECT_DANGER_MEDIUM, 0 },
	{ "url",              ENGINE_DETECT_TYPE_UNDEFINED, ENGINE_DETECT_DANGER_MEDIUM, 0 },
	{ "virtool",          ENGINE_DETECT_TYPE_MALWARE,   ENGINE_DETECT_DANGER_MEDIUM, 0 },
	{ "not-a-virus:RiskW",ENGINE_DETECT_TYPE_RISKWARE,  ENGINE_DETECT_DANGER_MEDIUM, 0 },
	{ "not-a-virus:Porn", ENGINE_DETECT_TYPE_PORNWARE,  ENGINE_DETECT_DANGER_LOW, 0 },
	{ "not-a-virus:AdWar",ENGINE_DETECT_TYPE_ADWARE,    ENGINE_DETECT_DANGER_LOW, 0 },
	{ "not-a-virus:Hack", ENGINE_DETECT_TYPE_MALWARE,   ENGINE_DETECT_DANGER_MEDIUM, 0 },
	{ "not-virus",        ENGINE_DETECT_TYPE_UNDEFINED, ENGINE_DETECT_DANGER_MEDIUM, 0 },
	{ "not-a-virus",      ENGINE_DETECT_TYPE_UNDEFINED, ENGINE_DETECT_DANGER_LOW, 0 },
	{ "",                 ENGINE_DETECT_TYPE_VIRWARE,   ENGINE_DETECT_DANGER_HIGH, 0 }
};

typedef struct tag_VerdictLeaf
{
	tDWORD         type;
	tDWORD         danger;
	tDWORD         behaviour;
	tPTR           next;
	tCHAR          prefix[];
}VerdictLeaf;

// DB_Time is treated as tQWORD, therefore it should be aligned on 8byte boundary
typedef struct tag_data 
{
	tDWORD    DB_Count;             // --
	tDATETIME DB_Time;              // --
	hSTRING   DB_Name;              // --
	tDWORD    DB_State;             // --
	tBOOL     UseMultithreadDriver; // --
	hSTRING   DB_LoadName;          // --
	tDATETIME DB_LoadTime;          // --
	tDWORD    MFlags;               // --
	tBOOL     SendMtDebug;          // --
	tVOID**   vndArray;             // --
	tDWORD    vndArrayCount;        // --
	tPTR      pDupData;             // --
	tPTR      Base;                 // --
	tBYTE     DB_Verdicts;          // --

	tBOOL     UseSignCheck; 
	VerdictLeaf* VerdictHead;
	tBOOL     DB_Broken;            // --
	tBOOL     SafeScan;            // --

} data;

// AVP Prague stamp end
static unsigned InstanceCounter=0;

#ifdef _WIN32
static CRITICAL_SECTION LoadCS;
static DWORD    TlsTickCountIndex=(DWORD)-1;
#else 
#define InitializeCriticalSection(x)
#define DeleteCriticalSection(x)
#define EnterCriticalSection(x)
#define LeaveCriticalSection(x)
#define InterlockedIncrement(x) ((*x)++)
#define InterlockedDecrement(x) ((*x)--)
#endif


// AVP Prague stamp begin( Object declaration,  )

typedef struct tag_hi_Engine 
{
	const iEngineVtbl* vtbl; // pointer to the "Engine" virtual table
	const iSYSTEMVTBL* sys;  // pointer to the "SYSTEM" virtual table
	data*              data; // pointer to the "Engine" data structure
} *hi_Engine;

// AVP Prague stamp end



// AVP Prague stamp begin( Internal method table prototypes,  )
tERROR pr_call Engine_ObjectInit( hi_Engine _this );
tERROR pr_call Engine_ObjectInitDone( hi_Engine _this );
tERROR pr_call Engine_ObjectPreClose( hi_Engine _this );
// AVP Prague stamp end
tERROR pr_call Engine_MsgReceive( hi_Engine _this, tDWORD msg_cls_id, tDWORD msg_id, hOBJECT send_point, hOBJECT ctx, hOBJECT receive_point, tVOID* par_buf, tDWORD* par_buf_len );



tERROR MakeNameType(hi_Engine _this, hOBJECT obj, hOBJECT obj_src);
tBOOL VndInit( hi_Engine _this, const char* filename);
tVOID VndDestroy( hi_Engine _this );

#ifdef _DEBUG
static tDWORD engine_alloc_counter=0;
#define DEBUG_ALLOC_COUNTER_INC(ptr) if(ptr) engine_alloc_counter++;
#define DEBUG_ALLOC_COUNTER_DEC(ptr) if(ptr) engine_alloc_counter--;
#else
#define DEBUG_ALLOC_COUNTER_INC(ptr)
#define DEBUG_ALLOC_COUNTER_DEC(ptr)
#endif

tVOID* obj_malloc(tUINT size){
	tVOID* ptr=NULL;
	if(g_root){
    hOBJECT obj;
    if( 0 != (obj=(hOBJECT)Get_CurrentIO()) ) 
		CALL_SYS_ObjHeapAlloc(obj,&ptr,size);
	else if( 0 != (obj=(hOBJECT)Get_AVP1()) )
		CALL_SYS_ObjHeapAlloc(obj,&ptr,size);
    else if ( !ptr )
		CALL_SYS_ObjHeapAlloc(g_root,&ptr,size);
  }
	else{ 
		ptr=malloc(size);
		if(ptr)	memset(ptr,0,size);
	}
	
	DEBUG_ALLOC_COUNTER_INC(ptr);
	return ptr;
}

tVOID  obj_free(tVOID* ptr){
	if(ptr){
		tERROR error=errUNEXPECTED;
		if(g_root) error=CALL_SYS_ObjHeapFree(g_root,ptr);
		if(PR_FAIL(error)) 
			free(ptr);
	}
	DEBUG_ALLOC_COUNTER_DEC(ptr);
}




// AVP Prague stamp begin( Internal method table,  )
static iINTERNAL i_Engine_vtbl = 
{
	(tIntFnRecognize)        NULL,
	(tIntFnObjectNew)        NULL,
	(tIntFnObjectInit)       Engine_ObjectInit,
	(tIntFnObjectInitDone)   Engine_ObjectInitDone,
	(tIntFnObjectCheck)      NULL,
	(tIntFnObjectPreClose)   Engine_ObjectPreClose,
	(tIntFnObjectClose)      NULL,
	(tIntFnChildNew)         NULL,
	(tIntFnChildInitDone)    NULL,
	(tIntFnChildClose)       NULL,
	(tIntFnMsgReceive)       Engine_MsgReceive,
	(tIntFnIFaceTransfer)    NULL
};
// AVP Prague stamp end



// AVP Prague stamp begin( Interface function forward declarations,  )
typedef   tERROR (pr_call *fnpEngine_Process)           ( hi_Engine _this, hOBJECT obj );   // -- Обработать объект.//eng:Process object.;
typedef   tERROR (pr_call *fnpEngine_GetAssociatedInfo) ( hi_Engine _this, hOBJECT obj, tDWORD info_id, tDWORD param, tPTR buffer, tDWORD size, tDWORD* out_size ); // -- Returns runtime information about processing object;
typedef   tERROR (pr_call *fnpEngine_GetExtendedInfo)   ( hi_Engine _this, tDWORD info_id, tDWORD param, tPTR buffer, tDWORD size, tDWORD* out_size ); // -- Returns information about engine abilities;
typedef   tERROR (pr_call *fnpEngine_LoadConfiguration) ( hi_Engine _this, hREGISTRY reg, tRegKey key ); // -- Инициализирует конфигурацию Engine ;
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
	Engine_Process,
		Engine_GetAssociatedInfo,
		Engine_GetExtendedInfo,
		Engine_LoadConfiguration
};
// AVP Prague stamp end


#define CALL_SYS_SendMsg_Callback(_obj, _cls, _msg, _ctx, _buf, _len) \
	DupCallbackCall(error=CALL_SYS_SendMsg(_obj,_cls,_msg,_ctx,_buf,_len))

tERROR CALL_SYS_SendMsg_ObjectError(hi_Engine _this, hOBJECT obj, tDWORD cls,tDWORD msg, tPTR ctx, tPTR buf, tDWORD* len)
{
	tERROR error;
	CALL_SYS_SendMsg_Callback(obj,cls,msg,ctx,buf,len);

	if(PR_FAIL(error)){
		if(error==errOPERATION_CANCELED) 	
			BaseAvpCancelProcessObject( _this->data->Base,error);
		CALL_SYS_PropertySetDWord(obj, propid_processing_error, error);
	}
	else error=errOK;
	return error;
}


// AVP Prague stamp begin( Forwarded property methods declarations,  )
tERROR pr_call Engine_PROP_PropertyGetStr( hi_Engine _this, tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );
// AVP Prague stamp end



// AVP Prague stamp begin( Global property variable declaration,  )
// Interface "Engine". Static(shared) property table variables
// Interface "Engine". Global(shared) property table variables
// AVP Prague stamp end



// AVP Prague stamp begin( Interface version,  )
#define Engine_VERSION ((tVERSION)1)
// AVP Prague stamp end



// AVP Prague stamp begin( Property table,  )
mPROPERTY_TABLE(Engine_PropTable)
	mSHARED_PROPERTY( pgINTERFACE_VERSION, Engine_VERSION )
	mSHARED_PROPERTY_PTR( pgINTERFACE_COMMENT, "AV Plugin 3.0", 14 )
	mSHARED_PROPERTY( pgENGINE_VERSION, ((tVERSION)(1)) )
	mLOCAL_PROPERTY_FN( pgENGINE_DB_NAME, Engine_PROP_PropertyGetStr, NULL )
	mLOCAL_PROPERTY_BUF( pgENGINE_DB_COUNT, data, DB_Count, cPROP_BUFFER_READ )
	mLOCAL_PROPERTY_BUF( pgENGINE_DB_TIME, data, DB_Time, cPROP_BUFFER_READ )
	mLOCAL_PROPERTY_BUF( pgENGINE_DB_STATE, data, DB_State, cPROP_BUFFER_READ )
	mLOCAL_PROPERTY_BUF( pgENGINE_DB_LOAD_TIME, data, DB_LoadTime, cPROP_BUFFER_READ )
	mLOCAL_PROPERTY_BUF( pgENGINE_DB_VERDICTS, data, DB_Verdicts, cPROP_BUFFER_READ )
	mLOCAL_PROPERTY_FN( pgENGINE_DB_LOAD_NAME, Engine_PROP_PropertyGetStr, NULL )
	mSHARED_PROPERTY( pgENGINE_SUBENGINE_TABLE, ((tPTR)(0)) )
	mSHARED_PROPERTY( pgENGINE_SUBENGINE_COUNT, ((tDWORD)(0)) )
	mLOCAL_PROPERTY_BUF( plENGINE_USE_MULTITHREAD_DRIVER, data, UseMultithreadDriver, cPROP_BUFFER_READ_WRITE | cPROP_WRITABLE_ON_INIT )
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
		PID_AVP1,                               // plugin identifier
		0x00000000,                             // subtype identifier
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
			PR_TRACE( (root,prtDANGER,"Engine(IID_ENGINE) registered [error=0x%08x]",error) );
	#endif // _PRAGUE_TRACE_

	PR_TRACE_A1( root, "Leave \"Engine::Register\" method, ret tERROR = 0x%08x", error );
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Property method implementation, PropertyGetStr )
// Interface "Engine". Method "Get" for property(s):
//  -- ENGINE_DB_NAME
//  -- ENGINE_DB_LOAD_NAME
tERROR pr_call Engine_PROP_PropertyGetStr( hi_Engine _this, tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size )
{
	tERROR error = errOK;
	tCODEPAGE cp=cCP_ANSI;
	PR_TRACE_A0( _this, "Enter *GET* multyproperty method Engine_PROP_PropertyGetStr" );
	DupRegisterThread(_this->data->pDupData);

	if(out_size) *out_size = 0;

	switch ( prop )
	{
		case pgENGINE_DB_NAME:
			error=CALL_String_ExportToBuff(_this->data->DB_Name,out_size,cSTRING_WHOLE,buffer,size,cp,cSTRING_Z);
			break;

		case pgENGINE_DB_LOAD_NAME:
			error=CALL_String_ExportToBuff(_this->data->DB_LoadName,out_size,cSTRING_WHOLE,buffer,size,cp,cSTRING_Z);
			break;

		default:
			error=errPROPERTY_NOT_FOUND;
			break;
	}

	PR_TRACE_A2( _this, "Leave *GET* multyproperty method Engine_PROP_PropertyGetStr, ret tDWORD = %u(size), error = 0x%08x", *out_size, error );
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Property method implementation, PropertySetStr )
// Interface "Engine". Method "Set" for property(s):
//  -- ENGINE_DB_NAME
tERROR pr_call Engine_PROP_PropertySetStr( hi_Engine _this, tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size )
{
	tERROR error = errOK;
	tCODEPAGE cp=cCP_ANSI;
	PR_TRACE_A0( _this, "Enter *SET* multyproperty method Engine_PROP_PropertySetStr" );
	DupRegisterThread(_this->data->pDupData);
	
	switch ( prop )
	{
		case pgENGINE_DB_NAME:
			error=CALL_String_ImportFromBuff(_this->data->DB_Name,out_size,buffer,size,cp,cSTRING_Z);
			break;

		default:
			error=errPROPERTY_NOT_FOUND;
			break;
	}

	PR_TRACE_A2( _this, "Leave *SET* multyproperty method Engine_PROP_PropertySetStr, ret tDWORD = %u(size), error = 0x%08x", *out_size, error );
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Internal (kernel called) interface method implementation, ObjectInit )
// Extended method comment
//  Kernel notifies an object about successful creating of object
// Behaviour comment
//  Initializes internal object data
// Result comment
//  Returns value differ from errOK if object cannot be initialized
// Parameters are:
tERROR pr_call Engine_ObjectInit( hi_Engine _this )
{
	tERROR error = errOK;
	hHEAP hHeap;
	PR_TRACE_A0( _this, "Enter \"Engine::ObjectInit\" method" );

	if(1==InterlockedIncrement(&InstanceCounter)){
		InitializeCriticalSection(&LoadCS);
#ifdef _WIN32
		TlsTickCountIndex=TlsAlloc();
		if(TlsTickCountIndex==(DWORD)-1)
			error=errNOT_ENOUGH_MEMORY;
#endif
	}
	_this->data->UseSignCheck=1;

	if(PR_SUCC(error)) error=CALL_SYS_ObjectCreateQuick(_this, &_this->data->DB_Name,IID_STRING,PID_ANY,SUBTYPE_ANY );
	if(PR_SUCC(error)) error=CALL_SYS_ObjectCreateQuick(_this, &_this->data->DB_LoadName,IID_STRING,PID_ANY,SUBTYPE_ANY );
	if(PR_SUCC(error)) error=CALL_SYS_ObjectCreateQuick(_this, &hHeap, IID_HEAP, PID_LOADER, SUBTYPE_ANY);
	if(PR_SUCC(error))
	{
		if (PR_FAIL(error=CALL_SYS_PropertySetDWord(_this, pgOBJECT_HEAP, (tDWORD)hHeap)))
			CALL_SYS_ObjectClose(hHeap);
	}
	PR_TRACE_A1( _this, "Leave \"Engine::ObjectInit\" method, ret tERROR = 0x%08x", error );
	return error;
}
// AVP Prague stamp end



extern DWORD callbackRecords; 

static tERROR SyncLoadBase( hi_Engine _this, BOOL load)
{
	tERROR error=errOK;
	tBYTE setname[DB_NAME_STACK_LEN]="UNLOAD";
	if(load){
		tDWORD l=sizeof(DWORD);
		CALL_SYS_SendMsg(_this,pmc_ENGINE_OPTIONS, pm_ENGINE_OPTIONS_SEND_LOAD_RECORD_MESSAGE,_this,&callbackRecords,&l);
		error=CALL_String_ExportToBuff(_this->data->DB_Name,0,cSTRING_WHOLE,setname,DB_NAME_STACK_LEN,cCP_ANSI,cSTRING_Z);
	}

	EnterCriticalSection(&LoadCS);
	if(!DupEnter(NULL)){
		PR_TRACE(( _this, prtERROR, "AVP1     SYNCRONYZATION ERROR" ));
		PR_TRACE(( _this, prtERROR, "AVP1     BASE LOADING ABORTED" ));
		error=errLOCKED;
	}
	else {
		error=errENGINE_BAD_AVP_SET;
		PR_TRACE(( _this, prtALWAYS_REPORTED_MSG, "AVP1 Base load beginning: %s", setname ));
		_TRY_
		{
			if(BaseAvpLoadBase(_this->data->Base, load?setname:0)){
				if(_this->data->DB_Broken == 0)
					error=errOK;
			}
		}
		_FINALLY_
		{
			char buf[MAX_PATH];
			tDWORD l=1+sprintf_s(buf,countof(buf),"AVP1\tLoadBase done: %d",_this->data->DB_Count);
			CALL_SYS_SendMsg(_this,pmc_ENGINE_LOAD, pm_ENGINE_LOAD_OUTPUT_STRING,_this,buf,&l);
			PR_TRACE(( _this, prtALWAYS_REPORTED_MSG, buf ));
			if(PR_FAIL(error))
				PR_TRACE(( _this, prtERROR, "AVP1\tLoadBase ERROR: %terr", error ));
			if(!DupExit(NULL))
				error=errENGINE_MT_DRIVER_NOT_LOADED;
		}
	}
	LeaveCriticalSection(&LoadCS);
	return error;
}

tERROR UnloadVerdicts( hi_Engine _this)
{
	VerdictLeaf* verdict=_this->data->VerdictHead;
	_this->data->VerdictHead=0;
	while(verdict)
	{ 
		VerdictLeaf* next=verdict->next;
		CALL_SYS_ObjHeapFree(_this,verdict);
		verdict=next;
	}
	return errOK;
}

tERROR LoadVerdicts( hi_Engine _this, tCHAR* name)
{
	tERROR error=errNOT_FOUND;
	VerdictLeaf* verdict;

	UnloadVerdicts(_this);

	if(name)
	{
		error=errOK;
		if(SignCheck)
			error=SignCheck((hOBJECT)name,1,0,0,0,0);
	}

	if(PR_SUCC(error))
	{
		hIO io;
		if (PR_SUCC(error=CALL_SYS_ObjectCreate(_this, &io, IID_IO, PID_WIN32_NFIO, 0)))
		{
			CALL_SYS_PropertySetDWord(io, pgOBJECT_OPEN_MODE, fOMODE_OPEN_IF_EXIST);
			CALL_SYS_PropertySetDWord(io, pgOBJECT_ACCESS_MODE, fACCESS_READ);
			CALL_SYS_PropertySetStr(  io,0,pgOBJECT_FULL_NAME,name,strlen(name)+1,cCP_ANSI);
			if(PR_SUCC(error=CALL_SYS_ObjectCreateDone(io)))
			{
				sIniSection* ini=0;
				if(PR_SUCC(error=IniLoad(io,&ini,NULL)))
				{
					VerdictLeaf* prev=_this->data->VerdictHead;
					sIniLine* verdict_line=NULL;
					sIniSection* s_behaviour=IniGetSection(ini,"behaviour", NULL);
					if(s_behaviour)
						verdict_line=s_behaviour->line_head;

					while(verdict_line)
					{
						tCHAR* behaviour_param[2];
						if(2==IniParseValue(behaviour_param,2,verdict_line->value))
						{
							tCHAR* line;
							char* end;
							tDWORD danger=ENGINE_DETECT_DANGER_HIGH;
							tDWORD type=ENGINE_DETECT_TYPE_UNDEFINED;
							tDWORD behaviour=0;
							tSIZE_T verdictPrefixSz;

							behaviour=strtoul(behaviour_param[0],&end,10);

							line=IniGetValue(ini,"verdicts",behaviour_param[1], cFALSE);
							if(!line)
								line=IniGetValue(ini,"verdicts","UNDEFINED", cFALSE);
							if(line)
							{
								tCHAR* type_param[2];
								if(2==IniParseValue(type_param,2,line))
								{
									type=strtoul(type_param[0],&end,10);
									line=IniGetValue(ini,"danger",type_param[1], cFALSE);
									if(line)
										danger=strtoul(line,&end,10);
								}
							}
							
							verdictPrefixSz = strlen(verdict_line->name)+1;
							if(PR_SUCC(error=CALL_SYS_ObjHeapAlloc(_this,&verdict,sizeof(VerdictLeaf)+verdictPrefixSz)))
							{
								if(!_stricmp(verdict_line->name,"DEFAULT"))
									verdict_line->name[0]=0;
								strcpy_s(verdict->prefix,verdictPrefixSz,verdict_line->name);
								verdict->danger=danger;
								verdict->type=type;
								verdict->behaviour=behaviour;

								if(prev==0)
									_this->data->VerdictHead=verdict;
								else
									prev->next=verdict;
								prev=verdict;
							}
						}
						verdict_line=verdict_line->next;
					}
					IniFree(ini);
				}
			}
			CALL_SYS_ObjectClose(io);
		}
	}

	if(PR_FAIL(error))
	{
		int i=sizeof(VerdictTable)/sizeof(sVerdict);
		UnloadVerdicts(_this);
		while(i--)
		{
			tSIZE_T verdictPrefixSz = strlen(VerdictTable[i].prefix)+1;
			if(PR_SUCC(error=CALL_SYS_ObjHeapAlloc(_this,&verdict,sizeof(VerdictLeaf)+verdictPrefixSz)))
			{
				strcpy_s(verdict->prefix,verdictPrefixSz,VerdictTable[i].prefix);
				verdict->danger=VerdictTable[i].danger;
				verdict->type=VerdictTable[i].type;
				verdict->behaviour=VerdictTable[i].behaviour;
				verdict->next=_this->data->VerdictHead;
				_this->data->VerdictHead=verdict;
			}
		}
	}
	return error;
}


// AVP Prague stamp begin( Internal (kernel called) interface method implementation, ObjectInitDone )
// Extended method comment
//  Notification that all necessary properties have been set and object must go to operation state
// Result comment
//  Returns value differ from errOK if object cannot function properly
// Parameters are:
tERROR pr_call Engine_ObjectInitDone( hi_Engine _this )
{
	tERROR error = errOK;
	tDWORD l=sizeof(tBOOL);
	PR_TRACE_A0( _this, "Enter \"Engine::ObjectInitDone\" method" );
	CALL_SYS_SendMsg(_this,pmc_ENGINE_OPTIONS,pm_ENGINE_OPTIONS_SEND_MT_DEBUG_MESSAGE,_this,&_this->data->SendMtDebug,&l);
	CALL_SYS_SendMsg(_this,pmc_ENGINE_OPTIONS,pm_ENGINE_OPTIONS_USE_MULTITHREAD_DRIVER,_this,&_this->data->UseMultithreadDriver,&l);
	CALL_SYS_SendMsg(_this,pmc_ENGINE_OPTIONS,pm_ENGINE_OPTIONS_USE_SIGN_CHECK,_this,&_this->data->UseSignCheck,&l);
	if(_this->data->UseSignCheck == cFALSE)
		SignCheck=NULL;
	
	_this->data->pDupData=DupInit(_this->data->UseMultithreadDriver);

	if(_this->data->pDupData == NULL)
	{
		error=errENGINE_MT_DRIVER_NOT_LOADED;
	}
	else
	{
		Set_AVP1(_this);
		EnterCriticalSection(&LoadCS);
		if(PR_SUCC(error))	if(!LoadIO()) error=errENGINE_BAD_AVP1_CONFIGURATION;
		if(PR_SUCC(error))	_this->data->Base=BaseAvpInit();
		LeaveCriticalSection(&LoadCS);
		
		if(NULL==_this->data->Base)	error=errENGINE_BAD_AVP1_CONFIGURATION;
			
		if(PR_FAIL(error)){
			DupDestroy(_this->data->pDupData);
			_this->data->pDupData=NULL;
		}
	}
	CALL_SYS_RegisterMsgHandler( _this,pmc_DIAGNOSTIC, rmhDECIDER, g_root, IID_ANY, PID_ANY, IID_ANY, PID_ANY );

	if( PR_SUCC(error) )
	{
		hSTRING str = NULL;

		_this->data->SafeScan = cTRUE;
		if(PR_SUCC(CALL_SYS_ObjectCreateQuick(_this, &str, IID_STRING,PID_ANY,SUBTYPE_ANY )) )
		{
			tSYMBOL c = 0;
			CALL_String_AddFromBuff(str,0,"%SafeScan%",0,cCP_ANSI,0);
			CALL_SYS_SendMsg(_this,pmc_PRODUCT_ENVIRONMENT, pm_EXPAND_ENVIRONMENT_STRING, (hOBJECT)str,0,0);
			CALL_String_Symbol(str, &c, cSTRING_WHOLE, 0, cCP_ANSI, 0);
			if( c == '0' )
				_this->data->SafeScan = cFALSE;
			CALL_SYS_ObjectClose(str);
		}
	}

	PR_TRACE_A1( _this, "Leave \"Engine::ObjectInitDone\" method, ret tERROR = 0x%08x", error );
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
	DupRegisterThread(_this->data->pDupData);
	
	if(_this->data->DB_State){
		PR_TRACE(( _this, prtALWAYS_REPORTED_MSG, "AVP1 base unloading..." ));
		error=SyncLoadBase(_this, 0);
		PR_TRACE(( _this, prtALWAYS_REPORTED_MSG, "AVP1 base unloaded." ));
		VndDestroy( _this );
		PR_TRACE(( _this, prtALWAYS_REPORTED_MSG, "VND destroyed." ));
		UnloadVerdicts(_this);
		PR_TRACE(( _this, prtALWAYS_REPORTED_MSG, "Verdicts destroyed." ));

		_this->data->DB_State=0;
	}
	if(_this->data->Base){
		EnterCriticalSection(&LoadCS);
		BaseAvpDestroy(_this->data->Base);
		LeaveCriticalSection(&LoadCS);
	}


	PR_TRACE(( _this, prtALWAYS_REPORTED_MSG, "AVP 30 engine destroyed." ));

	if(_this->data->DB_Name){
		CALL_SYS_ObjectClose((hOBJECT)_this->data->DB_Name);
	}
	if(_this->data->DB_LoadName){
		CALL_SYS_ObjectClose((hOBJECT)_this->data->DB_LoadName);
	}
	
	if(_this->data->pDupData){
		DupDestroy(_this->data->pDupData);
		PR_TRACE(( _this, prtALWAYS_REPORTED_MSG, "DupMem destroyed." ));
	}

	if(0==InterlockedDecrement(&InstanceCounter)){
		DeleteCriticalSection(&LoadCS);
#ifdef _WIN32
		if(TlsTickCountIndex!=(DWORD)-1)
		{
			TlsFree(TlsTickCountIndex);
			TlsTickCountIndex=(DWORD)-1;
		}
#endif
	}
	
	if(PR_SUCC(error))
		PR_TRACE(( _this, prtIMPORTANT, "AVP1 successfully unloaded." ));
	else
		PR_TRACE(( _this, prtERROR, "AVP1 unload error." ));

	PR_TRACE_A1( _this, "Leave \"Engine::ObjectPreClose\" method, ret tERROR = 0x%08x", error );
	return error;
}
// AVP Prague stamp end


tERROR pr_call Engine_MsgReceive( hi_Engine _this, tDWORD msg_cls_id, tDWORD msg_id, hOBJECT send_point, hOBJECT ctx, hOBJECT receive_point, tVOID* par_buf, tDWORD* par_buf_len )
{
	if( msg_cls_id == pmc_DIAGNOSTIC )
		if ( msg_id == pm_DIAGNOSTIC_IS_SAFE_IP )
			if( par_buf && 	_this->data->DB_Verdicts )
				if( BaseIsLinkedCode(_this->data->Base, *(PCHAR*)par_buf) )
					return errOK_DECIDED;
	return errOK;
}

#ifdef FULL_pmc_ENGINE_ACTION
tERROR ActionRequest( hi_Engine _this, hOBJECT obj, tDWORD cls)
{
	tERROR error=errOK;
	tDWORD len=sizeof(cls);
	error=CALL_SYS_SendMsg(obj,pmc_ENGINE_ACTION,ENGINE_ACTION_TYPE_NO_ACTION,_this,&cls,&len);
	if(error==errOPERATION_CANCELED) 	
		BaseAvpCancelProcessObject( _this->data->Base,error);
	return error; 
}

#define MFlags2Class(a,b) if(cls&b){ if(PR_SUCC(ActionRequest(_this,obj,b))) *mflags|=a;}else{*mflags&=~a;}
#define MFlags2ClassRev(a,b) if(!(cls&b)){*mflags|=a;}else{ if(PR_SUCC(ActionRequest(_this,obj,b))) *mflags&=~a;}
#else
#define MFlags2Class(a,b) if(cls&b){ *mflags|=a;} else{ *mflags&=~a;}
#define MFlags2ClassRev(a,b) if(!(cls&b)){ *mflags|=a;} else{ *mflags&=~a;}
#endif

tERROR ActionClassToMFlags( hi_Engine _this, hOBJECT obj, tDWORD *mflags){
	tDWORD cls=CALL_SYS_PropertyGetDWord(obj,propid_action_class_mask);

	MFlags2Class(    MF_ARCHIVED,      ENGINE_ACTION_CLASS_ARCHIVE);
	MFlags2Class(    MF_PACKED,        ENGINE_ACTION_CLASS_PACKED);
	MFlags2Class(    MF_CA,            ENGINE_ACTION_CLASS_VIRUS_HEURISTIC);
	MFlags2Class(    MF_MAILBASES,     ENGINE_ACTION_CLASS_MAIL_DATABASE);
	MFlags2Class(    MF_MAILPLAIN,     ENGINE_ACTION_CLASS_MAIL_PLAIN);
	MFlags2Class(    MF_SFX_ARCHIVED,  ENGINE_ACTION_CLASS_ARCHIVE_SFX);
	MFlags2ClassRev( MF_SKIP_EMBEDDED, ENGINE_ACTION_CLASS_OLE_EMBEDDED);
	MFlags2ClassRev( MF_D_DENYACCESS,  ENGINE_ACTION_CLASS_OBJECT_MODIFY);
	return errOK;
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
	tDWORD count;
	AVPScanObject* so;
	tORIG_ID origin;
	tBOOL recursed=0;
	tBOOL process=cTRUE;
	CWorkArea* wa;
	void* s=NULL;
	tPROPID nameprop=0;

	if( !(_this->data->DB_State & cENGINE_DB_STATE_FUNCTIONAL))
		return errENGINE_BAD_CONFIGURATION;

	DupRegisterThread(_this->data->pDupData);
	
	error=CALL_SYS_ObjHeapAlloc(obj,(tVOID *) &so,sizeof(AVPScanObject));
	if(PR_FAIL(error)) return error;
//	memset(so,0,sizeof(AVPScanObject));
	
	PR_TRACE_A0( _this, "Enter \"Engine::Process\" method" );
	
	CALL_SYS_PropertySetDWord(obj, propid_processing_error,0);
	
	if((count=CALL_SYS_PropertySize(obj,propid_virtual_name))!=0){
		nameprop=propid_virtual_name;
	}
	else if((count=CALL_SYS_PropertySize(obj,pgOBJECT_FULL_NAME))!=0){
		nameprop=pgOBJECT_FULL_NAME;
	}
	else{
		error=errPROPERTY_NOT_FOUND;
		goto ret;
	}

	if(count < 0x200){
		error=CALL_SYS_PropertyGetStr(obj,NULL,nameprop,so->Name,0x200,cCP_ANSI);
	}else{
		tCHAR* buf;
		error=CALL_SYS_ObjHeapAlloc(obj,(tPTR*)&buf,count);
		if(PR_SUCC(error)){
			error=CALL_SYS_PropertyGetStr(obj,NULL,nameprop,buf,count,cCP_ANSI);
			if(PR_SUCC(error)){
				strncpy_s(so->Name,countof(so->Name),buf,0x100);
				strcat_s(so->Name,countof(so->Name)," ... ");
				strcat_s(so->Name,countof(so->Name),buf+count-0xF0);

			}
			CALL_SYS_ObjHeapFree(obj,buf);
		}
	}
	if(PR_FAIL(error))	goto ret;

	if(!*so->Name)
	{
		sprintf_s(so->Name,countof(so->Name),"Prague IO Object with unknown name: 0x%08X",(tUINT)obj);
	}

	so->MFlags=_this->data->MFlags;

	ActionClassToMFlags( _this,obj,&so->MFlags);

	if(PR_FAIL(CALL_SYS_PropertyGet(obj,&count,pgOBJECT_ORIGIN,&origin,sizeof(origin))))
		origin=OID_GENERIC_IO;

	switch(origin){

	case OID_AVP3_DOS_MEMORY:
#if !defined (__unix__)	
		so->SO1.SType=OT_MEMORY;
		so->Size = 1024;  
		break;
#endif /* __unix__ */

	case OID_AVP3_BOOT:
	case OID_AVP3_MBR:
		error=errNOT_SUPPORTED;
		break;

	case OID_LOGICAL_DRIVE:
		so->SO1.SType=OT_BOOT;
		so->Size=CALL_SYS_PropertyGetDWord(obj,plBytesPerSector);
		so->SO1.Drive=CALL_SYS_PropertyGetDWord(obj,plDriveNumber);
		if(CALL_SYS_PropertyGetBool(obj,plIsPartition) )
			so->SO1.Disk=0x80;
		else
			so->SO1.Disk=CALL_SYS_PropertyGetDWord(obj,plDriveNumber);
		

		if(0x200 == CALL_SYS_PropertyGetDWord(obj,plBytesPerSector))
		{
			tBYTE buf[0x200];
			tDWORD count;
			if(PR_SUCC(CALL_IO_SeekRead((hIO)obj,&count,0,buf,0x200)))
			{
				if(count==0x200	&& *(WORD*)(buf + 0x1fe)==0xAA55)
				{
					switch(buf[0x15])
					{
					case 0xf0:          // "high-density 3.5-inch floppy disk";
					case F5_1Pt2_512:   // "5.25, 1.2MB,  512 bytes/sector";
					case F3_1Pt44_512:  // "3.5,  1.44MB, 512 bytes/sector";
					case F3_2Pt88_512:  // "3.5,  2.88MB, 512 bytes/sector";
					case F3_720_512:    // "3.5,  720KB,  512 bytes/sector";
					case F5_360_512:    // "5.25, 360KB,  512 bytes/sector";
					case F5_320_512:    // "5.25, 320KB,  512 bytes/sector";
					case F5_180_512:    // "5.25, 180KB,  512 bytes/sector";
					case F5_160_512:    // "5.25, 160KB,  512 bytes/sector";
					case F3_640_512:    // "3.5 ,  640KB,  512 bytes/sector";
					case F5_640_512:    // "5.25,  640KB,  512 bytes/sector";
					case F5_720_512:    // "5.25,  720KB,  512 bytes/sector";
					case F3_1Pt2_512:   // "3.5 ,  1.2Mb,  512 bytes/sector";
						{
							tDWORD Cylinders = 0;
							tDWORD BytesPerSector=*(WORD*)(buf+ 0x0b);
							tDWORD SectorsPerTrack=*(WORD*)(buf+ 0x18);
							tDWORD TracksPerCylinder=*(WORD*)(buf+ 0x1a);
							tDWORD TotalSectors=*(WORD*)(buf+ 0x13);
							
							if (TracksPerCylinder && SectorsPerTrack)
								Cylinders = TotalSectors/SectorsPerTrack/TracksPerCylinder;

							CALL_SYS_PropertySetDWord(obj,propid_boot_BytesPerSector,BytesPerSector);
							CALL_SYS_PropertySetDWord(obj,propid_boot_SectorsPerTrack,SectorsPerTrack);
							CALL_SYS_PropertySetDWord(obj,propid_boot_TracksPerCylinder,TracksPerCylinder);
							CALL_SYS_PropertySetQWord(obj,propid_boot_Cylinders,Cylinders);
							so->SO1.Drive=0;
							so->SO1.Disk=0;
						}

						PR_TRACE(( _this, prtIMPORTANT, "AVP1\tBootDisk - known format %x", (DWORD) buf[0x15]));
						break;
					default:
						{
							PR_TRACE(( _this, prtIMPORTANT, "AVP1\tBootDisk - unknown format %x", (DWORD) buf[0x15]));
						}
						break;
					}
				}
			}
		}



		break;

	case OID_PHYSICAL_DISK:
		so->SO1.SType=OT_MBR;
		so->MFlags|=MF_ALLENTRY;
		so->Size=CALL_SYS_PropertyGetDWord(obj,plBytesPerSector);
		so->SO1.Disk=CALL_SYS_PropertyGetDWord(obj,plDriveNumber);

		if(cFILE_DEVICE_DISK == CALL_SYS_PropertyGetDWord(obj,plDriveType)
		&& (cMEDIA_TYPE_FIXED == CALL_SYS_PropertyGetDWord(obj,plMediaType)
		||  cMEDIA_TYPE_REMOVABLE == CALL_SYS_PropertyGetDWord(obj,plMediaType) ))
			so->SO1.Disk |= 0x80;
		break;

	case OID_COMMAND_RELOAD_BASE:
		error=SyncLoadBase(_this,1);
		goto ret;
		
	case OID_MEMORY_PROCESS_MODULE:
		so->SO1.SType=OT_MEMORY_FILE;
		break;
		
	case OID_GENERIC_IO:
	default:
		so->SO1.SType=OT_FILE;
		break;
	}
	
	if(PR_FAIL(error))
		goto ret;
	
	recursed=CALL_SYS_PropertyGetDWord(obj,propid_recursed);
	
	if(!DupEnter( &recursed))
	{
		PR_TRACE(( _this, prtERROR, "AVP1     SYNCRONYZATION ERROR" ));
		PR_TRACE(( _this, prtERROR, "AVP1     SCAN FAILED" ));
		error=errLOCKED;
	}
	else
	{
		hIO SavedIO;
		DupRegisterThread(_this->data->pDupData);
		SavedIO=Get_CurrentIO();
		CALL_SYS_PropertySetDWord(obj,propid_recursed,recursed+1);
//		if(ExclusiveOK)
//			PR_TRACE(( _this, prtNOTIFY, "AVP1     SYNCRONYZATION: SINGLE THREAD PROCESSING" ));

		if(_this->data->SendMtDebug){
			tDWORD MtDebug[4];
			count=16;
			if(DupGetDebugData(MtDebug,&count)){
				//			MtDebug[1]++;
				CALL_SYS_SendMsg(obj,pmc_ENGINE_IO,pm_ENGINE_IO_OUTPUT_MT_DEBUG,_this,&MtDebug,&count);
			}
		}
		
		so->Handle=obj;
		Set_CurrentIO(obj);
		PR_TRACE(( _this, prtNOTIFY, "AVP1     processing begin: %s", so->Name ));

		wa=(CWorkArea*)CALL_SYS_PropertyGetDWord( obj, propid_workarea_need_save);
		if(wa)
			s=SaveWorkArea(wa);

		if( _this->data->SafeScan )
		{
			_TRY_
			{
				BaseAvpProcessObject( _this->data->Base, so );
			}
			_EXCEPT_( _EXCEPTION_FILTER_ )
			{
				PR_TRACE(( _this, prtALWAYS_REPORTED_MSG, "\nAVP1     EXCEPTION 0x%8X in processing %s\n",
					_EXCEPTION_CODE_, so->Name ));

				error=errEXCEPTION;
			}
		}
		else
		{
			BaseAvpProcessObject( _this->data->Base, so );
		}

		PR_TRACE(( _this, prtNOTIFY, "AVP1     processing done : %s", so->Name ));

		if(s)
			RestoreWorkArea(wa,s);

		Set_CurrentIO(SavedIO);
		
		DupExit(&recursed);
		CALL_SYS_PropertySetDWord(obj,propid_recursed,recursed);
	}

	if(_this->data->SendMtDebug){
		tDWORD MtDebug[4];
		count=16;
		if(DupGetDebugData(MtDebug,&count)){
			CALL_SYS_SendMsg(obj,pmc_ENGINE_IO,pm_ENGINE_IO_OUTPUT_MT_DEBUG,_this,&MtDebug,&count);
		}
	}
	
ret:
	if(PR_SUCC(error))	error=CALL_SYS_PropertyGetDWord(obj, propid_processing_error);
	if(PR_SUCC(error) && so->RFlags)
	{
		if (so->RFlags & RF_INTERNAL_ERROR)            error=errUNEXPECTED;
		else if (so->RFlags & RF_KERNEL_FAULT)         error=errUNEXPECTED;
		else if (so->RFlags & RF_PROTECTED)            error=errOBJECT_PASSWORD_PROTECTED;
		else if (so->RFlags & RF_DISK_WRITE_PROTECTED) error=errOBJECT_WRITE;
		else if (so->RFlags & RF_IO_ERROR)             error=errOBJECT_READ;
		else if (so->RFlags & (RF_CURE_FAIL|RF_CURE_DELETE|RF_DELETE))
		{
			tPID pid=OBJ_PID(obj);
			if( PID_TMPFILE==pid || PID_TEMPIO2==pid )
			{
				hOBJECT parent=(hOBJECT)obj;
				while((parent=CALL_SYS_ParentGet(parent,IID_IO))!=0)
				{
					if(DMAP_ID==OBJ_PID(obj))
						continue;
					if(CALL_SYS_PropertyGetBool(parent,propid_has_special_cure))
					{
						MakeNameType(_this, parent, (hOBJECT)obj);
						error=errENGINE_OS_STOPPED;
					}
					break;
				}
			}
			else if (so->RFlags & RF_NOT_OPEN)
				error=errOBJECT_READ_ONLY;
		}
		else if (so->RFlags & (RF_CURE|RF_MODIFIED))   
			error=errENGINE_IO_MODIFIED;
		else if (so->RFlags & RF_NOT_OPEN)
			error=errOBJECT_NOT_CREATED;
	}
	CALL_SYS_PropertySetDWord(obj,propid_scan_object,(tDWORD)0);
	CALL_SYS_ObjHeapFree(obj,so);

	if(IID_POSIO!=OBJ_IID(obj)){
		tPTR pCurrPos;
		if((pCurrPos=CALL_SYS_PropertyGetPtr(obj,propid_io_position_ptr))!=0){
			CALL_SYS_ObjHeapFree(obj,pCurrPos);
			CALL_SYS_PropertyDelete(obj,propid_io_position_ptr);
		}
	}
	
	
	PR_TRACE_A1( _this, "Leave \"Engine::Process\" method, ret tERROR = 0x%08x", error );
	return error;
}
// AVP Prague stamp end


tERROR MakeNameType(hi_Engine _this, hOBJECT obj, hOBJECT obj_src)
{
	tERROR error = errOK;
	tCHAR buf_name[0x200];
	tCHAR* name=buf_name;
	AVPScanObject* so;
	tDWORD disinfectability;

	so=(AVPScanObject*)CALL_SYS_PropertyGetDWord(obj_src,propid_scan_object);
	if(so==0 || ((so->RFlags & (RF_DETECT|RF_WARNING|RF_SUSPIC))==0))
		return errNOT_FOUND;

	*name=0;

	if(so->RFlags & RF_DETECT)
	{
		BaseAvpMakeFullName(_this->data->Base,name,so->VirusName);
		error=CALL_SYS_PropertySetDWord(obj,propid_detect_certanity,ENGINE_DETECT_CERTANITY_SURE);

		if(CALL_SYS_PropertyGetDWord(obj,propid_boot_BytesPerSector))
// if this is a floppy
			disinfectability=ENGINE_DETECT_DISINFECTABILITY_UNKNOWN;
		else if(so->RFlags & RF_CURE_DELETE)
			disinfectability=ENGINE_DETECT_DISINFECTABILITY_YES_BY_DELETE;
		else
			disinfectability=ENGINE_DETECT_DISINFECTABILITY_UNKNOWN;
	}
	else if(so->RFlags & RF_WARNING){
		BaseAvpMakeFullName(_this->data->Base,name,so->WarningName);
		disinfectability=ENGINE_DETECT_DISINFECTABILITY_NO;
		error=CALL_SYS_PropertySetDWord(obj,propid_detect_certanity,ENGINE_DETECT_CERTANITY_PARTIAL);
	}
	else{ // RF_SUSPICION
		BaseAvpMakeFullName(_this->data->Base,name,so->SuspicionName);
		disinfectability=ENGINE_DETECT_DISINFECTABILITY_NO;
		error=CALL_SYS_PropertySetDWord(obj,propid_detect_certanity,ENGINE_DETECT_CERTANITY_HEURISTIC);
	}

	if(CALL_SYS_PropertyGetBool(obj,propid_has_special_cure)) 
			disinfectability=ENGINE_DETECT_DISINFECTABILITY_UNKNOWN;

	error=CALL_SYS_PropertySetDWord(obj,propid_detect_disinfectability,disinfectability);

	if(PR_SUCC(error))
		error=CALL_SYS_PropertySetStr(obj, 0, propid_detect_name, name, strlen(name)+1, cCP_ANSI);
	if(PR_SUCC(error))
	{
		VerdictLeaf* verdict=_this->data->VerdictHead;
		while(verdict)
		{
			if(!_strnicmp(name,verdict->prefix,strlen(verdict->prefix)))
			{
				if(verdict->type==ENGINE_DETECT_TYPE_UNDETECT)
				{
					CALL_SYS_PropertyDelete(obj,propid_detect_type);
					CALL_SYS_PropertyDelete(obj,propid_detect_danger);
					CALL_SYS_PropertyDelete(obj,propid_detect_behaviour);
					CALL_SYS_PropertyDelete(obj,propid_detect_certanity);
					CALL_SYS_PropertyDelete(obj,propid_detect_disinfectability);
					so->RFlags=0;
				}
				else
				{
					CALL_SYS_PropertySetDWord(obj,propid_detect_type,verdict->type);
					CALL_SYS_PropertySetDWord(obj,propid_detect_danger,verdict->danger);
					CALL_SYS_PropertySetDWord(obj,propid_detect_behaviour,verdict->behaviour);
				}
				break;
			}
			verdict=verdict->next;
		}
	}

	return error;
}


tERROR ExportDword(tDWORD dw, tPTR buffer, tDWORD size, tDWORD* out_size )
{
	tERROR error=errOK;
	if(size<sizeof(tDWORD))  error=errBUFFER_TOO_SMALL;
	else if(buffer==0) error=errPARAMETER_INVALID;
	else memcpy(buffer,&dw,sizeof(tDWORD));
	if(out_size)*out_size=sizeof(tDWORD);
	return error;
}


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
	DupRegisterThread(_this->data->pDupData);
	
	PR_TRACE_A0( _this, "Enter \"Engine::GetAssociatedInfo\" method" );

	switch(info_id)
	{
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
		{
			hi_Engine mgr=(hi_Engine)CALL_SYS_ParentGet(_this,IID_ENGINE);
			if(mgr)
				error=CALL_Engine_GetAssociatedInfo( mgr, obj, info_id, param, buffer, size, out_size );
		}
		break;
	}
	
	PR_TRACE_A1( _this, "Leave \"Engine::GetAssociatedInfo\" method, ret tERROR = 0x%08x", error );
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
	tERROR error = errNOT_FOUND;
	PR_TRACE_A0( _this, "Enter \"Engine::GetExtendedInfo\" method" );
	DupRegisterThread(_this->data->pDupData);
	
	if(info_id==ENGINE_EXT_DETECT_TYPE_BIT_NAME_ANSI && out_size)
	{
		tDWORD len=0;
		hSTRING str;
		error=CALL_SYS_ObjectCreateQuick(_this, &str,IID_STRING,PID_ANY,SUBTYPE_ANY );
		if(PR_SUCC(error) && out_size)
		{
			switch(*out_size)	
			{
			case ENGINE_DETECT_TYPE_VIRWARE:
				error=CALL_String_ImportFromBuff( str, &len, "Virus", 0,cCP_ANSI,cSTRING_Z);
				break;
			case ENGINE_DETECT_TYPE_TROJWARE:
				error=CALL_String_ImportFromBuff( str, &len, "Trojan", 0,cCP_ANSI,cSTRING_Z);
				break;
			case ENGINE_DETECT_TYPE_ADWARE:
				error=CALL_String_ImportFromBuff( str, &len, "Adware", 0,cCP_ANSI,cSTRING_Z);
				break;
			case ENGINE_DETECT_TYPE_PORNWARE:
				error=CALL_String_ImportFromBuff( str, &len, "Pornware", 0,cCP_ANSI,cSTRING_Z);
				break;
			case ENGINE_DETECT_TYPE_RISKWARE:
				error=CALL_String_ImportFromBuff( str, &len, "Riskware", 0,cCP_ANSI,cSTRING_Z);
				break;
			case ENGINE_DETECT_TYPE_MALWARE:
			default:
				error=CALL_String_ImportFromBuff( str, &len, "Malware", 0,cCP_ANSI,cSTRING_Z);
				break;
			}
			if(PR_SUCC(error) && len)
			{
				if(len>size)
				{
					*out_size=len;
					error=errBUFFER_TOO_SMALL;
				}
				else if(buffer==0)error=errPARAMETER_INVALID;
				else error=CALL_String_ExportToBuff( str, out_size, cSTRING_WHOLE, buffer, size, param, cSTRING_Z );
			}
		}
	}

	PR_TRACE_A1( _this, "Leave \"Engine::GetExtendedInfo\" method, ret tERROR = 0x%08x", error );
	return error;
}
// AVP Prague stamp end


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
	tDWORD l;
	char name[0x200];
	PR_TRACE_A0( _this, "Enter Engine::LoadConfiguration method" );
	DupRegisterThread(_this->data->pDupData);
	
	type=tid_DWORD;
	CALL_Registry_GetValue(reg,0,key,"MFlags",&type,&(_this->data->MFlags),sizeof(tDWORD));

	type=tid_STRING;
	if(PR_SUCC(CALL_Registry_GetValue(reg,0,key,"SetPath",&type,name,0x200))){
		CALL_String_ImportFromBuff(_this->data->DB_Name,0,name,strlen(name)+1,cCP_ANSI,cSTRING_Z);
	}

	if(PR_FAIL(CALL_String_Length(_this->data->DB_Name, &l)) || l==0)
		error=errENGINE_BAD_AVP_SET;

	if(PR_SUCC(error))
	{
		char buf[MAX_PATH]="";
		char* name_ptr; tSIZE_T name_sz;
		CALL_String_ExportToBuff(_this->data->DB_Name,0,cSTRING_WHOLE,buf,MAX_PATH,cCP_ANSI,cSTRING_Z);
		name_ptr=strrchr(buf,'\\');
		if(!name_ptr)
			name_ptr=strrchr(buf,'/');
		if(!name_ptr)
			name_ptr=buf;
		else
			name_ptr++;

		name_sz = buf+countof(buf)-name_ptr;
		strcpy_s(name_ptr,name_sz,"AVP.VND");
		if(!VndInit(_this,buf))
			PR_TRACE(( _this, prtERROR, "AVP1 Failed to init VND: %s", buf ));

		strcpy_s(name_ptr,name_sz,"verdicts.ini");
		if(PR_FAIL(LoadVerdicts(_this,buf)))
			PR_TRACE(( _this, prtERROR, "AVP1 Failed to init Verdicts: %s", buf ));
	}
	if(PR_SUCC(error)) error=SyncLoadBase(_this, 1);

	if(PR_SUCC(error))
	{
		if(0==_this->data->DB_Count)	error=errENGINE_BAD_CONFIGURATION;
		else	_this->data->DB_State=cENGINE_DB_STATE_FUNCTIONAL;
	}

	
	PR_TRACE_A1( _this, "Leave Engine::LoadConfiguration method, ret tERROR = 0x%08x", error );
	return error;
}
// AVP Prague stamp end

void TraceZEROS()
{
	PR_TRACE(( Get_AVP1(), prtDANGER, "AVP1\t ZEROS in Header.!!!"));
}


HANDLE Cache_GetRealHandle(HANDLE  hFile);

#define HREAD_SIZE 0x40

LRESULT WINAPI _AvpCallback(WPARAM wParam,LPARAM lParam)
{
	tERROR error=errOK;
	AVPScanObject* so=(AVPScanObject*)lParam;
	CWorkArea* wa=(CWorkArea*)lParam;
	tDWORD wa_prev;

	//tUINT size=sizeof(AVPScanObject);
	hOBJECT l_obj;
	hOBJECT l_packed;
	hi_Engine _this;
	hSTRING str;
	char* name;
	
#if !defined (__unix__)
	BOOL stack_align=0;
	_asm{
		test esp,3;
		jz no_align;
		push bx;
	}
	stack_align=1;
no_align:
#endif

	if(wParam == AVP_CALLBACK_ROTATE) //more probable param
	{
		tBOOL b_yield=cTRUE;
rotate:
#ifdef _WIN32
		{
			
			tDWORD t_count=(tDWORD)TlsGetValue(TlsTickCountIndex);
			tDWORD c_count=GetTickCount();
			if((c_count-t_count)<100) b_yield=cFALSE;
			else TlsSetValue(TlsTickCountIndex,(LPVOID)c_count);
		}
#endif
		if(b_yield){
			l_obj=(hOBJECT)Get_CurrentIO();
			if(l_obj)
			{
				_this=(hi_Engine)Get_AVP1();
				error=CALL_SYS_SendMsg_ObjectError(_this, l_obj,pmc_PROCESSING,pm_PROCESSING_YIELD,_this,0,0);
				if(error==errOPERATION_CANCELED)
					error=1;
			} 
		}
	}
	else
	{
		l_obj=(hOBJECT)Get_CurrentIO();
		_this=(hi_Engine)Get_AVP1();

		switch(wParam)
		{
		case AVP_CALLBACK_REGISTER_PASSWORD:
			if(l_obj && lParam){
				tDWORD len=strlen((tCHAR*)lParam);
				if(len)
					CALL_SYS_SendMsg(l_obj,pmc_AVPMGR_INTERNAL,pm_AVPMGR_INTERNAL_IO_REGISTER_PASSWORD,0,(tPTR)lParam,&len);
			}
			break;
			
		case AVP_CALLBACK_OBJECT_SKIP_REQEST:
			l_obj=((hOBJECT)Cache_GetRealHandle(so->Handle));
			{
				tDWORD cls=ENGINE_ACTION_CLASS_VIRUS_SCAN;
				tDWORD len=sizeof(cls);
				error=CALL_SYS_PropertyGetDWord(l_obj, propid_processing_error);
				if(PR_SUCC(error))
				{
					if(CALL_SYS_PropertyGetBool(l_obj,propid_object_skip))
						error=errENGINE_ACTION_SKIP;
					else
						error=CALL_SYS_SendMsg(l_obj,pmc_ENGINE_ACTION,ENGINE_ACTION_TYPE_SUBENGINE_PROCESS,_this,&cls,&len);
				}
				if(error==errENGINE_ACTION_SKIP) error=1;
				else if(PR_FAIL(error)){
					if(error==errOPERATION_CANCELED) 	
						BaseAvpCancelProcessObject( _this->data->Base,error);
					error=2;
				}
				else error=errOK;
			}
			break;
		case AVP_CALLBACK_WA_RECORD:
			{
				tDWORD size=sizeof(CWorkArea);
				l_obj = (hOBJECT)Cache_GetRealHandle(wa->Handle);
				CALL_SYS_SendMsg(l_obj, pmc_ENGINE_DETECT, pm_ENGINE_DETECT_WA, _this, wa, &size);
			}
			break;


		case AVP_CALLBACK_WA_PROCESS_OBJECT:
			{
				l_packed = (hOBJECT)Cache_GetRealHandle(wa->P_Handle);
				l_obj = (hOBJECT)Cache_GetRealHandle(wa->Handle);
				
				if(CALL_SYS_PropertyGetBool(l_packed,propid_object_skip)
					//			|| CALL_SYS_PropertyGetBool(l_obj,propid_object_skip)
					){
					error=1; //skip
					break;
				}
				else
					CALL_SYS_PropertyDelete(l_obj, propid_object_skip );
				
				
				if(CALL_SYS_PropertySize(l_packed, propid_npENGINE_OBJECT_OBTAINED_TRANSFORMER_NAME)){
					if(PR_SUCC(CALL_SYS_ObjectCreateQuick(_this, &str, IID_STRING,PID_ANY,SUBTYPE_ANY ))){
						if(PR_SUCC(CALL_String_ImportFromProp(str,0,l_packed,propid_npENGINE_OBJECT_OBTAINED_TRANSFORMER_NAME)))
							CALL_String_ExportToProp(str,0,cSTRING_WHOLE,l_packed,propid_npENGINE_OBJECT_TRANSFORMER_NAME);
						CALL_SYS_ObjectClose(str);
					}
				}
				
				if( wa->ScanObject->InArc && !(wa->ScanObject->RFlags & RF_IN_CURABLE_EMBEDDED) )
				{
					if(PR_SUCC(CALL_SYS_PropertyGetDWord(l_obj,   propid_object_readonly_error)))
					{
						CALL_SYS_PropertySetDWord(l_obj,propid_object_readonly_error,errOBJECT_WRITE_NOT_SUPPORTED);
						CALL_SYS_PropertySetObj(l_obj,propid_object_readonly_object,l_packed);
					}
				}
				
				if(wa->ScanObject->InPack)
				{
					tERROR err=CALL_SYS_PropertyGetDWord(l_packed,propid_object_readonly_error);
					if( PR_FAIL(err) )
					{
						if(PR_SUCC(CALL_SYS_PropertyGetDWord(l_obj,   propid_object_readonly_error)))
						{
							INT3; //should not happened
							CALL_SYS_PropertySetDWord(l_obj,propid_object_readonly_error,err);
							CALL_SYS_PropertySetObj(l_obj,propid_object_readonly_object,l_packed);
						}
					}
					
					//propid_integral_parent_io will be overwritten in case of disinfection.
					CALL_SYS_PropertySetObj(l_obj,propid_integral_parent_io,l_packed);
				}
				
				wa_prev=CALL_SYS_PropertyGetDWord(l_obj, propid_workarea_need_save);
				if(wa_prev != (tDWORD)lParam)
					CALL_SYS_PropertySetDWord(l_obj, propid_workarea_need_save,	lParam);
				
				CALL_SYS_SendMsg_Callback(l_obj,pmc_AVPMGR_INTERNAL,pm_AVPMGR_INTERNAL_IO_PROCESS,_this,0,0);
				
				if(wa_prev != (tDWORD)lParam)
					CALL_SYS_PropertySetDWord(l_obj, propid_workarea_need_save,	wa_prev);
				
				if(PR_SUCC(error)){
					error=1; //skip processing - this object already processed
				}
				else{
					hOBJECT integral_parent_io=CALL_SYS_PropertyGetObj(l_obj,propid_integral_parent_io);
					if(integral_parent_io){
						CALL_SYS_PropertySetDWord(integral_parent_io, propid_processing_error,	error);
					}
					CALL_SYS_PropertySetDWord(l_obj, propid_processing_error,	error);
				}
				
				if(CALL_SYS_PropertyGetDWord(l_obj,propid_detect_counter))
				{
					wa->ScanObject->RFlags &= ~(RF_WARNING|RF_SUSPIC);
					wa->RFlags &= ~(RF_WARNING|RF_SUSPIC);
					wa->RFlags |= RF_DETECT;
					wa->ScanObject->RFlags |= RF_DETECT;
				}
				
			}
			break;
			
		case AVP_CALLBACK_OBJECT_BEGIN:
			l_obj=((hOBJECT)Cache_GetRealHandle(so->Handle));
			CALL_SYS_PropertyDelete(l_obj, propid_object_skip );
			if(so->RFlags & RF_IN_CURABLE_EMBEDDED)
			{
				CALL_SYS_PropertySetBool(l_obj,propid_has_special_cure,1);
				Set_CurrentIO(l_obj);
			}
			CALL_SYS_PropertySetDWord(l_obj,propid_scan_object,(tDWORD)so);
			CALL_SYS_SendMsg_ObjectError(_this, l_obj, pmc_ENGINE_IO, pm_ENGINE_IO_BEGIN, _this,0,0);
			break;
			
		case AVP_CALLBACK_OBJECT_DONE:
			l_obj=((hOBJECT)Cache_GetRealHandle(so->Handle));
			if(so->RFlags & RF_CORRUPT){
				PR_TRACE(( _this, prtIMPORTANT, "AVP1     RF_IO_CORRUPT" ));
				CALL_SYS_SendMsg_ObjectError(_this, l_obj,pmc_ENGINE_IO,pm_ENGINE_IO_CORRUPTED,_this,0,0);
				so->RFlags &=~RF_CORRUPT;
			}
			if (so->RFlags & RF_ENCRYPTED){
				PR_TRACE(( _this, prtIMPORTANT, "AVP1     RF_IO_ENCRYPTED" ));
				CALL_SYS_SendMsg_ObjectError(_this, l_obj,pmc_ENGINE_IO,pm_ENGINE_IO_ENCRYPTED,_this,0,0);
				so->RFlags &=~RF_ENCRYPTED;
			}
			if (so->RFlags & RF_PROTECTED){
				PR_TRACE(( _this, prtIMPORTANT, "AVP1     RF_IO_PROTECTED" ));
				CALL_SYS_SendMsg_ObjectError(_this, l_obj,pmc_ENGINE_IO,pm_ENGINE_IO_PASSWORD_PROTECTED,_this,0,0);
				so->RFlags &=~RF_PROTECTED;
			}
			if (so->RFlags & RF_CURE_FAIL)
			{
				tERROR err=CALL_SYS_PropertyGetDWord(l_obj, propid_processing_error);
				tDWORD len=sizeof(err);
				if(CALL_SYS_PropertyGetBool(l_obj,propid_has_special_cure))
				{
					PR_TRACE(( _this, prtIMPORTANT, "AVP1     Disinfection transferred to parent." ));
					//				CALL_SYS_SendMsg_ObjectError(_this, l_obj,pmc_ENGINE_IO,pm_ENGINE_IO_DISINFECTION_TRANSFERRED,_this,&err,&len);
				}
				else
				{
					if(so->RFlags & RF_CURE_DELETE){
						PR_TRACE(( _this, prtIMPORTANT, "AVP1     Disinfection impossible!." ));
						CALL_SYS_SendMsg_ObjectError(_this, l_obj,pmc_ENGINE_IO,pm_ENGINE_IO_DISINFECTION_IMPOSSIBLE,_this,&err,&len);
					}
					else
					{
						PR_TRACE(( _this, prtIMPORTANT, "AVP1     Disinfection FAILED!." ));
						CALL_SYS_SendMsg_ObjectError(_this, l_obj,pmc_ENGINE_IO,pm_ENGINE_IO_DISINFECTION_FAILED,_this,&err,&len);
					}
				}
			}
			
			PR_TRACE(( _this, prtNOTIFY, "AVP1     OBJECT_DONE: %08X %s", so->RFlags, so->Name ));
			CALL_SYS_SendMsg_ObjectError(_this, l_obj, pmc_ENGINE_IO, pm_ENGINE_IO_DONE, _this,0,0);

			if( CALL_SYS_PropertyGetBool(l_obj,propid_has_special_cure) )
			{
				hOBJECT parent = CALL_SYS_ParentGet(l_obj,IID_IO);
				if( parent )
					Set_CurrentIO(parent);
			}
			break;
			
		case AVP_CALLBACK_ROTATE_OFF:
			goto rotate;
			break;
			
		case AVP_CALLBACK_WA_PROCESS_ARCHIVE:
			{
				tDWORD offset=wa->P_N;
				tDWORD l=sizeof(offset);
				l_obj=((hOBJECT)Cache_GetRealHandle(wa->Handle));

				if(CALL_SYS_PropertyGetBool(l_obj,propid_object_skip))
					error=1;
				else
				{
					wa_prev=CALL_SYS_PropertyGetDWord(l_obj, propid_workarea_need_save);
					if(wa_prev != (tDWORD)lParam)
						CALL_SYS_PropertySetDWord(l_obj, propid_workarea_need_save,	lParam);
					
					CALL_SYS_SendMsg_Callback(l_obj,pmc_AVPMGR_INTERNAL,pm_AVPMGR_INTERNAL_IO_PROCESS_ARCHIVE,_this,&offset,&l);
					
					if(wa_prev != (tDWORD)lParam)
						CALL_SYS_PropertySetDWord(l_obj, propid_workarea_need_save,	wa_prev);

					if(error==errOPERATION_CANCELED) 	
						BaseAvpCancelProcessObject( _this->data->Base,error);
					else if(l==(tDWORD)-1) 
						error=errEOF; //stop zip processing - this offset already processed
					else 
						error=errOK;
					
					if(CALL_SYS_PropertyGetDWord(l_obj,propid_detect_counter))
					{
						wa->ScanObject->RFlags &= ~(RF_WARNING|RF_SUSPIC);
						wa->RFlags &= ~(RF_WARNING|RF_SUSPIC);
					}
				}
			}
			break;
			
		case AVP_CALLBACK_GET_SECTOR_IMAGE:
			{
				tPTR  ptr=0;
				tDWORD size=sizeof(ptr);
				error=CALL_SYS_SendMsg(Get_CurrentIO(),pmc_ENGINE_IO,
					(lParam==ST_MBR)?pm_ENGINE_IO_GET_ORIGINAL_MBR:pm_ENGINE_IO_GET_ORIGINAL_BOOT,
					_this,&ptr,&size);
				if(PR_FAIL(error)){
					ptr=0;
				}
				error= (LRESULT)ptr;
				break;
			}
			
		case AVP_CALLBACK_CHECK_FILE_INTEGRITY:
			if(SignCheck)
				SignCheck( (hOBJECT)lParam, 1, _this->data->vndArray, _this->data->vndArrayCount, 0, (tDWORD*)&error);
			break;
			
		case AVP_CALLBACK_OBJECT_DETECT:
			l_obj=((hOBJECT)Cache_GetRealHandle(so->Handle));
			MakeNameType(_this, l_obj, l_obj);
			CALL_SYS_SendMsg_ObjectError(_this, l_obj,pmc_ENGINE_IO,pm_ENGINE_IO_DETECT,_this,0,0);
			PR_TRACE(( _this, prtIMPORTANT, "AVP1     VIRUS DETECTED! %s\n", so->Name ));
			break;
			
		case AVP_CALLBACK_OBJECT_WARNING:
		case AVP_CALLBACK_OBJECT_SUSP:
			l_obj=((hOBJECT)Cache_GetRealHandle(so->Handle));
			MakeNameType(_this, l_obj, l_obj);
			CALL_SYS_SendMsg_ObjectError(_this, l_obj,pmc_ENGINE_IO,pm_ENGINE_IO_DETECT,_this,0,0);
			PR_TRACE(( _this, prtIMPORTANT, "AVP1     SUSPECT DETECTED! %s\n", so->Name ));
			
			CALL_SYS_SendMsg_Callback(l_obj,pmc_AVPMGR_INTERNAL,pm_AVPMGR_INTERNAL_IO_DISINFECT_REQUEST,_this,0,0);
			if(PR_FAIL(error)){
				if(error==errOPERATION_CANCELED) 	
					BaseAvpCancelProcessObject( _this->data->Base,error);
				else if(error==errENGINE_IO_MODIFIED)
					error=7;
				else
					PR_TRACE(( _this, prtIMPORTANT, "AVP1     Disinfection impossible!." ));
				error=0;
			}
			else 
				error=1;//disinfect command
			break;
			
		case AVP_CALLBACK_OBJECT_CURE:
			l_obj=((hOBJECT)Cache_GetRealHandle(so->Handle));
			CALL_SYS_SendMsg_ObjectError(_this, l_obj,pmc_ENGINE_IO,pm_ENGINE_IO_DISINFECTED,_this,0,0);
			CALL_SYS_PropertySetDWord(l_obj, propid_processing_error,	errENGINE_IO_MODIFIED);
			PR_TRACE(( _this, prtIMPORTANT, "AVP1     Virus body removed."));
			break;
			
		case AVP_CALLBACK_OBJECT_DELETE:
			PR_TRACE(( _this, prtIMPORTANT, "AVP1     command to delete Infected object." ));
			break;
			
		case AVP_CALLBACK_OBJECT_CURE_FAIL:
			// handle could be of father, vir names property not present...
			// use OBJECT_DONE
			break;

		case AVP_CALLBACK_OBJECT_ZERO_HEADER:
			TraceZEROS();
			break;

		case AVP_CALLBACK_OBJECT_GET_EMUL_FLAGS:
			l_obj=((hOBJECT)Cache_GetRealHandle(so->Handle));
			{
				tDWORD l=sizeof(error);
				CALL_SYS_SendMsg(l_obj, pmc_ENGINE_IO, pm_ENGINE_IO_GET_EMUL_FLAGS ,_this, &error, &l);
			}
			break;

		case AVP_CALLBACK_OBJECT_FALSE_ALARM:
			l_obj=((hOBJECT)Cache_GetRealHandle(so->Handle));
			{
				tDWORD l=sizeof(error);
				CALL_SYS_SendMsg(l_obj, pmc_ENGINE_IO, pm_ENGINE_IO_FALSE_ALARM ,_this, &error, &l);
			}
			break;
			
		case AVP_CALLBACK_WA_PACKED_NAME:
			l_obj=((hOBJECT)Cache_GetRealHandle(wa->Handle));
			{
				tDWORD l=1;
				CALL_SYS_ParentSet(l_obj,0,(hOBJECT)(Cache_GetRealHandle(wa->P_Handle)));
				if(PR_SUCC(CALL_SYS_PropertyGetStr((hOBJECT)(Cache_GetRealHandle(wa->P_Handle)),0,propid_npENGINE_OBJECT_OBTAINED_TRANSFORMER_NAME,wa->ZipName,MAX_PATH,cCP_ANSI)))
				{
					l=strlen(wa->ZipName)+1;
					CALL_SYS_SendMsg(l_obj, pmc_ENGINE_IO, pm_ENGINE_IO_MAKE_VIRTUAL_NAME,_this,wa->ZipName,&l);
				}
				else
					CALL_SYS_SendMsg(l_obj, pmc_ENGINE_IO, pm_ENGINE_IO_MAKE_VIRTUAL_NAME,_this,"",&l);
			}
			break;
			
		case AVP_CALLBACK_WA_ARCHIVE_NAME:
			l_obj=((hOBJECT)Cache_GetRealHandle(wa->Handle));
			{
				tDWORD l=strlen(wa->ZipName)+1;
				CALL_SYS_ParentSet(l_obj,0,(hOBJECT)(Cache_GetRealHandle(wa->P_Handle)));
			
				CALL_SYS_PropertyDelete(l_obj, propid_virtual_name);
				CALL_SYS_PropertyDelete(l_obj, propid_object_skip );
				CALL_SYS_PropertyDelete(l_obj, propid_processing_error );

				CALL_SYS_SendMsg(l_obj, pmc_ENGINE_IO, pm_ENGINE_IO_MAKE_VIRTUAL_NAME,_this,wa->ZipName,&l);
				CALL_SYS_SendMsg_Callback(l_obj, pmc_ENGINE_IO, pm_ENGINE_IO_NAME_CHECK,0,0,0);
			}
			if(PR_SUCC(error))error=errOK;
			break;
			
		case AVP_CALLBACK_WA_PACKED:
			l_obj=((hOBJECT)Cache_GetRealHandle(wa->Handle));
			l_packed = (hOBJECT)Cache_GetRealHandle(wa->P_Handle);
			CALL_SYS_PropertySetObj(l_obj,propid_integral_parent_io,l_packed);
			break;
			
		case AVP_CALLBACK_OBJECT_PACKED:
			l_obj=((hOBJECT)Cache_GetRealHandle(so->Handle));
			PR_TRACE(( _this, prtIMPORTANT, "AVP1     IO created %s on object %08X (Packed)", so->VirusName, l_obj ));
			if(PR_SUCC(CALL_SYS_ObjHeapAlloc(l_obj,(tPTR*)&name,0x100))){
				BaseAvpMakeFullName(_this->data->Base,name,so->VirusName);
				CALL_SYS_PropertySetStr(l_obj,0,propid_npENGINE_OBJECT_OBTAINED_TRANSFORMER_NAME,name,strlen(name)+1,cCP_ANSI);
				CALL_SYS_ObjHeapFree(l_obj,name);
			}
			if( NULL==CALL_SYS_PropertyGetObj( l_obj,propid_object_executable_parent)) 
				CALL_SYS_PropertySetObj( l_obj,propid_object_executable_parent,l_obj);
			CALL_SYS_SendMsg_ObjectError(_this, l_obj,pmc_ENGINE_IO,pm_ENGINE_IO_SUB_IO_ENTER,_this,0,0);
			break;
			
		case AVP_CALLBACK_OBJECT_PACKED_DONE:
			l_obj=((hOBJECT)Cache_GetRealHandle(so->Handle));
			PR_TRACE(( _this, prtIMPORTANT, "AVP1     IO closed %s on object %08X (Packed)", so->VirusName, l_obj ));
			CALL_SYS_SendMsg_ObjectError(_this, l_obj,pmc_ENGINE_IO,pm_ENGINE_IO_SUB_IO_LEAVE,_this,0,0);
			break;
			
		case AVP_CALLBACK_WA_ARCHIVE:
			break;
			
		case AVP_CALLBACK_OBJECT_ARCHIVE:
			l_obj=((hOBJECT)Cache_GetRealHandle(so->Handle));
			if(PR_SUCC(CALL_SYS_ObjHeapAlloc(l_obj,(tPTR*)&name,0x100)))
			{
				tDWORD  l_action_class=-1;
				tDWORD  size=sizeof(l_action_class);
				
				BaseAvpMakeFullName(_this->data->Base,name,so->VirusName);
				CALL_SYS_PropertySetStr(l_obj,0,propid_npENGINE_OBJECT_OBTAINED_TRANSFORMER_NAME,name,strlen(name)+1,cCP_ANSI);
				
				if(*(DWORD*)name == *(DWORD*)"Embe")
				{
					l_action_class= ENGINE_ACTION_CLASS_OLE_EMBEDDED;
					if(NULL==CALL_SYS_PropertyGetObj( l_obj,propid_object_executable_parent)) 
						CALL_SYS_PropertySetObj( l_obj,propid_object_executable_parent,l_obj);
				}
				else
				{
					if(*(DWORD*)name == *(DWORD*)"Mail")
					{
						if(name[4]){ // mail format
							l_action_class= ENGINE_ACTION_CLASS_MAIL_DATABASE;
						}
						else{ //plain mail
							l_action_class= ENGINE_ACTION_CLASS_MAIL_PLAIN;
						}
						CALL_SYS_PropertySetBool(l_obj,propid_auto_password_bool,cTRUE);
					}
					else 
					{
						tDWORD dwSize;
						tERROR err;
						tBYTE *buf;
						l_action_class= ENGINE_ACTION_CLASS_ARCHIVE | ENGINE_ACTION_CLASS_ARCHIVE_SFX;
/*	Full-size hash
						{
							tQWORD qwSize;
							CALL_IO_GetSize((hIO)l_obj,&qwSize,IO_SIZE_TYPE_EXPLICIT);
							if(PR_SUCC(CALL_SYS_ObjHeapAlloc(l_obj,(tPTR*)&buf,qwSize))){
								if(PR_SUCC(CALL_IO_SeekRead((hIO)l_obj,&dwSize,so->Connection,buf,HREAD_SIZE))){
									tQWORD qwHash=CalcSum(buf,dwSize);

									dwSize=sizeof(qwHash);
									if(errENGINE_ACTION_SKIP == CALL_SYS_SendMsg(l_obj, pmc_AVPMGR_INTERNAL, pm_AVPMGR_INTERNAL_IO_HASH_FRAME_CHECK, _this, &qwHash, &dwSize))
										error=1;
								}
								CALL_SYS_ObjHeapFree(l_obj,buf);
							}				
						}
*/
						
						if(PR_SUCC(CALL_SYS_ObjHeapAlloc(l_obj,(tPTR*)&buf,HREAD_SIZE+HREAD_SIZE+sizeof(tQWORD)))){
							if(PR_SUCC(CALL_IO_SeekRead((hIO)l_obj,&dwSize,so->Connection,buf,HREAD_SIZE))){
								tQWORD qwHash;
								tQWORD* qwSize=(tQWORD*)(buf+dwSize);
								CALL_IO_GetSize((hIO)l_obj,qwSize,IO_SIZE_TYPE_EXPLICIT);
								if(*qwSize >= HREAD_SIZE){
									tDWORD dwLSize=(tDWORD)min(*qwSize-HREAD_SIZE, HREAD_SIZE);
									if(PR_SUCC(CALL_IO_SeekRead((hIO)l_obj,&dwLSize,*qwSize-dwLSize,(buf+dwSize+sizeof(tQWORD)),dwLSize))){
										dwSize+=dwLSize;
									}
								}
								*qwSize-=so->Connection;
								qwHash=CalcSum(buf,dwSize+sizeof(tQWORD));
								
								dwSize=sizeof(qwHash);
								DupCallbackCall(err=CALL_SYS_SendMsg(l_obj, pmc_AVPMGR_INTERNAL, pm_AVPMGR_INTERNAL_IO_HASH_FRAME_CHECK, _this, &qwHash, &dwSize));
								if(errENGINE_ACTION_SKIP == err)
									error=1;
							}
							CALL_SYS_ObjHeapFree(l_obj,buf);
						}				

					}
				}
				CALL_SYS_ObjHeapFree(l_obj,name);
				
				if(error!=1)
				{
					CALL_SYS_PropertySetDWord(l_obj,propid_action_class_mask_current,l_action_class);
					
					if(CALL_SYS_PropertyGetBool(l_obj,propid_object_skip))
						error=errENGINE_ACTION_SKIP;
					else
						CALL_SYS_SendMsg_Callback(l_obj,pmc_ENGINE_ACTION,ENGINE_ACTION_TYPE_PROCESS_OS_ON_IO,_this,&l_action_class,&size);
					
					if(PR_SUCC(error))
						error=CALL_SYS_SendMsg_ObjectError(_this, l_obj,pmc_ENGINE_IO,pm_ENGINE_IO_OS_RECOGNIZE,_this,0,0);
					if(PR_SUCC(error))
						error=CALL_SYS_SendMsg_ObjectError(_this, l_obj,pmc_ENGINE_IO,pm_ENGINE_IO_OS_ENTER,_this,&l_action_class,&size);
					if(PR_SUCC(error))
					{
						ActionClassToMFlags( _this,l_obj,&so->MFlags);
						PR_TRACE(( _this, prtIMPORTANT, "AVP1     OS created %s on object %08X (ARCHIVE)", so->VirusName, l_obj ));
					}
				}
			}
			break;
			
		case AVP_CALLBACK_OBJECT_ARCHIVE_DONE:
			l_obj=((hOBJECT)Cache_GetRealHandle(so->Handle));
			if(!( so->RFlags & RF_ERROR))
			{
				tQWORD par_off=so->Entry;
				tDWORD l=sizeof(par_off);
				CALL_SYS_SendMsg(l_obj,pmc_AVPMGR_INTERNAL,pm_AVPMGR_INTERNAL_IO_OS_OFFSET_REGISTER,_this,&par_off,&l);
				CALL_SYS_SendMsg(l_obj, pmc_AVPMGR_INTERNAL, pm_AVPMGR_INTERNAL_IO_HASH_FRAME_LEAVE, _this, 0, 0);
			}
			PR_TRACE(( _this, prtIMPORTANT, "AVP1     OS closed %s on object %08X (ARCHIVE)", so->VirusName, l_obj ));
			CALL_SYS_SendMsg_ObjectError(_this, l_obj,pmc_ENGINE_IO,pm_ENGINE_IO_OS_LEAVE,_this,0,0);
			
			break;
			
		case AVP_CALLBACK_ASK_CURE_MEMORY:
			l_obj=((hOBJECT)Cache_GetRealHandle(so->Handle));
			CALL_SYS_SendMsg_Callback(l_obj,pmc_AVPMGR_INTERNAL,pm_AVPMGR_INTERNAL_IO_DISINFECT_REQUEST,_this,0,0);
			if(PR_FAIL(error)){
				if(error==errOPERATION_CANCELED) 	
					BaseAvpCancelProcessObject( _this->data->Base,error);
				else if(error==errENGINE_ACTION_SKIP)
					CALL_SYS_PropertySetDWord(l_obj, propid_processing_error, error);
				error=2; //skip
			}
			else 
				error=0;//disinfect command
			break;
			
		case AVP_CALLBACK_ASK_CURE:
			l_obj=((hOBJECT)Cache_GetRealHandle(so->Handle));
			CALL_SYS_SendMsg_Callback(l_obj,pmc_AVPMGR_INTERNAL,pm_AVPMGR_INTERNAL_IO_DISINFECT_REQUEST,_this,0,0);
			if(PR_FAIL(error)){
				if(error==errOPERATION_CANCELED) 	
					BaseAvpCancelProcessObject( _this->data->Base,error);
				else if(error==errENGINE_IO_MODIFIED)
				{
					error=7;
					break;
				}
				else if(error==errENGINE_ACTION_SKIP)
					CALL_SYS_PropertySetDWord(l_obj, propid_processing_error, error);
				error=0;
			}
			else 
				error=1;//disinfect command
			break;
			
		case AVP_CALLBACK_ASK_DELETE:
			l_obj=((hOBJECT)Cache_GetRealHandle(so->Handle));
			if(so->SO1.SType & OT_SECTOR){
				CALL_SYS_SendMsg_Callback(l_obj,pmc_AVPMGR_INTERNAL,pm_AVPMGR_INTERNAL_IO_SECTOR_OVERWRITE_REQUEST,_this,0,0);
				if (error==errENGINE_ACTION_SKIP)
				{
					error = errOPERATION_CANCELED;
				}
				if(PR_FAIL(error)){
					if(error==errOPERATION_CANCELED)
						BaseAvpCancelProcessObject( _this->data->Base,error);
					error=0;
				}
				else 
					error=2; //overwrite command
			}
			break;
			
		case AVP_CALLBACK_OBJECT_MESS:
			break;
			
		case AVP_CALLBACK_OBJECT_UNKNOWN:
			l_obj=((hOBJECT)Cache_GetRealHandle(so->Handle));
			error=CALL_SYS_SendMsg_ObjectError(_this, l_obj,pmc_ENGINE_IO,pm_ENGINE_IO_UNSUPPORTED_COMPRESSION_METHOD,_this,0,0);
			break;
			
		case AVP_CALLBACK_PUT_STRING:
			PR_TRACE(( _this, prtIMPORTANT, (char*)so ));
			{
				tDWORD l=strlen((char*)so)+1;
				CALL_SYS_SendMsg(l_obj?l_obj:((hOBJECT)_this),pmc_ENGINE_IO, pm_ENGINE_IO_OUTPUT_STRING,_this,so,&l);
			}
			break;
		case AVP_CALLBACK_FILE_INTEGRITY_FAILED:
		case AVP_CALLBACK_SETFILE_INTEGRITY_FAILED:
			PR_TRACE(( _this, prtIMPORTANT, "SET file not signed: %s",(char*)so ));
			if(lParam && PR_SUCC(CALL_SYS_ObjectCreateQuick(_this, &str, IID_STRING,PID_ANY,SUBTYPE_ANY ))){
				if(PR_SUCC(CALL_String_ImportFromBuff(str,0,(tSTRING)lParam,strlen((tSTRING)lParam)+1,cCP_ANSI,cSTRING_Z)))
					error=CALL_SYS_SendMsg(_this,pmc_ENGINE_LOAD, pm_ENGINE_LOAD_BASE_INTEGRITY_FAILED,str,0,0);
				CALL_SYS_ObjectClose(str);
				if(PR_SUCC(error))error=errOK;
			}
			break;
			
		case AVP_CALLBACK_ERROR_FILE_OPEN:
		case AVP_CALLBACK_ERROR_SET:
		case AVP_CALLBACK_ERROR_BASE:
			if(lParam && PR_SUCC(CALL_SYS_ObjectCreateQuick(_this, &str, IID_STRING,PID_ANY,SUBTYPE_ANY ))){
				if(PR_SUCC(CALL_String_ImportFromBuff(str,0,(tSTRING)lParam,strlen((tSTRING)lParam)+1,cCP_ANSI,cSTRING_Z)))
					CALL_SYS_SendMsg(_this,pmc_ENGINE_LOAD, pm_ENGINE_LOAD_BASE_ERROR_OPEN,str,0,0);
				CALL_SYS_ObjectClose(str);
			}

		case AVP_CALLBACK_ERROR_SYMBOL:
		case AVP_CALLBACK_ERROR_KERNEL_PROC:
			{
				tDWORD len=lParam ? strlen((tSTRING)lParam)+1 : 0;
				CALL_SYS_SendMsg(_this,pmc_ENGINE_LOAD, pm_ENGINE_LOAD_BASE_UNRESOLVED_SYMBOL,0,(tPTR)lParam,&len);
				_this->data->DB_Broken = 1;
			}
			break;

		case AVP_CALLBACK_ERROR_SYMBOL_DUP:
			{
				tDWORD len=lParam ? strlen((tSTRING)lParam)+1 : 0;
				CALL_SYS_SendMsg(_this,pmc_ENGINE_LOAD, pm_ENGINE_LOAD_BASE_DUPLICATE_SYMBOL,0,(tPTR)lParam,&len);
			}
			break;

		case AVP_CALLBACK_FILE_OPEN_FAILED_TRY_AGAIN:
		case AVP_CALLBACK_SETFILE_OPEN_FAILED_TRY_AGAIN:
			PR_TRACE(( _this, prtERROR, "ERROR CB_0x%08x: %s",wParam,(char*)so ));
			break;
			
		case AVP_CALLBACK_LOAD_RECORD_NAME:
			if(lParam && PR_SUCC(CALL_SYS_ObjectCreateQuick(_this, &str, IID_STRING,PID_ANY,SUBTYPE_ANY ))){
				if(PR_SUCC(CALL_String_ImportFromBuff(str,0,(tSTRING)lParam,strlen((tSTRING)lParam)+1,cCP_ANSI,cSTRING_Z)))
					CALL_SYS_SendMsg(_this,pmc_ENGINE_LOAD, pm_ENGINE_LOAD_RECORD,str,0,0);
				CALL_SYS_ObjectClose(str);
			}
			break;
		case AVP_CALLBACK_LINK_NAME:
		case AVP_CALLBACK_OBJ_NAME:
			break;
			
		case AVP_CALLBACK_LOAD_PROGRESS:
			break;
			
		case AVP_CALLBACK_LOAD_BASE_DONE:
			{
				LOAD_PROGRESS_STRUCT* lps =(LOAD_PROGRESS_STRUCT*)lParam;
				_this->data->DB_Verdicts=lps->Verdicts;
				_this->data->DB_Count=lps->recCount;
				DTSet(&(_this->data->DB_Time),
					lps->LastUpdateYear,
					lps->LastUpdateMonth,
					lps->LastUpdateDay,
					(BYTE)(lps->LastUpdateTime>>24),
					(BYTE)(lps->LastUpdateTime>>16),
					(BYTE)(lps->LastUpdateTime>>8),
					((BYTE)lps->LastUpdateTime) * 10000000);
				
				{
					tQWORD minutes;
					tDWORD year, month, day, hour, minute, second;
					DTGet((const tDT *) &_this->data->DB_Time, &year, &month, &day, &hour, &minute, &second, 0);
					DTIntervalGet(&(_this->data->DB_Time), 0, &minutes, 60000000000);
					minutes++;
				}
				
			}
			break;
			
		case AVP_CALLBACK_LOAD_BASE:
			{
				LOAD_PROGRESS_STRUCT* lps =(LOAD_PROGRESS_STRUCT*)lParam;
				tDWORD size=sizeof(tDT);
				_this->data->DB_Count=lps->recCount;
				CALL_String_ImportFromBuff(_this->data->DB_LoadName,0,lps->curName,strlen(lps->curName)+1,cCP_ANSI,cSTRING_Z);
				DTSet(&(_this->data->DB_LoadTime),
					lps->CurBaseYear,
					lps->CurBaseMonth,
					lps->CurBaseDay,
					(BYTE)(lps->CurBaseTime>>24),
					(BYTE)(lps->CurBaseTime>>16),
					(BYTE)(lps->CurBaseTime>>8),
					((BYTE)lps->CurBaseTime) * 10000000);
				if(PR_FAIL(CALL_SYS_SendMsg(_this,pmc_ENGINE_LOAD, pm_ENGINE_LOAD_BASE,_this->data->DB_LoadName,&_this->data->DB_LoadTime,&size)))
					error=1;
			}
			break;
			
		case AVP_CALLBACK_ERROR_LINK:
			break;
			
		case AVP_CALLBACK_IS_PRAGUE_HERE:
			error=1;
			break;
			
		default:
			break;
	}
	}

	
	if(error==errOPERATION_CANCELED) 	
		BaseAvpCancelProcessObject( _this->data->Base,error);

#if !defined (__unix__)	
	if(stack_align)
		_asm pop bx;
#endif		

	return error;
}

tVOID VndDestroy( hi_Engine _this )
{
	tDWORD i;
	if(_this->data->vndArray){
		for(i=0;i<_this->data->vndArrayCount;i++)
			obj_free(_this->data->vndArray[i]);
		obj_free(_this->data->vndArray);
	}
	_this->data->vndArrayCount=0;
	_this->data->vndArray=NULL;
}

tBOOL VndInit( hi_Engine _this, const char* filename)
{
	HDATA hRootData=NULL;
	HPROP hKernelProp;
	AVP_dword nCount;
	AVP_dword i;
	MAKE_ADDR1( hKernelAddr, AVP_PID_VNDKERNELDATA );
	
	VndDestroy(_this);
	if(filename==NULL) return cFALSE;
	if(SignCheck)
		if(PR_FAIL(SignCheck((hOBJECT)filename,1,0,0,0,0)))
			return cFALSE;
	if(!KLDT_Init_Library(obj_malloc,obj_free))
		return cFALSE;
	if(!DATA_Init_Library(obj_malloc,obj_free))
		return cFALSE;
	
	if(!KLDT_DeserializeUsingSWM( filename, &hRootData ))return cFALSE;
	if(hRootData==NULL) return cFALSE;
	if(AVP_PID_VNDROOT!=DATA_Get_Id( hRootData, 0 )){
		DATA_Remove( hRootData, 0 );
		return cFALSE;
	}
	
	
	hKernelProp = DATA_Find_Prop( DATA_Find(hRootData, hKernelAddr), NULL, 0 );
	
	nCount = PROP_Arr_Count( hKernelProp );
	
	_this->data->vndArray= (void**)obj_malloc(sizeof(DWORD)*nCount);
	if(_this->data->vndArray==NULL)goto dr;
	
	for ( i=0; i<nCount; i++ ) {
		AVP_dword nItemSize = PROP_Arr_Get_Items( hKernelProp, i, 0, 0 );
		AVP_char *pBuffer = (AVP_char *) obj_malloc(nItemSize);
		if(pBuffer==NULL)break;
		PROP_Arr_Get_Items( hKernelProp, i, pBuffer, nItemSize );
		_this->data->vndArray[i]=pBuffer;
	}
	_this->data->vndArrayCount=i;
dr:
	DATA_Remove( hRootData, 0 );
	return !!_this->data->vndArrayCount;
}


#if defined (__unix__)

#undef declare
#define declare(x) \
    t_Avp##x Avp##x = 0;

declare( Callback )
declare( Strrchr )
declare( Strlwr )
declare( ConvertChar )

declare( CreateFile )
declare( CloseHandle )
declare( ReadFile )
declare( WriteFile )
declare( GetFileSize )
declare( SetFilePointer )
declare( SetEndOfFile )
declare( GetDiskFreeSpace )
declare( GetFullPathName )
declare( DeleteFile )
//declare( MoveFile )
declare( GetFileAttributes )
declare( SetFileAttributes )
declare( GetTempPath )
declare( GetTempFileName )

declare( MemoryRead )
declare( MemoryWrite )
declare( GetDosMemSize )
declare( GetFirstMcbSeg )
declare( GetIfsApiHookTable )
declare( GetDosTraceTable )
declare( GetDiskParam )
declare( Read13 )
declare( Write13 )
declare( Read25 )
declare( Write26 )
declare( ExecSpecial )

#endif
