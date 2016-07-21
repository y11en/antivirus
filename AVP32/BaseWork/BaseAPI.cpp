////////////////////////////////////////////////////////////////////
//
//  BASEAPI.CPP
//  BASEWORK.LIB API implementation
//  AVP 3.0 Engine
//  Alexey de Mont de Rique [graf@avp.ru], Kaspersky Labs. 1999
//  Copyright (c) Kaspersky Labs.
//
////////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#ifdef __MWERKS__
	#include <string.h>
#endif	

#if (defined(__WATCOMC__) && (defined(__DOS__)||defined(__OS2__)))
  #define AFXAPI
  #include <os2.h>
  #include "../typedef.h"
  #include "../retcode3.h"
  #include "../../common.fil/scanapi/avp_msg.h"
  #include <tchar.h>
  #include "../../common.fil/stuff/_carray.h"
  #ifndef LPARAM
    #define LPARAM DWORD
  #endif
#endif

#if defined (__unix__)
	#include <IOCache/fileio.h>
#endif

#include "Basework.h"
#include <ScanAPI/BaseAPI.h>

static CBaseWork* Base=NULL;
BYTE* AVP_PagePtr=NULL;

DWORD prefix( BAvpGetVersion)()
{
	return 0x00030000;
}


DWORD prefix( BAvpInit)()
{
	DWORD ret=0;
#ifdef _DEBUG
    AvpCallback(AVP_CALLBACK_PUT_STRING,(long)"BAvpInit enter");
#endif
	BAvpDestroy();
	
	CBaseWork* b=new CBaseWork;
	if(b)
	{
		ret=b->IsValid();
		if(ret)
		{
			Base=b;
			AVP_PagePtr=b->WorkArea->Header;
		}
		else delete b;
	}
#ifdef _DEBUG
    AvpCallback(AVP_CALLBACK_PUT_STRING,(long)"BAvpInit exit");
#endif
	return ret;
}

DWORD prefix( BAvpDestroy)()
{
	CBaseWork* b=Base;
    Base=NULL;
    AVP_PagePtr=NULL;

	if(b) delete b;

	return 1;
}

DWORD prefix( BAvpLoadBase)(PCHAR dbname_)
{
	if(Base==NULL) return 0;
	return Base->LoadBase(dbname_, 0);
}
DWORD prefix( BAvpRegisterCallback)(t_AvpCallback* callback_)
{
	if(callback_==NULL)return 0;
#if ((defined(__WATCOMC__) && defined(__DOS__))||(defined(__unix__))||(defined(__MWERKS__)))
#else
	AvpCallback=callback_;
#endif
	return 1;
}

DWORD prefix( BAvpProcessObject)(AVPScanObject * obj_)
{
    if(Base==NULL) return 0;
	bdata->CancelProcess=0;
	bdata->ArchHeaderSum=0;
	if(obj_->Handle){
		HANDLE new_h=Cache_AvpCreateFile(0,0,0,0,0,0,obj_->Handle);
		if(new_h!=INVALID_HANDLE_VALUE)
		{
			obj_->Handle=new_h;
			DWORD ret=Base->ProcessObject(obj_);
			Cache_AvpCloseHandle(new_h);
			return ret;
		}
	}
	return Base->ProcessObject(obj_);
}

DWORD prefix( BAvpDispatch)(AVPScanObject * obj_, BYTE SubType)
{
    if(Base==NULL) return 0;
    return Base->Dispatch(obj_,SubType);
}

VOID  prefix( BAvpCancelProcessObject)(LONG code_)
{
    if(Base==NULL) return;
    bdata->CancelProcess=code_;
}

PCHAR prefix( BAvpMakeFullName)(PCHAR buf_, PCHAR ptr_)
{
    if(Base==NULL) return 0;
    return Base->MakeFullName(buf_, ptr_);
}

DWORD prefix( BAvpAddMask)(PCHAR buf_)
{
    if(Base==NULL) return 0;
    return Base->AddMask(buf_,Base->MaskArray);
}

DWORD prefix( BAvpAddExcludeMask)(PCHAR buf_)
{
    if(Base==NULL) return 0;
    return Base->AddMask(buf_,Base->ExcludeArray);
}

DWORD prefix( BAvpErrorFlush)()
{
    if(Base==NULL) return 0;
	return Base->ErrorFlush();
}

extern "C" void* SaveWorkArea(void* wa)
{
	void* s=(void*)new CWorkArea;
	if(s)
	{
		memcpy(s,wa,sizeof(CWorkArea));
		if(((CWorkArea*)s)->P_Handle)
			((CWorkArea*)s)->P_Position=Cache_AvpSetFilePointer(((CWorkArea*)s)->P_Handle,0,0,SEEK_CUR);
		else
			((CWorkArea*)s)->P_Position=-1;
	}
	return s;
}

extern "C" void  RestoreWorkArea(void* wa,void* s)
{
	if(s && wa){
		memcpy(wa,s,sizeof(CWorkArea));
		if(((CWorkArea*)s)->P_Position!=-1)
			Cache_AvpSetFilePointer(((CWorkArea*)s)->P_Handle,((CWorkArea*)s)->P_Position,0,SEEK_SET);
	}
	if(s) delete s;
}


///-------------------------------------------------------------------

VOID* prefix( BaseAvpInit)()
{
	CBaseWork* b=new CBaseWork;
	if(b && !b->IsValid()){
		delete b;
		b=NULL;
	}
	return b;
}

DWORD prefix( BaseAvpDestroy)(VOID* base)
{
	if(base) delete ((CBaseWork*)base);
	return 1;
}

DWORD prefix( BaseAvpLoadBase)(VOID* base, PCHAR dbname_)
{
	if(base==NULL) return 0;
	return ((CBaseWork*)base)->LoadBase(dbname_, 0);
}

DWORD prefix( BaseAvpProcessObject)(VOID* base, AVPScanObject * obj_)
{
    if(base==NULL) return 0;
	bdata->CancelProcess=0;
	bdata->ArchHeaderSum=0;
	
	if(obj_->Handle){
		HANDLE new_h=Cache_AvpCreateFile(0,0,0,0,0,0,obj_->Handle);
		if(new_h!=INVALID_HANDLE_VALUE)
		{
			obj_->Handle=new_h;
			DWORD ret=((CBaseWork*)base)->ProcessObject(obj_);
			Cache_AvpCloseHandle(new_h);
			return ret;
		}
	}
	return ((CBaseWork*)base)->ProcessObject(obj_);
}

VOID  prefix( BaseAvpCancelProcessObject)(VOID* base, LONG code_)
{
    if(base==NULL) return;
    bdata->CancelProcess=code_;
}

PCHAR prefix( BaseAvpMakeFullName)(VOID* base, PCHAR buf_, PCHAR ptr_)
{
    if(base==NULL) return 0;
    return ((CBaseWork*)base)->MakeFullName(buf_, ptr_);
}

BOOL prefix( BaseIsLinkedCode)(VOID* base, PCHAR ptr_)
{
    if(base==NULL || ptr_==NULL) return 0;
    return ((CBaseWork*)base)->m_pPageMemoryManager->isAllocatedAddress(ptr_);
}
