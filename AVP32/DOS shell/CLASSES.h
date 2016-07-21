#ifndef __CLASSES_H__
#define __CLASSES_H__

#ifndef Uses_TIndicator
#define Uses_TIndicator
#endif

#define cpMyColor \
  "\x77\x70\x78\x74\x20\x28\x24\x17\x1F\x1A\x31\x31\x1E\x71\x00" \
  "\x37\x3F\x3A\x13\x13\x3E\x21\x00\x70\x7F\x7A\x13\x13\x70\x7F\x00" \
  "\x70\x7F\x7A\x13\x13\x70\x70\x7F\x7E\x20\x2B\x2F\x78\x2E\x70\x30" \
  "\x3f\x7e\x1f\x2f\x1A\x20\x72\x31\x31\x30\x2F\x3E\x31\x13\x00\x00"

#define cpMyButton "\x0a\x0b\x0c\x0d\x0e\x0e\x0e\x01"

// ------------ start Notebook -------------------------------

#ifdef __TNotebook

//class far TRect;
//class far TSItem;
//class far TEvent;
//class far TPoint;
//class far TStringCollection;

#define MAXPAGENAMELEN 20
#define MAXPAGE    10
typedef struct _NotPages
 {
  char Name[MAXPAGENAMELEN];
  TView* Page;
 } TNotPages;

class TNotebook:public TGroup
 {
public:
  TNotebook(const TRect& bounds);
  ~TNotebook();
  void insertPage(TView* p,char* Name);
  virtual void setState( ushort aState, Boolean enable );
  virtual void draw();
  virtual void handleEvent( TEvent& event );
  ushort getHelpCtx();
  virtual Boolean mark( int item );
  virtual void press( int item );
  virtual void movedTo( int item );
  virtual size_t dataSize();
  virtual void getData( void *rec );
  virtual void setData( void *rec );

  int sel;
  ushort topPage;
  TNotPages NotPages[MAXPAGE];
  unsigned short usTabX,usTabY;

protected:
  ushort value;

private:
//  int column( int item );
  int findSel( TPoint p );
//  int row( int item );
 };

//#define loByte(w)    (((uchar *)&w)[0])
//#define hiByte(w)    (((uchar *)&w)[1])

//inline void TNotebook::putsAttr(ushort indent,ushort attr,ushort far *data)
// {
//  hiByte(data[indent]) = uchar(attr);
// }

//inline void TNotebook::putsChar(ushort indent,ushort c,ushort far *data)
// {
//  loByte(data[indent]) = uchar(c);
// }

#endif  // Uses_TNotebook

// ------------ end Notebook -------------------------------
#ifdef __TPanel
class TPanelTitle : public TView
 {
public:
  TPanelTitle( const TRect& bounds, const char *aText);
  ~TPanelTitle();
  //virtual void shutDown();
  virtual void getText( char * );
  virtual TPalette& getPalette() const;
  virtual void draw();
  virtual void handleEvent( TEvent& event );

protected:
  //TView *link;
//  const char *text;
  char *text;
  Boolean light;
 };

class TPanel: public TGroup//, public virtual TWindowInit
 {
public:
  TPanel(const TRect& bounds,const char *aTitle,short aNumber);
  ~TPanel();
  virtual void shutDown();
  virtual void close();

  virtual TPalette& getPalette() const;
  virtual void draw();
  virtual void handleEvent( TEvent& event );
  virtual void setState( ushort aState, Boolean enable );
  virtual void sizeLimits( TPoint& min, TPoint& max );

  TPanelTitle *PanelTitle;
  short number;
  uchar flags;

private:
//  static const char near initFrame[19];
  static const char * near shadows;
  static char near frameChars[33];
 };
#endif  // Uses_TPanel

//------------ TCheckBoxes -------------------
#ifdef __TCheckBox

//#define MAXCHBTNNAMELEN 30
#define MAXCHBTN    20
typedef struct _ChBtn
 {
//  char    Name[MAXCHBTNNAMELEN];
  char    *Name;
  ushort  id;
  Boolean flEnable;
  ushort  EditDelta;
 } TChBtn;

class TCheckBox: public TView
 {
public:
  TCheckBox(const TRect& bounds,int frames,ushort delta);
  ~TCheckBox();

  void insertChBtn(char* Name,ushort id,Boolean flEnable,ushort EditDelta);
  virtual void setState(ushort aState, Boolean enable);
  virtual TPalette& getPalette() const;
  void draw();
  virtual void handleEvent( TEvent& event );
  ushort getHelpCtx();
  virtual Boolean mark( int item );
  virtual void press( int item );
  virtual void movedTo( int item );
  virtual size_t dataSize();
  virtual void getData( void *rec );
  virtual void setData( void *rec );

  ushort topChBtn;
  TChBtn ChBtn[MAXCHBTN];
  int    flFrames;
  ushort Delta;
  ushort far *buffer;

protected:
  ushort value;
  int sel;

private:
  int findSel(TPoint p);
 };
#endif  // Uses_TCheckBox

#ifdef __TRadioBattons
//#define MAXRBBTNNAMELEN 30
#define MAXRBBTN    20
typedef struct _RbBtn
 {
//  char    Name[MAXRBBTNNAMELEN];
  char    *Name;
  ushort  id;
  Boolean flEnable;
 } TRbBtn;

class TRadioBattons:public TView
 {
public:
  TRadioBattons( const TRect& bounds,ushort delta);
  ~TRadioBattons();

  void insertRbBtn(char* Name,ushort id,Boolean flEnable);
  virtual void setState( ushort aState, Boolean enable);
  virtual TPalette& getPalette() const;
  virtual void draw();
  virtual void handleEvent( TEvent& event );
  ushort getHelpCtx();
  virtual size_t dataSize();
  virtual void getData( void *rec );
  virtual void setData( void *rec );
  virtual Boolean mark( int item );
  virtual void press( int item );
  virtual void movedTo( int item );

  ushort topRbBtn;
  TRbBtn RbBtn[MAXRBBTN];
//  int    flFrames;
  ushort Delta;
  ushort far *buffer;

protected:
  ushort value;
  int sel;

private:
  int findSel( TPoint p );
 };
#endif  // Uses_TRadioBattons

#ifdef __TListViews
#ifndef _PLIST_
#define _PLIST_
typedef struct _List
 {
  CHAR    szName[CCHMAXPATH];// Found file's path & name
  LONG    szType;
  BYTE    Selected;
  struct  _List  *pNext;     // Pointer of next
 } LIST;
typedef LIST *PLIST;
#endif

//#define MAXLISTITEMNAMELEN 40
//#define MAXLISTITEMS    20
//typedef struct _ListItem
// {
//  char   diskName[MAXLISTITEMNAMELEN];
//  char   flSelection;
//  ushort type;
// } TListItem;

class TListViews : public TView
 {
public:
  TListViews(const TRect& bounds,TScrollBar *aHScrollBar,short MaxLines); //,TScrollBar *aVScrollBar

  void insertItem(char* Name,char flSelection);
  void deleteItem(char* Name);
  void deleteList(void);
  virtual void changeBounds( const TRect& bounds );
  virtual TPalette& getPalette() const;
  virtual void getText(char *dest,short item,short maxLen);
  virtual void draw();
  virtual void setState( ushort aState, Boolean enable );
  virtual void handleEvent( TEvent& event );
  virtual void focusItem(short item);
  virtual void focusItemNum(short item);
  short findSel(TPoint p);
  virtual void selectItem( short item );
  /*virtual Boolian*/ char isSelected(short item);
  void setRange( short aRange );

  virtual size_t dataSize();
  virtual void getData( void *rec );
  virtual void setData( void *rec );
//  virtual void newList( TCollection *aList );
//  TListItem items[MAXLISTITEMS]; //  TCollection *items;
  PLIST pTail,pListTop;

  virtual void shutDown();

  TScrollBar *hScrollBar;
//  TScrollBar *vScrollBar;
  short  topItem;
  short  focused;
  short  range;
  short  maxLines;
  ushort far *lvBuffer;
 };

#endif  // Uses_TListViews

#ifdef __TBuffer
class TBuffer
 {
  //friend class TSystemError;
  //friend class TView;
  //friend void genRefs();
  //friend class TFrame;

public:
  void getBuffer(ushort far **data,TView *v);
  void putsAttr(ushort indent,ushort attr,ushort far *data);
  void putsChar(ushort indent,ushort c,ushort far *data);
  void movChar(ushort indent,char c,ushort attr,ushort count,ushort far *data);
  //void moveStr( ushort indent, const char far *str, ushort attrs );
  void movCStr(ushort indent, const char far *str, ushort attrs,ushort far *data);
  void movNCStr(ushort indent,const char far *str,ushort attrs,ushort chars,ushort far *usData);
  //void moveBuf( ushort indent, const void far *source,ushort attr, ushort count );
//  void wrtLine(short x,short y,short w,short h,ushort far *b,TView *v);
  void wrtBuf(short x,short y,short w,short h,ushort far *b,TView *v);

protected:
  ushort *data;

private:
  void wrtViewRec1(short x1,short x2,TView* p,int shadowCounter);
  void wrtViewRec2(short x1,short x2,TView* p,int shadowCounter);
  void wrtView(short x1,short x2,short y,ushort far *buf,TView *v);

 };

#define loByte(w)    (((uchar *)&w)[0])
#define hiByte(w)    (((uchar *)&w)[1])

inline void TBuffer::putsAttr(ushort indent,ushort attr,ushort far *data)
 {
  hiByte(data[indent]) = uchar(attr);
 }

inline void TBuffer::putsChar(ushort indent,ushort c,ushort far *data)
 {
  loByte(data[indent]) = uchar(c);
 }

#endif  // Uses_TBuffer

#ifdef __TFile
class TFileLists : public TListViewer//TSortedListBoxs
 {
public:
  TFileLists(const TRect& bounds,TScrollBar *aScrollBar);
  ~TFileLists();

  virtual void focusItem( short item );
  virtual void handleEvent( TEvent& event );
  void readDirectory( const char *dir, const char *wildCard );
  void readDirectory( const char *wildCard );

  virtual size_t dataSize();
  virtual void getData( void *rec );
  virtual void setData( void *rec );
  virtual void getText( char *dest, short item, short maxLen );

  virtual void newList(TFileCollection *aList);
  TFileCollection *list();

protected:
  uchar shiftState;

private:
  virtual void *getKey( const char *s );
  static const char * near tooManyFiles;

  ushort searchPos;
  TFileCollection *items;
 };

inline TFileCollection *TFileLists::list()
 {
  return (TFileCollection *)items;;
 }

const fdSaveButton    = 0x0020;      // Put an Save button in the dialog
class TFileDialogs : public TDialog
 {
public:
  TFileDialogs( const char *aWildCard, const char *aTitle,
               const char *inputName, ushort aOptions, uchar histId );
  ~TFileDialogs();
  virtual void shutDown();

  virtual void handleEvent( TEvent& event );
  virtual void setData( void *rec );
  virtual void getData( void *rec );
  void getFileName( char *s );
  void readDirectory();
  virtual Boolean valid(ushort command);

  TFileInputLine *fileName;
  TFileLists *fileList;
  char wildCard[MAXPATH];
  const char *directory;

private:

  Boolean checkDirectory( const char * );

  static const char * near filesText;
  static const char * near openText;
  static const char * near okText;
//  static const char * near replaceText;
//  static const char * near clearText;
  static const char * near cancelText;
  static const char * near helpText;
  static const char * near invalidDriveText;
  static const char * near invalidFileText;
 };

#endif  // Uses_TFile

#ifdef __TDirListBoxs
class TDirListBoxs : public TListBox
 {
public:
  TDirListBoxs( const TRect& bounds, TScrollBar *aScrollBar );
  ~TDirListBoxs();

  virtual void getText( char *, short, short );
  virtual void draw();
  virtual void setState( ushort aState, Boolean enable );
  virtual void handleEvent( TEvent& );
  virtual Boolean isSelected( short );
  void newDirectory( const char * );

  TDirCollection *list();

private:
  void showDrives( TDirCollection * );
  void showDirs( TDirCollection * );

  char dir[MAXDRIVE+MAXPATH+MAXFILE+MAXEXT+1];
  ushort cur;
  ushort far *buffer;

  static const char * near pathDir;
  static const char * near firstDir;
  static const char * near middleDir;
  static const char * near lastDir;
  static const char * near drives;
  static const char * near graphics;
 };

inline TDirCollection *TDirListBoxs::list()
 {
  return (TDirCollection *)TListBox::list();
 }

class TChDirDialogs : public TDialog
 {
public:
  friend class TDirListBoxs;

  TChDirDialogs(char* tName,ushort aOptions, ushort histId );
  virtual size_t dataSize();
  virtual void getData(void *rezDir);
  virtual void handleEvent( TEvent& );
  virtual void setData(void *targetDir);
  virtual Boolean valid(ushort);
  virtual void shutDown();

private:
  void setUpDialog();

  TInputLine *dirInput;
  TDirListBoxs *dirList;
  TButton *okButton;

  static const char * near dirNameText;
  static const char * near dirTreeText;
  static const char * near okText;
  static const char * near cancelText;
  static const char * near revertText;
  static const char * near helpText;
  static const char * near drivesText;
  static const char * near invalidText;
 };

#endif  // Uses_TDirListBoxs

#ifdef __TViewer
class TViewer : public TView
 {
public:
  friend void genRefs();

  TViewer(const TRect&,TScrollBar *,TScrollBar *,TIndicator *,ULONG aBufSize,const char *FileName);
  virtual ~TViewer();
  virtual void shutDown();

  //Boolean insertBuffer( char *,ULONG,ULONG, Boolean, Boolean );
  //Boolean insertText( const void *,ULONG, Boolean );
  //virtual Boolean insertFrom( TViewer * );
  virtual void changeBounds( const TRect& );
  virtual void convertEvent( TEvent& );
  virtual void handleEvent( TEvent& );
  void lock();
  void unlock();
  virtual TPalette& getPalette() const;
  virtual void draw();
  void drawLines( int, int,ULONG );
  //Boolean search( const char *, ushort );
  void setCmdState( ushort, Boolean );
  virtual void setState( ushort, Boolean );
  void update( uchar );
  void doUpdate();
  virtual void updateCommands();
  virtual Boolean valid(ushort command);

  //void deleteRange(ULONG,ULONG, Boolean );
  //void doSearchReplace();
  void formatLine(void *,ULONG, int, ushort );
  //void find();
  ULONG lineEnd(ULONG);
  ULONG lineMove(ULONG, int);
  ULONG lineStart(ULONG);
  void newLine();
  ULONG prevLine(ULONG);
  ULONG nextLine(ULONG);
  ULONG prevWord(ULONG);
  ULONG nextWord(ULONG);
  ULONG prevChar(ULONG);
  ULONG nextChar(ULONG);
  //void replace();
  ULONG getMousePtr(TPoint);
  void setCurPtr(ULONG,uchar );
  Boolean cursorVisible();
  void trackCursor( Boolean );
  void toggleInsMode();

  TScrollBar *hScrollBar;
  TScrollBar *vScrollBar;
  void scrollTo( int, int );
  void checkScrollBar( const TEvent&, TScrollBar *, int& );
  TIndicator *indicator;

  void setBufLen(ULONG);
  virtual Boolean getBufSize(ULONG);
  virtual Boolean setBufSize(ULONG);
  virtual void initBuffer();
  virtual void doneBuffer();
  char *frame;  //buffer;
  ULONG bufSize;
  ULONG bufLen;
//ULONG gapLen;
  ULONG curPtr;
  TPoint curPos;
  TPoint delta;
  TPoint limit;
  int drawLine;
  char bufChar(ULONG);
  int charPos(ULONG,ULONG);
  //ULONG bufPtr(ULONG);
  ULONG charPtr(ULONG ,int );
  ULONG drawPtr;
  //ULONG delCount;
  //ULONG insCount;
  Boolean isValid;
  Boolean canUndo;
  //Boolean modified;
  Boolean overwrite;
  Boolean autoIndent;

  static TEditorDialog near editorDialog;
  //static ushort near editorFlags;
  //static char near findStr[maxFindStrLen];
  //static char near replaceStr[maxReplaceStrLen];
  uchar lockCount;
  uchar updateFlags;
  int keyState;

  Boolean selecting;
  //ULONG selStart;
  //ULONG selEnd;
  //Boolean hasSelection();
  //void startSelect();
  void setSelect(ULONG,ULONG, Boolean);
  //void deleteSelect();
  //void hideSelect();
  //void undo();

  //static TViewer * near clipboard;
  //Boolean clipCopy();
  //void clipCut();
  //void clipPaste();
  //Boolean isClipboard();

  int  hFile;
  Boolean loadFromFile();
 };

class TViewWindow : public TWindow
 {
public:
  TViewWindow( const TRect&, const char *, int );
  virtual void close();
  //virtual const char *getTitle( short );
  virtual void handleEvent(TEvent&);
  virtual void sizeLimits(TPoint& min,TPoint& max );

  TViewer *editor;

private:
  //static const char * near clipboardTitle;
  //static const char * near untitled;
 };

#endif  // Uses_TViewer

#endif
