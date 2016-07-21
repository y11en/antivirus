#ifndef _CONTAIN_H_
#define _CONTAIN_H_

#pragma pack (1)

typedef struct _List
 {
  CHAR    szName[CCHMAXPATH];// Found file's path & name
  LONG    szType;
  BYTE    Selected;
  struct  _List  *pNext;     // Pointer of next
 } LIST;
typedef LIST *PLIST;

extern PLIST pTail,pListTop;

void SelectDiskType(LONG Type,BYTE znach);
BOOL InsertRecords(char* ItemName,LONG itemtype,BYTE Select);
VOID AddRecord(CHAR* StrName,BYTE Select);
VOID SelectRecord(CHAR* StrName);
VOID SelectAllRecords(BYTE sel);
//int  CountSelectedRecs(VOID);
unsigned long IfSelectRecords(unsigned char TypeSel);

#endif