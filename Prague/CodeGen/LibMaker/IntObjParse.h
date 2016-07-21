#ifndef __INT_OBJ_PARSE
#define __INT_OBJ_PARSE

#include "stdafx.h"
#include <winnt.h>
//IMAGE_SYMBOL

typedef void (__cdecl *OBJPARSE_CALLBACK)(DWORD type,char* name, DWORD UserValue);

int ObjParse(BYTE* body, DWORD body_len, OBJPARSE_CALLBACK callback, DWORD UserValue);

// OBJPARSE_CALLBACK type constants
#define OBJPARSE_EXPORT          1
#define OBJPARSE_IMPORT          2
#define OBJPARSE_DATA            3
#define OBJPARSE_BSS             4

// return constants
#define OBJPARSE_OK              0
#define OBJPARSE_UNEXPECTED      1
#define OBJPARSE_BAD_COFF        2
#define OBJPARSE_NO_MEM          3


#define _DEBUG_SEGMENT			".debug$F"

#endif	//__INT_OBJ_PARSE