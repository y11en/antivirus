#ifndef _SUBDEFINES_H_
#define _SUBDEFINES_H_

typedef struct _COUNTSTRUCT{
	DWORD GlobalStep;
	DWORD GlobalStepCount;
	DWORD LocalStep;
	DWORD LocalStepCount;
	char* ObjectName;
}COUNTSTRUCT,*pCOUNTSTRUCT;

#endif //_SUBDEFINES_H_