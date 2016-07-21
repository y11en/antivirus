#include "enumctx.h"

tERROR __stdcall cbBatEnum(const wchar_t* sFileName,const tCHAR* m_sSection,const tCHAR* m_sValue,tDWORD dwFlag, EnumContext* pContext)
{
	return pContext->BatEnum(sFileName, m_sSection, m_sValue, dwFlag);
}

tERROR EnumContext::BatEnum(const wchar_t* sFileName,const tCHAR* m_sSection,const tCHAR* m_sValue,tDWORD dwFlag)
{
	tERROR		error;
	tERROR		fRet;
	tBOOL		bMissFile=cFALSE;
	hIO			hIo;
	tQWORD		qsize=0;
	tDWORD		size=0;
	tDWORD		fpointer=0;
	tCHAR*		buffer=NULL;
	tDWORD		dToWrite=2048;
	tDWORD		dRealRead=0;
	tDWORD		readed=0;
	tCHAR*		chLine=NULL;
	tCHAR*		chNext=NULL;
	cPrStrW     wsData;
	char		EndChar=0;
	
	cPrStrW     sFullFileName;
	const wchar_t* pFullFileName=NULL;
	
	
//INT3;
	PR_TRACE((m_pStartUpEnum,prtNOTIFY,"startupenum2\tParsing bat <%S>",sFileName));
	
	if (IfFileExist(sFileName, true)!=warnOBJECT_FALSE)
	{
		sFullFileName = m_sFullFilePathName;
		pFullFileName = sFileName;
	}
	else
		pFullFileName = sFileName;
	m_sFullFilePathName[0]=0; // ???
	m_SendData.m_ObjType=OBJECT_BAT_TYPE;
	m_SendData.m_sRoot = const_cast<wchar_t*>(pFullFileName);
	m_SendData.m_sSection=NULL;
	m_SendData.m_sValue=NULL;
	
	PR_TRACE((m_pStartUpEnum,prtSPAM,"startupenum2\tParsing bat <%S>",pFullFileName));
	
	error = m_pStartUpEnum->sysCreateObject((hOBJECT*)&hIo, IID_IO, PID_NATIVE_FIO);
	if (PR_FAIL(error))
	{
		PR_TRACE((m_pStartUpEnum,prtERROR,"startupenum2\tCannot create IO object for <%S> file, %terr",sFileName,error));
		return error;
	}
	cAutoObj<cIO> _bat(hIo);
	
	error = hIo->set_pgOBJECT_FULL_NAME((tPTR)pFullFileName,0,cCP_UNICODE);
	if (PR_SUCC(error))
	{
		// locked read
		cAutoNativeCS _lock(&m_pStartUpEnum->m_sCommonCriticalSection);
		error = hIo->sysCreateObjectDone();
	}
	if (PR_SUCC(error))
		error=CALL_IO_GetSize(hIo,&qsize,IO_SIZE_TYPE_EXPLICIT);
	if (PR_FAIL(error))
	{
		PR_TRACE((m_pStartUpEnum,prtERROR,"startupenum2\tCannot done IO object for <%S> file, %terr",sFileName,error));
		return error;
	}

	size=(tDWORD)qsize;	
	if (size<dToWrite) 
		dToWrite=size;
	if(PR_SUCC(error=hIo->heapAlloc((tPTR*)&buffer,dToWrite+1)))
	{
		while (fpointer<size)
		{
//INT3;
			if ((size-fpointer)<dToWrite) 
				dToWrite=size-fpointer;
			if(PR_FAIL(error=CALL_IO_SeekRead(hIo,&dRealRead,fpointer,buffer,dToWrite))) 
				break;
			buffer[dRealRead]=0; // ensure zero-terminate
			// skip zeroes
			for (readed=0;readed<dToWrite;readed++)
			{
				if (buffer[readed]!=0) 
					break;
			}
			chNext=buffer+readed;
			chLine=buffer+readed;
			fpointer+=readed;
//INT3;
			while (readed=GetLine(chLine,&chNext,&EndChar,&bMissFile))
			{
				if (chLine[0]!=0)
				{
					wsData = chLine;
					fRet=IfFileExist(wsData);
					if (fRet==warnOBJECT_INFECTED && (m_dwFlags&CURE_ACTION_ACTIVE))
					{
//INT3;
						cAutoNativeCS _lock(&m_pStartUpEnum->m_sCommonCriticalSection);
						tERROR	OpRet=errOK;
						tBOOL	NotTheEnd=FALSE;
						tDWORD  data_left = size-fpointer-readed;
						cPrStrA ToSaveBuf;

						if (data_left < 0)
							data_left = 0;
						if (data_left)
						{
							if (!ToSaveBuf.reserve_len(data_left+1))
							{
								PR_TRACE((m_pStartUpEnum,prtERROR,"startupenum2\tCannot allocate memory, %terr",OpRet));
								return errNOT_ENOUGH_MEMORY;
							}
							if(PR_SUCC(OpRet=CALL_IO_SeekRead(hIo,&dRealRead,fpointer+readed,ToSaveBuf,data_left)))
							{
								ToSaveBuf[data_left]=0;
								NotTheEnd=TRUE;
							}
							else 
								PR_TRACE((m_pStartUpEnum,prtERROR,"startupenum2\tCannot read file, %terr",OpRet));
						}
						tDWORD old_accessmode=hIo->get_pgOBJECT_ACCESS_MODE();
						hIo->set_pgOBJECT_ACCESS_MODE(fACCESS_WRITE);
						cPrStrA sToCopy = m_sDefaultValue;
						m_SendData.m_dAction=GetActionType(m_dwFlags,m_sDefaultValue,m_sOriginalString);
						switch (m_SendData.m_dAction) 
						{
						case CutData:
							sToCopy=m_sOriginalString;
						case RestDef:
							if(PR_SUCC(OpRet=CALL_IO_SeekWrite(hIo,&dRealRead,fpointer,sToCopy,strlen(sToCopy))))
							{
								if(PR_SUCC(OpRet=CALL_IO_SeekWrite(hIo,&dRealRead,fpointer+strlen(sToCopy),&EndChar,1)))
								{
									if (NotTheEnd)
									{
										if(PR_FAIL(OpRet=CALL_IO_SeekWrite(hIo,&dRealRead,fpointer+strlen(sToCopy)+1,ToSaveBuf,data_left))) PR_TRACE((m_pStartUpEnum,prtERROR,"startupenum2\tCannot write the end of data, %terr",OpRet));
									}
									if (PR_SUCC(OpRet)) size=strlen(sToCopy)+1+size-readed;
								}
								else 
									PR_TRACE((m_pStartUpEnum,prtERROR,"startupenum2\tCannot write the end of data, %terr",OpRet));
							}
							else 
								PR_TRACE((m_pStartUpEnum,prtERROR,"startupenum2\tCannot write default data, %terr",OpRet));
							break;
						case DelData:
							OpRet=errOK;
							if (NotTheEnd)
							{
								char* pToWrite = ToSaveBuf;
								if (ToSaveBuf[0]==0x0a) 
								{
									readed++;
									pToWrite++;
								}
								OpRet=CALL_IO_SeekWrite(hIo,&dRealRead,fpointer,pToWrite,data_left);
							}
							if(PR_SUCC(OpRet)) 
								size=size-readed;
							else 
								PR_TRACE((m_pStartUpEnum,prtERROR,"startupenum2\tCannot write the end of data, %terr",OpRet));
							break;
						default:
							OpRet=errNOT_FOUND;
						}
						if (PR_SUCC(OpRet))
						{
							OpRet=SendFileFound();
							error=hIo->SetSize(size);
							InterlockedIncrement(&m_pStartUpEnum->m_nModifyCounter);
						}
						hIo->set_pgOBJECT_ACCESS_MODE(old_accessmode);
						hIo->Flush();
						if (PR_FAIL(OpRet)) 
						{
							PR_TRACE((m_pStartUpEnum,prtNOTIFY,"startupenum2\t<BatEnum> error restore %x.",OpRet));
							OpRet = SendError(pFullFileName, NULL, NULL, OBJECT_BAT_TYPE, OpRet);
							if ((OpRet==ERROR_CANCELLED)||(OpRet==errOPERATION_CANCELED)) 
								return OpRet;
						}
						else 
							break;
					}
				}
				fpointer+=readed;
				chLine=chNext;
				if (!chLine) break;
			}
			if (bMissFile)
				break;
		}
		if (buffer) 
			hIo->heapFree(buffer);
	}
	else
		PR_TRACE((m_pStartUpEnum,prtERROR,"startupenum2\tCannot allocate memory, %terr",error));
	return error;
}