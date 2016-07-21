#include "kl_netcomm.h"

NTSTATUS TdiFuncs_InitializeTransportHandles( USHORT AddrType, PTDI_HANDLE pTdiHandle)
{
    NTSTATUS NtStatus;
    HANDLE hTransport, hConnection;
    PFILE_OBJECT  pfoTransport, pfoConnection;

    NtStatus = TdiFuncs_OpenTransportAddress( AddrType, &hTransport, &pfoTransport);

    if(NT_SUCCESS(NtStatus))
    {
        /* 
        *  The second step is to create a connection endpoint
        */

        NtStatus = TdiFuncs_OpenConnection(AddrType, &hConnection, &pfoConnection);

        if(NT_SUCCESS(NtStatus))
        {

            /*
            *  The third step is to associate the transport and the connection objects
            */
            NtStatus = TdiFuncs_AssociateTransportAndConnection(hTransport, pfoConnection);

            if(NT_SUCCESS(NtStatus))
            {
                pTdiHandle->hConnection   = hConnection;
                pTdiHandle->hTransport    = hTransport;

                pTdiHandle->pfoConnection = pfoConnection;
                pTdiHandle->pfoTransport  = pfoTransport;
            }
            else
            {
                TdiFuncs_CloseTdiOpenHandle(hConnection, pfoConnection);
                TdiFuncs_CloseTdiOpenHandle(hTransport, pfoTransport);
            }
        }
        else
        {
            TdiFuncs_CloseTdiOpenHandle(hTransport, pfoTransport);
        }
    }

    return NtStatus;
}

/**********************************************************************
* 
*  TdiFuncs_FreeHandles
*
*    This function destroys the handles
*
**********************************************************************/
void TdiFuncs_FreeHandles(PTDI_HANDLE pTdiHandle)
{
    TdiFuncs_DisAssociateTransportAndConnection(pTdiHandle->pfoConnection);
    TdiFuncs_CloseTdiOpenHandle(pTdiHandle->hConnection, pTdiHandle->pfoConnection);
    TdiFuncs_CloseTdiOpenHandle(pTdiHandle->hTransport, pTdiHandle->pfoTransport);

    pTdiHandle->hConnection   = NULL;
    pTdiHandle->hTransport    = NULL;

    pTdiHandle->pfoConnection = NULL;
    pTdiHandle->pfoTransport  = NULL;
}



/**********************************************************************
* 
*  TdiFuncs_OpenTransportAddress
*
*    This is the first step in creating a connection you need to
*    create a Transport Address
*
**********************************************************************/
NTSTATUS TdiFuncs_OpenTransportAddress(USHORT AddrType, PHANDLE pTdiHandle, PFILE_OBJECT *pFileObject)
{
    NTSTATUS NtStatus = STATUS_INSUFFICIENT_RESOURCES;
    UNICODE_STRING usTdiDriverNameString;
    OBJECT_ATTRIBUTES oaTdiDriverNameAttributes;
    IO_STATUS_BLOCK IoStatusBlock;
    char DataBlob[sizeof(FILE_FULL_EA_INFORMATION) + TDI_TRANSPORT_ADDRESS_LENGTH + 300] = {0};
    PFILE_FULL_EA_INFORMATION pExtendedAttributesInformation = (PFILE_FULL_EA_INFORMATION)&DataBlob;
    UINT dwEASize = 0;
    PTRANSPORT_ADDRESS pTransportAddress = NULL;
    PTDI_ADDRESS_IP pTdiAddressIp = NULL;
    PTDI_ADDRESS_IP6_XP pTdiAddressIp6 = NULL;

    USHORT       TaLength = ( AddrType == TDI_ADDRESS_TYPE_IP6 ) ? 
                            sizeof ( TDI_ADDRESS_IP6_XP ) : 
                            sizeof ( TDI_ADDRESS_IP );

    /*
    * Initialize the name of the device to be opened.  ZwCreateFile takes an
    * OBJECT_ATTRIBUTES structure as the name of the device to open.  This is then
    * a two step process.
    *
    *  1 - Create a UNICODE_STRING data structure from a unicode string.
    *  2 - Create a OBJECT_ATTRIBUTES data structure from a UNICODE_STRING.
    *
    */

    if ( AddrType == TDI_ADDRESS_TYPE_IP6 )    
        RtlInitUnicodeString(&usTdiDriverNameString, L"\\Device\\Tcp6");    
    else    
        RtlInitUnicodeString(&usTdiDriverNameString, L"\\Device\\Tcp");    
    
    InitializeObjectAttributes(&oaTdiDriverNameAttributes, &usTdiDriverNameString, OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE, NULL, NULL);

    /*
    * The second step is to initialize the Extended Attributes data structure.
    *
    *  EaName        =  TdiTransportAddress, 0, TRANSPORT_ADDRESS
    *  EaNameLength  = Length of TdiTransportAddress
    *  EaValueLength = Length of TRANSPORT_ADDRESS
    */
    RtlCopyMemory(&pExtendedAttributesInformation->EaName, TdiTransportAddress, TDI_TRANSPORT_ADDRESS_LENGTH);

    pExtendedAttributesInformation->EaNameLength = TDI_TRANSPORT_ADDRESS_LENGTH;
    pExtendedAttributesInformation->EaValueLength = 
                    TDI_TRANSPORT_ADDRESS_LENGTH + 
                    sizeof(TRANSPORT_ADDRESS) + 
                    TaLength;

    pTransportAddress =  (PTRANSPORT_ADDRESS)(&pExtendedAttributesInformation->EaName + TDI_TRANSPORT_ADDRESS_LENGTH + 1);

    /*
    * The number of transport addresses
    */
    pTransportAddress->TAAddressCount = 1;

    /*
    *  This next piece will essentially describe what the transport being opened is.
    *
    *     AddressType   =  Type of transport
    *     AddressLength =  Length of the address
    *     Address       =  A data structure that is essentially related to the chosen AddressType.
    *
    */

    pTransportAddress->Address[0].AddressType    = AddrType; // TDI_ADDRESS_TYPE_IP;
    pTransportAddress->Address[0].AddressLength  = TaLength;

    if ( AddrType == TDI_ADDRESS_TYPE_IP6 )
    {        
        pTdiAddressIp6 = (TDI_ADDRESS_IP6_XP *)&pTransportAddress->Address[0].Address;

        RtlZeroMemory(pTdiAddressIp6, sizeof(TDI_ADDRESS_IP6_XP));
    }
    else
    {        
        pTdiAddressIp = (TDI_ADDRESS_IP *)&pTransportAddress->Address[0].Address;

        RtlZeroMemory(pTdiAddressIp, sizeof(TDI_ADDRESS_IP));
    }

    /*
    * The TDI_ADDRESS_IP data structure is essentially simmilar to the usermode sockets data structure. 
    *
    *    sin_port
    *    sin_zero
    *    in_addr
    *
    *    NOTE: This is the _LOCAL ADDRESS OF THE CURRENT MACHINE_ Just as with sockets, if you don't
    *          care what port you bind this connection to then just use "0".  If you also only have
    *          one network card interface, there's no reason to set the IP.  "0.0.0.0" will simply
    *          use the current machine's IP.  If you have multiple NIC's or a reason to specify
    *          the local IP address then you must set TDI_ADDRESS_IP to that IP.  If you are creating
    *          a server side component you may want to specify the port, however usually to connect
    *          to another server you really don't care what port the client is opening.
    */

    dwEASize = sizeof(DataBlob);

    NtStatus = ZwCreateFile(pTdiHandle, FILE_READ_EA | FILE_WRITE_EA, &oaTdiDriverNameAttributes, &IoStatusBlock, NULL, FILE_ATTRIBUTE_NORMAL, 0, FILE_OPEN_IF, 0, pExtendedAttributesInformation, dwEASize);

    if(NT_SUCCESS(NtStatus))
    {
        NtStatus = ObReferenceObjectByHandle(*pTdiHandle, GENERIC_READ | GENERIC_WRITE, NULL, KernelMode, (PVOID *)pFileObject, NULL);      

        if(!NT_SUCCESS(NtStatus))
        {
            ZwClose(*pTdiHandle);
        }
    }

    return NtStatus;
}




/**********************************************************************
* 
*  TdiFuncs_OpenConnection
*
*    This is the second step in creating a connection you need to
*    create a Connection Context
*
**********************************************************************/
NTSTATUS TdiFuncs_OpenConnection(USHORT AddrType, PHANDLE pTdiHandle, PFILE_OBJECT *pFileObject)
{
    NTSTATUS NtStatus = STATUS_INSUFFICIENT_RESOURCES;
    UNICODE_STRING usTdiDriverNameString;
    OBJECT_ATTRIBUTES oaTdiDriverNameAttributes;
    IO_STATUS_BLOCK IoStatusBlock;
    char DataBlob[sizeof(FILE_FULL_EA_INFORMATION) + TDI_CONNECTION_CONTEXT_LENGTH + 300] = {0};
    PFILE_FULL_EA_INFORMATION pExtendedAttributesInformation = (PFILE_FULL_EA_INFORMATION)&DataBlob;
    UINT dwEASize = 0;

    /*
    * Initialize the name of the device to be opened.  ZwCreateFile takes an
    * OBJECT_ATTRIBUTES structure as the name of the device to open.  This is then
    * a two step process.
    *
    *  1 - Create a UNICODE_STRING data structure from a unicode string.
    *  2 - Create a OBJECT_ATTRIBUTES data structure from a UNICODE_STRING.
    *
    */


    if ( AddrType == TDI_ADDRESS_TYPE_IP6 )    
        RtlInitUnicodeString(&usTdiDriverNameString, L"\\Device\\Tcp6");    
    else    
        RtlInitUnicodeString(&usTdiDriverNameString, L"\\Device\\Tcp");    

    InitializeObjectAttributes(&oaTdiDriverNameAttributes, &usTdiDriverNameString, OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE, NULL, NULL);

    /*
    * The second step is to initialize the Extended Attributes data structure.
    *
    *  EaName        =  TdiConnectionContext, 0, Your User Defined Context Data (Actually, a pointer to it I believe)
    *  EaNameLength  = Length of TdiConnectionContext
    *  EaValueLength = Length (Total Length I think?)
    */
    RtlCopyMemory(&pExtendedAttributesInformation->EaName, TdiConnectionContext, TDI_CONNECTION_CONTEXT_LENGTH);

    pExtendedAttributesInformation->EaNameLength = TDI_CONNECTION_CONTEXT_LENGTH;
    pExtendedAttributesInformation->EaValueLength = sizeof(PVOID); /* Must be at least TDI_CONNECTION_CONTEXT_LENGTH */

    dwEASize = sizeof(DataBlob);

    NtStatus = ZwCreateFile(pTdiHandle, 
                FILE_WRITE_ATTRIBUTES | FILE_WRITE_EA | FILE_READ_EA, 
                &oaTdiDriverNameAttributes, 
                &IoStatusBlock, NULL, 0, 0, 
                FILE_CREATE, 
                0, 
                pExtendedAttributesInformation, 
                dwEASize);

    /*
    NtStatus = IoCreateFile( pTdiHandle, 
                FILE_WRITE_ATTRIBUTES | FILE_WRITE_EA | FILE_READ_EA, 
                &oaTdiDriverNameAttributes, 
                &IoStatusBlock, NULL, 0, 0, 
                FILE_CREATE, 
                0, 
                pExtendedAttributesInformation, 
                dwEASize, 
                CreateFileTypeNone, 
                NULL, 
                IO_NO_PARAMETER_CHECKING );
                */

    if(NT_SUCCESS(NtStatus))
    {
        NtStatus = ObReferenceObjectByHandle(*pTdiHandle, GENERIC_READ | GENERIC_WRITE, NULL, KernelMode, (PVOID *)pFileObject, NULL);      

        if(!NT_SUCCESS(NtStatus))
        {
            ZwClose(*pTdiHandle);
        }
    }

    return NtStatus;
}


/**********************************************************************
* 
*  TdiFuncs_AssociateTransportAndConnection
*
*    This is called to associate the transport with the connection
*
**********************************************************************/

NTSTATUS TdiFuncs_AssociateTransportAndConnection(HANDLE hTransportAddress, PFILE_OBJECT pfoConnection)
{
    NTSTATUS NtStatus = STATUS_INSUFFICIENT_RESOURCES;
    PIRP pIrp;
    IO_STATUS_BLOCK IoStatusBlock = {0};
    PDEVICE_OBJECT pTdiDevice;
    TDI_COMPLETION_CONTEXT TdiCompletionContext;

    KeInitializeEvent(&TdiCompletionContext.kCompleteEvent, NotificationEvent, FALSE);

    /*
    * The TDI Device Object is required to send these requests to the TDI Driver.
    * 
    */

    pTdiDevice = IoGetRelatedDeviceObject(pfoConnection);

    /*
    * Step 1: Build the IRP.  TDI defines several macros and functions that can quickly
    *         create IRP's, etc. for variuos purposes.  While this can be done manually
    *         it's easiest to use the macros.
    *
    *  http://msdn.microsoft.com/library/en-us/network/hh/network/34bldmac_f430860a-9ae2-4379-bffc-6b0a81092e7c.xml.asp?frame=true
    */
    pIrp = TdiBuildInternalDeviceControlIrp(TDI_ASSOCIATE_ADDRESS, pTdiDevice, pfoConnection, &TdiCompletionContext.kCompleteEvent, &IoStatusBlock);

    if(pIrp)
    {
        /*
        * Step 2: Add the correct parameters into the IRP.
        */
        TdiBuildAssociateAddress(pIrp, pTdiDevice, pfoConnection, NULL, NULL, hTransportAddress);

        NtStatus = IoCallDriver(pTdiDevice, pIrp);

        /*
        *  If the status returned is STATUS_PENDING this means that the IRP will not be completed synchronously
        *  and the driver has queued the IRP for later processing.  This is fine but we do not want to return this
        *  thread, we are a synchronous call so we want to wait until it has completed.  The EVENT that we provided
        *  will be set when the IRP completes.
        */

        if(NtStatus == STATUS_PENDING)
        {
            KeWaitForSingleObject(&TdiCompletionContext.kCompleteEvent, Executive, KernelMode, FALSE, NULL);
            /*
            * Find the Status of the completed IRP
            */

            NtStatus = IoStatusBlock.Status;
        }

    }

    return NtStatus;
}


/**********************************************************************
* 
*  TdiFuncs_SetEventHandler
*
*    This is called to set an Event Handler Callback
*
**********************************************************************/
NTSTATUS TdiFuncs_SetEventHandler(PFILE_OBJECT pfoTdiFileObject, LONG InEventType, PVOID InEventHandler, PVOID InEventContext)
{
    NTSTATUS NtStatus = STATUS_INSUFFICIENT_RESOURCES;
    PIRP pIrp;
    IO_STATUS_BLOCK IoStatusBlock = {0};
    PDEVICE_OBJECT pTdiDevice;
    LARGE_INTEGER TimeOut = {0};
    UINT NumberOfSeconds = 60*3;
    TDI_COMPLETION_CONTEXT TdiCompletionContext;

    KeInitializeEvent(&TdiCompletionContext.kCompleteEvent, NotificationEvent, FALSE);

    /*
    * The TDI Device Object is required to send these requests to the TDI Driver.
    * 
    */

    pTdiDevice = IoGetRelatedDeviceObject(pfoTdiFileObject);

    /*
    * Step 1: Build the IRP.  TDI defines several macros and functions that can quickly
    *         create IRP's, etc. for variuos purposes.  While this can be done manually
    *         it's easiest to use the macros.
    *
    *  http://msdn.microsoft.com/library/en-us/network/hh/network/34bldmac_f430860a-9ae2-4379-bffc-6b0a81092e7c.xml.asp?frame=true
    */
    pIrp = TdiBuildInternalDeviceControlIrp(TDI_SET_EVENT_HANDLER, pTdiDevice, pfoConnection, &TdiCompletionContext.kCompleteEvent, &IoStatusBlock);

    if(pIrp)
    {
        /*
        * Step 2: Set the IRP Parameters
        */

        TdiBuildSetEventHandler(pIrp, pTdiDevice, pfoTdiFileObject, NULL, NULL, InEventType, InEventHandler, InEventContext);

        NtStatus = IoCallDriver(pTdiDevice, pIrp);

        /*
        *  If the status returned is STATUS_PENDING this means that the IRP will not be completed synchronously
        *  and the driver has queued the IRP for later processing.  This is fine but we do not want to return this
        *  thread, we are a synchronous call so we want to wait until it has completed.  The EVENT that we provided
        *  will be set when the IRP completes.
        */

        if(NtStatus == STATUS_PENDING)
        {
            KeWaitForSingleObject(&TdiCompletionContext.kCompleteEvent, Executive, KernelMode, FALSE, NULL);

            /*
            * Find the Status of the completed IRP
            */

            NtStatus = IoStatusBlock.Status;
        }

    }

    return NtStatus;
}

/**********************************************************************
* 
*  TdiFuncs_Connect
*
*    This is called to make a connection with a remote computer
*
**********************************************************************/

NTSTATUS TdiFuncs_Connect(PFILE_OBJECT pfoConnection, _ip_addr_t* rAddress, USHORT rPort)
{
    NTSTATUS NtStatus = STATUS_INSUFFICIENT_RESOURCES;
    PIRP pIrp;
    IO_STATUS_BLOCK IoStatusBlock = {0};
    PDEVICE_OBJECT pTdiDevice;
    TDI_CONNECTION_INFORMATION  RequestConnectionInfo = {0};
    TDI_CONNECTION_INFORMATION  ReturnConnectionInfo  = {0};
    LARGE_INTEGER TimeOut = {0};
    UINT NumberOfSeconds = 60*3;
    char cBuffer[256] = {0};
    PTRANSPORT_ADDRESS pTransportAddress = (PTRANSPORT_ADDRESS)&cBuffer;
    PTDI_ADDRESS_IP pTdiAddressIp;
    PTDI_ADDRESS_IP6_XP pTdiAddressIp6;
    TDI_COMPLETION_CONTEXT TdiCompletionContext;

    USHORT       TaLength = ( rAddress->IpVer == 6 ) ? 
                            sizeof ( TDI_ADDRESS_IP6_XP ) : 
                            sizeof ( TDI_ADDRESS_IP );

    KeInitializeEvent(&TdiCompletionContext.kCompleteEvent, NotificationEvent, FALSE);

    /*
    * The TDI Device Object is required to send these requests to the TDI Driver.
    * 
    */

    pTdiDevice = IoGetRelatedDeviceObject(pfoConnection);

    /*
    * Step 1: Build the IRP.  TDI defines several macros and functions that can quickly
    *         create IRP's, etc. for variuos purposes.  While this can be done manually
    *         it's easiest to use the macros.
    *
    *  http://msdn.microsoft.com/library/en-us/network/hh/network/34bldmac_f430860a-9ae2-4379-bffc-6b0a81092e7c.xml.asp?frame=true
    */
    pIrp = TdiBuildInternalDeviceControlIrp(TDI_CONNECT, pTdiDevice, pfoConnection, &TdiCompletionContext.kCompleteEvent, &IoStatusBlock);

    if(pIrp)
    {
        /*
        * Step 2: Add the correct parameters into the IRP.
        */

        /*
        *  Time out value
        */

        TimeOut.QuadPart = 10000000L;
        TimeOut.QuadPart *= NumberOfSeconds;
        TimeOut.QuadPart = -(TimeOut.QuadPart);

        /*
        * Initialize the RequestConnectionInfo which specifies the address of the REMOTE computer
        *
        */

        RequestConnectionInfo.RemoteAddress       = (PVOID)pTransportAddress;
        RequestConnectionInfo.RemoteAddressLength = sizeof(PTRANSPORT_ADDRESS) + TaLength; 

        /*
        * The number of transport addresses
        */
        pTransportAddress->TAAddressCount = 1;

        /*
        *  This next piece will essentially describe what the transport being opened is.
        *
        *     AddressType   =  Type of transport
        *     AddressLength =  Length of the address
        *     Address       =  A data structure that is essentially related to the chosen AddressType.
        *
        */

        pTransportAddress->Address[0].AddressType    = ( rAddress->IpVer == 4 )  ? 
            TDI_ADDRESS_TYPE_IP     : TDI_ADDRESS_TYPE_IP6;    
        pTransportAddress->Address[0].AddressLength  = TaLength;

        pTdiAddressIp = (TDI_ADDRESS_IP *)&pTransportAddress->Address[0].Address;
        pTdiAddressIp6 = (TDI_ADDRESS_IP6_XP *)&pTransportAddress->Address[0].Address;

        /*
        * The TDI_ADDRESS_IP data structure is essentially simmilar to the usermode sockets data structure. 
        *
        *    sin_port
        *    sin_zero
        *    in_addr
        */

        /*
        *  Remember, these must be in NETWORK BYTE ORDER (Big Endian)
        */

        if ( rAddress->IpVer == 4 )
        {
            pTdiAddressIp->sin_port = rPort;       /* Example: 1494 = 0x05D6 (Little Endian) or 0xD605 (Big Endian)                  */
            pTdiAddressIp->in_addr  = rAddress->Ipv4;    /* Example: 10.60.2.159 = 0A.3C.02.9F (Little Endian) or 9F.02.3C.0A (Big Endian) */
        }
        else
        {
            pTdiAddressIp6->sin6_port = rPort;
            RtlCopyMemory( pTdiAddressIp6->sin6_addr, rAddress->Ipv6_uchar, sizeof ( UCHAR ) * 16 );
        }

        TdiBuildConnect(pIrp, pTdiDevice, pfoConnection, NULL, NULL, &TimeOut, &RequestConnectionInfo, &ReturnConnectionInfo);

        NtStatus = IoCallDriver(pTdiDevice, pIrp);

        /*
        *  If the status returned is STATUS_PENDING this means that the IRP will not be completed synchronously
        *  and the driver has queued the IRP for later processing.  This is fine but we do not want to return this
        *  thread, we are a synchronous call so we want to wait until it has completed.  The EVENT that we provided
        *  will be set when the IRP completes.
        */

        if(NtStatus == STATUS_PENDING)
        {
            KeWaitForSingleObject(&TdiCompletionContext.kCompleteEvent, Executive, KernelMode, FALSE, NULL);

            /*
            * Find the Status of the completed IRP
            */

            NtStatus = IoStatusBlock.Status;
        }

    }

    return NtStatus;
}



/**********************************************************************
* 
*  TdiFuncs_Send
*
*    This API demonstrates how to send data
*
**********************************************************************/
NTSTATUS TdiFuncs_Send(PFILE_OBJECT pfoConnection, PVOID pData, UINT uiSendLength, UINT *pDataSent)
{
    NTSTATUS NtStatus = STATUS_INSUFFICIENT_RESOURCES;
    PIRP pIrp;
    IO_STATUS_BLOCK IoStatusBlock = {0};
    PDEVICE_OBJECT pTdiDevice;
    PMDL pSendMdl;
    TDI_COMPLETION_CONTEXT TdiCompletionContext;

    KeInitializeEvent(&TdiCompletionContext.kCompleteEvent, NotificationEvent, FALSE);

    /*
    * The TDI Device Object is required to send these requests to the TDI Driver.
    * 
    */

    pTdiDevice = IoGetRelatedDeviceObject(pfoConnection);

    *pDataSent = 0;

    /*
    *  The send requires an MDL which is what you may remember from DIRECT_IO.  However,
    *  instead of using an MDL we need to create one.
    */
    pSendMdl = IoAllocateMdl((PCHAR )pData, uiSendLength, FALSE, FALSE, NULL);

    if(pSendMdl)
    {

        __try {

            MmProbeAndLockPages(pSendMdl, KernelMode, IoModifyAccess);

        } __except (EXCEPTION_EXECUTE_HANDLER) {
            IoFreeMdl(pSendMdl);
            pSendMdl = NULL;
        };

        if(pSendMdl)
        {

            /*
            * Step 1: Build the IRP.  TDI defines several macros and functions that can quickly
            *         create IRP's, etc. for variuos purposes.  While this can be done manually
            *         it's easiest to use the macros.
            *
            *  http://msdn.microsoft.com/library/en-us/network/hh/network/34bldmac_f430860a-9ae2-4379-bffc-6b0a81092e7c.xml.asp?frame=true
            */
            pIrp = TdiBuildInternalDeviceControlIrp(TDI_SEND, pTdiDevice, pfoConnection, &TdiCompletionContext.kCompleteEvent, &IoStatusBlock);

            if(pIrp)
            {
                /*
                * Step 2: Add the correct parameters into the IRP.
                */


                TdiBuildSend(pIrp, pTdiDevice, pfoConnection, NULL, NULL, pSendMdl, 0, uiSendLength);

                NtStatus = IoCallDriver(pTdiDevice, pIrp);

                /*
                *  If the status returned is STATUS_PENDING this means that the IRP will not be completed synchronously
                *  and the driver has queued the IRP for later processing.  This is fine but we do not want to return this
                *  thread, we are a synchronous call so we want to wait until it has completed.  The EVENT that we provided
                *  will be set when the IRP completes.
                */

                if(NtStatus == STATUS_PENDING)
                {
                    KeWaitForSingleObject(&TdiCompletionContext.kCompleteEvent, Executive, KernelMode, FALSE, NULL);

                }

                NtStatus   = IoStatusBlock.Status;
                *pDataSent = (UINT)IoStatusBlock.Information;

                /* 
                * I/O Manager will free the MDL
                *
                if(pSendMdl)
                {
                MmUnlockPages(pSendMdl);
                IoFreeMdl(pSendMdl);
                } */

            }
        }
    }

    return NtStatus;
}



/**********************************************************************
* 
*  TdiFuncs_Disconnect
*
*    This is called to disconnect from the remote computer
*
**********************************************************************/

NTSTATUS TdiFuncs_Disconnect(PFILE_OBJECT pfoConnection)
{
    NTSTATUS NtStatus = STATUS_INSUFFICIENT_RESOURCES;
    PIRP pIrp;
    IO_STATUS_BLOCK IoStatusBlock = {0};
    PDEVICE_OBJECT pTdiDevice;
    TDI_CONNECTION_INFORMATION  ReturnConnectionInfo  = {0};
    LARGE_INTEGER TimeOut = {0};
    UINT NumberOfSeconds = 60*3;
    TDI_COMPLETION_CONTEXT TdiCompletionContext;

    KeInitializeEvent(&TdiCompletionContext.kCompleteEvent, NotificationEvent, FALSE);

    /*
    * The TDI Device Object is required to send these requests to the TDI Driver.
    * 
    */

    pTdiDevice = IoGetRelatedDeviceObject(pfoConnection);

    /*
    * Step 1: Build the IRP.  TDI defines several macros and functions that can quickly
    *         create IRP's, etc. for variuos purposes.  While this can be done manually
    *         it's easiest to use the macros.
    *
    *  http://msdn.microsoft.com/library/en-us/network/hh/network/34bldmac_f430860a-9ae2-4379-bffc-6b0a81092e7c.xml.asp?frame=true
    */
    pIrp = TdiBuildInternalDeviceControlIrp(TDI_DISCONNECT, pTdiDevice, pfoConnection, &TdiCompletionContext.kCompleteEvent, &IoStatusBlock);

    if(pIrp)
    {
        /*
        * Step 2: Add the correct parameters into the IRP.
        */

        /*
        *  Time out value
        */

        TimeOut.QuadPart = 10000000L;
        TimeOut.QuadPart *= NumberOfSeconds;
        TimeOut.QuadPart = -(TimeOut.QuadPart);


        TdiBuildDisconnect(pIrp, pTdiDevice, pfoConnection, NULL, NULL, &TimeOut, TDI_DISCONNECT_ABORT, NULL, &ReturnConnectionInfo);

        NtStatus = IoCallDriver(pTdiDevice, pIrp);

        /*
        *  If the status returned is STATUS_PENDING this means that the IRP will not be completed synchronously
        *  and the driver has queued the IRP for later processing.  This is fine but we do not want to return this
        *  thread, we are a synchronous call so we want to wait until it has completed.  The EVENT that we provided
        *  will be set when the IRP completes.
        */

        if(NtStatus == STATUS_PENDING)
        {
            KeWaitForSingleObject(&TdiCompletionContext.kCompleteEvent, Executive, KernelMode, FALSE, NULL);

            /*
            * Find the Status of the completed IRP
            */

            NtStatus = IoStatusBlock.Status;
        }


    }

    return NtStatus;
}



/**********************************************************************
* 
*  TdiFuncs_DisAssociateTransportAndConnection
*
*    This is called to disassociate the transport with the connection
*
**********************************************************************/

NTSTATUS TdiFuncs_DisAssociateTransportAndConnection(PFILE_OBJECT pfoConnection)
{
    NTSTATUS NtStatus = STATUS_INSUFFICIENT_RESOURCES;
    PIRP pIrp;
    IO_STATUS_BLOCK IoStatusBlock = {0};
    PDEVICE_OBJECT pTdiDevice;
    TDI_COMPLETION_CONTEXT TdiCompletionContext;

    KeInitializeEvent(&TdiCompletionContext.kCompleteEvent, NotificationEvent, FALSE);


    /*
    * The TDI Device Object is required to send these requests to the TDI Driver.
    * 
    */

    pTdiDevice = IoGetRelatedDeviceObject(pfoConnection);

    /*
    * Step 1: Build the IRP.  TDI defines several macros and functions that can quickly
    *         create IRP's, etc. for variuos purposes.  While this can be done manually
    *         it's easiest to use the macros.
    *
    *  http://msdn.microsoft.com/library/en-us/network/hh/network/34bldmac_f430860a-9ae2-4379-bffc-6b0a81092e7c.xml.asp?frame=true
    */
    pIrp = TdiBuildInternalDeviceControlIrp(TDI_DISASSOCIATE_ADDRESS, pTdiDevice, pfoConnection, &TdiCompletionContext.kCompleteEvent, &IoStatusBlock);

    if(pIrp)
    {
        /*
        * Step 2: Add the correct parameters into the IRP.
        */
        TdiBuildDisassociateAddress(pIrp, pTdiDevice, pfoConnection, NULL, NULL);

        NtStatus = IoCallDriver(pTdiDevice, pIrp);

        /*
        *  If the status returned is STATUS_PENDING this means that the IRP will not be completed synchronously
        *  and the driver has queued the IRP for later processing.  This is fine but we do not want to return this
        *  thread, we are a synchronous call so we want to wait until it has completed.  The EVENT that we provided
        *  will be set when the IRP completes.
        */

        if(NtStatus == STATUS_PENDING)
        {
            KeWaitForSingleObject(&TdiCompletionContext.kCompleteEvent, Executive, KernelMode, FALSE, NULL);

            /*
            * Find the Status of the completed IRP
            */

            NtStatus = IoStatusBlock.Status;
        }

    }

    return NtStatus;
}



/**********************************************************************
* 
*  TdiFuncs_CloseTdiOpenHandle
*
*    This is called to close open handles.
*
**********************************************************************/

NTSTATUS TdiFuncs_CloseTdiOpenHandle(HANDLE hTdiHandle, PFILE_OBJECT pfoTdiFileObject)
{
    NTSTATUS NtStatus = STATUS_SUCCESS;

    /*
    * De-Reference the FILE_OBJECT and Close The Handle
    */

    ObDereferenceObject(pfoTdiFileObject);
    ZwClose(hTdiHandle);

    return NtStatus;
}

/**********************************************************************
* 
*  TdiFuncs_CompleteIrp
*
*    This is called when the IRP is completed
*
**********************************************************************
NTSTATUS TdiFuncs_CompleteIrp(PDEVICE_OBJECT DeviceObject, PIRP Irp, PVOID Context)
{
PTDI_COMPLETION_CONTEXT pTdiCompletionContext = (PTDI_COMPLETION_CONTEXT)Context;

Do not mark the IRP as pending, we created it we may overwrite memory locations
we don't want to !


return STATUS_CONTINUE_COMPLETION ;
}
*/



/**********************************************************************
* 
*  TdiExample_ClientEventReceive
*
*    This function is called when data is recieved.
*
**********************************************************************/
NTSTATUS TdiExample_ClientEventReceive(
                    PVOID TdiEventContext, 
                    CONNECTION_CONTEXT ConnectionContext, 
                    ULONG ReceiveFlags, 
                    ULONG  BytesIndicated, 
                    ULONG  BytesAvailable, 
                    ULONG  *BytesTaken, 
                    PVOID  Tsdu, 
                    PIRP  *IoRequestPacket)
{
    NTSTATUS NtStatus = STATUS_SUCCESS;
    UINT uiDataRead = 0;
    TDI_HANDLE* pTdiHandle = (TDI_HANDLE*)TdiEventContext;
    PIRP pIrp;

    DbgPrint("TdiExample_ClientEventReceive 0x%0x, %i, %i\n", ReceiveFlags, BytesIndicated, BytesAvailable);


    *BytesTaken = BytesAvailable;
    
    *IoRequestPacket = NULL;

    return NtStatus;
}