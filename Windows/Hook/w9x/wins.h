extern "C"
{
void InitWins(void);
void DoneWins(void);
unsigned long MsgBox(char *Title, char *Message);

long wherex(void);
long wherey(void);
void SetColour(unsigned char Clr);
void textcolor(unsigned char Clr);
void textbackground(unsigned char Clr);
void cprintf(char* str,...);
unsigned char kbhit(void);
unsigned char getch(void);
void gettext(long left, long top, long right, long bottom, char* destin);
void puttext(long left, long top, long right, long bottom, char* source);
void outchar(unsigned short Ch);
void outstr(char* str);
void Coutstr(char* str);
void shadow(long posx,long posy,long len);
void *GlobalAlloc(unsigned long Flags,unsigned long Size);
void GlobalFree(void* Ptr);
void delay(unsigned long Pause);
void _setcursortype(unsigned char ctype);
void gotoxy(long Xpos,long Ypos);
}

//=====================================================================================

class TPoint {
public:
    long x,y;
    TPoint( long ax, long ay);
};

class TRect {
public:
   TPoint a, b;
   TRect( long ax, long ay, long bx, long by );
   TRect( TPoint p1, TPoint p2 );
};

class TView {
public:
    TPoint Pos;
    TPoint Size;
    TView( const TRect& bounds );
//    ~TView();
    virtual void Draw(void) {};
};


class TWin : public TView {
public:
   char *UnderWin;
   const char *Title;
	unsigned char Colour;
   TWin(TRect aBounds,  const char* aTitle, unsigned char aColour);
   virtual ~TWin();
   virtual void Draw(void);
};

#define BS_Normal 0
#define BS_Active 1
#define BS_Pressed 2

class TButton : public TView {
public:
	TWin *Owner;
	char *Title;
	long State;
	TButton(TRect aBounds, char* aTitle);
	virtual void Draw(void);	
};

struct TBList {
   TBList *Next;
   TButton *Button;
   long RetVal;
};

class TDialog : public TWin {
public:
	TBList *BList;
	TBList *Current;
	char *Msg;
	TDialog(TRect aBounds,  const char* aTitle, char* aMsg);
	~TDialog();
	void InsButton(TButton* Button, long RetVal);
	virtual void Draw(void);
	void DrawButtons(void);
	long Exec(void);
};

//=====================================================================================


#define _NOCURSOR      0
#define _SOLIDCURSOR   1
#define _NORMALCURSOR  2

enum COLORS {
    BLACK,
    BLUE,
    GREEN,
    CYAN,
    RED,
    MAGENTA,
    BROWN,
    LIGHTGRAY,
    DARKGRAY,
    LIGHTBLUE,
    LIGHTGREEN,
    LIGHTCYAN,
    LIGHTRED,
    LIGHTMAGENTA,
    YELLOW,
    WHITE
};

