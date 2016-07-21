// FSAVPDevice.cpp
// Implementation of FSAVPDevice device class

#include <vdw.h>
#include <Sign/sign.h>

#include "FSBase.h"
#include "FSAVP.h"
#include "FSAVPDev.h"
#include "msglog.h"

extern PVOID CallbackHandle;

KEvent* appNotificationEvent;
CPtrArray DetectArray;
KMutex DetectArrayMutex(3);
KMutex StatMutex(3);

AVPScanObject* DetectArrayRemoveAt(int n)
{
	AVPScanObject* p=NULL;
	DetectArrayMutex.Wait();
	if(n<DetectArray.GetSize())
	{
		p=(AVPScanObject*)DetectArray[n];
		DetectArray.RemoveAt(n);
	}
	DetectArrayMutex.Release();
	return p;
}

void DetectArrayRemoveAll()
{
	int i;
	DetectArrayMutex.Wait();
	while(i=DetectArray.GetSize()){
		i--;
		AVPScanObject* p=(AVPScanObject*)DetectArray[i];
		DetectArray.RemoveAt(i);
		if(p)delete p;
	}
	DetectArrayMutex.Release();
}

void DetectSetEvent()
{
	if(appNotificationEvent)
	{
		int size;
		DetectArrayMutex.Wait();
		size=DetectArray.GetSize();
		DetectArrayMutex.Release();
		if(size)
			appNotificationEvent->Pulse();
	}
}

void DetectArrayAdd(AVPScanObject* p, char* realName, BOOL add_always)
{
    _DebugTrace(TraceInfo,"DetectArrayAddAndSetEvent: %s \n",p->Name);
	if(appNotificationEvent || add_always)
	{
		AVPScanObject *so=new(POOL) AVPScanObject;
		if(so){
			memcpy(so,p,sizeof(AVPScanObject));
			if(realName)strcpy(so->Name,realName);
			DetectArrayMutex.Wait();
			DetectArray.Add(so);
			DetectArrayMutex.Release();
		}
	}
}




#pragma code_seg("INIT")

// FSAVPDevice Constructor
//              The device constructor is typically responsible for allocating
//              any physical resources that are associated with the device.
//
//              The device constructor often reads the registry to setup
//              various configurable parameters.
//
FSAVPDevice::FSAVPDevice(ULONG Unit) :
	KDevice(
		KUnitizedName(L"FSAVPDevice", Unit),
		FILE_DEVICE_UNKNOWN,
		KUnitizedName(L"FSAVPDevice", Unit),
		0,
		DO_BUFFERED_IO)


	// Initialize KTrace object in device class
	,t(
		KUnitizedName(L"FSAVPDevice", Unit),
		defTraceDevice,
		defTraceOutputLevel,
		defTraceBreakLevel)
{
	if ( ! NT_SUCCESS(m_ConstructorStatus) )
	{
		_DebugTrace(TraceError, "Failed to create device FSAVPDevice unit number %d, status %x\n", Unit, m_ConstructorStatus);
		return;
	}
	m_Unit = Unit;

	m_RegPath = CreateRegistryPath(L"FSAVPDevice", Unit);
	if (m_RegPath == NULL)
	{
		// Error, cannot allocate memory for registry path
		_DebugTrace(TraceError, "Failed to create registry path\n");
		m_ConstructorStatus = STATUS_INSUFFICIENT_RESOURCES;
		return;
	}


}
#pragma code_seg()

// FSAVPDevice Destructor
//
FSAVPDevice::~FSAVPDevice()
{

    if (appNotificationEvent)
    {
        delete appNotificationEvent;
        appNotificationEvent = NULL;
		DetectArrayRemoveAll();
    }
	delete m_RegPath;

}

static char includeMask[0x200];
static char excludeMask[0x200];

extern void AddMaskEx(char* buff, BOOL exclude);

static void AddMask(char* buff, BOOL exclude)
{
	if(WaitIfBusy()){
		AddMaskEx(buff, exclude);
		ReleaseWait();
	}

	KRegistryKey Params(sRegistryPath, L"Parameters");
	if(!NT_SUCCESS(Params.LastError()))return;
	KUstring s(0x200,POOL);

	PWSTR key;
	if(exclude) key=L"ExcludeMask";
	else		key=L"IncludeMask";

	s.Assign(L"");
	if(buff && *buff)
	{				
		KUstring b(buff,POOL);
		PWSTR str;
		ULONG l=0;
		if( NT_SUCCESS(Params.QueryValue(key, str, l, POOL)) )
		{
			s.Assign(str);
			delete str;
			if(s.Length())	s.Append(L",");
		}
		s.Append(b);
	}
	Params.WriteValue(key,((PUNICODE_STRING)s)->Buffer);
}

// Member functions of FSAVPDevice
//

// Major function handlers
//
NTSTATUS FSAVPDevice::DeviceControl(KIrp I)
{
//    t.Trace(TraceInfo, "Entering DeviceControl, IRP= %x, IOCTL code= %x\n",(PIRP)I, I.IoctlCode());
    NTSTATUS status = STATUS_SUCCESS;
	I.Information()=0;


// TODO: Handle Internal Device Control request
    switch (I.IoctlCode())
    {
	case IOCTL_AVP_GET_VERSION:
//OutBuf: DWORD Version; OutBufSize 4;
		if(I.IoctlOutputBufferSize() < sizeof(DWORD)){
			status = STATUS_BUFFER_TOO_SMALL;
			break;
		}
		*(DWORD*)I.IoctlBuffer() = AVP_IOCTL_Version;
		I.Information() = sizeof(DWORD);
		break;

	case IOCTL_AVP_GET_MFLAGS:
//OutBuf: DWORD Enable; OutBufSize 4;
		if(I.IoctlOutputBufferSize() < sizeof(DWORD)){
			status = STATUS_BUFFER_TOO_SMALL;
			break;
		}
		*(DWORD*)I.IoctlBuffer() = MFlags;
		I.Information() = sizeof(DWORD);
		break;

	case IOCTL_AVP_SET_MFLAGS      :
//InBuf: DWORD MFlags; InBufSize 4;
		if(I.IoctlInputBufferSize() < sizeof(DWORD)){
			status = STATUS_BUFFER_TOO_SMALL;
			break;
		}
		if(!WaitIfBusy()) break;
		BFlags = MFlags = *(DWORD*)I.IoctlBuffer();
		ReleaseWait();
		{
			KRegistryKey Params(sRegistryPath, L"Parameters");
			if(NT_SUCCESS(Params.LastError()))
				Params.WriteValue(L"MFlags",*(DWORD*)I.IoctlBuffer());
		}
		Flush();
		break;

	case IOCTL_AVP_GET_STAT        :
//OutBuf: AVPStat stat; OutBufSize sizeof(AVPStat);
		if(I.IoctlOutputBufferSize() < sizeof(AVPStat)){
			status = STATUS_BUFFER_TOO_SMALL;
			break;
		}
		StatMutex.Wait();
		Stat->MFlags=MFlags;
		Stat->Enable=Enable;
		Stat->StaticMFlags=StaticMFlags;
		memcpy(I.IoctlBuffer(),Stat,sizeof(AVPStat));
		StatMutex.Release();
		I.Information() = sizeof(AVPStat);

//    t.Trace(TraceInfo, "Stat->Suspicions=%d\n",Stat->Suspicions);

		break;

	case IOCTL_AVP_STATIC_MFLAGS   :
//InBuf: DWORD StaticMFlags; InBufSize 4; (BOOL)
		if(I.IoctlInputBufferSize() < sizeof(DWORD)){
			status = STATUS_BUFFER_TOO_SMALL;
			break;
		}
		StaticMFlags = *(DWORD*)I.IoctlBuffer();
		Flush();
		break;

	case IOCTL_AVP_RELOAD_BASE       :
	case IOCTL_AVP_LOAD_BASE       :
//InBuf: char* SetName; InBufSize strlen(SetName);
		if(I.IoctlInputBufferSize() ==0 ){
			if(*SetName==0){
				status = STATUS_BUFFER_TOO_SMALL;
				break;
			}
		}
		else{
			*SetName=0;
			if(*((char*)I.IoctlBuffer()+1)==':')strcat(SetName,NT_ROOT_PREFIX);
			strcat(SetName,(char*)I.IoctlBuffer());
		}
		BaseLoad();
	
		Flush();
		DetectSetEvent();//report about BAD BASES if such exists
		break;

	case IOCTL_AVP_PROCESS_OBJECT  :
//InBuf:  AVPScanObject so; InBufSize sizeof(AVPScanObject);
		if(I.IoctlInputBufferSize() < sizeof(AVPScanObject) ||
		   I.IoctlOutputBufferSize() < sizeof(AVPScanObject))
		{
			status = STATUS_BUFFER_TOO_SMALL;
			break;
		}
		else
		{
//			AVPScanObject *so=(AVPScanObject*)I.IoctlBuffer();
			AVPScanObject *so=new AVPScanObject;
			if (so==NULL){
				status = STATUS_INTERNAL_ERROR;
				break;
			}
			memcpy(so,I.IoctlBuffer(),sizeof(AVPScanObject));
			HandlerInfo* hi=new HandlerInfo;
			if (hi==NULL){
				delete so;
				status = STATUS_INTERNAL_ERROR;
				break;
			}
			memset(hi,0,sizeof(HandlerInfo));
			so->Connection=0;
			hi->uncFilename=so->Name;
			hi->plainFilename=so->Name;
			if(so->Name[1]==':')
			{
				char* buf=new char[0x200];
				if(buf)
				{
					strncpy(buf,so->Name,0x200);
					strcpy(so->Name,NT_ROOT_PREFIX);
					strcat(so->Name,buf);
					hi->plainFilename+=4;
					delete buf;
				}
			}

			if(CallbackHandle
			&& !_FilterRegisterThread(PsGetCurrentThread())){
				status = STATUS_INTERNAL_ERROR;
				break;
			}
	
			ProcessObject(so, hi, TRUE);

			memcpy(I.IoctlBuffer(),so,sizeof(AVPScanObject));
			delete so;
			delete hi;
			I.Information() = sizeof(AVPScanObject);

			if(CallbackHandle
			&& !_FilterDeregisterThread(PsGetCurrentThread())){
				status = STATUS_INTERNAL_ERROR;
				break;
			}

		}
		break;

	case IOCTL_AVP_MAKE_FULL_NAME  :
		if(I.IoctlInputBufferSize() < 4 ||
		   I.IoctlOutputBufferSize() ==0 )
		{
			status = STATUS_BUFFER_TOO_SMALL;
			break;
		}
		else
		{
			char* buf=new char[0x200];
			if(!buf) break;
			if(WaitIfBusy()){
				BAvpMakeFullName(buf,*(char**)I.IoctlBuffer());
				ReleaseWait();
				UINT i=strlen(buf)+1;
				if(i>I.IoctlOutputBufferSize())
				{
					status = STATUS_BUFFER_TOO_SMALL;
					break;
				}else{
					memcpy(I.IoctlBuffer(),buf,i);
					I.Information() = i;
				}
			}
			delete buf;
		}
		break;

	case IOCTL_AVP_GET_BASE_INFO   :
//OutBuf: AVPBaseInfo baseinfo; OutBufSize sizeof(AVPBaseInfo);
		if(I.IoctlOutputBufferSize() < sizeof(AVPBaseInfo)){
			status = STATUS_BUFFER_TOO_SMALL;
			break;
		}
		if(!WaitIfBusy()) break;
		memcpy(I.IoctlBuffer(),BaseInfo,sizeof(AVPBaseInfo));
		ReleaseWait();
		I.Information() = sizeof(AVPBaseInfo);
		break;

	case IOCTL_AVP_ENABLE:
		if(I.IoctlInputBufferSize() < sizeof(DWORD)){
			status = STATUS_BUFFER_TOO_SMALL;
			break;
		}
		if(!WaitIfBusy()) break;
		Enable = *(DWORD*)I.IoctlBuffer();
		ReleaseWait();
		Flush();
		break;

	case IOCTL_AVP_ADD_MASK:
//InBuf: char Mask[]; InBufSize strlen(Mask);
		if(I.IoctlInputBufferSize() < 1){
			status = STATUS_BUFFER_TOO_SMALL;
			break;
		}
		AddMask((char*)I.IoctlBuffer(),0);
		Flush();
		break;

	case IOCTL_AVP_ADD_EXCLUDE_MASK:
//InBuf: char Mask[]; InBufSize strlen(Mask);
		if(I.IoctlInputBufferSize() < 1){
			status = STATUS_BUFFER_TOO_SMALL;
			break;
		}
		AddMask((char*)I.IoctlBuffer(),1);
		Flush();
		break;

	case IOCTL_AVP_SET_MESSAGE_TEXT:
		{
		if(I.IoctlInputBufferSize() < 2*sizeof(DWORD)){
			status = STATUS_BUFFER_TOO_SMALL;
			break;
		}
			int i=((DWORD*)I.IoctlBuffer())[0];
			if(i>=MESS_TEXT_MAX){
				status = STATUS_INVALID_PARAMETER;
				break;
			}
			char* s=(char*)((DWORD*)I.IoctlBuffer())[1];
			if(s==NULL)return 1;
			if(MessText[i])delete MessText[i];
			if(!(MessText[i]=new char[strlen(s) + 1]))return 1;
			strcpy(	MessText[i],s);
		}
		break;

	case IOCTL_AVP_LIMITCOMPOUNDSIZE      :
//InBuf: DWORD MFlags; InBufSize 4;
		if(I.IoctlInputBufferSize() < sizeof(DWORD)){
			status = STATUS_BUFFER_TOO_SMALL;
			break;
		}
		if(!WaitIfBusy()) break;
		LimitCompoundSize = *(DWORD*)I.IoctlBuffer();
		ReleaseWait();
		{
			KRegistryKey Params(sRegistryPath, L"Parameters");
			if(NT_SUCCESS(Params.LastError()))
				Params.WriteValue(L"LimitCompoundSize",*(DWORD*)I.IoctlBuffer());
		}
		break;

	case IOCTL_AVP_REGISTERTHREAD:
		if(!WaitIfBusy()) break;
		if(CallbackHandle)
		if(!_FilterRegisterThread(PsGetCurrentThread()))
			status = STATUS_INTERNAL_ERROR;
		ReleaseWait();
		Flush();
		break;

	case IOCTL_AVP_UNREGISTERTHREAD:
		if(!WaitIfBusy()) break;
		if(CallbackHandle)
		if(!_FilterDeregisterThread(PsGetCurrentThread()))
			status = STATUS_INTERNAL_ERROR;
		ReleaseWait();
		Flush();
		break;

	case IOCTL_AVP_CALLBACK_SYNC_DATA   :
		if(I.IoctlOutputBufferSize() < sizeof(AVPScanObject)){
			status = STATUS_BUFFER_TOO_SMALL;
			break;
		}
		AVPScanObject* ptr;
		ptr=DetectArrayRemoveAt(0);
		if(ptr==NULL){
			status = STATUS_UNSUCCESSFUL;
			I.Information() = 0;
			break;
		}
		memcpy(I.IoctlBuffer(),ptr,sizeof(AVPScanObject));
		delete ptr;
		I.Information() = sizeof(AVPScanObject);
		break;

	case IOCTL_AVP_CALLBACK_CREATE:
		if(I.IoctlInputBufferSize() < sizeof(DWORD)
		|| I.IoctlOutputBufferSize() < sizeof(DWORD)){
			status = STATUS_BUFFER_TOO_SMALL;
			break;
		}
		HANDLE h;
		h= *(HANDLE*)I.IoctlBuffer();
//		DetectArrayRemoveAll();
		appNotificationEvent = new (NonPagedPool) KEvent(h);
		if (!appNotificationEvent)
			   status = STATUS_UNSUCCESSFUL;

		memcpy(I.IoctlBuffer(),&appNotificationEvent,4);
		I.Information() = 4;
		break;

	case IOCTL_AVP_CALLBACK_DELETE:
		if(I.IoctlInputBufferSize() < sizeof(DWORD)){
			status = STATUS_BUFFER_TOO_SMALL;
			break;
		}
        if (appNotificationEvent){  // == (KEvent*)I.IoctlBuffer())
            delete appNotificationEvent;
            appNotificationEvent = NULL;
			DetectArrayRemoveAll();
        }else
			status = STATUS_UNSUCCESSFUL;
		break;

	case IOCTL_AVP_CHECK_FILE_INTEGRITY:
//InBuf: char* SetName; InBufSize strlen(SetName);
		if(I.IoctlInputBufferSize() ==0 ){
			status = STATUS_BUFFER_TOO_SMALL;
			break;
		}
		{
			if(!WaitIfBusy()) break;
			int res=sign_check_file((char*)I.IoctlBuffer(),1,NULL,0,0);
			memcpy(I.IoctlBuffer(),&res,4);
			I.Information()=4;
			ReleaseWait();

		}
		break;

	default:
	    status = STATUS_INVALID_PARAMETER;
		break;
    }

    return I.Complete(status);
}


NTSTATUS FSAVPDevice::CreateClose(KIrp I)
{
//	t.Trace(TraceInfo, "Entering CreateClose, IRP= %x, Major Function= %x\n",
//		   (PIRP)I, I.MajorFunction());
    if (I.MajorFunction() == IRP_MJ_CREATE)
	{
		// TODO: Insert CREATE handler code
	}
	else
	{
		// Must be CLOSE since we are in CREATECLOSE handler
		// TODO: Insert CLOSE handler code
	}

	I.Information() = 0;
	return I.Complete(STATUS_SUCCESS);
}

