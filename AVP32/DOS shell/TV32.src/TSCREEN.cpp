/*------------------------------------------------------------*/
/* filename -       tscreen.cpp                               */
/*                                                            */
/* function(s)                                                */
/*                  TScreen member functions                  */
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

#define Uses_TScreen
#define Uses_TEvent
#define Uses_TThreaded
#include "include\tv.h"

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#ifdef __MSDOS__
#include <dos.h>
ushort far * near TDisplay::equipment = (ushort far *)MK_FP( 0, 0x410 );
uchar far * near TDisplay::crtInfo = (uchar far *)MK_FP( 0, 0x487 );
uchar far * near TDisplay::crtRows = (uchar far *)MK_FP( 0, 0x484 );
#endif

ushort near TScreen::startupMode = 0xFFFF;
ushort near TScreen::startupCursor = 0;
#ifdef __MSDOS__
ushort near TScreen::screenMode = 0;
#else
ushort near TScreen::screenMode = (80 << 8) | 25;
#endif
uchar near TScreen::screenWidth = 0;
uchar near TScreen::screenHeight = 0;
Boolean near TScreen::hiResScreen = False;
Boolean near TScreen::checkSnow = False;
uchar far * near TScreen::screenBuffer = 0;
ushort near TScreen::cursorLines = 0;

inline void checkColumns(void) {
  if ( TScreen::screenWidth > maxViewWidth ) {
    fprintf(stderr,"Tvision: too wide screen!\n");
    abort();
  }
}

//--------------------------------------------------------------------------

#ifdef __MSDOS__
void TScreen::resume()
{
    startupMode = getCrtMode();
    startupCursor = getCursorType();
    if (screenMode != startupMode)
       setCrtMode( screenMode );
    setCrtData();
}

TScreen::~TScreen()
{
    suspend();
}
#endif // __MSDOS__

//--------------------------------------------------------------------------

#ifdef __DOS16__

ushort TDisplay::getCursorType()
{
    _AH = 3;
    _BH = 0;
    videoInt();
    return _CX;
}

void TDisplay::setCursorType( ushort ct )
{
    _AH = 1;
    _CX = ct;
    videoInt();
}

void TDisplay::clearScreen( uchar w, uchar h )
{
    _BH = 0x07;
    _CX = 0;
    _DL = w;
    _DH = h - 1;
    _AX = 0x0600;
    videoInt();
}

void TDisplay::videoInt()
{
    asm {
        PUSH    BP
        PUSH    ES
        INT     10h
        POP     ES
        POP     BP
    }
}

ushort TDisplay::getRows()
{
    _AX = 0x1130;
    _BH = 0;
    _DL = 0;
    videoInt();
    if( _DL == 0 )
        _DL = 24;
    return _DL + 1;
}

ushort TDisplay::getCols()
{
    _AH = 0x0F;
    videoInt();
    return _AH;
}

ushort TDisplay::getCrtMode()
{
    _AH = 0x0F;
    videoInt();
    ushort mode = _AL;
#ifndef __NOROW__
    if( getRows() > 25 )
        mode |= smFont8x8;
#endif
    return mode;
}


void TDisplay::setCrtMode( ushort mode )
{
    *equipment &= 0xFFCF;
    *equipment |= (mode == smMono) ? 0x30 : 0x20;
    *crtInfo &= 0x00FE;

    _AH = 0;
    _AL = mode;
    videoInt();


#ifndef __NOROW__
    if( (mode & smFont8x8) != 0 )
        {
        _AX = 0x1112;
        _BL = 0;
        videoInt();

        if( getRows() > 25 )
            {
            *crtInfo |= 1;

            _AH = 1;
            _CX = 0x0607;
            videoInt();

            _AH = 0x12;
            _BL = 0x20;
            videoInt();
            }
        }
#endif
}

ushort TScreen::fixCrtMode( ushort mode )
{
    _AX = mode;
#ifndef __NOROW__
    if( _AL != smMono && _AL != smCO80 && _AL != smBW80 )
        _AL = smCO80;
#endif
    return _AX;
}

void TScreen::setCrtData()
{
    screenMode = getCrtMode();
    screenWidth = getCols();
    screenHeight = getRows();
    checkColumns();
    hiResScreen = Boolean(screenHeight > 25);

    if ( screenMode == smMono ) {
      screenBuffer = (uchar far *)MK_FP( 0xB000, 0 );
      checkSnow = False;
    } else {
      screenBuffer = (uchar far *)MK_FP( 0xB800, 0 );
      if( hiResScreen ) checkSnow = False;
    }

    cursorLines = getCursorType();
    setCursorType( 0x2000 );

}

#endif // __DOS16_
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
#ifdef __DOS32__

ushort TDisplay::getCursorType()
{
  REGS r;
  r.h.ah = 3;
  r.h.bh = 0;
  int386(0x10,&r,&r);
  return r.w.cx;
}

void TDisplay::setCursorType( ushort ct )
{
  REGS r;
  r.h.ah = 1;
  r.w.cx = ct;
  int386(0x10,&r,&r);
}

void TDisplay::clearScreen( uchar w, uchar h )
{
  REGS r;
  r.w.ax = 0x0600;
  r.h.bh = 0x07;
  r.w.cx = 0;
  r.h.dl = w;
  r.h.dh = h - 1;
  int386(0x10,&r,&r);
}

ushort TDisplay::getRows()
{
  REGS r;
  r.w.ax = 0x1130;
  r.h.bh = 0x0;
  r.h.dl = 0x0;
  int386(0x10,&r,&r);
  if( r.h.dl == 0 ) r.h.dl = 24;
  return r.h.dl + 1;
}

ushort TDisplay::getCols()
{
  REGS r;
  r.h.ah = 0x0F;
  int386(0x10,&r,&r);
  return r.h.ah;
}

ushort TDisplay::getCrtMode()
{
    REGS r;
    r.h.ah = 0x0F;
    int386(0x10,&r,&r);
    ushort mode = r.h.al;
#ifndef __NOROW__
    if( getRows() > 25 )
        mode |= smFont8x8;
#endif
    return mode;
}


void TDisplay::setCrtMode( ushort mode )
{
    *equipment &= 0xFFCF;
    *equipment |= (mode == smMono) ? 0x30 : 0x20;
    *crtInfo &= 0x00FE;

  REGS r;
  r.h.ah = 0x0;
  r.h.al = mode;
  int386(0x10,&r,&r);

#ifndef __NOROW__
    if( (mode & smFont8x8) != 0 )
        {
        r.w.ax = 0x1112;
        r.h.bl = 0;
        int386(0x10,&r,&r);

        if( getRows() > 25 )
            {
            *crtInfo |= 1;

            r.h.ah = 1;
            r.w.cx = 0x0607;
            int386(0x10,&r,&r);

            r.h.ah = 0x12;
            r.h.bl = 0x20;
            int386(0x10,&r,&r);
            }
        }
#endif // __NOROW__
}

ushort TScreen::fixCrtMode( ushort mode )
{
#ifdef __NOROW__
    return mode;
#else
    char m = mode;
    if( m != smMono && m != smCO80 && m != smBW80 )
        m = smCO80;
    return (mode & ~0xFF) + m;
#endif // __NOROW__
}

void TScreen::setCrtData()
{
    screenMode = getCrtMode();
    screenWidth = getCols();
    screenHeight = getRows();
    checkColumns();
    hiResScreen = Boolean(screenHeight > 25);

    if( screenMode == smMono )
        {
        screenBuffer = (uchar far *)MK_FP( 0, 0xB0000 );
        checkSnow = False;
        }
    else
        {
        screenBuffer = (uchar far *)MK_FP( 0, 0xB8000 );
        if( hiResScreen )
            checkSnow = False;
        }

    cursorLines = getCursorType();
    setCursorType( 0x2000 );

}

#endif // __DOS32__
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
#ifdef __OS2__

ushort TDisplay::getCursorType()
{
   VIOCURSORINFO cinfo;
   VioGetCurType(&cinfo,NULL);
   return (cinfo.yStart << 8) + cinfo.cEnd;
}

void TDisplay::setCursorType( ushort ct )
{
   VIOCURSORINFO cinfo;
   VioGetCurType(&cinfo,NULL);
   if ( ct>>8 == 0x20 ) {
     cinfo.attr   = 0xFFFF;          // hide
   } else {
     cinfo.attr   = 0;               // show
     cinfo.yStart = uchar(ct>>8);
     cinfo.cEnd   = uchar(ct);
     cinfo.cx     = 0;
   }
   VioSetCurType(&cinfo,NULL);
}

void TDisplay::clearScreen( uchar w, uchar h )
{
   char cell = ' ';
   VioScrollUp (0,0,h,w,h,&cell,NULL);
}

ushort TDisplay::getRows()
{
    VIOMODEINFO info;
    info.cb=sizeof(VIOMODEINFO);
    VioGetMode(&info,0);
    return info.row;
}

ushort TDisplay::getCols()
{
    VIOMODEINFO info;
    info.cb=sizeof(VIOMODEINFO);
    VioGetMode(&info,0);
    return info.col;
}

ushort TDisplay::getCrtMode()
{
  return getRows() + (getCols() << 8);
}

void TDisplay::setCrtMode( ushort mode )
{
    VIOMODEINFO info;
    info.cb = sizeof(VIOMODEINFO);
    VioGetMode(&info,0);
    info.cb = 8;
    info.col = uchar(mode >> 8);
    info.row = uchar(mode);
    VioSetMode(&info,0);
    clearScreen(info.col,info.row);
}

void TScreen::resume()
{
    if (screenMode != startupMode)
       setCrtMode( screenMode );
    setCrtData();
}

TScreen::~TScreen()
{
}

ushort TScreen::fixCrtMode( ushort mode )
{
  if ( char(mode) == 0 || char(mode>>8) == 0 ) return 0;
  return mode;
}

void TScreen::setCrtData()
{
    screenMode = getCrtMode();
    screenWidth = getCols();
    screenHeight = getRows();
    checkColumns();
    hiResScreen = Boolean(screenHeight > 25);

    uchar _far16 *s;
    ushort l;
    VioGetBuf( (PULONG) &s, &l, 0 ); // !!! OS/2 specific
    screenBuffer = s; // Automatic conversion

    checkSnow = False;
    cursorLines = getCursorType();
    setCursorType( 0x2000 );

}

#endif  // __OS2__

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
#ifdef __NT__

ushort TDisplay::getCursorType()
{
   GetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE),&TThreads::crInfo);
   int ct = TThreads::crInfo.bVisible
                ? TThreads::crInfo.dwSize*31/100
                : 0x2000;
   return ct;
}

void TDisplay::setCursorType( ushort ct )
{
  if ( (ct>>8) == 0x20 ) {
    TThreads::crInfo.bVisible = False;
    TThreads::crInfo.dwSize = 1;
  } else {
    TThreads::crInfo.bVisible = True;
    TThreads::crInfo.dwSize = (uchar(ct)-uchar(ct>>8))*100/31;
  }
  SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE),&TThreads::crInfo);
}

void TDisplay::clearScreen( uchar w, uchar h )
{
  COORD coord = { 0, 0 };
  DWORD read;

  FillConsoleOutputAttribute( TThreads::chandle[cnOutput], 0x07, w*h, coord, &read );
  FillConsoleOutputCharacterA( TThreads::chandle[cnOutput], ' ', w*h, coord, &read );
}

ushort TDisplay::getRows()
{
  GetConsoleScreenBufferInfo( TThreads::chandle[cnOutput], &TThreads::sbInfo );
  return TThreads::sbInfo.dwSize.Y;
}

ushort TDisplay::getCols()
{
  GetConsoleScreenBufferInfo( TThreads::chandle[cnOutput], &TThreads::sbInfo );
  return TThreads::sbInfo.dwSize.X;
}

ushort TDisplay::getCrtMode()
{
  return getRows() + (getCols() << 8);
}

void TDisplay::setCrtMode( ushort mode )
{
  int cols = uchar(mode >> 8);
  int rows = uchar(mode);
  if ( cols == 0 ) cols = TThreads::sbInfo.dwSize.X;
  if ( rows == 0 ) rows = TThreads::sbInfo.dwSize.Y;
  COORD newSize = { cols, rows };
  SMALL_RECT rect = { 0, 0, cols-1, rows-1 };

  COORD maxSize = GetLargestConsoleWindowSize( TThreads::chandle[cnOutput] );
  if ( newSize.Y > maxSize.Y ) {
    newSize.Y = maxSize.Y;
    rect.Bottom = newSize.Y-1;
  }
  assert(SetConsoleScreenBufferSize( TThreads::chandle[cnOutput], newSize ));
  assert(SetConsoleWindowInfo( TThreads::chandle[cnOutput], True, &rect ));
  GetConsoleScreenBufferInfo( TThreads::chandle[cnOutput], &TThreads::sbInfo );
}

void TScreen::resume()
{
  if (screenMode != startupMode)
     setCrtMode( screenMode );
  setCrtData();
}

TScreen::~TScreen()
{
  if ( screenBuffer != NULL ) VirtualFree( screenBuffer, 0, MEM_RELEASE );
  screenBuffer = NULL;
}

ushort TScreen::fixCrtMode( ushort mode )
{
  if ( char(mode) == 0 || char(mode>>8) == 0 ) return 0;
  return mode;
}

void TScreen::setCrtData()
{
  screenMode = getCrtMode();
  screenWidth = getCols();
  screenHeight = getRows();
  checkColumns();
  hiResScreen = Boolean(screenHeight > 25);

  short x = screenWidth;
  short y = screenHeight;
  if( x < 80 )        // Make sure we allocate at least enough for
      x = 80;         //   a 80x25 screen.
  if( y < 25 )
      y = 25;
  if ( screenBuffer != NULL ) VirtualFree( screenBuffer, 0, MEM_RELEASE );
  screenBuffer = (uchar *) VirtualAlloc( 0, x * y * sizeof(ushort), MEM_COMMIT, PAGE_READWRITE );

  checkSnow = False;
  cursorLines = getCursorType();
  setCursorType( 0x2000 );
}

#endif  // __NT__

//---------------------------------------------------------------------------
TScreen::TScreen()
{
    startupMode = getCrtMode();
    startupCursor = getCursorType();
    setCrtData();
}

void TScreen::suspend()
{
    if( startupMode != screenMode )
        setCrtMode( startupMode );
    clearScreen();
    setCursorType( startupCursor );
}

void TScreen::clearScreen()
{
    TDisplay::clearScreen( screenWidth, screenHeight );
}

void TScreen::setVideoMode( ushort mode )
{
    setCrtMode( fixCrtMode( mode ) );
    setCrtData();
}
