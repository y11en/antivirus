#include "stdafx.h"
#include "AvpEdit.h"

#if (defined (KL_MULTIPLATFORM_LINKS))
    #include <new>
#endif

#include <Bases/Format/Base.h>
#include <ScanAPI/types.h>
#include <Bases/Format/Records.h>
#include <fileformat/coff.h>
#include "../Obj16.h"

#include <ScanAPI/Scanobj.h>
#include "../basework/WorkArea.h"
#include <Bases/Method.h>

#include "REdit.h"
#include "OptDlg.h"

#include "dialogs.h"


static int RSTable[]={
	sizeof(R_Kernel),
	sizeof(R_Jump),
	sizeof(R_Memory),
	sizeof(R_Sector),
	sizeof(R_File),
	sizeof(R_CA),
	sizeof(R_Unpack),
	sizeof(R_Extract),
};

CRecordEdit::CRecordEdit(WORD type):CObject()
{
	SubType = 0;
	Record = NULL;
	Link16Buffer = NULL;
	Link32Buffer = NULL;
	ModifyFlag = 0;

	Type=(type==8)?RT_SEPARATOR:type;
}

#if (!defined (KL_MULTIPLATFORM_LINKS))
CRecordEdit::CRecordEdit(void* ptr):CObject()
{
	memcpy(&Type,ptr,sizeof(R_Edit));

	SubType = 0;
	Record = NULL;
	Link16Buffer = NULL;
	Link32Buffer = NULL;
	ModifyFlag = 0;

}
#else
CRecordEdit::CRecordEdit (R_Edit * ptr, const char * extraInfoString)
:   ExtraInfo (extraInfoString ? extraInfoString : "")
{
	memcpy(&Type,ptr,sizeof(R_Edit));

	SubType = 0;
	Record = NULL;
	Link16Buffer = NULL;
	Link32Buffer = NULL;
	ModifyFlag = 0;

    extern bool g_bShouldStop;
    extern bool g_bConversionAllowed;
    
    if (!g_bConversionAllowed && !ExtraInfo.IsMyData())
    {
        if (IDYES == AfxMessageBox ("Do you want this base converted to the new format?", MB_YESNO))
        {
            g_bConversionAllowed = true;
        }
        else
        {
            g_bShouldStop = true;
            return;
        }
    }
    
//    ASSERT (g_bConversionAllowed);
    
    VERIFY (ExtraInfo.ConvertOldData());
    Comment = ExtraInfo.GetComment();
}
#endif

CRecordEdit::CRecordEdit(CRecordEdit* re):CObject()
{
	memcpy(&Type,&(re->Type),sizeof(R_Edit));

	Name=re->Name;
	Comment=re->Comment;

	SubType = re->SubType;
	Record = NULL;
	Link16Buffer = NULL;
	Link32Buffer = NULL;
	ModifyFlag = 0;

	if(re->Record!=NULL){
		Record=new BYTE[RSTable[Type]];
		memcpy(Record,re->Record,RSTable[Type]);
	}
	if(re->Link16Buffer!=NULL){
		Link16Buffer=new BYTE[*(DWORD*)re->Link16Buffer];
		memcpy(Link16Buffer,re->Link16Buffer,*(DWORD*)re->Link16Buffer);
	}
	if(re->Link32Buffer!=NULL){
		Link32Buffer=new BYTE[*(DWORD*)re->Link32Buffer];
		memcpy(Link32Buffer,re->Link32Buffer,*(DWORD*)re->Link32Buffer);
	}
}

CRecordEdit::~CRecordEdit()
{
	if(Record)delete Record;
	if(Link16Buffer)delete Link16Buffer;
	if(Link32Buffer)delete Link32Buffer;
}

static char* TypeStrings[]={
_T("Kernel"),
_T("Jump"),
_T("Mem"),
_T("Sector"),
_T("File"),
_T("CA"),
_T("Unpack"),
_T("Extract"),
_T("")
};

static char* SubTypeStrings[]={
_T("Com "),
_T("Exe "),
_T("Sys "),
_T("NE "),
_T("OLE2 "),
_T(""),
_T(""),
_T("")
};

static char* SubTypeSector[]={
_T("A"),
_T("F"),
_T("H"),
_T("M"),
_T("")
};


extern COptDlg OptDlg;
static	CString s;

const char*	CRecordEdit::GetComment(){
	if(Type == RT_SEPARATOR){
		s.Format("%s  %s",OptDlg.m_SepLine,Comment);
		return s;
	}
	return Comment;
}

const char*	CRecordEdit::GetName(){
	if(Type == RT_SEPARATOR) return _T("");
	return Name;
}

const char*	CRecordEdit::GetTypeString()
{
	if(Type == RT_SEPARATOR) return _T("");
	return TypeStrings[Type];
}

const char*	CRecordEdit::GetMethodString()
{
	BYTE m;
	int i;
	int j=0;
	s.Empty();
	switch(Type){
	case RT_FILE:
		i=1;
		m=((R_File*)Record)->CureMethod & 0x7F;
		break;
	case RT_SECTOR:
		i=2;
		m=((R_Sector*)Record)->CureObjAndMethod1 & 0x0F;
		if(m < DLL_GetMethodCount(i))
			s=(char*)DLL_GetMethodText(i,m)[0];
		else s+="!!!ERROR!!!";
		s+=" / ";
		m=((R_Sector*)Record)->CureObjAndMethod2 & 0x0F;
		break;
	case RT_JUMP:
		i=0;
		m=((R_Jump*)Record)->Method;
		break;
	case RT_MEMORY:
		i=3;
		m=0;
		j=((R_Memory*)Record)->Method;
		break;
	default:
		return "";
	}
	if(m < DLL_GetMethodCount(i))
		s+=(char*)DLL_GetMethodText(i,m)[j];
	else s+="!!!ERROR!!!";
	return s;
}

const char*	CRecordEdit::GetSubTypeString()
{
	int t;
	s.Empty();
	switch(Type){
	case RT_FILE:
	case RT_CA:
	case RT_UNPACK:
	case RT_EXTRACT:
		for(t=0;t<8;t++)
			if(SubType & (1<<t)) s += SubTypeStrings[t];
		break;
	case RT_SECTOR:
		{
			R_Sector* r=(R_Sector*)Record;
			if(r->CureObjAndMethod1 & ST_ABOOT
			|| r->CureObjAndMethod2 & ST_ABOOT)	s += SubTypeSector[0];
			if(r->CureObjAndMethod1 & ST_FDBOOT
			|| r->CureObjAndMethod2 & ST_FDBOOT)s += SubTypeSector[1];
			if(r->CureObjAndMethod1 & ST_HDBOOT
			|| r->CureObjAndMethod2 & ST_HDBOOT)s += SubTypeSector[2];
			if(r->CureObjAndMethod1 & ST_MBR
			|| r->CureObjAndMethod2 & ST_MBR)	s += SubTypeSector[3];
		}
		break;
	default:
		break;
	}
	return s;
}

const char*	CRecordEdit::GetLink16String()
{
	if(Link16Buffer){
//		return (const char*)Link16Buffer+8;
		s.Empty();
		BYTE* buf=Link16Buffer;
		DWORD len=*(DWORD*)(Link16Buffer);
		DWORD i= *(DWORD*)(Link16Buffer+4) + 8;
		ObjRecHead* orh=(ObjRecHead*)(buf+i);
		while(i<len)
		{
			orh=(ObjRecHead*)(buf+i);
			switch(orh->type)
			{
			case MODEND:	i=len;	break;
			case THEADR:
				if(OptDlg.m_ShowSourceFile)
				{
					char* ptr=(char*)buf+i+sizeof(ObjRecHead);
					s+="<";
					s+=CString(ptr+1,*ptr);
					s+=">";
				}
				break;
			case PUBDEF:
				{
					char* pdptr=(char*)buf+i+sizeof(ObjRecHead);
					char* ptr=pdptr;
					if(*(WORD*)ptr==0)ptr+=2;
					ptr+=2;
					for(; ptr < pdptr + orh->length - 1; ptr+= *ptr+4)
					{
						s+=" ";
						s+=CString(ptr+1,*ptr);
					}
				}
				break;
			default:	break;
			}
			i+=orh->length + sizeof(ObjRecHead);
		}
		return s;
	}
	return _T("");
}

const char*	CRecordEdit::GetLink32String()
{
	if(Link32Buffer){
//		return (const char*)Link32Buffer+8;
		s.Empty();
		BYTE* buf=Link32Buffer + *(DWORD*)(Link32Buffer+4) + 8;
		COFF_Header* Header=(COFF_Header*)buf;
		COFF_Symbol* Symbol=(COFF_Symbol*)(buf+Header->P_SymbolTable);;

		for(DWORD i=0;i<Header->N_Symbols;i+=Symbol[i].Skip, i++){
			CString sn;
			if(Symbol[i].Type == 0x67)
				if(OptDlg.m_ShowSourceFile)
			{
				s+="<";
				s+=(char*)(Symbol+i+1);
				s+=">";
				continue;
			}

			if(Symbol[i].Type != 0x02) continue; //Not code
			if(Symbol[i].Section == 0) continue; //External
			if((Symbol[i].Access & 0x20) == 0)continue; //Not function
			if(Symbol[i].NameWord ==0)
				sn=(char*)(Symbol+Header->N_Symbols)+Symbol[i].NameOffset;
			else{
				sn=Symbol[i].Name;
				if(sn.GetLength()>8)
					sn=sn.Left(8);
			}

			s+=" ";
			s+=sn;
		}
		return s;
	}
	return _T("");
}

CRecordEdit::	SetRecord(void* ptr){
	Record=(BYTE*)ptr;
	switch(Type){
	case RT_FILE:
	case RT_CA:
	case RT_UNPACK:
	case RT_EXTRACT:
		SubType=((R_File*)ptr)->Type;
		break;
	default:
		break;
	}
	return Type;
}
CRecordEdit::	SetLink16(void* ptr){
	Link16Buffer=(BYTE*)ptr;
	return *(int*)ptr;
}
CRecordEdit::	SetLink32(void* ptr){
	Link32Buffer=(BYTE*)ptr;
	return *(int*)ptr;
}

CRecordEdit::	MakeNameIdx(CPtrArray& array, BOOL pack, BOOL init){
	static int off;
	static int len;
	if(init) len=off=0;
		
	if(pack && len && *(const char*)Name=='#')
		NameIdx=array.GetUpperBound();
	else
	{
		off+=len;
		len=Name.GetLength();
		if(len){
			NameIdx=array.Add((void*)(const char*)Name);
			len++;
		} else NameIdx=(DWORD)-1;
	}

	switch(Type)
	{
	case RT_MEMORY:	((R_Memory*)Record)->NameIdx=off; break;
	case RT_SECTOR:	((R_Sector*)Record)->NameIdx=off; break;
	case RT_FILE:	((R_File*)Record)->NameIdx=off; break;
	case RT_CA:		((R_CA*)Record)->NameIdx=off; break;
	case RT_UNPACK:	((R_Unpack*)Record)->NameIdx=off; break;
	case RT_EXTRACT:((R_Extract*)Record)->NameIdx=off; break;
	default:	break;
	}
	return NameIdx;
}
#if (!defined (KL_MULTIPLATFORM_LINKS))
CRecordEdit::	MakeCommentIdx(CPtrArray& array){
	CommentIdx=(Comment.GetLength())?array.Add((void*)(const char*)Comment):-1;
	return CommentIdx;
}
#else
CRecordEdit::	MakeCommentIdx(CPtrArray& array){
    ExtraInfo.SetComment (Comment);
    if (SerializedExtraInfo = const_cast<char *> (ExtraInfo.Serialize()))
    {
        CommentIdx = array.Add (SerializedExtraInfo);
    }
    else
    {
        CommentIdx = -1;
    }

    return CommentIdx;
}
#endif // KL_MULTIPLATFORM_LINKS
CRecordEdit::	MakeRecordIdx(CPtrArray& array){
	RecordIdx=(Record==NULL)?-1:array.Add(Record);
	return RecordIdx;
}
CRecordEdit::SetRecordLinkIdx(int Idx)
{
	switch(Type)
	{
	case RT_KERNEL:	((R_Kernel*)Record)->LinkIdx=Idx; break;
	case RT_JUMP:	((R_Jump*)Record)->LinkIdx=Idx; break;
	case RT_MEMORY:	((R_Memory*)Record)->LinkIdx=Idx; break;
	case RT_SECTOR:	((R_Sector*)Record)->LinkIdx=Idx; break;
	case RT_FILE:	((R_File*)Record)->LinkIdx=Idx; break;
	case RT_CA:		((R_CA*)Record)->LinkIdx=Idx; break;
	case RT_UNPACK:	((R_Unpack*)Record)->LinkIdx=Idx; break;
	case RT_EXTRACT:((R_Extract*)Record)->LinkIdx=Idx; break;
	default:	break;
	}
	return Idx;
}
CRecordEdit::	MakeLinkIdx(CPtrArray& link32array, CPtrArray& link16array){
/*	if(Link16Buffer && !Link32Buffer){
		Link32Buffer=(BYTE*)new COFF_Header;
		memset(Link32Buffer,0,sizeof(COFF_Header));
		((COFF_Header*)Link32Buffer)->Machine=0x014c;
	}
*/
	Link32Idx=(Link32Buffer==NULL)?-1:link32array.Add(Link32Buffer);
	Link16Idx=(Link16Buffer==NULL)?-1:link16array.Add(Link16Buffer);

	if(OptDlg.m_LinkConfirm && (Link32Idx != Link16Idx)){
		CString s=":ERROR! 16-32bit link mismatch.";
		AfxMessageBox(s);
	}

	SetRecordLinkIdx(Link32Idx);
	return Link32Idx;
}

WORD WorkAreaPage[][2]={
	{HEADER_BEGIN,HEADER_END},
	{PAGE_A_BEGIN,PAGE_A_END},
	{PAGE_B_BEGIN,PAGE_B_END},
	{PAGE_C_BEGIN,PAGE_C_END},
	{PAGE_E_BEGIN,PAGE_E_END},
	{0,0}
};


WORD MakeAreaPage(WORD word, int& Page){
	for(int i=0;i<PAGE_COUNT;i++)
	{
		if(word>=WorkAreaPage[i][0] && word<WorkAreaPage[i][1])
		{
			Page=i;
			return word-WorkAreaPage[i][0];
		}
	}
	return 0;
}


CRecordEdit::EditRecord()
{
	int ret =IDCANCEL;

	switch(Type){
	case RT_KERNEL:
		{
			if(Record==NULL){
				Record= new BYTE[RSTable[Type]];
				memset(Record,0,RSTable[Type]);
			}
			CDKernel d(this);
			d.m_Name=Name;
			d.m_Comment=Comment;
			d.m_Link16=GetLink16String();
			d.m_Link32=GetLink32String();
			
			ret=d.DoModal();

			if(ret==IDOK){
				Comment=d.m_Comment;
				Name=d.m_Name;
			}
			break;
		}

	case RT_JUMP:
		{
			if(Record==NULL){
				Record= new BYTE[RSTable[Type]];
				memset(Record,0,RSTable[Type]);
			}
			R_Jump* r=(R_Jump*)Record;
			CDJump d(this);
			d.m_Name=Name;
			d.m_Comment=Comment;
			d.m_Link16=GetLink16String();
			d.m_Link32=GetLink32String();

			d.m_Sum1=r->Sum1;
			d.m_2Bytes=r->ControlWord;
			d.m_Method=r->Method;
			d.m_Data1=r->Data1;
			d.m_Data2=r->Data2;
			d.m_Data3=r->Data3;
		
			d.m_Mask1=r->BitMask1;
			d.m_Mask2=r->BitMask2;
			d.m_Len1=r->Len1;


			ret=d.DoModal();

			if(ret==IDOK){
				Comment=d.m_Comment;
				Name=d.m_Name;

				r->Len1=d.m_Len1;
				r->Sum1=d.m_Sum1;
				r->ControlWord=d.m_2Bytes;
				r->Method=(r->Method==-1)?0:d.m_Method;
				r->Data1=d.m_Data1;
				r->Data2=d.m_Data2;
				r->Data3=d.m_Data3;
				r->BitMask1=d.m_Mask1;
				r->BitMask2=d.m_Mask2;
			}
			break;
		}

	case RT_MEMORY:
		{
			R_Memory* r;
			if(Record==NULL){
				Record= new BYTE[RSTable[Type]];
				memset(Record,0,RSTable[Type]);
				r=(R_Memory*)Record;
				r->Method=MEM_MCB;
				r->Len1=0x10;

			}
			r=(R_Memory*)Record;

			CDMemory d(this);
			d.m_Name=Name;
			d.m_Comment=Comment;
			d.m_Link16=GetLink16String();
			d.m_Link32=GetLink32String();

			d.m_Method=r->Method;
			d.m_Offset=r->Offset;
			d.m_Segment=r->Segment;
			d.m_Len1=r->Len1;
			d.m_Sum1=r->Sum1;
			d.m_Byte=r->ControlByte;
			d.m_Off2=r->CureOffset;
			d.m_Len2=r->CureLength;
			for(int i=0;i<5;i++) (d.m_Replace)[i]=(r->CureData)[i];
			
			ret=d.DoModal();

			if(ret==IDOK){
				Comment=d.m_Comment;
				Name=d.m_Name;

				r->Method=(r->Method==-1)?0:d.m_Method;

				r->ControlByte=d.m_Byte;
				r->Len1=d.m_Len1;
				r->Sum1=d.m_Sum1;
				r->Offset=d.m_Offset;
				r->Segment=d.m_Segment;
				r->CureOffset=d.m_Off2;
				r->CureLength=d.m_Len2;
				for(int i=0;i<5;i++) (r->CureData)[i]=(d.m_Replace)[i];
			}
			break;
		}
	case RT_SECTOR:
		{
			R_Sector* r;
			if(Record==NULL){
				Record= new BYTE[RSTable[Type]];
				memset(Record,0,RSTable[Type]);
				r=(R_Sector*)Record;
				r->Len1=0x10;
				r->Len2=0x40;
				r->Off1=WorkAreaPage[0][0];
				r->Off2=WorkAreaPage[0][0];
				r->CureObjAndMethod1=SECT_ADDRESS|ST_MBR;
				r->CureObjAndMethod2=SECT_ADDRESS|ST_FDBOOT;
			}
			r=(R_Sector*)Record;

			CDSector d(this);
			d.m_Name=Name;
			d.m_Comment=Comment;
			d.m_Link16=GetLink16String();
			d.m_Link32=GetLink32String();

			d.m_Method1=r->CureObjAndMethod1 & 0x0F;
			d.m_Method2=r->CureObjAndMethod2 & 0x0F;
			
			d.m_Aboot1 = (r->CureObjAndMethod1 & ST_ABOOT)?TRUE:FALSE;
			d.m_Aboot2 = (r->CureObjAndMethod2 & ST_ABOOT)?TRUE:FALSE;
			d.m_HDboot1= (r->CureObjAndMethod1 & ST_HDBOOT)?TRUE:FALSE;
			d.m_HDboot2= (r->CureObjAndMethod2 & ST_HDBOOT)?TRUE:FALSE;
			d.m_FDboot1= (r->CureObjAndMethod1 & ST_FDBOOT)?TRUE:FALSE;
			d.m_FDboot2= (r->CureObjAndMethod2 & ST_FDBOOT)?TRUE:FALSE;
			d.m_MBR1=	 (r->CureObjAndMethod1 & ST_MBR)?TRUE:FALSE;
			d.m_MBR2=	 (r->CureObjAndMethod2 & ST_MBR)?TRUE:FALSE;

			d.m_2Bytes=r->ControlWord;
			d.m_Len1=r->Len1;
			d.m_Len2=r->Len2;
			d.m_Sum1=r->Sum1;
			d.m_Sum2=r->Sum2;
			
			d.m_Off1=MakeAreaPage(r->Off1,d.m_Page1);
			d.m_Off2=MakeAreaPage(r->Off2,d.m_Page2);
			
			d.m_Data1=r->CureData1;
			d.m_Data2=r->CureData2;

			d.m_Page_C1=0;
			d.m_Page_C2=0;

			if(*DLL_GetMethodText(2,d.m_Method1)[1]=='*')
					d.m_Addr11=MakeAreaPage(r->CureAddr11,d.m_Page_C1);
			else	d.m_Addr11=r->CureAddr11;

			if(*DLL_GetMethodText(2,d.m_Method1)[2]=='*')
					d.m_Addr12=MakeAreaPage(r->CureAddr12,d.m_Page_C1);
			else	d.m_Addr12=r->CureAddr12;


			if(*DLL_GetMethodText(2,d.m_Method2)[1]=='*')
					d.m_Addr21=MakeAreaPage(r->CureAddr21,d.m_Page_C2);
			else	d.m_Addr21=r->CureAddr21;

			if(*DLL_GetMethodText(2,d.m_Method2)[2]=='*')
					d.m_Addr22=MakeAreaPage(r->CureAddr22,d.m_Page_C2);
			else	d.m_Addr22=r->CureAddr22;

			
			ret=d.DoModal();  //SECTOR

			if(ret==IDOK){
				Comment=d.m_Comment;
				Name=d.m_Name;

				r->CureObjAndMethod1=(d.m_Method1==-1)?0:d.m_Method1;
				r->CureObjAndMethod2=(d.m_Method2==-1)?0:d.m_Method2;
				r->CureObjAndMethod1|=(d.m_Aboot1)? ST_ABOOT: 0;
				r->CureObjAndMethod2|=(d.m_Aboot2)? ST_ABOOT: 0;
				r->CureObjAndMethod1|=(d.m_HDboot1)? ST_HDBOOT: 0;
				r->CureObjAndMethod2|=(d.m_HDboot2)? ST_HDBOOT: 0;
				r->CureObjAndMethod1|=(d.m_FDboot1)? ST_FDBOOT: 0;
				r->CureObjAndMethod2|=(d.m_FDboot2)? ST_FDBOOT: 0;
				r->CureObjAndMethod1|=(d.m_MBR1)? ST_MBR: 0;
				r->CureObjAndMethod2|=(d.m_MBR2)? ST_MBR: 0;

				r->ControlWord=d.m_2Bytes;
				r->Len1=d.m_Len1;
				r->Len2=d.m_Len2;
				r->Sum1=d.m_Sum1;
				r->Sum2=d.m_Sum2;
			
				r->Off1=d.m_Off1 + WorkAreaPage[d.m_Page1][0];
				r->Off2=d.m_Off2 + WorkAreaPage[d.m_Page2][0];

				r->CureAddr11=d.m_Addr11;
				r->CureAddr12=d.m_Addr12;
				r->CureData1=d.m_Data1;
				r->CureAddr21=d.m_Addr21;
				r->CureAddr22=d.m_Addr22;
				r->CureData2=d.m_Data2;

				if(*DLL_GetMethodText(2,d.m_Method1)[1]=='*')
					r->CureAddr11+=WorkAreaPage[d.m_Page_C1][0];
				if(*DLL_GetMethodText(2,d.m_Method1)[2]=='*')
					r->CureAddr12+=WorkAreaPage[d.m_Page_C1][0];

				if(*DLL_GetMethodText(2,d.m_Method2)[1]=='*')
					r->CureAddr21+=WorkAreaPage[d.m_Page_C2][0];
				if(*DLL_GetMethodText(2,d.m_Method2)[2]=='*')
					r->CureAddr22+=WorkAreaPage[d.m_Page_C2][0];
			}	
			break;
		}
	case RT_FILE:
		{
			R_File* r;
			if(Record==NULL){
				Record= new BYTE[RSTable[Type]];
				memset(Record,0,RSTable[Type]);
				r=(R_File*)Record;
				r->Type=ST_COM;
				r->Len1=0x10;
				r->Len2=0x40;
				r->Off1=WorkAreaPage[1][0];
				r->Off2=WorkAreaPage[1][0];
				r->CureMethod=FILE_MOVE;
			}
			r=(R_File*)Record;

			CDFile d(this);

			d.m_Name=Name;
			d.m_Comment=Comment;
			d.m_Link16=GetLink16String();
			d.m_Link32=GetLink32String();

			d.m_Com= (r->Type & ST_COM)?TRUE:FALSE;
			d.m_Exe= (r->Type & ST_EXE)?TRUE:FALSE;
			d.m_Sys= (r->Type & ST_SYS)?TRUE:FALSE;
			d.m_NE = (r->Type & ST_NE)?TRUE:FALSE;
			d.m_Ole2=(r->Type & ST_OLE2)?TRUE:FALSE;
			d.m_Script=(r->Type & ST_SCRIPT)?TRUE:FALSE;
			d.m_Hlp=(r->Type & ST_HLP)?TRUE:FALSE;
			

			d.m_2Bytes=r->ControlWord;
			d.m_Len1=r->Len1;
			d.m_Len2=r->Len2;
			d.m_Sum1=r->Sum1;
			d.m_Sum2=r->Sum2;
			d.m_Off1=MakeAreaPage(r->Off1,d.m_Page1);
			d.m_Off2=MakeAreaPage(r->Off2,d.m_Page2);

			d.m_Method=r->CureMethod & 0x7F;
			
			d.m_Data1=r->CureData1;
			d.m_Data2=r->CureData2;
			d.m_Data3=r->CureData3;
			d.m_Data4=r->CureData4;
			d.m_Data5=r->CureData5;
			if(r->CureMethod & 0x80) d.m_Page_C=PAGE_COUNT;
			else{
				d.m_Page_C=0;
				if(*DLL_GetMethodText(1,d.m_Method)[1]=='*')
				d.m_Data1=MakeAreaPage(r->CureData1,d.m_Page_C);
				if(*DLL_GetMethodText(1,d.m_Method)[2]=='*')
				d.m_Data2=MakeAreaPage(r->CureData2,d.m_Page_C);
				if(*DLL_GetMethodText(1,d.m_Method)[3]=='*')
				d.m_Data3=MakeAreaPage(r->CureData3,d.m_Page_C);
				if(*DLL_GetMethodText(1,d.m_Method)[4]=='*')
				d.m_Data4=MakeAreaPage(r->CureData4,d.m_Page_C);
				if(*DLL_GetMethodText(1,d.m_Method)[5]=='*')
				d.m_Data5=MakeAreaPage(r->CureData5,d.m_Page_C);
			}

			ret=d.DoModal(); //FILE

			if(ret==IDOK){
				Comment=d.m_Comment;
				Name=d.m_Name;

				r->Type=0;
				r->Type|=(d.m_Com)? ST_COM: 0;
				r->Type|=(d.m_Exe)? ST_EXE: 0;
				r->Type|=(d.m_Sys)? ST_SYS: 0;
				r->Type|=(d.m_NE)?  ST_NE: 0;
				r->Type|=(d.m_Ole2)?ST_OLE2: 0;
				r->Type|=(d.m_Script)?  ST_SCRIPT: 0;
				r->Type|=(d.m_Hlp)?  ST_HLP: 0;
				SubType=r->Type;

				r->ControlWord=d.m_2Bytes;
				r->Len1=d.m_Len1;
				r->Len2=d.m_Len2;
				r->Sum1=d.m_Sum1;
				r->Sum2=d.m_Sum2;
			
				r->Off1=d.m_Off1 + WorkAreaPage[d.m_Page1][0];
				r->Off2=d.m_Off2 + WorkAreaPage[d.m_Page2][0];

				r->CureMethod=(d.m_Method==-1)?0:d.m_Method;

				WORD poff=0;
				if(d.m_Page_C ==PAGE_COUNT)	r->CureMethod |= 0x80;
				else poff=WorkAreaPage[d.m_Page_C][0];
				r->CureData1=d.m_Data1;
				r->CureData2=d.m_Data2;
				r->CureData3=d.m_Data3;
				r->CureData4=d.m_Data4;
				r->CureData5=d.m_Data5;

				if(*DLL_GetMethodText(1,d.m_Method)[1]=='*') r->CureData1+=poff;
				if(*DLL_GetMethodText(1,d.m_Method)[2]=='*') r->CureData2+=poff;
				if(*DLL_GetMethodText(1,d.m_Method)[3]=='*') r->CureData3+=poff;
				if(*DLL_GetMethodText(1,d.m_Method)[4]=='*') r->CureData4+=poff;
				if(*DLL_GetMethodText(1,d.m_Method)[5]=='*') r->CureData5+=poff;

			}	
			break;
		}

	case RT_CA:
		{
			R_CA* r;
			if(Record==NULL){
				Record= new BYTE[RSTable[Type]];
				memset(Record,0,RSTable[Type]);
			}
			r=(R_CA*)Record;

			CDCA d(this);

			d.m_Name=Name;
			d.m_Comment=Comment;
			d.m_Link16=GetLink16String();
			d.m_Link32=GetLink32String();

			d.m_Com= (r->Type & ST_COM)?TRUE:FALSE;
			d.m_Exe= (r->Type & ST_EXE)?TRUE:FALSE;
			d.m_Sys= (r->Type & ST_SYS)?TRUE:FALSE;
			d.m_NE = (r->Type & ST_NE)?TRUE:FALSE;
			d.m_Ole2=(r->Type & ST_OLE2)?TRUE:FALSE;
			d.m_Script=(r->Type & ST_SCRIPT)?TRUE:FALSE;
			d.m_Hlp=(r->Type & ST_HLP)?TRUE:FALSE;
			
			d.m_2Bytes=r->ControlWord;
			d.m_Len1=r->Len1;
			d.m_Len2=r->Len2;
			d.m_Sum1=r->Sum1;
			d.m_Sum2=r->Sum2;
			d.m_Off1=MakeAreaPage(r->Off1,d.m_Page1);
			d.m_Off2=MakeAreaPage(r->Off2,d.m_Page2);

			d.m_FalseAlarm=d.m_Len2? TRUE: FALSE;

			ret=d.DoModal(); //CA

			if(ret==IDOK){
				Comment=d.m_Comment;
				Name=d.m_Name;

				if(!d.m_FalseAlarm)d.m_Len2=0;

				r->Type=0;
				r->Type|=(d.m_Com)? ST_COM: 0;
				r->Type|=(d.m_Exe)? ST_EXE: 0;
				r->Type|=(d.m_Sys)? ST_SYS: 0;
				r->Type|=(d.m_NE)?  ST_NE: 0;
				r->Type|=(d.m_Ole2)?ST_OLE2: 0;
				r->Type|=(d.m_Script)?  ST_SCRIPT: 0;
				r->Type|=(d.m_Hlp)?  ST_HLP: 0;
				SubType=r->Type;

				r->ControlWord=d.m_2Bytes;
				r->Len1=d.m_Len1;
				r->Len2=d.m_Len2;
				r->Sum1=d.m_Sum1;
				r->Sum2=d.m_Sum2;
			
				r->Off1=d.m_Off1 + WorkAreaPage[d.m_Page1][0];
				r->Off2=d.m_Off2 + WorkAreaPage[d.m_Page2][0];
			}	
			break;
		}

	case RT_UNPACK:
		{
			R_Unpack* r;
			if(Record==NULL){
				Record= new BYTE[RSTable[Type]];
				memset(Record,0,RSTable[Type]);
			}
			r=(R_Unpack*)Record;

			CDUnpack d(this);

			d.m_Name=Name;
			d.m_Comment=Comment;
			d.m_Link16=GetLink16String();
			d.m_Link32=GetLink32String();

			d.m_Com= (r->Type & ST_COM)?TRUE:FALSE;
			d.m_Exe= (r->Type & ST_EXE)?TRUE:FALSE;
			d.m_Sys= (r->Type & ST_SYS)?TRUE:FALSE;
			d.m_NE = (r->Type & ST_NE)?TRUE:FALSE;
			d.m_Ole2=(r->Type & ST_OLE2)?TRUE:FALSE;
			d.m_Script=(r->Type & ST_SCRIPT)?TRUE:FALSE;
			d.m_Hlp=(r->Type & ST_HLP)?TRUE:FALSE;
			
			d.m_2Bytes=r->ControlWord;
			d.m_Len1=r->Len1;
			d.m_Len2=r->Len2;
			d.m_Sum1=r->Sum1;
			d.m_Sum2=r->Sum2;
			d.m_Off1=MakeAreaPage(r->Off1,d.m_Page1);
			d.m_Off2=MakeAreaPage(r->Off2,d.m_Page2);

			ret=d.DoModal(); //UNPACK

			if(ret==IDOK){
				Comment=d.m_Comment;
				Name=d.m_Name;

				r->Type=0;
				r->Type|=(d.m_Com)? ST_COM: 0;
				r->Type|=(d.m_Exe)? ST_EXE: 0;
				r->Type|=(d.m_Sys)? ST_SYS: 0;
				r->Type|=(d.m_NE)?  ST_NE: 0;
				r->Type|=(d.m_Ole2)?ST_OLE2: 0;
				r->Type|=(d.m_Script)?  ST_SCRIPT: 0;
				r->Type|=(d.m_Hlp)?  ST_HLP: 0;
				SubType=r->Type;

				r->ControlWord=d.m_2Bytes;
				r->Len1=d.m_Len1;
				r->Len2=d.m_Len2;
				r->Sum1=d.m_Sum1;
				r->Sum2=d.m_Sum2;
			
				r->Off1=d.m_Off1 + WorkAreaPage[d.m_Page1][0];
				r->Off2=d.m_Off2 + WorkAreaPage[d.m_Page2][0];
			}	
			break;
		}
	case RT_EXTRACT:
		{
			R_Extract* r;
			if(Record==NULL){
				Record= new BYTE[RSTable[Type]];
				memset(Record,0,RSTable[Type]);
			}
			r=(R_Extract*)Record;

			CDExtract d(this);

			d.m_Name=Name;
			d.m_Comment=Comment;
			d.m_Link16=GetLink16String();
			d.m_Link32=GetLink32String();

			d.m_Com= (r->Type & ST_COM)?TRUE:FALSE;
			d.m_Exe= (r->Type & ST_EXE)?TRUE:FALSE;
			d.m_Sys= (r->Type & ST_SYS)?TRUE:FALSE;
			d.m_NE = (r->Type & ST_NE)?TRUE:FALSE;
			d.m_Ole2=(r->Type & ST_OLE2)?TRUE:FALSE;
			d.m_Script=(r->Type & ST_SCRIPT)?TRUE:FALSE;
			d.m_Hlp=(r->Type & ST_HLP)?TRUE:FALSE;
			
			d.m_2Bytes=r->ControlWord;
			d.m_Len1=r->Len1;
			d.m_Len2=r->Len2;
			d.m_Sum1=r->Sum1;
			d.m_Sum2=r->Sum2;
			d.m_Off1=MakeAreaPage(r->Off1,d.m_Page1);
			d.m_Off2=MakeAreaPage(r->Off2,d.m_Page2);

			ret=d.DoModal(); //EXTRACT

			if(ret==IDOK){
				Comment=d.m_Comment;
				Name=d.m_Name;

				r->Type=0;
				r->Type|=(d.m_Com)? ST_COM: 0;
				r->Type|=(d.m_Exe)? ST_EXE: 0;
				r->Type|=(d.m_Sys)? ST_SYS: 0;
				r->Type|=(d.m_NE)?  ST_NE: 0;
				r->Type|=(d.m_Ole2)?ST_OLE2: 0;
				r->Type|=(d.m_Script)?  ST_SCRIPT: 0;
				r->Type|=(d.m_Hlp)?  ST_HLP: 0;
				SubType=r->Type;

				r->ControlWord=d.m_2Bytes;
				r->Len1=d.m_Len1;
				r->Len2=d.m_Len2;
				r->Sum1=d.m_Sum1;
				r->Sum2=d.m_Sum2;
			
				r->Off1=d.m_Off1 + WorkAreaPage[d.m_Page1][0];
				r->Off2=d.m_Off2 + WorkAreaPage[d.m_Page2][0];
			}	
			break;
		}

	case RT_SEPARATOR:
		{
			CDSeparator d(this);
			d.m_Comment=Comment;
			
			ret=d.DoModal();

			if(ret==IDOK){
				Comment=d.m_Comment;
			}
			break;
		}
	default:
		ret=AfxMessageBox("Sure?",MB_OKCANCEL);
		break;
	}
	
	if(ret==IDOK)	SetModifyFlag();

	return ret;
}



BYTE CRecordEdit::AddLink(const char* filename, BOOL nodlg_)
{
	BYTE b=0;
	CFile cf;
	CString PublicNames;
	if(filename[0]==0)return b;

nextfile:
	if(!cf.Open(filename, CFile::modeRead|CFile::typeBinary)) return b;
	PublicNames.Empty();

	DWORD len=cf.GetLength();
	char* buf=new char[len];
	cf.ReadHuge(buf,len);
	
	COFF_Header* Header=(COFF_Header*)buf;
	ObjRecHead* orh=(ObjRecHead*)buf;
	if(Header->Machine==0x014c)				//COFF file
	{
		COFF_Symbol* Symbol=(COFF_Symbol*)(buf+Header->P_SymbolTable);;
		for(DWORD i=0;i<Header->N_Symbols;i+=Symbol[i].Skip, i++){
			CString sn;
			if(Symbol[i].Type != 0x02) continue; //Not code
			if(Symbol[i].Section == 0) continue; //External
			if((Symbol[i].Access & 0x20) == 0)continue; //Not function
			if(Symbol[i].NameWord ==0)
				sn=(char*)(Symbol+Header->N_Symbols)+Symbol[i].NameOffset;
			else{
				sn=Symbol[i].Name;
				if(sn.GetLength()>8)
					sn=sn.Left(8);
			}

			if(sn=="_cure" || sn=="_Cure" )b|=2;
			else if(sn=="_decode" || sn=="_jmp" || sn=="_Link" )b|=1;
			else if(Type==RT_KERNEL)b|=1;

//			PublicNames+=(sn[0]=='_')?(((const char*)sn)+1):sn;
//			PublicNames+=" ";
		}
		if(b){
			if(Link32Buffer){
				if(nodlg_ || !OptDlg.m_LinkConfirm||
					IDYES==AfxMessageBox("Replace existing Link32?",MB_YESNO)){
					delete Link32Buffer;
				}else { b=0;goto clo; }
			}
			int pnl=PublicNames.GetLength() + 1;
			int total=len+pnl+2*sizeof(DWORD);
			char* lnk=new char[total];

			memcpy(lnk,&total,sizeof(DWORD));
			memcpy(lnk+sizeof(DWORD),&pnl,sizeof(DWORD));
			memcpy(lnk+2*sizeof(DWORD),(const char*)PublicNames,pnl);
			memcpy(lnk+2*sizeof(DWORD)+pnl,buf,len);
			SetLink32(lnk);
		}
	}
	else  //Old obj
	{
		CPtrArray Array;
		CStringArray LNames;
		int size=0;
		DWORD i=0;
		while(i<len)
		{
			orh=(ObjRecHead*)(buf+i);
			switch(orh->type)
			{
			case MODEND:
				{

				i=len;
				Array.Add(orh);
				size+=orh->length + sizeof(ObjRecHead);
				break;
				}
			case PUBDEF:
				{
					char* pdptr=buf+i+sizeof(ObjRecHead);
					
					char* ptr=pdptr;
					if(*(WORD*)ptr==0)ptr+=2;
					ptr+=2;
					for(; ptr < pdptr + orh->length - 1; ptr+= *ptr+4)
					{
						CString sn(ptr+1,*ptr);

						if(sn=="_cure")b|=2;
						else if(sn=="_decode" || sn=="_jmp")b|=1;
						else if(Type==RT_KERNEL)b|=1;

//						PublicNames+=sn;
//						PublicNames+=" ";
					}
				}
				Array.Add(orh);
				size+=orh->length + sizeof(ObjRecHead);
				break;
			case SEGDEF:
				{
					char* pdptr=buf+i+sizeof(ObjRecHead);
					if(LNames[pdptr[4]-1]!="CODE"){
						*pdptr|=1;
						pdptr+=orh->length-1;  //CRC correction
						*pdptr-=1;
					}



				}
			case EXTDEF:
			case LEDATA:
			case THEADR:
			case FIXUPP:
				Array.Add(orh);
				size+=orh->length + sizeof(ObjRecHead);
				break;
			case LNAMES:
				{
					char* pdptr=buf+i+sizeof(ObjRecHead);
					for(int j=0;j<orh->length-1;){
						LNames.Add(CString(pdptr+j+1,pdptr[j]));
						j+=pdptr[j]+1;
					}
				}
				break;
			default:
/*			case LINNUM:
			case COMENT:
			case GRPDEF:
*/
				break;
			}
			i+=orh->length + sizeof(ObjRecHead);
		}
		
		if(b){
			b|=8;
			if(Link16Buffer){
				if(nodlg_ || !OptDlg.m_LinkConfirm||
					IDYES==AfxMessageBox("Replace existing Link16?",MB_YESNO)){
					delete Link16Buffer;
				} else { b=0;goto clo; }
			}
			int pnl=PublicNames.GetLength() + 1;
			int total=size+pnl+2*sizeof(DWORD);
			char* lnk=new char[total];
			memcpy(lnk,&total,sizeof(DWORD));
			memcpy(lnk+sizeof(DWORD),&pnl,sizeof(DWORD));
			memcpy(lnk+2*sizeof(DWORD),(const char*)PublicNames,pnl);
			char* ptr=lnk+pnl+2*sizeof(DWORD);
			for(int i=0;i<Array.GetSize();i++){
				orh=(ObjRecHead*)Array[i];
				memcpy(ptr,orh,orh->length + sizeof(ObjRecHead));
				ptr+=orh->length + sizeof(ObjRecHead);
			}
			SetLink16(lnk);
		}
	}
clo:
	delete buf;
	cf.Close();


	char FileName[512];
	char FDrive[256];
	char FDir[256];
	char FName[256];
	char FExt[128];

	if(FileName!=filename){
		_splitpath( filename, FDrive, FDir, FName, FExt );
				if(*(WORD*)(FExt+2)==0x3631)	*(WORD*)(FExt+2)=0x3233;
		else	if(*(WORD*)(FExt+2)==0x3233)	*(WORD*)(FExt+2)=0x3631;
		else	return b;

		_makepath ( FileName, FDrive, FDir, FName, FExt );
		filename=FileName;
		goto nextfile;
	}

	return b;
}




BOOL CRecordEdit::Unlink()
{
	if(Link16Buffer==NULL && Link32Buffer==NULL)return FALSE;
	if(OptDlg.m_LinkConfirm && IDYES!=AfxMessageBox("Remove all linked code?",MB_YESNO))return FALSE;
	if(Link16Buffer){
		delete Link16Buffer;
		Link16Buffer=NULL;
	}
	if(Link32Buffer){
		delete Link32Buffer;
		Link32Buffer=NULL;
	}

    #if (defined (KL_MULTIPLATFORM_LINKS))
        VERIFY (new (&ExtraInfo) TExtraRecordInfo() == &ExtraInfo);
        ExtraInfo.SetComment(Comment);
    #endif

	return TRUE;
}


