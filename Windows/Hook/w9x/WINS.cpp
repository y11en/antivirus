#include <vtoolsc.h>
#include "wins.h"
#include "..\hook\avpgcom.h"
char TmpStr[512];
const char WinBorder[]={'É','Í','»','È','º','¼'};

//BYTE StCurX,StCurY;

extern "C" {
char* VideoMem;
}
unsigned char ScrAttr;
long CursrX;
long CursrY;

__inline long wherex(void)
{
    return CursrX;
}

__inline long wherey(void)
{
    return CursrY;
}

void _setcursortype(unsigned char ctype)
{
}

__inline void gotoxy(long Xpos,long Ypos)
{
    CursrX=Xpos;
    CursrY=Ypos;
}

__inline void textcolor(unsigned char Clr)
{
    ScrAttr&=0xf0;
    ScrAttr|=Clr;
}

__inline void textbackground(unsigned char Clr)
{
    ScrAttr&=0xf;
    ScrAttr|=Clr<<4;
}

inline void SetColour(unsigned char Clr)
{
   ScrAttr=Clr;
}

void *GetVidMem(void) 
{
void* retval;
	__asm {
	mov eax,CursrY
	mov ebx,eax
	shl eax,2
	add eax,ebx
	shl eax,4
	add eax,CursrX
	add eax,eax
	add eax,VideoMem
	mov retval,eax
	};
	return retval;
}

void outchar(unsigned short Ch)
{
	*((unsigned short*)GetVidMem())=(((unsigned short)(ScrAttr))<<8) | (Ch & 0xff);
}

void outstr(LPSTR str)
{
register char *SrcMem;
register char *DestMem;
    DestMem = (char*)GetVidMem();
    SrcMem=str;
    while (*SrcMem) {
		*(DestMem++)=*(SrcMem++);
		*(DestMem++)=ScrAttr;
    }
}

void Coutstr(LPSTR str)
{
register char *SrcMem;
register char *DestMem;
register unsigned char Attr;
int cnt=0;
    DestMem = (char*)GetVidMem();
    Attr=ScrAttr;
    SrcMem=str;
    while (*SrcMem) {
		if(cnt) {
			cnt--;
			if(!cnt) Attr=ScrAttr;
		} else { 
			if((*SrcMem==' ')&&(*(SrcMem-1)!=' ')) Attr=ScrAttr;
			if(*SrcMem=='`') {Attr=(unsigned char)(Attr & 0xf0 | 0x84); *SrcMem=' ';}
		}
		if(*SrcMem=='^') {Attr=(unsigned char)(Attr & 0xf0 | 0x84); cnt=3;SrcMem++;continue;}
		*(DestMem++)=*(SrcMem++);
		*(DestMem++)=Attr;
    }
}

void shadow(long posx,long posy,long len)
{
register char *DestMem;
	DestMem = (char*)VideoMem + (posx+posy*80)*2+1;
	while(len) {
		*(DestMem++)=LIGHTGRAY;
		DestMem++;
		len--;
	}
}

void cprintf(LPSTR str,...)
{
char Buf[512];
va_list marker;
PVOID DestMem;
	va_start(marker, str);
	vsprintf(Buf,str,marker);
	va_end(marker);
	DestMem = (UCHAR*)VideoMem + (CursrX+CursrY*80)*2;
	__asm {
		cld
			mov edi,DestMem
			lea esi,Buf
			mov al, ScrAttr
lp:     cmp byte ptr [esi],0
        jz  endstr
        movsb
        mov byte ptr [edi],al
        inc edi
        jmp lp
endstr: nop
	}
}

unsigned char  KbdCode;

unsigned char kbhit(void)
{
CLIENT_STRUCT	SavedRegs;
unsigned char Scan,State;
VMHANDLE CurVM;
	CurVM=Get_Sys_VM_Handle();
	Save_Client_State(&SavedRegs);
	Begin_Nest_Exec();
	VKD_Flush_Msg_Key_Queue(CurVM);
	Resume_Exec();
	if(!VKD_Get_Msg_Key(CurVM,&Scan,&State))
		KbdCode=Scan=0;
	End_Nest_Exec();
	Restore_Client_State(&SavedRegs);
	if(Scan){
		KbdCode=Scan;
	}
	return Scan;
}

__inline unsigned char getch(void)
{
    return KbdCode;
}

void delay(unsigned long nMicroseconds)
{
	CONFIGMG_Yield(nMicroseconds,CM_YIELD_NO_RESUME_EXEC);
}

void gettext(int left, int top, int right, int bottom, PCHAR destin)
{
    for(int i=top;i<bottom;i++) {
//        memcpy(destin,VideoMem+(left+i*80)*2,(right-left)*2);
        destin+=(right-left)*2;
    }
}

void puttext(int left, int top, int right, int bottom, PCHAR source)
{
    for(int i=top;i<bottom;i++) {
//        memcpy(VideoMem+(left+i*80)*2,source,(right-left)*2);
        source+=(right-left)*2;
    }
}

inline void * __cdecl operator new(unsigned int nSize)
{ 
    return HeapAllocate(nSize,0);
}


inline void __cdecl operator delete(void* p)
{ 
    if (p) HeapFree(p,0);
}

inline void *GlobalAlloc(ULONG Flags, ULONG Size)
{
    return HeapAllocate(Size,0);
}

inline void GlobalFree(void* Ptr)
{
    HeapFree(Ptr,0);
}

unsigned long MsgBox(char *Title, char *Message)
{
TDialog *Dlg;
long SS=25;//GetScreenSize();
   Dlg=new TDialog(TRect(9,SS/2-5,61,10),Title,Message);
   Dlg->InsButton(new TButton(TRect(41,7,13,2),"Kill"),Verdict_Kill);
   Dlg->InsButton(new TButton(TRect(23,7,13,2),"No"),Verdict_Discard);
   Dlg->InsButton(new TButton(TRect(5,7,13,2),"Yes"),Verdict_Pass);
   SS=Dlg->Exec();
   delete Dlg;
   return SS;
}


//===========================================================================================================
char *FillStr(char* Str, char val, size_t len)
{
   memset(Str,val,len);
   Str[len]=0;
   return Str;
}

void InitWins(void)
{
//   StCurX=0;
//   StCurY=0;
//  disp_hidecursor();
}

void DoneWins(void)
{
//   gotoxy(StCurX,StCurY);
//   disp_showcursor();
//   textcolor(WHITE);textbackground(BLACK);
}

TPoint::TPoint( long ax, long ay)
{
   x=ax; y=ay;
}

TRect::TRect( long ax, long ay, long bx, long by) : a(ax,ay), b (bx,by)
{
}

inline TRect::TRect( TPoint p1, TPoint p2 ) : a(p1),b(p2)
{
}

TView::TView( const TRect& bounds ) :Pos(bounds.a),Size(bounds.b)
{
}


TWin::TWin(TRect aBounds, const char * aTitle, unsigned char aColour) : TView(aBounds), Title(aTitle), Colour(aColour)
{
//	if(UnderWin=(char*)GlobalAlloc(0,(aBounds.b.x+2)*(aBounds.b.y+1)*2))
//		gettext(aBounds.a.x,aBounds.a.y,aBounds.a.x+aBounds.b.x+2,aBounds.a.y+aBounds.b.y+1,UnderWin);
	Draw();
}

TWin::~TWin()
{
//   if(UnderWin) {
//      puttext(Pos.x,Pos.y,Pos.x+Size.x+2,Pos.y+Size.y+1,UnderWin);
//      GlobalFree(UnderWin);
//   }
}

void TWin::Draw(void)
{
long i;
char *csc="%c%s%c";
	SetColour(Colour);
   gotoxy(Pos.x,Pos.y);
   FillStr(TmpStr,WinBorder[1],Size.x-2);
   memcpy(TmpStr+(Size.x-strlen(Title))/2,Title,strlen(Title));
   cprintf(csc,WinBorder[0],TmpStr,WinBorder[2]);
   for(i=1;i<Size.y-1;i++) {
      gotoxy(Pos.x,Pos.y+i);
      cprintf(csc,WinBorder[4],FillStr(TmpStr,' ',Size.x-2),WinBorder[4]);
		shadow(Pos.x+Size.x,Pos.y+i,2);
      }
	shadow(Pos.x+Size.x,Pos.y+i,2);
   gotoxy(Pos.x,Pos.y+Size.y-1);
   cprintf(csc,WinBorder[3],FillStr(TmpStr,WinBorder[1],Size.x-2),WinBorder[5]);
	shadow(Pos.x+2,Pos.y+Size.y,Size.x);
}


TButton::TButton(TRect aBounds, char* aTitle) : TView (aBounds), State(0), Title(aTitle), Owner(NULL)
{
}

void TButton::Draw(void)
{
long TmpL=strlen(Title);
   textbackground(GREEN);
	if(State) textcolor(WHITE);
	else textcolor(BLACK);
   FillStr(TmpStr,' ',Size.x);
   TmpL=Size.x>TmpL?TmpL:Size.x;
   memcpy(TmpStr+(Size.x-TmpL)/2,Title,TmpL);
	if(State!=BS_Pressed) {
	   gotoxy(Owner->Pos.x+Pos.x,Owner->Pos.y+Pos.y);
	   outstr(TmpStr);
	   gotoxy(Owner->Pos.x+Pos.x+Size.x,Owner->Pos.y+Pos.y);
		SetColour(Owner->Colour);
		textcolor(BLACK);
		outchar('\xdc');
	   FillStr(TmpStr,'\xdf',Size.x);
	} else {
		gotoxy(Owner->Pos.x+Pos.x+1,Owner->Pos.y+Pos.y);
		outstr(TmpStr);
		SetColour(Owner->Colour);
	   gotoxy(Owner->Pos.x+Pos.x,Owner->Pos.y+Pos.y);
		outchar(' ');
	   FillStr(TmpStr,' ',Size.x);
	}
   gotoxy(Owner->Pos.x+Pos.x+1,Owner->Pos.y+Pos.y+1);
   outstr(TmpStr);
}

TDialog::TDialog(TRect aBounds,  const char* aTitle, char* aMsg)
					 : TWin(aBounds, aTitle, WHITE+LIGHTGRAY*16),
					 Msg(aMsg),BList(NULL)
		
{
}

TDialog::~TDialog()
{
TBList *BLTmp;
   if(!BList) return;
   do {
		BLTmp=BList;
		BList=BList->Next;
      if(BLTmp->Button) delete BLTmp->Button;
		delete BLTmp;
   } while(BList);
}

void TDialog::InsButton(TButton* Button, long RetVal)
{
TBList *BLTmp;
	BLTmp=new TBList;
	if(BLTmp) {
		BLTmp->Button=Button;
		BLTmp->Button->Owner=this;
		BLTmp->RetVal=RetVal;
		BLTmp->Next=BList;
		Current=BList=BLTmp;
	}
}

void TDialog::Draw(void)
{
long TmpL,MWLen=Size.x-2,Spec;
long Row=Pos.y+2;
char *SSStart,*SSEnd,*Tmp,*Tmp1;
	TWin::Draw();
	SSStart=Msg;
	textcolor(BLACK);
   if(Msg) do{
		if(Tmp=strchr(SSStart,'"'))
			SSEnd=Tmp;
		else
			SSEnd=Msg+strlen(Msg);
      gotoxy(Pos.x+1,Row++);
		for(Tmp1=SSStart,Spec=0;Tmp1<SSEnd;Tmp1++)
			if(*Tmp1=='^') Spec++;
		TmpL=SSEnd-SSStart-Spec;
	   TmpL=MWLen>TmpL?TmpL:MWLen;
      FillStr(TmpStr,' ',MWLen+Spec);
   	memcpy(TmpStr+(MWLen-TmpL)/2,SSStart,TmpL+Spec);
      Coutstr(TmpStr);
		SSStart=SSEnd+1;
   } while(Tmp);
	DrawButtons();
}

void TDialog::DrawButtons(void)
{
TBList *BLTmp=BList;
	while(BLTmp) {
		if(BLTmp==Current)
			BLTmp->Button->State=BS_Active;
		else
			BLTmp->Button->State=BS_Normal;
		BLTmp->Button->Draw();
		BLTmp=BLTmp->Next;
	}
}

long TDialog::Exec(void)
{
unsigned char Ch;
TBList *BLTmp;
   Draw();
	if(!BList) return 0;
	do {
		while(!kbhit());
		Ch=getch();
		switch (Ch) {
		case 0x4d:
				if(Current->Next){
					Current=Current->Next;
					DrawButtons();
				}
				break;
		case 0x4b:
				if(Current!=BList) {
					BLTmp=BList;
					while(BLTmp->Next!=Current) BLTmp=BLTmp->Next;
					Current=BLTmp;
					DrawButtons();
				}
				break;
		case 0x1c:
		case 0x39:
				Current->Button->State=BS_Pressed;
				Current->Button->Draw();
				delay(300000);
				return Current->RetVal;
/*		case 0x1b:
				return -1;
*/		}
   } while(1);
}


