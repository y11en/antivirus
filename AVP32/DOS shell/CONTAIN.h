#ifndef _CONTAIN_H_
#define _CONTAIN_H_

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

//extern PLIST pTail,pListTop;

ULONG IfSelectRecords(BYTE TypeSel,TListViews *lv);
void SelectDiskType(LONG Type,BYTE znach,TListViews *lv);
VOID SelectAllRecords(BYTE sel,TListViews *lv);
int  CountSelectedRecs(VOID);
BOOL InsertRecords(char* ItemName,LONG itemtype,BYTE Select,TListViews *lv);
VOID AddRecord(CHAR* StrName,BYTE Select,TListViews *lv);
VOID SelectRecord(CHAR* StrName);
#endif
