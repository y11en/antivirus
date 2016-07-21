#ifndef USE_WIN32_API
#ifndef DONT_USE_WIN32_API
# Error: one of USE_WIN32_API or DONT_USE_WIN32_API global define needed !!!
#endif // DONT_USE_WIN32_API
#endif // USE_WIN32_API

#ifdef USE_WIN32_API
# include <windows.h>
#else
# define NEED_SYSTEMTIME
#endif

#include <time.h>
#include <stdio.h>
#ifndef __unix__
#include <io.h>
#else
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#endif
#include <stdlib.h>
#include <string.h>
#include "Base.h"
#include "AvcPacker.h"
#include "fakeoop.h"
#include "Sq_su.h"

#include "records.h"
#include "calcsum.h"
#include "coff.h"

int AvcToAve2(SMemFile mfAvc, SMemFile *pmfAve2);
int Ave2ToAvc(SMemFile mfAve2, SMemFile *pmfAvc);
int TransformBase(SMemFile mfInBase, SMemFile *pmfOutBase, BOOL bCompress);
int CompressBase( const BYTE *pAveBuffer, DWORD dwAveBufferSize, const char *pszAvcFileName);

/*---------------------------Main API----------------------------*/

SPtrArray g_PointersArr;
static DWORD g_dwPointersCount = 0;

int UnpackAvc(const char *pszAvcFileName, void **ppAveBuffer, unsigned long *pdwAveBufferSize)
{
  SMemFile mfAvc;
  SMemFile mfAve;
#ifdef USE_WIN32_API
  HANDLE hFile;
  DWORD  Rdt;
#else
  FILE *fp=0;
#endif
  DWORD dwFileSize=0;
  BYTE *buf=0;
  int ret=ERR_SUCCESS;

  if (!pszAvcFileName || !ppAveBuffer || !pdwAveBufferSize)
    return ERR_INVALID_PARAMS;

#ifdef USE_WIN32_API
  if ( INVALID_HANDLE_VALUE == (hFile = CreateFile(pszAvcFileName, 
    GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL)) )
  {
    return ERR_FILE_IO_ERROR;
  }
  dwFileSize = GetFileSize(hFile, NULL);
  buf = (BYTE*)malloc(dwFileSize);
  if (buf==NULL)
  {
    CloseHandle(hFile);
    return ERR_OUTOFMEMORY;
  }
  SetFilePointer(hFile, 0, NULL, FILE_BEGIN);
  Rdt = 0;
  if ( FALSE == ReadFile(hFile, buf, dwFileSize, &Rdt, NULL) || 
       Rdt != dwFileSize )
  {
    CloseHandle(hFile);
    free(buf);
    return ERR_FILE_IO_ERROR;
  }
  CloseHandle(hFile);
#else


#ifdef __unix__
  struct stat S;
  if( stat(pszAvcFileName, &S)!= 0 )
	  return ERR_FILE_IO_ERROR;
  dwFileSize = (DWORD)S.st_size;
#endif

  fp=fopen(pszAvcFileName,"rb");
  if (fp==NULL)
    return ERR_FILE_IO_ERROR;

#ifndef __unix__
  dwFileSize = filelength(fileno(fp));
#endif

  buf = (BYTE*)malloc(dwFileSize);
  if (buf==NULL)
  {
    fclose(fp);
    return ERR_OUTOFMEMORY;
  }
  fseek(fp,0,SEEK_SET);
  if (fread(buf, 1, dwFileSize, fp) != dwFileSize)
  {
    fclose(fp);
    free(buf);
    return ERR_FILE_IO_ERROR;
  }
  fclose(fp);
#endif

  MemFileAttach(&mfAvc,buf,dwFileSize);
  MemFileReset(&mfAve);
  ret = AvcToAve2(mfAvc,&mfAve);

  MemFileDelete(&mfAvc);
  *pdwAveBufferSize = MemFileGetLength(&mfAve);
  *ppAveBuffer = MemFileDetach(&mfAve);

  // add ptr into list
  if (g_dwPointersCount==0)
    PtrArrayReset(&g_PointersArr);
  PtrArrayAdd(&g_PointersArr, *ppAveBuffer);
  g_dwPointersCount = g_PointersArr.dwSize;

  return ret;
}

int PackInAvc( void *pAveBuffer, unsigned long dwAveBufferSize, const char *pszAvcFileName )
{
  SMemFile mfAve;
  SMemFile mfAvc;
#ifdef USE_WIN32_API
  HANDLE hFile;
  DWORD Wrt;
#else
  FILE *fp=0;
#endif
  DWORD dwFileSize=0;
  BYTE *buf=0;
  int ret=ERR_SUCCESS;

  if (!pAveBuffer || !dwAveBufferSize || !pszAvcFileName)
    return ERR_INVALID_PARAMS;

  //return CompressBase( pAveBuffer, dwAveBufferSize, pszAvcFileName);
  MemFileAttach(&mfAve, pAveBuffer, dwAveBufferSize);
  MemFileReset(&mfAvc);
  ret = Ave2ToAvc(mfAve,&mfAvc);
  if (ret != ERR_SUCCESS)
    return ret;

#ifdef USE_WIN32_API
  if ( INVALID_HANDLE_VALUE == (hFile = CreateFile(pszAvcFileName,
    GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, 0, NULL)) )
  {
    return ERR_FILE_IO_ERROR;
  }
  Wrt = 0;
  if ( FALSE == WriteFile(hFile, mfAvc.ptr, mfAvc.dwSize, &Wrt, NULL) ||
       Wrt != mfAvc.dwSize )
  {
    ret = ERR_FILE_IO_ERROR;
  }
  CloseHandle(hFile);
#else
  fp = fopen(pszAvcFileName,"wb");
  if (fp==NULL)
    return ERR_FILE_IO_ERROR;

  if ( (fwrite(mfAvc.ptr, 1, mfAvc.dwSize, fp)) != mfAvc.dwSize)
    ret = ERR_FILE_IO_ERROR;
  fclose(fp);
#endif

  return ret;
}

int PackAveInAvc( void *pAveBuffer, unsigned long dwAveBufferSize, const char *pszAvcFileName )
{
  if (!pAveBuffer || !dwAveBufferSize || !pszAvcFileName)
    return ERR_INVALID_PARAMS;

  return CompressBase( pAveBuffer, dwAveBufferSize, pszAvcFileName);
}

int FreeBuffer( void *pBuffer)
{
  int i;
  if (!pBuffer)
    return ERR_INVALID_PARAMS; // NULL POINTER

  for (i=0; i<(int)g_PointersArr.dwSize; i++)
  {
    if (g_PointersArr.pp[i] == pBuffer)
    {
      PtrArrayFreeAt(&g_PointersArr, i);
      g_dwPointersCount = g_PointersArr.dwSize;
      return ERR_SUCCESS; // SUCCESS
    }
  }
  return ERR_BAD_FORMAT; // NOT OUR POINTER
}

/*-----------------------------------------------------------------*/

int AvcToAve2(SMemFile mfAvc, SMemFile *pmfAve2)
{
  return TransformBase(mfAvc, pmfAve2, 0);
}

int Ave2ToAvc(SMemFile mfAve2, SMemFile *pmfAvc)
{
  return TransformBase(mfAve2, pmfAvc, 1);
}

int AveToAvc()
{
  return ERR_GENERAL_ERROR;
}

/*----------------------Ported from Scarabey-------------------------*/

//BOOL add_16bit_links=1;

BOOL StripComments(BYTE* body)
{
	COFF_Header* Header;
	COFF_Symbol* Symbol;
	COFF_SectionHeader* SHeader;
	BOOL ready=0;

	DWORD i;
	int	TEMP_Idx=0;
	Header=(COFF_Header*)body;
	if(Header->Machine!=0x014c)	goto ret;

	Symbol=(COFF_Symbol*)(body+Header->P_SymbolTable);
	SHeader=(COFF_SectionHeader*)(body+sizeof(COFF_Header)+Header->OptionHeaderSize);

	for(i=0;i<Header->N_Symbols;i++)
  {
		char buf[9];
		char* Name = buf;
		buf[8]=0;

		if(Symbol[i].Type & 0x60) goto strip;
		if(Symbol[i].Section > Header->N_Sections) goto strip;

		if(Symbol[i].NameWord==0)
			Name=(char*)(Symbol+Header->N_Symbols)+Symbol[i].NameOffset;
		else memcpy(buf,Symbol[i].Name,8); 

		
		// PUBLIC symbols
		if(Symbol[i].Section)
		{
			if(*Name=='_'){
				if(Symbol[i].Type&1) goto strip;
			}
			else goto strip;
		}
		goto skip;

strip:
		if(Symbol[i].NameWord==0)
		{ 
			Name=(char*)(Symbol+Header->N_Symbols)+Symbol[i].NameOffset;
			memset(Name,0,strlen(Name));
			strcpy(Name,".strip.");
		}
		else strcpy(Symbol[i].Name,".strip."); 

skip:
		i+=Symbol[i].Skip;
	}

	for(i=0;i<Header->N_Sections;i++){
		if(	!memcmp(SHeader[i].Name,".drect",6)
		||	!memcmp(SHeader[i].Name,".debug",6))
			memset(body+SHeader[i].P_RawData,SHeader[i].RawDataSize,0);
	}

	ready=1;
ret:
	return ready;
}


SMemFile mfRecordsArray[NUMBER_OF_RECORD_BLOCK_TYPES][NUMBER_OF_RECORD_TYPES];
int RNumTable[NUMBER_OF_RECORD_BLOCK_TYPES][NUMBER_OF_RECORD_TYPES];

SMemFile mfNames;
int NameIdx;



int LoadBase(const BYTE *pAveBuffer, DWORD dwAveBufferSize)
{
	int rCount=0;
  AVP_BaseHeader		BaseHeader;
	SPtrArray 	BlockHeaderArray;
	AVP_BlockHeader *	bhp;

  PtrArrayReset(&BlockHeaderArray);

//BaseHeader read

  memcpy(&BaseHeader, pAveBuffer, sizeof(BaseHeader));

	if(*(DWORD*)BaseHeader.Authenticity==AVP_MAGIC)
    memcpy(BaseHeader.Authenticity, pAveBuffer, sizeof(BaseHeader)-0x40);
	
	if(BaseHeader.Magic==AVP_MAGIC)
	{
		BYTE* nbuf=NULL;
		UINT i;
		UINT j;
    BOOL names_done;
      
//BlockHeaderTable read

    bhp = (AVP_BlockHeader*)&pAveBuffer[BaseHeader.BlockHeaderTableFO];
    for(i=0; i<BaseHeader.BlockHeaderTableSize; i++)
    {
      PtrArrayAdd(&BlockHeaderArray,bhp);
      bhp++;
		}

		names_done=0;

lab_names_done:

		for(i=0; i<BaseHeader.BlockHeaderTableSize; i++)
    {
      BYTE* buf = 0;
			DWORD npos;
			DWORD k;

			bhp=(AVP_BlockHeader*)BlockHeaderArray.pp[i];
			if(bhp->Compression) goto abort_;

      // Fix LINK16 bug
      if (bhp->CompressedSize == 0 || bhp->UncompressedSize == 0)
        continue;
			
			switch(bhp->BlockType)
      {
			case BT_NAME:
				break;
			case BT_LINK16:
				//if(!add_16bit_links)
          //continue;
			case BT_RECORD:
			case BT_LINK32:
				if(names_done)
					break;
			case BT_EDIT:
			case BT_COMMENT:
			default: continue;
			}
			
			/*BYTE* */ buf=malloc(bhp->CompressedSize);

			if(buf==NULL) goto abort_;

      memcpy(buf,&pAveBuffer[bhp->BlockFO], bhp->CompressedSize);

			if( bhp->CRC!=CalcSum(buf,bhp->CompressedSize) )
			{
        free(buf);
				goto abort_;
			}

			switch(bhp->BlockType)
      {
			case BT_NAME:
				if(names_done)
        {
					MemFileWrite( &mfNames, buf, bhp->CompressedSize );
				}
				else
        {
					if(nbuf) free(nbuf);
          nbuf=malloc(bhp->CompressedSize+1);
					memcpy(nbuf+1,buf,bhp->CompressedSize);
					nbuf[0]='\n';
					for(k=1; k<=bhp->CompressedSize; k++)
						if(nbuf[k]=='\n') nbuf[k]=0;
					free(buf);
					buf=0;
					names_done=1;
					goto lab_names_done;
				}
				break;

			case BT_RECORD:
				rCount+=bhp->NumRecords;
				npos=MemFileGetPosition(&mfNames);
				
				switch(bhp->RecordType)
        {
				case RT_MEMORY:
          {
					  R_Memory* p=(R_Memory*)buf;
					  for( j=0; j<bhp->NumRecords; j++,p++ )
					  {
						  p->NameIdx +=npos;
						  if(p->LinkIdx!=(WORD)-1)  p->LinkIdx +=RNumTable[BT_LINK32][bhp->RecordType];
					  }
            break;
					}
				case RT_SECTOR:
          {
					  R_Sector* p=(R_Sector*)buf;
					  for(j=0;j<bhp->NumRecords;j++,p++)
					  {
						  p->NameIdx +=npos;
						  if(p->LinkIdx!=(WORD)-1)	p->LinkIdx +=RNumTable[BT_LINK32][bhp->RecordType];
					  }
            break;
          }
				case RT_FILE:
          {
					  R_File* p=(R_File*)buf;
					  for(j=0;j<bhp->NumRecords;j++,p++)
					  {
						  p->NameIdx +=npos;
						  if(p->LinkIdx!=(WORD)-1)	p->LinkIdx +=RNumTable[BT_LINK32][bhp->RecordType];
					  }
            break;
          }
				case RT_CA:
          {
					  R_CA* p=(R_CA*)buf;
					  for(j=0;j<bhp->NumRecords;j++,p++)
					  {
						  p->NameIdx +=npos;
						  if(p->LinkIdx!=(WORD)-1)	p->LinkIdx +=RNumTable[BT_LINK32][bhp->RecordType];
					  }
            break;
          }
				case RT_UNPACK:
          {
					  R_Unpack* p=(R_Unpack*)buf;
					  for(j=0;j<bhp->NumRecords;j++,p++)
					  {
						  p->NameIdx +=npos;
						  if(p->LinkIdx!=(WORD)-1)	p->LinkIdx +=RNumTable[BT_LINK32][bhp->RecordType];
					  }
            break;
          }
				case RT_EXTRACT:
          {
					  R_Extract* p=(R_Extract*)buf;
					  for(j=0;j<bhp->NumRecords;j++,p++)
					  {
						  p->NameIdx +=npos;
						  if(p->LinkIdx!=(WORD)-1)	p->LinkIdx +=RNumTable[BT_LINK32][bhp->RecordType];
					  }
            break;
          }
				case RT_KERNEL:
          {
					  R_Kernel* p=(R_Kernel*)buf;
					  for(j=0;j<bhp->NumRecords;j++,p++)
            {
						  if(p->LinkIdx!=(WORD)-1)	p->LinkIdx +=RNumTable[BT_LINK32][bhp->RecordType];
					  }
            break;
          }
				case RT_JUMP:
          {
					  R_Jump* p=(R_Jump*)buf;
					  for(j=0;j<bhp->NumRecords;j++,p++)
            {
						  if(p->LinkIdx!=(WORD)-1)	p->LinkIdx +=RNumTable[BT_LINK32][bhp->RecordType];
					  }
            break;
          }
				default: break;
				}
        MemFileWrite(&mfRecordsArray[bhp->BlockType][bhp->RecordType],buf,bhp->CompressedSize);
				break;
			case BT_LINK32:
			case BT_LINK16:
				{
					BYTE* b=buf;
          UINT k;
					for(k=0; k<bhp->NumRecords; k++)
          {
						DWORD* dptr=(DWORD*)b;
						DWORD q=0;
						DWORD l=dptr[0];
						b+=l;
						l-=dptr[1];

						if(BT_LINK32==bhp->BlockType)
							StripComments(((BYTE*)dptr)+dptr[1]+8);
						MemFileWrite(&mfRecordsArray[bhp->BlockType][bhp->RecordType],&l,4);
						MemFileWrite(&mfRecordsArray[bhp->BlockType][bhp->RecordType],&q,4);
						MemFileWrite(&mfRecordsArray[bhp->BlockType][bhp->RecordType],((BYTE*)dptr)+dptr[1]+8,l-8);
					}
				}
				break;
			default:
				break;
			}
			if(buf) free(buf);
		}
		if(nbuf) free(nbuf);

		for(i=0; i<BaseHeader.BlockHeaderTableSize; i++)
    {
			bhp=(AVP_BlockHeader*)BlockHeaderArray.pp[i];
			switch(bhp->BlockType)
      {
			case BT_NAME:	
				NameIdx+=bhp->NumRecords;
				break;
			case BT_RECORD:
			case BT_LINK32:
			case BT_LINK16:
				RNumTable[bhp->BlockType][bhp->RecordType]+=bhp->NumRecords;
			default:
				break;
			}
		}
	}
abort_:;
  free(BlockHeaderArray.pp);
	return rCount;
}

void GetLocalTime2(SYSTEMTIME *st)
{
  struct tm *lt;
  time_t ltime;

  time(&ltime);
  lt = localtime(&ltime);

  st->wMilliseconds = 0;
  st->wSecond = lt->tm_sec;
  st->wMinute = lt->tm_min;
  st->wHour = lt->tm_hour;
  st->wDay = lt->tm_mday;
  st->wDayOfWeek = lt->tm_wday;
  st->wMonth = lt->tm_mon + 1;
  st->wYear = lt->tm_year + 1900;
}

char * TimeFormat(char *pszBuffer, DWORD nCount, const char *pFormat)
{
  struct tm *lt;
  time_t ltime;

  if(!pszBuffer) return 0;

  time(&ltime);
  lt = localtime(&ltime);

  strftime(pszBuffer, nCount, pFormat, lt);
  return pszBuffer;
}


int CompressBase( const BYTE *pAveBuffer, DWORD dwAveBufferSize, const char *pszAvcFileName)
{
	int NameCnt=0;
  AVP_BaseHeader	BaseHeader;
	int ret=ERR_SUCCESS;
  FILE *fp=0;

  memset(mfRecordsArray,0,NUMBER_OF_RECORD_BLOCK_TYPES*NUMBER_OF_RECORD_TYPES*sizeof(SMemFile));
	memset(RNumTable,     0,NUMBER_OF_RECORD_BLOCK_TYPES*NUMBER_OF_RECORD_TYPES*sizeof(int));
  MemFileReset(&mfNames);
	NameIdx=0;
	
  memcpy(&BaseHeader, pAveBuffer, sizeof(BaseHeader));
	if(BaseHeader.Magic==AVP_MAGIC)
  {
		if(!LoadBase(pAveBuffer,dwAveBufferSize))
    {
			ret=ERR_BAD_FORMAT;
			goto err;
		}
	}
	else
  {
    ret=ERR_BAD_FORMAT;
    goto err;
	}

	if((fp=fopen(pszAvcFileName,"wb")) != NULL)
  {
    //char timeBuf[0x200];
		//SYSTEMTIME st;
 		SPtrArray BlockHeaderArray;
		AVP_BlockHeader* bhp;
		int i,j;
		int nbh;

    PtrArrayReset(&BlockHeaderArray);

    /*GetLocalTime2(&st);

		memset(BaseHeader.Text,0,0x40);
		sprintf((char *)BaseHeader.Text,"AVP Antiviral Database. (c)Kaspersky Lab 1997-%d.",st.wYear);
		*(DWORD*)(BaseHeader.Text+0x3E)='\xd\xa';

		memset(BaseHeader.Authenticity,0,0x40);
		strcpy((char *)(BaseHeader.Authenticity), TimeFormat(timeBuf,0x200,"Kaspersky Lab. %d %b %Y  %H:%M:%S"));
		*(DWORD*)(BaseHeader.Authenticity+0x3C)='\xd\xa\xd\xa';*/

		BaseHeader.VXs.BXs.VerH=   0x03;
		BaseHeader.VXs.BXs.VerL=   0x00;
		BaseHeader.Flags=0x0001;
		BaseHeader.NumberOfRecordBlockTypes=NUMBER_OF_RECORD_BLOCK_TYPES;
		BaseHeader.NumberOfRecordTypes=NUMBER_OF_RECORD_TYPES;
		/*BaseHeader.CreationTime=st;
		BaseHeader.ModificationTime=st;*/

		BaseHeader.Flags &=~1;
	
//BaseHeader write
    fwrite(&BaseHeader,sizeof(BaseHeader),1,fp);

//BlockTableHeader write
		BaseHeader.BlockHeaderTableFO = ftell(fp);
		nbh=NameIdx?1:0;
		for (i=0; i<NUMBER_OF_RECORD_BLOCK_TYPES; i++)
			for (j=0; j<NUMBER_OF_RECORD_TYPES; j++)
				if(RNumTable[i][j])nbh++;


		bhp=(AVP_BlockHeader *)malloc( sizeof(AVP_BlockHeader) );
		memset(bhp,0,sizeof(AVP_BlockHeader));
		for(i=0;i<nbh;i++) fwrite(bhp,sizeof(AVP_BlockHeader),1,fp);
		free(bhp);

//Records & Links write
		for (i=0; i<NUMBER_OF_RECORD_BLOCK_TYPES; i++)
		for (j=0; j<NUMBER_OF_RECORD_TYPES; j++)
		{
			SMemFile cmf;
      UINT ib;
      BYTE *mb;
			if(0==RNumTable[i][j])continue;

			bhp=(AVP_BlockHeader *)malloc(sizeof(AVP_BlockHeader));
			memset(bhp,0,sizeof(AVP_BlockHeader));
      PtrArrayAdd(&BlockHeaderArray, bhp);

			bhp->BlockFO=ftell(fp);
			bhp->BlockType=i;   //HERE define TYPES
			bhp->RecordType=j;  
			bhp->NumRecords=RNumTable[i][j];

			if(i==BT_RECORD){
				switch(j){
				case RT_KERNEL: bhp->RecordSize=sizeof(R_Kernel); break;
				case RT_JUMP:   bhp->RecordSize=sizeof(R_Jump); break;
				case RT_MEMORY: bhp->RecordSize=sizeof(R_Memory); break;
				case RT_SECTOR: bhp->RecordSize=sizeof(R_Sector); break;
				case RT_FILE:   bhp->RecordSize=sizeof(R_File); break;
				case RT_CA:     bhp->RecordSize=sizeof(R_CA); break;
				case RT_UNPACK: bhp->RecordSize=sizeof(R_Unpack); break;
				case RT_EXTRACT:bhp->RecordSize=sizeof(R_Extract); break;
				default:  bhp->RecordSize=0; break;
				}
			}
			else  bhp->RecordSize=0;
			
			bhp->UncompressedSize=MemFileGetLength( &mfRecordsArray[i][j] );
			bhp->Compression=1;
			
      MemFileReset(&cmf);
			MemFileSeekToBegin( &mfRecordsArray[i][j] );
			bhp->CompressedSize=squeeze(&mfRecordsArray[i][j],&cmf,bhp->UncompressedSize,0);
			/* BYTE* */mb = MemFileDetach(&cmf);
			for(ib=0; ib<bhp->CompressedSize; ib++) mb[ib]^=(BYTE)ib;

			bhp->CRC=CalcSum(mb, bhp->CompressedSize);
			fwrite(mb,bhp->CompressedSize,1,fp);
			free(mb);
		}
		
//Names write
		if(NameIdx)
		{
      SMemFile *mf;
      SMemFile cmf;
      UINT ib;
      BYTE *mb;

			bhp=(AVP_BlockHeader *)malloc(sizeof(AVP_BlockHeader));
			memset(bhp,0,sizeof(AVP_BlockHeader));
      PtrArrayAdd( &BlockHeaderArray, bhp );

			bhp->BlockFO=ftell(fp);
			bhp->BlockType=BT_NAME;
			bhp->NumRecords=NameIdx;

			/* CMemFile* */mf=&mfNames;

			bhp->UncompressedSize=MemFileGetLength(mf);
			bhp->Compression=1;
			
			//CMemFile cmf(0x1000);

      MemFileReset(&cmf);
			MemFileSeekToBegin(mf);
			bhp->CompressedSize=squeeze(mf,&cmf,bhp->UncompressedSize,0);
			/* BYTE* */mb=MemFileDetach( &cmf );
			for(ib=0; ib<bhp->CompressedSize; ib++)	mb[ib]^=(BYTE)ib;

			bhp->CRC=CalcSum(mb,bhp->CompressedSize);
      fwrite(mb,bhp->CompressedSize,1,fp);
			free(mb);
			
			mb=MemFileDetach(mf);
			free(mb);
		}

		BaseHeader.FileSize = ftell(fp)+64;
		BaseHeader.BlockHeaderTableSize = PtrArrayGetSize( &BlockHeaderArray ); 
		BaseHeader.AuthenticityCRC=(CalcSum(BaseHeader.Authenticity,0x40));
		BaseHeader.HeaderCRC=(CalcSum((BYTE*)&BaseHeader.Magic,sizeof(BaseHeader)-0x84));
		fseek(fp,0,SEEK_SET);
    fwrite(&BaseHeader,sizeof(BaseHeader),1,fp);

//BlockTableHeader Again write
    fseek(fp, BaseHeader.BlockHeaderTableFO, SEEK_SET);
		for(i=0; i<(int)BaseHeader.BlockHeaderTableSize; i++)
    {
      fwrite(BlockHeaderArray.pp[i],sizeof(AVP_BlockHeader),1,fp);
		}
    fclose(fp);
		PtrArrayFreeAllPtr(&BlockHeaderArray);

		for (i=0; i<NUMBER_OF_RECORD_BLOCK_TYPES; i++)
    {
      for (j=0; j<NUMBER_OF_RECORD_TYPES; j++)
        MemFileDelete(&mfRecordsArray[i][j]);
    }
    MemFileDelete(&mfNames);
		
	}
  else
  {
    ret = ERR_FILE_IO_ERROR;
  }
err:;
	return ret;
}

/*---------------avc->ave2->avc transformation procedure------------------*/

/*
bCompress:
  0 - выполнять распаковку (avc->ave2)
  1 - выполнять упаковку (ave2->avc)
*/
int TransformBase(SMemFile mfInBase, SMemFile *pmfOutBase, BOOL bCompress)
{
  int i;
  AVP_BaseHeader  BaseHeader;
	SPtrArray 	BlockHeaderArray;
	AVP_BlockHeader *	bhp;
	int rCount=0;
	DWORD bread;
  int ret = ERR_SUCCESS; // NO ERROR

  PtrArrayReset(&BlockHeaderArray);
  MemFileSeekToBegin(&mfInBase);

//BaseHeader read
	bread = MemFileRead(&mfInBase, &BaseHeader, sizeof(BaseHeader));

	if(*(DWORD*)BaseHeader.Authenticity==AVP_MAGIC)
  {
    MemFileSeekToBegin(&mfInBase);
    bread = MemFileRead(&mfInBase, BaseHeader.Authenticity, sizeof(BaseHeader)-0x40);
	}

	if(BaseHeader.Magic!=AVP_MAGIC)
  {
    ret = ERR_BAD_FORMAT;
	}
	else
	{
    //char timeBuf[0x200];
		//SYSTEMTIME st;
    BOOL bBaseCompressed = 0;

//BlockHeaderTable read
    MemFileSeek(&mfInBase, BaseHeader.BlockHeaderTableFO, SEEK_SET);

    for(i=0; i<(int)BaseHeader.BlockHeaderTableSize; i++)
    {
			bhp=(AVP_BlockHeader*)malloc(sizeof(AVP_BlockHeader));
      bread = MemFileRead(&mfInBase, bhp, sizeof(AVP_BlockHeader));
			if(bread!=sizeof(AVP_BlockHeader))
      {
				free(bhp);
        ret = ERR_FILE_CORRUPTED;
				goto abort_;
			}
      PtrArrayAdd(&BlockHeaderArray,bhp);
      if (bhp->Compression) bBaseCompressed = 1;
		}

    if ( (!bBaseCompressed && !bCompress) || (bBaseCompressed && bCompress))
    {
      ret = ERR_BAD_FORMAT;  // WRONG COMPRESSION STATUS
      goto abort_;
    }

    MemFileReset(pmfOutBase);

		/*GetLocalTime2(&st);

		memset(BaseHeader.Text,0,0x40);
		sprintf((char *)BaseHeader.Text,"AVP Antiviral Database. (c)Kaspersky Lab 1997-%d.",st.wYear);
		*(DWORD*)(BaseHeader.Text+0x3E)='\xd\xa';

		memset(BaseHeader.Authenticity,0,0x40);
		strcpy((char *)(BaseHeader.Authenticity), TimeFormat(timeBuf,0x200,"Kaspersky Lab. %d %b %Y  %H:%M:%S"));
		*(DWORD*)(BaseHeader.Authenticity+0x3C)='\xd\xa\xd\xa';

		BaseHeader.CreationTime=st;
		BaseHeader.ModificationTime=st;*/

//BaseHeader write
    MemFileWrite(pmfOutBase, &BaseHeader, sizeof(BaseHeader));

//BlockTableHeader write
		BaseHeader.BlockHeaderTableFO = MemFileGetPosition(pmfOutBase);
		bhp=(AVP_BlockHeader *)malloc( sizeof(AVP_BlockHeader) );
		memset(bhp,0,sizeof(AVP_BlockHeader));
    for(i=0; i<(int)BaseHeader.BlockHeaderTableSize; i++)
      MemFileWrite(pmfOutBase, bhp, sizeof(AVP_BlockHeader));
		free(bhp);

//Records & Links write
    for(i=0; i<(int)BaseHeader.BlockHeaderTableSize; i++)
    {
      UINT ib;
      BYTE* buf = 0;

			bhp=(AVP_BlockHeader*)BlockHeaderArray.pp[i];

      if (bhp->CompressedSize == 0 && bhp->UncompressedSize == 0)
        continue;

      if (bhp->CompressedSize == 0 || bhp->UncompressedSize == 0)
      {
        ret = ERR_AVP_BLOCKHEADER_CORRUPTED;  // 16-битные линки надо лучше чистить !!!
        goto abort_;
      }

      // read block
      MemFileSeek(&mfInBase, bhp->BlockFO, SEEK_SET);
      buf = (BYTE*)malloc(bhp->CompressedSize);
      bread = MemFileRead(&mfInBase, buf, bhp->CompressedSize);

			if(bread!=bhp->CompressedSize || bhp->CRC!=CalcSum(buf,bhp->CompressedSize))
			{
        free(buf);
        ret = ERR_FILE_CORRUPTED;
				goto abort_;
			}

      if(bhp->Compression)
      { // decompress block
        BYTE *out_buf = (BYTE*)malloc(bhp->UncompressedSize+1);

        for(ib=0; ib<bhp->CompressedSize; ib++) buf[ib]^=(BYTE)ib;
        bread = unsqu( buf, out_buf);
        if (bread != bhp->UncompressedSize)
        {
          ret = ERR_FILE_CORRUPTED;
          goto abort_;
        }
        free(buf);
        buf = out_buf;
        out_buf = 0;

        bhp->Compression = 0;
        bhp->CompressedSize = bhp->UncompressedSize;
      }

      if (bCompress)
      { // compress block
        SMemFile mfBlock;
        SMemFile mfOutBlock;

        MemFileReset(&mfOutBlock);
        MemFileAttach(&mfBlock, buf, bhp->UncompressedSize);

        bhp->Compression = 1;
        bhp->CompressedSize = squeeze(&mfBlock, &mfOutBlock, bhp->UncompressedSize, 0);
        MemFileDelete(&mfBlock);
        buf = MemFileDetach(&mfOutBlock);
        for(ib=0; ib<bhp->CompressedSize; ib++) buf[ib]^=(BYTE)ib;
      }

      bhp->BlockFO = MemFileGetPosition(pmfOutBase);
      bhp->CRC = CalcSum( buf, bhp->CompressedSize);
      MemFileWrite(pmfOutBase, buf, bhp->CompressedSize);
    }

//BaseHeader Again write
    BaseHeader.FileSize = MemFileGetLength(pmfOutBase) + ((bCompress)?64:0);
		BaseHeader.BlockHeaderTableSize = PtrArrayGetSize( &BlockHeaderArray );
		BaseHeader.AuthenticityCRC=(CalcSum(BaseHeader.Authenticity,0x40));
		BaseHeader.HeaderCRC=(CalcSum((BYTE*)&BaseHeader.Magic,sizeof(BaseHeader)-0x84));

    MemFileSeekToBegin(pmfOutBase);
    MemFileWrite(pmfOutBase,&BaseHeader,sizeof(BaseHeader));

//BlockTableHeader Again write
    MemFileSeek(pmfOutBase, BaseHeader.BlockHeaderTableFO, SEEK_SET);
		for(i=0; i<(int)BaseHeader.BlockHeaderTableSize; i++)
      MemFileWrite(pmfOutBase, BlockHeaderArray.pp[i], sizeof(AVP_BlockHeader));

  }
abort_:
  PtrArrayFreeAllPtr(&BlockHeaderArray);
  return ret;
}