#include "stdafx.h"
#include <process.h>
#include "../Bases/Format/Base.h"
#include "../Bases/Format/Records.h"
#include <Stuff/calcsum.h>

extern DWORD squeeze (CFile& inf, CFile& outf, DWORD length,BYTE xor);
BOOL add_16bit_links=0;

void FreeAllPtr(CPtrArray& array){
	int i;
	while(i=array.GetSize()){
		i--;
		void* ptr=array.GetAt(i);
		if(ptr!=NULL)delete (char*)ptr; 
		array.RemoveAt(i);
	}
}

#include <fileformat/coff.h>

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

	for(i=0;i<Header->N_Symbols;i++){
		char buf[9];
		buf[8]=0;
		char* Name=buf;

		if(Symbol[i].Type & 0x60) goto strip;
		if(Symbol[i].Section > Header->N_Sections) goto strip;

		if(Symbol[i].NameWord==0)
			Name=(char*)(Symbol+Header->N_Symbols)+Symbol[i].NameOffset;
		else memcpy(buf,Symbol[i].Name,8); 

		
		// PUBLIC symbols
		if(Symbol[i].Section)
		{
//			Symbol[i].Value += (DWORD)SectionArray[Symbol[i].Section-1];
			
			if(*Name=='_'){
				if(Symbol[i].Type&1)goto strip;
//				if(!strcmp(Name,"_cure"))	EntryC=Symbol[i].Value;
//				else if(!strcmp(Name,"_decode"))	Entry= Symbol[i].Value;
//				else if(!strcmp(Name,"_jmp"))	Entry= Symbol[i].Value;
//				else GlobalNamesPtr->SetName(Name,Symbol[i].Value,this);
			}
			else goto strip;
		}
/*		// EXTERNAL symbols (UNDEF section)
		else 
		{
			// External Function
			if( Symbol[i].Access & 0x20 )
			{
//				Symbol[i].Value =GlobalNamesPtr->GetName(Name);
				;
			}
			//PUBLIC uninitialized data ?
			else if(Symbol[i].Value)
			{
//				Symbol[i].Value =GlobalNamesPtr->SetName(Name,0,this,Symbol[i].Value);
				;
			}
			//External data
			else{	
//				Symbol[i].Value =GlobalNamesPtr->GetName(Name);
				;
			}
		}
*/		
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

class CNameClass
{
public:
	CNameClass(char* name, char* inifile, int level=0);
	~CNameClass();
	BOOL CheckName(char* name);
	void Print(CFile& f);

private:
	CString Name;
	CString Header;
	CPtrArray SubClasses;
	CString SubNames;
	CStringArray Names;
	int Skip;
	int Level;
};

CNameClass::CNameClass(char* name, char* inifile, int level)
{
	Level=level;
	Name=name;
	Skip=GetPrivateProfileInt(name,"Skip",0,inifile);
	char* hbuf=Header.GetBuffer(0x2000);
	int nb=GetPrivateProfileString(name,"Header",NULL,hbuf,0x2000,inifile);
	for (int i=0;i<100;i++)
	{
		char buf[0x200];
		char nam[0x50];
		sprintf(nam,"Header%d",i);
		GetPrivateProfileString(name,nam,"f%&k!",buf,0x200,inifile);
		if(!strcmp(buf,"f%&k!")) break;
		if(nb)nb+=sprintf(hbuf+nb,"\xd\xa");
		nb+=sprintf(hbuf+nb,"%s",buf);
	}
	if(!nb)strcpy(hbuf,name);



	Header.ReleaseBuffer();
	
	char subNames[0x500];
	GetPrivateProfileString(name,"SubNames","",subNames,0x500,inifile);
	int l=strlen(subNames);
	for(i=0;i<l;i++)
	{
		char* s=subNames+i;
		for(int j=i;j<l,*s!=',';j++,s++);
		*s=0;
		s=subNames+i;
		i=j;
		CNameClass* nc=new CNameClass(s,inifile,level+1);
		if(nc) SubClasses.Add(nc);
	}
}

CNameClass::~CNameClass()
{
	int i,l;
	l=SubClasses.GetSize();
	for (i=0;i<l;i++)
		delete (CNameClass*)SubClasses.GetAt(i);
}

int Width=0;
void CNameClass::Print(CFile& f)
{
	int i,l,k;
	char s[0x1000];
	char tab[0x100];
	tab[0]=0;
	for(i=0;i<Level;i++)
		strcat(tab,"  ");

	l=Names.GetSize();
	k=SubClasses.GetSize();

	if(l||k)
		f.Write(s,sprintf(s,"\xd\xa%s%s",tab,Header));
		
	if(l){
		strcat(tab,"    ");
		int j=Width+1;
		for (i=0;i<l;i++){
			int m=Names.GetAt(i).GetLength();
			if((j+m)>Width){
				j=Level*2+4;
				f.Write(s,sprintf(s,"\xd\xa%s",tab));
			}
			f.Write(s,sprintf(s,"%s",(const char*)Names.GetAt(i)));
			j+=m;
			if((i+1)<l){
				f.Write(s,sprintf(s,", "));
				j+=2;
			}
		}
		f.Write(s,sprintf(s,"\xd\xa"));
	}
	if(k)
	{
		for (i=0;i<k;i++)
			((CNameClass*)SubClasses.GetAt(i))->Print(f);
		f.Write(s,sprintf(s,"\xd\xa"));
	}
}

BOOL CNameClass::CheckName(char* name)
{
	if(strncmp(Name,name,Name.GetLength())) return FALSE;
	if(Skip) return TRUE;

	char* nextpoint=strchr(name,'.');
	if(nextpoint)
	{
		nextpoint++;
		int l=SubClasses.GetSize();
		for (int i=0;i<l;i++)
		{
			CNameClass* nc=(CNameClass*)SubClasses.GetAt(i);
			if(nc->CheckName(nextpoint)) return TRUE;
		}
	}
	else nextpoint=name;

	int l=Names.GetSize();
	for(int i=0;i<l;i++)
	{
		if(Names.GetAt(i)>=nextpoint)
		{
			if(Names.GetAt(i)==nextpoint)
				return TRUE;
			break;
		}
	}
	Names.InsertAt(i,nextpoint);
	return TRUE;
}

_TCHAR* FindPrevString(_TCHAR* pi)
{
	if(*(pi-1)=='\n') return NULL;
	pi--;
	while(*(--pi))if(*pi=='\n') break;
	return pi+1;
}

#define GET_NAME_BUF_SIZE 512

_TCHAR* MakeFullName(_TCHAR* dest,const _TCHAR* src)
{
	*dest=0;
	if(src==NULL) return dest;
	
	_TCHAR s[GET_NAME_BUF_SIZE];
	_TCHAR *p=(char*)src;
	
	
	while(p && *p=='#')p=FindPrevString(p);
	if(p==NULL) return dest;
	
	int l=GET_NAME_BUF_SIZE-strlen(p)-10;
	strcpy(s+l,p);
	
	int i;
	for(i=0;p[i]=='.' && i<16;i++) l++;
	
	while(i && NULL!=(p=FindPrevString(p))){
		_TCHAR* cp;
		if(*p=='#')continue;
again:
		cp=p;
		for(int k=i-1;k;k--){
			cp=strchr(cp,'.');
			if(cp==NULL)cp=".";
			cp++;
		}
		if(*cp!='.'){
			s[--l]='.';
			int n;
			for(n=0; n<128 && cp[n]!=0 && cp[n]!='.' && l>2 ; n++)l--;
			memcpy(s+l,cp,n);
			if(--i)
				goto again;
		}
	}
	return strcpy (dest,s+l);
}




CNameClass* Classes=0;

int ReadString(CString& s, CFile& f)
{ 
	char buf[0x200];
	char* p=buf;
	for(int i=0;i<0x200;i++ ,p++){
		if( !f.Read(p,1))break;
		if(*p==0x0D || *p==0x0A ){
			f.Read(p,1);
			break;
		}
	}
	*p=0;
	s=buf;
	return i;

}
CMemFile mfRecordsArray[NUMBER_OF_RECORD_BLOCK_TYPES][NUMBER_OF_RECORD_TYPES];
int RNumTable[NUMBER_OF_RECORD_BLOCK_TYPES][NUMBER_OF_RECORD_TYPES];
CMemFile mfNames;
int NameIdx;

AVP_BaseHeader		BaseHeader;


int LoadBase(CString& baseName)
{
	int rCount=0;
	CPtrArray 	BlockHeaderArray;
	AVP_BlockHeader*	bhp;

	HANDLE f;
	f=CreateFile(baseName,GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,0,NULL);
	if(f==INVALID_HANDLE_VALUE)
		return 0;

	printf("\nRead base %s",baseName);
//BaseHeader read
	DWORD bread;

	ReadFile(f,&BaseHeader,sizeof(BaseHeader),&bread,0);

	if(*(DWORD*)BaseHeader.Authenticity==AVP_MAGIC){
		SetFilePointer(f,0,0,FILE_BEGIN);
		ReadFile(f,BaseHeader.Authenticity,sizeof(BaseHeader)-0x40,&bread,0);
	}
	
	if(BaseHeader.Magic!=AVP_MAGIC){
abort_:	
		SetFilePointer(f,0,0,FILE_END);
	}
	else
	{
		BYTE* nbuf=NULL;
		UINT i;
		UINT j;
//BlockHeaderTable read
		SetFilePointer(f,BaseHeader.BlockHeaderTableFO,0,FILE_BEGIN);
		for(i=0;i<BaseHeader.BlockHeaderTableSize;i++){
			bhp=new AVP_BlockHeader;
			ReadFile(f,bhp,sizeof(AVP_BlockHeader),&bread,0);
			if(bread!=sizeof(AVP_BlockHeader)){
				delete bhp;
				goto abort_;
			}
			BlockHeaderArray.Add(bhp);
		}
BOOL names_done;
		names_done=0;
names_done:

		for(i=0;i<BaseHeader.BlockHeaderTableSize;i++){
			bhp=(AVP_BlockHeader*)BlockHeaderArray[i];
			if(bhp->Compression) goto abort_;
			SetFilePointer(f,bhp->BlockFO,0,FILE_BEGIN);
			printf(".");
			
			switch(bhp->BlockType){
			case BT_NAME:
				break;
			case BT_LINK16:
			case BT_RECORD:
			case BT_LINK32:
				if(names_done)
					break;
			case BT_COMMENT:
				rCount+=bhp->NumRecords;
			case BT_EDIT:
			default:	continue;
			}
			
			BYTE* buf=new BYTE[bhp->CompressedSize];

			if(buf==NULL)goto abort_;

			ReadFile(f,buf,bhp->CompressedSize,&bread,0);

			if(bread!=bhp->CompressedSize
			|| bhp->CRC!=CalcSum(buf,bhp->CompressedSize)
			)
			{
				delete buf;
				goto abort_;
			}
			DWORD npos;
			char NameBuffer[0x200];
			char* NamePtr;\
			DWORD k;

			switch(bhp->BlockType){
			case BT_NAME:
				if(names_done){
					mfNames.Write(buf,bhp->CompressedSize);
				}
				else{
					if(nbuf)delete nbuf;
					nbuf=new BYTE[bhp->CompressedSize+1];
					memcpy(nbuf+1,buf,bhp->CompressedSize);
					nbuf[0]='\n';
					for(k=1;k<=bhp->CompressedSize;k++)
						if(nbuf[k]=='\n')nbuf[k]=0;
					delete buf;
					buf=0;
					names_done=1;
					goto names_done;
				}
				break;

			case BT_RECORD:	
				rCount+=bhp->NumRecords;
				npos=mfNames.GetPosition();
				
				switch(bhp->RecordType){
				case RT_MEMORY:	{
					R_Memory* p=(R_Memory*)buf;
					strcpy(NameBuffer,"Classes.Class Viruses.");
					NamePtr=NameBuffer+strlen(NameBuffer);
					for(j=0;j<bhp->NumRecords;j++,p++)
					{
						MakeFullName(NamePtr,(char*)nbuf+1+p->NameIdx);
						Classes->CheckName(NameBuffer);
						p->NameIdx +=npos;
						if(p->LinkIdx!=(WORD)-1)	p->LinkIdx +=RNumTable[BT_LINK32][bhp->RecordType];
					}	break;
								}
				case RT_SECTOR:	{
					R_Sector* p=(R_Sector*)buf;
					strcpy(NameBuffer,"Classes.Class Viruses.");
					NamePtr=NameBuffer+strlen(NameBuffer);
					for(j=0;j<bhp->NumRecords;j++,p++)
					{
						MakeFullName(NamePtr,(char*)nbuf+1+p->NameIdx);
						Classes->CheckName(NameBuffer);
						p->NameIdx +=npos;
						if(p->LinkIdx!=(WORD)-1)	p->LinkIdx +=RNumTable[BT_LINK32][bhp->RecordType];
					}	break;
								}
				case RT_FILE:	{
					R_File* p=(R_File*)buf;
					strcpy(NameBuffer,"Classes.Class Viruses.");
					NamePtr=NameBuffer+strlen(NameBuffer);
					for(j=0;j<bhp->NumRecords;j++,p++)
					{
						MakeFullName(NamePtr,(char*)nbuf+1+p->NameIdx);
						Classes->CheckName(NameBuffer);
						p->NameIdx +=npos;
						if(p->LinkIdx!=(WORD)-1)	p->LinkIdx +=RNumTable[BT_LINK32][bhp->RecordType];
					}	break;
								}
				case RT_CA:		{
					R_CA* p=(R_CA*)buf;
					strcpy(NameBuffer,"Classes.Class Heuristics.");
					NamePtr=NameBuffer+strlen(NameBuffer);
					for(j=0;j<bhp->NumRecords;j++,p++)
					{
						if(p->Len2)
							Classes->CheckName("Classes.Class False Alarms.?not expanded by scarabey?");
						else{
							MakeFullName(NamePtr,(char*)nbuf+1+p->NameIdx);
							Classes->CheckName(NameBuffer);
						}
						p->NameIdx +=npos;
						if(p->LinkIdx!=(WORD)-1)	p->LinkIdx +=RNumTable[BT_LINK32][bhp->RecordType];
					}	break;
								}
				case RT_UNPACK:	{
					R_Unpack* p=(R_Unpack*)buf;
					strcpy(NameBuffer,"Classes.Class Packers.");
					NamePtr=NameBuffer+strlen(NameBuffer);
					for(j=0;j<bhp->NumRecords;j++,p++)
					{
						MakeFullName(NamePtr,(char*)nbuf+1+p->NameIdx);
						Classes->CheckName(NameBuffer);
						p->NameIdx +=npos;
						if(p->LinkIdx!=(WORD)-1)	p->LinkIdx +=RNumTable[BT_LINK32][bhp->RecordType];
					}	break;
								}
				case RT_EXTRACT:{
					R_Extract* p=(R_Extract*)buf;
					strcpy(NameBuffer,"Classes.Class Archives.");
					NamePtr=NameBuffer+strlen(NameBuffer);
					for(j=0;j<bhp->NumRecords;j++,p++)
					{
						MakeFullName(NamePtr,(char*)nbuf+1+p->NameIdx);
						Classes->CheckName(NameBuffer);
						p->NameIdx +=npos;
						if(p->LinkIdx!=(WORD)-1)	p->LinkIdx +=RNumTable[BT_LINK32][bhp->RecordType];
					}	break;
								}
				case RT_KERNEL:{
					R_Kernel* p=(R_Kernel*)buf;
					for(j=0;j<bhp->NumRecords;j++,p++){
						if(p->LinkIdx!=(WORD)-1)	p->LinkIdx +=RNumTable[BT_LINK32][bhp->RecordType];
					}	break;
								}
				case RT_JUMP:	{
					R_Jump* p=(R_Jump*)buf;
					for(j=0;j<bhp->NumRecords;j++,p++){
						if(p->LinkIdx!=(WORD)-1)	p->LinkIdx +=RNumTable[BT_LINK32][bhp->RecordType];
					}	break;
								}
				default:	break;
				}
				mfRecordsArray[bhp->BlockType][bhp->RecordType].Write(buf,bhp->CompressedSize);
				break;

			case BT_LINK16:
				if(!add_16bit_links)
					break;
				
			case BT_LINK32:
				{
					BYTE* b=buf;
					for(UINT k=0;k<bhp->NumRecords;k++){
						DWORD* dptr=(DWORD*)b;
						DWORD q=0;
						DWORD l=dptr[0];
						b+=l;
						l-=dptr[1];

						if(BT_LINK32==bhp->BlockType)
							StripComments(((BYTE*)dptr)+dptr[1]+8);
						mfRecordsArray[bhp->BlockType][bhp->RecordType].Write(&l,4);
						mfRecordsArray[bhp->BlockType][bhp->RecordType].Write(&q,4);
						mfRecordsArray[bhp->BlockType][bhp->RecordType].Write(((BYTE*)dptr)+dptr[1]+8,l-8);
					}
				}
		
				break;
			default:
				break;
			}
			if(buf)delete buf;
		}
		if(nbuf)delete nbuf;

		for(i=0;i<BaseHeader.BlockHeaderTableSize;i++){
			bhp=(AVP_BlockHeader*)BlockHeaderArray[i];
			switch(bhp->BlockType){
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

	FreeAllPtr(BlockHeaderArray);
	CloseHandle(f);
	return rCount;
//	return 1;
}
// 7.41.  Напишите функцию match(строка,шаблон); для проверки соответствия строки  упро-
// щенному регулярному выражению в стиле Шелл. Метасимволы шаблона:
//
//         * - любое число любых символов (0 и более);
//         ? - один любой символ.
//                       Усложнение:
//         [буквы]  - любая из перечисленных букв.
//         [!буквы] - любая из букв, кроме перечисленных.
//         [h-z]    - любая из букв от h до z включительно.
//
// Указание: для проверки "остатка" строки используйте рекурсивный вызов этой  же  функ-
// ции.
//      Используя эту функцию, напишите программу,  которая  выделяет  из  файла  СЛОВА,
// удовлетворяющие  заданному шаблону (например, "[Ии]*о*т"). Имеется в виду, что каждую
// строку надо сначала разбить на слова, а потом проверить каждое слово.

//    #include <stdio.h>
//    #include <string.h>
//    #include <locale.h>

    // Проверить, что smy лежит между smax и smin
int syinsy(unsigned smin, unsigned smy, unsigned smax)
     {
      char left   [2];
      char right  [2];
      char middle [2];

      left  [0]   = smin;  left  [1]   = '\0';
      right [0]   = smax;  right [1]   = '\0';
      middle[0]   = smy;   middle[1]   = '\0';

//      return (strcoll(left, middle) <= 0 && strcoll(middle, right) <= 0);
        return (strcmp(left, middle) <= 0 && strcmp(middle, right) <= 0);
     }

    #define  U(c) ((c) & 0377)  // подавление расширения знака
    #define  QUOT    '\\'       // экранирующий символ
    #ifndef  MATCH_ERR
    # define MATCH_ERR printf("Нет ]\n")
//    # define MATCH_ERR
    #endif

    // * s - сопоставляемая строка
    // * p - шаблон. Символ \ отменяет спецзначение метасимвола.

int match(register char *s, register char *p)
     {
      register int    scc; // текущий символ строки
      int     c, cc, lc;   // lc - предыдущий символ в [...] списке
      int     ok, notflag;

      for(;;)
       {
        scc = U(*s++);          // очередной символ строки
        switch (c = U (*p++))
         { // очередной символ шаблона
          case QUOT:          // a*\*b
            c = U (*p++);
            if( c == 0 ) return(0); // ошибка: pattern
            else goto def;
          case '[':   // любой символ из списка
            ok = notflag = 0;
            lc = 077777;      // достаточно большое число
            if(*p == '!')
             { notflag=1; p++; }

            while ((cc=U(*p++))!=0)
             {
              if(cc == ']')
               {    // конец перечисления
                if (ok)
                      break;      // сопоставилось
                return (0);     // не сопоставилось
               }
              if(cc == '-')
               {    // интервал символов
                if (notflag)
                 {
                  // не из диапазона - OK
                  if (!syinsy (lc, scc, U (*p++)))
                     ok++;
                  // из диапазона - неудача
                  else return (0);
                 }
                else
                 {
                  // символ из диапазона - OK
                  if (syinsy (lc, scc, U (*p++)))
                     ok++;
                 }
               }
              else
               {
                if (cc == QUOT)
                 {      // [\[\]]
                  cc = U(*p++);
                  if(!cc) return(0);// ошибка
                 }
                if (notflag)
                 {
                  if (scc && scc != (lc = cc))
                    ok++;          // не входит в список
                  else return (0);
                 }
                else
                 {
                  if (scc == (lc = cc)) // входит в список
                    ok++;
                 }
               }
             }
            if (cc == 0)
             {    // конец строки
              MATCH_ERR;
              return (0);        // ошибка
             }
            continue;
          case '*':   // любое число любых символов
            if(!*p)  return (1);
            for(s--; *s; s++)
                if(match (s, p))
                    return (1);
            return (0);
          case 0:
            return (scc == 0);
          default: 
def:
            if(c != scc) return (0);
            continue;
          case '?':   // один любой символ
            if (scc == 0) return (0);
            continue;
         }
       }
     }


// Обратите внимание на то, что в UNIX расширением  шаблонов  имен  файлов,  вроде  *.c,
// занимается  не  операционная система (как в MS DOS), а программа-интерпретатор команд
// пользователя (shell: /bin/sh, /bin/csh, /bin/ksh).   Это  позволяет  обрабатывать  (в
// принципе) разные стили шаблонов имен.

int main( int argc, char *argv[ ], char *envp[ ] )
{
	int NameCnt=0;
	int a;
	int ret=6;
	if(argc<2){
		printf("\nusage:   scarabey setname [/16] [/u] [/l]");
		printf("\n      or scarabey basename [/16] [/u] [/l] [/d] (wildcards allowed)\n");
		printf("\n      /16 - insert 16-bit links");
		printf("\n      /u  - uppercase filename, otherwise lower"); 
		printf("\n      /d  - \"daily\" name of the result file,"); 
		printf("\n            otherwise name will be formed from current date.\n");
		return 5;
	}
	for(a=2; a<argc; a++){
		if(!stricmp(argv[a],"/16")){
			add_16bit_links=1;
		}
	}

	CFile cf;
	CString name=argv[1];
	memset(RNumTable,0,NUMBER_OF_RECORD_BLOCK_TYPES*NUMBER_OF_RECORD_TYPES*sizeof(int));
	NameIdx=0;

	char FDrive[512];
	char FDir[512];
	char FName[512];
	char FExt[512];
	char inifile[512];
	_splitpath( argv[0], FDrive, FDir, FName, FExt );
	_makepath( inifile, FDrive, FDir, FName, ".INI");
	
	Width=GetPrivateProfileInt("Options","Width",0,inifile);
	Classes=new CNameClass("Classes",inifile);
	
	GetFullPathName(name,0x200,name.GetBuffer(0x200),NULL);
	name.ReleaseBuffer();
	_splitpath( name, FDrive, FDir, FName, FExt );

    {
        char newCurDir[512];
        _makepath ( newCurDir, FDrive, FDir, NULL, NULL );
        if ( ! SetCurrentDirectory ( newCurDir ) )
        {
            printf ( "\n error changing directory to %s", newCurDir );
            ret = 100;
            goto err;
        }
    }
	
	if(!cf.Open(name,CFile::modeRead)){
		WIN32_FIND_DATA fd;
		HANDLE h=FindFirstFile(name,&fd);
		if(h==INVALID_HANDLE_VALUE)
		{
			printf("\n no such files %s",name);
			ret=3;
			goto err;
		}
		do{
			_makepath( inifile, FDrive, FDir, "", "");
			strcat(inifile,fd.cFileName);
			if(!LoadBase(CString(inifile))){
				FindClose(h);
				printf("\n error database");
				ret=2;
				goto err;
			}
		}while(FindNextFile(h,&fd));
		FindClose(h);

		strcpy(FName,(CTime::GetCurrentTime()).Format("up%y%m%d"));
	}
	else{
		cf.Read(&BaseHeader,sizeof(BaseHeader));
		if(BaseHeader.Magic==AVP_MAGIC){
			cf.Close();
			if(!LoadBase(name)){
				printf("\n error database");
				ret=2;
				goto err;
			}
			strcpy(FName,(CTime::GetCurrentTime()).Format("up%y%m%d"));
		}
		else{
			cf.Seek(0,CFile::begin);
			CString s;
			while(ReadString(s,cf)){
				if(s[0]==';')continue;
				if(s[0]==' ')continue;
				if(s[0]== 0 )continue;
				if(!LoadBase(s)){
					cf.Close();
					printf("\n error database name in setfile");
					ret=4;
					goto err;
				}
			}
			cf.Close();
		}
	}
	
	int reg;
	reg=0;
	for(a=2; a<argc; a++){
		if(!stricmp(argv[a],"/d"))
			strcpy(FName,"daily");
		if(!stricmp(argv[a],"/u"))
			reg='u';
	}
	
	_makepath ( name.GetBuffer(512), FDrive, FDir, FName, ".avc" );
	name.ReleaseBuffer();

	if(reg=='u')
		name.MakeUpper();
	else
		name.MakeLower();


	if(cf.Open(name,CFile::modeWrite|CFile::modeCreate)){
//		AVP_BaseHeader BaseHeader;
//		memset(&BaseHeader,0,sizeof(BaseHeader));

		SYSTEMTIME st;
		GetLocalTime(&st);

		memset(BaseHeader.Text,0,0x40);
		sprintf((char *)BaseHeader.Text,"AVP Antiviral Database. (c)Kaspersky Lab 1997-%d.",st.wYear);
		*(DWORD*)(BaseHeader.Text+0x3E)='\xd\xa';

		memset(BaseHeader.Authenticity,0,0x40);
		CTime t=CTime::GetCurrentTime();
		strcpy((char *)(BaseHeader.Authenticity),t.Format(_T("Kaspersky Lab. %d %b %Y  %H:%M:%S")));
		*(DWORD*)(BaseHeader.Authenticity+0x3C)='\xd\xa\xd\xa';

//		BaseHeader.Magic=AVP_MAGIC;
		BaseHeader.VerH=   0x03;
		BaseHeader.VerL=   0x00;
		BaseHeader.Flags=0x0001;
		BaseHeader.NumberOfRecordBlockTypes=NUMBER_OF_RECORD_BLOCK_TYPES;
		BaseHeader.NumberOfRecordTypes=NUMBER_OF_RECORD_TYPES;
		BaseHeader.CreationTime=st;
		BaseHeader.ModificationTime=st;


		CPtrArray BlockHeaderArray;
		AVP_BlockHeader* bhp;
		int i,j;
		int nbh;

		BaseHeader.Flags &=~1;
	
		printf("\nPacking");

//BaseHeader write
		cf.Write(&BaseHeader,sizeof(BaseHeader));

//BlockTableHeader write
		BaseHeader.BlockHeaderTableFO = cf.GetPosition();
		nbh=NameIdx?1:0;
		for (i=0; i<NUMBER_OF_RECORD_BLOCK_TYPES; i++)
			for (int j=0; j<NUMBER_OF_RECORD_TYPES; j++)
				if(RNumTable[i][j])nbh++;


		bhp=new AVP_BlockHeader;
		memset(bhp,0,sizeof(AVP_BlockHeader));
		for(i=0;i<nbh;i++)	cf.Write(bhp,sizeof(AVP_BlockHeader));
		delete bhp;

//Records & Links write
		for (i=0; i<NUMBER_OF_RECORD_BLOCK_TYPES; i++)
		for (j=0; j<NUMBER_OF_RECORD_TYPES; j++)
		{
			if(0==RNumTable[i][j])continue;

			bhp=new AVP_BlockHeader;
			memset(bhp,0,sizeof(AVP_BlockHeader));
			BlockHeaderArray.Add(bhp);

			bhp->BlockType=i;   //HERE define TYPES
			bhp->RecordType=j;  
			
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
				default:		break;
				}
			}
			
			bhp->UncompressedSize=mfRecordsArray[i][j].GetLength();
			if(bhp->UncompressedSize){
				bhp->BlockFO=cf.GetPosition();
				bhp->Compression=1;
				bhp->NumRecords=RNumTable[i][j];
				
				CMemFile cmf;
				mfRecordsArray[i][j].SeekToBegin();
				bhp->CompressedSize=squeeze(mfRecordsArray[i][j],cmf,bhp->UncompressedSize,0);
				BYTE* mb=cmf.Detach();
				for(UINT ib=0;ib<bhp->CompressedSize;ib++)	mb[ib]^=(BYTE)ib;
				
				bhp->CRC=CalcSum(mb,bhp->CompressedSize);
				cf.Write(mb,bhp->CompressedSize);
				delete mb;
			}
			printf(".");
			
		}
		
//Names write
		if(NameIdx)
		{
			bhp=new AVP_BlockHeader;
			memset(bhp,0,sizeof(AVP_BlockHeader));
			BlockHeaderArray.Add(bhp);

			bhp->BlockFO=cf.GetPosition();
			bhp->BlockType=BT_NAME;
			bhp->NumRecords=NameIdx;

			CMemFile* mf=&mfNames;

			bhp->UncompressedSize=mf->GetLength();
			bhp->Compression=1;
			
			CMemFile cmf(0x1000);
			mf->SeekToBegin();
			bhp->CompressedSize=squeeze(*mf,cmf,bhp->UncompressedSize,0);
			BYTE* mb=cmf.Detach();
			for(UINT ib=0;ib<bhp->CompressedSize;ib++)	mb[ib]^=(BYTE)ib;

			bhp->CRC=CalcSum(mb,bhp->CompressedSize);
			cf.Write(mb,bhp->CompressedSize);
			delete mb;
			
			char txtname[MAX_PATH];
			mb=mf->Detach();
			_splitpath( name, FDrive, FDir, FName, FExt );
			_makepath ( txtname, FDrive, FDir, FName, ".txt" );
			CFile cftxt;
			cftxt.Open(txtname,CFile::modeWrite|CFile::modeCreate);
			Classes->Print(cftxt);

/*			for(DWORD i=0;i<bhp->UncompressedSize;i++){
				BOOL realname=0;
				DWORD j=i;
				if(mb[i]!='#'){
					NameCnt++;
					realname=1;
				}
				for(;i<bhp->UncompressedSize;i++){
					if(mb[i]==0xA){
						if(realname){
							cftxt.Write(mb+j,i-j);
							cftxt.Write("\xD\xA",2);
						}
						break;
					}
				}
			}
*/

			cftxt.Close();
			delete mb;

			printf(".");
		
		}

		BaseHeader.FileSize = cf.GetPosition(); 
		BaseHeader.BlockHeaderTableSize = BlockHeaderArray.GetSize(); 
		BaseHeader.AuthenticityCRC=(CalcSum(BaseHeader.Authenticity,0x40));
		BaseHeader.HeaderCRC=(CalcSum((BYTE*)&BaseHeader.Magic,sizeof(BaseHeader)-0x84));
		cf.SeekToBegin();
		cf.Write(&BaseHeader,sizeof(BaseHeader));

//BlockTableHeader Again write
		cf.Seek(BaseHeader.BlockHeaderTableFO,CFile::begin);
		for(i=0;i<(int)BaseHeader.BlockHeaderTableSize;i++)	{
			cf.Write(BlockHeaderArray[i],sizeof(AVP_BlockHeader));
		}
		cf.Close();
		FreeAllPtr(BlockHeaderArray);
		
		printf("\nNames: %d",NameCnt);

		for (i=0; i<=NUMBER_OF_RECORD_BLOCK_TYPES; i++)
		{
			switch(i)
			{
			case 1: printf("\nRec's    ");break;
			case 2: printf("\n16-bit   ");break;
			case 3: printf("\n32-bit   ");break;
			default:printf("\n         ");break;
			}
			for (j=0; j<=NUMBER_OF_RECORD_TYPES; j++)
			{
				if(i==0){
					switch(j)
					{
					case 1: printf("  Kernel");break;
					case 2: printf("    Jump");break;
					case 3: printf("  Memory");break;
					case 4: printf("  Sector");break;
					case 5: printf("    File");break;
					case 6: printf("      CA");break;
					case 7: printf("  Unpack");break;
					case 8: printf(" Extract");break;
					default:break;
					}
				}
				else if(j)
					printf("% 8d",RNumTable[i-1][j-1]);
			}
		}
		if(Classes){
			delete Classes;
			Classes=0;
		}

		printf("\nDone.\n");

		GetModuleFileName(NULL,FName,0x200);
		strcpy(1+strrchr(FName,'\\'),"tsigner.exe ");
		ret=_spawnl(_P_WAIT,FName,"-s",name,NULL);
	}
err:
	if(Classes) delete Classes;
	if(ret) printf("\nError %d occured.\n",ret);
	return ret;
}
  
