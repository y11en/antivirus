/*------------------------------------------------------------------------*/
/* filename - geninc.cpp                                                  */
/*                                                                        */
/*      generates assembler EQUates for offsets of                        */
/*      class data members                                                */
/*                                                                        */
/*  Used only before build!  Compile to produce GENINC.EXE,               */
/*  then execute GENINC >TVWRITE.INC to produce the .INC                  */
/*  file needed by the assembler files                                    */
/*                                                                        */
/*------------------------------------------------------------------------*/

/*------------------------------------------------------------------------*/
/*                                                                        */
/*    Turbo Vision -  Version 1.0                                         */
/*                                                                        */
/*                                                                        */
/*    Copyright (c) 1991 by Borland International                         */
/*    All Rights Reserved.                                                */
/*                                                                        */
/*------------------------------------------------------------------------*/

#define Uses_TPoint
#define Uses_TView
#define Uses_TGroup
#define Uses_TEvent
#define Uses_TFrame
#define Uses_TDrawBuffer
#define Uses_TEditor
#define Uses_TTerminal
#include "include\tv.h"

#include <iostream.h>
#include <iomanip.h>
#include <stddef.h>

#define genConst( n )  generate( #n, n )

#define gen( n, c, o ) generate( #n, offsetof( c, o ) )
#define genadd( n, c, o, a ) generate( #n, offsetof( c, o ) + a )
#define genx( n, c, o ) genadd(n,c,o,offsetof(TPoint,x))
#define geny( n, c, o ) genadd(n,c,o,offsetof(TPoint,y))
#define genax( n, c, o ) genadd(n,c,o,offsetof(TRect,a)+offsetof(TPoint,x))
#define genay( n, c, o ) genadd(n,c,o,offsetof(TRect,a)+offsetof(TPoint,y))
#define genbx( n, c, o ) genadd(n,c,o,offsetof(TRect,b)+offsetof(TPoint,x))
#define genby( n, c, o ) genadd(n,c,o,offsetof(TRect,b)+offsetof(TPoint,y))

void generate( const char *name, size_t offset )
{
    cout << setw( 19 ) << setiosflags( ios::left )
         << name << " equ " << offset << endl;
}

#ifdef __WATCOMC__
#define _class  class
#else
#define _class
#endif

void genRefs()
{
    gen( TPointX,           TPoint, x );
    gen( TPointY,           TPoint, y );

    genx( TViewSizeX,        TView,  size );
    geny( TViewSizeY,        TView,  size );
    gen( TViewState,        TView,  state );
    gen( TViewOwner,        TView,  owner );
    geny( TViewOriginY,      TView,  origin );
    genx( TViewOriginX,      TView,  origin );
    geny( TViewCursorY,      TView,  cursor );
    genx( TViewCursorX,      TView,  cursor );
    gen( TViewNext,         TView,  next );
    gen( TViewOptions,      TView,  options );

    genay( TGroupClipAY,      TGroup, clip );
    genax( TGroupClipAX,      TGroup, clip );
    genby( TGroupClipBY,      TGroup, clip );
    genbx( TGroupClipBX,      TGroup, clip );
    gen( TGroupLast,        TGroup, last );
    gen( TGroupBuffer,      TGroup, buffer );
    gen( TGroupLockFlag,    TGroup, lockFlag );

    genx( MsEventWhereX,     MouseEventType, where );
    geny( MsEventWhereY,     MouseEventType, where );

    genx( TFrameSizeX,       TFrame, size );
    gen( TFrameOwner,       TFrame, owner );

    gen( TDrawBufferData,   TDrawBuffer, data );
    gen( TEditorCurPtr,     TEditor, curPtr );
    gen( TEditorGapLen,     TEditor, gapLen );
    gen( TEditorBuffer,     TEditor, buffer );
    gen( TEditorSelStart,   TEditor, selStart );
    gen( TEditorSelEnd,     TEditor, selEnd );
    gen( TEditorBufSize,    TEditor, bufSize );
    gen( TEditorBufLen,     TEditor, bufLen );
    gen( TTerminalBuffer,   TTerminal, buffer );
    gen( TTerminalBufSize,  TTerminal, bufSize );
    gen( TTerminalQueBack,  TTerminal, queBack );

    genConst( sfVisible );
    genConst( sfCursorVis );
    genConst( sfCursorIns );
    genConst( sfFocused );
    genConst( sfShadow );
    genConst( sfExposed );
    genConst( ofFramed );
}

int main()
{
    genRefs();
    return 0;
}
