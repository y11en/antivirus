#include "../osspec.h"
#include "cd9x.h"

PFNISP ILB_service_rtn = NULL;          // addr of service routine; see below
PVOID  ILB_internal_request = NULL;     // addr of request routine; see below
PFNIOP ILB_int_io_criteria_rtn = NULL;  // addr of IOP criteria routine

VOID __stdcall memset_int(void *dest, char value, int count)
{
	__asm
	{
		SaveStdRegister
		mov		al,value
		mov		edi,dest
		mov		ecx,count
		rep		stosb
		RestoreStdRegister
	}
}

VOID __stdcall memcpy_int(void *dest, void *src, int count)
{
	__asm
	{
		SaveStdRegister
		mov		esi,src
		mov		edi,dest
		mov		ecx,count
		rep		movsb
		RestoreStdRegister
	}
}

BOOLEAN __stdcall Win9x_IOS_Get_Version(VOID)
{
	__asm
	{
		SaveStdRegister
		VxDCall(IOS_Get_Version)
		jnc		exit
		xor		eax,eax
	exit:
		RestoreStdRegister
	}
}

VOID __stdcall Win9x_IOS_Register(PDRP drp)
{
	__asm
	{
		SaveStdRegister
		push	dword ptr [drp]
		VxDCall(IOS_Register)
		add		esp,4
		RestoreStdRegister
	}
}

ULONG __stdcall Win9x_PageReserve(ULONG page, ULONG npages, ULONG flags)
{
	__asm
	{
		SaveStdRegister
		push	flags
		push	npages
		push	page
		VMMCall(_PageReserve)
		add		esp,12
		RestoreStdRegister
	}
}

ULONG __stdcall Win9x_PageCommitContig(
	ULONG page,
	ULONG npages,
	ULONG flags,
    ULONG alignmask,
	ULONG minphys,
	ULONG maxphys)
{
	__asm
	{
		SaveStdRegister
		push	maxphys
		push	minphys
		push	alignmask
		push	flags
		push	npages
		push	page
		VMMCall(_PageCommitContig)
		add		esp,6*4
		RestoreStdRegister
	}
}

ULONG __stdcall Win9x_PageFree(PVOID hMem, DWORD flags)
{
	__asm
	{
		SaveStdRegister
		push	flags
		push	hMem
		VMMCall(_PageFree)
		add		esp,8
		RestoreStdRegister
	}
}

NTSTATUS __stdcall InitializeDriver(VOID)
{
	if(!iosRegister)
	{
		iosRegister = 1;
		IOS_Register(&DRIVER_Drp);

		DbgBreakPoint();
	}

	ILB_service_rtn = DRIVER_Ilb.ILB_service_rtn;
	ILB_internal_request = DRIVER_Ilb.ILB_internal_request;
	ILB_int_io_criteria_rtn = DRIVER_Ilb.ILB_int_io_criteria_rtn;

	return STATUS_SUCCESS;
}

NTSTATUS __stdcall OpenCDROM(
	DWORD LetterIndex,
	DWORD MaximumDataTransferLength,
	HANDLE *Handle,
	PVOID *Buffer)
{
	ISP_dcb_get dcb_get;
	PDCB dcb;
	ISP_IOP_alloc iop_alloc;
	pIOP riop;
	PORT_SRB *port_srb;
	ULONG page_count;
	ULONG buffer;

	memset_int(&dcb_get,0,sizeof(ISP_dcb_get));
	memset_int(&iop_alloc,0,sizeof(ISP_IOP_alloc));

	// get DCB strcture
	dcb_get.ISP_g_d_hdr.ISP_func = ISP_GET_DCB;
	dcb_get.ISP_g_d_drive = LetterIndex;
	ILB_service_rtn( (PISP)(&dcb_get) );

	if (dcb_get.ISP_g_d_hdr.ISP_result != 0)
		return STATUS_UNSUCCESSFUL;

	dcb = (PDCB)(dcb_get.ISP_g_d_dcb);

	// check if DCB correct
	if (dcb->DCB_cmn.DCB_device_type != DCB_type_cdrom)
		return STATUS_UNSUCCESSFUL;

	if ((PDCB)(dcb->DCB_cmn.DCB_physical_dcb) != dcb)
		return STATUS_UNSUCCESSFUL;

	// allocate buffer
	page_count = (MaximumDataTransferLength + 0xFFF)>>12;
	buffer = Win9x_PageReserve(PR_SYSTEM,page_count,0);
	
	if (buffer == -1L)
		return STATUS_INSUFFICIENT_RESOURCES;

	if ( Win9x_PageCommitContig(buffer>>12,page_count,PC_WRITEABLE,0x00,0,-1) == -1 )
	{
		Win9x_PageFree((PVOID)buffer,0);
		return STATUS_INSUFFICIENT_RESOURCES;
	}
	
	memset_int((PVOID) buffer, 0, MaximumDataTransferLength);

	// allocate IOP
	iop_alloc.ISP_i_c_hdr.ISP_func = ISP_CREATE_IOP;
	iop_alloc.ISP_i_c_flags	= ISP_M_FL_MUST_SUCCEED;

	iop_alloc.ISP_delta_to_ior = dcb->DCB_cmn.DCB_expansion_length +
		 sizeof(IOP) - sizeof(IOR);

	iop_alloc.ISP_IOP_size = (USHORT) (dcb->DCB_cmn.DCB_expansion_length +
		sizeof(IOP) + 
		dcb->DCB_max_sg_elements*sizeof(SGD) +
		sizeof(PORT_SRB) +
		dcb->DCB_srb_ext_size +
		SENSE_LEN);

	ILB_service_rtn( (PISP)(&iop_alloc) );

	if (iop_alloc.ISP_i_c_hdr.ISP_result != 0)
		return STATUS_UNSUCCESSFUL;

	riop = (pIOP)(iop_alloc.ISP_IOP_ptr + dcb->DCB_cmn.DCB_expansion_length);

	// setup IOP
	riop->IOP_original_dcb = (DCB_COMMON*)dcb;
	riop->IOP_ior.IOR_next = 0;
	riop->IOP_ior.IOR_func = IOR_SCSI_PASS_THROUGH;
	riop->IOP_ior.IOR_status = IORS_CMD_IN_PROGRESS;
	riop->IOP_ior.IOR_flags = 0x4221ce09 | IORF_SYNC_COMMAND;
	riop->IOP_ior.IOR_req_req_handle = 0;
	riop->IOP_ior.IOR_req_vol_handle = 0;
	riop->IOP_ior.IOR_vol_designtr = dcb->DCB_cmn.DCB_unit_number;
	riop->IOP_ior.IOR_buffer_ptr = buffer;
	riop->IOP_ior.IOR_sgd_lin_phys = (ULONG)riop + sizeof(IOP);
	riop->IOP_ior.IOR_num_sgds = 0;
	riop->IOP_srb = (ULONG)riop + sizeof(IOP) + dcb->DCB_max_sg_elements*sizeof(SGD);
	riop->IOP_callback_ptr = 0;

	// setup SRB
	port_srb = (PORT_SRB *)(riop->IOP_srb);
	port_srb->BaseSrb.Length = sizeof(SCSI_REQUEST_BLOCK);
	port_srb->BaseSrb.Function = SRB_FUNCTION_EXECUTE_SCSI;
	port_srb->BaseSrb.PathId = dcb->DCB_bus_number;
	port_srb->BaseSrb.TargetId = dcb->DCB_scsi_target_id;
	port_srb->BaseSrb.Lun = dcb->DCB_scsi_lun;
	port_srb->BaseSrb.NextSrb = NULL;
	port_srb->BaseSrb.SrbFlags = SRB_FLAGS_DISABLE_DISCONNECT |
								 SRB_FLAGS_DISABLE_AUTOSENSE |
								 SRB_FLAGS_DATA_IN;
	port_srb->BaseSrb.SrbExtension = port_srb + sizeof(PORT_SRB);
	port_srb->BaseSrb.DataBuffer = (PVOID)buffer;
	port_srb->BaseSrb.TimeOutValue = TIMEOUT_VALUE;
	port_srb->BaseSrb.SenseInfoBuffer = (PVOID)((ULONG)port_srb->BaseSrb.SrbExtension + 
		dcb->DCB_srb_ext_size);
	port_srb->BaseSrb.SenseInfoBufferLength = SENSE_LEN;
	port_srb->SrbNextSrb = NULL; 
	port_srb->SrbIopPointer = riop;

	// if eax == 0 - success
	/*if ( ILB_int_io_criteria_rtn(riop) != 0)
	{
		Win9x_PageFree((PVOID)buffer,0);
		return STATUS_INSUFFICIENT_RESOURCES;
	}*/

	ILB_int_io_criteria_rtn(riop);

	// return pointer to IOP as handle
	*Handle = (HANDLE)riop;
	*Buffer = (PVOID)buffer;

	return STATUS_SUCCESS;
}

VOID __stdcall CloseCDROM(HANDLE Handle)
{
	pIOP iop;
	PDCB dcb;
	ISP_mem_dealloc mem_dealloc;

	iop = (pIOP)Handle;
	dcb = (PDCB)iop->IOP_original_dcb;

	Win9x_PageFree((PVOID)(iop->IOP_ior.IOR_buffer_ptr),0);

	mem_dealloc.ISP_mem_da_hdr.ISP_func = ISP_DEALLOC_MEM;
	mem_dealloc.ISP_mem_ptr_da = (ULONG)iop - dcb->DCB_cmn.DCB_expansion_length;
	ILB_service_rtn( (PISP)(&mem_dealloc) );
}

SRBSTATUS __stdcall SendCDB(
	HANDLE Handle,
	PVOID Cdb,
	DWORD CdbLength,
	DWORD DataTransferLength)
{
	IOP *iop;
	DCB *dcb;
	PORT_SRB *port_srb;

	iop = (IOP *)Handle;
	dcb = (DCB *)iop->IOP_original_dcb;
	port_srb = (PORT_SRB *)(iop->IOP_srb);

	// set data transfer length
	iop->IOP_ior.IOR_xfer_count = DataTransferLength;
	port_srb->BaseSrb.DataTransferLength = DataTransferLength;

	// setup CDB
	port_srb->BaseSrb.CdbLength = (UCHAR) CdbLength;
	memcpy_int(port_srb->BaseSrb.Cdb,Cdb,CdbLength);

	// fixup LUN
	port_srb->BaseSrb.Cdb[1] = (port_srb->BaseSrb.Cdb[1]&0x07) | (dcb->DCB_scsi_lun<<3);

	__asm
	{
		pushad
		mov		edi,iop
		mov		ebx,dcb
		xor		edx,edx
		call	[ILB_internal_request]
		popad
	}

	return port_srb->BaseSrb.SrbStatus;
}

BOOL try_cdr_read(BYTE Drive, DWORD Sector, DWORD NumSectors, DWORD BufferSize, BYTE* Buffer)
{
	BOOL bret = FALSE;
	BYTE *buf = 0;
	HANDLE handle;
	BYTE CDB[16] = {
		0x28,
		0x00,
		(BYTE)((Sector>>24)&0xFF),
		(BYTE)((Sector>>16)&0xFF),
		(BYTE)((Sector>>8)&0xFF),
		(BYTE)((Sector>>0)&0xFF),
		0x00,
		0x00,
		0x01,
		0x00,
		0x00,
		0x00,
		0x00,
		0x00,
		0x00,
		0x00
	};

	_cdb_* pcdb = (_cdb_*) CDB;

	if (BufferSize > NumSectors * SECTOR_SIZE)
	{
		DbPrint(DC_LLDISKIO, DL_WARNING, ("try_cdr_read BufferSize ???\n"));
		DbgBreakPoint();

		return FALSE;
	}

	if (STATUS_SUCCESS == InitializeDriver())
	{
		if (STATUS_SUCCESS == OpenCDROM(Drive, NumSectors * SECTOR_SIZE, &handle, &buf))
		{
			DbPrint(DC_LLDISKIO, DL_IMPORTANT, ("try_cdr_read BufferSize %d, NumSectors %d (max size %d)\n", BufferSize, NumSectors, NumSectors * SECTOR_SIZE));
			if (SendCDB(handle, CDB, 10, BufferSize) == SRB_STATUS_SUCCESS)	// 10 - must be!!! not sizeof(CDB)
			{
				memcpy(Buffer, buf, BufferSize);
				bret = TRUE;
			}
			else
			{
				//DbgBreakPoint();
			}

			CloseCDROM(handle);
		}
	}

	return bret;
}