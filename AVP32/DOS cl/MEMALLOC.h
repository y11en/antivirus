#ifndef __MEMALLOC_H
#define __MEMALLOC_H

#ifndef _SIZE_T_DEFINED_
#define _SIZE_T_DEFINED_
 typedef unsigned size_t;
#endif

//extern unsigned long ulFree,ulMalloc,defMem;
void Free(void *__ptr);
void *Malloc(unsigned long Size);
int  heapchk(void);

#endif

