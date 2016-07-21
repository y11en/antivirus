/*------------------------------------------------------------*/
/* filename -       tevent.cpp                                */
/*                                                            */
/* function(s)                                                */
/*                  TEvent member functions                   */
/*------------------------------------------------------------*/

/*------------------------------------------------------------*/
/*                                                            */
/*    Turbo Vision -  Version 1.0                             */
/*                                                            */
/*                                                            */
/*    Copyright (c) 1991 by Borland International             */
/*    All Rights Reserved.                                    */
/*                                                            */
/*------------------------------------------------------------*/

#define Uses_TEventQueue
#define Uses_TEvent
#define Uses_TScreen
#define Uses_TKeys
#define Uses_TSystemError
#define Uses_TThreaded
#include "include\tv.h"
#include <stdio.h>

#ifdef __OS2__
#include <assert.h>
#endif

TEvent near TEventQueue::eventQueue[ eventQSize ] = { {0} };
TEvent *near TEventQueue::eventQHead = TEventQueue::eventQueue;
TEvent *near TEventQueue::eventQTail = TEventQueue::eventQueue;
Boolean near TEventQueue::mouseIntFlag = False;

#ifdef __MSDOS__

#include <dos.h>
#include <bios.h>

ushort *TEventQueue::Ticks = (ushort *)MK_FP( 0x000, 0x046c );
unsigned long getTicks(void) { return *(TEventQueue::Ticks); }

#else   // __MSDOS__

TEvent TEventQueue::keyboardEvent;
unsigned char TEventQueue::shiftState;

#endif  // __MSDOS__

ushort near TEventQueue::eventCount = 0;
Boolean near TEventQueue::mouseEvents = False;
Boolean near TEventQueue::mouseReverse = False;
ushort near TEventQueue::doubleDelay = 8;
ushort near TEventQueue::repeatDelay = 8;
ushort near TEventQueue::autoTicks = 0;
ushort near TEventQueue::autoDelay = 0;

MouseEventType near TEventQueue::lastMouse;
MouseEventType near TEventQueue::curMouse;
MouseEventType near TEventQueue::downMouse;
ushort near TEventQueue::downTicks = 0;

TEventQueue::TEventQueue()
{
#ifdef __MSDOS__
    resume();
#endif
}

void TEventQueue::resume()
{
#ifdef __OS2__
  {
    KBDINFO ki;
    ki.cb = sizeof(ki);
    assert(!KbdGetStatus(&ki,0));
    ki.fsMask &= ~KEYBOARD_ASCII_MODE;
    ki.fsMask |= KEYBOARD_BINARY_MODE;
    assert(!KbdSetStatus(&ki,0));
  }
#endif
    if( mouse.present() == False )
        mouse.resume();
    if( mouse.present() == False )
        return;
    mouse.getEvent( curMouse );
    lastMouse = curMouse;
    mouse.registerHandler( 0xFFFF, (void (*)(void))mouseInt );
    mouseEvents = True;
    mouse.show();
    TMouse::setRange( TScreen::screenWidth-1, TScreen::screenHeight-1 );
}

void TEventQueue::suspend()
{
#ifdef __OS2__
  {
    KBDINFO ki;
    ki.cb = sizeof(ki);
    assert(!KbdGetStatus(&ki,0));
    ki.fsMask |= KEYBOARD_ASCII_MODE;
    ki.fsMask &= ~KEYBOARD_BINARY_MODE;
    assert(!KbdSetStatus(&ki,0));
  }
#endif
    mouse.suspend();
}

TEventQueue::~TEventQueue()
{
#ifdef __MSDOS__
    suspend();
#endif
}

void TEventQueue::getMouseEvent( TEvent& ev )
{
    if( mouseEvents == True )
        {

        getMouseState( ev );

        if( ev.mouse.buttons == 0 && lastMouse.buttons != 0 )
            {
            ev.what = evMouseUp;
            lastMouse = ev.mouse;
            return;
            }

        if( ev.mouse.buttons != 0 && lastMouse.buttons == 0 )
            {
            if( ev.mouse.buttons == downMouse.buttons &&
                ev.mouse.where == downMouse.where &&
                ev.what - downTicks <= doubleDelay )
                    ev.mouse.doubleClick = True;

            downMouse = ev.mouse;
            autoTicks = downTicks = ev.what;
            autoDelay = repeatDelay;
            ev.what = evMouseDown;
            lastMouse = ev.mouse;
            return;
            }

        ev.mouse.buttons = lastMouse.buttons;

        if( ev.mouse.where != lastMouse.where )
            {
            ev.what = evMouseMove;
            lastMouse = ev.mouse;
            return;
            }

        if( ev.mouse.buttons != 0 && ev.what - autoTicks > autoDelay )
            {
            autoTicks = ev.what;
            autoDelay = 1;
            ev.what = evMouseAuto;
            lastMouse = ev.mouse;
            return;
            }
        }

    ev.what = evNothing;
}

void TEventQueue::getMouseState( TEvent & ev )
{
#ifdef __NT__
  ev.mouse.where   = lastMouse.where;
  ev.mouse.buttons = lastMouse.buttons;
  if ( lastMouse.buttons != 0 ) ev.what = getTicks();  // Temporarily save tick count when event was read.

  INPUT_RECORD *irp = TThreads::get_next_event();
  if ( irp == NULL || irp->EventType != MOUSE_EVENT ) return;
  TThreads::accept_event();
  curMouse.where.x = irp->Event.MouseEvent.dwMousePosition.X;
  curMouse.where.y = irp->Event.MouseEvent.dwMousePosition.Y;
  curMouse.buttons = irp->Event.MouseEvent.dwButtonState;
  curMouse.doubleClick = (irp->Event.MouseEvent.dwEventFlags & DOUBLE_CLICK)
                                ? True
                                : False;
  ev.mouse = curMouse;
  if ( lastMouse.buttons == 0 ) ev.what = getTicks();
#else   // __NT__
#ifdef __OS2__
    assert(! DosRequestMutexSem(TThreads::hmtxMouse1,SEM_INDEFINITE_WAIT) );
#else
    _disable();
#endif
    if( eventCount == 0 )
        {
        ev.what = getTicks();
        ev.mouse = curMouse;
        }
    else
        {
        ev = *eventQHead;
        if( ++eventQHead >= eventQueue + eventQSize )
            eventQHead = eventQueue;
        eventCount--;
        }
#ifdef __OS2__
    if (eventCount==0) {
      ULONG dummy;
      DosResetEventSem( TThreads::hevMouse1, &dummy );
    };
    assert(! DosReleaseMutexSem(TThreads::hmtxMouse1) );
#else
    _enable();
#endif  // __OS2__
#endif  // __NT__
    if( mouseReverse != False && ev.mouse.buttons != 0 && ev.mouse.buttons != 3 )
        ev.mouse.buttons ^= 3;
}

#ifdef __DOS16__
#pragma saveregs

void huge TEventQueue::mouseInt()
{
    unsigned flag = _AX;
    MouseEventType tempMouse;

    tempMouse.buttons = _BL;
    tempMouse.doubleClick = False;
    tempMouse.where.x = _CX >> 3;
    tempMouse.where.y = _DX >> 3;

    if( (flag & 0x1e) != 0 && eventCount < eventQSize )
        {
        eventQTail->what = *Ticks;
        eventQTail->mouse = curMouse;
        if( ++eventQTail >= eventQueue + eventQSize )
            eventQTail = eventQueue;
        eventCount++;
        }

    curMouse = tempMouse;
    mouseIntFlag = True;
}

void TEvent::getKeyEvent()
{
    asm {
        MOV AH,1;
        INT 16h;
        JNZ keyWaiting;
        };
        what = evNothing;
    return;

keyWaiting:
        what = evKeyDown;
    asm {
        MOV AH,0;
        INT 16h;
        };
        keyDown.keyCode = _AX;
    return;
}

#endif // __DOS16__
//---------------------------------------------------------------------
//---------------------------------------------------------------------
//---------------------------------------------------------------------
const unsigned char scSpaceKey          = 0x39;
const unsigned char scInsKey            = 0x52;
const unsigned char scDelKey            = 0x53;
const unsigned char scGreyEnterKey      = 0xE0;

#ifdef __DOS32__
void _loadds _far mouseInt(int flag,int buttons,int x,int y)
{
    MouseEventType tempMouse;

    tempMouse.buttons = buttons;
    tempMouse.doubleClick = False;
    tempMouse.where.x = x >> 3;
    tempMouse.where.y = y >> 3;

    if( (flag & 0x1e) != 0 && TEventQueue::eventCount < eventQSize )
        {
        TEventQueue::eventQTail->what = getTicks();
        TEventQueue::eventQTail->mouse.buttons     = TEventQueue::curMouse.buttons    ;
        TEventQueue::eventQTail->mouse.doubleClick = TEventQueue::curMouse.doubleClick;
        TEventQueue::eventQTail->mouse.where.x     = TEventQueue::curMouse.where.x    ;
        TEventQueue::eventQTail->mouse.where.y     = TEventQueue::curMouse.where.y    ;
        if( ++TEventQueue::eventQTail >= TEventQueue::eventQueue + eventQSize )
            TEventQueue::eventQTail = TEventQueue::eventQueue;
        TEventQueue::eventCount++;
        }

    TEventQueue::curMouse.buttons     = tempMouse.buttons    ;
    TEventQueue::curMouse.doubleClick = tempMouse.doubleClick;
    TEventQueue::curMouse.where.x     = tempMouse.where.x    ;
    TEventQueue::curMouse.where.y     = tempMouse.where.y    ;
    TEventQueue::mouseIntFlag = True;
}

//#include <stdio.h>
void TEvent::getKeyEvent() {
  if ( TSystemError::ctrlBreakHit == Boolean(1) ) {
    _bios_keybrd(_KEYBRD_READ);
    TSystemError::ctrlBreakHit = Boolean(2);
  }
  if ( _bios_keybrd(_KEYBRD_READY) == 0 ) {
        what = evNothing;
  } else {
        what = evKeyDown;
        keyDown.keyCode = _bios_keybrd(_KEYBRD_READ);
//printf("keyCode=%04X\n",keyDown.keyCode);
    switch (     keyDown.charScan.scanCode ) {
      case scSpaceKey:
        if ( getShiftState() & kbAltShift )     keyDown.keyCode = kbAltSpace;
        break;
      case scInsKey:
        {
          unsigned char state = getShiftState();
          if ( state & kbCtrlShift )     keyDown.keyCode = kbCtrlIns;
          else if ( state & (kbLeftShift|kbRightShift) )     keyDown.keyCode = kbShiftIns;
        }
        break;
      case scDelKey:
        {
          unsigned char state = getShiftState();
          if ( state & kbCtrlShift )     keyDown.keyCode = kbCtrlDel;
          else if ( state & (kbLeftShift|kbRightShift) )     keyDown.keyCode = kbShiftDel;
        }
        break;
    }
  }
}

#endif // __DOS32__
//---------------------------------------------------------------------
//---------------------------------------------------------------------
//---------------------------------------------------------------------
#ifdef __OS2__

void huge TEventQueue::mouseInt() { /* no mouse... */ }

// These variables are used to reduce the CPU time used by the
// two threads if they don't receive any input from the user for some time.
volatile int mousePollingDelay = 0;
volatile int keyboardPollingDelay = 0;

#define jsSuspendThread if (TThreads::shutDownFlag) break;

void TEventQueue::mouseThread(void* arg) {
  arg = arg;
  assert(! DosWaitEventSem(TThreads::hevMouse2, SEM_INDEFINITE_WAIT) );
  MouseEventType tempMouse;
  MOUEVENTINFO *info = &TThreads::tiled->mouseInfo;

  while (1) {
    do {
      jsSuspendThread
      USHORT type = 0; // non-blocking read
      MouReadEventQue(info, &type, TThreads::tiled->mouseHandle);
      if (info->time==0) {
        DosSleep(mousePollingDelay);
        if (mousePollingDelay < 500) mousePollingDelay += 5;
      } else {
        mousePollingDelay=0;
      }
    } while (info->time==0);
    tempMouse.buttons = ((info->fs & 06)  != 0) +
               (((info->fs & 030) != 0) << 1)+
               (((info->fs & 0140) != 0) << 2);
    tempMouse.where.x = info->col;
    tempMouse.where.y = info->row;
    tempMouse.doubleClick = False;

    jsSuspendThread
    DosRequestMutexSem(TThreads::hmtxMouse1,SEM_INDEFINITE_WAIT);
    if( (tempMouse.buttons!=curMouse.buttons) && eventCount < eventQSize ) {
      eventQTail->what = info->time/52; //*Ticks;
      eventQTail->mouse = tempMouse; // curMouse;
      if( ++eventQTail >= eventQueue + eventQSize ) eventQTail = eventQueue;
      eventCount++;
    }

    curMouse = tempMouse;

    APIRET rc = DosPostEventSem(TThreads::hevMouse1);
    assert( rc==0 || rc==ERROR_ALREADY_POSTED );
    assert(! DosReleaseMutexSem(TThreads::hmtxMouse1));
  }
  TThreads::deadEnd();
}


void TEventQueue::keyboardThread( void * arg ) {
  arg = arg;
  KBDKEYINFO *info = &TThreads::tiled->keyboardInfo;

  while (1) {
    jsSuspendThread
    USHORT errCode = KbdCharIn( info, IO_NOWAIT, 0 );
    jsSuspendThread
    if ( errCode || (info->fbStatus & 0xC0)!=0x40 || info->fbStatus & 1) { // no char
      keyboardEvent.what = evNothing;
      DosSleep(keyboardPollingDelay);
      if (keyboardPollingDelay < 500) keyboardPollingDelay += 5;
    } else {
    keyboardEvent.what = evKeyDown;

    if ((info->fbStatus & 2) && (info->chChar==0xE0)) info->chChar=0; // OS/2 cursor keys.
    keyboardEvent.keyDown.charScan.charCode=info->chChar;
    keyboardEvent.keyDown.charScan.scanCode=info->chScan;
    shiftState = info->fsState & 0xFF;

    jsSuspendThread

    assert(! DosPostEventSem(TThreads::hevKeyboard1) );

    jsSuspendThread
    assert(! DosWaitEventSem(TThreads::hevKeyboard2, SEM_INDEFINITE_WAIT) );
    keyboardEvent.what = evNothing;
    ULONG dummy;
    jsSuspendThread
    assert(! DosResetEventSem(TThreads::hevKeyboard2, &dummy) );
    keyboardPollingDelay=0;
    }
  }
  TThreads::deadEnd();
}

// Some Scancode conversion.. in OS/2 ShiftIns/ShiftDel only
// I got understand the CtrlDel/Ins Conversion. Maybe this is
// XT-Keyboard only

static unsigned char translateTable1[] =
  { scSpaceKey,
    scInsKey, scDelKey,
    scInsKey, scDelKey  };

static unsigned char translateTable2[] =
  { kbAltShift,
    kbCtrlShift, kbCtrlShift,
    kbLeftShift+kbRightShift, kbLeftShift+kbRightShift };

static unsigned short translateTable3[]=
  { kbAltSpace,
    kbCtrlIns,  kbCtrlDel,
    kbShiftIns, kbShiftDel };

const unsigned translateTableLength = 5;


void TEvent::getKeyEvent()
{
  unsigned char shiftState = 0;
  ULONG dummy;
  assert(! DosResetEventSem(TThreads::hevKeyboard1, &dummy) );

  *this = TEventQueue::keyboardEvent;
  shiftState = TEventQueue::shiftState;
  assert(! DosPostEventSem(TThreads::hevKeyboard2) );

    for (int i=0; i<translateTableLength; i++) {
      if ((translateTable1[i]==    keyDown.charScan.scanCode) &&
          (translateTable2[i] & shiftState))
      {
            keyDown.keyCode = translateTable3[i];
        break;
      };
    };
    if (     keyDown.charScan.scanCode == scGreyEnterKey )
          keyDown.keyCode = kbEnter;

  return;

}








#endif  // __OS2__

//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
//-------------------------------------------------------------------------

#ifdef __NT__

static ushort ShiftCvt[89] = {
         0,      0,      0,      0,      0,      0,      0,      0,
         0,      0,      0,      0,      0,      0,      0, 0x0F00,
         0,      0,      0,      0,      0,      0,      0,      0,
         0,      0,      0,      0,      0,      0,      0,      0,
         0,      0,      0,      0,      0,      0,      0,      0,
         0,      0,      0,      0,      0,      0,      0,      0,
         0,      0,      0,      0,      0,      0,      0,      0,
         0,      0,      0, 0x5400, 0x5500, 0x5600, 0x5700, 0x5800,
    0x5900, 0x5A00, 0x5B00, 0x5C00, 0x5D00,      0,      0,      0,
         0,      0,      0,      0,      0,      0,      0,      0,
         0,      0, 0x0500, 0x0700,      0,      0,      0, 0x8700,
    0x8800
};

static ushort CtrlCvt[89] = {
         0,      0,      0,      0,      0,      0,      0,      0,
         0,      0,      0,      0,      0,      0,      0,      0,
    0x0011, 0x0017, 0x0005, 0x0012, 0x0014, 0x0019, 0x0015, 0x0009,
    0x000F, 0x0010,      0,      0,      0,      0, 0x0001, 0x0013,
    0x0004, 0x0006, 0x0007, 0x0008, 0x000A, 0x000B, 0x000C,      0,
         0,      0,      0,      0, 0x001A, 0x0018, 0x0003, 0x0016,
    0x0002, 0x000E, 0x000D,      0,      0,      0,      0,      0,
         0,      0,      0, 0x5E00, 0x5F00, 0x6000, 0x6100, 0x6200,
    0x6300, 0x6400, 0x6500, 0x6600, 0x6700,      0,      0, 0x7700,
         0, 0x8400,      0, 0x7300,      0, 0x7400,      0, 0x7500,
         0, 0x7600, 0x0400, 0x0600,      0,      0,      0, 0x8900,
    0x8A00
};

static ushort AltCvt[89] = {
         0,      0, 0x7800, 0x7900, 0x7A00, 0x7B00, 0x7C00, 0x7D00,
    0x7E00, 0x7F00, 0x8000, 0x8100, 0x8200, 0x8300, 0x0800,      0,
    0x1000, 0x1100, 0x1200, 0x1300, 0x1400, 0x1500, 0x1600, 0x1700,
    0x1800, 0x1900,      0,      0,      0,      0, 0x1E00, 0x1F00,
    0x2000, 0x2100, 0x2200, 0x2300, 0x2400, 0x2500, 0x2600,      0,
         0,      0,      0,      0, 0x2C00, 0x2D00, 0x2E00, 0x2F00,
    0x3000, 0x3100, 0x3200,      0,      0,      0,      0,      0,
         0, 0x0200,      0, 0x6800, 0x6900, 0x6A00, 0x6B00, 0x6C00,
    0x6D00, 0x6E00, 0x6F00, 0x7000, 0x7100,      0,      0,      0,
         0,      0,      0,      0,      0,      0,      0,      0,
         0,      0,      0,      0,      0,      0,      0, 0x8B00,
    0x8C00
};

void huge TEventQueue::mouseInt() { /* no mouse... */ }

void TEvent::getKeyEvent()
{
      what = evNothing;
  INPUT_RECORD *irp = TThreads::get_next_event();
  if ( irp == NULL || irp->EventType != KEY_EVENT ) return;
  TThreads::accept_event();
      keyDown.charScan.scanCode = irp->Event.KeyEvent.wVirtualScanCode;
      keyDown.charScan.charCode = irp->Event.KeyEvent.uChar.AsciiChar;

//  Skip shifts and caps,num,scroll locks
  static char ignore[] = { 0x1D,0x2A,0x38,0x36,0x3A,0x45,0x46,0 };
  if ( strchr(ignore,    keyDown.charScan.scanCode) != NULL )
    return;
      what = evKeyDown;

#define kbShift (kbLeftShift|kbRightShift)
  int shift = getShiftState();

  /* Convert NT style virtual scan codes to PC BIOS codes.
   */
  if ( (shift & (kbShift | kbAltShift | kbCtrlShift)) != 0 ) {
    uchar index =     keyDown.charScan.scanCode;

    if ((shift & kbShift) && ShiftCvt[index] != 0)
            keyDown.keyCode = ShiftCvt[index];
    else if ((shift & kbCtrlShift) && CtrlCvt[index] != 0)
            keyDown.keyCode = CtrlCvt[index];
    else if ((shift & kbAltShift) && AltCvt[index] != 0)
            keyDown.keyCode = AltCvt[index];
  }
//  printf("virt=%04X code=%04X\n",
//              TThreads::irBuffer.Event.KeyEvent.wVirtualScanCode,
//              keyDown.keyCode);

  // Need to handle special case of Alt-Space, Ctrl-Ins, Shift-Ins,
  // Ctrl-Del, Shift-Del

  switch(     keyDown.keyCode ) {
    case ' ':
        if( shift & kbAltShift )     keyDown.keyCode = kbAltSpace;
        break;

    case kbDel:
        if( shift & kbCtrlShift )
                keyDown.keyCode = kbCtrlDel;
        else if( shift & kbShift )
                keyDown.keyCode = kbShiftDel;
        break;

    case kbIns:
        if( shift & kbCtrlShift )
                keyDown.keyCode = kbCtrlIns;
        else if( shift & kbShift )
                keyDown.keyCode = kbShiftIns;
        break;
  }
}

#endif // __NT__
