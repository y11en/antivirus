#ifndef _DEBUG_H
#define _DEBUG_H

#ifdef _DEBUG

#define ODSN(x) dbgprintf x
#define ODS(x) { dbgprintf x ; dbgprintf("\n"); }

void dbgprintf(const char* format, ...); // via OutputDebugString

#else

#define ODSN(x) 
#define ODS(x) {}

#endif	


#endif