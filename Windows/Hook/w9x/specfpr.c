#include "specfpr.h"

#include "../debug.h"
#include "../InvThread.h"
#include "../hook/HookSpec.h"

#define _specfn	"c:\\klif.spi"

PERESOURCE gpSPMutex = NULL;			// to protect client's list

typedef struct _tSpecFileBlock
{
	struct _tSpecFileBlock* m_pNext;
	SPECFILEFUNC_FILEBLOCK	m_Data;
}SpecFileBlock, *PSpecFileBlock;

SpecFileBlock* gpSpecFileBlock = NULL;
SpecFileBlock* gpSpecFileBlockLast = NULL;

ULONG CalcFNLen(PSPECFILEFUNC_FILEBLOCK pBlock)
{
	ULONG fnlen = strlen(pBlock->m_FileNames);
	
	if (fnlen)
	{
		if (_sfop_rename == pBlock->m_FileOp)
			fnlen += strlen(pBlock->m_FileNames + fnlen + 1);
	}
	
	if (fnlen)
	{
		fnlen++;		// add last 0
		if (_sfop_rename == pBlock->m_FileOp)
			fnlen++;		// add last 0
	}

	return fnlen;
}
//+ ------------------------------------------------------------------------------------------
SpecFile_FuncResult GetW95Error(WORD ioStatus)
{
	switch (ioStatus)
	{
	case 0x2:
	case 0x3:
		return _sfopr_file_not_found;
		break;
	case 0x5:
		return _sfopr_access_denied;
		break;
	}

	return _sfopr_notok;
}

//+ ------------------------------------------------------------------------------------------

VOID
SPImpProcessListImp(PULONG pContainerID)
{
	BOOLEAN bTryBlock;

	WORD    ioStatus;
	BYTE	ActionTaken = 0;

	SpecFileBlock* pBlock;

	AddInvisibleThread((ULONG) PsGetCurrentThreadId());

	pBlock = gpSpecFileBlock;

	while (pBlock)
	{
		bTryBlock = TRUE;
		
		if (pBlock->m_Data.m_Result == _sfopr_new_request)
			bTryBlock = FALSE;

		if (pBlock->m_Data.m_Result == _sfopr_ok)
			bTryBlock = FALSE;

		if (bTryBlock)
		{
			pBlock->m_Data.m_Result = _sfopr_ok;
			switch (pBlock->m_Data.m_FileOp)
			{
			case _sfop_delete:
				{
					R0_SetFileAttributes(pBlock->m_Data.m_FileNames, 0, &ioStatus);
					if (!R0_DeleteFile(pBlock->m_Data.m_FileNames, 0, &ioStatus))
					{
						pBlock->m_Data.m_Result = GetW95Error(ioStatus);
						DbgBreakPoint();
					}
				}
				break;
			case _sfop_rename:
				{
					char* pold = pBlock->m_Data.m_FileNames;
					char* pnew = pBlock->m_Data.m_FileNames + strlen(pBlock->m_Data.m_FileNames) + 1;

					R0_SetFileAttributes(pold, 0, &ioStatus);
					R0_SetFileAttributes(pnew, 0, &ioStatus);

					if (!R0_RenameFile(pold, pnew, &ioStatus))
					{
						HANDLE hSrc;
						pBlock->m_Data.m_Result = GetW95Error(ioStatus);

						hSrc = R0_OpenCreateFile(TRUE, pold, OPEN_ACCESS_READONLY | OPEN_SHARE_DENYWRITE, ATTR_NORMAL, 
							ACTION_IFEXISTS_OPEN, 0, &ioStatus, &ActionTaken);
						if(!ioStatus && hSrc)
						{
							HANDLE hDst = R0_OpenCreateFile(TRUE, pnew, OPEN_ACCESS_WRITEONLY | OPEN_SHARE_DENYREADWRITE, ATTR_NORMAL, 
								ACTION_IFEXISTS_TRUNCATE | ACTION_IFNOTEXISTS_CREATE, 0, &ioStatus, &ActionTaken);

							if(!ioStatus && hDst)
							{
								ULONG Offset = 0;
								char pbuf[512];
								DWORD bRead;

								while (1)
								{
									bRead = R0_ReadFile(TRUE, hSrc, pbuf, sizeof(pbuf), Offset, &ioStatus);
									if (ioStatus)
										break;
									
									if (!bRead)
										break;

									R0_WriteFile(TRUE, hDst, pbuf, bRead, Offset, &ioStatus);
									Offset += bRead;

								};

								R0_CloseFile(hDst, &ioStatus);

								pBlock->m_Data.m_Result = _sfopr_ok;
							}
							
							R0_CloseFile(hSrc, &ioStatus);
							R0_DeleteFile(pold, 0, &ioStatus);
						}
					}
				}
				break;
			default:
				{
					pBlock->m_Data.m_Result = _sfopr_file_not_found;
					DbgBreakPoint();
				}
				break;
			}
		}

		pBlock = pBlock->m_pNext;
	}

	DelInvisibleThread((ULONG) PsGetCurrentThreadId(), FALSE);
}

VOID SPImp()
{
	WORD    ioStatus;
	BYTE	ActionTaken = 0;

	AddInvisibleThread((ULONG) PsGetCurrentThreadId());

	{
		HANDLE hLog = R0_OpenCreateFile(TRUE, _specfn, OPEN_ACCESS_READONLY | OPEN_SHARE_DENYWRITE, ATTR_HIDDEN | ATTR_SYSTEM, 
			ACTION_IFEXISTS_OPEN, 0, &ioStatus, &ActionTaken);
		if(!ioStatus && hLog)
		{
			//! read and process
			DWORD bRead;
			ULONG fnlen;
			ULONG Offset = 0;

			while(TRUE)
			{
				bRead = R0_ReadFile(TRUE, hLog, &fnlen, sizeof(fnlen), Offset, &ioStatus);
				if (ioStatus || bRead != sizeof(fnlen))
					break;
				else
				{
					_xor_drv_func(&fnlen, sizeof(fnlen), 1);
					if (!fnlen)
					{
						// strange block size
						DbgBreakPoint();
						break;
					}
					else
					{
						SpecFileBlock* pBlock = ExAllocatePoolWithTag(PagedPool, sizeof(SpecFileBlock) + fnlen, 'sBOS');
						if (pBlock)
						{
							bRead = R0_ReadFile(TRUE, hLog, pBlock, sizeof(SpecFileBlock) + fnlen, Offset, &ioStatus);
							if (ioStatus || (bRead != sizeof(SpecFileBlock) + fnlen))
								ExFreePool(pBlock);
							else
							{
								_xor_drv_func(pBlock, sizeof(SpecFileBlock) + fnlen, 1);
								pBlock->m_pNext = 0;
								pBlock->m_Data.m_Result = _sfopr_notok;

								if (gpSpecFileBlock)
								{
									gpSpecFileBlockLast->m_pNext = pBlock;
									gpSpecFileBlockLast = pBlock;
								}
								else
								{
									gpSpecFileBlock = pBlock;
									gpSpecFileBlockLast = pBlock;
								}

							}
						}
					}

					Offset += sizeof(SpecFileBlock) + fnlen;
				}
			}

			R0_CloseFile(hLog, &ioStatus);
		}
	}

	R0_SetFileAttributes(_specfn, 0, &ioStatus);
	R0_DeleteFile(_specfn, 0, &ioStatus);

	DelInvisibleThread((ULONG) PsGetCurrentThreadId(), FALSE);

	SPImpProcessListImp(NULL);
}

void SpecialFileProcessing()
{
	DbPrint(DC_SYS, DL_IMPORTANT, ("SpecialFileProcessing: start"));

	gpSPMutex = InitKlgResourse();
	if (gpSPMutex == NULL)
	{
		DbPrint(DC_CLIENT, DL_ERROR, ("Can't init gpSPMutex\n"));
		DbgBreakPoint();
		return;
	}

	SPImp();
}

NTSTATUS SpecFileRequest(PSPECFILEFUNC_REQUEST pSpecRequest, ULONG ReqSize, PVOID OutputBuffer, ULONG OutputBufferLength, ULONG* pOutSize)
{
	NTSTATUS ntStatus = STATUS_INVALID_PARAMETER;

	switch(pSpecRequest->m_Request)
	{
	case _sfr_add:
		{
			pSpecRequest->m_Data.m_Result = _sfopr_new_request;
			switch (pSpecRequest->m_Data.m_FileOp)
			{
			case _sfop_delete:
			case _sfop_rename:
				{
					ULONG fnlen = CalcFNLen(&pSpecRequest->m_Data);
					if (fnlen)
					{
						PSpecFileBlock pNewBlock;
						
						pNewBlock = ExAllocatePoolWithTag(PagedPool, sizeof(SpecFileBlock) + fnlen, 'SBOS');
						if (!pNewBlock)
							ntStatus = STATUS_INSUFFICIENT_RESOURCES;
						else
						{
							pNewBlock->m_pNext = NULL;
							memcpy(&pNewBlock->m_Data, &pSpecRequest->m_Data, sizeof(SPECFILEFUNC_FILEBLOCK) + fnlen);
							
							AcquireResource(gpSPMutex, TRUE);
							
							if (gpSpecFileBlock)
							{
								gpSpecFileBlockLast->m_pNext = pNewBlock;
								gpSpecFileBlockLast = pNewBlock;
							}
							else
							{
								gpSpecFileBlock = pNewBlock;
								gpSpecFileBlockLast = pNewBlock;
							}
							
							ReleaseResource(gpSPMutex);
							
							ntStatus = STATUS_SUCCESS;
						}
					}
				}
				break;
			default:
				{
					// strange case
					DbgBreakPoint();
				}
				break;
			}
		}
		break;
	case _sfr_get:
		if (OutputBuffer && OutputBufferLength >= sizeof(ULONG))
		{
			SpecFileBlock* pBlock;
			SpecFileBlock* pBlockFound = NULL;
			ULONG idx = 0;
			AcquireResource(gpSPMutex, TRUE);

			pBlock = gpSpecFileBlock;

			while (!pBlockFound && pBlock)
			{
				if (pBlock->m_Data.m_ContainerID == pSpecRequest->m_Data.m_ContainerID)
				{
					if (idx == pSpecRequest->m_Idx)
						pBlockFound = pBlock;

					idx++;
				}
				pBlock = pBlock->m_pNext;
			}

			if (!pBlockFound)
			{
				*pOutSize = 0;
				ntStatus = STATUS_SUCCESS;
			}
			else
			{
				ULONG fnlen = CalcFNLen(&pBlockFound->m_Data);
				
				*(ULONG*) OutputBuffer = sizeof(SPECFILEFUNC_FILEBLOCK) + fnlen;
				
				if (OutputBufferLength < sizeof(SPECFILEFUNC_FILEBLOCK) + fnlen)
				{
					*pOutSize = sizeof(ULONG);

					ntStatus = STATUS_BUFFER_TOO_SMALL;
				}
				else
				{
					memcpy(OutputBuffer, &pBlockFound->m_Data, sizeof(SPECFILEFUNC_FILEBLOCK) + fnlen);
					*pOutSize = sizeof(SPECFILEFUNC_FILEBLOCK) + fnlen;

					ntStatus = STATUS_SUCCESS;
				}

			}

			ReleaseResource(gpSPMutex);
		}
		break;
	case _sfr_flush:
		if (!gpSpecFileBlock)
			ntStatus = STATUS_SUCCESS;
		else
		{
			ntStatus = STATUS_UNSUCCESSFUL;

			AddInvisibleThread((ULONG) PsGetCurrentThreadId());

			{
				int		BlockProcessed = 0;
				WORD    ioStatus;
				BYTE	ActionTaken = 0;

				BYTE*	ptr;
				ULONG	fnlen;
				ULONG	block_len;
				ULONG	Offset = 0;
				DWORD   Written;
				
				HANDLE hLog = R0_OpenCreateFile(TRUE, _specfn, OPEN_ACCESS_WRITEONLY | OPEN_SHARE_DENYWRITE, ATTR_HIDDEN | ATTR_SYSTEM, 
					ACTION_IFEXISTS_TRUNCATE | ACTION_IFNOTEXISTS_CREATE, 0, &ioStatus, &ActionTaken);
				if(!ioStatus && hLog)
				{
					SpecFileBlock* pBlock;
					AcquireResource(gpSPMutex, TRUE);

					ntStatus = STATUS_SUCCESS;

					pBlock = gpSpecFileBlock;
					while (pBlock &&  (STATUS_SUCCESS == ntStatus))
					{
						if (_sfopr_new_request == pBlock->m_Data.m_Result)
						{
							BlockProcessed++;

							fnlen = CalcFNLen(&pBlock->m_Data);

							block_len = sizeof(SPECFILEFUNC_FILEBLOCK) + fnlen + sizeof(ULONG);
							ptr = ExAllocatePoolWithTag(PagedPool, block_len, 'sBOS');
							if (ptr)
							{
								*(ULONG*)ptr = fnlen;
								memcpy(ptr + sizeof(fnlen), &pBlock->m_Data, sizeof(SPECFILEFUNC_FILEBLOCK) + fnlen);
								_xor_drv_func(ptr, block_len, 0);

								Written = R0_WriteFile(TRUE, hLog, ptr, block_len, Offset, &ioStatus);
								if (!ioStatus && Written == block_len)
									Offset += block_len;
								else
									ntStatus = STATUS_UNSUCCESSFUL;

								ExFreePool(ptr);
							}
						}

						pBlock = pBlock->m_pNext;
					}
					ReleaseResource(gpSPMutex);
					R0_CloseFile(hLog, &ioStatus);

					if (!BlockProcessed)
						R0_DeleteFile(_specfn, 0, &ioStatus);
				}
			}

			DelInvisibleThread((ULONG) PsGetCurrentThreadId(), FALSE);			
		}
		break;
	case _sfr_proceed_container:
		SPImpProcessListImp(&pSpecRequest->m_Data.m_ContainerID);
		ntStatus = STATUS_SUCCESS;
		break;
	default:
		{
			// strange case
			DbgBreakPoint();
		}
		break;
	}

	return ntStatus;
}
