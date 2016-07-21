#include "stdafx.h"
#include "SysInfoDefines.h"
#include "HardDriveWork.h"
#include "MemoryScan.h"
#include "port32.h"

extern FILE*				LFile;

BYTE IdOutCmd[sizeof (SENDCMDOUTPARAMS)+IDENTIFY_BUFFER_SIZE-1];

DWORD ReadPhysicalDriveInNT(HWND hProgressCtrl,pCOUNTSTRUCT CountStruct,HANDLE hEv,bool* Ret)
{
   DWORD	done=ERROR_INVALID_FUNCTION;
   int		drive=0;
   HANDLE	hPhysicalDriveIOCTL=NULL;
   
   __try
   {
		for (drive=0;drive<MAX_IDE_DRIVES;drive++)
		{
			char driveName [256];
			sprintf (driveName,"\\\\.\\PhysicalDrive%d",drive);
			hPhysicalDriveIOCTL=CreateFile(driveName,GENERIC_READ|GENERIC_WRITE,FILE_SHARE_READ|FILE_SHARE_WRITE,NULL,OPEN_EXISTING,0,NULL);
			if (hPhysicalDriveIOCTL!=INVALID_HANDLE_VALUE)
			{
				GETVERSIONOUTPARAMS VersionParams;
				DWORD               cbBytesReturned=0;

				memset((void*)&VersionParams,0,sizeof(VersionParams));
				if (DeviceIoControl(hPhysicalDriveIOCTL,DFP_GET_VERSION,NULL,0,&VersionParams,sizeof(VersionParams),&cbBytesReturned,NULL))
				{         
					if (VersionParams.bIDEDeviceMap>0)
					{
						BYTE             bIDCmd=0;
						SENDCMDINPARAMS  scip;

						bIDCmd=(VersionParams.bIDEDeviceMap>>drive&0x10)?IDE_ATAPI_IDENTIFY:IDE_ATA_IDENTIFY;
						memset(&scip,0,sizeof(scip));
						memset(IdOutCmd,0,sizeof(IdOutCmd));
						if (DoIDENTIFY(hPhysicalDriveIOCTL,&scip,(PSENDCMDOUTPARAMS)&IdOutCmd,(BYTE)bIDCmd,(BYTE)drive,&cbBytesReturned))
						{
							DWORD	diskdata[256];
							int		ijk=0;
							USHORT *pIdSector=(USHORT*)((PSENDCMDOUTPARAMS)IdOutCmd)->bBuffer;
							for (ijk=0;ijk<256;ijk++) diskdata[ijk]=pIdSector[ijk];
							CollectIdeInfo(diskdata);
							done=NO_ERROR;
						}
					}
				}
				CloseHandle(hPhysicalDriveIOCTL);
				hPhysicalDriveIOCTL=NULL;
			}
			if (CheckStop(hProgressCtrl,IDC_PROGRESS_CURRENT,CountStruct,false,hEv)) 
			{
				if (Ret!=NULL) *Ret=true;
				done=NO_ERROR;
				break;
			}
		}
		return done;
   }
   __finally
   {
	   if (hPhysicalDriveIOCTL!=NULL) CloseHandle(hPhysicalDriveIOCTL);
   }
   return done;
}

DWORD ReadIdeDriveAsScsiDriveInNT(HWND hProgressCtrl,pCOUNTSTRUCT CountStruct,HANDLE hEv,bool* Ret)
{
   DWORD	done=ERROR_INVALID_FUNCTION;
   int		controller=0;
   HANDLE	hScsiDriveIOCTL=NULL;
   
   __try
   {
	   for (controller=0; controller<2; controller++)
	   {
		   char   driveName [256];
		   
		   sprintf (driveName, "\\\\.\\Scsi%d:", controller);
		   hScsiDriveIOCTL=CreateFile(driveName,GENERIC_READ|GENERIC_WRITE,FILE_SHARE_READ|FILE_SHARE_WRITE,NULL,OPEN_EXISTING,0,NULL);
		   if (hScsiDriveIOCTL!=INVALID_HANDLE_VALUE)
		   {
			   int drive=0;
			   for (drive=0;drive<2;drive++)
			   {
				   char buffer[sizeof(SRB_IO_CONTROL)+SENDIDLENGTH];
				   SRB_IO_CONTROL *p=(SRB_IO_CONTROL*)buffer;
				   SENDCMDINPARAMS *pin=(SENDCMDINPARAMS*)(buffer+sizeof(SRB_IO_CONTROL));
				   DWORD dummy;
				   
				   memset(buffer,0,sizeof(buffer));
				   p->HeaderLength=sizeof(SRB_IO_CONTROL);
				   p->Timeout=10000;
				   p->Length=SENDIDLENGTH;
				   p->ControlCode=IOCTL_SCSI_MINIPORT_IDENTIFY;
				   strncpy((char*)p->Signature,"SCSIDISK",8);
				   pin->irDriveRegs.bCommandReg=IDE_ATA_IDENTIFY;
				   pin->bDriveNumber=drive;
				   if (DeviceIoControl(hScsiDriveIOCTL,IOCTL_SCSI_MINIPORT,buffer,sizeof(SRB_IO_CONTROL)+sizeof(SENDCMDINPARAMS)-1,buffer,sizeof(SRB_IO_CONTROL)+SENDIDLENGTH,&dummy,NULL))
				   {
					   SENDCMDOUTPARAMS *pOut=(SENDCMDOUTPARAMS*)(buffer+sizeof(SRB_IO_CONTROL));
					   IDSECTOR *pId=(IDSECTOR*)(pOut->bBuffer);
					   if (pId->sModelNumber[0])
					   {
						   DWORD diskdata[256];
						   int ijk=0;
						   USHORT *pIdSector=(USHORT*)pId;
						   
						   for (ijk=0;ijk<256;ijk++) diskdata[ijk]=pIdSector[ijk];
						   CollectIdeInfo(diskdata);
						   done=NO_ERROR;
					   }
				   }
				   if (CheckStop(hProgressCtrl,IDC_PROGRESS_CURRENT,CountStruct,false,hEv)) 
				   {
					   controller=2;
					   if (Ret!=NULL) *Ret=true;
					   done=NO_ERROR;
					   break;
				   }
			   }
			   CloseHandle(hScsiDriveIOCTL);
			   hScsiDriveIOCTL=NULL;
		   }
	   }
	   return done;
   }
   __finally
   {
	   if (hScsiDriveIOCTL!=NULL) CloseHandle(hScsiDriveIOCTL);
   }
   return done;
}

DWORD ReadDrivePortsInWin9X (HWND hProgressCtrl,pCOUNTSTRUCT CountStruct,HANDLE hEv,bool* Ret)
{
	DWORD	done=ERROR_INVALID_FUNCTION;
	int		drive=0;
	DWORD	PortVal=0;

	__try
	{
		for (drive=0;drive<8;drive++)
		{
			DWORD diskdata[256];
			WORD  baseAddress=0;
			DWORD portValue=0;
			int waitLoop=0;
			int index=0;
			
			switch (drive/2)
			{
				case 0: 
					baseAddress=0x1f0; 
					break;
				case 1: 
					baseAddress=0x170; 
					break;
				case 2: 
					baseAddress=0x1e8; 
					break;
				case 3: 
					baseAddress=0x168; 
					break;
			}
			waitLoop=5;
			while (--waitLoop>0)
			{
				CallRing0((PVOID)Ring0GetPortVal,(WORD)(baseAddress+7),&portValue,(BYTE)1);
				if (CheckStop(hProgressCtrl,IDC_PROGRESS_CURRENT,CountStruct,false,hEv)) 
				{
					if (Ret!=NULL) *Ret=true;
					return done;
				}
				if ((portValue&0x40)==0x40) break;
				if ((portValue&0x01)==0x01) break;
			}
			if (hProgressCtrl) SendMessage(GetDlgItem(hProgressCtrl,IDC_PROGRESS_CURRENT),PBM_SETSTEP,100000-waitLoop,0);
			if (CountStruct) CountStruct->LocalStepCount -= waitLoop;
			if (CheckStop(hProgressCtrl,IDC_PROGRESS_CURRENT,CountStruct,false,hEv)) 
			{
				if (Ret!=NULL) *Ret=true;
				return done;
			}
			//if (CountStruct) CountStruct->LocalStep=CurrentStepChange;
			if (waitLoop<1) 
			{
				if (hProgressCtrl) SendMessage(GetDlgItem(hProgressCtrl,IDC_PROGRESS_CURRENT),PBM_SETSTEP,100258,0);
				if (CountStruct) CountStruct->LocalStepCount -= 1+1+5+1+256+1+1;
				if (CheckStop(hProgressCtrl,IDC_PROGRESS_CURRENT,CountStruct,false,hEv)) 
				{
					if (Ret!=NULL) *Ret=true;
					return done;
				}
				continue;
			}
			if ((drive%2)==0) PortVal=0xA0;
			else PortVal=0xB0;
			CallRing0((PVOID)Ring0SetPortVal,(WORD)(baseAddress+6),&PortVal,1);
			if (CheckStop(hProgressCtrl,IDC_PROGRESS_CURRENT,CountStruct,false,hEv)) 
			{
				if (Ret!=NULL) *Ret=true;
				return done;
			}
			PortVal=0xEC;
			CallRing0((PVOID)Ring0SetPortVal,(WORD)(baseAddress+7),&PortVal,1);
			if (CheckStop(hProgressCtrl,IDC_PROGRESS_CURRENT,CountStruct,false,hEv)) 
			{
				if (Ret!=NULL) *Ret=true;
				return done;
			}
			waitLoop=5;
			while (--waitLoop>0)
			{
				CallRing0((PVOID)Ring0GetPortVal,(WORD)(baseAddress+7),&portValue,1);
				if (CheckStop(hProgressCtrl,IDC_PROGRESS_CURRENT,CountStruct,false,hEv)) 
				{
					if (Ret!=NULL) *Ret=true;
					return done;
				}
				if ((portValue&0x48)==0x48) break;
				if ((portValue&0x01)==0x01) break;
			}
			if (hProgressCtrl) SendMessage(GetDlgItem(hProgressCtrl,IDC_PROGRESS_CURRENT),PBM_SETSTEP,100000-waitLoop,0);
			if (CheckStop(hProgressCtrl,IDC_PROGRESS_CURRENT,CountStruct,false,hEv)) 
			{
				if (Ret!=NULL) *Ret=true;
				return done;
			}
			if (waitLoop<1||portValue&0x01) 
			{
				if (hProgressCtrl) SendMessage(GetDlgItem(hProgressCtrl,IDC_PROGRESS_CURRENT),PBM_SETSTEP,257,0);
				if (CountStruct) CountStruct->LocalStepCount -= 1+1;
				if (CheckStop(hProgressCtrl,IDC_PROGRESS_CURRENT,CountStruct,false,hEv)) 
				{
					if (Ret!=NULL) *Ret=true;
					return done;
				}
				continue;
			}
			for (index=0;index<256;index++)
			{
				diskdata[index]=0;
				CallRing0((PVOID)Ring0GetPortVal,baseAddress,&(diskdata[index]),2);
				if (CheckStop(hProgressCtrl,IDC_PROGRESS_CURRENT,CountStruct,false,hEv)) 
				{
					if (Ret!=NULL) *Ret=true;
					return done;
				}
			}
			if (hProgressCtrl) SendMessage(GetDlgItem(hProgressCtrl,IDC_PROGRESS_CURRENT),PBM_SETSTEP,256-index,0);
			if (CountStruct) CountStruct->LocalStepCount -= index;
			if (CheckStop(hProgressCtrl,IDC_PROGRESS_CURRENT,CountStruct,false,hEv)) 
			{
				if (Ret!=NULL) *Ret=true;
				return done;
			}
			CollectIdeInfo(diskdata);
			done=NO_ERROR;
			if (CheckStop(hProgressCtrl,IDC_PROGRESS_CURRENT,CountStruct,false,hEv)) 
			{
				if (Ret!=NULL) *Ret=true;
				return done;
			}
		}
		return done;
	}
	__finally
	{
	}
	return done;
}

BOOL DoIDENTIFY(HANDLE hPhysicalDriveIOCTL,PSENDCMDINPARAMS pSCIP,PSENDCMDOUTPARAMS pSCOP,BYTE bIDCmd,BYTE bDriveNum,PDWORD lpcbBytesReturned)
{
	pSCIP->cBufferSize=IDENTIFY_BUFFER_SIZE;
	pSCIP->irDriveRegs.bFeaturesReg=0;
	pSCIP->irDriveRegs.bSectorCountReg=1;
	pSCIP->irDriveRegs.bSectorNumberReg=1;
	pSCIP->irDriveRegs.bCylLowReg=0;
	pSCIP->irDriveRegs.bCylHighReg=0;
	pSCIP->irDriveRegs.bDriveHeadReg=0xA0|((bDriveNum&1)<<4);
	pSCIP->irDriveRegs.bCommandReg=bIDCmd;
	pSCIP->bDriveNumber=bDriveNum;
	pSCIP->cBufferSize=IDENTIFY_BUFFER_SIZE;
	return (DeviceIoControl(hPhysicalDriveIOCTL,DFP_RECEIVE_DRIVE_DATA,(LPVOID)pSCIP,sizeof(SENDCMDINPARAMS)-1,(LPVOID)pSCOP,sizeof(SENDCMDOUTPARAMS)+IDENTIFY_BUFFER_SIZE-1,lpcbBytesReturned,NULL));
}

__declspec(naked) void Ring0GetPortVal()
{
	_asm
	{
		Cmp	CL,1
		Je	ByteVal
		Cmp CL,2
		Je	WordVal
		Cmp CL,4
		Je	DWordVal
			
ByteVal:
		In	AL,DX
		Mov [EBX],AL
		Retf
			
WordVal:
		
		In	AX,DX
		Mov [EBX],AX
		Retf
			
DWordVal:
		In	EAX,DX
		Mov [EBX],EAX
		Retf
	}
}

__declspec(naked) void Ring0SetPortVal()
{
	_asm
	{
		Cmp	CL,1
		Je	ByteVal
		Cmp CL,2
		Je	WordVal
		Cmp CL,4
		Je	DWordVal
			
ByteVal:
		Mov AL,[EBX]
		Out DX,AL
		Retf
			
WordVal:
		Mov AX,[EBX]
		Out DX,AX
		Retf
			
DWordVal:
		Mov EAX,[EBX]
		Out DX,EAX
		Retf
	}
}

bool CallRing0(PVOID pvRing0FuncAddr,WORD wPortAddr,PDWORD pdwPortVal,BYTE bSize)
{
	bool	Ret=false;
	struct	GDT_DESCRIPTOR *pGDTDescriptor;
	struct	GDTR gdtr;
	WORD	CallgateAddr[3];
	WORD	wGDTIndex=1;

	__try
	{
		_asm 
		{
			Sgdt[gdtr]
		}
//
//		HANDLE hMapCode=CreateFileMapping(INVALID_HANDLE_VALUE,NULL,PAGE_READWRITE,0,MaxSizeLow,FILE_MAPPING_VIRUS_PATCH);
//		if (hMapCode==NULL) return Ret;
//		LPVOID pMapAddr=MapViewOfFile(hMapCode,FILE_MAP_WRITE,0,0,MaxSizeLow);
//		
		pGDTDescriptor=(struct GDT_DESCRIPTOR*)(gdtr.dwGDTBase+8);
		for (wGDTIndex=1;wGDTIndex<(gdtr.wGDTLimit/8);wGDTIndex++)
		{
			if (!IsBadReadPtr(pGDTDescriptor,sizeof(GDT_DESCRIPTOR)))
			{
				if ((pGDTDescriptor->Type==0)&&(pGDTDescriptor->System==0)&&(pGDTDescriptor->DPL==0)&&(pGDTDescriptor->Present==0))
				{
					struct CALLGATE_DESCRIPTOR *pCallgate;
					pCallgate=(struct CALLGATE_DESCRIPTOR*)pGDTDescriptor;
					pCallgate->Offset_0_15=LOWORD(pvRing0FuncAddr);
					pCallgate->Selector=0x28;
					pCallgate->ParamCount=0;
					pCallgate->Unused=0;
					pCallgate->Type=0xc;
					pCallgate->System=0;
					pCallgate->DPL=3;
					pCallgate->Present=1;
					pCallgate->Offset_16_31=HIWORD(pvRing0FuncAddr);
					CallgateAddr[0]=0x0;
					CallgateAddr[1]=0x0;
					CallgateAddr[2]=(wGDTIndex<<3)|3;
					if (!IsBadCodePtr(*(FARPROC*)(&CallgateAddr)))
					{
						_asm
						{
							mov DX,[wPortAddr]
							mov EBX,[pdwPortVal]
							mov CL,[bSize]
							Call FWORD PTR [CallgateAddr]
						}
						memset(pGDTDescriptor, 0, 8);
						Ret=true;
						return Ret;
					}
					else return Ret;
				}
			}
			else return Ret;
//			else
//			{
//				pGDTDescriptor=pGDTDescriptor+sizeof(GDT_DESCRIPTOR)-1;
//				wGDTIndex=wGDTIndex+sizeof(GDT_DESCRIPTOR)-1;
//			}
			pGDTDescriptor++; 
		}
		return Ret;
	}
	__finally
	{
	}
	return Ret;
}

void CollectIdeInfo(DWORD diskdata[256])
{
	char*	Name=NULL;
	__try
	{
		if (diskdata[0]&0x0040)
		{
			ConvertToString(&Name,diskdata,27,46);
			if (Name!=NULL)
			{
				fprintf(LFile,"\t\t%s\n",Name);
				fflush(LFile);
			}
		}
		return;
	}
	__finally
	{
		if (Name!=NULL) free(Name);
	}
}

char* ConvertToString(char** Name,DWORD diskdata[256],int firstIndex,int lastIndex)
{
	char*		RetStr=NULL;
	static char string[1024];
	int			index=0;
	int			position=0;

	__try
	{
		for (index=firstIndex;index<=lastIndex;index++)
		{
			string[position]=(char)(diskdata[index]/256);
			position++;
			string[position]=(char)(diskdata[index]%256);
			position++;
		}
		string[position]='\0';
		for (index=position-1;(index>0)&&(' '==string[index]);index--) string[index]='\0';
		if (Name!=NULL) 
		{
			*Name=(char*)malloc(strlen(string)+1);
			if (*Name!=NULL) strcpy(*Name,string);
			RetStr=*Name;
		}
		return RetStr;
	}
	__finally
	{
	}
	return RetStr;
}
