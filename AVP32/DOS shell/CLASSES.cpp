#include <os2.h>
#include "tv32def.h"

#define Uses_TIndicator
#define Uses_TListViewer
#define Uses_TListBox
#define Uses_TIndicator
#define Uses_TDirCollection
#define Uses_THistory
#define Uses_TButton
#define Uses_TFileInfoPane
#define Uses_TFileInputLine
#define Uses_TFileList
#define Uses_TFileDialog
#define Uses_TVMemMgr
#define Uses_TLabel
#define Uses_TStaticText
#define Uses_TScrollBar
#define Uses_TKeys
#define Uses_TEvent
#define Uses_TGroup
#include "include\tv.h"
#define Uses_MsgBox
#include "include\msgbox.h"

#include <Errno.h>
#include <Assert.h>
#include <limits.h>
#include <IO.h>
#include <Stdio.h>
#include <ctype.h>
#include <String.h>
#include <Dos.h>

#include "..\\typedef.h"
#include "avp_os2.h"
#include "..\\locale\\resource.h"
#define __TBuffer
#define __TListViews
#define __TCheckBox
#define __TNotebook
#define __TPanel
#define __TFile
#define __TViewer
#define __TDirListBoxs
#define __TRadioBattons
#define __TListViews
#include "consts.h"
#include "classes.h"
#include "contain.h"
#include "memalloc.h"
#include "disk.h"
BOOL TestPresentDisk(UCHAR disk,char *typePath);

// ------------ start Notebook -------------------------------
TNotebook::TNotebook(const TRect& bounds): TGroup(bounds),value(0),sel(0)
 {
  state|=sfExposed|sfVisible;  //state &=~(sfExposed);
  options|=ofSelectable|ofBuffered|ofFirstClick;//|ofTopSelect|ofPreProcess|ofPostProcess;
  growMode=0;// gfGrowHiX | gfGrowHiY;
  clip = getExtent();
  eventMask = 0xFFFF;

  topPage=0;
  usTabX=16; //size.x
  usTabY=2;  //size.y
  buffer=0;

  //short i = 0;
  //TSItem *p;
  //for( p = aStrings; p != 0; p = p->next ) i++;

  //strings = new TStringCollection( i, 0 );

  //while( aStrings != 0 )
  // {
  //  p = aStrings;
  //  strings->atInsert( strings->getCount(), newStr(aStrings->value) );
  //  aStrings = aStrings->next;
  //  delete p;
  // }

  //setCursor(2,0);
  //showCursor();
 }

TNotebook::~TNotebook()
 {
  //destroy( (TCollection *)strings );
 }

void TNotebook::insertPage(TView* p,char* Name)
 {
  insert(p); //TGroup::

  if(topPage<MAXPAGE)
   {
    if(strlen(Name)<MAXPAGENAMELEN)
     strcpy(NotPages[topPage].Name,Name);
    else
     {
      strncpy(NotPages[topPage].Name,Name,MAXPAGENAMELEN);
      NotPages[topPage].Name[MAXPAGENAMELEN]=0;
     }
    NotPages[topPage].Page=p;
    topPage++;

    ((TGroup*)(NotPages[sel].Page))->select();
   }
 }

void TNotebook::setState(ushort aState,Boolean enable)
 {
//  TGroup::setState(aState,enable);
  TView::setState(aState,enable);
  if( (aState&sfSelected) != 0 ) setState(sfActive,enable);
//  if(aState==sfSelected) draw(); //View
 }

void TNotebook::draw()
 {
  TBuffer BufClass;

  if(buffer==0) getBuffer();
  if(buffer!= 0)
   {
    lockFlag++;
    BufClass.movChar(0,' ',getColor(0x01),(ushort)(size.x*size.y),buffer); //заполнение всей строчки

    for(TView* p=first();p!=0;p=p->nextView())
     {
      //if(p->exposed())
      p->draw();
     }
    lockFlag--;
    BufClass.wrtBuf(0, 0,(short)size.x,(short)size.y,buffer,this);
   }
 }

int TNotebook::findSel(TPoint p)
 {
  TRect r = getExtent();
  if( !r.contains(p) ) return -1;
  else
   {
    if(p.y>(usTabY-1)) return -1;
    int s = p.x / usTabX;
    if(s>=topPage) return -1;
    else           return s;
   }
 }
/*
void TView::putInFrontOf( TView *Target )
 {
  TView *p, *lastView;

  if( owner != 0 && Target != this && Target != nextView() &&
       ( Target==0 || Target->owner==owner) )
   if( (state & sfVisible) == 0 )
    {
     owner->removeView(this);
     owner->insertView(this, Target);
    }
   else
    {
     lastView = nextView();
     p = Target;
     while( p != 0 && p != this ) p = p->nextView();
     if( p == 0 ) lastView = Target;
     state &= ~sfVisible;
//     if( lastView == Target ) drawHide(lastView);
     owner->removeView(this);
     owner->insertView(this, Target);
     state|=sfVisible;
//     if( lastView != Target ) drawShow(lastView);
//     if( (options & ofSelectable) != 0 ) owner->resetCurrent();
    }
 }
*/
void TNotebook::handleEvent(TEvent& event)
 {
  TGroup::handleEvent(event);
  if( !(options & ofSelectable) ) return;
  if(event.what==evMouseDown)
   {
    TPoint mouse=makeLocal(event.mouse.where);
    int i=findSel(mouse);
    if(i!= -1) sel=i;
    makeFirst();
//    setCurrent(this,normalSelect);
    ((TGroup*)(NotPages[sel].Page))->makeFirst();//select();
//    ((TGroup*)(NotPages[sel].Page))->putInFrontOf(first());
    setCurrent((TGroup*)(NotPages[sel].Page),normalSelect);
//    movedTo(sel);
    do
     {
      mouse=makeLocal(event.mouse.where );
      if(findSel(mouse)==sel) showCursor();
      else                    hideCursor();
     } while(mouseEvent(event,evMouseMove) );
    showCursor();
    mouse=makeLocal( event.mouse.where );
    if(findSel(mouse)==sel)
     {
      press(sel);
      drawView();
     }
    clearEvent(event);
   }
  else
   if( event.what == evKeyDown)
    switch (ctrlToArrow(event.keyDown.keyCode))
     {
      case kbUp:
      case kbLeft:
        if((state & sfFocused)!=0)
         {
          if(--sel<0) sel=topPage-1;
          //if( (options & ofTopSelect) != 0 )
          ((TGroup*)(NotPages[sel].Page))->makeFirst();//select();
          //((TGroup*)(NotPages[sel].Page))->putInFrontOf(first());
          setCurrent((TGroup*)(NotPages[sel].Page),normalSelect);
          movedTo(sel);
          clearEvent(event);
         }
        break;
      case kbDown:
      case kbRight:
        if((state & sfFocused)!=0)
         {
          if( ++sel >=topPage) sel = 0;
          ((TGroup*)(NotPages[sel].Page))->makeFirst();//select();
          //((TGroup*)(NotPages[sel].Page))->putInFrontOf(first());
          setCurrent((TGroup*)(NotPages[sel].Page),normalSelect);
          movedTo(sel);
          clearEvent(event);
         }
        break;
      default:
        //for(int i=0; i<strings->getCount();i++)
        // {
        //  char c=hotKey( (char *)(strings->at(i)) );
        //  if(getAltCode(c)==event.keyDown.keyCode ||
        //     ( ( owner->phase==phPostProcess ||(state&sfFocused) != 0) &&
        //        c!=0 && toupper(event.keyDown.charScan.charCode)==c ) )
        //   {
        //    select();
        //    sel=i;
        //    movedTo(sel);
        //    press(sel);
        //    draw();//View();
        //    clearEvent(event);
        //    return;
        //   }
        // }
        if(event.keyDown.charScan.charCode==' ' && (state & sfFocused) != 0 )
         {
          press(sel);
          draw();//View();
          clearEvent(event);
         }
     }
 }

ushort TNotebook::getHelpCtx()
 {
  if(helpCtx == hcNoContext) return (ushort)hcNoContext;
  else                       return (ushort)(helpCtx + sel);
 }

size_t TNotebook::dataSize() { return sizeof(value); }

void TNotebook::getData(void *rec)
 {
  *(ushort*)rec = value;
  //draw();//View();
 }

void TNotebook::setData(void *rec)
 {
  value=*(ushort *)rec; //tcluster
  //draw();//View();

  sel = value;             //tradiobutton
 }

Boolean TNotebook::mark(int item)  //{ return False; }//tcluster
 {
  return Boolean(item==value);                     //tradiobutton
 }

void TNotebook::press(int item)  // {}              //tcluster
 {
  value=(ushort)item;                                        //tradiobutton
 }

void TNotebook::movedTo(int item) //{}              //tcluster
 {
  value=(ushort)item;                                        //tradiobutton
 }
/*
int TNotebook::column( int item )
 {
  if(item<size.y) return 0;
  else
   {
    int width = 0;
    int col = -6;
    int l = 0;
    for( int i = 0; i <= item; i++ )
     {
      if( i % size.y == 0 )
       {
        col += width + 6;
        width = 0;
       }

//      if( i < strings->getCount() ) l = cstrlen( (char *)(strings->at(i)) );
      if( l > width ) width = l;
      }
    return col;
   }
 }

int TNotebook::row( int item )
 {
  return item % size.y;
 }
*/
// ------------ end Notebook -------------------------------

const TPoint minWinSize = {16, 6};

//#define cpFrame "\x01\x01\x02\x02\x03"

//const char near TPanel::initFrame[19] =
//  "\x06\x0A\x0C\x05\x00\x05\x03\x0A\x09\x16\x1A\x1C\x15\x00\x15\x13\x1A\x19";

const char * near TPanel::shadows= "\xDC\xDB\xdb";//F";
char near TPanel::frameChars[33] =
    "   ╚ ║╔╟ ╝═╧╗╢╤    └ │┌├ ┘─┴┐┤┬┼"; // for UnitedStates code page

TPanel::TPanel(const TRect& bounds,const char *aTitle,short aNumber) :
    TGroup(bounds),
    flags( wfMove | wfGrow | wfClose | wfZoom ),
    number(aNumber)//,TWindowInit( TWindow::initFrame )
 {
  state|=sfShadow|sfVisible|sfExposed; //state &=~(sfExposed);
  options|=ofSelectable|ofBuffered|ofFirstClick;//|ofTopSelect;
  growMode=0;//gfGrowAll|gfGrowRel;
  //  eventMask |= evBroadcast;
  ushort tabX=16;//((TNotebook*)owner)->usTabX
  ushort tabPos=(ushort)(tabX*aNumber);

  PanelTitle=new TPanelTitle(TRect(tabPos+2,1,tabPos+tabX-3,2),aTitle);
  PanelTitle->options |=ofSelectable;
  insert(PanelTitle);
 }

TPanel::~TPanel()
 {
  //delete PanelTitle;
 }

void TPanel::close()
 {
  if(valid( cmClose ))
   {
    //frame = 0;  // so we don't try to use the frame after it's been deleted
    destroy( this );
   }
 }

void TPanel::shutDown()
 {
  //frame = 0;
  TGroup::shutDown();
 }

#define cpGrayPanel "\x20\x02\x22\x23\x24\x25\x02\x02\x28\x29\x2A\x2B\x2C\x2D\x2E\x2F"  \
                    "\x30\x31\x32\x33\x34\x35\x36\x37\x38\x39\x3e\x3B\x3C\x3D\x3E\x3F"
TPalette& TPanel::getPalette() const
 {
  static TPalette palettes(cpGrayPanel,sizeof(cpGrayPanel)-1);
  return palettes;
 }

struct setBlock
 {
  ushort st;
  Boolean en;
 };

static void doSetState( TView *p, void *b )
 {
  p->setState( ((setBlock *)b)->st, ((setBlock *)b)->en );
 }

void TPanel::setState(ushort aState,Boolean enable )
 {
//  TGroup::setState(aState, enable);
  setBlock sb;
  sb.st = aState;
  sb.en = enable;

  if( enable == True ) state |= aState;
  else                 state &= ~aState;

  if( (aState & (sfActive | sfDragging)) != 0 )
   {
    lock();
    forEach(doSetState,&sb);
    unlock();
   }

  if( (aState&sfSelected) !=0 ) setState(sfActive,enable);
 }

void TPanel::draw()
 {
  TBuffer BufClass;
  if(((TNotebook*)owner)->buffer==0) return;
  ushort* b=((TNotebook*)owner)->buffer;

  clip=getExtent();//getClipRect();

  ushort cShadow; //цвета
  ushort cButton=getColor(0x0703); //0x711a
  Boolean down;
  char   ch;
  ushort tabPos=(ushort)(((TNotebook*)owner)->usTabX*number);

  if(((TNotebook*)owner)->sel==number) down=False; else down=True;
  cButton=getColor(0x0501); //0x711a
  if( (state & sfActive) != 0 )
    if( (state & sfSelected) != 0 ) cButton=getColor(0x0703);//0x727d;
//    else
//      if( amDefault )            cButton = getColor(0x0602);

  cShadow=getColor(0x07);     //0x71

  for(int y=0;y<((TNotebook*)owner)->usTabY;y++)
   {                 //1a    //количество
    //((TNotebook*)owner)->
    BufClass.movChar((ushort)(tabPos+y*size.x),' ',cButton,(ushort)((TNotebook*)owner)->usTabX,b); //заполнение всей строчки
    BufClass.putsAttr((ushort)(tabPos+y*size.x),cShadow,b); //0            //меняем фон у первого символа
    if(down)         //0x71
     {
      if(y==0)
       {
        BufClass.putsChar((ushort)(tabPos+y*size.x+2),frameChars[down*16+6],b);
        BufClass.movChar((ushort)(tabPos+y*size.x+3),frameChars[down*16+10],cButton,(ushort)(((TNotebook*)owner)->usTabX-4),b);
        BufClass.putsChar((ushort)(tabPos+y*size.x+((TNotebook*)owner)->usTabX-1),frameChars[down*16+12],b);
       }
      else
       {
        BufClass.putsChar((ushort)(tabPos+y*size.x+2),frameChars[down*16+5],b);
        BufClass.putsChar((ushort)(tabPos+y*size.x+((TNotebook*)owner)->usTabX-1),frameChars[down*16+5],b);
       }
      BufClass.putsAttr((ushort)(tabPos+y*size.x+1),cShadow,b);  //1         //если нажата второй символ фон
      ch=' ';
     }
    else
     {
      BufClass.putsAttr((ushort)(tabPos+y*size.x+((TNotebook*)owner)->usTabX-1),cShadow,b); //фон в последний символ(0-(usTabX-1))
      if(y==0)
       {
        BufClass.putsChar((ushort)(tabPos+y*size.x+1),frameChars[down*16+6],b);
        BufClass.putsChar((ushort)(tabPos+y*size.x+((TNotebook*)owner)->usTabX-2),frameChars[down*16+12],b);
        BufClass.movChar((ushort)(tabPos+y*size.x+2),frameChars[down*16+10],cButton,(ushort)(((TNotebook*)owner)->usTabX-4),b);
        BufClass.putsChar((ushort)(tabPos+y*size.x+((TNotebook*)owner)->usTabX-1),shadows[0],b);
       }
      else
       {
        BufClass.putsChar((ushort)(tabPos+y*size.x+1),frameChars[down*16+5],b);
        BufClass.putsChar((ushort)(tabPos+y*size.x+((TNotebook*)owner)->usTabX-2),frameChars[down*16+5],b);
        BufClass.putsChar((ushort)(tabPos+y*size.x+((TNotebook*)owner)->usTabX-1),shadows[1],b);
       }
      ch=shadows[2];
     }
//    ((TNotebook*)owner)->wrtLine((ushort)tabPos,(ushort)y,(ushort)((TNotebook*)owner)->usTabX,1,b);
   }

//  char *title=(char *)(strings->at(cur));
//  if(title!=0)
//   {
//    int l;//,scOff;
    //if( (flags & bfLeftJust) != 0 ) l = 1;
    //else
    // {
//      l=((((TNotebook*)owner)->usTabX-1)-1-cstrlen(title) )/2;
//      if(l<1) l=1;
    // }
//    ushort firstChOfBut=(ushort)(size.x+((TNotebook*)owner)->usTabX*number);
//    if(down) firstChOfBut+=2;
//    else     firstChOfBut++;

//    BufClass.movCStr((ushort)(firstChOfBut+l),title,cButton,b);
//   }

  if(down==False)         //0x71
   {
    BufClass.movChar((ushort)(2*size.x),frameChars[10],cButton,(ushort)size.x,b); //заполнение всей строчки
    BufClass.putsChar((ushort)(2*size.x+tabPos+1),frameChars[9],b);
    BufClass.putsChar((ushort)(2*size.x+tabPos+((TNotebook*)owner)->usTabX-2),frameChars[3],b);
    BufClass.movChar((ushort)(2*size.x+tabPos+2),' ',cButton,(ushort)(((TNotebook*)owner)->usTabX-4),b);
    BufClass.putsChar((ushort)(2*size.x),frameChars[6],b);
    BufClass.putsChar((ushort)(2*size.x+size.x-2),frameChars[12],b);
    BufClass.putsAttr((ushort)(2*size.x+size.x-1),cShadow,b); //фон в последний символ(0-(usTabX-1))
    BufClass.putsChar((ushort)(2*size.x+size.x-1),shadows[0],b);
//    ((TNotebook*)owner)->wrtLine(0,2,(ushort)size.x,1,b);

    for(int yp=3;yp<size.y-2;yp++)
     {
      BufClass.movChar((ushort)(yp*size.x),' ',cButton,(ushort)size.x,b); //заполнение всей строчки
      BufClass.putsAttr((ushort)(yp*size.x+size.x-1),cShadow,b); //фон в последний символ(0-(usTabX-1))
      BufClass.putsChar((ushort)(yp*size.x),frameChars[5],b);
      BufClass.putsChar((ushort)(yp*size.x+size.x-2),frameChars[5],b);
      BufClass.putsChar((ushort)(yp*size.x+size.x-1),shadows[1],b);
     }
//    for(int yp=3;yp<size.y-2;yp++) ((TNotebook*)owner)->wrtLine(0,(ushort)yp,(ushort)size.x,1,b);

    BufClass.movChar((ushort)((size.y-2)*size.x),frameChars[10],cButton,(ushort)size.x,b); //заполнение всей строчки
    BufClass.putsAttr((ushort)((size.y-2)*size.x+size.x-1),cShadow,b); //фон в последний символ(0-(usTabX-1))
    BufClass.putsChar((ushort)((size.y-2)*size.x),frameChars[3],b);
    BufClass.putsChar((ushort)((size.y-2)*size.x+size.x-2),frameChars[9],b);
    BufClass.putsChar((ushort)((size.y-2)*size.x+size.x-1),shadows[1],b);
//    ((TNotebook*)owner)->wrtLine(0,(ushort)(size.y-2),(ushort)size.x,1,b);

    //нижняя строчка - тень
    //b.putAttribute(0,cShadow);  //1         //если нажата второй символ фон
    BufClass.movChar((ushort)((size.y-1)*size.x),shadows[2],cShadow,(ushort)size.x,b); //заполнение всей строчки
    BufClass.putsChar((ushort)((size.y-1)*size.x),' ',b);
//    ((TNotebook*)owner)->wrtLine(0,(ushort)(size.y-1),(ushort)size.x,1,b);

    //for(int i=0;i<topPage;i++) NotPages[i].Page->draw();
    for(TView* p=first();p!=0;p=p->nextView())
      p->draw();//View();
   }
  else
   PanelTitle->draw();
 }

//const char *TPanel::getTitle( short ){return title;}

//  if( event.what== evMouseDown && (state & sfActive) != 0 )
//   {
//    TPoint mouse = makeLocal( event.mouse.where );
//    if( mouse.y == 0 )
//        {
//        if( ( ((TWindow *)owner)->flags & wfClose ) != 0 &&
//                mouse.x >= 2 &&
//                mouse.x <= 4 )
//            {
//            event.what= evCommand;
//            event.message.command = cmClose;
//            event.message.infoPtr = owner;
//            putEvent( event );
//            clearEvent( event );
//            }
//        }
//   }

void TPanel::handleEvent(TEvent& event)
 {
  TRect  limits;

  TGroup::handleEvent(event);
  if(event.what == evKeyDown)
   switch (event.keyDown.keyCode)
    {
     case  kbTab:
       selectNext(False);
       drawView();
       clearEvent(event);
       break;
     case  kbShiftTab:
       selectNext(True);
       drawView();
       clearEvent(event);
       break;
    }
  else
   if(event.what == evBroadcast &&
      event.message.command==cmSelectWindowNum &&
      event.message.infoInt==number &&(options & ofSelectable) != 0)
    {
     select();
     clearEvent(event);
    }
 }

void TPanel::sizeLimits( TPoint& min, TPoint& max )
 {
  TView::sizeLimits(min,max);
  min=minWinSize;
 }

//------------ TCheckBoxes -------------------
//#define Uses_TCheckBoxes

#define cpCheckBox "\x01\x11\x12\x12"

TCheckBox::TCheckBox(const TRect& bounds,int frames,ushort delta) :
    TView(bounds),value(0),sel(0)
 {
  state|=sfExposed|sfVisible;
  options|=ofSelectable|ofBuffered|ofFirstClick|ofPreProcess|ofPostProcess;//|ofTopSelect
//  eventMask = evMouseDown | evKeyDown | evCommand | evBroadcast;
  buffer=0;
  topChBtn=0;
  flFrames=frames;
  Delta=delta;

  //short i=0;
  //TSItem *p;
  //for(p = aStrings;p!=0;p = p->next) i++;

  //strings = new TStringCollection( i, 0 );

  //while(aStrings!=0)
  // {
  //  p = aStrings;
  //  strings->atInsert( strings->getCount(), newStr(aStrings->value) );
  //  aStrings = aStrings->next;
  //  delete p;
  // }

  setCursor( 2, 0 );
  showCursor();
 }

TCheckBox::~TCheckBox()
 {
  //destroy( (TCollection *)strings );
 }

void TCheckBox::insertChBtn(char* Name,ushort id,Boolean flEnable,ushort EditDelta)
 {
  if(topChBtn<MAXCHBTN)
   {
    ChBtn[topChBtn].Name=new char[strlen(Name)+1];
    if((size.x>5)&&(strlen(Name)>(size.x-5)))
     {
      strncpy(ChBtn[topChBtn].Name,Name,size.x-5);
      ChBtn[topChBtn].Name[size.x-5]=0;
     }
    else
      strcpy(ChBtn[topChBtn].Name,Name);
    ChBtn[topChBtn].id=id;
    ChBtn[topChBtn].flEnable=flEnable;
    ChBtn[topChBtn].EditDelta=EditDelta;
    topChBtn++;
   }
 }

void TCheckBox::setState( ushort aState, Boolean enable )
 {
//  if(owner!=0) owner->setState(aState,enable);
  TView::setState(aState,enable);
//  if( (aState&sfSelected) != 0 ) setState(sfActive,enable);
  if( aState==sfSelected ) drawView();
 }

TPalette& TCheckBox::getPalette() const
 {
  static TPalette palette(cpCheckBox,sizeof(cpCheckBox)-1);
  return palette;
 }

void TCheckBox::draw()
 {
  TBuffer BufClass;
  ushort color;

  ushort yp=0;

  if(buffer==0) BufClass.getBuffer(&buffer,this);
  if(buffer!= 0)
   {
    BufClass.movChar(0,' ',getColor(0x01),(ushort)(size.x*size.y),buffer); //заполнение всей строчки
    for(int cur=0;(cur<topChBtn)&&(yp<size.y);cur++)
     {
      if( (cur==sel) && (state & sfSelected) != 0 )
       {
        color=getColor(0x0402);
        setCursor(2,yp);
       }
      else color=getColor(0x0301); //0x3e70;

 //      if(ChBtn[cur].flEnable==False) color = cDisabl;
      BufClass.movCStr((ushort)(yp*size.x)," [ ] ",color,buffer);
      BufClass.movChar((ushort)(yp*size.x+5),' ',color,(ushort)(size.x-5),buffer);
      if(mark(cur) ) BufClass.putsChar((ushort)(yp*size.x+2),'X',buffer);
      BufClass.movCStr((ushort)(yp*size.x+5),(char *)ChBtn[cur].Name,color,buffer);
      //if(showMarkers && (state & sfSelected) != 0 && cur == sel )
      // {
      //  b.putChar( col, specialChars[0] );
      //  b.putChar( column(cur+size.y)-1, specialChars[1] );
      // }
      yp+=Delta+ChBtn[cur].EditDelta;
     }
    BufClass.wrtBuf(0,0,(ushort)(size.x),(ushort)(size.y),buffer,this);
   }
 }

void TCheckBox::handleEvent(TEvent& event)
 {
  TView::handleEvent(event);
//  if( ! (options & ofSelectable) ) return;
  if(event.what==evMouseDown)
   {
    TPoint mouse = makeLocal(event.mouse.where);
    int i=findSel(mouse);
    if( i!= -1 ) sel=i;
    //drawView();
    do
     {
      mouse = makeLocal( event.mouse.where );
      if(findSel(mouse)==sel) showCursor();
      else                    hideCursor();
     } while( mouseEvent(event,evMouseMove) );
    showCursor();
    mouse = makeLocal( event.mouse.where );
    if(findSel(mouse) == sel)
     {
      press(sel);
      drawView();
     }
    clearEvent(event);
   }
  else
   if(event.what==evKeyDown)
    switch (ctrlToArrow(event.keyDown.keyCode))
     {
      case kbUp:
      case kbLeft:
        if( (state & sfFocused) != 0 )
         {
          if( --sel < 0 ) sel=topChBtn-1;
          movedTo(sel);
          drawView();
          clearEvent(event);
         }
        break;
      case kbDown:
      case kbRight:
        if( (state & sfFocused)!=0)
         {
          if( ++sel >=topChBtn) sel = 0;
          movedTo(sel);
          drawView();
          clearEvent(event);
         }
        break;
      default:
        //for(int i=0; i<strings->getCount(); i++ )
        for(int i=0;i<topChBtn;i++)
         {
          char c=hotKey((char *)ChBtn[i].Name); //(strings->at(i)) );

          if( getAltCode(c)==event.keyDown.keyCode ||
            ( (owner->phase==phPostProcess ||
            (state & sfFocused) != 0 ) && c != 0 &&
            toupper(event.keyDown.charScan.charCode) == c))
           {
            select();
            sel =  i;
            movedTo(sel);
            press(sel);
            drawView();
            clearEvent(event);
            return;
           }
          }
        if(event.keyDown.charScan.charCode==' ' && (state & sfFocused)!=0)
         {
          press(sel);
          drawView();
          clearEvent(event);
         }
     }
 }

void TCheckBox::movedTo( int ) { }

void TCheckBox::press(int item)
 {
  if(item>=topChBtn) return;
  value=(ushort)(value^(1 << item));

  //message( owner, evBroadcast, cmRecordHistory, 0 );
  //if( (flags & bfBroadcast)!=0 )
    //message( owner, evBroadcast, command, this );
  //else
   {
    TEvent e;
    e.what=evCommand;
    e.message.command=ChBtn[item].id;//command;
    e.message.infoPtr=this;
    putEvent(e);
   }
 }

ushort TCheckBox::getHelpCtx()
 {
  if(helpCtx==hcNoContext) return hcNoContext;
  else                     return (ushort)(helpCtx + sel);
 }

size_t TCheckBox::dataSize()
 {
  return sizeof(value);
 }

void TCheckBox::getData(void * rec)
 {
  *(ushort*)rec=value;
 }

void TCheckBox::setData(void * rec)
 {
  value=*(ushort *)rec;
  //drawView();
 }

Boolean TCheckBox::mark(int item) {return Boolean( (value & (1 << item))!=0);}

int TCheckBox::findSel( TPoint p )
 {
  TRect r = getExtent();
  if( !r.contains(p) ) return -1;
  else
   {
    int s=0;
    for(int cur=0;cur<=p.y;)
     {
      if(cur>=p.y) return s;
      cur+=Delta+ChBtn[s].EditDelta;
      s++;
     }
    return -1;
   }
 }

//----------------- RadioBattons -----------------

#define cpRadioBattons "\x01\x11\x12\x12"

TRadioBattons::TRadioBattons(const TRect& bounds,ushort delta):
    TView(bounds),value( 0 ),sel( 0 )
 {
  state|=sfExposed|sfVisible;
  options|=ofSelectable|ofBuffered|ofFirstClick|ofPreProcess|ofPostProcess;//|ofTopSelect
  buffer=0;
  topRbBtn=0;
  Delta=delta;
//  short i=0;
//  TSItem *p;
//  for( p=aStrings; p != 0; p = p->next ) i++;

//  strings=new TStringCollection( i, 0 );

//  while(aStrings!=0)
//   {
//    p = aStrings;
//    strings->atInsert( strings->getCount(), newStr(aStrings->value) );
//    aStrings = aStrings->next;
//    delete p;
//   }

  setCursor(2,0);
  showCursor();
 }

TRadioBattons::~TRadioBattons()
 {
//  destroy( (TCollection *)strings );
 }

void TRadioBattons::insertRbBtn(char* Name,ushort id,Boolean flEnable)
 {
  if(topRbBtn<MAXRBBTN)
   {
    RbBtn[topRbBtn].Name=new char[strlen(Name)+1];
    if((size.x>5)&&(strlen(Name)>(size.x-5)))
     {
      strncpy(RbBtn[topRbBtn].Name,Name,size.x-5);
      RbBtn[topRbBtn].Name[size.x-5]=0;
     }
    else
      strcpy(RbBtn[topRbBtn].Name,Name);
    RbBtn[topRbBtn].id=id;
    RbBtn[topRbBtn].flEnable=flEnable;
    topRbBtn++;
   }
 }

void TRadioBattons::setState( ushort aState, Boolean enable )
 {
  TView::setState( aState, enable );
  if( aState == sfSelected ) drawView();
 }

TPalette& TRadioBattons::getPalette() const
 {
  static TPalette palette(cpRadioBattons,sizeof(cpRadioBattons)-1 );
  return palette;
 }

void TRadioBattons::draw()
 {
  TBuffer BufClass;
  ushort color;

  ushort yp=0;

  if(buffer==0) BufClass.getBuffer(&buffer,this);
  if(buffer!= 0)
   {
    BufClass.movChar(0,' ',getColor(0x01),(ushort)(size.x*size.y),buffer); //заполнение всей строчки
    for(int cur=0;(cur<topRbBtn)&&(yp<size.y);cur++)
     {
      if( (cur==sel) && (state & sfSelected) != 0 )
       {
        color=getColor(0x0402);
        setCursor(2,yp);
       }
      else  color=getColor(0x0301);
 //      if(ChBtn[cur].flEnable==False) color = cDisabl;
      BufClass.movCStr((ushort)(yp*size.x)," ( ) ",color,buffer);
      BufClass.movChar((ushort)(yp*size.x+5),' ',color,(ushort)(size.x-5),buffer);
      if(mark(cur) ) BufClass.putsChar((ushort)(yp*size.x+2),7,buffer);
      BufClass.movCStr((ushort)(yp*size.x+5),(char *)RbBtn[cur].Name,color,buffer);
      //if(showMarkers && (state & sfSelected) != 0 && cur == sel )
      // {
      //  b.putChar( col, specialChars[0] );
      //  b.putChar( column(cur+size.y)-1, specialChars[1] );
      // }
      yp+=Delta;
     }
    BufClass.wrtBuf(0,0,(ushort)(size.x),(ushort)(size.y),buffer,this);
   }
 }

void TRadioBattons::handleEvent( TEvent& event )
 {
  TView::handleEvent(event);
  if ( ! (options & ofSelectable) ) return;
  if( event.what == evMouseDown )
   {
    TPoint mouse = makeLocal( event.mouse.where );
    int i = findSel(mouse);
    if( i != -1 ) sel = i;
    //drawView();
    do
     {
      mouse = makeLocal( event.mouse.where );
      if( findSel(mouse) == sel ) showCursor();
      else                        hideCursor();
     } while( mouseEvent(event,evMouseMove) );
    showCursor();
    mouse = makeLocal( event.mouse.where );
    if( findSel(mouse) == sel )
     {
      press(sel);
      drawView();
     }
    clearEvent(event);
   }
  else
   if( event.what == evKeyDown )
    switch (ctrlToArrow(event.keyDown.keyCode))
     {
      case kbUp:
      case kbLeft:
       if( (state & sfFocused) != 0 )
        {
         if( --sel < 0 ) sel=topRbBtn-1;
         movedTo(sel);
         drawView();
         clearEvent(event);
        }
       break;
      case kbDown:
      case kbRight:
       if( (state & sfFocused) != 0 )
        {
         if( ++sel >=topRbBtn) sel = 0;
         movedTo(sel);
         drawView();
         clearEvent(event);
        }
       break;
      default:
       //for(int i=0; i<strings->getCount();i++)
       for(int i=0; i<topRbBtn;i++)
        {
         char c=hotKey( (char *)RbBtn[i].Name);//(strings->at(i)) );
         if(getAltCode(c)==event.keyDown.keyCode ||
             ( ( owner->phase==phPostProcess ||
             (state & sfFocused)!=0 ) && c!=0 &&
             toupper(event.keyDown.charScan.charCode)==c) )
          {
           select();
           sel =  i;
           movedTo(sel);
           press(sel);
           drawView();
           clearEvent(event);
           return;
          }
        }
       if( event.keyDown.charScan.charCode == ' ' && (state & sfFocused) != 0 )
        {
         press(sel);
         drawView();
         clearEvent(event);
        }
     }
 }

ushort TRadioBattons::getHelpCtx()
 {
  if(helpCtx==hcNoContext) return hcNoContext;
  else                     return (ushort)(helpCtx + sel);
 }

size_t TRadioBattons::dataSize()
 {
  return sizeof(value);
 }

void TRadioBattons::getData(void * rec)
 {
  *(ushort*)rec=value;
 }

void TRadioBattons::setData(void * rec)
 {
  value=*(ushort *)rec;
  //drawView();
  sel=value;
 }

Boolean TRadioBattons::mark( int item)
 {
  return Boolean( item == value );
 }

void TRadioBattons::movedTo( int item)
 {
  value =(ushort)item;
 }

void TRadioBattons::press( int item)
 {
  value=(ushort)item;
 }

int TRadioBattons::findSel( TPoint p )
 {
  TRect r = getExtent();
  if( !r.contains(p) ) return -1;
  else
   {
    int s = p.y/Delta;//i + p.y;
    if( s >=topRbBtn) return -1;
    else return s;
   }
}

//---------------- ListViewsRec ----------------

#define cpListViews "\x1A\x1A\x1d\x1C\x1D"

struct TListViewsRec
 {
//  TCollection *items;
  ushort selection;
 };

TListViews::TListViews(const TRect& bounds,
          TScrollBar *aHScrollBar/*,TScrollBar *aVScrollBar*/,short MaxLines) :
    TView(bounds),
    range(0),
//    numCols(1),
//    items( 0 ),
    topItem(0),
    focused(0)
 {
  state|=sfExposed|sfVisible;
  options|=ofSelectable|ofBuffered|ofFirstClick;//|ofTopSelect
  eventMask |=(ushort)evBroadcast;
  pTail=NULL;
  pListTop=NULL;
  //if(aVScrollBar != 0)
   {
  //short pgStep,arStep;
  //  if( numCols == 1 )
  //   {
  //    pgStep = size.y - 1;
  //    arStep = 1;
  //   }
  //  else
  //   {
  //    pgStep = size.y * numCols;
  //    arStep = size.y;
  //   }
  //  aVScrollBar->setStep( pgStep, arStep );
   }

  if(aHScrollBar!=0)
   {
//    if(range==0) aHScrollBar->setStep((short)(size.x-1),1);
//    else         aHScrollBar->setStep(range,(short)size.x);
//    aHScrollBar->setParams(0,0,(ushort)(size.x-1),8,1);
    aHScrollBar->setParams(0,0,(ushort)(size.x-1),1,1);
   }

  hScrollBar=aHScrollBar;
  //vScrollBar=aVScrollBar;

  lvBuffer=0;
  maxLines=MaxLines,
  setRange(0);
 }

void TListViews::shutDown()
 {
  hScrollBar=0;
  //vScrollBar = 0;
  TView::shutDown();
 }

void TListViews::insertItem(char* Name,char flSelection)
 {
  if(Name!=0)
  AddRecord(Name,flSelection,this);
//  strncpy(items[range].diskName,Name,MAXLISTITEMNAMELEN);
//  items[range].diskName[MAXLISTITEMNAMELEN] = '\0';
//  items[range].flSelection=flSelection;
//  items[range].type=usType;
//  range++;

  range=0;
  if(pTail!=NULL)
   {
    PLIST pList=pTail;
    do
     {
      pList=pList->pNext;
      range++;
     } while(pList != NULL);
   }
//  numCols=(short)(range/size.y);
//  if((numCols*size.y)<range) numCols++;
  if(flSelection!=0xff)
   {
    setRange(range);
    focusItemNum((short)(range-1));
   }
  //drawView();
 }

void TListViews::deleteItem(char* Name)
 {
  if(Name==0) return;
  if((strlen(Name)<=3)&&(Name[1]==':')&&((Name[2]==0)||(Name[2]=='\\'))) return;
  //messageBox(Name,1);
  PLIST pList,pDel;
  if(pTail!=NULL)
   {
    while((pTail!=NULL)&&(strcmp(pTail->szName,Name)==0))
     {
      pDel=pTail;
      pTail=pTail->pNext;
      Free(pDel);
     }
   if(pTail!=NULL)
    if(pTail->pNext!=NULL)
     {
      pList=pTail;
      do
       {
        if(pList->pNext!=NULL)
          if(strcmp(pList->pNext->szName,Name)==0)
           {
            pDel=pList->pNext;
            if(pList->pNext==pListTop)
             {
              pListTop=pList;
              pList->pNext=NULL;
             }
            if(pList->pNext!=NULL)
             pList->pNext=pList->pNext->pNext;
            Free(pDel);
            goto exR;
           }
        pList=pList->pNext;
       } while(pList != NULL);
     }
   }
exR:
  range=0;
  if(pTail!=NULL)
   {
    PLIST pList=pTail;
    do
     {
      pList=pList->pNext;
      range++;
     } while(pList != NULL);
   }
  setRange(range);
  focusItem((short)(range-1));
  //drawView();
 }

void TListViews::deleteList(void)
 {
  if(pTail!=NULL)
   {
    PLIST pList;
    do
     {
      pList=pTail;
      pTail=pTail->pNext;
      Free(pList);
     } while(pTail != NULL);
   }
  range=0;
 }

void TListViews::changeBounds( const TRect& bounds )
 {
  TView::changeBounds( bounds );
//  if(hScrollBar!=0) hScrollBar->setStep((short)(range-1),(short)size.x);
 }

void TListViews::getText(char *dest,short item,short maxChars)
 {
  *dest=EOS;
  if(pTail!=NULL)
   {
    int   i=0;
    PLIST pList=pTail;
    do
     {
      if(i==item)
       {
        strncpy(dest,(const char *)(pList->szName),maxChars);
        dest[maxChars]='\0';
        return;
       }
      pList=pList->pNext;
      i++;
     } while(pList != NULL);
   }
 }

TPalette& TListViews::getPalette() const
 {
  static TPalette palette(cpListViews,sizeof(cpListViews)-1);
  return palette;
 }

void TListViews::draw()
 {
  TBuffer BufClass;

  ushort normalColor, selectedColor, focusedColor, color;
  uchar scOff;

  if( (state&(sfSelected|sfActive))==(sfSelected|sfActive))
   {
    normalColor  =getColor(1);
    focusedColor =getColor(3);
    selectedColor=getColor(4);
   }
  else
   {
    normalColor  =getColor(2);
    selectedColor=getColor(4);
   }

  short indent;
  if(hScrollBar!=0) indent=hScrollBar->value;
  else              indent=0;
//  if((colWidth*numCols)<size.x) indent=0;

//  char buf[256];
  if(lvBuffer==0) BufClass.getBuffer(&lvBuffer,this);
  if(lvBuffer!=0)
   {
    BufClass.movChar(0,' ',normalColor,(ushort)(size.x*size.y),lvBuffer); //заполнение всей строчки
    short colWidth,curCol=0;
    if(pTail!=NULL)
     {
      short item=0;
      PLIST listItem=pTail;
      while((item!=topItem)&&(listItem!=NULL))
       {
        listItem=listItem->pNext;
        item++;
       }
      int MaxI=((maxLines==NULL)?size.y:maxLines);
      if(MaxI>size.y) MaxI=size.y;
      int i;
      while(listItem!=NULL)
       {
        colWidth=(short)4;
        if(colWidth>size.x) colWidth=(short)size.x;
        for(i=0;i<MaxI;i++)
         {
          if((item>=range)||(listItem==NULL)) goto exdrw;

          if( (state & (sfSelected|sfActive) )==(sfSelected|sfActive) &&
              focused==item && range>0)
           {
            color=focusedColor; scOff=0;
            setCursor((short)(curCol+1),(short)i);
           }
          else
           if(item<range && ((isSelected(item)==1) || (isSelected(item)==2)) )
                        { color=selectedColor; scOff=2; }
           else         { color=normalColor;   scOff=4; }

          if((strlen(listItem->szName)+4)>colWidth) colWidth=(short)(strlen(listItem->szName)+4);

          if((curCol+2)<size.x)
           {
            if(isSelected(item)==1) //text[0]=0xfb;
              BufClass.putsChar((ushort)(i*size.x+curCol),0xfb,lvBuffer);
            if(isSelected(item)==2) //text[0]=0xfb;
              BufClass.putsChar((ushort)(i*size.x+curCol),'-',lvBuffer);

            char text[0x200];
            strcpy(text,listItem->szName);
            if(strlen(text)>0)
             {
              char *pStr=text;
              while(*pStr) { if(*pStr=='~') *pStr=' '; pStr++; }
             }
            if(strlen(text)<(size.x-(curCol+2)))
              BufClass.movCStr((ushort)(i*size.x+curCol+2),(char *)text,color,lvBuffer);
            else
              BufClass.movNCStr((ushort)(i*size.x+curCol+2),(char*)text,color,(short)(size.x-(curCol+2)),lvBuffer);
           }
          //if(showMarkers)
          // {
          //  b.putChar( curCol, specialChars[scOff] );
          //  b.putChar((short)(curCol+colWidth-2), specialChars[scOff+1] );
          // }

          listItem=listItem->pNext;
          item++;
         }
        if((maxLines!=NULL)&&(i<maxLines))
         while((i<maxLines)&&(listItem!=NULL))
          {
           listItem=listItem->pNext;
           item++;
           i++;
          }
        curCol+=colWidth;//-indent;
       }
      // else if(i==0 && j==0) BufClass.moveStr((ushort)(curCol+1),"<empty>",getColor(1));
     }
exdrw:
    BufClass.wrtBuf(0,0,(ushort)(size.x),(ushort)(size.y),lvBuffer,this);
   }
 }

short TListViews::findSel(TPoint p)
 {
  TRect r = getExtent();
  if(!r.contains(p) ) return -1;
  else
   {
//newItem =(short)(mouse.y+(size.y*(mouse.x/colWidth))+topItem);
    short i,item,topColItem;
    short colWidth,curCol;
    curCol=0;
    i=0;
    if(pTail!=NULL)
     {
      PLIST listItem=pTail;
      do
       {
        if(i==topItem)
         {
          item=(short)topItem;
          while(1)
           {
            topColItem=item;
            colWidth=(short)4;
            if(colWidth>size.x) colWidth=(short)size.x;
            for(i=0;i<size.y;i++)
             {
              if(item>=range)
               {
                if((p.x>=curCol)&&(p.x<(curCol+colWidth)))
                if((topColItem+p.y)<range) return (short)(topColItem+p.y);
                return -1;
               }

              if((strlen(listItem->szName)+4)>colWidth) colWidth=(short)(strlen(listItem->szName)+4);

              listItem=listItem->pNext;
              item++;
             }
            if((p.x>=curCol)&&(p.x<(curCol+colWidth)))
              return (short)(topColItem+p.y);
            curCol+=colWidth;
           }
         }
        listItem=listItem->pNext;
        i++;
       } while(listItem != NULL);
     }
    if((p.x>=curCol)&&(p.x<(curCol+colWidth)))
      if((topColItem+p.y)<range) return (short)(topColItem+p.y);
    return -1;
   }
 }

void TListViews::focusItem(short item)
 {
  focused=item;
  if(hScrollBar!=0 ) hScrollBar->setValue(item);
  //if(vScrollBar!=0 ) vScrollBar->setValue( item );
  //else
  // drawView();

  if(item<topItem)
   //if( numCols == 1 ) topItem = item;
   //else
   topItem=(short)(item-item % ((maxLines==NULL)?size.y:maxLines));
  else
   if(pTail!=NULL)
    {
     int i=0,Item=(short)topItem;
     short colWidth,curCol=0;
     PLIST listItem=pTail;

     while((i!=topItem)&&(listItem!=NULL))
      {
       listItem=listItem->pNext;
       i++;
      }
     int MaxI=((maxLines==NULL)?size.y:maxLines);
     if(MaxI>size.y) MaxI=size.y;
     while(listItem!=NULL)
      {
       colWidth=(short)4;
       if(colWidth>size.x) colWidth=(short)size.x;
       for(i=0;i<MaxI;i++)
        {
         if((Item>=range)||(listItem==NULL)) goto exfsel;

         if((strlen(listItem->szName)+4)>colWidth) colWidth=(short)(strlen(listItem->szName)+4);

         listItem=listItem->pNext;
         Item++;
        }
       if((maxLines!=NULL)&&(i<maxLines))
        while((i<maxLines)&&(listItem!=NULL))
         {
          listItem=listItem->pNext;
          Item++;
          i++;
         }
       curCol+=colWidth;
       if(curCol>size.x)
        {
         if(item>=Item)
         if((Item-topItem)<((maxLines==NULL)?size.y:maxLines))//( numCols == 1 )
          topItem=(short)(item-((maxLines==NULL)?size.y:maxLines)+1);
         else
          topItem=(short)(item-item%((maxLines==NULL)?size.y:maxLines)
           -(Item-topItem-((maxLines==NULL)?size.y:maxLines)));
           //topItem=(short)(item-item%size.y-(size.y*(numCols-1)));
        }
      }
    }
exfsel:
  drawView();
 }

void TListViews::focusItemNum( short item )
 {
  if(item<0) item=0;
  else
   if(item>=range && range>0) item=(short)(range-1);

  if(range!=0) focusItem(item);
 }

char TListViews::isSelected(short item)
 {
  if(pTail!=NULL)
   {
    short i=0;
    PLIST listItem=pTail;
    do
     {
      if(i==item) return listItem->Selected;//Boolean( item == focused );
      listItem=listItem->pNext;
      i++;
     } while(listItem != NULL);
   }
  return 0;
 }

void TListViews::selectItem(short item)
 {
  if((pTail!=NULL)&&(maxLines==NULL))
   {
    short i=0;
    PLIST listItem=pTail;
    do
     {
      if(i==item)
       {
        listItem->Selected++;
        if(listItem->Selected==3) listItem->Selected=0;
        return;
       }
      listItem=listItem->pNext;
      i++;
     } while(listItem!=NULL);
   }
  //message(owner,evBroadcast,cmListItemSelected,this);
 }

void TListViews::handleEvent( TEvent& event )
 {
  TPoint mouse;
  short  oldItem,newItem;
  ushort count;
  int mouseAutosToSkip = 4;

  TView::handleEvent(event);

  if(event.what==evMouseDown)
   {
    oldItem=focused;
    mouse=makeLocal(event.mouse.where);
    newItem=findSel(mouse);
//ushort colWidth=(ushort)(size.x/numCols + 1);
//newItem =(short)(mouse.y+(size.y*(mouse.x/colWidth))+topItem);
    count = 0;
    do
     {
      if(newItem!=oldItem) focusItemNum( newItem );
      oldItem=newItem;
      if(event.mouse.doubleClick && range>focused)
       {
        selectItem(focused);
        break;
       };
      mouse=makeLocal( event.mouse.where );
      if(mouseInView(event.mouse.where) )
      newItem=findSel(mouse);
//newItem=(short)(mouse.y+(size.y*(mouse.x/colWidth))+topItem);
      else
       {
        if((range-1)<=size.y) //if(numCols==1)
         {
          if(event.what==evMouseAuto) count++;
          if(count==mouseAutosToSkip)
           {
            count = 0;
            if(mouse.y<0) newItem =(short)(focused - 1);
            else
             if(mouse.y>=size.y) newItem =(short)(focused + 1);
           }
         }
        else
         {
          if( event.what==evMouseAuto ) count++;
          if( count==mouseAutosToSkip )
           {
            count = 0;
            if( mouse.x<0 ) newItem=(short)(focused - size.y);
            else
             if( mouse.x>=size.x ) newItem=(short)(focused + size.y);
             else
              if(mouse.y<0 ) newItem=(short)(focused - focused % size.y);
              else
               if( mouse.y>size.y ) newItem=(short)(focused-focused % size.y + size.y - 1);
           }
         }
       }
     } while( mouseEvent( event, evMouseMove|evMouseAuto ) );
    focusItemNum(newItem);
    clearEvent( event );
   }
  else
   if(event.what == evKeyDown )
    {
     if(event.keyDown.charScan.charCode==' ' && focused<range )
      {
       selectItem(focused);
       newItem=focused;
      }
     else
      {
       switch (ctrlToArrow(event.keyDown.keyCode))
        {
         case kbUp:
           newItem=(short)(focused-1);
           break;
         case kbDown:
           newItem=(short)(focused+1);
           break;
         case kbRight:
           if((focused+((maxLines==NULL)?size.y:maxLines))<range)
             newItem=(short)(focused+((maxLines==NULL)?size.y:maxLines));
           else newItem=(short)(range-1);
           break;
         case kbLeft:
           if(focused>((maxLines==NULL)?size.y:maxLines))
            newItem=(short)(focused-((maxLines==NULL)?size.y:maxLines));
           else newItem=0;
           break;
         //case kbPgDn:
         //    newItem =(short)(focused+size.y*numCols);
         //    break;
         //case  kbPgUp:
         //    newItem =(short)(focused-size.y*numCols);
         //    break;
         case kbHome:
           newItem=topItem;
           break;
         //case kbEnd:
         //    newItem =(short)(topItem+(size.y*numCols)-1);
         //    break;
         case kbCtrlPgDn:
           newItem=(short)(range-1);
           break;
         case kbCtrlPgUp:
           newItem=0;
           break;
         case kbDel:
          if(pTail!=NULL)
           {
            short i=0;
            PLIST listItem=pTail;
            do
             {
              if(i==focused)
               {
                deleteItem(listItem->szName);
                return;
               }
              listItem=listItem->pNext;
              i++;
             } while(listItem != NULL);
           }
           break;
         default:
           return;
        }
      }
     focusItemNum(newItem);
     clearEvent(event);
    }
  else
   if( event.what==evBroadcast )
    {
     if( (options & ofSelectable) != 0 )
      {
       if( event.message.command==cmScrollBarClicked &&
         ( event.message.infoPtr==hScrollBar /*||event.message.infoPtr==vScrollBar*/ ))
        select();
       else
        if( event.message.command==cmScrollBarChanged )
         {
          //if( vScrollBar == event.message.infoPtr )
          // {
          //  focusItemNum( vScrollBar->value );
          //  drawView();
          // }
          //else
          if(hScrollBar==event.message.infoPtr)
           {
//            if( hScrollBar->value != d)
             {
              //newItem=(short)((range-1)*hScrollBar->value/(size.x-1));
              newItem=hScrollBar->value;
              focusItemNum(newItem);
             }
            clearEvent(event);
//             drawView();
           }
         }
      }
   }
 }

void TListViews::setRange( short aRange )
 {
  range = aRange;
  if( focused > aRange ) focused = 0;
  if(hScrollBar!=0 )
    hScrollBar->setParams(focused,0,(short)(aRange-1),hScrollBar->pgStep,hScrollBar->arStep);
  //if( vScrollBar != 0 )
  //  vScrollBar->setParams( focused,0,aRange - 1,vScrollBar->pgStep,vScrollBar->arStep);
  else
   drawView();
 }

void TListViews::setState( ushort aState, Boolean enable )
 {
  TView::setState( aState, enable );
  if( (aState & (sfSelected|sfActive)) !=0 )
   {
    if(hScrollBar!=0)
     if(getState(sfActive)) hScrollBar->show();
     else                   hScrollBar->hide();
    //if(vScrollBar!=0)
    // if(getState(sfActive)) vScrollBar->show();
    // else                   vScrollBar->hide();
    drawView();
   }
 }

size_t TListViews::dataSize()
 {
  return sizeof(TListViewsRec);
 }

void TListViews::getData( void * rec )
 {
  TListViewsRec *p = (TListViewsRec*)rec;
//  p->items = items;
  p->selection = focused;
 }

void TListViews::setData( void *rec )
 {
  TListViewsRec *p = (TListViewsRec *)rec;
//  newList(p->items);
  focusItem(p->selection);
  drawView();
 }

//void TListViews::newList( TCollection *aList )
// {
//  destroy(       items );
//  items =       aList;
//  if( aList != 0 ) setRange( aList->getCount() );
//  else             setRange(0);
//  if( range > 0 )  focusItem(0);
//  drawView();
// }

//--------------- TBuffer --------------

void TBuffer::getBuffer(ushort far **dataBuff,TView *v)
 {
  if( (v->state & sfExposed) != 0 )
   if( (v->options & ofBuffered) != 0 && (*dataBuff == 0 ))
     TVMemMgr::allocateDiscardable( (void* &)*dataBuff,v->size.x*v->size.y*sizeof(ushort) );
 }

void TBuffer::movChar(ushort indent,char c,ushort attr,ushort count,ushort far *usData)
 {
  if(attr && c)
   for(unsigned int i=0;i<count; i++)
      usData[indent+i]=(ushort)(((unsigned char) c)+(attr << 8));
  else
   if(c)
    for(unsigned int i=0;i<count; i++)
       usData[indent+i]=(ushort)( ((unsigned char) c)+(usData[indent+i] & 0xFF00));
   else
    for(unsigned int i=0;i<count; i++) // change attribute byte only
       usData[indent+i]=(ushort)((attr << 8)+(usData[indent+i] & 0x00FF));
 }

void TBuffer::movCStr(ushort indent,const char far *str,ushort attrs,ushort far *usData)
 {
  int offset=0;
  for(int i=0;str[i];i++)
   {
    if(str[i]=='~') attrs=(ushort)((attrs>>8)+((attrs & 0xFF)<<8));
    else usData[indent+offset++]=(ushort)( ((unsigned char) (str[i]) ) + ((attrs&0xFF)<<8));
   };
 }

void TBuffer::movNCStr(ushort indent,const char far *str,ushort attrs,ushort chars,ushort far *usData)
 {
  int offset=0;
  for(int i=0;(str[i])&&(i<chars);i++)
   {
    if(str[i]=='~') attrs=(ushort)((attrs>>8)+((attrs & 0xFF)<<8));
    else usData[indent+offset++]=(ushort)( ((unsigned char) (str[i]) ) + ((attrs&0xFF)<<8));
   };
 }

extern TPoint shadowSize;
extern uchar shadowAttr;
typedef struct _StaticVars1
 {
  const void* buf;
 } StaticVars1;

typedef struct _StaticVars2
 {
  TView*      target;
  short       offset;
  short       y;
 } StaticVars2;

static StaticVars1 wrtVars1;
static StaticVars2 wrtVars2;

void TBuffer::wrtBuf(short x,short y,short w,short h,ushort far *buf,TView *v)
 {
  for(int i=0;i<h;i++)
    wrtView(x,(ushort)(x+w),(ushort)(y+i),(ushort*)(buf + w*i),v);
 }

//void TBuffer::wrtLine(short x,short y,short w,short h,ushort far *buf,TView *v)
// {
//  if(h==0) return;
//  for(int i=0;i<h;i++)
//    wrtView(x,(ushort)(x+w),(ushort)(y+i),buf,v);
// }

void TBuffer::wrtView(short x1,short x2,short y,ushort far *buf,TView *v)
 {
  if(y<0 || y>=v->size.y) return;
  if(x1<0) x1=0;
  if(x2>v->size.x) x2=(short)(v->size.x);
  if(x1>=x2) return;
  wrtVars2.offset=x1;
  wrtVars2.y=y;
  wrtVars1.buf=buf;
                  //this
  wrtViewRec2(x1,x2,v,0);
 }

void TBuffer::wrtViewRec2(short x1,short x2,TView* p,int shadowCounter)
 {
  if(!(p->state & sfVisible) || p->owner==0) return;

  StaticVars2 savedStatics = wrtVars2;

  wrtVars2.y+=p->origin.y;       //переводим координаты в абсюлютные
  x1+=p->origin.x;
  x2+=p->origin.x;
  wrtVars2.offset+= p->origin.x; //абсолютный x1
  wrtVars2.target=p;             //this

  TGroup* g=p->owner;
  // проверка на вхождение в отрисовываемую область
  if(wrtVars2.y<g->clip.a.y || wrtVars2.y>=g->clip.b.y)
   { wrtVars2=savedStatics; return; }

  if(x1<g->clip.a.x) x1=(short) g->clip.a.x;
  if(x2>g->clip.b.x) x2=(short) g->clip.b.x;
  if(x1>=x2)
   { wrtVars2=savedStatics; return; }

  //g->last самый дальний от z вершины у родителя
  wrtViewRec1(x1,x2,g->last,shadowCounter);
  wrtVars2 = savedStatics;
 }

void TBuffer::wrtViewRec1(short x1,short x2,TView* p,int shadowCounter)
 {
  while (1)
   {
    p=p->next;
    if(p==wrtVars2.target)
     { // alle durch
      // рисуем!
      if(p->owner->buffer)
       {
        TMouse::hide(); // !!! Mouse: Cursor must be off if the buffer is the physical screen.
        if(shadowCounter==0)
          memmove(p->owner->buffer+((p->owner->size.x*wrtVars2.y)+x1),
            (ushort*)wrtVars1.buf+(x1-wrtVars2.offset),(x2-x1 )*2);
        else
         { // paint with shadowAttr
          ushort *src=(ushort*)wrtVars1.buf + (x1-wrtVars2.offset);
          ushort *dst=p->owner->buffer+((p->owner->size.x*wrtVars2.y)+x1);
          int l=(x2-x1);
          while (l--) *dst++ =(ushort)(((*src++) & 0xFF)|(shadowAttr << 8));
         }
        TMouse::show();
       }
      if(p->owner->lockFlag==0) wrtViewRec2(x1,x2,p->owner,shadowCounter);
      return ; // (p->owner->lockFlag==0);
     }
    if(!(p->state & sfVisible) || wrtVars2.y<p->origin.y) continue; // keine Verdeckung

    if(wrtVars2.y<p->origin.y+p->size.y)
     {
      if(x1<p->origin.x)
       { // fДngt links vom Object an.
        if (x2<=p->origin.x) continue;
        wrtViewRec1( x1,(short)(p->origin.x), p, shadowCounter );
        x1=(short)(p->origin.x);
       }
      if(x2<=p->origin.x+p->size.x ) return;
      if(x1<p->origin.x+p->size.x ) x1=(short)(p->origin.x+p->size.x);
      if((p->state & sfShadow) && (wrtVars2.y>=p->origin.y+shadowSize.y))
       {
        if(x1>=p->origin.x+p->size.x+shadowSize.x) continue;
        else
         {
          shadowCounter++;
          if (x2<=p->origin.x+p->size.x+shadowSize.x)
            continue;
          else
           {
            wrtViewRec1( x1,(short)(p->origin.x+p->size.x+shadowSize.x), p, shadowCounter );
            x1=(short)(p->origin.x+p->size.x+shadowSize.x);
            shadowCounter--;
            continue;
           }
         }
       }
      else
       {
        continue;
       }
     }
    if( (p->state & sfShadow) && (wrtVars2.y < p->origin.y+p->size.y+shadowSize.y) )
     {
      if(x1<p->origin.x+shadowSize.x)
       {
        if (x2<= p->origin.x+shadowSize.x) continue; // links vorbei
        wrtViewRec1(x1,(short)(p->origin.x+shadowSize.x),p,shadowCounter );
        x1 =(short)(p->origin.x+shadowSize.x);
       };
      if (x1>=p->origin.x+shadowSize.x+p->size.x) continue;
      shadowCounter++;
      if (x2<=p->origin.x+p->size.x+shadowSize.x)
       {
        continue;
       }
      else
       {
        wrtViewRec1(x1,(short)(p->origin.x+p->size.x+shadowSize.x), p, shadowCounter );
        x1=(short)(p->origin.x+p->size.x+shadowSize.x);
        shadowCounter--;
        continue;
       }
     }
    else
     {
      continue;
     }
   }
 }

#define cpPanelTitle "\x07\x08\x09\x09\x22\x26"

TPanelTitle::TPanelTitle( const TRect& bounds, const char *aText) :
    TView(bounds),
//    link( aLink ),
    light(False)
 {
  state|=sfExposed|sfVisible;
  options|=ofSelectable|ofBuffered|ofFirstClick|ofPreProcess|ofPostProcess;//|ofTopSelect
  eventMask|=(ushort)evBroadcast;
//    text(newStr(aText)),
  text=new char[strlen(aText)+1];
  if(strlen(aText)>size.x)
   {
    strncpy((char*)text,aText,size.x);
    text[size.x]=0;
   }
  else
   strcpy((char*)text,aText);
 }

TPanelTitle::~TPanelTitle()
 {
  delete (char *)text;
//  link = 0;
 }

TPalette& TPanelTitle::getPalette() const
 {
  static TPalette palette(cpPanelTitle,sizeof(cpPanelTitle)-1 );
  return palette;
 }

void TPanelTitle::getText( char *s )
 {
  if(text==0) *s=EOS;
  else        strcpy(s,text);
 }

void TPanelTitle::draw()
 {
  ushort* b=((TNotebook*)((TPanel*)owner)->owner)->buffer;

  if(( ((TPanel*)owner)->owner!=0)&&(b!=0))
   {
    ushort color;
    TBuffer BufClass;

    if(((TNotebook*)((TPanel*)owner)->owner)->sel!=((TPanel*)owner)->number)
     color= getColor(0x0301);
    else
     if(light)
      color=getColor(0x0504);
     else
      color=getColor(0x0402);

    //BufClass.movChar((ushort)(((TPanel*)owner)->size.x+TView::origin.x),' ',color,(ushort)size.x,b);
    if(text!=0)
     if(((TNotebook*)((TPanel*)owner)->owner)->sel==((TPanel*)owner)->number)
      BufClass.movCStr((ushort)(((TPanel*)owner)->size.x+TView::origin.x+1),text,color,b);
     else
      BufClass.movCStr((ushort)(((TPanel*)owner)->size.x+TView::origin.x+2),text,color,b);
   }
 }

void TPanelTitle::handleEvent( TEvent& event )
 {
  TView::handleEvent(event);
  if( event.what == evMouseDown )
   {
    //if( link && (link->options & ofSelectable) && !(link->state & sfDisabled) )
    //    link->select();
    clearEvent(event );
   }
  else
   if( event.what == evKeyDown )
    {
     char c=hotKey( text );
     if( getAltCode(c)==event.keyDown.keyCode ||
             ( c != 0 && owner->phase==TGroup::phPostProcess &&
             toupper(event.keyDown.charScan.charCode) ==  c ) )
      {
       //if( link && (link->options & ofSelectable) && !(link->state & sfDisabled) )
       //    link->select();
       clearEvent( event );
      }
    }
   else
    if( event.what == evBroadcast &&
          ( event.message.command == cmReceivedFocus ||
            event.message.command == cmReleasedFocus ) )
     {
      light=Boolean( (state & sfFocused) != 0 );
      ((TNotebook*)((TPanel*)owner)->owner)->drawView();
      //((TPanel*)owner)->draw();
      //drawView();
      clearEvent( event );
     }
 }

// File dialog flags
const
    ffOpen        = 0x0001,
    ffSaveAs      = 0x0002;

const
    cmOpenDialogOpen    = 100,
    cmOpenDialogReplace = 101;

static void near squeezes( char *path )
 {
  char *
  dest = path;
  char *
  src = path;
  while( *src != 0 )
   {
    if(*src == '.')
      if(src[1] == '.')
       {
        src += 2;
        if(dest > path)
         {
          dest--;
          while ((*--dest != '\\')&&(dest > path)) // back up to the previous '\'
            ;
          dest++;         // move to the next position
         }
       }
      else
       if (src[1] == '\\') src++;
       else                *dest++ = *src++;
    else
      *dest++ = *src++;
   }
  *dest = EOS;                // zero terminator
  dest = path;
  src = path;
  while( *src != 0 )
   {
    if ((*src == '\\')&&(src[1] == '\\')) src++;
    else                                  *dest++ = *src++;
   }
  *dest = EOS;                // zero terminator
 }

// fexpands: reimplementation of pascal's FExpand routine.  Takes a
//           relative DOS path and makes an absolute path of the form
//
//               drive:\[subdir\ ...]filename.ext
//
//           works with '/' or '\' as the subdir separator on input;
//           changes all to '\' on output.

void fexpands( char *rpath )
 {
  char path[MAXPATH];
  char drive[MAXDRIVE];
  char dir[MAXDIR];
  char file[MAXFILE];
  char ext[MAXEXT];

  int flags = fnsplit( rpath, drive, dir, file, ext );
  if( (flags & DRIVE) == 0 )
   {
    drive[0]=(char)(getdisk() + 'A');
    drive[1]= ':';
    drive[2]= '\0';
   }
  drive[0]=(char)toupper(drive[0]);
  if( (flags & DIRECTORY) == 0 || (dir[0] != '\\' && dir[0] != '/') )
   {
    char curdir[MAXDIR];
    getcurdir( drive[0] - 'A' + 1, curdir );
    // ++ V.Timonin : better more than nothing
    int
    len = strlen(curdir);
    if (curdir[len - 1] != '\\')
     {
      curdir[len] = '\\';
      curdir[len + 1] = EOS;
     }
    // -- V.Timonin
    strcat( curdir, dir );
    if( *curdir != '\\' && *curdir != '/' )
     {
      *dir = '\\';
      strcpy( dir+1, curdir );
     }
    else
        strcpy( dir, curdir );
   }

  //++ V.Timonin - squeeze must be after '/' --> '\\'
  char *p = dir;
  while( (p = strchr( p, '/' )) != 0 )
      *p = '\\';
  squeezes( dir );
  //-- V.Timonin
  fnmerge( path, drive, dir, file, ext );
#ifdef __MSDOS__
  strupr( path );
#endif
  strcpy( rpath, path );
 }

TFileDialogs::TFileDialogs(const char *aWildCard,const char *aTitle,
                        const char *inputName,ushort aOptions,uchar histId):
    TDialog(TRect(15, 1,66,20),aTitle),
    directory(0),
    TWindowInit(TFileDialogs::initFrame)
 {
  options|=ofCentered;
  strcpy(wildCard,aWildCard);

  fileName=new TFileInputLine(TRect( 3, 3,44, 4),MAXPATH+MAXFILE+MAXEXT+1);
  strcpy(fileName->data,wildCard);
  insert(fileName);
  insert(new TLabel(TRect(2,2,3+cstrlen(inputName),3),inputName,fileName) );

  insert(new THistory(TRect(45, 3,48, 4),fileName,histId) );

  TScrollBar *sb=new TScrollBar(TRect( 3,16,34,17));
  insert(sb);
  insert(fileList = new TFileLists(TRect( 3, 6,34,16),sb) );
  insert(new TLabel( TRect( 2, 5, 8, 6),LoadString(IDS_TAB2_FILES,(char*)filesText),fileList ) );

  ushort opt=bfDefault;
  TRect r(35,6,48,8);

  TPalette MyButtonPal(cpMyButton,sizeof(cpMyButton)-1);
  TButton *btn;
  //if( (aOptions&fdOKButton)!=0 )
  // {
  //  btn=new TButton(r,"O~K~",cmFileOpen,opt);
  //  insert(btn);
  //  opt=bfNormal;
  //  r.a.y+=3; r.b.y+=3;
  // }

  if( (aOptions&fdOpenButton) != 0 )
   {
    btn=new TButton(r,LoadString(ID_OPEN_FILE,"~S~ave"),cmFileOpen,opt);
    insert(btn);
    opt=bfNormal;
    r.a.y+=3; r.b.y+=3;
   }

  if( (aOptions&fdSaveButton)!=0 )
   {
    btn=new TButton(r,LoadString(ID_SAVE_FILE,"~S~ave"),cmFileOpen,opt);
    insert(btn);
    opt=bfNormal;
    r.a.y+=3; r.b.y+=3;
   }

  //if( (aOptions & fdReplaceButton) != 0 )
  // {
  //  insert( new TButton( r, replaceText, cmFileReplace, opt ) );
  //  opt = bfNormal;
  //  r.a.y += 3; r.b.y += 3;
  // }

  //if( (aOptions & fdClearButton) != 0 )
  // {
  //  insert( new TButton( r, clearText, cmFileClear, opt ) );
  //  //opt = bfNormal;
  //  r.a.y += 3; r.b.y += 3;
  // }

  insert(new TButton(r,LoadString(IDS_CANCEL,(char*)cancelText),cmCancel,bfNormal) );
  r.a.y+=3; r.b.y+=3;

  if( (aOptions&fdHelpButton)!=0 )
   {
    insert( new TButton( r, helpText, cmHelp, bfNormal ) );
//    opt = bfNormal;
    r.a.y += 3;
    r.b.y += 3;
   }

  selectNext( False );
  if( (aOptions & fdNoLoadDir) == 0 ) readDirectory();
 }

TFileDialogs::~TFileDialogs()
 {
  //delete (char *)directory;
  //if(fileList!=0) delete fileList;
 }

void TFileDialogs::shutDown()
 {
  fileName=0;
  fileList=0;
  TDialog::shutDown();
 }

void TFileDialogs::handleEvent(TEvent& event)
 {
  TRect  limits;

  TGroup::handleEvent(event);

  switch (event.what)
   {
    case evCommand:
      switch( event.message.command )
       {
        case  cmClose:
         if( (flags & wfClose) != 0 &&
             ( event.message.infoPtr == 0 || event.message.infoPtr == this ) )
          {
           if( (state&sfModal)==0 ) close();
           else
            {
             event.what = evCommand;
             event.message.command = cmCancel;
             putEvent( event );
            }
           clearEvent( event );
          }
         break;
        case cmOK:
        case cmCancel:
        case cmYes:
        case cmNo:
        case cmFileOpen:     //FileDialog
        case cmFileReplace:
        case cmFileClear:
          if( (state & sfModal) != 0 )
           {
            endModal(event.message.command);
            clearEvent(event);
           }
          break;
        default:
          break;
       }
      break;
    case evKeyDown:
      switch (event.keyDown.keyCode)
       {
        case  kbTab:        //win
          selectNext(False);
          clearEvent(event);
          break;
        case  kbShiftTab:
          selectNext(True);
          clearEvent(event);
          break;
        case kbEsc:         //dialog
          event.what = evCommand;
          event.message.command=cmCancel;
          event.message.infoPtr=0;
          putEvent(event);
          clearEvent(event);
          break;
        case kbEnter:
          event.what=evBroadcast;
          event.message.command=cmDefault;
          event.message.infoPtr=0;
          putEvent(event);
          clearEvent(event);
          break;
       }
      break;
    case evBroadcast: //win
      if(event.message.command==cmSelectWindowNum &&
          event.message.infoInt == number && (options & ofSelectable) != 0 )
       {
        select();
        clearEvent(event);
       }
      break;
   }
 }

static Boolean relativePath( const char *path )
 {
  if( path[0] != EOS && (path[0] == '\\' || path[1] == ':') ) return False;
  else                                                        return True;
 }

static void noWildChars( char *dest, const char *src )
 {
  while( *src != EOS )
   {
    if( *src != '?' && *src != '*' ) *dest++ = *src;
    src++;
   }
  *dest = EOS;
 }

#ifdef __MSDOS__
static void trim( char *dest, const char *src )
 {
  while( *src != EOS && isspace(uchar(*src)) ) src++;
  while( *src != EOS && !isspace(uchar(*src)) ) *dest++ = *src++;
  *dest = EOS;
 }
#else
#define trim strcpy
#endif

void TFileDialogs::getFileName(char *s)
 {
  char buf[MAXDRIVE+MAXPATH+MAXFILE+MAXEXT+1];
  char drive[MAXDRIVE];
  char path[MAXDIR];
  char name[MAXFILE];
  char ext[MAXEXT];
  char TName[MAXFILE];
  char TExt[MAXEXT];

  if(fileName!=0)
   trim(buf,fileName->data);
  else buf[0]=0;
  if(relativePath(buf)==True)
   {
    strcpy(buf, directory );
    if(fileName!=0) trim( buf + strlen(buf), fileName->data );
    else            buf[0]=0;
   }
  fexpands( buf );
  fnsplit( buf, drive, path, name, ext );
  if( (name[0]==EOS || ext[0]==EOS) && !isDir( buf ) )
   {
    fnsplit(wildCard, 0, 0, TName, TExt );
    if(name[0]==EOS && ext[0]==EOS ) fnmerge(buf,drive,path,TName,TExt );
    else
     if(name[0]==EOS) fnmerge(buf,drive,path,TName,ext);
     else
      if(ext[0]==EOS)
      {
       if( isWild( name ) ) fnmerge( buf, drive, path, name, TExt );
       else
        {
         fnmerge( buf, drive, path, name, 0 );
         noWildChars( buf + strlen(buf), TExt );
        }
      }
   }
  strcpy( s, buf );
 }

void TFileDialogs::readDirectory()
 {
  if(fileList!=0)
   {
    fileList->readDirectory(wildCard);
    char curDir[MAXPATH];
    getCurDir( curDir );
    delete (char *)directory;
    directory=newStr(curDir);
   }
 }

void TFileDialogs::setData( void *rec )
 {
  TDialog::setData( rec );
  if( *(char *)rec != EOS && isWild( (char *)rec ) )
   {
    valid(cmFileInit);
    if(fileName!=0) fileName->select();
   }
 }

void TFileDialogs::getData(void *rec)
 {
  getFileName((char *)rec);
 }

Boolean TFileDialogs::checkDirectory( const char *str )
 {
  if(pathValid(str) ) return True;
  else
   {
    messageBox(invalidDriveText,mfError|mfOKButton);
    if(fileName!=0) fileName->select();
    return False;
   }
 }

Boolean TFileDialogs::valid(ushort command)
 {
  if( !TDialog::valid( command )) return False;
  if( command==cmValid || command==cmCancel || command==cmFileClear ) return True;

  char fName[MAXDRIVE+MAXPATH+MAXFILE+MAXEXT+1],drive[MAXDRIVE],dir[MAXDIR],name[MAXFILE],ext[MAXEXT];
  getFileName(fName);
  if(isWild(fName))
   {
    fnsplit(fName,drive,dir,name,ext);
    char path[MAXDRIVE+MAXPATH+MAXFILE+MAXEXT+1];
    strcpy(path,drive);
    strcat(path,dir);
    if(checkDirectory(path))
     {
      delete (char *)directory;
      directory = newStr( path );
      strcpy( wildCard, name );
      strcat( wildCard, ext );
      if(fileList!=0)
       {
      if( command != cmFileInit ) fileList->select();
      fileList->readDirectory( directory, wildCard );
       }
     }
    return False;
   }
  if( isDir(fName) )
   {
    if ( checkDirectory(fName) )
     {
      delete (char *)directory;
      strcat( fName, "\\" );
      directory = newStr( fName );
      if(fileList!=0)
       {
        if( command != cmFileInit ) fileList->select();
        fileList->readDirectory( directory, wildCard );
       }
     }
    return False;
   }
  if( validFileName(fName) ) return True;

  messageBox( invalidFileText,mfError|mfOKButton);
  return False;
 }

const char * near TFileDialogs::filesText="~F~iles";
//const char * near TFileDialogs::openText="~O~pen";
//const char * near TFileDialogs::okText="O~K~";
//const char * near TFileDialogs::replaceText = "~R~eplace";
//const char * near TFileDialogs::clearText = "~C~lear";
const char * near TFileDialogs::cancelText = "Cancel";
const char * near TFileDialogs::helpText = "~H~elp";
const char * near TFileDialogs::invalidDriveText="Invalid drive or directory";
const char * near TFileDialogs::invalidFileText ="Invalid file name.";

//------------- TFileLists ---------------
static Boolean equal( const char *s1, const char *s2, ushort count)
 {
  return Boolean( strnicmp( s1, s2, count ) == 0 );
 }

TFileLists::TFileLists(const TRect& bounds,TScrollBar *aScrollBar) :
    TListViewer(bounds,2, 0, aScrollBar),
    items( 0 ),
    searchPos(-1),
    shiftState(0)
 {
  setRange(0);
  showCursor();
  setCursor(1, 0);
 }

TFileLists::~TFileLists()
 {
  if(list()) destroy (list());
 }

void TFileLists::focusItem(short item)
 {
  TListViewer::focusItem( item );
  message(owner,evBroadcast,cmFileFocused,list()->at(item));
 }

void TFileLists::handleEvent( TEvent & event )
 {
  if( event.what == evMouseDown && event.mouse.doubleClick )
   {
    event.what=evCommand;
    event.message.command=cmOK;
    putEvent( event );
    clearEvent( event );
   }
  else //TSortedListBoxs::handleEvent( event );
   {
    char curString[MAXDRIVE+MAXPATH+MAXFILE+MAXEXT+1],newString[MAXDRIVE+MAXPATH+MAXFILE+MAXEXT+1];
    void* k;
    int value, oldPos, oldValue;

    oldValue = focused;
    TListViewer::handleEvent( event );
    if( oldValue != focused ) searchPos = USHRT_MAX;
    if( event.what == evKeyDown )
     {
      if( event.keyDown.charScan.charCode != 0 )
       {
        value = focused;
        if( value < range ) getText(curString,(short)value,255 );
        else                *curString = EOS;
        oldPos = searchPos;
        if(event.keyDown.keyCode == kbBack )
         {
          if(searchPos == USHRT_MAX ) return;
          searchPos--;
          if(searchPos==USHRT_MAX) shiftState = getShiftState();
          curString[searchPos] = EOS;
         }
        else
         if( (event.keyDown.charScan.charCode == '.') )
          {
           char *loc = strchr( curString, '.' );
           if( loc == 0 ) searchPos = USHRT_MAX;
           else           searchPos = ushort(loc - curString);
          }
         else
          {
           searchPos++;
           if( searchPos == 0 ) shiftState = getShiftState();
           curString[searchPos] = event.keyDown.charScan.charCode;
           curString[searchPos+1] = EOS;
          }
        k = getKey(curString);
        list()->search( k, value );
        if( value < range )
         {
          getText( newString,(short)value, 255 );
          if( equal( curString, newString,(ushort)(searchPos+1) ) )
           {
            if( value != oldValue )
             {
              focusItem((short)value);
              setCursor((ushort)(cursor.x+searchPos),(ushort)(cursor.y) );
             }
            else
              setCursor((ushort)(cursor.x+(searchPos-oldPos)),(ushort)(cursor.y));
           }
          else
            searchPos =(ushort)oldPos;
         }
        else
          searchPos=(ushort)oldPos;
        if(searchPos!=oldPos || isalpha(event.keyDown.charScan.charCode) )
          clearEvent(event);
       }
     }
   }
 }

size_t TFileLists::dataSize()
 {
  //return sizeof(TListBoxRecs);
  return 0;
 }

void TFileLists::getData( void *)// rec )
 {
//  TListBoxRecs *p = (TListBoxRecs *)rec;
//  p->items = items;
//  p->selection = focused;
 }

void TFileLists::setData( void *)//rec )
 {
//  TListBoxRecs *p = (TListBoxRecs *)rec;
//  newList(p->items);
//  focusItem(p->selection);
//  drawView();
 }

//void TFileLists::getText( char *dest, short item, short maxChars )
// {
//  if(items != 0 )
//   {
//    strncpy( dest, (const char *)(items->at(item)), maxChars );
//    dest[maxChars] = '\0';
//   }
//  else *dest = EOS;
// }

void TFileLists::getText( char *dest, short item, short maxChars )
 {
  TSearchRec *f=(TSearchRec *)(list()->at(item));

  strncpy(dest,f->name, maxChars );
  dest[maxChars]='\0';
  if( f->attr & FA_DIREC ) strcat( dest, "\\" );
 }

//void* TFileLists::getKey( const char *s )
// { return (void *)s; }
void* TFileLists::getKey( const char *s )
 {
  static TSearchRec sR;

  if( (getShiftState() & 0x03) != 0 || *s == '.' ) sR.attr = FA_DIREC;
  else                                             sR.attr = 0;
  strcpy(sR.name,s);
  strupr(sR.name);
  return &sR;
 }

void TFileLists::newList(TFileCollection *aList )
 {
  destroy( items );
  items = aList;
  if( aList != 0 ) setRange((ushort)(aList->getCount()) );
  else             setRange(0);
  if(range > 0 ) focusItem(0);
  drawView();

  searchPos = -1;
 }

void TFileLists::readDirectory( const char *dir, const char *wildCard )
 {
  char path[MAXDRIVE+MAXPATH+MAXFILE+MAXEXT+1];
  strcpy( path, dir );
  strcat( path, wildCard );
  readDirectory(path);
 }

struct DirSearchRecs : public TSearchRec
 {
  void *operator new( size_t );
 };

void *DirSearchRecs::operator new( size_t sz )
 {
  void *temp = ::operator new( sz );
  if( TVMemMgr::safetyPoolExhausted() )
   {
    delete temp;
    temp = 0;
   }
  return temp;
 }

void TFileLists::readDirectory( const char *aWildCard )
 {
  ffblk s;

  char path[MAXDRIVE+MAXPATH+MAXFILE+MAXEXT+1];
  char drive[MAXDRIVE];
  char dir[MAXDIR];
  char file[MAXFILE];
  char ext[MAXEXT];
  int res;
  DirSearchRecs *p;

  const unsigned findAttr = FA_RDONLY | FA_ARCH;

  TFileCollection *fileList = new TFileCollection(15,15);

  strcpy( path, aWildCard );
  fexpands( path );
  fnsplit( path, drive, dir, file, ext );

  res=findfirst(aWildCard, &s, findAttr );
  p=(DirSearchRecs *)&p;
  while( p != 0 && res == 0 )
   {
    if( (s.ff_attrib & FA_DIREC)==0)
     {
      p=new DirSearchRecs;
      if( p!=0)
       {
        p->attr=s.ff_attrib;
        p->time=s.ff_ftime + (((long)s.ff_fdate) << 16);
        p->size=s.ff_fsize;
        strcpy(p->name,s.ff_name);
        fileList->insert(p);
       }
     }
    res=findnext(&s);
   }
  findclose(&s);

  fnmerge(path,drive,dir,"*",".*");

  int  upattr=FA_DIREC;
  long uptime=0x210000uL;
  long upsize=0;

  res = findfirst( path, &s, FA_DIREC );
  while( p != 0 && res == 0 )
   {
    if((s.ff_attrib & FA_DIREC)!=0)
     {
      if(s.ff_name[0]!='.')
       {
        p = new DirSearchRecs;
        if ( p != 0 )
         {
          p->attr =s.ff_attrib;
          p->time =s.ff_ftime+ (((long)s.ff_fdate) << 16);
          p->size =s.ff_fsize;
          strcpy(p->name,s.ff_name);
          fileList->insert(p);
         }
       }
      else
       if ( strcmp(s.ff_name,"..") == 0 )
        {
         upattr = s.ff_attrib;
         uptime = s.ff_ftime + (((long)s.ff_fdate) << 16);
         upsize = s.ff_fsize;
        }
     }
    res = findnext( &s );
   }
  findclose( &s );
  if ( strlen( dir ) > 4 )
   {
    p = new DirSearchRecs;
    if ( p != 0 )
     {
      p->attr=(char)upattr;
      p->time = uptime;
      p->size = upsize;
      strcpy( p->name, ".." );
      fileList->insert( p );
     }
   }
  if( p == 0 ) messageBox( tooManyFiles, mfOKButton | mfWarning );
  newList(fileList);
  if( list()->getCount() > 0 )
      message( owner, evBroadcast, cmFileFocused, list()->at(0) );
  else
   {
    static DirSearchRecs noFile;
    message( owner, evBroadcast, cmFileFocused, &noFile );
   }
 }

const char * near TFileLists::tooManyFiles = "Too many files.";

const
   cdNormal     = 0x0000, // Option to use dialog immediately
   cdNoLoadDir  = 0x0001, // Option to init the dialog to store on a stream
   cdHelpButton = 0x0002; // Put a help button in the dialog

TChDirDialogs::TChDirDialogs(char* tName,ushort opts, ushort histId ) :
    TDialog( TRect(16, 2,66,20),tName),
    TWindowInit( TChDirDialogs::initFrame )
 {
  options|=ofCentered;

  dirInput=new TInputLine( TRect(3,3,43,4 ),261);
  insert( dirInput );
  insert( new TLabel( TRect( 2, 2,19, 3),LoadString(ID_DIRECTORY_NAME,(char*)dirNameText),dirInput ));
  insert( new THistory( TRect(44, 3,47, 4),dirInput,histId) );

  TScrollBar *sb = new TScrollBar( TRect(32, 6,33,16) );
  insert( sb );
  dirList=new TDirListBoxs(TRect( 3, 6,32,16), sb );
  insert(dirList);
  insert(new TLabel(TRect(2,5,21,6),LoadString(ID_DIRECTORY_TREE,(char*)dirTreeText), dirList) );

  okButton = new TButton(TRect(35, 6,47, 8),okText,cmOK,bfDefault);
  insert( okButton );
  insert(new TButton(TRect(35, 9,47,11),LoadString(ID_CANCEL,(char*)cancelText),cmCancel,bfNormal) );
  insert(new TButton(TRect(35,12,47,14),LoadString(ID_REVERT,(char*)revertText),cmRevert,bfNormal) );
  //if( (opts&cdHelpButton) != 0 ) insert( new TButton( TRect(35,15,45,17),helpText,cmHelp,bfNormal) );
  if( (opts&cdNoLoadDir) == 0 ) setUpDialog();
  selectNext( False );
 }

void TChDirDialogs::shutDown()
 {
  dirList = 0;
  dirInput = 0;
  okButton = 0;
  TDialog::shutDown();
 }

void TChDirDialogs::handleEvent( TEvent& event )
 {
  TDialog::handleEvent( event );
  switch( event.what )
   {
    case evCommand:
     {
      char curDir[MAXPATH];
      switch( event.message.command )
       {
        case cmRevert:
          getCurDir(curDir);
          break;
        case cmChangeDir:
         {
          TDirEntry *p=dirList->list()->at( dirList->focused );
          strcpy( curDir, p->dir() );
          if( strcmp( curDir, drivesText ) == 0 ) break;
          else
           {
            char tstDrv[]=" :\\";
            tstDrv[0]=curDir[0];
            if((curDir[0]<'C')&&(TestPresentDisk((UCHAR)(curDir[0]-'A'),tstDrv)))
             {
              if(curDir[strlen(curDir)-1]!='\\') strcat( curDir, "\\" );
             }
            else
             if((curDir[0]>='C')&&driveValid(curDir[0]) )
              {
               if(curDir[strlen(curDir)-1]!='\\') strcat( curDir, "\\" );
              }
             else return;
           }
          break;
         }
        default: return;
       }
      dirList->newDirectory(curDir);
      int len = strlen( curDir );
      if( len > 3 && curDir[len-1] == '\\' ) curDir[len-1] = EOS;
      strcpy(dirInput->data,curDir);
      dirInput->drawView();
      dirList->select();
      clearEvent( event );
     }
    default:
        break;
   }
 }

size_t TChDirDialogs::dataSize() { return 0; }

void TChDirDialogs::getData(void *rezDir)
 {
  strcpy((char*)rezDir,dirInput->data);
 }

void TChDirDialogs::setData(void *targetDir)
 {
  strcpy(dirInput->data,(char*)targetDir);
  if(dirInput->data[strlen(dirInput->data)-1]=='\\') dirInput->data[strlen(dirInput->data)-1]='0';
  setUpDialog();
 }

void TChDirDialogs::setUpDialog()
 {
  if(dirList != 0)
   {
    char curDir[MAXPATH];
    getCurDir(curDir);
    dirList->newDirectory(curDir);
    if( dirInput != 0 )
     {
      int len = strlen( curDir );
      if( len > 3 && curDir[len-1] == '\\' ) curDir[len-1] = EOS;
      strcpy( dirInput->data, curDir );
      dirInput->drawView();
     }
   }
 }

Boolean TChDirDialogs::valid( ushort command )
 {
  if( command != cmOK ) return True;

  char path[MAXPATH];
  strcpy(path, dirInput->data );
  fexpand( path );

  int len = strlen( path );
  if( len > 3 && path[len-1] == '\\' ) path[len-1] = EOS;

  //if( changeDir( path ) != 0 )
  // {
  //  messageBox( invalidText, mfError | mfOKButton );
  //  return False;
  // }
  return True;
 }

const char * near TChDirDialogs::dirNameText = "Directory ~n~ame";
const char * near TChDirDialogs::dirTreeText = "Directory ~t~ree";
const char * near TChDirDialogs::okText = "O~K~";
const char * near TChDirDialogs::cancelText = "Cancel";
const char * near TChDirDialogs::revertText = "~R~evert";
const char * near TChDirDialogs::helpText = "Help";
const char * near TChDirDialogs::drivesText = "Drives";
const char * near TChDirDialogs::invalidText = "Invalid directory";

TDirListBoxs::TDirListBoxs( const TRect& bounds, TScrollBar *aScrollBar ) :
    TListBox( bounds,1,aScrollBar),cur( 0 )
 {
  options|=ofSelectable|ofBuffered;
  buffer=0;
  *dir = EOS;
 }

TDirListBoxs::~TDirListBoxs()
 {
  if(list() ) destroy(list() );
 }

void TDirListBoxs::getText( char *text, short item, short maxChars )
 {
  strncpy( text, list()->at(item)->text(), maxChars );
  text[maxChars] = '\0';
 }

void TDirListBoxs::draw()
 {
  TBuffer BufClass;
  short i,item;
  ushort normalColor, selectedColor, focusedColor, color;
  uchar scOff;

  if( (state&(sfSelected|sfActive)) == (sfSelected|sfActive))
   {
    normalColor = getColor(1);
    focusedColor = getColor(3);
    selectedColor = getColor(4);
   }
  else
   {
    normalColor = getColor(2);
    selectedColor = getColor(4);
   }

  if(buffer==0) BufClass.getBuffer(&buffer,this);
  if(buffer!=0)
   {
    //lockFlag++;
    BufClass.movChar(0,' ',getColor(0x01),(ushort)(size.x*size.y),buffer); //заполнение всей строчки
    item=(short)topItem;
    for(i=0;i<size.y;i++)
     {
      if(item>=range) goto exdrw;

      if( (state & (sfSelected|sfActive) )==(sfSelected|sfActive) &&
          focused==item && range>0)
       {
        color=focusedColor; scOff=0;
        setCursor(1,i);
       }
      else
       if(item<range && isSelected(item)) { color = selectedColor; scOff = 2; }
       else         { color=normalColor;   scOff=4; }

      char text[256];
      getText(text,item,(ushort)size.x);
      if(strlen(text)>0)
       {
        char *pStr=text;
        while(*pStr) { if(*pStr=='~') *pStr=' '; pStr++; }
       }
      if(strlen(text)<size.x)
        BufClass.movCStr((ushort)(i*size.x),(char *)text,color,buffer);
      else
        BufClass.movNCStr((ushort)(i*size.x),(char*)text,color,(short)(size.x),buffer);
      item++;
     }
exdrw: ;
//    lockFlag--;
    BufClass.wrtBuf(0, 0,(short)size.x,(short)size.y,buffer,this);
   }
 }

void TDirListBoxs::handleEvent( TEvent& event )
 {
  switch (event.what)
   {
    case evKeyDown:
      switch (event.keyDown.keyCode)
       {
        case kbEnter:
          event.what = evCommand;
          event.message.command = cmChangeDir;
          putEvent(event);
          clearEvent(event);
          return;
        }
      break;
    case evMouseDown:
      if(event.mouse.doubleClick)
       {
        event.what = evCommand;
        event.message.command = cmChangeDir;
        putEvent( event );
        clearEvent( event );
        return;
       }
      break;
   }
  TListBox::handleEvent( event );
 }

Boolean TDirListBoxs::isSelected( short item )
 {
  return Boolean( item == cur );
 }

void TDirListBoxs::showDrives( TDirCollection *dirs )
 {
  Boolean isFirst = True;
  char oldc[5],tstDrv[]=" :\\";
  strcpy(oldc,"0:\\");

  //unsigned ulCurDisk,totaldrives;
  //ulCurDisk=_getdrive();
  //_dos_setdrive(ulCurDisk,&totaldrives);
  for( char c = 'A'; c <= 'Z'; c++ )
   {
    tstDrv[0]=c;
//    if(((c<'C')&&(TestPresentDisk((UCHAR)(c-'A'),tstDrv)))|| ((c>='C')&&driveValid(c)) )
    if(TestPresentDisk((UCHAR)(c-'A'),tstDrv))
     {
      if(oldc[0] != '0' )
       {
        char s[16];
        if(isFirst )
         {
          strcpy( s, firstDir );
          s[ strlen(firstDir) ] = oldc[0];
          s[ strlen(firstDir)+1 ] = EOS;
          isFirst = False;
         }
        else
         {
          strcpy( s, middleDir );
          s[ strlen(middleDir) ] = oldc[0];
          s[ strlen(middleDir)+1 ] = EOS;
         }
        dirs->insert( new TDirEntry( s, oldc ) );
       }
      if( c == getdisk() + 'A' ) cur=(ushort)dirs->getCount();
      oldc[0] = c;
     }
   }
  if( oldc[0] != '0' )
   {
    char s[ 16 ];
    strcpy( s, lastDir );
    s[ strlen(lastDir) ] = oldc[0];
    s[ strlen(lastDir)+1 ] = EOS;
    dirs->insert( new TDirEntry( s, oldc ) );
   }
 }

void TDirListBoxs::showDirs( TDirCollection *dirs )
 {
  const indentSize = 2;
  int indent=indentSize;

  char buf[MAXPATH+MAXFILE+MAXEXT+10];
  memset(buf,' ',(ushort)sizeof(buf) );
  char *name =buf+sizeof(buf)-(MAXFILE+MAXEXT);

  char *org=name-strlen(pathDir);
  strcpy(org,pathDir);

  char *curDir = dir;
  char *end = dir + 3;
  char hold = *end;
  *end = EOS;         // mark end of drive name
  strcpy( name, curDir );
  dirs->insert(new TDirEntry( org, name ) );

  *end = hold;        // restore full path
  curDir = end;
  while( (end = strchr( curDir, '\\' )) != 0 )
   {
    *end = EOS;
    strncpy(name, curDir, size_t(end-curDir) );
    name[size_t(end-curDir)] = EOS;
    dirs->insert(new TDirEntry(org-indent,dir));
    *end = '\\';
    curDir = end+1;
    indent += indentSize;
   }

  cur=(ushort)(dirs->getCount()-1);

  //struct stat statbuf;
  //if(stat(dir,&statbuf)==0)
   {
    end = strrchr(dir, '\\' );
    char path[MAXPATH];
    strncpy(path,dir,size_t(end-dir+1) );
    end = path + unsigned(end-dir)+1;
    strcpy( end, "*.*" );

    Boolean isFirst = True;
    ffblk ff;
    int res=findfirst(path,&ff,FA_DIREC);
    while(res==0)
     {
      if( (ff.ff_attrib & FA_DIREC) != 0 && ff.ff_name[0] != '.' )
       {
        if( isFirst )
         {
          memcpy( org, firstDir, strlen(firstDir)+1 );
          isFirst = False;
         }
        else
          memcpy( org, middleDir, strlen(middleDir)+1 );
        strcpy( name, ff.ff_name );
        strcpy( end, ff.ff_name );
        dirs->insert( new TDirEntry( org - indent, path ) );
       }
      res = findnext( &ff );
     }
    findclose( &ff );
   }

  char *p = dirs->at(dirs->getCount()-1)->text();
  char *i = strchr( p, graphics[0] );
  if( i == 0 )
   {
    i = strchr( p, graphics[1] );
    if( i != 0 ) *i = graphics[0];
   }
  else
   {
    *(i+1) = graphics[2];
    *(i+2) = graphics[2];
   }
 }

void TDirListBoxs::newDirectory(const char *str )
 {
  strcpy( dir, str );
  TDirCollection *dirs = new TDirCollection(15,10);
  dirs->insert( new TDirEntry( drives, drives ) );
  if( strcmp( dir, drives ) == 0 )
   showDrives( dirs );
  else
   showDirs( dirs );
  newList( dirs );
  focusItem( cur );
 }

void TDirListBoxs::setState( ushort nState, Boolean enable )
 {
  TListBox::setState( nState, enable );
//  if( (nState & sfFocused) && ((TChDirDialogs *)owner)->chDirButton )
//      ((TChDirDialogs *)owner)->chDirButton->makeDefault( enable );
 }

const char * near TDirListBoxs::pathDir  ="└─┬";
const char * near TDirListBoxs::firstDir =  "└┬─";
const char * near TDirListBoxs::middleDir=  " ├─";
const char * near TDirListBoxs::lastDir  =  " └─";
const char * near TDirListBoxs::drives   ="Drives";
const char * near TDirListBoxs::graphics ="└├─";

