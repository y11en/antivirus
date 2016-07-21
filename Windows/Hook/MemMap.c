/*void FreeMappedMemory(PMEMORY_MAPPING pMemStruct)
{
	DbPrint(DC_NOCAT,DL_NOTIFY, ("Free mapped memory\n"));
#ifdef _HOOK_VXD_	// 9x code
#else				// nt code
	if (pMemStruct->m_MempMdl != NULL)
	{
		if (pMemStruct->m_MemUserAddr != NULL )
			MmUnmapLockedPages(pMemStruct->m_MemUserAddr, pMemStruct->m_MempMdl);
		IoFreeMdl(pMemStruct->m_MempMdl);
		pMemStruct->m_MempMdl = NULL;
	}
	
	if (pMemStruct->m_MemBaseAdress != NULL)
		ExFreePool(pMemStruct->m_MemBaseAdress);
#endif
	pMemStruct->m_MemBaseAdress = NULL;
	
	pMemStruct->m_MemSize = 0;
	pMemStruct->m_MemStartFreeAddr = NULL;
	pMemStruct->m_StartBisyAddr = NULL;
}

BOOLEAN AllocateMappedMemory(PMEMORY_MAPPING pMemStruct, PVOID BaseAdressIn)
{
	PVOID pResult = NULL;
#ifdef _HOOK_VXD_	// 9x code
	DbgBreakPoint();
#else				// nt code
	DbPrint(DC_NOCAT,DL_NOTIFY, ("Allocating memory for mapping\n"));
	
	pMemStruct->m_MemBaseAdress = ExAllocatePoolWithTag(NonPagedPool, pMemStruct->m_MemSize, '-boS');
	if (pMemStruct->m_MemBaseAdress == NULL)
	{
		DbPrint(DC_NOCAT,DL_ERROR, ("Cannot allocate memory for transfer to r3\n"));
		pMemStruct->m_MemSize = 0;
		return FALSE;
	}

	pMemStruct->m_MempMdl = IoAllocateMdl(pMemStruct->m_MemBaseAdress, pMemStruct->m_MemSize, FALSE, FALSE, NULL);
	
	if (pMemStruct->m_MempMdl == NULL)
	{
		DbPrint(DC_NOCAT,DL_ERROR, ("Could not allocate mdl\n"));
		ExFreePool(pMemStruct->m_MemBaseAdress);
		pMemStruct->m_MemBaseAdress = NULL;
		pMemStruct->m_MempMdl = NULL;
		pMemStruct->m_MemSize = 0;
		return FALSE;
	}
	
	//!! ? это надо??
	//pMemStruct->m_MempMdl->MdlFlags |= MDL_MAPPING_CAN_FAIL;
	//
	MmBuildMdlForNonPagedPool(pMemStruct->m_MempMdl); 
	
	use MmMapLockedPagesSpecifyCache instead for hct 12.0
	pMemStruct->m_MemUserAddr = (PVOID)(((ULONG)MmMapLockedPages(pMemStruct->m_MempMdl, UserMode)) | MmGetMdlByteOffset(pMemStruct->m_MempMdl));
#endif

#ifdef __DBG__
	memset(pMemStruct->m_MemBaseAdress, 0, pMemStruct->m_MemSize);
#endif
	
	pMemStruct->m_MemStartFreeAddr = pMemStruct->m_MemBaseAdress;
	pMemStruct->m_StartBisyAddr = NULL;

	return TRUE;
}
*/