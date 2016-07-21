#include "BaseDrv.h"

KLSTATUS
IoctlHandler(ULONG nIoctl, PVOID pInBuffer, ULONG InBufferSize, PVOID pOutBuffer, ULONG OutBufferSize, ULONG* nReturned)
{
    return  CKl_Filter::ReceiveMessage( nIoctl,pInBuffer, InBufferSize, pOutBuffer, OutBufferSize, nReturned );
}

ULONG
BaseEntry()
{
    pDisp = new CKl_Dispatcher();
    if ( pDisp->Start() != KL_SUCCESS )
    {
        delete pDisp;
        return STATUS_UNSUCCESSFUL;
    }
    
	Dbg->Stop("compiled at " __DATE__ " " __TIME__"\n");

    if ( Dbg->m_ID == DISPATCHER_ID_LOG )
    {
        CKl_Registry    MyReg( 0, RegistryPath->Buffer );
        wchar_t     buf[1000];
        ULONG       size = 1000 * sizeof (wchar_t);
     
        if ( KL_SUCCESS == MyReg.QueryValue(NULL, L"TraceFile", (char*)buf, &size) )
        {
            UNICODE_STRING  us;
            ANSI_STRING     as;
            RtlInitUnicodeString(&us, buf);
            RtlUnicodeStringToAnsiString(&as, &us, TRUE);

            ((CKl_LogDebugInfo*)Dbg)->SetOutputFile( as.Buffer );
            RtlFreeAnsiString(&as);
        }
        else
        {
            ((CKl_LogDebugInfo*)Dbg)->SetOutputFile("C:\\log.txt");
        }
    }    

    Dbg->Print(BIT_INIT, LEVEL_NOTIFY, "------------   Initialized ------------- \n");
}