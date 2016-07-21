#include <os2.h>
#include <fcntl.h>
#include <share.h>
#include <io.h>

#include "tv32def.h"

#define Uses_TKeys
#define Uses_TIndicator
#define Uses_TScrollBar
#define Uses_TWindow
#define Uses_TEvent
#include "include\tv.h"
#define __TListViews
#define __TViewer
#include "classes.h"
#include "contain.h"

#if !defined( __STRING_H )
#include <string.h>
#endif  // __STRING_H

#if !defined( __CTYPE_H )
#include <ctype.h>
#endif  // __CTYPE_H

#if !defined( __DOS_H )
#include <Dos.h>
#endif

inline int isWordChar( int ch )
 {
  return isalnum(ch) || ch=='_';
 }

static const ushort firstKeys[] =
 {
  37,
  kbCtrlA, cmWordLeft,
  kbCtrlC, cmPageDown,
  kbCtrlD, cmCharRight,
  kbCtrlE, cmLineUp,
  kbCtrlF, cmWordRight,
  kbCtrlG, cmDelChar,
  kbCtrlH, cmBackSpace,
  kbCtrlK, 0xFF02,
  kbCtrlL, cmSearchAgain,
  kbCtrlM, cmNewLine,
  kbCtrlO, cmIndentMode,
  kbCtrlQ, 0xFF01,
  kbCtrlR, cmPageUp,
  kbCtrlS, cmCharLeft,
  kbCtrlT, cmDelWord,
  kbCtrlU, cmUndo,
  kbCtrlV, cmInsMode,
  kbCtrlX, cmLineDown,
  kbCtrlY, cmDelLine,
  kbLeft, cmCharLeft,
  kbRight, cmCharRight,
  kbCtrlLeft, cmWordLeft,
  kbCtrlRight, cmWordRight,
  kbHome, cmLineStart,
  kbEnd, cmLineEnd,
  kbUp, cmLineUp,
  kbDown, cmLineDown,
  kbPgUp, cmPageUp,
  kbPgDn, cmPageDown,
  kbCtrlPgUp, cmTextStart,
  kbCtrlPgDn, cmTextEnd,
  kbIns, cmInsMode,
  kbDel, cmDelChar,
  kbShiftIns, cmPaste,
  kbShiftDel, cmCut,
  kbCtrlIns, cmCopy,
  kbCtrlDel, cmClear
 };

static const ushort quickKeys[] =
 {
  8,
  'A', cmReplace,
  'C', cmTextEnd,
  'D', cmLineEnd,
  'F', cmFind,
  'H', cmDelStart,
  'R', cmTextStart,
  'S', cmLineStart,
  'Y', cmDelEnd
 };

//static const ushort blockKeys[] =
// {
//  5,
//  'B', cmStartSelect,
//  'C', cmPaste,
//  'H', cmHideSelect,
//  'K', cmCopy,
//  'Y', cmCut
// };

static const ushort *keyMap[] = { firstKeys, quickKeys/*, blockKeys */};

ushort defEditorDialog(int, ... )
 {
  return cmCancel;
 }

ushort scanKeyMap( const void *keyMap, int keyCode )
 {
  ushort * map = (ushort*) keyMap;
  for (int count=0; count< map[0]; count++)
   {
    int i=count*2+1;
    if ((keyCode & 0xFF)==(map[i] & 0xFF) &&
        ((map[i] & 0xFF00)==0 || (keyCode & 0xFF00)==(map[i] & 0xFF00)) )
      return map[i+1];
   };
  return 0;
 }

#define cpEditor    "\x06\x07"

TViewer::TViewer(const TRect& bounds,TScrollBar *aHScrollBar,
                 TScrollBar *aVScrollBar,TIndicator *aIndicator,
                 ULONG aBufSize,const char *FileName) :
    TView(bounds),
    hScrollBar(aHScrollBar),vScrollBar(aVScrollBar),indicator(aIndicator),
    bufSize(aBufSize),
    canUndo(True),
    selecting(False),
    overwrite(False),
    autoIndent(False),
    lockCount(0),
    keyState(0)
 {
  growMode=gfGrowHiX|gfGrowHiY;
  options|=ofSelectable;
  eventMask=evMouseDown|evKeyDown|evCommand|evBroadcast;
  showCursor();
  hFile=-1;                        // File handle
 //     strcpy(fileName,FileName);
  fexpand((char*)FileName);
  if((hFile=sopen((char*)FileName,O_RDONLY|O_BINARY,SH_COMPAT|SH_DENYNO,0))==-1)
   {
    editorDialog(edReadError,FileName);
    setBufLen(0);
    return;
   }

  bufSize=filelength(hFile);//!=(-1))
  initBuffer();
  //setBufSize(length);
  //setBufLen(bufSize);
  if(frame!=0)
   {
    isValid=True;
    if(FileName==0) ;//fileName[0]=EOS;
    else
     {
      if(isValid) isValid=loadFromFile();
      setBufLen(bufSize);
     }
   }
  else
   {
    editorDialog(edOutOfMemory);
    bufSize=0;
    isValid=False;
   }
 }

TViewer::~TViewer()
 {
  close(hFile);
 }

void TViewer::shutDown()
 {
  setCmdState(cmSave, False); //TFileViewer
  setCmdState(cmSaveAs, False);

  doneBuffer();
  TView::shutDown();
 }

int countLines(void *buf,ULONG count)
 {
  int anzahl=0;
  char *str=(char*)buf;
  for(unsigned int i=0; i<count; i++) if(*(str++)==0x0d) anzahl++;
  return anzahl;
 }

void TViewer::setBufLen(ULONG length)
 {
  bufLen  =length;
//  gapLen  =bufSize-length;
  //selStart=0;
  //selEnd  =0;
  curPtr  =0;
  delta.x =0;
  delta.y =0;
  curPos  =delta;
  unsigned int len=0;
  limit.x =maxLineLength;
  for(unsigned int i=0;i<bufLen;i++)
   {
    len++;
    if(frame[i]==0x0d)
     {
      if(len>limit.x) limit.x=len+2;
      len=0;
     }
   }
  limit.y =countLines(frame,bufLen)+1;
//  limit.y =countLines(&frame[gapLen],bufLen)+1;
  drawLine=0;
  drawPtr =0;
  //delCount=0;
  //insCount=0;
  //modified=False;
  update(ufView);
 }

Boolean TViewer::getBufSize(ULONG newSize)
 {
  return Boolean(newSize<=bufSize); //old
 }

Boolean TViewer::setBufSize(ULONG newSize)
 {
  newSize=(newSize+0x0FFF) & (UINT_MAX-0x0FFF);
  if(newSize!=bufSize)
   {
    char *temp=frame;
    if( (frame=new char[newSize])==0)
     {
      delete temp; //old buffer
      return False;
     }
    ULONG n = bufLen-curPtr;//+delCount;
    memcpy(frame,temp,min(newSize,bufSize) );
    memmove(&frame[newSize-n],&temp[bufSize-n],n);
    delete temp;
    bufSize=newSize;
//    gapLen =bufSize-bufLen;
   }
  return True;
 }

void TViewer::initBuffer()
 {
  frame=0;
//bufSize=32768;
  while(frame==0)
   {
    frame=new char[bufSize+1];
    if(frame!=NULL)
     {
      memset(frame,0,bufSize+1);
      return;
     }
    bufSize/=2;
   }
 }

void TViewer::doneBuffer()
 {
  delete frame;
 }

//Boolean TViewer::insertBuffer( char *p,ULONG offset,ULONG length,
//                               Boolean allowUndo,Boolean selectText)
// {
//  selecting = False;
//  ULONG selLen = selEnd - selStart;
//  if( selLen == 0 && length == 0 ) return True;

//  ULONG delLen = 0;
//  if( allowUndo == True )
//   if( curPtr == selStart )
//     delLen = selLen;
//   else
//     if( selLen > insCount ) delLen = selLen - insCount;
//
//  long newSize = long(bufLen + delCount - selLen + delLen) + length;
//
//  if( newSize > bufLen + delCount )
//   if( newSize > (UINT_MAX-0x1F) || getBufSize((ULONG)newSize)==False )
//    {
//     editorDialog( edOutOfMemory );
//     return False;
//    }
//
//  ULONG selLines = countLines( &frame[bufPtr(selStart)], selLen );
//  if( curPtr == selEnd )
//   {
//    if( allowUndo == True )
//     {
//      if( delLen > 0 )
//          memmove(&frame[curPtr + gapLen - delCount - delLen],
//                   &frame[selStart],delLen);
//      insCount -= selLen - delLen;
//     }
//    curPtr = selStart;
//    curPos.y -= selLines;
//   }
//  if( delta.y > curPos.y )
//   {
//    delta.y -= selLines;
//    if( delta.y < curPos.y ) delta.y = curPos.y;
//   }
//
//  if( length > 0 ) memmove(&frame[curPtr],&p[offset],length);
//
//  ULONG lines = countLines( &frame[curPtr], length );
//  curPtr  +=length;
//  curPos.y+=lines;
//  drawLine =curPos.y;
//  drawPtr  =lineStart(curPtr);
//  curPos.x =charPos(drawPtr, curPtr);
//  if( selectText==False ) selStart=curPtr;
//  selEnd   =curPtr;
//  bufLen  +=length-selLen;
//  gapLen  -=length-selLen;
//  if( allowUndo == True )
//   {
//    delCount +=delLen;
//    insCount +=length;
//   }
//  limit.y += lines - selLines;
//  delta.y = max(0, min(delta.y, limit.y - size.y));
//  //if( isClipboard() == False ) modified=True;
//  getBufSize(bufLen + delCount);
//  if( selLines == 0 && lines == 0 ) update(ufLine);
//  else                              update(ufView);
//  return True;
// }
//
//Boolean TViewer::insertFrom( TViewer *editor )
// {
//  return insertBuffer(editor->frame,editor->bufPtr(editor->selStart),
//               editor->selEnd - editor->selStart,canUndo,False);//isClipboard() );
// }

//Boolean TViewer::insertText( const void *text,ULONG length, Boolean selectText )
// {
//  return insertBuffer( (char *)text, 0, length, canUndo, selectText);
// }

void TViewer::update(uchar aFlags)
 {
  updateFlags|=aFlags;
  if(lockCount==0) doUpdate();
 }

void TViewer::doUpdate()
 {
  if(updateFlags!=0)
   {
    setCursor((ushort)(curPos.x-delta.x),(ushort)(curPos.y-delta.y));
    if((updateFlags&ufView)!=0 ) drawView();
    else
     if((updateFlags&ufLine)!= 0)
       drawLines(curPos.y-delta.y,1,lineStart(curPtr) );
    if(hScrollBar!=0)
     hScrollBar->setParams((ushort)delta.x,0,(ushort)(limit.x-size.x),(ushort)(size.x/2),1);
    if(vScrollBar!=0)
     vScrollBar->setParams((ushort)delta.y,0,(ushort)(limit.y-size.y),(ushort)(size.y-1),1);
    if(indicator!=0) indicator->setValue(curPos,True);//modified);
    if((state&sfActive)!=0 )  updateCommands();
    updateFlags=0;
   }
 }

void TViewer::updateCommands()
 {
  //setCmdState( cmUndo, Boolean( delCount != 0 || insCount != 0 ) );
  //if( isClipboard() == False )
  // {
  //  setCmdState(cmCut,  hasSelection());
  //  setCmdState(cmCopy, hasSelection());
  //  setCmdState(cmPaste,Boolean(clipboard!=0 && (clipboard->hasSelection())) );
  // }
  //setCmdState(cmClear, hasSelection());
  //setCmdState(cmFind, True);
  //setCmdState(cmReplace, True);
  //setCmdState(cmSearchAgain, True);
  setCmdState(cmSave, True);  //TFileViewer
  setCmdState(cmSaveAs, True);
 }

TPalette& TViewer::getPalette() const
 {
  static TPalette palette( cpEditor, sizeof( cpEditor )-1 );
  return palette;
 }

void TViewer::draw()
 {
  if(drawLine!=delta.y)
   {
    drawPtr=lineMove(drawPtr,delta.y-drawLine);
    drawLine=delta.y;
   }
  drawLines(0,size.y,drawPtr);
 }

void TViewer::drawLines( int y, int count,ULONG linePtr )
 {
  ushort color=getColor(0x0201);
  while( count-- > 0 )
   {
    ushort *b;//[maxLineLength];
    b=new ushort[limit.x];
    formatLine(b,linePtr,delta.x+size.x, color);
    writeBuf(0,(short)y,(short)size.x,1,(TDrawBuffer *)&b[delta.x]);
    delete b;
    linePtr=nextLine(linePtr);
    y++;
   }
 }

void TViewer::changeBounds( const TRect& bounds )
 {
  setBounds(bounds);
  delta.x=max(0,min(delta.x,limit.x-size.x));
  delta.y=max(0,min(delta.y,limit.y-size.y));
  update(ufView);
 }

void TViewer::convertEvent( TEvent& event )
 {
  if(event.what==evKeyDown)
   {
    if((getShiftState()&0x03)!=0 &&
        event.keyDown.charScan.scanCode>=0x47 &&
        event.keyDown.charScan.scanCode<=0x51 )
     event.keyDown.charScan.charCode=0;

    ushort key = event.keyDown.keyCode;
    if(keyState != 0)
     {
      if( (key & 0xFF) >= 0x01 && (key&0xFF) <= 0x1A ) key += 0x40;
      if( (key & 0xFF) >= 0x61 && (key&0xFF) <= 0x7A ) key -= 0x20;
     }
    key=scanKeyMap(keyMap[keyState],key);
    keyState = 0;
    if( key != 0 )
     if( (key & 0xFF00) == 0xFF00 )
      {
       keyState = (key & 0xFF);
       clearEvent(event);
      }
     else
      {
       event.what = evCommand;
       event.message.command = key;
      }
   }
 }

void TViewer::handleEvent( TEvent& event )
 {
  TView::handleEvent(event);
  convertEvent(event);
  Boolean centerCursor = Boolean(!cursorVisible());
  uchar selectMode = 0;

  //if(selecting==True || (getShiftState() & 0x03)!=0 ) selectMode=(uchar)smExtend;

  switch(event.what)
   {
    case evMouseDown:
      if(event.mouse.doubleClick==True) selectMode|=(uchar)smDouble;
      do
       {
        //lock();
        if(event.what==evMouseAuto)
         {
        //  TPoint mouse = makeLocal( event.mouse.where );
        //  TPoint d = delta;
        //  if( mouse.x < 0 )      d.x--;
        //  if( mouse.x >= size.x) d.x++;
        //  if( mouse.y < 0 )      d.y--;
        //  if( mouse.y >= size.y) d.y++;
        //  scrollTo(d.x, d.y);
         }
        setCurPtr(getMousePtr(event.mouse.where), selectMode);
        //selectMode |=(uchar)smExtend;
        //unlock();
       } while( mouseEvent(event, evMouseMove + evMouseAuto) );
      break;
    case evKeyDown:
//      switch (event.keyDown.keyCode)
//       {
//        case kbEsc:
//         if( (flags&wfClose)!=0 &&
//             (event.message.infoPtr==0 || event.message.infoPtr==this) )
//          {
//           if( (state&sfModal)==0 ) close();
//           else
//            {
//             event.what=evCommand;
//             event.message.command=cmCancel;
//             putEvent(event);
//            }
//           clearEvent(event);
//          }
//         break;
//       }
      if(event.keyDown.charScan.charCode==9 ||
          (event.keyDown.charScan.charCode>=32 && event.keyDown.charScan.charCode<255 ) )
       {
        lock();
        //if( overwrite == True && False/*hasSelection() == False*/ )
        //    if( curPtr!=lineEnd(curPtr) ) selEnd=nextChar(curPtr);
        //insertText( &event.keyDown.charScan.charCode, 1, False);
        //trackCursor(centerCursor);
        unlock();
       }
      else  return;
      break;
    case evCommand:
      switch( event.message.command )
        {
         //case cmFind:        find(); break;
         //case cmReplace:     replace(); break;
         //case cmSearchAgain: doSearchReplace(); break;
         default:
           lock();
           switch( event.message.command )
            {
             //case cmCut:   clipCut();      break;
             //case cmCopy:  clipCopy(); // hideSelect(); // JS 12.4.94 break;
             //case cmPaste: clipPaste();    break;
             //case cmUndo:  undo();         break;
             //case cmClear: deleteSelect(); break;
             case cmCharLeft:
                 setCurPtr(prevChar(curPtr), selectMode);
                 break;
             case cmCharRight:
                 setCurPtr(nextChar(curPtr), selectMode);
                 break;
             case cmWordLeft:
                 setCurPtr(prevWord(curPtr), selectMode);
                 break;
             case cmWordRight:
                 setCurPtr(nextWord(curPtr), selectMode);
                 break;
             case cmLineStart:
                 setCurPtr(lineStart(curPtr), selectMode);
                 break;
             case cmLineEnd:
                 setCurPtr(lineEnd(curPtr), selectMode);
                 break;
             case cmLineUp:
                 setCurPtr(lineMove(curPtr,-1),selectMode);
                 break;
             case cmLineDown:
                 setCurPtr(lineMove(curPtr,1),selectMode);
                 break;
             case cmPageUp:
                 setCurPtr(lineMove(curPtr,-(size.y-1)), selectMode);
                 break;
             case cmPageDown:
                 setCurPtr(lineMove(curPtr, size.y-1), selectMode);
                 break;
             case cmTextStart:
                 setCurPtr(0, selectMode);
                 break;
             case cmTextEnd:
                 setCurPtr(bufLen, selectMode);
                 break;
             //case cmBackSpace: deleteRange(prevChar(curPtr),curPtr,True);
             //    break;
             //case cmDelChar:   deleteRange(curPtr, nextChar(curPtr),True);
             //    break;
             //case cmDelWord:   deleteRange(curPtr, nextWord(curPtr),False);
             //    break;
             //case cmDelStart:  deleteRange(lineStart(curPtr),curPtr,False);
             //    break;
             //case cmDelEnd:    deleteRange(curPtr, lineEnd(curPtr),False);
             //    break;
             //case cmDelLine:   deleteRange(lineStart(curPtr),nextLine(curPtr), False);
             //    break;
             //case cmNewLine:   newLine(); break;
             //case cmStartSelect:
             //    startSelect();
             //    break;
             //case cmHideSelect:
             //    hideSelect();
             //    break;
             case cmIndentMode:
                 autoIndent = Boolean(!autoIndent);
                 break;
             case cmInsMode:
                 toggleInsMode();
                 break;
             default:
                 unlock();
                 return;
            }
           trackCursor(centerCursor);
           unlock();
           break;
        }
    case evBroadcast:
      switch( event.message.command )
       {
        case cmScrollBarChanged:
          checkScrollBar( event, hScrollBar, delta.x );
          checkScrollBar( event, vScrollBar, delta.y );
          break;
        default:
          return;
       }
   }
  clearEvent(event);
 }

int TViewer::charPos(ULONG p,ULONG target )
 {
  int pos = 0;
  while( p < target )
   {
    if( bufChar(p) ==0x9) pos |= 7;
    pos++;
    p++;
  }
  return pos;
 }

ULONG TViewer::charPtr(ULONG p, int target )
 {
  int pos = 0;
  while( (pos < target) && (p < bufLen) && (bufChar(p)!=0x0D) )
   {
    if(bufChar(p)==0x09) pos |= 7;
    pos++;
    p++;
   }
  if( pos > target ) p--;
  return p;
 }

void TViewer::checkScrollBar( const TEvent& event,TScrollBar *p,int& d)
 {
  if( (event.message.infoPtr == p) && (p->value != d) )
   {
    d = p->value;
    update(ufView);
   }
 }

void TViewer::scrollTo( int x, int y )
 {
  x = max(0, min(x,limit.x-size.x));
  y = max(0, min(y,limit.y-size.y));
  if( x != delta.x || y != delta.y )
   {
    delta.x = x;
    delta.y = y;
    update(ufView);
   }
 }

void TViewer::trackCursor( Boolean center )
 {
  if(center==True )
    scrollTo(curPos.x-size.x+1,curPos.y-size.y / 2);
  else
    scrollTo(max(curPos.x-size.x+1,min(delta.x, curPos.x)),
             max(curPos.y-size.y+1,min(delta.y, curPos.y)));
 }

Boolean TViewer::cursorVisible()
 {
  return Boolean((curPos.y >= delta.y) && (curPos.y < delta.y + size.y));
 }

ULONG TViewer::getMousePtr( TPoint m )
 {
  TPoint mouse = makeLocal( m );
  mouse.x = max(0, min(mouse.x,size.x-1));
  mouse.y = max(0, min(mouse.y, size.y - 1));
  return charPtr(lineMove(drawPtr,mouse.y+delta.y-drawLine),mouse.x+delta.x);
 }

// These Routines are taken from Rogue Wave Tools++
//ULONG scan( const void *block,ULONG size, const char *str )
// {
//  const long   q      =33554393L;
//  const long   q32    =q<<5;

//  int   testLength    =size;
//  int   patternLength =strlen(str);
//  if( testLength < patternLength || patternLength == 0 ) return UINT_MAX;

//  long  patternHash   =0;
//  long  testHash      =0;

//  register const char* testP= (const char*)block;
//  register const char* patP = str;
//  register long   x = 1;
//  int             i = patternLength-1;
//  while(i--) x =  (x<<5)%q;

//  for (i=0; i<patternLength; i++)
//   {
//    patternHash = ( (patternHash<<5) + *patP++  ) % q;
//    testHash    = ( (testHash   <<5) + *testP++ ) % q;
//   }

//  testP = (const char*)block;
//  const char* end = testP + testLength - patternLength;

//  while (1)
//   {
//    if(testHash == patternHash) return testP-(const char*)block;
//
//    if (testP >= end) break;

    // Advance & calculate the new hash value:
//    testHash = ( testHash + q32 - *testP * x                 ) % q;
//    testHash = ( (testHash<<5)  + *(patternLength + testP++) ) % q;
//   }
//  return UINT_MAX;              // Not found.
// };

//ULONG iScan( const void *block,ULONG size, const char *str )
// {
//  const long   q        = 33554393L;
//  const long   q32      = q<<5;

//  int   testLength      = size;
//  int   patternLength   = strlen(str);
//  if( testLength < patternLength || patternLength == 0 ) return UINT_MAX;

//  long  patternHash     = 0;
//  long  testHash        = 0;

//  register const char*  testP= (const char*)block;
//  register const char*  patP = str;
//  register long   x = 1;
//  int             i = patternLength-1;
//  while(i--) x =  (x<<5)%q;

//  for (i=0; i<patternLength; i++)
//   {
//    patternHash = ( (patternHash<<5) + toupper(*patP++)  ) % q;
//    testHash    = ( (testHash   <<5) + toupper(*testP++) ) % q;
//   }

//  testP = (const char*)block;
//  const char* end = testP + testLength - patternLength;

//  while (1)
//   {
//     if(testHash == patternHash)
//         return testP-(const char*)block;

//     if (testP >= end) break;

     // Advance & calculate the new hash value:
//     testHash = ( testHash + q32 - toupper(*testP) * x                 ) % q;
//     testHash = ( (testHash<<5)  + toupper(*(patternLength + testP++)) ) % q;
//   }
//  return UINT_MAX;              // Not found.
// };

void TViewer::setCmdState( ushort command, Boolean enable )
 {
  TCommandSet s;
  s += command;
  if( enable == True && (state & sfActive) != 0 )
      enableCommands(s);
  else
      disableCommands(s);
 }

void TViewer::setCurPtr(ULONG p,uchar selectMode )
 {
  ULONG anchor;
  if( (selectMode & smExtend)==0 ) anchor = p;
  //else
  // if( curPtr == selStart )
  //    anchor = selEnd;
  //else
  //    anchor = selStart;

  if(p<anchor)
   {
    if( (selectMode & smDouble)!=0)
     {
      p=prevLine(nextLine(p));
      anchor=nextLine(prevLine(anchor));
     }
    setSelect(p,anchor,True);
   }
  else
   {
    if( (selectMode & smDouble) != 0 )
     {
      p = nextLine(p);
      anchor = prevLine(nextLine(anchor));
     }
    setSelect(anchor,p,False);
   }
 }

void TViewer::setState( ushort aState, Boolean enable )
 {
  TView::setState(aState, enable);
  switch( aState )
   {
    case sfActive:
      if(hScrollBar!=0) hScrollBar->setState(sfVisible, enable);
      if(vScrollBar!=0) vScrollBar->setState(sfVisible, enable);
      if(indicator !=0) indicator->setState(sfVisible, enable);
      updateCommands();
      break;
    case sfExposed:
      if(enable==True) unlock();
   }
 }

void TViewer::toggleInsMode()
 {
  overwrite = Boolean(!overwrite);
  setState(sfCursorIns, Boolean(!getState(sfCursorIns)));
 }

Boolean TViewer::valid(ushort command)
 {
  if(command==cmValid) return isValid;  //TFileViewer
  else
   {
    //if(modified==True)
    // {
    //  int d;
    //  if(*fileName==EOS) d=edSaveUntitled;
    //  else               d=edSaveModify;

    //  switch(editorDialog(d,fileName) )
    //   {
    //    case cmYes: //save();
    //      return True;
    //    case cmNo:
    //      modified=False;
    //      return True;
    //    case cmCancel:
    //      return False;
    //    }
    //  }
    }
  return True;
  //return isValid; //old
 }

char TViewer::bufChar(ULONG p)
 {
  return frame[p];//+((p>=curPtr) ? gapLen : 0)];
 }
/*
ULONG TViewer::bufPtr(ULONG p)
 {
  return p;//(p>=curPtr) ? p+gapLen : p;
 }
*/
ULONG TViewer::lineStart(ULONG p)
 {
  while(p>curPtr)
//   if(frame[--p + gapLen]==0x0D)
   if(frame[--p]==0x0D)
//    if(p+1 == bufLen || frame[p+gapLen+1]!=0x0A) return p + 1;
    if(p+1==bufLen || frame[p+1]!=0x0A) return p+1;
    else                                return p+2;

  if(curPtr == 0) return 0;

  while(p>0)
   if(frame[--p]==0x0D)
    if(p+1 ==bufLen || p+1 ==curPtr || frame[p+1]!=0x0A) return p+1;
    else                                                 return p+2;
  return 0;
 }

ULONG TViewer::lineEnd(ULONG p)
 {
//  ULONG pStart=lineStart(p);

  if(p<curPtr)
   {
    while(p<curPtr)//&&(p<(pStart+256)))
     if(frame[p]==0x0D) return p;
     else               p++;
    if(curPtr==bufLen)  return bufLen;
   }
  else
    if(p==bufLen) return bufLen;

//  while((p+gapLen)<bufSize)
  while((p)<bufSize)//&&(p<(pStart+256)))
//    if(frame[p+gapLen]==0x0D) return p;
    if(frame[p]==0x0D) return p; //||((curPtr-p)>)
    else p++;

  return p;
 }

void TViewer::formatLine( void *DrawBuf,ULONG LinePtr,
                          int Width, ushort Color )
 {
  register int i = 0;       // index in the DrawBuf
  register ULONG p = LinePtr; // index in the Buffer
  auto ushort curColor;
  while((p<curPtr) && (frame[p]!=0x0D) && (i<=Width))
   {
    curColor=(ushort)(/* (p>=selStart && p<selEnd) ? (Color & 0xFF00) :*/ ((Color & 0xFF) << 8) );
    if(frame[p] == 0x9)
     {
      do
       {
        ((ushort *) DrawBuf) [i]=(ushort)(' ' + curColor);
        i++;
       } while ((i % 8) && (i <= Width));
      p++;
     }
    else
     {
      ((ushort *) DrawBuf)[i]=(ushort)(frame[p]+curColor);
      p++; i++;
     }
   }

  if(p>=curPtr)
   {
//    p += gapLen;

    while((p<bufSize) && (frame[p]!=0x0D) && (i <= Width))
     {
      curColor=(ushort)( /*((p-gapLen)>=selStart && (p-gapLen)<selEnd)
                    ? (Color & 0xFF00) :*/ ((Color & 0xFF) << 8) );
      if(frame[p] == 0x9)
       {
        do
         {
          ((ushort *) DrawBuf) [i]=(ushort)(' ' + curColor);
          i++;
         } while ((i % 8) && (i <= Width));
        p++;
       }
      else
       {
        ((ushort *) DrawBuf) [i] =(ushort)(frame[p]+curColor);
        p++; i++;
       };
     };
//    p -= gapLen;
   }

  curColor =(ushort)( /*(p>=selStart && p<selEnd) ? (Color & 0xFF00) : */((Color & 0xFF) << 8) );
  while (i <= Width)
   {
    ((ushort *) DrawBuf) [i] =(ushort)(' '+curColor);
    i++;
   }
 }

ULONG TViewer::lineMove(ULONG p, int count)
 {
  ULONG i=p;
  p=lineStart(p);
  int pos=charPos(p,i);
  while(count!=0)
   {
    i=p;
    if(count<0)
     {
      p=prevLine(p);
      count++;
     }
    else
     {
      p=nextLine(p);
      count--;
     }
   }
  if(p!=i) p=charPtr(p,pos);
  return p;
 }

//void TViewer::newLine()
// {
//  const char crlf[] = "\x0D\x0A";
//  ULONG p = lineStart(curPtr);
//  ULONG i = p;
//  while( i < curPtr && ( (frame[i] == ' ') || (frame[i] =='0x9')) )
//       i++;
//  insertText(crlf, 2, False);
//  if( autoIndent == True )
//      insertText( &frame[p], i - p, False);
// }

ULONG TViewer::nextLine(ULONG p )
 {
  return nextChar(lineEnd(p));
 }

ULONG TViewer::prevLine(ULONG p )
 {
  return lineStart(prevChar(p));
 }

ULONG TViewer::nextWord(ULONG p )
 {
  if(isWordChar(bufChar(p)) )
     while( p<bufLen && isWordChar(bufChar(p)) ) p=nextChar(p);
  else if ( p < bufLen ) p = nextChar(p);

  while( p < bufLen && !isWordChar(bufChar(p)) ) p=nextChar(p);
  return p;
 }

ULONG TViewer::prevWord(ULONG p )
 {
  while(p>0 && isWordChar(bufChar(prevChar(p)))==0) p = prevChar(p);
  while(p>0 && isWordChar(bufChar(prevChar(p)))!=0) p = prevChar(p);
  return p;
 }

ULONG TViewer::nextChar(ULONG p )
 {
  if(p  ==bufLen) return p;
  if(++p==bufLen) return p;

  int t=p;// (p >= curPtr) ? p + gapLen : p;

  return ((frame[t-1]==0x0D) && (frame[t]==0x0A)) ? p + 1 : p;
 }

ULONG TViewer::prevChar(ULONG p)
 {
  if(p  ==0) return p;
  if(--p==0) return p;

  int t=p;// (p>=curPtr) ? p+gapLen : p;

  return ((frame[t-1]==0x0D) && (frame[t]==0x0A)) ? p-1 : p;
 }

TEditorDialog near TViewer::editorDialog = defEditorDialog;
//ushort near TViewer::editorFlags = efBackupFiles|efPromptOnReplace;

//Boolean TViewer::hasSelection()
// {
//  return Boolean(selStart != selEnd);
// }

//void TViewer::startSelect()
// {
//  hideSelect();
//  selecting = True;
// }

void TViewer::setSelect(ULONG newStart,ULONG newEnd, Boolean curStart )
 {
  ULONG p;
  if( curStart!=0 ) p=newStart;
  else              p=newEnd;

  uchar flags=ufUpdate;

//  if( newStart != selStart || newEnd != selEnd )
//      if( newStart != newEnd || selStart != selEnd ) flags = ufView;

  if(p!=curPtr)
   {
    if(p>curPtr)
     {
      ULONG l = p - curPtr;
//      memmove( &frame[curPtr], &frame[curPtr + gapLen], l);
      memmove( &frame[curPtr], &frame[curPtr], l);
      curPos.y += countLines(&frame[curPtr], l);
      curPtr = p;
     }
    else
     {
      ULONG l = curPtr - p;
      curPtr = p;
      curPos.y -=countLines(&frame[curPtr], l);
//      memmove( &frame[curPtr + gapLen], &frame[curPtr], l);
      memmove( &frame[curPtr], &frame[curPtr], l);
     }
    drawLine=curPos.y;
    drawPtr =lineStart(p);
    curPos.x=charPos(drawPtr, p);
    //delCount = 0;
    //insCount = 0;
    getBufSize(bufLen);
   }
//  selStart = newStart;
//  selEnd = newEnd;
  update(flags);
 }

//void TViewer::hideSelect()
// {
//  selecting = False;
//  setSelect(curPtr, curPtr, False);
// }

//TViewer * near TViewer::clipboard = 0;

//Boolean TViewer::isClipboard()
// {
//  return Boolean(clipboard==this);
// }

//Boolean TViewer::clipCopy()
// {
//  Boolean res = False;
//  if( (clipboard != 0) && (clipboard != this) )
//   {
//    res = clipboard->insertFrom(this);
//    selecting = False;
//    update(ufUpdate);
//   }
//  return res;
// }

//void TViewer::clipCut()
// {
//  if( clipCopy() == True ) deleteSelect();
// }

//void TViewer::clipPaste()
// {
//  if( (clipboard != 0) && (clipboard != this) )
//      insertFrom(clipboard);
// }

//void TViewer::undo()
// {
//  if( delCount != 0 || insCount != 0 )
//   {
//    selStart = curPtr - insCount;
//    selEnd = curPtr;
//    ULONG length = delCount;
//    delCount = 0;
//    insCount = 0;
//    insertBuffer(frame, curPtr + gapLen - length, length, False, True);
//   }
// }

//char near TViewer::findStr[maxFindStrLen] ="";
//char near TViewer::replaceStr[maxReplaceStrLen]="";
//
//void TViewer::find()
// {
//  TFindDialogRec findRec( findStr, editorFlags );
//  if( editorDialog( edFind, &findRec ) != cmCancel )
//   {
//    strcpy( findStr, findRec.find );
//    editorFlags=(ushort)(findRec.options & ~efDoReplace);
//    doSearchReplace();
//   }
// }

//Boolean TViewer::search( const char *findStr, ushort opts )
// {
//  ULONG pos = curPtr;
//  ULONG i;
//  do
//   {
//    if( (opts & efCaseSensitive) != 0 )
//        i = scan( &frame[bufPtr(pos)], bufLen - pos, findStr);
//    else
//        i = iScan( &frame[bufPtr(pos)], bufLen - pos, findStr);
//
//    if( i != sfSearchFailed )
//     {
//      i += pos;
//      if( (opts & efWholeWordsOnly) == 0 ||
//        !(
//          ( i != 0 && isWordChar(bufChar(i - 1)) != 0 ) ||
//          ( i+strlen(findStr) != bufLen && isWordChar(bufChar(i+strlen(findStr))) )
//         ))
//       {
//        lock();
//        setSelect(i, i + strlen(findStr), False);
//        trackCursor(Boolean(!cursorVisible()));
//        unlock();
//        return True;
//       }
//      else
//          pos = i + 1;
//     }
//   } while( i != sfSearchFailed );
//  return False;
// }

//void TViewer::replace()
// {
//  TReplaceDialogRec replaceRec( findStr, replaceStr, editorFlags );
//  if( editorDialog( edReplace, &replaceRec ) != cmCancel )
//   {
//    strcpy( findStr, replaceRec.find );
//    strcpy( replaceStr, replaceRec.replace );
//    editorFlags=(ushort)(replaceRec.options|efDoReplace);
//    doSearchReplace();
//   }
// }

//void TViewer::doSearchReplace()
// {
//  int i;
//  do
//   {
//    i = cmCancel;
//    if( search(findStr, editorFlags) == False )
//     {
//      if( (editorFlags & (efReplaceAll | efDoReplace)) !=
//          (efReplaceAll | efDoReplace) )
//              editorDialog(edSearchFailed );
//     }
//    else
//     if( (editorFlags & efDoReplace) != 0 )
//      {
//       i = cmYes;
//       if( (editorFlags & efPromptOnReplace) != 0 )
//           {
//           TPoint c = makeGlobal( cursor );
//           i = editorDialog( edReplacePrompt, &c );
//           }
//       if( i == cmYes )
//           {
//           lock();
//           insertText( replaceStr, strlen(replaceStr), False);
//           trackCursor(False);
//           unlock();
//           }
//      }
//   } while( i != cmCancel && (editorFlags & efReplaceAll) != 0 );
// }

//void TViewer::deleteRange(ULONG startPtr,ULONG endPtr,Boolean delSelect)
// {
//  if( hasSelection() == True && delSelect == True )
//    deleteSelect();
//  else
//   {
//    setSelect(curPtr, endPtr, True);
//    deleteSelect();
//    setSelect(startPtr, curPtr, False);
//    deleteSelect();
//   }
// }

//void TViewer::deleteSelect()
// {
//  insertText( 0, 0, False );
// }

void TViewer::lock()
 {
  lockCount++;
 }

void TViewer::unlock()
 {
  if(lockCount>0)
   {
    lockCount--;
    if(lockCount==0) doUpdate();
   }
 }

Boolean TViewer::loadFromFile()
 {
  if(hFile==-1) return False;
  //long fSize=filelength(hFile);// f.rdbuf()->fd() );
  //if(fSize>(UINT_MAX-0x1F) || getBufSize(ushort(fSize))==False)
  // {
  //  editorDialog(edOutOfMemory);
  //  return False;
  // }
  //else
  //if(fSize>bufSize)//INT_MAX )
   {
    read(hFile,frame,bufSize);
    //read(hFile,&frame[bufSize-(ULONG)fSize],INT_MAX );
    //read(hFile,&frame[bufSize-(ULONG)fSize+INT_MAX],(ULONG)(fSize-INT_MAX) );
    for(int i=0;i<bufSize;i++)
      if(frame[i]<0x20) if((frame[i]!=0x0a)&&(frame[i]!=0x0d)) frame[i]='.';
    setBufLen((ULONG)bufSize);
   }
//  else
   {
 //    read(hFile,&frame[bufSize-(ULONG)fSize],(ULONG)fSize);
//    read(hFile,frame,(ULONG)fSize);
//    for(int i=0;i<fSize;i++)
 //      if(frame[bufSize-fSize+i]<0x20)
 //       if((frame[bufSize-fSize+i]!=0x0a)&&(frame[bufSize-fSize+i]!=0x0d))
 //        frame[bufSize-fSize+i]='.';
//      if(frame[i]<0x20) if((frame[i]!=0x0a)&&(frame[i]!=0x0d)) frame[i]='.';
//    setBufLen((ULONG)fSize);
   }
  return True;
 }

//---------------
const TPoint minViewWinSize={24, 6};

TViewWindow::TViewWindow(const TRect& bounds,const char *fileName,int aNumber) :
    TWindow(bounds,0,(ushort)aNumber),TWindowInit(TViewWindow::initFrame)
 {
  state|=sfExposed;
  options|=ofTileable|ofSelectable|ofFirstClick; //|ofTopSelect

  TScrollBar *hScrollBar=new TScrollBar(TRect(18,size.y-1,size.x-2,size.y ) );
  hScrollBar->hide();
  insert(hScrollBar);

  TScrollBar *vScrollBar=new TScrollBar(TRect(size.x-1,1,size.x,size.y-1));
  vScrollBar->hide();
  insert(vScrollBar);

  TIndicator *indicator=new TIndicator(TRect(2,size.y-1,16,size.y) );
  indicator->hide();
  insert(indicator);

  TRect r( getExtent() );
  r.grow(-1,-1);
  editor=new TViewer(r,hScrollBar,vScrollBar,indicator,32768,fileName);
  insert(editor);
 }

void TViewWindow::close()
 {
  //if( editor->isClipboard() == True ) hide();
  //else
  TWindow::close();
 }
//#include <stdio.h>
void TViewWindow::handleEvent(TEvent& event)
 {
  TWindow::handleEvent(event);
  switch (event.what)
   {
    case evMouseDown:
      makeFirst();
      //setCurrent(this,normalSelect);
      do
       {
       } while(mouseEvent(event,evMouseMove+evMouseAuto) );
      clearEvent(event);
      break;
    case evKeyDown:
      switch (event.keyDown.keyCode)
       {
        case kbEsc:
         if( (flags&wfClose)!=0 /*&&
             (event.message.infoPtr==0 || event.message.infoPtr==this)*/ )
          {
           if( (state&sfModal)==0 ) close();
           else
            {
             event.what=evCommand;
             event.message.command=cmCancel;
             putEvent(event);
            }
           clearEvent(event);
          }
         break;
       }
      break;
    case evBroadcast:
      if(event.message.command==cmUpdateTitle )
       {
        //if(frame != 0 ) frame->drawView();
        clearEvent(event);
       }
      break;
   }
 }

void TViewWindow::sizeLimits(TPoint& min,TPoint& max)
 {
  TWindow::sizeLimits(min, max);
  min=minViewWinSize;
 }

//const char * near TViewWindow::clipboardTitle = "Clipboard";
//const char * near TViewWindow::untitled = "Untitled";

