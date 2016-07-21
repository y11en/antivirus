
#define   DEVICE_MAIN
#include  "vxdhook.h"
#undef    DEVICE_MAIN

//Declare_Virtual_Device(AVPG)
#include "..\hook\avpgcom.h"

#include "../syscalls.h"

Declare_Layered_Driver (KLIF, DRP_MISC_PD, "KLIF_VXD", 1, 0, 0)

VOID LocalControlDispatcherAvpg(VOID);

__EXC__ DDB The_DDB_Avpg = { 0, DDK_VERSION, 0x382E, KLIF_Major,
	KLIF_Minor,  0, {'A','V','P','G',' ',' ',' ',' '},
	KLIF_Init_Order, (DWORD) LocalControlDispatcherAvpg,
	(DWORD) LocalV86handler,
	(DWORD) LocalPMhandler, 0, 0, 0, (DWORD) VXD_SERVICE_TABLE, 
	0,
	0,
	__SIG__} ;

__EXC__ VOID __cdecl KLIF_Aer(AEP* BABA)
{
}

#if DEBUG
#define __DBG__
#endif


#define DBG_PREFIX "HOOK"

#include "../osspec.h"
#include "../debug.h"

#include "../hook\avpgcom.h"
#include "../Hook\HookSpec.h"


NTSTATUS ReloadSettings(ULONG SettingsMask);
BOOL _DiskIO(PDISK_IO pdio, BYTE* Buffer);

#include "specfpr.h"

#include "../Vxdcommn.cpp"

#include "../client.h"
#include "../osspec.h"
#include "../kldefs.h"
#include "../klsecur.h"
#include "../InvThread.h"
#include "../namecache.h"
#include "../evqueue.h"
#include "../filter.h"
#include "../TSPv2.h"

#include "../nt/runproc.h"

ULONG	gDrvFlags = 0;
#include "..\common.c"

#include "io.c"
#include "..\r3.c"

/*
typedef struct {
        UCHAR Drive;
        UCHAR   Len;
        UCHAR   Head;
        UCHAR Sector;
        ULONG Cylinder;
} READ_INT13,*PREAD_INT13;

BOOLEAN ReadI13(PREAD_INT13 Req, PVOID OutBuff)
{
VMHANDLE			SysVMHandl;
CLIENT_STRUCT	SavedRegs;
CLIENT_STRUCT	*pRegs;
DWORD				Actual;
SEGOFFSET		pBuffer;
DWORD				BuffSize=Req->Len*512;
BOOLEAN			ret=FALSE;
WORD				CopySeg;
	DbPrint(DC_SYS,4, ("ReadI13 Disk=%x Head=%d Cyl=%d Sec=%d Len=%d\n",Req->Drive,Req->Head,Req->Cylinder,Req->Sector,Req->Len));
	SysVMHandl=Get_Sys_VM_Handle();
	pRegs=(CLIENT_STRUCT*)SysVMHandl->CB_Client_Pointer;
	Save_Client_State(&SavedRegs);
	if(V86MMGR_Allocate_Buffer(SysVMHandl,pRegs,BuffSize,0,NULL,FALSE,&Actual,&pBuffer)) {
		if(Actual>=BuffSize) {
			Begin_Nest_V86_Exec();
			_clientAH=2;
			_clientAL=Req->Len;
			_clientCH=(UCHAR)(Req->Cylinder & 0xff);
			_clientCL=(UCHAR)((Req->Cylinder & ~0xff)>>2) + Req->Sector;
			_clientDH=Req->Head;
			_clientDL=Req->Drive;
			_clientES=HIWORD(pBuffer);
			_clientBX=LOWORD(pBuffer);
			Exec_Int(0x13);
			if(!(_clientFlags & 1))
				ret=TRUE;
			else
				DbPrint(DC_SYS,2, ("Exec_Int13 return carry flag\n"));
			End_Nest_Exec();
		} else {
			DbPrint(DC_SYS,2, ("V86MMGR_Allocate_Buffer - too small buffer\n"));
		}
		_asm {
			mov ax, ds;
			mov CopySeg, ax;
		}
		V86MMGR_Free_Buffer(SysVMHandl,pRegs,Actual,CopySeg,OutBuff,TRUE);
	} else {
		DbPrint(DC_SYS,2, ("V86MMGR_Allocate_Buffer failed\n"));
	}
	Restore_Client_State(&SavedRegs);
	return ret;
}
*/

VOID KillCurrentProcess(VOID)
{
/*VMHANDLE CurVM;
	CurVM=Get_Cur_VM_Handle();
	if(!Test_Sys_VM_Handle(CurVM)) {
		Close_VM(3*1000,CurVM,0);
	} else {
*/		VWIN32_TerminateApp((HANDLE)Get_Cur_Thread_Handle());
//	}
}

#include "registry.c"
#include "ios.c"
#include "ifs.c"
#include "i21.c"

/******************************** Low level disk IO ***************************************************/
#include "LLDiskIO.c"
#include "cd9x.c"
/*****************************************************************************************************/
#include "firewall.c"

BOOLEAN IsAvpgExist()
{
	BOOLEAN		bRet = TRUE;
	NTSTATUS	ntstatus;
	HKEY		AvpgKeyHandle;

	PCHAR AvpgReg = "System\\CurrentControlSet\\Services\\VxD\\AVPG";

	if((ntstatus = RegOpenKey(HKEY_LOCAL_MACHINE, AvpgReg, &AvpgKeyHandle)) != ERROR_SUCCESS)
		bRet = FALSE;
	else
		RegCloseKey(AvpgKeyHandle);

	return bRet;
}

/******************************************* Hookers *************************************************/
/*
BOOLEAN Hook(VOID)
{
	I21Hook();
	if(!(PrevIFSHookProc=IFSMgr_InstallFileSystemApiHook(IFSHookProc)))
		goto err;
#ifdef __DBG__
	{
	char vxdname[100];
	vxdname[0] = 0;
	GetVxDName((ULONG)*PrevIFSHookProc, vxdname);
	DbPrint(DC_SYS,3, ("IFS hooked. Previos IFSHook is %x (%s)\n", *PrevIFSHookProc,vxdname));
	}
#endif
	if(!(PrevIOSHookProc=Hook_Device_Service(__IOS_SendCommand,IOSHookProc, &IOSThunk)))
		goto err;
	DbPrint(DC_SYS,3, ("IOS hooked. Previos IOSHook is %x\n", PrevIOSHookProc));
	
	return TRUE;
err:
	return FALSE;
}

VOID Unhook(VOID)
{
NTSTATUS Status;
	RegUnHook();
	Status=Unhook_Device_Service(__IOS_SendCommand,IOSHookProc, &IOSThunk);
	DbPrint(DC_SYS,3, ("IOS unhooked. Status=%x\n",Status));
	Status=IFSMgr_RemoveFileSystemApiHook(IFSHookProc);
	DbPrint(DC_SYS,3, ("IFS unhooked. Status=%x\n",Status));
	I21UnHook();
}
*/

BOOLEAN DrvRequestHook(ULONG HookID)
{
	DbPrint(DC_SYS,DL_NOTIFY, ("----------------------- DrvRequestHook - HookID 0x%x -----------------------\n", HookID));
	switch(HookID) {
	case FLTTYPE_REGS:
		return RegHook();
	case FLTTYPE_IOS:
		return IOSHook();
	case FLTTYPE_IFS:
		return IFSHook();
	case FLTTYPE_I21:
		DbPrint(DC_I21,DL_IMPORTANT, ("I21HookUsed\n"));
		I21HookUsed=TRUE;
		return TRUE;//I21Hook();  //!!
	case FLTTYPE_R3:
		DbPrint(DC_R3,DL_IMPORTANT, ("R3HookUsed\n"));
		R3HookUsed=TRUE;
		return TRUE;//R3Hook();
	case FLTTYPE_PID:
		return PIDStart();
	case FLTTYPE_IDS:
		return IDSStart();
	case FLTTYPE_KLPF:
		return KLPFStart();
	case FLTTYPE_MCHECK:
		return KLMCStart();
	case FLTTYPE_FLT:
	case FLTTYPE_CKAH_IDS:
		return TRUE;
	case FLTTYPE_KLICK:
		return KlickStart();
	case FLTTYPE_KLIN:
		return KlinStart();
	default:
		DbPrint(DC_SYS,DL_WARNING, ("DrvRequestHook - Unknown HookID - %d\n", HookID));
		return FALSE;
	}
	return TRUE;
}

BOOLEAN DrvRequestUnhook(ULONG HookID)
{
	DbPrint(DC_SYS,DL_NOTIFY, ("----------------------- DrvRequestUnHook - HookID 0x%x -----------------------\n", HookID));
	switch(HookID) {
	case FLTTYPE_REGS:
		return RegUnHook();
	case FLTTYPE_IOS:
		return IOSUnHook();
	case FLTTYPE_IFS:
		// 	 on 95 windows - sometimes lock
		//return IFSUnHook();
		return TRUE;
	case FLTTYPE_I21:
		DbPrint(DC_I21,DL_IMPORTANT, ("I21Hook is not used\n"));
		I21HookUsed=FALSE;
		return TRUE;//I21UnHook();  //!!
	case FLTTYPE_R3:
		DbPrint(DC_R3,DL_IMPORTANT, ("R3Hook is not used\n"));
		R3HookUsed=FALSE;
		return TRUE;//R3UnHook();
	case FLTTYPE_PID:
		return PIDStop();
	case FLTTYPE_IDS:
		return IDSStop();
	case FLTTYPE_KLPF:
		return KLPFStop();
	case FLTTYPE_MCHECK:
		return KLMCStop();
	case FLTTYPE_FLT:
	case FLTTYPE_CKAH_IDS:
		return TRUE;
	case FLTTYPE_KLICK:
		return KlickStop();
	case FLTTYPE_KLIN:
		return KlinStop();
	default:
		DbPrint(DC_SYS,DL_WARNING, ("DrvRequestUnhook - Unknown HookID - %d\n", HookID));
		return FALSE;
	}
	return TRUE;
}

VOID UnHookAll(VOID)
{
	RegUnHook();
	IOSUnHook();
	IFSUnHook();
	I21UnHook();
//	R3UnHook();
}

/****************************************** Dispatchers **********************************************/
VOID OnDestroyProcess(DWORD ProcessToken)
{
	if(Obsfucator) {
		DbPrint(DC_SYS,DL_SPAM, ("OnDestroyProcess called. ProcToken=%x\n", ProcessToken));
		NameCacheCleanupByProc((HANDLE)(ProcessToken ^ Obsfucator));
//		MyUnLockMemoryByProc((PVOID)(ProcessToken ^ Obsfucator));
		TSPProcCrash((PVOID)(ProcessToken ^ Obsfucator));
	}
	DisconnectClientByProcID((ULONG)(ProcessToken/* ^ Obsfucator*/));
}

VOID OnDestroyThread(THREADHANDLE hThread)
{
	DbPrint(DC_SYS,DL_SPAM, ("OnDestroyThread called. hThread=%x CurTh=%x\n", hThread,Get_Cur_Thread_Handle()));
	DisconnectClientByThreadID(hThread);
	DelInvisibleThread((ULONG) hThread, TRUE);
	TSPThreadCrash((PVOID)hThread);
}

BOOL OnW32Deviceiocontrol(PIOCTLPARAMS p, BOOLEAN bNew)
{
ULONG StatStat;
	*p->dioc_bytesret=0;
	switch (p->dioc_IOCtlCode) {
	case DIOC_OPEN:
		DbPrint(DC_SYS,DL_NOTIFY, ("IOCTLHOOK_OPEN\n"));
		return FALSE;
	case DIOC_CLOSEHANDLE:
		DbPrint(DC_SYS,DL_NOTIFY, ("IOCTLHOOK_CLOSEHANDLE. gActiveClientCounter=%d\n", gActiveClientCounter));
		if(gActiveClientCounter != 0) {
			//Здесь что то нужно придумать по поводу разрешения/отказа выгрузки
			}
		return FALSE;
	default:
		return !HOOKDeviceControl(p->dioc_InBuf,p->dioc_cbInBuf,p->dioc_OutBuf,p->dioc_cbOutBuf,p->dioc_IOCtlCode,p->dioc_bytesret,&StatStat, bNew, FALSE);
	}
}

VOID OnSystemExit(VMHANDLE hVM)
{
	DbPrint(DC_SYS,DL_IMPORTANT, ("OnSystemExit\n"));
	UnHookAll();
	CommonDone();
}


BOOL OnSysDynamicDeviceExit()
{
	DbPrint(DC_SYS,DL_NOTIFY, ("OnSysDynamicDeviceExit\n"));
	UnloadInProgress=TRUE;
	UnHookAll();
	CommonDone();
	DbPrint(DC_SYS,DL_IMPORTANT, ("ControlDispatcher addr=%X\n",ControlDispatcher));
	DbPrint(DC_SYS,DL_IMPORTANT, ("unloaded\n"));
	return TRUE;
}

BOOL OnSysDynamicDeviceInit()
{
//NTSTATUS				ntstatus;
ULONG VmmVer;
ULONG VmmDebRev;
	RecognizeFuncs((ULONG) OnSysDynamicDeviceInit);
	
	VmmVer=Get_VMM_Version(&VmmDebRev);
	DbPrint(DC_SYS,DL_IMPORTANT, ("DYNAMIC_DEVICE_INIT. VMM ver %x, debug rev %x\n",VmmVer,VmmDebRev));
#ifdef __DBG__
	_VolFlush(0,0);
	_VolFlush(1,0);
	_VolFlush(2,0);
	_VolFlush(3,0);
#endif //__DBG__
	if(VmmVer>=0x45a) {
		W16mtxOThOffset=0x0c;
	} else {
		W16mtxOThOffset=0x10;
	}
// Init commons NT & VxD data	
	IFSInit();
	if(CommonInit()!=STATUS_SUCCESS) {
		DbPrint(DC_SYS,DL_FATAL_ERROR, ("error_on_init\n"));
		return FALSE;
	}
	I21Hook();

	R3HookInit();
	
	DbPrint(DC_SYS,DL_IMPORTANT,("loading filters\n"));
	LoadFilters();
	DbPrint(DC_SYS,DL_IMPORTANT,("end loading filters\n"));

	if (!IsAvpgExist())
	{
		DbPrint(DC_SYS, DL_IMPORTANT,("register klif as avpg\n"));
		VMM_Add_DDB(&The_DDB_Avpg);
	}
	
	StartAWDog();
	
	return TRUE;
}

VOID OnKernel32Initialized(VOID)
{
	DbPrint(DC_SYS,DL_IMPORTANT, ("OnKernel32Initialized\n"));
	R3Hook();
}

VOID OnKernel32Shutdown(VOID)
{
	DbPrint(DC_SYS,DL_IMPORTANT, ("OnKernel32Shutdown\n"));
//	R3UnHook();
}

BOOL __cdecl ControlDispatcher(DWORD dwControlMessage,DWORD EBX,DWORD EDX,DWORD ESI,DWORD EDI,DWORD ECX)
{
	switch(dwControlMessage) {
	case SYS_DYNAMIC_DEVICE_INIT:
		OnSysDynamicDeviceInit();
		R3Hook();//!! ТОлько для вновь запущенных процессов
		return TRUE;
//	case DEVICE_INIT:
	case INIT_COMPLETE:
		bUpperTableInited = FALSE;
		return OnSysDynamicDeviceInit();
	case SYS_DYNAMIC_DEVICE_EXIT:
		return OnSysDynamicDeviceExit();
	case W32_DEVICEIOCONTROL:
		return OnW32Deviceiocontrol((PIOCTLPARAMS)ESI, TRUE);
	case DESTROY_PROCESS:
		OnDestroyProcess(EDX);
		return TRUE;
	case SYSTEM_EXIT:
		OnSystemExit((VMHANDLE)EBX);
		return TRUE;
	case DESTROY_THREAD:
		OnDestroyThread(EDI);
		break;
	case KERNEL32_INITIALIZED:
		OnKernel32Initialized();
		SpecialFileProcessing();
		break;
	case KERNEL32_SHUTDOWN:
		OnKernel32Shutdown();
		break;
	default: break;
	}
	return TRUE;
}

BOOL __cdecl ControlDispatcherAvpg(DWORD dwControlMessage,DWORD EBX,DWORD EDX,DWORD ESI,DWORD EDI,DWORD ECX)
{
	if (dwControlMessage == W32_DEVICEIOCONTROL)
	{
		DbgBreakPoint();
		return OnW32Deviceiocontrol((PIOCTLPARAMS)ESI, FALSE);
	}

	return TRUE;
}

__declspec(naked) VOID LocalControlDispatcherAvpg(VOID)
{
	_asm
	{
	push	ecx
	push	edi
	push	esi
	push	edx
	push	ebx
	push	eax

;	bts	bInitialized,0		;; test initialization state
;	jnc	LocalInit		;; jump if not initialized

;LocalInitReturn:

;; check for possible failure conditions from local initialization
;;
	cld				;; Ensure direction clear for C code
	call	ControlDispatcherAvpg	;; call C dispatcher

	pop	ebx			;; Calling EAX holds message number

	cmp	ebx, PNP_NEW_DEVNODE	;; new devnode needs value AND carry bit
	  je	Value_newdevnode
	cmp	ebx, W32_DEVICEIOCONTROL;; same treatment for W32_DEVICEIOCONTROL
	  je	Value_newdevnode

	bt	eax, 0		;; store low bit into carry
	cmc				;; bool (1) means true, no carry


Return_newdevnode:

	pop	ebx			;;    (for W32_DEVICEIOCONTROL, PNP_NEWDEVNODE)
	pop	edx
	pop	esi
	pop	edi
	pop	ecx

	ret


Value_newdevnode:
	or	eax, eax		;; test for CR_SUCCESS (== 0)
	setne	bl			;; if not 0, bl := -1
	sar	bl, 1			;; set carry bit if not 0
	jmp	Return_newdevnode	;; return eax and carry bit

	}
}// EndProc		_LocalControlDispatcher

//+ ------------------------------------------------------------------------------------------
NTSTATUS ReloadSettings(ULONG SettingsMask)
{
	LoadSputniks();
	return STATUS_SUCCESS;
}
//+ ------------------------------------------------------------------------------------------

/*
Known bugz and incompleteness:
1.Kill.Убиение из IFS_Hook чревато зависанием на IFSMgr_RemoveFileSystemApiHook
2.Сделать нормальную обработку незапуска логфлушера.
3.Кэш имен неэффиктивный!!! пофиксить owner для виртуальных машин + подчистка по DESTROY_VM
4.Проверить OnDestroyProcess для виртуальных машин. (Destroy_VM)

To Do List
0.Можно вернуться к динамической загрузке если сделать обработчик Create_VM и в нем навешивать хуки i21 (+повесить для всех существующих в момент загрузки)
1.Посадка на функции FS вместо FileSystemApiHook (SK!)
2.Проверка не проникновение в 0 кольцо.
3.Check rehookers
4.Защита FlashBIOS
SHELL_ShellExecute

	_PELDR_LoadModuleEx
 _PELDR_LoadModule	
	
*/
