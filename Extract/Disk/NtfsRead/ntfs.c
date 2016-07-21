#if defined (_WIN32)

#include <crtdbg.h>
#include "ntfs.h"

//#define _MEM_TRACE

#define MAX_LOAD_DATA_LEN 64*1024*1024

#ifdef _DEBUG
#include <stdio.h>
#define OUT_DEBUG(a) printf(a)
#define OUT_DEBUG1(a,b) printf(a,b)
#else
#define OUT_DEBUG(a) 
#define OUT_DEBUG1(a,b) 
#endif

#define DEL_MODE			1
#define DUMP_MODE			2
#define RECORD_DUMP_MODE	3
#define DIR_MODE			4
#define COPY_ATTRIB			5
#define CHECK_BAD			6

#ifdef _MEM_TRACE
DWORD g_dwAllocated = 0;
#endif

#define FAILURE(ret_val) (_ASSERT(FALSE), ret_val)
#define assert _ASSERT


const WCHAR $I30[]=L"$I30";

#define NTFS_GETU8(p)      (*(LPBYTE)(p))
#define NTFS_GETU16(p)     (*(LPWORD)(p))
#define NTFS_GETU24(p)     (NTFS_GETU32(p) & 0xFFFFFF)
#define NTFS_GETU32(p)     (*(LPDWORD)(p))
#define NTFS_GETU64(p)     (*(LONGLONG*)(p))
			
#define NTFS_GETS8(p)      (*(char *)(p))
#define NTFS_GETS16(p)     (*(signed short *)(p))
#define NTFS_GETS24(p)     (NTFS_GETU24(p) < 0x800000 ? (int)NTFS_GETU24(p) : (int)(NTFS_GETU24(p) | 0xFF000000))
#define NTFS_GETS32(p)     (*(int*)(p))
#define NTFS_GETS64(p)     (*(LONGLONG*)(p))
			
#define NTFS_PUTU8(p,v)      (*(LPBYTE)(p))=(v)
#define NTFS_PUTU16(p,v)     (*(LPWORD)(p))=(v)
#define NTFS_PUTU24(p,v)     NTFS_PUTU16(p,(v) & 0xFFFF);\
			NTFS_PUTU8(((char*)p)+2,(v)>>16)
#define NTFS_PUTU32(p,v)     (*(LPDWORD)(p))=(v)
#define NTFS_PUTU64(p,v)     (*(unsigned LONGLONG*)(p))=(v)
			
NTFS_FILE* NtfsOpenFile(HNTFS pVolume, MFT_REF MftRef);
void NtfsCloseFile(NTFS_FILE* pFile);

__inline LPVOID NtfsAllocMem(DWORD size)
{
	LPVOID ptr = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, size);
#ifdef _MEM_TRACE
	g_dwAllocated += size;
	printf("Mem:\t%08X\t%d\n", ptr, size);
#endif
	return ptr;
}

__inline LPVOID NtfsReallocMem(void* pMem, DWORD size)
{
#ifdef _MEM_TRACE
	DWORD nOldSize;
#endif
	LPVOID ptr;
	if (!pMem)
		return NtfsAllocMem(size);
#ifdef _MEM_TRACE
	nOldSize = HeapSize(GetProcessHeap(), 0, pMem);
	g_dwAllocated -= nOldSize;
	g_dwAllocated += size;
#endif
	ptr = HeapReAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, pMem, size);
#ifdef _MEM_TRACE
	printf("Mem:\t%08X\t-%d\n", pMem, nOldSize);
	printf("Mem:\t%08X\t%d\n", ptr, size);
#endif
	return ptr;
}

__inline void NtfsFreeMem(LPVOID ptr)
{
#ifdef _MEM_TRACE
	DWORD nOldSize;
#endif
	if (!ptr)
		return;
#ifdef _MEM_TRACE
	nOldSize = HeapSize(GetProcessHeap(), 0, ptr);
	g_dwAllocated -= nOldSize;
	printf("Mem:\t%08X\t-%d\n", ptr, nOldSize);
#endif
	HeapFree(GetProcessHeap(), 0, ptr);
}


int NtfsDecodeRun(BYTE **data, int *length, int *cluster,int *ctype)
{
	BYTE type=*(*data)++;
	
	*ctype=0;
	switch(type & 0xF)		// Numbers of Clusters
	{
	case 1: *length=NTFS_GETU8(*data);(*data)++;break;
	case 2: *length=NTFS_GETU16(*data);
		*data+=2;
		break;
	case 3: *length = NTFS_GETU24(*data);
		*data+=3;
		break;
	case 4: *length = NTFS_GETU32(*data);
		*data+=4;
		break;
		/* TODO: case 4-8 */
	default:
		OUT_DEBUG1("!!! Can't decode run type field %x",type);
		return -1;
	}
	
	switch(type & 0xF0)	//Offset
	{
	case 0:    *ctype=2;
		break;	// Compressed
	case 0x10: *cluster+=NTFS_GETS8(*data);
		(*data)++;
		break;
	case 0x20: *cluster+=NTFS_GETS16(*data);
		*data+=2;
		break;
	case 0x30: 	*cluster+=NTFS_GETS24(*data);
		*data+=3;
		break;
		/* TODO: case 0x40-0x80 */
	case 0x40:
		*cluster+=NTFS_GETS32(*data);
		*data+=4;
		break;
	default:
		OUT_DEBUG1("!!! Can't decode run type field %x",type);
		return -1;
	}
	return 0;
}

void NtfsFreeRunList(NTFS_RUN_LIST* rl)
{
	NTFS_RUN_LIST* rl1;
	while (rl)
	{
		rl1=rl->Next;
		NtfsFreeMem(rl);
		rl=rl1;
	}
	return;
}

NTFS_RUN_LIST* NtfsMakeRunList(LPVOID data, VCN startvcn, VCN endvcn, UINT* puTotalClusters, BOOL bIsCompr)
{
	VCN vcn;
	int len=0,ctype=0,cluster=0;
	NTFS_RUN_LIST* rls=NULL;
	NTFS_RUN_LIST* rl_prev = NULL;
	UINT tot=0;
	
	for(vcn=startvcn; vcn<=endvcn; vcn+=len)
	{
		NTFS_RUN_LIST* rl;
		if(NtfsDecodeRun((LPBYTE*)&data,&len,&cluster,&ctype))
			break;
		rl=NtfsAllocMem(sizeof(NTFS_RUN_LIST));
		if (!rl)
		{
			if (rls)
				NtfsFreeRunList(rls);
			return FAILURE(NULL);
		}
		if (!rls)
			rls = rl;
		if (rl_prev)	
			rl_prev->Next = rl;
		rl_prev = rl;
		if(ctype)		//Empty Run
			rl->cluster=-1;
		else
			rl->cluster=cluster;
		rl->len=len;
		tot+=len;
	}
	if(puTotalClusters)
		*puTotalClusters=tot;
	
	return rls;
}


void NtfsFreeDA(NTFS_ATTR* pAttr)
{
	if (!pAttr)
		return;
	if (!pAttr->nra)
		return;
	if (!pAttr->nra->IsNonResident)
		return;
	if (pAttr->pData)
		NtfsFreeMem(pAttr->pData);
	if (pAttr->rl)
		NtfsFreeRunList(pAttr->rl);
	if (pAttr->pComp)
	{
		NtfsFreeMem(pAttr->pComp->compressed);
		NtfsFreeMem(pAttr->pComp->decompressed);
		NtfsFreeMem(pAttr->pComp);
	}
	return;
}

void NtfsFreeAttrList(NTFS_ATTR_LIST* pAttrList)
{
	UINT i;
	NTFS_ATTR* Attributes;
	if (!pAttrList)
		return;
	for (i=0,Attributes=pAttrList->Attributes; i<pAttrList->nAttrCount; i++,Attributes++)
		NtfsFreeDA(Attributes);
	NtfsFreeMem(pAttrList->Attributes);
	if (pAttrList->ppMFTRecords)
	{
		void** ppMFTRecords;
		for (i=0,ppMFTRecords=pAttrList->ppMFTRecords; i<pAttrList->nMFTRecordsCount; i++,ppMFTRecords++)
		{
			if (*ppMFTRecords)
				NtfsFreeMem(*ppMFTRecords);
		}
		NtfsFreeMem(pAttrList->ppMFTRecords);
	}
	NtfsFreeMem(pAttrList);
	return;
}

/*
void FreeAttribList(NTFSListOfAttrib *al)
{
	NTFSListOfAttrib* al1;
	while(al)
	{
		al1=al->Next;
		NtfsFreeMem(al->DataToFree);
		NtfsFreeMem(al);
		al=al1;
	}
	return;
}

int CloseNTFSFile(HNTFSFile h)
{
	NtfsFreeRunList(h->rl);
	if(h->data)
		NtfsFreeMem(h->data);
	FreeAttribList(h->AttribList);
	if (h->pCacheData)
		NtfsFreeMem(h->pCacheData);
	NtfsFreeMem(h);
	return 0;
}
*/

static const char* arrAttrNames[] = {
	"<Unknown(0)>",
	"$STANDARD_INFORMATION",
	"$ATTRIBUTE_LIST",
	"$FILE_NAME",
	"$OBJECT_ID",
	"$SECURITY_DESCRIPTOR",
	"$VOLUME_NAME",
	"$VOLUME_INFORMATION",
	"$DATA",
	"$INDEX_ROOT",
	"$INDEX_ALLOCATION",
	"$BITMAP",
	"$REPARSE_POINT",
	"$EA_INFORMATION",
	"$EA",
	"$PROPERTY_SET",
	"$LOGGED_UTILITY_STREAM",
};

const char* NtfsGetAttrName(ATTRIB_TYPE nAttrType)
{
	if (0 == (nAttrType & 0x0F))
	{
		nAttrType >>= 4;
		if (nAttrType < countof(arrAttrNames))
		{
			return arrAttrNames[nAttrType];
		}
	}
	return "Unknown";
}

/*
#if !defined(_CONSOLE)

#define PrintFileName(lpName,iNO)
#define PrintMFTRecord(smft,iNo,bFullInfo,pVolume)

#else // _CONSOLE

void PrintFileName(NTFSFName* lpName,int iNO)
{
	int Len;
	char FName[MAX_PATH];
	FILETIME locFT;
	WORD Date,Time;
// 	LONGLONG MFTNo;
// 	FILETIME CreateTime;
// 	FILETIME LastModTime;
// 	FILETIME LastModMFT;
// 	FILETIME LastAccess;
// 	LONGLONG FileSize;
// 	LONGLONG AttribSize;
// 	LONGLONG Flags;
// 	BYTE NameLen;		//In words
// 	BYTE TypeOfName;
// 	WORD Name[1];
	
	ZeroMemory(FName,sizeof(FName));
	Len=WideCharToMultiByte(CP_ACP,0,lpName->Name,lpName->NameLen,FName,sizeof(FName),NULL,NULL);
	if (strcmp(FName, "acl.test") == 0)
		FName[0] = FName[0];
	printf("%s",FName);
	printf("\n\tType of name %d",lpName->TypeOfName);
	//	printf("%d ",Len);
	//	for(i=0;i<Len;i++)
	//		printf("%c",FName[i]);
// 	printf("\n");
// 	for(i=0;i<lpName->NameLen;i++)
// 	{
// 	printf("%C",lpName->Name[i]);
// 	wsprintf(&FName[i],"%C",lpName->Name[i]);
// 	}
	printf("\n\t File Size %I64u\n",lpName->FileSize);
	printf("\t Father    %d\n",lpName->MFTNo);
	
	FileTimeToLocalFileTime((FILETIME*)&lpName->CreateTime,&locFT);
	FileTimeToDosDateTime(&locFT,&Date,&Time);
	printf("\t+Create time\t\t\t%02d:%02d:%02d ",(Time >> 11) & 0x1F,(Time >> 5) & 0x3F,Time & 0x1F);
	printf("%02d/%02d/%4d\n",Date & 0x1F,(Date >> 5) & 0xF,((Date >> 9) & 0x7F) + 1980);
	
}
*/
/*
void PrintMFTRecord(MFT* smft,int iNo,BOOL bFullInfo,HNTFS pVolume)
{
	NTFSResAttrib* ra;
	NTFSNonResAttrib* nra;
	//NTFSVolumeInformation* volInfo;
	int i;
	char FName[MAX_PATH];
	//LONGLONG vcn;
	LONGLONG startvcn;
	LONGLONG endvcn;
	LPBYTE data;
	int len=0,ctype=0,cluster=0;
	//data* tmp;
	
	if(bFullInfo)
	{
		printf("MagicNumber = '%C%C%C%C'\n",*((LPBYTE)(smft)),*((LPBYTE)(smft)+1),*((LPBYTE)(smft)+2),*((LPBYTE)(smft)+3));
		printf("Offset to FixUp %d (%xh)\n",smft->OffsetToFixup,smft->OffsetToFixup);
		printf("Number Of FixUp %d\n",smft->NumberOfFixups);
		printf("Sequence Number %d\n",smft->SequenceNumber);
		printf("Number Of Hard Links %d\n",smft->HardLinkCount);
		
		printf("Length of Header + Attributes %d\n",smft->LengthOfPart);
		printf("Allocated Length of record %d\n",smft->AllocatedLength);
		printf("Offset To Attributes %d \n",smft->OffsetToAttributes);
	}
	printf("Flags %d ",smft->Flags);
	
	if((smft->Flags & 1) == 0)
	{
		printf("Entry deleted \n+++++++\n\n");
		return;
	}
	
	if((smft->Flags & 1) == 1)
		printf("Present ");
	if((smft->Flags & 2)==2)
	{	
		printf("Directory");
		//		tmp=HeapAlloc(GetProcessHeap(),HEAP_ZERO_MEMORY,sizeof(data));
		//		tmp->data=TotalDirs;
	}
	printf("\n");
	printf("Main MFT Record %I64u\n",smft->MainMFTRecord);
	printf("Maximum Attribute number %X\n",smft->MaximumAttr);
	
	if(smft->OffsetToAttributes!=0)
	{
		int Attrlen=0;
		ra=(NTFSResAttrib*)&((LPBYTE)smft)[smft->OffsetToAttributes];
		nra=(NTFSNonResAttrib* )ra;
		while(ra->AttrType!=AttribNoMoreAttrib)
		{
			printf("\tAttribute type %Xh ",ra->AttrType);
			switch(ra->AttrType)
			{
			case AttribStandartInfo:
				printf("(Standart Information)\n");
				//					if(bFullInfo)
				//						PrintStandartInfo((NTFSStandartInfo*)&((LPBYTE)ra)[ra->OffsetToData]);
				break;
			case AttribAttribList:
				printf("(Attribute list)\n");
				//					if(ra->IsNonResident==0)		//Resident
				//						PrintAttribList((NTFSAttribList*)&((LPBYTE)ra)[ra->OffsetToData],ra->DataLength);
				//					else
				//					{
				//					LPBYTE lpData;
				//					DWORD dwPos;
				//					NtfsIo io;
				//						startvcn=nra->StartVCN;
				//						endvcn=nra->LastVCN;
				//						data=&((LPBYTE)nra)[nra->OffsetToData];
				//						lpData=NtfsAllocMem(((DWORD)(endvcn-startvcn+2))*4096);
				//						len=0,ctype=0,cluster=0;
				//						io.do_read=1;						
				//						for(vcn=startvcn,dwPos=0; vcn<=endvcn; vcn+=len)
				//						{
				//							if(NtfsDecodeRun(&data,&len,&cluster,&ctype))
				//								break;
				//							if(ctype)
				//								continue;
				//							else
				//							{
				//								io.param=&lpData[dwPos*GetClusterSize(pVolume)];
				//								io.size=len*GetClusterSize(pVolume);
				//								NtfsGetPutClusters(pVolume,cluster,0,&io);
				//								dwPos+=len;
				//							}
				//						}
				//						PrintAttribList((NTFSAttribList*)lpData,(int)nra->LengthInitData-1);
				//						NtfsFreeMem(lpData);
				//					}
				break;
			case AttribFileName:
				printf("(File name) -- ");
				PrintFileName((NTFSFName*)&((LPBYTE)ra)[ra->OffsetToData],iNo);
				break;
			case AttribObjectId:
				printf("(Object ID)\n");
				break;
			case AttribSecurityDescriptor:
				printf("(Security Descriptor)");
				if (ra->IsNonResident==0)		//Resident
				{
					printf(" >> ");
					for(i=0;i<(int)ra->Length;i++)
					{
						printf("%02X ",((LPBYTE)ra)[ra->OffsetToData+i]);
					}
				}
				else
				{
					printf("NON RESIDENT");
				}
				printf("\n");
				break;
			case AttribVolumeName:
				printf("(Volume Name)\n");
				if(!bFullInfo)
					break;
				i=WideCharToMultiByte(CP_ACP,0,(WCHAR*)&((LPBYTE)ra)[ra->OffsetToData],ra->DataLength/sizeof(WCHAR),FName,sizeof(FName),NULL,NULL);
				FName[i]=0;
				printf("\tVolume Label id '%s'\n",FName);
				break;
			case AttribVolumeInfo:
				printf("(Volume Info)\n");
				//					if(!bFullInfo)
				//						break;
				//					volInfo=(NTFSVolumeInformation*)&((LPBYTE)ra)[ra->OffsetToData];
				//					printf("\tVersion of NTFS %d.%d\n",volInfo->MajorVersion,volInfo->MinorVersion);
				break;
			case AttribData:
				printf("(Data)\n");break;
			case AttribIndexRoot:
				printf("(Index Root)\n");
				//					if(!bFullInfo)
				//						break;
				//					if(ra->IsNonResident==0)		//Resident
				//						PrintIndexRoot((NTFS_INDEX_ROOT*)&(((LPBYTE)ra)[ra->OffsetToData]));
				//					else
				//					{
				//						__asm nop;
				//					}
				break;
			case AttribIndexAllocation:
				printf("(Index Allocation)\n");
				//					if(!bFullInfo)
				//						break;
				//					if(ra->IsNonResident==0)		//Resident
				//						PrintIndexAllocator(pVolume,(NTFS_INDEX*) &(((LPBYTE)ra)[ra->OffsetToData]),ra->Length);
				//					else
				//					{
				//					LPBYTE lpData;
				//					DWORD dwPos;
				//					NtfsIo io;
				//						startvcn=nra->StartVCN;
				//						endvcn=nra->LastVCN;
				//						data=&((LPBYTE)nra)[nra->OffsetToData];
				//						lpData=NtfsAllocMem(((DWORD)(endvcn-startvcn+2))*4096);
				//						len=0,ctype=0,cluster=0;
				//						io.do_read=1;						
				//						for(vcn=startvcn,dwPos=0; vcn<=endvcn; vcn+=len)
				//						{
				//							if(NtfsDecodeRun(&data,&len,&cluster,&ctype))
				//								break;
				//							if(ctype)
				//								;
				////								printf("\t--Cluster %d (%X), length %d\n",-1,-1,len);
				//							else
				////								printf("\t--Cluster %d (%X), length %d\n",cluster,cluster,len);
				//							{
				//								io.param=&lpData[dwPos*GetClusterSize(pVolume)];
				//								io.size=len*GetClusterSize(pVolume);
				//								NtfsGetPutClusters(pVolume,cluster,0,&io);
				//								dwPos+=len;
				//							}
				//						}
				//						PrintIndexAllocator(pVolume,(NTFS_INDEX*) lpData,(int)nra->SizeOfAttrib);//dwPos*GetClusterSize(pVolume));
				//						NtfsFreeMem(lpData);
				//					}
				break;
			case AttribBitmap:
				printf("(Bitmap)\n");
				break;
			case AttribReparsePoint:
				//				{
				//				NTFSReparsePointData* repData;
				//					printf("(Reparse Point)\n");
				//					if(!bFullInfo)
				//						break;
				//					repData=(NTFSReparsePointData*)&((LPBYTE)ra)[ra->OffsetToData];
				//					printf("\tTag %x",repData->ReparseTag);
				//					if(repData->ReparseTag & 0x80000000)
				//						printf(" MS owned tag,");
				//					if(repData->ReparseTag & 0x40000000)
				//						printf(" High-latency,");
				//					if(repData->ReparseTag & 0x20000000)
				//						printf(" Name surrogate");
				//					switch(repData->ReparseTag)
				//					{
				//						case IO_REPARSE_TAG_SYMBOLIC_LINK|0x80000000:
				//							printf("\n\tSymbolic link");
				//							break;
				//						case IO_REPARSE_TAG_MOUNT_POINT:
				//							printf("\n\tMount point and junction");
				//							break;
				//						case IO_REPARSE_TAG_HSM:
				//							printf("\n\tHIERARCHICAL STORAGE MANAGEMENT REPARSE");
				//							break;
				//						case IO_REPARSE_TAG_NSS:
				//							printf("\n\tNATIVE STRUCTURED STORAGE POINT");
				//							break;
				//						case IO_REPARSE_TAG_NSSRECOVER:
				//							printf("\n\tNATIVE STRUCTURED STORAGE RECOVERY POINT");
				//							break;
				//						case IO_REPARSE_TAG_SIS:
				//							printf("\n\tSinlge Instance Store point");
				//							break;
				//						case IO_REPARSE_TAG_DFS:
				//							printf("\n\tDistributed File System");
				//							break;
				//					}
				//
				//
				//					printf("\n\tDataLength %x\n",repData->DataLength);
				//					i=WideCharToMultiByte(CP_ACP,0,(WCHAR*)&((LPBYTE)ra)[ra->OffsetToData+sizeof(NTFSReparsePointData)-1],(repData->DataLength-sizeof(GUID))/sizeof(WCHAR),FName,sizeof(FName),NULL,NULL);
				//					FName[i]=0;
				//					printf("\tReparce point data '%s'\n",FName);
				break;
				//				}
			case AttribEAInfo:
				printf("(HPFS EA Info)\n");
				break;
			case AttribEA:
				printf("(HPFS EA)\n");
				break;
			case AttribPropertySet:
				printf("(Property Set)\n");
				break;
			case AttribLoggedUtilityStream:
				printf("(Logged Utility Stream)\n");
				break;
			}
			if(!bFullInfo)
				goto loc_cont;
			
			
			printf("\tLength %d\n",ra->Length);
			printf("\tAttribute Is %s\n",ra->IsNonResident==0 ? "Resident":"Non resident");		//0=resident, 1=data stored in runs
			printf("\tName Length %d\n",ra->NameLength);
			printf("\tOffset to name or resident data %d\n",ra->Offset);			// to name or resident data
			if(ra->NameLength!=0)
			{
				printf("\t");
				for(i=0;i<ra->NameLength;i++)
					printf("%C",((LPWORD) &((LPBYTE)ra)[ra->Offset])[i]);
				printf("\n");
			}
			printf("\tAttribute Is %s\n",ra->IsCompressed==1 ? "Compressed": "Not Compressed");	//1=compressed
			printf("\tAttribute ID %X\n\n",ra->AttributeID);
			
			if(ra->IsNonResident==0)		//resid
			{
				printf("\tResident Attrib Info\n");
				printf("\tData Length %d\n",ra->DataLength);
				printf("\tOffset To Data %d\n",ra->OffsetToData);
				printf("\tIs Indexed %d\n",ra->IsIndexed);
			}
			else
			{
				startvcn=nra->StartVCN;
				endvcn=nra->LastVCN;
				len=0,ctype=0,cluster=0;
				printf("\tNon Resident Attrib Info\n");
				if(nra->SizeOfAttrib>0xffffffff)
					__asm nop;
				
				printf("\tStart VCN %I64u\n",nra->StartVCN);
				printf("\tEnd VCN %I64u\n",nra->LastVCN);
				printf("\tOffset To Data %d\n",nra->OffsetToData);
				//				printf("\tCompression Engine %d\n",nra->CompressionUnit);	// ? '4' for compressed files
				printf("\tSize Of Attribute %I64u\n",nra->SizeOfAttrib);
				printf("\tAllocated Disk Space %I64u\n",nra->DiskSpace);
				printf("\tLength of initialized data %I64u\n",nra->LengthInitData);
				if(nra->IsCompressed==1)
					printf("\tCompressed size %I64u\n",nra->ComprSize);
				
				data=&((LPBYTE)nra)[nra->OffsetToData];
				
				//				printf("\t");
				//				for(i=0;i<(int)ra->Length-nra->OffsetToData;i++)
				//					printf("%02X ",data[i]);
				//				printf("\n");
				//
				//				for(vcn=startvcn; vcn<=endvcn; vcn+=len)
				//				{
				//					if(NtfsDecodeRun(&data,&len,&cluster,&ctype))
				//						break;
				//					if(ctype)
				//						printf("\t--Cluster %d (%X), length %d\n",-1,-1,len);
				//					else
				//						printf("\t--Cluster %d (%X), length %d\n",cluster,cluster,len);
				//
				////
				////					if(ctype)
				////						ntfs_insert_run(attr,cnum,-1,len);
				////					else
				////						ntfs_insert_run(attr,cnum,cluster,len);
				////					cnum++;
				////
				//				}
			}
loc_cont:
			if(bFullInfo)
				printf("---\n");
			if((int)ra->Length>(1024-Attrlen))
				break;
			Attrlen+=ra->Length;
			ra=(NTFSResAttrib*)&((LPBYTE)ra)[ra->Length];
			nra=(NTFSNonResAttrib* )ra;
		}
	}
	{
// 	NtfsIo io;
// 	char Buffer1[1024];
// 	io.do_read=1;
// 	io.size=1024;
// 	io.param=Buffer1;
// 	NtfsGetPutClusters(pVolume,576252,0,&io);
	}
	printf("===========================================\n\n");
}

#endif // _CONSOLE
*/
/*
WORD  GetHDDDriveType(BYTE Drive,HDDINFO * hddi,HDDPARAM* HDDP)
{
int i;
char TextForOut[512];
HANDLE hDrive;
DWORD dwRet;
DISK_GEOMETRY dg;

  if(hddi==NULL)
  return 0xffff;
  wsprintf(TextForOut,"\\\\.\\PHYSICALDRIVE%d",Drive-0x80);
  hDrive=CreateFile(TextForOut,0,FILE_SHARE_READ|FILE_SHARE_WRITE,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
  if(hDrive==INVALID_HANDLE_VALUE)
  {
  //MessageBox(GetFocus(),TextForOut,"GetHDDDriveType Error Open",0);
  return 0xffff;
  }
  DeviceIoControl(hDrive,IOCTL_DISK_GET_DRIVE_GEOMETRY,NULL,0,&dg,sizeof(DISK_GEOMETRY),&dwRet,NULL);
  CloseHandle(hDrive);
  
	hddi->Heads=(WORD)dg.TracksPerCylinder;
	hddi->Cylinders=(WORD)dg.Cylinders.LowPart;
	hddi->Sectors=(WORD)dg.SectorsPerTrack;
	
	  for(i=0x80;i<0x90;i++)
	  {
	  wsprintf(TextForOut,"\\\\.\\PHYSICALDRIVE%d",i-0x80);
	  hDrive=CreateFile(TextForOut,0,FILE_SHARE_READ|FILE_SHARE_WRITE,NULL,OPEN_EXISTING,
	  FILE_ATTRIBUTE_NORMAL,NULL);
	  if(hDrive==INVALID_HANDLE_VALUE)
	  break;
	  hddi->HowHDD=i-0x80+1;
	  CloseHandle(hDrive);
	  }
	  
		if(HDDP)
		{
		HDDP->Cylinders=(WORD)dg.Cylinders.LowPart;
		HDDP->Sectors=(WORD)dg.SectorsPerTrack;
		HDDP->Heads=(WORD)dg.TracksPerCylinder;
		HDDP->HDDId=(BYTE)Drive;
		}
		
		  //	wsprintf(TextForOut,"HDDP->Cylinders=%d\nHDDP->Sectors=%d\nHDDP->Heads=%d\nHDDP->HDDId=%X\nhddi->HowHDD=%d",
		  //		HDDP->Cylinders,HDDP->Sectors,HDDP->Heads,HDDP->HDDId,hddi->HowHDD);
		  //	MessageBox(0,TextForOut,"GetHDDDriveType()",0);
		  
			return 0;
			}
			*/
			
			/*
			void FreeHandles(void)
			{
			#ifdef _NT_
			if(hDrive && hDrive !=INVALID_HANDLE_VALUE)
			{
			CloseHandle(hDrive);
			}
			hDrive=INVALID_HANDLE_VALUE;
			#endif
			}
			}
			}
			}
			*/
			
// Drive name ('A', 'B', etc)

BOOL NtfsSeekReadLogicalDrive(HANDLE hDrive, LPVOID pBuffer, LONGLONG nOffset, DWORD dwNumberOfBytesToRead, DWORD* pdwNumberOfBytesRead)
{
	static LONGLONG nLastOffset = 0;
	BOOL bRes;
	LONG* OffsetLongPart = (LONG*)&nOffset;
	DWORD dwNumberOfBytesRead;
	if (!pdwNumberOfBytesRead)
		pdwNumberOfBytesRead = &dwNumberOfBytesRead;
	
	SetFilePointer(hDrive,OffsetLongPart[0],&OffsetLongPart[1],FILE_BEGIN);
	bRes = ReadFile(hDrive,pBuffer,dwNumberOfBytesToRead,pdwNumberOfBytesRead,NULL);
	if (!pdwNumberOfBytesRead && dwNumberOfBytesRead!=dwNumberOfBytesToRead)
		return FAILURE(FALSE);
	
	//if (nLastOffset	== nOffset)
	//{
	//	printf("Read: %16I64X %8X %d\n", nOffset, dwNumberOfBytesToRead, dwError);
	//}
	nLastOffset = nOffset;
	return bRes;
}

BOOL NtfsSeekReadVolume(HNTFS pVolume, LPVOID pBuffer, LONGLONG nOffset, DWORD dwNumberOfBytesToRead, DWORD* pdwNumberOfBytesRead)
{
#if defined(USE_LL_DISK_IO)
	if (pVolume->llio)
		return LLDiskIORead((LLDISKIO*)pVolume->llio, nOffset, pBuffer, dwNumberOfBytesToRead, pdwNumberOfBytesRead);
#endif
	return NtfsSeekReadLogicalDrive(pVolume->hDrive, pBuffer, nOffset, dwNumberOfBytesToRead, pdwNumberOfBytesRead);
}


int NtfsInitVolume(HNTFS vol, NTFSBOOT *boot)
{
	// Historical default values, in case we don't load $AttrDef
	/*
	vol->at_standard_information=0x10;
	vol->at_attribute_list=0x20;
	vol->at_file_name=0x30;
	vol->at_security_descriptor=0x50;
	vol->at_data=0x80;
	vol->at_index_root=0x90;
	vol->at_index_allocation=0xA0;
	vol->at_bitmap=0xB0;
	vol->at_symlink=0xC0;
	*/
	vol->blocksize=boot->secsize;//NTFS_GETU16(boot+0xB); //secsize
	vol->clusterfactor=boot->clustsize;//NTFS_GETU8(boot+0xD);	//clustsize
	vol->mft_clusters_per_record=boot->MFTRecordSize;//NTFS_GETS8(boot+0x40);	//MFTRecordSize
	vol->index_clusters_per_record=boot->IndexBufferSize;//NTFS_GETS8(boot+0x44);//IndexBufferSize

	/*	
	if(NTFS_GETU32(boot+0x40)>256)
	ntfs_error("Unexpected data #1 in boot block\n");
	if(NTFS_GETU32(boot+0x44)>256)
	ntfs_error("Unexpected data #2 in boot block\n");
	if(vol->index_clusters_per_record<0)
	{
	ntfs_error("Unexpected data #3 in boot block\n");
	vol->index_clusters_per_record=1;
	}
	// in some cases, 0xF6 meant 1024 bytes. Other strange values have not been observed

	if(vol->mft_clusters_per_record<0 && vol->mft_clusters_per_record!=-10)
	ntfs_error("Unexpected data #4 in boot block\n");
	*/

	vol->clustersize=vol->blocksize*vol->clusterfactor;
	
	if(vol->mft_clusters_per_record>0)
		vol->mft_recordsize=vol->clustersize*vol->mft_clusters_per_record;
	else
		vol->mft_recordsize=1<<(-vol->mft_clusters_per_record);
	
	vol->index_recordsize=vol->clustersize*vol->index_clusters_per_record;
	
	vol->mft_cluster=boot->MFTCluster;//NTFS_GETU64(boot+0x30);	//MFTCluster
	
	/* This will be initialized later */
	//	vol->upcase=0;
	//	vol->upcase_length=0;
	//	vol->mft_ino=0;
	return 0;
}
			
int NtfsFixupRecord(HNTFS vol, LPBYTE record, DWORD magic, DWORD size)
{
	int start, count, offset;
	WORD fixup;
	
	if(!IS_MAGIC(record,magic))
	{
		OUT_DEBUG("Error Check Magic sign\n");
		return 0;
	}
	
	start=NTFS_GETU16(record+4);
	count=NTFS_GETU16(record+6);
	count--;
	if(size && vol->blocksize*count != size)
	{
		OUT_DEBUG("Size not alligned on 512\n");
		return 0;
	}
	fixup = NTFS_GETU16(record+start);
	start+=2;
	offset=vol->blocksize-2;
	while(count--)
	{
		if(NTFS_GETU16(record+offset)!=fixup)
		{
			OUT_DEBUG1("Error fixup record# %d\n",count);
			return 0;
		}
		NTFS_PUTU16(record+offset, NTFS_GETU16(record+start));
		start+=2;
		offset+=vol->blocksize;
	}
	return 1;
}

void NtfsDecompress(LPBYTE dest, LPBYTE src, UINT l)
{
	UINT head,compressed_data;
	UINT copied=0;
	LPBYTE stop;
	int bits;
	UINT tag=0;
	int clear_pos;
	int i,len,delta,lmask,dshift;
	UINT code;
	
	while(1)
	{
		//		head = NTFS_GETU16(src) & 0xFF;
		//		if(head==0)
		// block is not used 
		//			return;// FIXME: copied 
		// high bit indicates that compression was performed
		/**		compressed_data = NTFS_GETU8(src) & 0x80;
		if(head==1)
		{
		src += 2;
		memcpy(dest,src,0x1000);
		dest+=0x1000;
		copied+=0x1000;
		src+=0x1000;
		if(l==copied)
		return;
		continue;
	}*/
		head = NTFS_GETU16(src) & 0xFFF;
		if(head==0)
			// block is not used 
			return;// FIXME: copied 
				   /*/*		if(head==0xfff)
				   {
				   src += 2;
				   memcpy(dest,src,0x1000);
				   dest+=0x1000;
				   copied+=0x1000;
				   src+=0x1000;
				   if(l==copied)
				   return;
				   continue;
	}*/
		compressed_data = NTFS_GETU16(src) & 0xb000;
		//		compressed_data = (head == 0xFFF);
		src += 2;
		stop = src+head;
		bits = 0;
		clear_pos=0;
		if((compressed_data & 0x8000 )!=0x8000 /*|| head==0xfff*/)
		{
			memcpy(dest,src,head+1);//0x1000);	//head+1
			dest+=head+1;//0x1000;
			copied+=head+1;//0x1000;
			src+=head+1;//0x1000;
			if(l==copied)
				return;
			continue;
		}
		/**		if(head==0)
		// block is not used 
		return;// FIXME: copied **/
		/***		if(!compressed_data && head==0xfff) // uncompressible
		{
		memcpy(dest,src,0x1000);
		dest+=0x1000;
		copied+=0x1000;
		src+=0x1000;
		if(l==copied)
		return;
		continue;
	}***/
		while(src<=stop)
		{
			if(clear_pos>4096)
			{
				OUT_DEBUG("Error 1 in decompress");
				return;
			}
			if(!bits)
			{
				tag=NTFS_GETU8(src);
				bits=8;
				src++;
				if(src>stop)
					break;
			}
			if(tag & 1)
			{
				code = NTFS_GETU16(src);
				src+=2;
				if(!clear_pos)
				{
					OUT_DEBUG("Error 2 in decompress");
					return;
				}
				for(i=clear_pos-1,lmask=0xFFF,dshift=12;i>=0x10;i>>=1)
				{
					lmask >>= 1;
					dshift--;
				}
				delta = code >> dshift;
				len = (code & lmask) + 3;
				for(i=0; i<len; i++)
				{
					dest[clear_pos]=dest[clear_pos-delta-1];
					clear_pos++;
					copied++;
					if(copied==l)
						return;
				}
			}else{
				dest[clear_pos++]=NTFS_GETU8(src);
				src++;
				copied++;
				if(copied==l)
					return;
			}
			tag>>=1;
			bits--;
		}
		dest+=clear_pos;
	}
}

BOOL NtfsReadCompressed(NTFS_ATTR* pAttr, LONGLONG offset, DWORD dwBytesToRead, LPVOID pBuffer, DWORD* pdwBytesRead)
{
	UINT clustersize,chunk;
	UINT rl_vcn,rl_cluster,rl_len,got,read_cluster,read_len,buffer_offset,rl_offset;
	UINT read_vcn;
	char *compressed_data,*pData;
	char *decompressed_data;
	NTFS_RUN_LIST *rl;
	BOOL bDecompress;
	DWORD dwBytesRead = 0;
	LPBYTE pDestPtr = (LPBYTE)pBuffer;
	NTFS_CACHE_DATA* pComp;
	DWORD dwBytesToReadSaved = dwBytesToRead;

	if (pdwBytesRead)
		*pdwBytesRead = 0;

	if (!pAttr)
		return FAILURE(FALSE);

	clustersize = pAttr->pFile->pVolume->clustersize;
	rl = pAttr->rl;
	rl_vcn = 0;

	if (!pAttr->pComp)
	{
		pAttr->pComp = NtfsAllocMem(sizeof(NTFS_CACHE_DATA));
		if (!pAttr->pComp)
			return FAILURE(FALSE);
		pAttr->pComp->compressed = NtfsAllocMem(32*clustersize);
		if (!pAttr->pComp->compressed)
		{
			NtfsFreeMem(pAttr->pComp);
			pAttr->pComp = NULL;
			return FAILURE(FALSE);
		}
		pAttr->pComp->decompressed = NtfsAllocMem(32*clustersize);
		if (!pAttr->pComp->decompressed)
		{
			NtfsFreeMem(pAttr->pComp->compressed);
			NtfsFreeMem(pAttr->pComp);
			pAttr->pComp = NULL;
			return FAILURE(FALSE);
		}
	}
	pComp = pAttr->pComp;
	compressed_data = pComp->compressed;
	decompressed_data = pComp->decompressed;

	while(dwBytesToRead)
	{
		chunk=0;
		read_vcn = (UINT)(offset / clustersize);
		read_vcn &= ~15; // round down to multiple of 16 
		if (!rl)
			return FAILURE(FALSE);
		while(rl_vcn+rl->len <= read_vcn)
		{
			rl_vcn += rl->len;
			rl = rl->Next;
			if (!rl)
				return FAILURE(FALSE);
		}

		rl_cluster = rl->cluster;
		rl_len = rl->len;

		if(rl_cluster == -1) // sparse
		{
			assert(((rl_vcn+rl_len-read_vcn) & 15) == 0); // verify sparse chunk size
			chunk = (UINT)min((LONGLONG)(rl_vcn+rl_len)*clustersize-offset, dwBytesToRead);
			ZeroMemory(pDestPtr, chunk);
		}
		else // reading
		{
			got=0;
			bDecompress = FALSE;
			// we might need to start in the middle of a run 
			rl_offset = read_vcn - rl_vcn;
			pData = pAttr->pComp->compressed;
			do{
				read_cluster = rl_cluster + rl_offset;
				read_len = min(rl_len-rl_offset, 16-got);

				if (pAttr->pComp->cached_lcn == read_cluster && pAttr->pComp->cached_clusters >= read_len)
				{
					// cache hit
					if (pComp->cached_data != pData)
					{
						memmove(pData, pComp->cached_data, read_len*clustersize);
						pComp->cached_clusters = read_len;
						bDecompress = TRUE;
					}
				}
				else
				{
					if (!NtfsSeekReadVolume(pAttr->pFile->pVolume, pData, (LONGLONG)read_cluster*clustersize, read_len*clustersize, NULL))
						return FAILURE(FALSE);
					pComp->cached_lcn = read_cluster;
					pComp->cached_clusters = read_len;
					pComp->cached_data = pData;
					bDecompress = TRUE;
				}

				got += read_len;
				if (got == 16)
					break;
				
				// next run
				rl_vcn += rl_len;
				rl=rl->Next;
				if(!rl)
					return FAILURE(FALSE);
				rl_cluster = rl->cluster;
				rl_len = rl->len;
				rl_offset = 0;
				pData += read_len*clustersize;
			} while (rl_cluster!=-1); // until empty run 

			chunk = 16*clustersize;
			if (got==16)
			{
				// uncompressible 
				assert(rl_cluster!=-1);
				pData = compressed_data;
			}
			else
			{
				if(bDecompress)
					NtfsDecompress(decompressed_data, compressed_data, chunk);
				pData = decompressed_data;
			}
			
			buffer_offset = (UINT)(offset - (LONGLONG)read_vcn*clustersize);
			chunk -= buffer_offset;
			chunk = min(dwBytesToRead, chunk);
			memcpy(pDestPtr,pData + buffer_offset, chunk);
		}
		
		dwBytesToRead -= chunk;
		dwBytesRead += chunk;
		pDestPtr += chunk;
		offset += chunk;
	}
	if (pdwBytesRead)
		*pdwBytesRead = dwBytesRead;
	if (!pdwBytesRead && dwBytesToReadSaved != dwBytesRead)
		return FALSE;
	return TRUE;
}

UINT NtfsVcnToLcnWLen(NTFS_RUN_LIST* rl,UINT vcn, UINT* plen)
{
	while(vcn>=rl->len)
	{
		vcn-=rl->len;
		rl=rl->Next;
		if(!rl)
			return -1;
	}
	if (plen)
		*plen = rl->len-vcn;
	return rl->cluster+vcn;
}

BOOL NtfsReadAttributeEx(NTFS_ATTR* pAttr, LONGLONG nPos, void* pData, DWORD nBytesToRead, DWORD* pnBytesRead)
{
	UINT Cluster,start,VCN,BufPos;
	DWORD uBytes = nBytesToRead; 
	DWORD nClustersToRead;
	void* SRBuffer;
	HNTFS pVolume;

	if (pnBytesRead)
		*pnBytesRead = 0;
	if (!pAttr)
		return FAILURE(FALSE);
	if (!nBytesToRead)
		return TRUE;

	pVolume = pAttr->pFile->pVolume;
	if(nPos>=pAttr->nDataLen)
		return 0;
	if(nPos+uBytes>pAttr->nDataLen)
		uBytes=(DWORD)(pAttr->nDataLen-nPos);
	if (pAttr->pData)
	{
		memcpy(pData, (LPBYTE)pAttr->pData+nPos, uBytes);
	}
	else
	{
		if (pAttr->ra->IsCompressed)
			return NtfsReadCompressed(pAttr, nPos, uBytes, pData, pnBytesRead);

		assert(pAttr->ra);
		assert(pAttr->ra->IsNonResident);
		if (!pAttr->ra)
			return FAILURE(FALSE);
		if (!pAttr->ra->IsNonResident)
			return FAILURE(FALSE);
		
		BufPos = 0;
		start = (UINT)nPos % pVolume->clustersize;
		nClustersToRead = (start + uBytes + (pVolume->clustersize-1)) / pVolume->clustersize;
		
		if (uBytes == nClustersToRead * pVolume->clustersize)
			SRBuffer = pData;
		else
			SRBuffer = NtfsAllocMem(nClustersToRead*pVolume->clustersize);
		VCN = (UINT)(nPos / pVolume->clustersize);
		while(nClustersToRead)
		{
			UINT len;
			Cluster=NtfsVcnToLcnWLen(pAttr->rl,VCN, &len);
			if (len > nClustersToRead)
				len = nClustersToRead;
			NtfsSeekReadVolume(pVolume, (LPBYTE)SRBuffer+BufPos, (LONGLONG)Cluster*pVolume->clustersize, len*pVolume->clustersize, NULL);
			VCN += len;
			nClustersToRead -= len;
			BufPos+=pVolume->clustersize*len;
		}
		if (SRBuffer != pData)
		{
			memcpy(pData, (LPBYTE)SRBuffer+start,uBytes);
			NtfsFreeMem(SRBuffer);
		}
	}
	if (pnBytesRead)
		*pnBytesRead = uBytes;
	if (!pnBytesRead && uBytes != nBytesToRead)
		return FALSE;
	return TRUE;
}

NTFS_ATTR* NtfsFindAttrEx(NTFS_ATTR_LIST* pAttrList, ATTRIB_TYPE AttrType, LPCWSTR szAttribName, UINT nAttribNameSize, VCN NextVCN)
{
	UINT i, nAttrCount;
	NTFS_ATTR* pDAttrs;
	if (!pAttrList)
		return FAILURE(NULL);
	pDAttrs = pAttrList->Attributes;
	nAttrCount = pAttrList->nAttrCount;
	for (i=0; i<nAttrCount; i++, pDAttrs++)
	{
		if (!pDAttrs->ra)
			continue;
		if ((ATTRIB_TYPE)pDAttrs->ra->AttrType != AttrType)
			continue;
		if (NextVCN && pDAttrs->ra->IsNonResident && pDAttrs->nra->LastVCN+1 != NextVCN)
			continue;
		if (nAttribNameSize != pDAttrs->ra->NameLength)
			continue;
		if (nAttribNameSize && szAttribName)
		{
			if (0 != _wcsnicmp(szAttribName, pDAttrs->pName, nAttribNameSize))
				continue;
		}
		return pDAttrs;
	}
	return NULL;
}

NTFS_ATTR* NtfsEnumAttributes(NTFS_FILE* pFile, NTFS_ATTR* pPrevAttr)
{
	NTFS_ATTR_LIST* pAttrList;
	NTFS_ATTR* pAttr;
	UINT i;
	if (!pFile)
		return FAILURE(NULL);
	pAttrList = pFile->pAttrList;
	if (!pAttrList)
		return FAILURE(NULL);
	if (!pAttrList->nAttrCount)
		return NULL;
	if (!pPrevAttr)
		return pAttrList->Attributes;
	for (pAttr = pAttrList->Attributes, i=0; i<pAttrList->nAttrCount; i++, pAttr++)
	{
		if (pPrevAttr == pAttr)
		{
			if (i+1 == pAttrList->nAttrCount) // last
				return NULL;
			return pAttr+1;
		}
	}
	return NULL;
}

NTFS_ATTR* NtfsFindAttr(NTFS_FILE* pFile, ATTRIB_TYPE AttrType, LPCWSTR szAttribName)
{
	if (!pFile)
		return FAILURE(NULL);
	return NtfsFindAttrEx(pFile->pAttrList, AttrType, szAttribName, szAttribName ? wcslen(szAttribName) : 0, 0);
}

BOOL NtfsReadAttribute(NTFS_FILE* pFile, ATTRIB_TYPE AttrType, LPCWSTR szAttribName, LONGLONG nPos, LPVOID pData, DWORD nBytesToRead, DWORD* pnBytesRead)
{
	NTFS_ATTR* pAttr;
	if (!pFile)
		return FAILURE(FALSE);
	pAttr = NtfsFindAttrEx(pFile->pAttrList, AttrType, szAttribName, szAttribName ? wcslen(szAttribName)*sizeof(WCHAR) : 0,  0);
	if (!pAttr)
		return FAILURE(FALSE);
	return NtfsReadAttributeEx(pAttr, nPos, pData, nBytesToRead, pnBytesRead);
}

BOOL NtfsReadFile(NTFS_FILE* pFile, LONGLONG Position, LPVOID lpBuffer, DWORD nBytesToRead, DWORD* pnBytesRead)
{
	return NtfsReadAttribute(pFile, AttribData, 0, Position, lpBuffer, nBytesToRead, pnBytesRead);
}

NTFS_ATTR* NtfsAddDA2List(NTFS_ATTR_LIST* pAttrList, NTFSResAttrib* ra, void* pData)
{
	NTFS_ATTR* pAttr;
	NTFS_ATTR* pDAttrs = pAttrList->Attributes;
	UINT nDAAllocated = pAttrList->nDAAllocated;
	UINT nAttrCount = pAttrList->nAttrCount;

	if (nDAAllocated <= nAttrCount)
	{
		nDAAllocated += 0x10;
		pDAttrs = (NTFS_ATTR*)NtfsReallocMem(pDAttrs, nDAAllocated*sizeof(NTFS_ATTR));
		if (!pDAttrs)
			return FAILURE(NULL);
		pAttrList->nDAAllocated = nDAAllocated;
		pAttrList->Attributes = pDAttrs;
	}
	pAttr = &pDAttrs[nAttrCount];
	pAttr->ra = ra;
	pAttr->pData = pData;

	pAttrList->nAttrCount++;
	return pAttr;
}

void* NtfsAllocAttrListMFTEntry(NTFS_ATTR_LIST* pAttrList, DWORD size)
{
	UINT nMFTRecordsAllocated = pAttrList->nMFTRecordsAllocated;
	UINT nMFTRecordsCount = pAttrList->nMFTRecordsCount;
	void** ppMftRecords = pAttrList->ppMFTRecords;
	void* pMftRecord;

	if (nMFTRecordsAllocated <= nMFTRecordsCount)
	{
		nMFTRecordsAllocated += 0x10;
		ppMftRecords = (MFT**)NtfsReallocMem(ppMftRecords, nMFTRecordsAllocated*sizeof(NTFS_ATTR));
		if (!ppMftRecords)
			return FAILURE(NULL);
		pAttrList->nMFTRecordsAllocated = nMFTRecordsAllocated;
		pAttrList->ppMFTRecords = ppMftRecords;
	}
	pMftRecord = ppMftRecords[nMFTRecordsCount] = NtfsAllocMem(size);
	if (!pMftRecord)
		return FAILURE(NULL);
	pAttrList->nMFTRecordsCount++;
	return pMftRecord;
}


BOOL NtfsDecodeAttribute(NTFS_FILE* pFile, NTFSResAttrib* ra)
{
	NTFS_ATTR* pAttr;
	NTFS_ATTR_LIST* pAttrList;

	if (!pFile)
		return FAILURE(FALSE);
	pAttrList = pFile->pAttrList;
	pAttr = NtfsAddDA2List(pAttrList, ra, NULL);;

	if (!pAttr)
		return FAILURE(FALSE);
	pAttr->pFile = pFile;
	if (ra->NameLength)
		pAttr->pName = (PWCHAR)((LPBYTE)ra + ra->Offset);
	if (ra->IsNonResident)
	{
		NTFSNonResAttrib* nra = (NTFSNonResAttrib*)ra;
		//if (!ReadNonResidentAttr(pVolume, pAttr->nra, &pAttr->pData, &pAttr->nDataLen))
		//	return FAILURE(FALSE);
		LPBYTE pPackedRun = (LPBYTE)nra + nra->OffsetToData;
		pAttr->rl = NtfsMakeRunList((LPBYTE)nra+nra->OffsetToData, nra->StartVCN, nra->LastVCN, NULL, nra->IsCompressed);
		pAttr->nDataLen = ((NTFSNonResAttrib*)ra)->LengthInitData;
	}
	else
	{
		pAttr->pData = (LPBYTE)ra + ra->OffsetToData;
		pAttr->nDataLen = ra->DataLength;
	}
#ifdef _ATTR_TRACE
	printf("Attr:%-*s ID:%x Res:%d Len:%I64d Name:%*.*S \n", 20, NtfsGetAttrName(ra->AttrType), ra->AttributeID, ra->IsNonResident ? 0 : 1, pAttr->nDataLen, ra->NameLength, ra->NameLength, pAttr->pName);
#endif
	return TRUE;
}

BOOL NtfsDecodeAttributesMft(NTFS_FILE* pFile, MFT* smft)
{
	NTFSResAttrib* ra=NULL;
	BOOL bFoundList=FALSE,bAttribListNotResident=FALSE;
	MFT* pmft=smft;
	int iData=0;
	LPBYTE lpTmpData=NULL;
	DWORD Attrlen;	
	NTFS_ATTR_LIST* pAttrList;

	if (!pFile)
		return FAILURE(FALSE);
	pAttrList = pFile->pAttrList;
	
	if((smft->Flags & MFT_RECORD_IN_USE) == 0)
		return FAILURE(FALSE);

	if(smft->Flags & MFT_RECORD_IS_DIRECTORY)
		pFile->bDir = TRUE;
	
	if(smft->OffsetToAttributes==0)
		return FAILURE(FALSE);

	ra=(NTFSResAttrib*)&((LPBYTE)smft)[smft->OffsetToAttributes];
	Attrlen=smft->OffsetToAttributes;
	while(ra->AttrType!=AttribNoMoreAttrib)
	{
		NtfsDecodeAttribute(pFile, ra);
		Attrlen += ra->Length;
		if(Attrlen>pFile->pVolume->mft_recordsize)
			break;
		ra=(NTFSResAttrib*)&((LPBYTE)ra)[ra->Length];
	}
	return TRUE;
}

BOOL NtfsDecodeAttributesLCN(NTFS_FILE* pFile, LCN LCN)
{
	NTFS_ATTR_LIST* pAttrList;
	BOOL bRes = FALSE;
	MFT* pMftRec;
	HNTFS pVolume;
	DWORD mft_record_size;
	DWORD mft_cluster_size;
	if (!pFile)
		return FAILURE(FALSE);
	pAttrList = pFile->pAttrList;
	pVolume = pFile->pVolume;
	mft_cluster_size = pVolume->clustersize;
	mft_record_size = pVolume->mft_recordsize;
	pMftRec = (MFT*)NtfsAllocAttrListMFTEntry(pAttrList, mft_record_size);
	if(!pMftRec)
		return FAILURE(FALSE);
	if (!NtfsSeekReadVolume(pVolume, pMftRec, LCN*mft_cluster_size, mft_record_size, NULL))
		return FAILURE(FALSE);
	if (!NtfsFixupRecord(pVolume,(LPBYTE)pMftRec, MFT_MAGIC_FILE, mft_record_size))
		return FAILURE(FALSE);
	return NtfsDecodeAttributesMft(pFile, pMftRec);
}

BOOL NtfsDecodeAttributesMftNr(NTFS_FILE* pFile, MFT_REF MftRef)
{
	NTFS_ATTR_LIST* pAttrList;
	BOOL bRes = FALSE;
	MFT* pMftRec;
	HNTFS pVolume;
	DWORD mft_record_size;

	if (!MftRef) // MFT itself
		return NtfsDecodeAttributesLCN(pFile, pFile->pVolume->mft_cluster);

	if (!pFile)
		return FAILURE(FALSE);
	pAttrList = pFile->pAttrList;
	pVolume = pFile->pVolume;
	mft_record_size = pVolume->mft_recordsize;
	
	pMftRec = (MFT*)NtfsAllocAttrListMFTEntry(pAttrList, mft_record_size);
	if(!pMftRec)
		return FAILURE(FALSE);
	if (!NtfsReadFile(pVolume->pMFT, MFT_REC_NUMBER(MftRef)*mft_record_size, pMftRec, mft_record_size, NULL))
		return FAILURE(FALSE);
	if (!NtfsFixupRecord(pVolume,(LPBYTE)pMftRec, MFT_MAGIC_FILE, mft_record_size))
		return FAILURE(FALSE);
	return NtfsDecodeAttributesMft(pFile, pMftRec);
}

BOOL NtfsAttachExtent(NTFSAttribList* la, NTFS_ATTR* pAttr, NTFS_ATTR_LIST* pExtentAttrList)
{
	NTFS_ATTR* pExtentDAttr;
	NTFSNonResAttrib* extent_nra;
	NTFSNonResAttrib* base_nra;
	UINT i;

	if (!pAttr)
		return FAILURE(FALSE);
	base_nra = pAttr->nra;
	if (!base_nra)
		return FAILURE(FALSE);

	if (!pExtentAttrList)
		return FAILURE(FALSE);
	if (!pExtentAttrList->Attributes)
		return FAILURE(FALSE);
	for (i=0, pExtentDAttr=pExtentAttrList->Attributes; i<pExtentAttrList->nAttrCount; i++, pExtentDAttr++)
	{
		if (!pExtentDAttr->nra)
			continue;
		break;
	}
	if (!pExtentDAttr)
		return FAILURE(FALSE);
	if (i != pExtentAttrList->nAttrCount-1)
		return FAILURE(FALSE);
	extent_nra = pExtentDAttr->nra;
	if (!extent_nra)
		return FAILURE(FALSE);

	if (extent_nra->StartVCN != la->StartVCN)
		return FAILURE(FALSE);
	if (extent_nra->StartVCN != base_nra->LastVCN + 1)
		return FAILURE(FALSE);
	if (!pExtentDAttr->rl)
		return FAILURE(FALSE);

	// move extent runlist to base attribute
	if (!pAttr->rl)
		pAttr->rl = pExtentDAttr->rl;
	else
	{
		NTFS_RUN_LIST* rl = pAttr->rl;
		while (rl->Next)
			rl = rl->Next;
		rl->Next = pExtentDAttr->rl;
	}
	pExtentDAttr->rl = 0; // moved to base NTFS_RUN_LIST
	base_nra->LastVCN = extent_nra->LastVCN;
	return TRUE;
}

BOOL NtfsDecodeAttributeListItem(NTFS_FILE* pFile, NTFSAttribList* la)
{
	NTFS_ATTR* pAttr;
	BOOL bRes = FALSE;
	NTFS_FILE* pFileExtent;
	pAttr = NtfsFindAttrEx(pFile->pAttrList, la->AttrType, la->AttrName, la->SizeOfAttrName, la->StartVCN);
	if (!pAttr)
		return NtfsDecodeAttributesMftNr(pFile, la->MFTRecord);

	pFileExtent = NtfsOpenFile(pFile->pVolume, la->MFTRecord);
	if (!pFileExtent)
		return FAILURE(FALSE);
	bRes = NtfsAttachExtent(la, pAttr, pFileExtent->pAttrList);
	NtfsCloseFile(pFileExtent);
	return bRes;

}

void* NtfsGetAttrData(NTFS_ATTR* pAttr, DWORD nLimitSize, DWORD* pnSize)
{
	void* pData;
	DWORD dwDataLen;
	LONGLONG llLimitSize = MAX_LOAD_DATA_LEN;
	if (pnSize)
		*pnSize = 0; 
	if (!pAttr)
		return FAILURE(NULL);
	if (!pAttr->ra)
		return FAILURE(NULL);
	dwDataLen = (DWORD)pAttr->nDataLen;
	if (pAttr->pData)
	{
		if (pnSize)
			*pnSize = dwDataLen; 
		return pAttr->pData;
	}
	if (nLimitSize)
		llLimitSize = nLimitSize;
	if (pAttr->nDataLen > llLimitSize)
		return FAILURE(NULL);
	pData = NtfsAllocMem(dwDataLen);
	if (!pData)
		return FAILURE(NULL);
	if (!NtfsReadAttributeEx(pAttr, 0, pData, dwDataLen, NULL))
	{
		NtfsFreeMem(pData);
		return FAILURE(NULL);
	}
	pAttr->pData = pData;
	if (pnSize)
		*pnSize = dwDataLen; 
	return pAttr->pData;
}


BOOL NtfsDecodeAttributes(NTFS_FILE* pFile, MFT_REF MftRef)
{
	NTFS_ATTR_LIST* pAttrList;
	NTFS_ATTR* pAttr;
	UINT i;
	pAttrList = (NTFS_ATTR_LIST*)NtfsAllocMem(sizeof(NTFS_ATTR_LIST));
	if (!pAttrList)
		return FAILURE(FALSE);
	pFile->pAttrList = pAttrList;
	if (!NtfsDecodeAttributesMftNr(pFile, MftRef))
	{
		pFile->pAttrList = NULL;
		NtfsFreeAttrList(pAttrList);
		return FAILURE(FALSE);
	}

	// load all extents
	pAttr = pAttrList->Attributes;
	for (i=0; i<pAttrList->nAttrCount; i++, pAttr++)
	{
		if (!pAttr->ra)
			continue;
		if(pAttr->ra->AttrType==AttribAttribList)
		{
			NTFSAttribList* al;
			DWORD len;
			MFT_REF nLastMFTRecNr = MftRef;
			al = (NTFSAttribList*)NtfsGetAttrData(pAttr, 0, &len);
			if (!al)
				return FAILURE(FALSE);
			while (len>=6 && al->AttrType!=AttribNoMoreAttrib)
			{
				// 02.06.2007 Mike - sometimes AttribList contain single record of zeros
				if (0 == al->SizeOfRecord)
					break;
#ifdef _ATTR_TRACE
				printf("ListAttr:%s Name:%*.*S MFT:%x VCN:%I64x\n", NtfsGetAttrName(al->AttrType), al->SizeOfAttrName, al->SizeOfAttrName, al->AttrName, al->MFTRecord, al->StartVCN);
#endif
				if (MFT_REC_NUMBER(nLastMFTRecNr) != MFT_REC_NUMBER(al->MFTRecord) 
					&& MFT_REC_NUMBER(MftRef) != MFT_REC_NUMBER(al->MFTRecord))
				{
					if (!NtfsDecodeAttributeListItem(pFile, al))
						return FAILURE(FALSE);
					nLastMFTRecNr = al->MFTRecord;
				}
				len -= al->SizeOfRecord;
				(LPBYTE)al += al->SizeOfRecord;
			};
		}
	}

	return TRUE;
}



BOOL NtfsInitDir(NTFS_FILE* pFile)
{
	NTFS_ATTR* pAttrIndexRoot;

	if (!pFile->bDir)
		return FAILURE(FALSE);

	pAttrIndexRoot = NtfsFindAttr(pFile, AttribIndexRoot, $I30);
	if (!pAttrIndexRoot)
	{
		if (NtfsFindAttr(pFile, AttribIndexAllocation, $I30))
		{
			// directory extent
			pFile->bDir = FALSE;
			return TRUE;
		}
		return FAILURE(FALSE);
	}
	pFile->pIndexRoot = (NTFS_INDEX_ROOT*)NtfsGetAttrData(pAttrIndexRoot, 0, NULL);
	if (!pFile->pIndexRoot)
		return FAILURE(FALSE);

	if (pFile->pIndexRoot->IndexHeader.nFlags & LARGE_INDEX)
	{
		pFile->pAttrIndexAllocation = NtfsFindAttr(pFile, AttribIndexAllocation, $I30);
		if (!pFile->pAttrIndexAllocation)
			return FAILURE(FALSE);
		if (!NtfsGetAttrData(pFile->pAttrIndexAllocation, 0, NULL))
			return FAILURE(FALSE);
		{ 
			DWORD dwOffset = 0;
			DWORD dwSizeOfIndexRecord = pFile->pIndexRoot->dwSizeOfIndexRecord;
			while (dwOffset + dwSizeOfIndexRecord <= pFile->pAttrIndexAllocation->nDataLen)
			{
				if(!NtfsFixupRecord(pFile->pVolume,(LPBYTE)pFile->pAttrIndexAllocation->pData+dwOffset, MFT_MAGIC_INDX, dwSizeOfIndexRecord))
				{
					OUT_DEBUG("INDX record got a bad Magic sign or FIXUP record\n");
					return FAILURE(FALSE);
				}
				dwOffset += dwSizeOfIndexRecord;
			}
		}
/*
		if (pFile->pAttrBitmap = NtfsFindAttr(pFile, AttribBitmap, $I30))
		{
			if (!NtfsGetAttrData(pFile, pFile->pAttrBitmap, 0, NULL))
				return FAILURE(FALSE);
		}
*/
	}

	return TRUE;
}

void NtfsCloseFile(NTFS_FILE* pFile)
{
	if (!pFile)
		return;
	if (pFile->pVolume->pMFT == pFile)
		pFile->pVolume->pMFT = NULL;
	NtfsFreeAttrList(pFile->pAttrList);
	if (pFile->ppIndexEntries)
		NtfsFreeMem(pFile->ppIndexEntries);
	NtfsFreeMem(pFile);
}

NTFS_FILE* NtfsOpenFile(HNTFS pVolume, MFT_REF MftRef)
{
	NTFS_FILE* pFile = (NTFS_FILE*)NtfsAllocMem(sizeof(NTFS_FILE));
	if (!pFile)
		return FAILURE(NULL);
	pFile->pVolume = pVolume;
	pFile->nMftRec = MftRef;	
	if (0 == MftRef && !pVolume->pMFT) // open MFT itself
		pVolume->pMFT = pFile;
	if (!NtfsDecodeAttributes(pFile, MftRef))
	{
		NtfsCloseFile(pFile);
		return FAILURE(NULL);
	}
	if (pFile->bDir)
	{
		if (!NtfsInitDir(pFile))
		{
			NtfsCloseFile(pFile);
			return FAILURE(NULL);
		}
	}
	return pFile;
}

BOOL NtfsFlushVolume(CHAR cDrive)
{
	HANDLE hDrive;
	CHAR LogDisk[10] = {'\\', '\\', '.', '\\', 0, ':', 0};
	LogDisk[4] = cDrive;
	hDrive=CreateFile(LogDisk,GENERIC_READ|GENERIC_WRITE,FILE_SHARE_READ|FILE_SHARE_WRITE,NULL,OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL|FILE_FLAG_NO_BUFFERING,NULL);
	if(hDrive==INVALID_HANDLE_VALUE)
		return FALSE;
	FlushFileBuffers(hDrive);
	CloseHandle(hDrive);
	return TRUE;
}

HANDLE NtfsOpenLogicalDrive(CHAR cDrive, BOOL bFlush)
{
	HANDLE hDrive;
	char LogDisk[10] = {'\\', '\\', '.', '\\', 0, ':', 0};
	LogDisk[4] = cDrive;
	if (bFlush)
		NtfsFlushVolume(cDrive);
	hDrive=CreateFile(LogDisk,GENERIC_READ,FILE_SHARE_READ|FILE_SHARE_WRITE,NULL,OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL|FILE_FLAG_NO_BUFFERING,NULL);
	return hDrive;
}

void NtfsClose(HNTFS pVolume)
{
	if (!pVolume)
		return;
	NtfsCloseFile(pVolume->pMFT);
	if (pVolume->hDrive != INVALID_HANDLE_VALUE)
		CloseHandle(pVolume->hDrive);
#if defined(USE_LL_DISK_IO)
	if (pVolume->llio)
		LLDiskIODone(pVolume->llio);
#endif
	NtfsFreeMem(pVolume);
}

HNTFS NtfsOpen(LPCWSTR wcsDriveName)
{
	NTFSBOOT boot;
	HANDLE hDrive = INVALID_HANDLE_VALUE;
	WCHAR wcDrive[2] = {0};
	BOOL bLogicalDrive = FALSE;
	HNTFS pVolume = NtfsAllocMem(sizeof(NTFS));
	if (!pVolume)
		return FAILURE(NULL);
	
#if defined(USE_LL_DISK_IO)
	pVolume->llio = LLDiskIOInit(wcsDriveName);
	if (!pVolume->llio)
#endif
	pVolume->hDrive = NtfsOpenLogicalDrive((CHAR)wcsDriveName[0], FALSE);

	if (NtfsSeekReadVolume(pVolume, &boot, 0, sizeof(boot), NULL))
	{
		if(IS_MAGIC(&boot.oem, MFT_MAGIC_NTFS))
		{
			NtfsInitVolume(pVolume, &boot);
			pVolume->pMFT = NtfsOpenFile(pVolume, 0);
			if (pVolume->pMFT)
				return pVolume;
		}
	}
	NtfsClose(pVolume);
	return FAILURE(NULL);
}

/*
void main( int argc, char *argv[])
{
const char Header[]="NTFS File Dumper ver 2.13.1 (C)1998-2000 Andy Nikishin (Kaspersky Lab)\nInternal Use Only\n";
const char Usage[]="Usage: %s [DriveLetter] switches\n"\
"\t-dump:DriveLetter -- Dump all MFT records for \"DriveLetter\"\n\t-full -- print full info\n\t-rec:XXXX -- Select MFT Record\n\t-dir:Path -- Get Directory List\n\t-cf:FileName -- Copy File \"FileName\"\n\t-ca:XX:<AttribName>:FileName -- Copy attribute XX (MUST be in HEX)\n\t\tnamed \
\"AttribName\" for \"FileName\"\n\t-cb -- Check Bad Clusters\n";
const char szAttribList[]="\nList of Attributes:\n10\tStandart Information\n20\tAttribute list\n30\tFile name\n40\tVolume Version\n50\tSecurity Descriptor\n60\tVolume Name\n70\tVolume Info\n80\tData\n90\tIndex Root\nA0\tIndex Allocation\nB0\tBitmap\nC0\tSymlink\nD0\tHPDS EA Info\nE0\tHPFS EA";
const char szPressAny[]="Press any key to continue\n";
int Mode=0;
LONG pos=0;
MFT* sMFT;
UINT i,uRecordPos=0;
HNTFS pVolume;
int drive=0;
//HDDPARAM HDDP;
//HDDINFO HDDI;
//UINT HDrive;
BOOL bFullInfo=FALSE;
HANDLE hDrive = INVALID_HANDLE_VALUE;

  TotalDirs=0;
  TotalDels=0;
  total=0;
  
	printf(Header);
	if( argc < 2)
	{
	printf(Usage, argv[0] );
	return ;
	}
	else if( argv[1][0] == '/' ) 
	{
	printf(Usage, argv[0] );
	printf(szPressAny);
	getch();
	printf(szAttribList);
	return ;
	}
	
	  drive=(argv[1][0]&~0x20)-'A';
	  if((WORD)drive>'z'-'a')
	  drive=0;
	  
		for(i=1;i<(UINT)argc;i++)
		{
		switch(argv[i][1])
		{
		case 'h':
		printf(Usage, argv[0] );
		printf(szAttribList);
		return ;
		//			case 'f':
		//				if(lstrcmp((LPSTR)argv[i],"-full")==0)
		//					bFullInfo=TRUE;
		//				break;
		//			case 'c':
		//				if(strnicmp(argv[i],"-cf:",4)==0)
		//				{
		//					lstrcpy(szDirPath,&argv[i][4]);
		//					drive=(szDirPath[0]&~0x20)-'A';
		//					dwCopyAttrib=0x80;
		//					szAttribName[0]=0;
		//					Mode=COPY_ATTRIB;
		//				}
		//				else if(strnicmp(argv[i],"-ca:",4)==0)
		//				{
		//				char szAttr[16];
		//				int ii;
		//					szAttribName[0]=0;
		//					for(ii=0;ii<4;ii++)
		//					{
		//						szAttr[ii]=argv[i][4+ii];
		//						if(szAttr[ii]==':' || szAttr[ii]==0)
		//						{
		//							szAttr[ii]=0;
		//							break;
		//						}
		//					}
		//					drive=ii+1;
		//					for(ii=drive;ii<drive+10;ii++)
		//					{
		//						szAttribName[ii-drive]=argv[i][4+ii];
		//						if(szAttribName[ii-drive]==':' || szAttribName[ii-drive]==0)
		//						{
		//							szAttribName[ii-drive]=0;
		//							break;
		//						}
		//					}
		//					drive=0;
		//					dwCopyAttrib=htoi(szAttr);
		//					lstrcpy(szDirPath,&argv[i][ii+1+4]);
		//					drive=(szDirPath[0]&~0x20)-'A';
		//					Mode=COPY_ATTRIB;
		//				}
		//				else if(strnicmp(argv[i],"-cb",4)==0)
		//				{
		//					Mode=CHECK_BAD;
		//				}
		//				break;
		case 'd':
		if(strnicmp(argv[i],"-dump",5)==0)
		{
		WORD drive1=0;
		drive1=(argv[i][6]&~0x20)-'A';
		if(drive1<'z'-'a')
		drive=drive1;
		Mode=DUMP_MODE;
		}
		//				if(strnicmp(argv[i],"-dir:",5)==0)
		//				{
		//					Mode=DIR_MODE;
		//					lstrcpy(szDirPath,&argv[i][5]);
		//					drive=(szDirPath[0]&~0x20)-'A';
		//				}
		break;
		//			case 'r':
		//				uRecordPos=atol(&argv[i][5]);
		//				Mode=RECORD_DUMP_MODE;
		//				break;
		}
		}
		if(Mode==0)
		{
		printf(Usage, argv[0] );
		return ;
		}
		if(drive==0 || (WORD)drive > 'z'-'a')
		{
		printf("Error! Drive not defined\n");
		return ;
		}
		
		  printf("\nDrive %c ",drive+'A');
		  
			//	if(!IninRwDll())
			//		return;
			//	StartSector=FillHDDInfoLog((BYTE)drive,&HDrive);
			//	GetHDDDriveType((BYTE)HDrive ,&HDDI,&HDDP);
			hDrive = NtfsOpenLogicalDrive((BYTE)drive);
			if (hDrive == INVALID_HANDLE_VALUE)
			{
			printf("Cannot open logival drive %C:\n", 'A'+drive);
			return;
			}
			
			  if(!(pVolume=NtfsOpen(hDrive, Mode==DUMP_MODE || Mode==RECORD_DUMP_MODE)))
			  {
			  printf("Error Open Drive\n");
			  //		DetachRwDll();
			  return;
			  }
			  if(Mode==RECORD_DUMP_MODE)
			  {
			  SeekAndReadNTFS(GetMFTHandle(pVolume), uRecordPos*GetMFTRecordSize(pVolume),Buffer,GetMFTRecordSize(pVolume));
			  sMFT=(MFT*)Buffer;
			  printf("MFT record# %d\n",uRecordPos);
			  if(!NtfsFixupRecord(pVolume,Buffer, MFT_MAGIC_FILE, pVolume->mft_recordsize))
			  printf("MFT record# %d got a bad Magic sign or FIXUP record\n",uRecordPos);
			  //		else
			  //			PrintMFTRecord(sMFT,uRecordPos,bFullInfo,pVolume);
			  }
			  else 
			  if(Mode==DUMP_MODE)
			  {
			  #define RECORD_PER_READ 1
			  BYTE Buffer[1024*RECORD_PER_READ];
			  DWORD record_size = GetMFTRecordSize(pVolume);
			  HNTFSFile hMFT = GetMFTHandle(pVolume);
			  int nRecordToRead = 0;
			  for(i=0;i<pVolume->TotalRecordOfMFT;i+=nRecordToRead)
			  {
			  int j;
			  nRecordToRead = min(pVolume->TotalRecordOfMFT-i, RECORD_PER_READ);
			  SeekAndReadNTFS(hMFT, i*record_size,Buffer,nRecordToRead*record_size);
			  for(j=0;j<nRecordToRead;j++)
			  {
			  sMFT=(MFT*)(Buffer+j*record_size);
			  //				printf("MFT record# %d\n",i);
			  if(!NtfsFixupRecord(pVolume, (LPBYTE)sMFT, MFT_MAGIC_FILE, record_size))
			  {
			  char* r = (char*)sMFT;
			  printf("MFT record# %d got a bad Magic sign or FIXUP record\n++++++++++++\n\n",i+j);
			  //					continue;
			  }
			  if ((sMFT->Flags & 1) == 0)
			  TotalDels++;
			  else if (sMFT->Flags & 2)
			  TotalDirs++;
			  else
			  total++;
			  
				PrintMFTRecord(sMFT,i+j,bFullInfo,pVolume);
				}
				}
				printf("Total Directories on Drive %d\n",TotalDirs);
				printf("Total Deleted Entries on Drive %d\n",TotalDels);
				printf("Total Files on Drive %d\n",total);
				}
				else 
				//	if(Mode==DIR_MODE)	//Get Dir list
				//	{
				//	DWORD MFTRecordNumber=5;		//Start from Root dir
				//	NTFSDirNode dn;
				//
				//		MFTRecordNumber=FindFile(szDirPath);
				//		if(MFTRecordNumber==0xffffffff)
				//		{
				//			OUT_DEBUG1("File not found %s\n",szDirPath);
				//			goto loc_exit;
				//		}
				//		ZeroMemory(&dn,sizeof(dn));
				//		if(!NtfsInitDirNode(&dn,pVolume,MFTRecordNumber))
				//		{
				//			OUT_DEBUG1("error opening %s\n",szDirPath);
				//			goto loc_exit;
				////			break;
				//		}
				
				  //
				  //	char *szNextPath;
				  //	DWORD MFTRecordNumber=5;		//Start from Root dir
				  //	NTFSDirNode dn;
				  //		szDirPath+=3;
				  //		printf("\nDirectory list\n");
				  //		ZeroMemory(&dn,sizeof(NTFSDirNode));
				  //		do{
				  //			NtfsFreeDirNode(&dn);
				  //			if(!NtfsInitDirNode(&dn,pVolume,MFTRecordNumber))
				  //			{
				  //				OUT_DEBUG1("error opening %s\n",szDirPath);
				  //				break;
				  //			}
				  //
				  //			if(!szDirPath || !*szDirPath)
				  //				break;
				  //			szNextPath=strchr(szDirPath,'\\');
				  //			if(szNextPath)
				  //			{
				  //				*szNextPath='\0';
				  //				szNextPath++;
				  //			}
				  //			MFTRecordNumber=(DWORD)NtfsFindFile(&dn,szDirPath);
				  //			if(MFTRecordNumber==-1)
				  //			{
				  //				printf("%s not found\n",szDirPath);
				  //				return ;
				  //			}
				  //			szDirPath=szNextPath;
				  //		}while(1);
				  //
				  
					
					  //		NtfsPrintDir(&dn);
					  //		NtfsFreeDirNode(&dn);
					  //
					  ////		RecurceNFTSDirs(5);
					  //	}
					  //	else if(Mode==COPY_ATTRIB)
					  //	{
					  //	DWORD MFTNr;
					  //	HFILE hf;
					  //	char* lp=szDirPath;
					  //
					  //		lp=strrchr(szDirPath,'\\');
					  //		lp=lp?(lp+1):szDirPath;
					  //
					  //		printf("\nCopy Attribute Mode\n");
					  //
					  //		MFTNr=FindFile(szDirPath);
					  //		if(MFTNr==0xffffffff)
					  //		{
					  //			OUT_DEBUG1("File not found %s\n",szDirPath);
					  //			goto loc_exit;
					  //		}
					  //		SeekAndReadNTFS(GetMFTHandle(pVolume), MFTNr*GetMFTRecordSize(pVolume),Buffer,GetMFTRecordSize(pVolume));
					  //		sMFT=(MFT*)Buffer;
					  //		printf("MFT record# %d\n",MFTNr);
					  //		GetCurrentDirectory(sizeof(szDestPath),szDestPath);
					  //		if(szDestPath[lstrlen(szDestPath)-1]!='\\')
					  //			lstrcat(szDestPath,"\\");
					  //		wsprintf(&szDestPath[lstrlen(szDestPath)],"%s.a%X",lp,dwCopyAttrib);
					  //		hf=_lcreat(szDestPath,0);
					  //		if(!NtfsFixupRecord(pVolume,Buffer, MFT_MAGIC_FILE, pVolume->mft_recordsize))
					  //			printf("MFT record# %d got a bad Magic sign or FIXUP record\n",uRecordPos);
					  //		else
					  //			CopyAttribute(pVolume,sMFT,dwCopyAttrib,szAttribName,hf);
					  //		_lclose(hf);
					  //	}
					  //	else if(Mode==CHECK_BAD)
					  //	{
					  //		SeekAndReadNTFS(GetMFTHandle(pVolume), 8*GetMFTRecordSize(pVolume),Buffer,GetMFTRecordSize(pVolume));
					  //		sMFT=(MFT*)Buffer;
					  //		if(!NtfsFixupRecord(pVolume,Buffer, MFT_MAGIC_FILE, pVolume->mft_recordsize))
					  //			printf("MFT record# %d got a bad Magic sign or FIXUP record\n",uRecordPos);
					  //		else
					  //			CheckBadClusters(pVolume,sMFT);
					  //	}
					  //loc_exit:
					  ;
					  //	DetachRwDll();
					  }
*/

/*
NTFS_INDEX_ENTRY* NtfsEnumIndexEntries(INDEX_HEADER* ih, NTFS_INDEX_ENTRY** ppLastEntry)
{
	LPBYTE pEntriesStart = (LPBYTE)ih + ih->dwEntriesOffset;
	LPBYTE pEntriesEnd = (LPBYTE)ih + ih->dwIndexLen;
	NTFS_INDEX_ENTRY* pLastEntry = *ppLastEntry;
	NTFS_INDEX_ENTRY* pNextEntry;
	if (pLastEntry)
	{
		if ((LPBYTE)pLastEntry < pEntriesStart) 
			return NULL;
		if ((LPBYTE)pLastEntry >= pEntriesEnd)
			return NULL;
		pNextEntry = (NTFS_INDEX_ENTRY*)((LPBYTE)pLastEntry + pLastEntry->Header.EntrySize);
		*ppLastEntry = NULL;
	}
	else
	{
		pNextEntry = (NTFS_INDEX_ENTRY*)pEntriesStart;
	}

	if ((LPBYTE)pNextEntry + sizeof(NTFS_INDEX_ENTRY) > pEntriesEnd)
		return NULL;
	if(pNextEntry->Header.EntrySize==0)
		return NULL;
	if(pNextEntry->Header.Flags & 2)
		return NULL;
	if(pNextEntry->Header.EntrySize<=0x18)
		return NULL;
	return pNextEntry;
}

NTFS_INDEX_ENTRY* NtfsEnumDir(NTFS_FILE* pDir, NTFS_INDEX_ENTRY* pLastEntry)
{
	NTFS_INDEX_ENTRY* pNextEntry;
	NTFS_INDEX_ROOT* ir;
	DWORD block,BlockPos;
	LPBYTE pBitmap = NULL;
	DWORD  nIndexBlockSize;

	if (!pDir->bDir)
		return NULL;
	ir = pDir->pIndexRoot;
	if (!ir)
		return NULL;

	// look into index root
	if (pNextEntry = NtfsEnumIndexEntries(&pDir->pIndexRoot->IndexHeader, &pLastEntry))
		return pNextEntry;

	if (!pDir->pIndexAllocation) // only IndexRoot present
		return NULL;

	if (pDir->pAttrBitmap)
		pBitmap = (LPBYTE)pDir->pAttrBitmap->pData;
	nIndexBlockSize = pDir->pIndexRoot->dwClusterPerIndexBlock*pDir->pVolume->clustersize;

	for (block=0, BlockPos=0; BlockPos < (DWORD)pDir->pAttrIndexAllocation->nDataLen;block++,BlockPos+=nIndexBlockSize)
	{
		NTFS_INDEX* id;
		if (pBitmap)
		{
			UINT bit  = 1 << (block & 7);
			UINT byte = block / 8;
			if (0 == (pBitmap[byte] & bit))
				continue;
		}

		id=(NTFS_INDEX*)&((LPBYTE)(pDir->pIndexAllocation))[BlockPos];
		if (pNextEntry = NtfsEnumIndexEntries(&id->IndexHeader, &pLastEntry))
			return pNextEntry;
	}
	return NULL;
}
*/
/*
BOOL NtfsEnumIndexEntriesCB(INDEX_HEADER* ih, void* pEnumContext, NTFS_ENUM_DIR_PROC fEnumProc)
{
	DWORD dwEntryOffset;
	NTFS_INDEX_ENTRY* ie;
	for (dwEntryOffset = ih->dwEntriesOffset; dwEntryOffset + sizeof(INDEX_HEADER) <= ih->dwIndexLen; dwEntryOffset += ie->Header.EntrySize)
	{
		ie = (NTFS_INDEX_ENTRY*)((LPBYTE)ih + dwEntryOffset);
		if(ie->Header.EntrySize==0)
			break;
		if(ie->Header.Flags & 1)
		{
			VCN next_vcn =  *(VCN*)((LPBYTE)ie + ie->Header.EntrySize - sizeof(VCN));
			printf("ie->Header.Flags = %d size=%d %I64d\n", ie->Header.Flags, ie->Header.EntrySize, next_vcn);
		}
		if(ie->Header.Flags & 2)
		{
			break;
		}
		if(ie->Header.EntrySize<=sizeof(INDEX_HEADER))
			break;
		if (!fEnumProc(pEnumContext, ie))
			return FALSE;
	}
	return TRUE;
}
*/
BOOL NtfsEnumIndexEntriesCB(NTFS_FILE* pDir, INDEX_HEADER* ih, void* pEnumContext, NTFS_ENUM_DIR_PROC fEnumProc)
{
	DWORD dwEntryOffset;
	NTFS_INDEX_ENTRY* ie;
	for (dwEntryOffset = ih->dwEntriesOffset; dwEntryOffset + sizeof(NTFS_INDEX_ENTRY_HEADER) <= ih->dwIndexLen; dwEntryOffset += ie->Header.EntrySize)
	{
		ie = (NTFS_INDEX_ENTRY*)((LPBYTE)ih + dwEntryOffset);
		if(ie->Header.EntrySize<=sizeof(NTFS_INDEX_ENTRY_HEADER))
			break;
		if(ie->Header.Flags & INDEX_ENTRY_NODE)
		{
			VCN next_vcn =  *(VCN*)((LPBYTE)ie + ie->Header.EntrySize - sizeof(VCN));
			LONGLONG IndexPos = next_vcn * pDir->pVolume->clustersize;
			_ASSERT(IndexPos + sizeof(NTFS_INDEX) < pDir->pAttrIndexAllocation->nDataLen);
			_ASSERT(pDir->pAttrIndexAllocation->pData);
			if (pDir->pAttrIndexAllocation->pData && IndexPos + sizeof(NTFS_INDEX) < pDir->pAttrIndexAllocation->nDataLen)
			{
				NTFS_INDEX* id=(NTFS_INDEX*)((LPBYTE)pDir->pAttrIndexAllocation->pData + IndexPos);
				if (!NtfsEnumIndexEntriesCB(pDir, &id->IndexHeader, pEnumContext, fEnumProc))
					return FALSE;
			}
		}
		if(ie->Header.Flags & INDEX_ENTRY_END)
		{
			break;
		}
		if (!fEnumProc(pEnumContext, ie))
			return FALSE;
	}
	return TRUE;
}

BOOL NtfsEnumDirCB(NTFS_FILE* pDir, void* pEnumContext, NTFS_ENUM_DIR_PROC fEnumProc)
{
	NTFS_INDEX_ROOT* ir;
	if (!pDir->bDir)
		return FALSE;
	ir = pDir->pIndexRoot;
	if (!ir)
		return FALSE;
	// start with index root
	return NtfsEnumIndexEntriesCB(pDir, &pDir->pIndexRoot->IndexHeader, pEnumContext, fEnumProc);
}

typedef struct tag_NTFS_FINDFILE_CONTEXT
{
	LPCWSTR  szFileName;
	UINT     nNameLen;
	MFT_REF  MftRef;
} NTFS_FINDFILE_CONTEXT;

BOOL __cdecl NtfsFindFileProc(void* pEnumContext, NTFS_INDEX_ENTRY* ie)
{
	NTFS_FINDFILE_CONTEXT* pFindContext = (NTFS_FINDFILE_CONTEXT*)pEnumContext;

	if (ie->NameLen != pFindContext->nNameLen)
		return TRUE;
	if(0==_wcsnicmp(ie->FileName,pFindContext->szFileName,ie->NameLen))
	{
		pFindContext->MftRef = ie->Header.MftRef;
		return FALSE; // stop search
	}
	return TRUE;
}

MFT_REF NtfsFindDirEntryW(NTFS_FILE* pDir, LPCWSTR szFileName)
{
	NTFS_FINDFILE_CONTEXT FindContext;
	
	if (!szFileName)
		return INVALID_MFT_REF;

	FindContext.MftRef = INVALID_MFT_REF;
	FindContext.nNameLen = wcslen(szFileName);
	FindContext.szFileName = szFileName;
	
	NtfsEnumDirCB(pDir, &FindContext, NtfsFindFileProc);

	return FindContext.MftRef;
}

typedef struct tag_NTFS_ENUM_DIR_DATA
{
	NTFS_FILE* pDir;
	UINT nCurrentIndex;
} NTFS_ENUM_DIR_DATA;

typedef struct tag_NTFS_BUILD_INDEXES_ARRAY 
{
	NTFS_FILE* pDir;
	NTFS_INDEX_ENTRY** ppIndexEntries;
	ULONG nIndexEntriesCount;
	ULONG nCurrEntry;
} NTFS_BUILD_INDEXES_ARRAY;

BOOL __cdecl NtfsBuildIndexesArrayProc(void* pEnumContext, NTFS_INDEX_ENTRY* ie)
{
	NTFS_BUILD_INDEXES_ARRAY* pBuild = (NTFS_BUILD_INDEXES_ARRAY*)pEnumContext;
	if (!pBuild->ppIndexEntries)
		pBuild->nIndexEntriesCount++;
	else
	{
		if (pBuild->nCurrEntry >= pBuild->nIndexEntriesCount)
			return FALSE;
		pBuild->ppIndexEntries[pBuild->nCurrEntry++] = ie;
	}
	return TRUE;
}

HANDLE NtfsEnumDirFirst(NTFS_FILE* pDir, NTFS_INDEX_ENTRY** ppIndexEntry)
{
	NTFS_ENUM_DIR_DATA* pData;
	if (!ppIndexEntry)
		return NULL;
	if (!pDir)
		return NULL;
	if (!pDir->bDir)
		return NULL;
	if (!pDir->ppIndexEntries)
	{
		NTFS_BUILD_INDEXES_ARRAY build = {0};
		if (!pDir->nIndexEntriesCount)
		{
			if (!NtfsEnumDirCB(pDir, &build, NtfsBuildIndexesArrayProc))
				return NULL;
			if (!build.nIndexEntriesCount)
				return NULL;
			pDir->nIndexEntriesCount = build.nIndexEntriesCount;
		}
		build.ppIndexEntries = (NTFS_INDEX_ENTRY**)NtfsAllocMem(sizeof(NTFS_INDEX_ENTRY*)*pDir->nIndexEntriesCount);
		if (!build.ppIndexEntries)
			return NULL;
		if (!NtfsEnumDirCB(pDir, &build, NtfsBuildIndexesArrayProc))
		{
			NtfsFreeMem(build.ppIndexEntries);
			return NULL;
		}
		pDir->ppIndexEntries = build.ppIndexEntries;
	}
	pData = (NTFS_ENUM_DIR_DATA*)NtfsAllocMem(sizeof(NTFS_ENUM_DIR_DATA));
	if (!pData)
		return NULL;
	pData->pDir = pDir;
	*ppIndexEntry = pDir->ppIndexEntries[0];
	return pData;
}

BOOL NtfsEnumDirNext(HANDLE hEnumDir, NTFS_INDEX_ENTRY** ppIndexEntry)
{
	NTFS_ENUM_DIR_DATA* pData = (NTFS_ENUM_DIR_DATA*)hEnumDir;
	if (!pData)
		return FALSE;
	if (!ppIndexEntry)
		return FALSE;
	if (!pData->pDir)
		return FALSE;
	if (TRUE != pData->pDir->bDir)
		return FALSE;
	if (pData->nCurrentIndex + 1 >= pData->pDir->nIndexEntriesCount)
	{
		*ppIndexEntry = NULL;
		return FALSE;
	}
	*ppIndexEntry = pData->pDir->ppIndexEntries[++pData->nCurrentIndex];
	return TRUE;	
}

BOOL NtfsEnumDirClose(HANDLE hEnumDir)
{
	NtfsFreeMem(hEnumDir);
	return TRUE;
}

MFT_REF NtfsFindFileW(HNTFS pVolume, LPCWSTR szDirPath)
{
	WCHAR* szNextPath;
	WCHAR* szPathCopy;
	NTFS_FILE* pDir;
	MFT_REF MFTRecordNumber = 5;		//Start from Root dir
	
	if (!szDirPath)
		return -1;
	if (szDirPath[0] == 0)
		return -1;
	if (szDirPath[1] != ':' || szDirPath[2] != '\\')
		return -1;
	szDirPath+=3;
	szPathCopy = (WCHAR*)NtfsAllocMem((wcslen(szDirPath)+1) * sizeof(WCHAR));
	memcpy(szPathCopy, szDirPath,(wcslen(szDirPath)+1) * sizeof(WCHAR)); 
	szDirPath = szPathCopy;
	do{
		if(!szDirPath || !*szDirPath)
			break;
		pDir = NtfsOpenFile(pVolume, MFTRecordNumber);
		if (!pDir)
		{
			OUT_DEBUG1("error opening %S\n",szDirPath);
			MFTRecordNumber = INVALID_MFT_REF;
			break;
		}
		
		szNextPath=wcschr(szDirPath,'\\');
		if(szNextPath)
		{
			while (*szNextPath == '\\')
			{
				*szNextPath='\0';
				szNextPath++;
			}
		}
		MFTRecordNumber = NtfsFindDirEntryW(pDir,szDirPath);
		NtfsCloseFile(pDir);
		if(INVALID_MFT_REF == MFTRecordNumber)
		{
			//printf("%s not found\n",szDirPath);
			break;
		}
		szDirPath=szNextPath;
	} while(1);
	NtfsFreeMem(szPathCopy);
	return MFTRecordNumber;
}

/*
#include <WinIoCtl.h>

#ifndef IOCTL_VOLUME_BASE 
#define IOCTL_VOLUME_BASE   ((ULONG) 'V')
#define IOCTL_VOLUME_GET_VOLUME_DISK_EXTENTS    CTL_CODE(IOCTL_VOLUME_BASE, 0, METHOD_BUFFERED, FILE_READ_ACCESS)
typedef struct _DISK_EXTENT {
	ULONG DiskNumber;
	LARGE_INTEGER StartingOffset;
	LARGE_INTEGER ExtentLength;
} DISK_EXTENT, *PDISK_EXTENT;

typedef struct _VOLUME_DISK_EXTENTS {
	ULONG NumberOfDiskExtents;
	DISK_EXTENT Extents[2];
} VOLUME_DISK_EXTENTS, *pVolumeUME_DISK_EXTENTS;
#endif

//Drive zero based
LONGLONG FillHDDInfoLog(BYTE Drive,UINT* HDrive)
{
	HANDLE hDrive;
	PARTITION_INFORMATION pi;
	DWORD dwRet;
	LPSTR pos;
	LONGLONG StartSector=0;
	char TextForOut[64];
	OSVERSIONINFO VerInfo;
	VOLUME_DISK_EXTENTS vde;
	BOOL bGotVer;
	BOOL bWin2k;

	VerInfo.dwOSVersionInfoSize=sizeof(OSVERSIONINFO);
	bGotVer=(DWORD)GetVersionEx(&VerInfo);
	bWin2k=FALSE;
	if(bGotVer == TRUE && VerInfo.dwPlatformId==VER_PLATFORM_WIN32_NT && VerInfo.dwMajorVersion==5)	//Windows NT 2000
	{
		//OUT_D("Win2k found\n");
		bWin2k=TRUE;
	}

	wsprintf(TextForOut,"\\\\.\\%C:",Drive+'A');
	hDrive=CreateFile(TextForOut,GENERIC_READ,FILE_SHARE_READ|FILE_SHARE_WRITE,NULL,OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,NULL);
	if(hDrive==INVALID_HANDLE_VALUE)
	{
		printf("Error open Logical Drive");
		return StartSector;
	}

	if(bWin2k)
	{
		if(!DeviceIoControl(hDrive,IOCTL_VOLUME_GET_VOLUME_DISK_EXTENTS,NULL,0,&vde,sizeof(VOLUME_DISK_EXTENTS),&dwRet,NULL))
		{
			printf("Error DeviceIoControl in Win2k\n");
		}
		*HDrive=(BYTE)vde.Extents->DiskNumber+0x80;

		//		DeviceIoControl(hDrive,IOCTL_DISK_GET_PARTITION_INFO,NULL,0,&pi,sizeof( PARTITION_INFORMATION),&dwRet,NULL);
		//		StartSector=(DWORD)(pi.StartingOffset.QuadPart/512);

		StartSector=(DWORD)(vde.Extents[0].StartingOffset.QuadPart/512);

		CloseHandle(hDrive);
		return StartSector;
	}

	DeviceIoControl(hDrive,IOCTL_DISK_GET_PARTITION_INFO,NULL,0,&pi,sizeof( PARTITION_INFORMATION),&dwRet,NULL);
	CloseHandle(hDrive);

	StartSector=(LONGLONG)pi.StartingOffset.LowPart/512;
	if(pi.StartingOffset.HighPart!=0)
		StartSector=(LONGLONG)pi.StartingOffset.QuadPart/512;

	wsprintf(TextForOut,"%C:",Drive+'A');
	if(!QueryDosDevice(TextForOut,TextForOut,sizeof(TextForOut)))
	{
		printf("Error QueryDosDevice()");
		*HDrive=0;
	}
	else
	{
		pos=strstr(TextForOut,"Harddisk");
		if(pos==NULL)
		{
			pos=strstr(TextForOut,"Volume");
			if(pos==NULL)
				*HDrive=0;
			else
				*HDrive=0x80;
		}
		else
		{
			pos+=8;
			*HDrive=pos[0]-'0'+0x80;
		}
	}
	return StartSector;
}
*/



/*
int htoi(LPSTR szInBuf)
{
int i,Res,Mul;
int Digit = lstrlen(szInBuf);
char Buf[64];

  if(Digit==0)
		return -1;
		if(Digit>sizeof(Buf))
		return -1;
		lstrcpy(Buf,szInBuf);
		CharUpperBuff(Buf,Digit);
		for(i=0;i<Digit;i++)
		{
		switch(Buf[i])
		{
		case '0':
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9':
		Mul=Buf[i]-'0';
		break;
		case 'A':
		case 'B':
		case 'C':
		case 'D':
		case 'E':
		case 'F':
		Mul=Buf[i]-'A'+10;
		break;
		}
		if(Digit==2)
		{
		if(i==0)
		Res=Mul*16;
		else Res+=Mul;
		}
		else
		{
		if(i==0)
		Res=Mul*(16*16*16);
		else if(i==1)
		Res+=Mul*256;
		else if(i==2)
		Res+=Mul*16;
		else Res+=Mul;
		}
		}
		return Res;
		}
*/

#endif //_WIN32
