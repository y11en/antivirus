#ifndef _KL_LOADER_H_
#define _KL_LOADER_H_

#include "pe.h"

typedef struct _MODULE_TEXT_SECTION
{
  ULONG Base;
  ULONG Length;
  LIST_ENTRY ListEntry;
  PWCH Name;
  PIMAGE_OPTIONAL_HEADER OptionalHeader;

} MODULE_TEXT_SECTION, *PMODULE_TEXT_SECTION;

typedef struct _MODULE_OBJECT
{
  CSHORT  ObjectType;
  CSHORT  ObjectSize;
  PVOID  Base;
  ULONG Length;
  ULONG Flags;
  PVOID  EntryPoint;
  LIST_ENTRY ListEntry;
  UNICODE_STRING FullName;
  UNICODE_STRING BaseName;
  PMODULE_TEXT_SECTION TextSection;
  union
    {
      struct
        {
          PIMAGE_FILE_HEADER FileHeader;
          PIMAGE_OPTIONAL_HEADER OptionalHeader;
          PIMAGE_SECTION_HEADER SectionList;
        } PE;
    } Image;
} MODULE_OBJECT, *PMODULE_OBJECT;

typedef MODULE_OBJECT MODULE, *PMODULE;

#define MODULE_FLAG_BIN  0x0001
#define MODULE_FLAG_MZ   0x0002
#define MODULE_FLAG_NE   0x0004
#define MODULE_FLAG_PE   0x0008
#define MODULE_FLAG_COFF 0x0010

typedef struct _INSTANCE
{
  HANDLE ModuleHandle;
} INSTANCE, *PINSTANCE;

NTSTATUS
LdrProcessModule(PVOID ModuleLoadBase,
		 PUNICODE_STRING ModuleName,
		 PMODULE_OBJECT *ModuleObject);

#endif