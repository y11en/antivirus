#ifndef __XMLStruct_h__
#define __XMLStruct_h__
//======================================================================//
typedef struct __TagAttribInfo__
{
  const char* name;
  bool optional;
  bool temporary;
} _TagAttribInfo;

typedef struct __SubTagInfo__
{
  struct __TagDescription__ *descr;
  bool multiple;
} _SubTagInfo;

class XMLTag;
typedef struct __TagDescription__
{
  const char* name;
  bool HasData;
  struct __TagAttribInfo__ *attribs;
  struct __SubTagInfo__ *subtags;
  bool (*validate_func)(XMLTag&);
} _TagDescription;
//======================================================================//
#endif
