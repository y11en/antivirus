/* ------------------------------------------------------------------------*/
/*                                                                         */
/*   SYSTEM.H                                                              */
/*                                                                         */
/*   Copyright (c) Borland International 1991                              */
/*   All Rights Reserved.                                                  */
/*                                                                         */
/*   defines the classes THWMouse, TMouse, TEventQueue, TDisplay,          */
/*   TScreen, and TSystemError                                             */
/*                                                                         */
/* ------------------------------------------------------------------------*/

#include "tvvo.h"

#if !defined( __EVENT_CODES )
#define __EVENT_CODES

/* Event codes */

const evMouseDown = 0x0001;
const evMouseUp   = 0x0002;
const evMouseMove = 0x0004;
const evMouseAuto = 0x0008;
const evKeyDown   = 0x0010;
const evCommand   = 0x0100;
const evBroadcast = 0x0200;

/* Event masks */

const evNothing   = 0x0000;
const evMouse     = 0x000f;
const evKeyboard  = 0x0010;
const evMessage   = 0xFF00;

/* Mouse button state masks */

const mbLeftButton  = 0x01;
const mbRightButton = 0x02;

#endif  // __EVENT_CODES


#if defined( Uses_TEvent ) && !defined( __TEvent )
#define __TEvent

struct MouseEventType
{
    uchar buttons;
    Boolean doubleClick;
    TPoint where;
};

class THWMouse
{

protected:

    THWMouse();
    THWMouse( const THWMouse& ) {};
    ~THWMouse();

    static void TV_CDECL show();
    static void TV_CDECL hide();
#ifdef __OS2__
    static void hide( const TRect& rect );
#endif

    static void setRange( ushort, ushort );
    static void getEvent( MouseEventType& );
    static void registerHandler( unsigned, void (far *)() );
    static Boolean present();

    static void suspend();
    static void resume();
    static void inhibit();

protected:

    static uchar near buttonCount;

private:

    static Boolean near handlerInstalled;
    static Boolean near noMouse;

};

inline Boolean THWMouse::present()
{
    return Boolean( buttonCount != 0 );
}

inline void THWMouse::inhibit()
{
    noMouse = True;
}

class TMouse : public THWMouse
{

public:

    TMouse();
    ~TMouse();

    static void show();
    static void hide();
#ifdef __OS2__
    static void hide( const TRect& rect ) { THWMouse::hide( rect ); }
#endif

    static void setRange( ushort, ushort );

    static void getEvent( MouseEventType& );
    static void registerHandler( unsigned, void (far *)() );
    static Boolean present();

    static void suspend() { THWMouse::suspend(); }
    static void resume() { THWMouse::resume(); show(); }

};

inline void TMouse::show()
{
    THWMouse::show();
}

inline void TMouse::hide()
{
    THWMouse::hide();
}

inline void TMouse::setRange( ushort rx, ushort ry )
{
    THWMouse::setRange( rx, ry );
}

inline void TMouse::getEvent( MouseEventType& me )
{
    THWMouse::getEvent( me );
}

inline void TMouse::registerHandler( unsigned mask, void (far *func)() )
{
    THWMouse::registerHandler( mask, func );
}

inline Boolean TMouse::present()
{
    return THWMouse::present();
}

#pragma pack(1)
struct CharScanType
{
    uchar charCode;
    uchar scanCode;
};
#pragma pack()

struct KeyDownEvent
{
    union
        {
        ushort keyCode;
        CharScanType charScan;
        };
};

struct MessageEvent
{
    ushort command;
    union
        {
        void *infoPtr;
        long infoLong;
        ushort infoWord;
        short infoInt;
        uchar infoByte;
        char infoChar;
        };
};

struct TEvent
{
    ushort what;
    union
    {
        MouseEventType mouse;
        KeyDownEvent keyDown;
        MessageEvent message;
    };
    void getMouseEvent();
    void getKeyEvent();
};

#endif  // Uses_TEvent

#if defined( Uses_TEventQueue ) && !defined( __TEventQueue )
#define __TEventQueue

class TEventQueue
{
public:
    TEventQueue();
    ~TEventQueue();

    static void getMouseEvent( TEvent& );
    static void suspend();
    static void resume();

    friend class TView;
    friend void genRefs();
    friend unsigned long getTicks(void);
    friend class TProgram;
    static ushort near doubleDelay;
    static Boolean near mouseReverse;

private:

    static TMouse near mouse;
    static void getMouseState( TEvent& );
#ifdef __DOS32__
    friend void _loadds _far mouseInt(int flag,int buttons,int x,int y);
#   pragma aux mouseInt parm [EAX] [EBX] [ECX] [EDX];
#else
    static void huge mouseInt();
#endif

    static void setLast( TEvent& );

    static MouseEventType near lastMouse;
    static MouseEventType near curMouse;

    static MouseEventType near downMouse;
    static ushort near downTicks;

    static ushort far * near Ticks;
    static TEvent near eventQueue[ eventQSize ];
    static TEvent * near eventQHead;
    static TEvent * near eventQTail;
    static Boolean near mouseIntFlag;
    static ushort near eventCount;

    static Boolean near mouseEvents;

    static ushort near repeatDelay;
    static ushort near autoTicks;
    static ushort near autoDelay;

#ifndef __MSDOS__
public:
    static void mouseThread(void* arg);
    static void keyboardThread( void * arg );
    static TEvent keyboardEvent;
    static unsigned char shiftState;
#endif

};

inline void TEvent::getMouseEvent()
{
    TEventQueue::getMouseEvent( *this );
}

#endif  // Uses_TEventQueue

#if defined( Uses_TScreen ) && !defined( __TScreen )
#define __TScreen

#ifdef PROTECT

extern ushort monoSeg;
extern ushort colrSeg;
extern ushort biosSeg;

#endif

class TDisplay
{

public:

    friend class TView;

    enum videoModes
        {
        smBW80      = 0x0002,
        smCO80      = 0x0003,
        smMono      = 0x0007,
        smFont8x8   = 0x0100
        };

    static void clearScreen( uchar, uchar );

    static void setCursorType( ushort );
    static ushort getCursorType();

    static ushort getRows();
    static ushort getCols();

    static void setCrtMode( ushort );
    static ushort getCrtMode();

protected:

    TDisplay() { updateIntlChars(); };
    TDisplay( const TDisplay& ) { updateIntlChars(); };
    ~TDisplay() {};

private:

    static void videoInt();
    static void updateIntlChars();

    static ushort far * near equipment;
    static uchar far * near crtInfo;
    static uchar far * near crtRows;

};

class TScreen : public TDisplay
{

public:

    TScreen();
    ~TScreen();

    static void setVideoMode( ushort mode );
    static void clearScreen();

    static ushort near startupMode;
    static ushort near startupCursor;
    static ushort near screenMode;
    static uchar near screenWidth;
    static uchar near screenHeight;
    static Boolean near hiResScreen;
    static Boolean near checkSnow;
    static uchar far * near screenBuffer;
    static ushort near cursorLines;

    static void setCrtData();
    static ushort fixCrtMode( ushort );

    static void suspend();
    static void resume();

};

#endif  // Uses_TScreen

#if defined( Uses_TSystemError ) && !defined( __TSystemError )
#define __TSystemError

class far TDrawBuffer;

class TSystemError
{

public:

    TSystemError();
    ~TSystemError();

    static Boolean near ctrlBreakHit;

    static void TV_CDECL suspend();
    static void TV_CDECL resume();
    static short ( far * TV_CDECL sysErrorFunc )( short, uchar );

private:

    static ushort near sysColorAttr;
    static ushort near sysMonoAttr;
    static Boolean near saveCtrlBreak;
    static Boolean near sysErrActive;

    static void TV_CDECL swapStatusLine( TDrawBuffer far & );
    static ushort selectKey();
    static short TV_CDECL sysErr( short, uchar );

    static Boolean near inIDE;

    static const char *const near errorString[24];
    static const char * near sRetryOrCancel;

    friend class Int11trap;

};

#ifdef __DOS16__

class Int11trap
{

public:

    Int11trap();
    ~Int11trap();

private:

    static void interrupt handler(...);
    static void interrupt (far * near oldHandler)(...);

};

#endif  // __DOS16__

#endif  // Uses_TSystemError

#if defined( Uses_TThreaded ) && !defined( __TThreaded)
#define __TThreaded

#ifdef __OS2__
#define INCL_NOPMAPI
#define INCL_KBD
#define INCL_MOU
#define INCL_VIO
#define INCL_DOSPROCESS
#define INCL_DOSMEMMGR
#define INCL_DOSSEMAPHORES
#define INCL_DOSFILEMGR
#define INCL_DOSERRORS
#define INCL_DOSMISC
#include <os2.h>

#pragma pack()
#pragma options -a.

// These must not cross a 64K boundary
struct TiledStruct {
  HMOU mouseHandle;
  MOUEVENTINFO mouseInfo;
  NOPTRRECT ptrArea;
  PTRLOC ptrLoc;
  VIOCURSORINFO cursorInfo;
  VIOMODEINFO modeInfo;
  KBDKEYINFO keyboardInfo;
  KBDINFO    keyboardShiftInfo;
};

class TThreads {
public:
  TThreads();
  ~TThreads();
  static void resume();
  static void suspend();
  static void deadEnd();
  static TiledStruct *tiled; // tiled will be allocated with DosAllocMem
  static volatile TID mouseThreadID;
  static volatile TID keyboardThreadID;
  static volatile SEMRECORD evCombined[2];
  static volatile HEV &hevMouse1;
  static volatile HEV &hevKeyboard1;
  static volatile HEV hevKeyboard2;
  static volatile HEV hevMouse2;
  static volatile HMTX hmtxMouse1;
  static volatile HMUX hmuxMaster;
  static volatile int shutDownFlag;
};

#endif // __OS2__

#ifdef __NT__
#define NOGDICAPMASKS     // CC_*, LC_*, PC_*, CP_*, TC_*, RC_
#define NOVIRTUALKEYCODES // VK_*
#define NOWINMESSAGES     // WM_*, EM_*, LB_*, CB_*
#define NOWINSTYLES       // WS_*, CS_*, ES_*, LBS_*, SBS_*, CBS_*
#define NOSYSMETRICS      // SM_*
#define NOMENUS           // MF_*
#define NOICONS           // IDI_*
#define NOKEYSTATES       // MK_*
#define NOSYSCOMMANDS     // SC_*
#define NORASTEROPS       // Binary and Tertiary raster ops
#define NOSHOWWINDOW      // SW_*
#define OEMRESOURCE       // OEM Resource values
#define NOATOM            // Atom Manager routines
#define NOCLIPBOARD       // Clipboard routines
#define NOCOLOR           // Screen colors
#define NODRAWTEXT        // DrawText() and DT_*
#define NONLS             // All NLS defines and routines
#define NOMB              // MB_* and MessageBox()
#define NOMEMMGR          // GMEM_*, LMEM_*, GHND, LHND, associated routines
#define NOMETAFILE        // typedef METAFILEPICT
#define NOMINMAX          // Macros min(a,b) and max(a,b)
#define NOOPENFILE        // OpenFile(), OemToAnsi, AnsiToOem, and OF_*
#define NOSCROLL          // SB_* and scrolling routines
#define NOSERVICE         // All Service Controller routines, SERVICE_ equates, etc.
#define NOSOUND           // Sound driver routines
#define NOTEXTMETRIC      // typedef TEXTMETRIC and associated routines
#define NOWH              // SetWindowsHook and WH_*
#define NOWINOFFSETS      // GWL_*, GCL_*, associated routines
#define NOCOMM            // COMM driver routines
#define NOKANJI           // Kanji support stuff.
#define NOHELP            // Help engine interface.
#define NOPROFILER        // Profiler interface.
#define NODEFERWINDOWPOS  // DeferWindowPos routines
#include <windows.h>

#pragma pack()
#pragma options -a.

class TThreads {
public:
  static void resume();
  static void suspend();
  static HANDLE chandle[2];
#define cnInput  0
#define cnOutput 1
  static DWORD consoleMode;
  static CONSOLE_CURSOR_INFO crInfo;
  static CONSOLE_SCREEN_BUFFER_INFO sbInfo;
#define IR_SIZE 16              // I decided to read up to 16 events at a time
  static INPUT_RECORD ir[IR_SIZE];
  static DWORD evptr;           // next event to process in 'ir'
  static DWORD evqty;           // number of events in 'ir'
  static INPUT_RECORD *get_next_event(void);    // returns number of events
  static inline int ispending(void) { return evptr < evqty; }
  static inline void accept_event(void) { evptr++; }
};

#endif // __NT__
#endif // Uses_TThreaded

#include "tvvo2.h"
