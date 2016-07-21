// AvpEditDoc.cpp : implementation of the CAvpEditDoc class
//

#include "stdafx.h"
#include "AvpEdit.h"
#include "MainFrm.h"
#include <io.h>

#include "AvpEditDoc.h"

#include <Bases/Format/Records.h>
#include "Dialogs.h"

#include <ScanAPI/scanobj.h>
#include <ScanAPI/avp_dll.h>
#include <Bases/Method.h>
#include <Bases/Sizes.h>

#include "OptDlg.h"
extern COptDlg OptDlg;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#if (defined (KL_MULTIPLATFORM_LINKS))
    bool g_bShouldStop = false;
    bool g_bConversionAllowed = false;
#endif

extern CMainFrame* MainFramePtr;
extern BYTE Authenticity[];

extern CPtrArray Clipboard;
extern DWORD squeeze (CFile& inf, CFile& outf, DWORD length ,BYTE xor);

void FreeAllStr(CPtrArray& array){
	int i;
	while(i=array.GetSize()){
		i--;
		void* ptr=array.GetAt(i);
		if(ptr!=NULL)delete (CString*)ptr; 
		array.RemoveAt(i);
	}
}
void FreeAllPtr(CPtrArray& array){
	int i;
	while(i=array.GetSize()){
		i--;
		void* ptr=array.GetAt(i);
		if(ptr!=NULL)delete (char*)ptr; 
		array.RemoveAt(i);
	}
}




/////////////////////////////////////////////////////////////////////////////
// CAvpEditDoc

IMPLEMENT_DYNCREATE(CAvpEditDoc, CDocument)

BEGIN_MESSAGE_MAP(CAvpEditDoc, CDocument)
	//{{AFX_MSG_MAP(CAvpEditDoc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAvpEditDoc construction/destruction

CAvpEditDoc::CAvpEditDoc()
{
	Compression=0;
	ColumnCount=8;

	ColumnCx[0]=100;
	ColumnCx[1]=20;
	ColumnCx[2]=50;
	ColumnCx[3]=80;
	ColumnCx[4]=50;
	ColumnCx[5]=100;
	ColumnCx[6]=100;
	ColumnCx[7]=200;

	ColumnName[0]="Name";
	ColumnName[1]="*";
	ColumnName[2]="Type";
	ColumnName[3]="SubType";
	ColumnName[4]="Method";
	ColumnName[5]="Link16";
	ColumnName[6]="Link32";
	ColumnName[7]="Comment";
	// TODO: add one-time construction code here
}

CAvpEditDoc::~CAvpEditDoc()
{
	int i;
	while(i=EditArray.GetSize()){
		i--;
		CRecordEdit* ptr=(CRecordEdit*)EditArray[i];
		if(ptr!=NULL)delete ptr; 
		EditArray.RemoveAt(i);
	}
}

BOOL CAvpEditDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)
	return InitHeader();
}

/////////////////////////////////////////////////////////////////////////////
// CAvpEditDoc serialization

#include "dbf_defs.h"
#include "dbf_head.h"
#include "dbf_recd.h"
#include "../obj16.h"

__int64 GetAvpBaseId(void* buf, DWORD type, const char* name)
{
	__int64 ret=0;
	switch(type){
	case RT_SECTOR:
		ret=(((__int64)CalcSum((BYTE*)buf,16))<<32) | CalcSum((BYTE*)name,strlen(name));
		break;
	case RT_FILE:
	case RT_CA:
	case RT_UNPACK:
	case RT_EXTRACT:
		ret=(((__int64)CalcSum((BYTE*)buf,17))<<32) | CalcSum((BYTE*)name,strlen(name));
		break;
		
	default:
		break;
	}
	return ret;
}


char* SignGetName();

void CAvpEditDoc::Serialize(CArchive& ar)
{
	CPtrArray ArraysTable[NUMBER_OF_RECORD_BLOCK_TYPES][NUMBER_OF_RECORD_TYPES];
	CPtrArray CommentsArray;
	CPtrArray NamesArray;
	CPtrArray BlockHeaderArray;
	AVP_BlockHeader* bhp;

	AfxGetApp()->BeginWaitCursor();
  
	DWORD fo;
	CFile* ar_fp=ar.GetFile();
	
	if(Compression)BaseHeader.Flags &=~1;
	else BaseHeader.Flags |=1;
	
	if (ar.IsStoring())	// TODO: add storing code here
	{
		int nbh;
		int i,j;
		MainFramePtr->SetMessageText(ar_fp->GetFilePath()
			+(Compression?" :   packing...":" :   saving..."));
		MainFramePtr->UpdateWindow();

//BaseHeader write
		SYSTEMTIME st;
		GetLocalTime(&st);
		BaseHeader.ModificationTime=st;
		
		memset(BaseHeader.Text,0,0x40);
		memset(BaseHeader.Authenticity,0,0x40);
		sprintf((char *)BaseHeader.Text,"AVP Antiviral Database. (c)Kaspersky Lab 1997-%d.",st.wYear);
		*(DWORD*)(BaseHeader.Text+0x3E)='\xd\xa';
		sprintf((char*)BaseHeader.Authenticity,"%s",SignGetName());
		*(DWORD*)(BaseHeader.Authenticity+0x3C)='\xd\xa\xd\xa';

		ar.Write(&BaseHeader,sizeof(BaseHeader));

		if(Compression==0){
			ar.Write(&ColumnCount,sizeof(int));
			ar.Write(ColumnCx,sizeof(int)*ColumnCount);
		}

//BlockTableHeader write
		ar.Flush();
		BaseHeader.BlockHeaderTableFO = ar_fp->GetPosition();
		
//CHECK integrity!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//		nbh=FillArrays();

		int eas=EditArray.GetSize();
		for( i=0;i<eas;i++){
			CRecordEdit* er;
			er=(CRecordEdit*)EditArray[i];
			
			er->MakeNameIdx(NamesArray,Compression,!i);
			er->MakeCommentIdx(CommentsArray);
			er->MakeRecordIdx(ArraysTable[BT_RECORD][er->Type]);
			er->MakeLinkIdx(ArraysTable[BT_LINK32][er->Type],ArraysTable[BT_LINK16][er->Type]);

			er->SetModifyFlag(FALSE);
		}
		nbh=eas?1:0;
		nbh+=NamesArray.GetSize()?1:0;

		if(Compression==0)
			nbh+=CommentsArray.GetSize()?1:0;
		else nbh--;

		for (i=0; i<NUMBER_OF_RECORD_BLOCK_TYPES; i++)
			for (int j=0; j<NUMBER_OF_RECORD_TYPES; j++)
				nbh+=ArraysTable[i][j].GetSize()?1:0;



		bhp=new AVP_BlockHeader;
		memset(bhp,0,sizeof(AVP_BlockHeader));
		for(i=0;i<nbh;i++)	ar.Write(bhp,sizeof(AVP_BlockHeader));
		delete bhp;

//Records & Links write
		for (i=0; i<NUMBER_OF_RECORD_BLOCK_TYPES; i++)
		for (j=0; j<NUMBER_OF_RECORD_TYPES; j++)
		{
			if(0==ArraysTable[i][j].GetSize())continue;

			bhp=new AVP_BlockHeader;
			memset(bhp,0,sizeof(AVP_BlockHeader));
			BlockHeaderArray.Add(bhp);

			ar.Flush();
			bhp->BlockFO=ar_fp->GetPosition();
			bhp->BlockType=i;   //HERE define TYPES
			bhp->RecordType=j;  
			bhp->NumRecords=ArraysTable[i][j].GetSize();

			CMemFile mf(0x1000);

			if(i==BT_RECORD){
				switch(j){
				case RT_KERNEL:	bhp->RecordSize=sizeof(R_Kernel); break;
				case RT_JUMP:	bhp->RecordSize=sizeof(R_Jump); break;
				case RT_MEMORY:	bhp->RecordSize=sizeof(R_Memory); break;
				case RT_SECTOR:	bhp->RecordSize=sizeof(R_Sector); break;
				case RT_FILE:	bhp->RecordSize=sizeof(R_File); break;
				case RT_CA:		bhp->RecordSize=sizeof(R_CA); break;
				case RT_UNPACK:	bhp->RecordSize=sizeof(R_Unpack); break;
				case RT_EXTRACT:bhp->RecordSize=sizeof(R_Extract); break;
				default:		bhp->RecordSize=0; break;
				}
				for(UINT k=0;k<bhp->NumRecords;k++){
					mf.Write(ArraysTable[i][j][k],bhp->RecordSize);
//					ar.Write(ArraysTable[i][j][k],bhp->RecordSize);
				}
			}
//Links write
			
			else{
				bhp->RecordSize=0;
				for(UINT k=0;k<bhp->NumRecords;k++){
					DWORD* dptr=(DWORD*)ArraysTable[i][j][k];

					if(Compression){
						DWORD q=0;
						DWORD l=dptr[0];
						l-=dptr[1];
						mf.Write(&l,4);
						mf.Write(&q,4);
						mf.Write(((BYTE*)dptr)+dptr[1]+8,l-8);
					}
					else{
						mf.Write(dptr,*dptr);
					}

//					ar.Write(dptr,*dptr);
				}
			}
			
			bhp->UncompressedSize=mf.GetLength();
			bhp->Compression=Compression;
			BYTE* mb;
			if(Compression==0){
				mb=mf.Detach();
				bhp->CompressedSize=bhp->UncompressedSize;
			}
			else if(Compression==1){
				CMemFile cmf(0x1000);
				mf.Seek(0,CFile::begin);
				bhp->CompressedSize=squeeze(mf,cmf,bhp->UncompressedSize,0);
				mb=cmf.Detach();
				for(UINT i=0;i<bhp->CompressedSize;i++)	mb[i]^=(BYTE)i;
			}
			bhp->CRC=CalcSum(mb,bhp->CompressedSize);
			ar.Write(mb,bhp->CompressedSize);
			delete mb;
			
		}
		
//Names write
		if(NamesArray.GetSize())
		{
			bhp=new AVP_BlockHeader;
			memset(bhp,0,sizeof(AVP_BlockHeader));
			BlockHeaderArray.Add(bhp);

			ar.Flush();
			bhp->BlockFO=ar_fp->GetPosition();
			bhp->BlockType=BT_NAME;
			bhp->NumRecords=NamesArray.GetSize();

			CMemFile mf(0x1000);

			for(i=0;i<(int)bhp->NumRecords;i++){
				CString s=(char*)NamesArray[i];
				char nl='\n';
				mf.Write((const char*)s,s.GetLength());
				mf.Write(&nl,1);
//				ar.WriteString((char*)NamesArray[i]);
//				ar << '\n';
			}
/*			ar.Flush();
			fo=ar_fp->GetPosition();
			bhp->CompressedSize=fo-bhp->BlockFO;
			bhp->UncompressedSize=bhp->CompressedSize;
*/
			bhp->UncompressedSize=mf.GetLength();
			bhp->Compression=Compression;
			BYTE* mb;
			if(Compression==0){
				mb=mf.Detach();
				bhp->CompressedSize=bhp->UncompressedSize;
			}
			else if(Compression==1){
				CMemFile cmf(0x1000);
				mf.Seek(0,CFile::begin);
				bhp->CompressedSize=squeeze(mf,cmf,bhp->UncompressedSize,0);
				mb=cmf.Detach();
				for(UINT i=0;i<bhp->CompressedSize;i++)	mb[i]^=(BYTE)i;
			}
			bhp->CRC=CalcSum(mb,bhp->CompressedSize);
			ar.Write(mb,bhp->CompressedSize);
			delete mb;
		
		}

//Comments write
		if(Compression==0)
		if(CommentsArray.GetSize())
		{
			bhp=new AVP_BlockHeader;
			memset(bhp,0,sizeof(AVP_BlockHeader));
			BlockHeaderArray.Add(bhp);

			ar.Flush();
			bhp->BlockFO=ar_fp->GetPosition();
			bhp->BlockType=BT_COMMENT;
			bhp->NumRecords=CommentsArray.GetSize();

			for(i=0;i<(int)bhp->NumRecords;i++){
				ar.WriteString((char*)CommentsArray[i]);
				ar << '\n';
			}
			ar.Flush();
			fo=ar_fp->GetPosition();
			bhp->CompressedSize=fo-bhp->BlockFO;
			bhp->UncompressedSize=bhp->CompressedSize;
		}

//Editor write
		if(Compression==0)
		if(EditArray.GetSize())
		{
			bhp=new AVP_BlockHeader;
			memset(bhp,0,sizeof(AVP_BlockHeader));
			BlockHeaderArray.Add(bhp);

			ar.Flush();
			bhp->BlockFO=ar_fp->GetPosition();
			bhp->BlockType=BT_EDIT;
			bhp->NumRecords=EditArray.GetSize();
			bhp->RecordSize=sizeof(R_Edit);

			for(i=0;i<(int)bhp->NumRecords;i++){
				ar.Write(&(((CRecordEdit*)EditArray[i])->Type),sizeof(R_Edit));
			}

			ar.Flush();
			fo=ar_fp->GetPosition();
			bhp->CompressedSize=fo - bhp->BlockFO;
			bhp->UncompressedSize=bhp->CompressedSize;
		}
		
//BaseHeader Again write
		ar.Flush();
		BaseHeader.FileSize = ar_fp->GetPosition(); 
		BaseHeader.BlockHeaderTableSize = BlockHeaderArray.GetSize(); 
		BaseHeader.AuthenticityCRC=(CalcSum(BaseHeader.Authenticity,0x40));
		BaseHeader.HeaderCRC=(CalcSum((BYTE*)&BaseHeader.Magic,sizeof(BaseHeader)-0x84));
		ar_fp->SeekToBegin();
		ar.Write(&BaseHeader,sizeof(BaseHeader));

//BlockTableHeader Again write
		ar.Flush();
		ar_fp->Seek(BaseHeader.BlockHeaderTableFO,CFile::begin);
		for(i=0;i<(int)BaseHeader.BlockHeaderTableSize;i++)	{
			ar.Write(BlockHeaderArray[i],sizeof(AVP_BlockHeader));
		}
/*
		ar.Flush();
		AVP_LoadBase(NULL,0);
*/

		MainFramePtr->SetMessageText(ar_fp->GetFilePath()+" :   saved.");

	}




	else // TODO: add loading code here
	{
#if (defined (KL_MULTIPLATFORM_LINKS))
        g_bConversionAllowed = false;
        g_bShouldStop = false;
#endif

		MainFramePtr->SetMessageText(GetTitle()+"  Opening...");
		MainFramePtr->UpdateWindow();

		UINT bread=ar.Read(&BaseHeader,sizeof(BaseHeader));
		if(*(DWORD*)BaseHeader.Authenticity==AVP_MAGIC){
			InitHeader();
			ar.Flush();
			ar_fp->Seek(0x40,CFile::begin);
			bread=ar.Read(&BaseHeader.Magic,sizeof(BaseHeader)-0x80);
		}
		if(BaseHeader.Magic!=AVP_MAGIC)
		{
#if (defined (KL_MULTIPLATFORM_LINKS))
			AfxMessageBox ("Old format is not supported any more");
			goto abort_;
#endif

			if(((DBF_HEADER*)&BaseHeader)->id==_ID_HEADER){
				ar.Flush();
				ar_fp->Seek(0,CFile::begin);
				InitHeader();
				ReadOldBase(ar);
				LoadSuccess=1;
				MainFramePtr->SetMessageText("Done.");
				Compression=0;
				AfxGetApp()->EndWaitCursor();
				return;
			}

/*
abort_:	
			ar.Flush();
			ar_fp->Seek(0,CFile::end);
*/
		}


		UINT i;
		UINT j;
		CString *s;

		ar.Read(&ColumnCount,sizeof(int));
		if(ColumnCount<=8)
			ar.Read(ColumnCx,sizeof(int)*ColumnCount);
		ColumnCount=8;

		ar.Flush();
		ar_fp->Seek(BaseHeader.BlockHeaderTableFO,CFile::begin);
//BaseHeader read
		for(i=0;i<BaseHeader.BlockHeaderTableSize;i++){
			bhp=new AVP_BlockHeader;
			bread=ar.Read(bhp,sizeof(AVP_BlockHeader));
			if(bread!=sizeof(AVP_BlockHeader)){
				delete bhp;
				goto abort_;
			}
			BlockHeaderArray.Add(bhp);
		}
//BlockHeaderTable read
		for(i=0;i<BaseHeader.BlockHeaderTableSize;i++){
			bhp=(AVP_BlockHeader*)BlockHeaderArray[i];
			if(bhp->Compression){
				AfxMessageBox("Can't open packed Antiviral base.");
				goto abort_;
			}
			ar.Flush();
			ar_fp->Seek(bhp->BlockFO,CFile::begin);
			
			
			switch(bhp->BlockType){
			case BT_NAME:
				NamesArray.SetSize(0,bhp->NumRecords);
				for(j=0;j<bhp->NumRecords;j++){
					s=new CString;
					ar.ReadString(*s);
					NamesArray.Add(s);
				}
				break;
			case BT_COMMENT:
				CommentsArray.SetSize(0,bhp->NumRecords);
				for(j=0;j<bhp->NumRecords;j++){
					s=new CString;
					ar.ReadString(*s);
					CommentsArray.Add(s);
				}
				break;
			case BT_RECORD:
			case BT_LINK32:
			case BT_LINK16:

				ArraysTable[bhp->BlockType][bhp->RecordType].SetSize(0,bhp->NumRecords);
				for(j=0;j<bhp->NumRecords;j++){
					BYTE* re;
					if(bhp->RecordSize){
						re=new BYTE[bhp->RecordSize];
						ar.Read(re,bhp->RecordSize);
					}
					else{
						DWORD l;
						ar.Read(&l,sizeof(DWORD));
						re=new BYTE[l];
						*(DWORD*)re=l;
						ar.Read(re+sizeof(DWORD), l-sizeof(DWORD));
					}
					ArraysTable[bhp->BlockType][bhp->RecordType].Add(re);
				}
				break;

#if (defined (KL_MULTIPLATFORM_LINKS))

			case BT_EDIT:
				EditArray.SetSize(0,bhp->NumRecords);
				for(j=0;j<bhp->NumRecords;j++){
					R_Edit RE;
					ar.Read(&RE,bhp->RecordSize);
					
					CRecordEdit * re = NULL;
                    
                    if (RE.CommentIdx != -1)
                    {
                        re = new CRecordEdit (&RE, *(CString*)CommentsArray[RE.CommentIdx]);
                    }
                    else
                    {
                        re = new CRecordEdit (&RE);
                    }

                    if (g_bShouldStop)
                    {
                        delete re;
                        goto abort_;
                    }
					if(RE.NameIdx !=-1)
					re->SetName(*(CString*)NamesArray[RE.NameIdx]);
					if(RE.RecordIdx!=-1)
					re->SetRecord(ArraysTable[BT_RECORD][re->Type][RE.RecordIdx]);
					if(RE.Link16Idx!=-1)
					re->SetLink16(ArraysTable[BT_LINK16][re->Type][RE.Link16Idx]);
					if(RE.Link32Idx!=-1)
					re->SetLink32(ArraysTable[BT_LINK32][re->Type][RE.Link32Idx]);

					EditArray.Add(re);
				}
				break;
#else
			case BT_EDIT:
				EditArray.SetSize(0,bhp->NumRecords);
				for(j=0;j<bhp->NumRecords;j++){
					R_Edit RE;
					ar.Read(&RE,bhp->RecordSize);
					
					CRecordEdit* re=new CRecordEdit(&RE);
//					if(RE.NameIdx >= (UINT)NamesArray.GetSize())	RE.NameIdx=(ULONG)-1;
					if(RE.NameIdx !=-1)
					re->SetName(*(CString*)NamesArray[RE.NameIdx]);
					if(RE.CommentIdx!=-1)
					re->SetComment(*(CString*)CommentsArray[RE.CommentIdx]);
					if(RE.RecordIdx!=-1)
					re->SetRecord(ArraysTable[BT_RECORD][re->Type][RE.RecordIdx]);
					if(RE.Link16Idx!=-1)
					re->SetLink16(ArraysTable[BT_LINK16][re->Type][RE.Link16Idx]);
					if(RE.Link32Idx!=-1)
					re->SetLink32(ArraysTable[BT_LINK32][re->Type][RE.Link32Idx]);

					EditArray.Add(re);
/*
					__int64 unic=0;
					if(RE.NameIdx!=-1){
						unic=GetAvpBaseId(re->Record,re->Type,re->Name);
					}
					if(unic){
						SetField(unic, FIELD_COMMENT, (void*)(const char*)re->Comment, strlen(re->Comment)+1);
					}
*/
				}
				break;
#endif // KL_MULTIPLATFORM_LINKS
			default:break;
			}
		}
		LoadSuccess=1;
		MainFramePtr->SetMessageText(ar_fp->GetFilePath()+" :   loaded.");
abort_:
		FreeAllStr(NamesArray);
		FreeAllStr(CommentsArray);
	}
	UpdateAllViews(NULL);
	FreeAllPtr(BlockHeaderArray);
	Compression=0;
	AfxGetApp()->EndWaitCursor();
}

BYTE JMT[]={
//#define		JMP_OFFSET			0
JMP_OFFSET,
//#define		JMP_ADDRESS			1
JMP_ADDRESS,
//#define		JMP_DATA			2
JMP_DATA,
//#define		JMP_EXE				3
JMP_SPC,
//#define		JMP_SYS				4
JMP_SPC,
//#define		JMP_SPC				5
JMP_FAR_A
};

BYTE MMT[]={
//#define		_TSR_ADDRESS			0
MEM_ADDRESS,
//#define		_TSR_CUT				1
MEM_CUT,
//#define		_TSR_MCB				2
MEM_MCB,
//#define		_TSR_TRACE			3
MEM_TRACE,
//#define		_TSR_SCAN			4
MEM_SCAN,
//#define		_TSR_FULL_SCAN		5
MEM_FULL_SCAN,
//#define		_TSR_SPC				6
MEM_SPC
};

BYTE SCMT[]={
//#define		_SECTOR_ADDRESS		0
SECT_ADDRESS,
//#define		_SECTOR_ABSOLUTE		1
SECT_ABSOLUTE,
//#define		_SECTOR_LOGICAL		2
SECT_LOGICAL,
//#define		_SECTOR_DELETE		3
SECT_OVERWRITE,
//#define		_SECTOR_SPC			4
SECT_SPC,
//#define		_SECTOR_FAIL			5
SECT_FAIL
};

BYTE FCMT[]={
//_FILE_MOV			0
FILE_MOVE,
//_FILE_LEH			1
FILE_LEHIGH,
//_FILE_JER			2
FILE_JERU,
//_FILE_STA			3
FILE_START,
//_FILE_EXE_CISS	4
FILE_E_CISS,
//_FILE_EXE_CISS_10	5
FILE_E_CISS_10,
//_FILE_EXE_CIS		6
FILE_E_CIS,
//_FILE_EXE_CIS_10	7
FILE_E_CIS_10,
//_FILE_EXE_CI		8
FILE_E_CI,
//_FILE_EXE_CI_10	9
FILE_E_CI_10,
//_FILE_SYS_SI		10
FILE_SYS_SI,
//_FILE_SYS_I		11
FILE_SYS_SI,							///????????????????????????????????
//_FILE_DELETE		12
FILE_DELETE,
//_FILE_SPC			13
FILE_SPC,
//_FILE_FAIL		14
FILE_FAIL
};

WORD PTO[]={
//#define		_HEADER			0
HEADER_BEGIN,
//#define		_PAGE_A			1
PAGE_A_BEGIN,
//#define		_PAGE_B			2
PAGE_B_BEGIN,
//#define		_PAGE_C			3
PAGE_C_BEGIN,
//#define		_DUMMY			4
0,0
};

void CAvpEditDoc::ReadOldBase(CArchive& ar)
{
	CFile* ar_fp=ar.GetFile();
	BYTE* buf;
	DWORD off;
	CRecordEdit* re=NULL;

	DWORD len=ar_fp->GetLength();
	if(len> 1024000 )return;
	buf=new BYTE[len];

	UINT bread=ar.Read(buf,len);
	if(bread!=len)return;

	if(((DBF_HEADER*)buf)->version != _VERSION ) goto fc;
	if(((DBF_HEADER*)buf)->id != _ID_HEADER ) goto fc;
	off=((DBF_HEADER*)buf)->First_Record;


	while(off+sizeof(DBF_RCB)<len){
		DBF_RCBT* rcb=(DBF_RCBT*)(buf+off);
		switch(rcb->id){
		case _ID_RECORD:
			if(rcb->type & _FILE_)
			{
				DB_FILE_RECORD* o_rec=(DB_FILE_RECORD*)(buf+off+sizeof(DBF_RCB));
				char* name = (char*)o_rec->Name;
				if(*name==0xFF){
					name++;
					re=new CRecordEdit(RT_SEPARATOR);
				}
				else
				{
					re=new CRecordEdit(RT_FILE);

					R_File* rec=new R_File;
					ASSERT(rec !=NULL);

					rec->Type=0;
					if(o_rec->Type & _COM_)rec->Type |= ST_COM;
					if(o_rec->Type & _EXE_)rec->Type |= ST_EXE;
					if(o_rec->Type & _SYS_)rec->Type |= ST_SYS;
					if(o_rec->Type & _WIN_)rec->Type |= ST_NE;

					rec->ControlWord=0;
					rec->Len1=o_rec->Len_1;
					rec->Len2=o_rec->Len_2;
					rec->Sum1=o_rec->Sum_1;
					rec->Sum2=o_rec->Sum_2;

					rec->Off1=o_rec->Off_1 + PTO[o_rec->Page_1];
					rec->Off2=o_rec->Off_2 + PTO[o_rec->Page_2];

					rec->CureData1=o_rec->Cure_Data_1;
					rec->CureData2=o_rec->Cure_Data_2;
					rec->CureData3=o_rec->Cure_Data_3;
					rec->CureData4=o_rec->Cure_Data_4;
					rec->CureData5=o_rec->Cure_Cut;
					
					rec->CureMethod=FCMT[o_rec->Cure_Method];
					WORD poff=PTO[o_rec->Cure_Page];

					if(*DLL_GetMethodText(1,rec->CureMethod)[1]=='*') rec->CureData1+=poff;
					if(*DLL_GetMethodText(1,rec->CureMethod)[2]=='*') rec->CureData2+=poff;
					if(*DLL_GetMethodText(1,rec->CureMethod)[3]=='*') rec->CureData3+=poff;
					if(*DLL_GetMethodText(1,rec->CureMethod)[4]=='*') rec->CureData4+=poff;
					if(*DLL_GetMethodText(1,rec->CureMethod)[5]=='*') rec->CureData5+=poff;

					if(o_rec->Cure_Page==_DUMMY) rec->CureMethod |= 0x80;

					re->SetRecord(rec);
				}
				re->SetName(name);
				EditArray.Add(re);
			}
			else if(rcb->type & _SECTOR_)
			{
				DB_SECTOR_RECORD* o_rec=(DB_SECTOR_RECORD*)(buf+off+sizeof(DBF_RCB));
				char* name = (char*)o_rec->Name;
				
				if(	re->Type == RT_SECTOR && *name == '#' )
				{
					R_Sector* rec=(R_Sector*)(re->Record);
					if(	(
							((rec->CureObjAndMethod1 & 0xF0)==ST_FDBOOT
							&& o_rec->Type == _MBR_) 
							|| 
							((rec->CureObjAndMethod1 & 0xF0)==ST_MBR
							&& o_rec->Type == _BOOT_ )
						)
						&& o_rec->Sum_1==rec->Sum1
						&& o_rec->Sum_2==rec->Sum2
						&& o_rec->Len_1==rec->Len1
						&& o_rec->Len_2==rec->Len2
						&& o_rec->Off_1==rec->Off1
						&& (o_rec->Off_2 + PTO[o_rec->Page_2])==rec->Off2
					){
						rec->CureData2=(BYTE)o_rec->Cure_Offset;
						rec->CureAddr21=o_rec->Cure_Addr_A;
						rec->CureAddr22=o_rec->Cure_Addr_B;
						
						rec->CureObjAndMethod2=SCMT[o_rec->Cure_Method];
						WORD poff=PTO[o_rec->Cure_Page];

						if(*DLL_GetMethodText(2,rec->CureObjAndMethod2)[1]=='*') rec->CureAddr21+=poff;
						if(*DLL_GetMethodText(2,rec->CureObjAndMethod2)[2]=='*') rec->CureAddr22+=poff;

						if(o_rec->Type & _MBR_) rec->CureObjAndMethod2 |= ST_MBR;
						if(o_rec->Type & _BOOT_)rec->CureObjAndMethod2 |= ST_FDBOOT;

						break;
					}
				}

				
				re=new CRecordEdit(RT_SECTOR);
				R_Sector* rec=new R_Sector;
				ASSERT(rec !=NULL);

				rec->ControlWord=0;
				rec->Len1=o_rec->Len_1;
				rec->Len2=o_rec->Len_2;
				rec->Sum1=o_rec->Sum_1;
				rec->Sum2=o_rec->Sum_2;
				rec->Off1=o_rec->Off_1;
				rec->Off2=o_rec->Off_2 + PTO[o_rec->Page_2];

				rec->CureData1=(BYTE)o_rec->Cure_Offset;
				rec->CureAddr11=o_rec->Cure_Addr_A;
				rec->CureAddr12=o_rec->Cure_Addr_B;
				
				rec->CureObjAndMethod1=SCMT[o_rec->Cure_Method];
				WORD poff=PTO[o_rec->Cure_Page];

				if(*DLL_GetMethodText(2,rec->CureObjAndMethod1)[1]=='*') rec->CureAddr11+=poff;
				if(*DLL_GetMethodText(2,rec->CureObjAndMethod1)[2]=='*') rec->CureAddr12+=poff;

				if(o_rec->Type & _MBR_) rec->CureObjAndMethod1 |= ST_MBR;
				if(o_rec->Type & _BOOT_)rec->CureObjAndMethod1 |= ST_FDBOOT;

				rec->CureObjAndMethod2=SECT_FAIL;
				rec->CureData2=0;
				rec->CureAddr21=0;
				rec->CureAddr22=0;
				
				re->SetRecord(rec);
				re->SetName(name);
				EditArray.Add(re);
			}
			else if(rcb->type & _TSR_)
			{
				DB_TSR_RECORD* o_rec=(DB_TSR_RECORD*)(buf+off+sizeof(DBF_RCB));
				char* name = (char*)o_rec->Name;

				re=new CRecordEdit(RT_MEMORY);
				R_Memory* rec=new R_Memory;

				rec->Method = MMT[o_rec->Find_Method];
				rec->Offset = o_rec->Off_1;
				rec->Segment = o_rec->Segment;
				rec->Len1 = o_rec->Len_1;
				rec->Sum1 = o_rec->Sum_1;
				rec->ControlByte = o_rec->Control_Byte;

				rec->CureOffset = o_rec->Off_2;
				rec->CureLength = o_rec->Len_2;
				rec->CureData[0] = o_rec->Replace_Bytes[0];
				rec->CureData[1] = o_rec->Replace_Bytes[1];
				rec->CureData[2] = o_rec->Replace_Bytes[2];
				rec->CureData[3] = o_rec->Replace_Bytes[3];
				rec->CureData[4] = o_rec->Replace_Bytes[4];
				
				re->SetRecord(rec);
				re->SetName(name);
				EditArray.Add(re);
			}
			else if(rcb->type & _JMP_)
			{
				DB_JMP_RECORD* o_rec=(DB_JMP_RECORD*)(buf+off+sizeof(DBF_RCB));
				char* name = (char*)o_rec->Name;

				re=new CRecordEdit(RT_JUMP);
				R_Jump* rec=new R_Jump;
				memset(rec,0,sizeof(R_Jump));

				rec->Len1		= o_rec->Off_2 + o_rec->Len_2;
				for(int j=0;j<o_rec->Len_1;j++){
					if(j<32)rec->BitMask1|=1<<j;
					else	rec->BitMask2|=1<<(j-32);
				}
				for(j=o_rec->Off_2; j<(o_rec->Off_2 + o_rec->Len_2); j++){
					if(j<32)rec->BitMask1|=1<<j;
					else	rec->BitMask2|=1<<(j-32);
				}

				rec->Method		= JMT[o_rec->Jmp_Method];
				rec->Data1		= o_rec->Jmp_Data;
				
				re->SetRecord(rec);
				re->SetName(name);
				EditArray.Add(re);

			}
			else if(rcb->type & _EMPTY_)
			{
				re=new CRecordEdit(RT_SEPARATOR);
				EditArray.Add(re);
			}
			else re=NULL;

			break;

		case _ID_COMMENT:
			if(re!=NULL)
			{
				re->SetComment((const char*)(buf+off+sizeof(DBF_RCB)));
			}
			break;
		case _ID_LINK:
			if(re!=NULL)
			{
				CString PublicNames;
				CPtrArray Array;
				BYTE b=0;
				DWORD i=off+sizeof(DBF_RCB);
				while(i < off + rcb->size + sizeof(DBF_RCB))
				{
					ObjRecHead* orh=(ObjRecHead*)(buf+i);
					switch(orh->type)
					{
					case PUBDEF:
						{
							char* pdptr=(char*)buf+i+sizeof(ObjRecHead);
							CString sn(pdptr+3,*(pdptr+2));
							PublicNames+=(sn[0]=='_')?(((const char*)sn)+1):sn;
							PublicNames+=" ";
						}
						break;
					case MODEND:
						i=off + rcb->size + sizeof(DBF_RCB);
					default:
						break;
					}
					i+=orh->length + sizeof(ObjRecHead);
				}
				
				int pnl=PublicNames.GetLength() + 1;
				int total=rcb->size+pnl+2*sizeof(DWORD);

				char* lnk=new char[total];
				memcpy(lnk,&total,sizeof(DWORD));
				memcpy(lnk+sizeof(DWORD),&pnl,sizeof(DWORD));
				memcpy(lnk+2*sizeof(DWORD),(const char*)PublicNames,pnl);
				memcpy(lnk+pnl+2*sizeof(DWORD),buf+off+sizeof(DBF_RCB),rcb->size);

				re->SetLink16(lnk);
			}
			break;
		default:
			 goto fc;
		}
		off += sizeof(DBF_RCB) + rcb->size;
	}

fc:
	delete buf;
}


/////////////////////////////////////////////////////////////////////////////
// CAvpEditDoc diagnostics

#ifdef _DEBUG
void CAvpEditDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CAvpEditDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CAvpEditDoc commands

/*
void CAvpEditDoc::InitialUpdateView(CView* view)
{

	ScanThreadPtr=AfxBeginThread(Scan,this);
	
	
	int j=EditArray.GetSize();
	for(int i=0;i<j;i++){
		UVStruct uv;
		uv.Sender=view; //NULL
		uv.Index=0;
		uv.REptr=(CRecordEdit*)EditArray[i];
//		UpdateAllViews(NULL,UV_ADD,&uv);

		UpdateAllViews(NULL,UV_ADD,&uv);
	}
}
*/
int CAvpEditDoc::DeleteRecord(int i)
{
	CRecordEdit* re;
	re=(CRecordEdit*)EditArray[i];
	if (re==NULL) return 0;
	delete re;
	EditArray.RemoveAt(i);

	SetModifiedFlag();

	UVStruct uv;
	uv.Sender=NULL;
	uv.Index=i;
	uv.REptr=NULL;
	UpdateAllViews(NULL,UV_DELETE,&uv);
	return 1;
}



void CAvpEditDoc::InsertRecord(int i,CRecordEdit* re)
{
	if(re==NULL)return;
	EditArray.InsertAt(i,re);

	SetModifiedFlag();

	UVStruct uv;
	uv.Sender=NULL;
	uv.Index=i;
	uv.REptr=re;
	UpdateAllViews(NULL,UV_INSERT,&uv);
}

int CAvpEditDoc::EditRecord(int i)
{
	int ret =IDCANCEL;
	CRecordEdit* re=(CRecordEdit*)EditArray[i];
	if(re==NULL)return ret;
	
	ret = re->EditRecord();
	
	if(ret==IDOK){

		SetModifiedFlag();
	
		UVStruct uv;
		uv.Sender=NULL;
		uv.Index=i;
		uv.REptr=re;
		UpdateAllViews(NULL,UV_CHANGE,&uv);
	}
	return ret;
}


int CAvpEditDoc::ClipboardCount()
{
	return Clipboard.GetSize();
}

void CAvpEditDoc::ClipboardFree()
{
	int i;
	while(i=Clipboard.GetSize()){
		i--;
		CRecordEdit* ptr=(CRecordEdit*)Clipboard[i];
		if(ptr!=NULL)delete ptr; 
		Clipboard.RemoveAt(i);
	}
}

int CAvpEditDoc::ClipboardCopy(int i)
{
	CRecordEdit* ptr=new CRecordEdit((CRecordEdit*)EditArray[i]);
	Clipboard.Add(ptr);
	return i;
}

int CAvpEditDoc::ClipboardPaste(int idx)
{
	UVStruct uv;
	uv.Sender=NULL;
	int n=Clipboard.GetSize();
	for(int i=0;i<n;i++)
	{
		CRecordEdit* ptr=new CRecordEdit((CRecordEdit*)Clipboard[i]);

		ptr->SetModifyFlag();
		EditArray.InsertAt(idx,ptr);
		uv.Index=idx;
		uv.REptr=ptr;
		UpdateAllViews(NULL,UV_INSERT,&uv);

		idx++;
	}
	SetModifiedFlag();
	return i;

}


BOOL CAvpEditDoc::InitHeader()
{
	memset(&BaseHeader,0,sizeof(BaseHeader));
//	strcpy((char *)(BaseHeader.Text),"AVP Antiviral Database. (c)Kaspersky Lab 1997-1998.");
	*(WORD*)(BaseHeader.Text+0x3e)=0x0A0D;
	memcpy(BaseHeader.Authenticity,Authenticity,0x40);

	BaseHeader.Magic=AVP_MAGIC;
	BaseHeader.VerH=   0x03;
	BaseHeader.VerL=   0x00;
	BaseHeader.Flags=0x0001;
	BaseHeader.NumberOfRecordBlockTypes=NUMBER_OF_RECORD_BLOCK_TYPES;
	BaseHeader.NumberOfRecordTypes=NUMBER_OF_RECORD_TYPES;
	
	SYSTEMTIME st;
	GetLocalTime(&st);

	EditArray.SetSize(0,200);

	return TRUE;
}
  
BOOL CAvpEditDoc::OnOpenDocument(LPCTSTR lpszPathName) 
{
	LoadSuccess=0;
	if (!CDocument::OnOpenDocument(lpszPathName))
		return FALSE;
	
	if(!LoadSuccess)return FALSE;
	
	return TRUE;
}

#define GET_NAME_BUF_SIZE 0x400

const char* CAvpEditDoc::MakeFullName(int index)
{
	char s[GET_NAME_BUF_SIZE];
	static CString line;
	const char *p;
	if(EditArray.GetSize()<=index)return _T("");
	CRecordEdit* ptr=(CRecordEdit*)EditArray[index];
	if(ptr->Type==RT_SEPARATOR)	return ptr->GetComment();
	CString comment=ptr->GetComment();

	do{
		if(index<0)return _T("");
		ptr=(CRecordEdit*)EditArray[index];
		p=ptr->Name;
		index--;
	}while(*p=='#' || !*p);

	int l=GET_NAME_BUF_SIZE-10;
	l-=strlen(p);
	strcpy(s+l,p);
	for(int i=0;p[i]=='.' && i<16;i++) l++;

	while(i && index>=0){
		const char* cp;
		ptr=(CRecordEdit*)EditArray[index];
		p=ptr->Name;
		index--;
		if(*p=='#' || !*p)continue;
again:
		cp=p;
		for(int k=i-1;k;k--){
			cp=strchr(cp,'.');
			if(cp==NULL)cp=".";
			cp++;
		}
		if(*cp!='.'){
			s[--l]='.';
			for( int n=0; n<16 && cp[n]!=0 && cp[n]!='.' && l>2 ; n++)l--;
			strncpy(s+l,cp,n);
			if(--i)
				goto again;
		}
	}
	

	if(comment.GetLength()){
		line.Format("%s    ( %s )",s+l,(const char*)comment);
	}else line=s+l;

	return line;
}

extern BOOL SignFile(const char* lpszPathName);

BOOL CAvpEditDoc::OnSaveDocument(LPCTSTR lpszPathName) 
{
	if(OptDlg.m_SaveBackup && !_access(lpszPathName,6))
	{
		CString s=lpszPathName;
		int t=s.GetLength()-1;
		char* c=(char*)(const char*)s;
		c[t]='~';	
		CopyFile(lpszPathName,s,FALSE);
	}
	BOOL ret=CDocument::OnSaveDocument(lpszPathName);
	if(!_access(lpszPathName,6))
		ret = ret && !SignFile(lpszPathName);

	return ret;
}
