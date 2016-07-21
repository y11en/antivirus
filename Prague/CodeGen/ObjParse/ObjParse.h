
// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the OBJPARSE_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// OBJPARSE_API functions as being imported from a DLL, wheras this DLL sees symbols
// defined with this macro as being exported.
#ifdef OBJPARSE_EXPORTS
#define OBJPARSE_API __declspec(dllexport)
#else
#define OBJPARSE_API __declspec(dllimport)
#endif

typedef void (__cdecl *OBJPARSE_CALLBACK)(DWORD type,char* name, DWORD UserValue);

OBJPARSE_API int ObjParse(BYTE* body, DWORD body_len, OBJPARSE_CALLBACK callback, DWORD UserValue);

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