// TypeTableGenerator.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <property/property.h>
#include <serialize/kldtser.h>
#include <_avpio.h>
#include <AVPPveID.h>
#include <Stuff\SArray.h>
#include <Stuff\CPointer.h>
#include <Stuff\PathStr.h>
#include <pr_compl.h>  // compiler dependent macro definitions
#include <Pr_types.h>
#include <Pr_prop.h>

// ---
typedef struct TypeID {
  char      *m_pName;
  AVP_dword  m_nType;
  char      *m_pDefName;
  AVP_dword  m_nDTType;
  AVP_dword  m_nSizeOf; 
} TypeID;

// ---
typedef struct PropDescr {
  char      *m_pName;
  AVP_dword  m_nType;
  char      *m_pDefName;
  AVP_dword  m_nDTType;
  AVP_bool   m_bShared;
  char      *m_pDefValue;
	char      *m_pShortComment;
	char      *m_pLargeComment;
	char      *m_pBehaviorComment;
} PropDescr;

// ---
static TypeID gTypes[] = {
  { "tVOID"       ,   tid_VOID        , "...              ", avpt_nothing , AVP_dword(-1)                   },
  { "tBYTE"       ,   tid_BYTE        , "pTYPE_BYTE       ", avpt_byte    , AVP_dword(sizeof(tBYTE))        },
  { "tWORD"       ,   tid_WORD        , "pTYPE_WORD       ", avpt_word    , AVP_dword(sizeof(tWORD))        },
  { "tDWORD"      ,   tid_DWORD       , "pTYPE_DWORD      ", avpt_dword   , AVP_dword(sizeof(tDWORD))       },
  { "tQWORD"      ,   tid_QWORD       , "pTYPE_QWORD      ", avpt_qword   , AVP_dword(sizeof(tQWORD))       },
  { "tBOOL"       ,   tid_BOOL        , "pTYPE_BOOL       ", avpt_bool    , AVP_dword(sizeof(tBOOL))        },  
  { "tCHAR"       ,   tid_CHAR        , "pTYPE_CHAR       ", avpt_char    , AVP_dword(sizeof(tCHAR))        },  
  { "tWCHAR"      ,   tid_WCHAR       , "pTYPE_WCHAR      ", avpt_wchar   , AVP_dword(sizeof(tWCHAR))       },  
  { "tSTRING"     ,   tid_STRING      , "pTYPE_STRING     ", avpt_str     , AVP_dword(-1)                   },  
  { "tWSTRING"    ,   tid_WSTRING     , "pTYPE_WSTRING    ", avpt_wstr    , AVP_dword(-1)                   },  
  { "tERROR"      ,   tid_ERROR       , "pTYPE_ERROR      ", avpt_dword   , AVP_dword(sizeof(tERROR))       },
  { "tPTR"        ,   tid_PTR         , "pTYPE_PTR        ", avpt_dword   , AVP_dword(sizeof(tPTR))         },  
  { "tINT"        ,   tid_INT         , "pTYPE_INT        ", avpt_int     , AVP_dword(sizeof(tINT))         },  
  { "tUINT"       ,   tid_UINT        , "pTYPE_UINT       ", avpt_uint    , AVP_dword(sizeof(tUINT))        },  
  { "tSBYTE"      ,   tid_SBYTE       , "pTYPE_SBYTE      ", avpt_char    , AVP_dword(sizeof(tSBYTE))       },      
  { "tSHORT"      ,   tid_SHORT       , "pTYPE_SHORT      ", avpt_short   , AVP_dword(sizeof(tSHORT))       },      
  { "tLONG"       ,   tid_LONG        , "pTYPE_LONG       ", avpt_long    , AVP_dword(sizeof(tLONG))        },      
  { "tLONGLONG"   ,   tid_LONGLONG    , "pTYPE_LONGLONG   ", avpt_longlong, AVP_dword(sizeof(tLONGLONG))    },      
  { "tIID"        ,   tid_IID         , "pTYPE_IID        ", avpt_dword   , AVP_dword(sizeof(tIID))         },  
  { "tPID"        ,   tid_PID         , "pTYPE_PID        ", avpt_dword   , AVP_dword(sizeof(tPID))         },  
  { "tORIG_ID"    ,   tid_ORIG_ID     , "pTYPE_ORIG_ID    ", avpt_dword   , AVP_dword(sizeof(tORIG_ID))     },  
  { "tOS_ID"      ,   tid_OS_ID       , "pTYPE_OS_ID      ", avpt_dword   , AVP_dword(sizeof(tOS_ID))       },  
  { "tVID"        ,   tid_VID         , "pTYPE_VID        ", avpt_dword   , AVP_dword(sizeof(tVID))         },  
  { "tPROPID"     ,   tid_PROPID      , "pTYPE_PROPID     ", avpt_dword   , AVP_dword(sizeof(tPROPID))      },  
  { "tVERSION"    ,   tid_VERSION     , "pTYPE_VERSION    ", avpt_dword   , AVP_dword(sizeof(tVERSION))     },  
  { "tCODEPAGE"   ,   tid_CODEPAGE    , "pTYPE_CODEPAGE   ", avpt_dword   , AVP_dword(sizeof(tCODEPAGE))    },  
  { "tLCID"       ,   tid_LCID        , "pTYPE_LCID       ", avpt_dword   , AVP_dword(sizeof(tLCID))        },  
  { "tDATA"       ,   tid_DATA        , "pTYPE_DATA       ", avpt_dword   , AVP_dword(sizeof(tDATA))        },  
  { "tFUNC_PTR"   ,   tid_FUNC_PTR    , "pTYPE_FUNC_PTR   ", avpt_dword   , AVP_dword(sizeof(tFUNC_PTR))    },  
  { "hOBJECT"     ,   tid_OBJECT      , "pTYPE_OBJECT     ", avpt_dword   , AVP_dword(-1)                   },  
  { "tEXPORT"     ,   tid_EXPORT      , "pTYPE_EXPORT     ", avpt_dword   , AVP_dword(sizeof(tEXPORT))      },  
  { "tIMPORT"     ,   tid_IMPORT      , "pTYPE_IMPORT     ", avpt_dword   , AVP_dword(sizeof(tIMPORT))      },  
  { "tTRACE_LEVEL",   tid_TRACE_LEVEL , "pTYPE_TRACE_LEVEL", avpt_dword   , AVP_dword(sizeof(tTRACE_LEVEL)) },  
  { "tTYPE_ID"    ,   tid_TYPE_ID     , "pTYPE_TYPE_ID"    , avpt_dword   , AVP_dword(sizeof(tTYPE_ID))     },  
  { "tDATETIME"   ,   tid_DATETIME    , "pTYPE_DATETIME"   , avpt_datetime, AVP_dword(sizeof(tDATETIME))    },
  { "tDT"         ,   tid_DATETIME    , "pTYPE_DATETIME"   , avpt_datetime, AVP_dword(sizeof(tDT))          },
  { "cSerializable",  tid_SERIALIZABLE, "...             " , avpt_dword   , AVP_dword(sizeof(tPTR))         },  
  { NULL          ,   0               , NULL               , avpt_nothing , AVP_dword(-1)                   },
};                                       

// ---
static TypeID gPropTypes[] = {
  { "tBYTE"         ,  tid_BYTE        , "pTYPE_BYTE    "    , avpt_byte    , AVP_dword(sizeof(tBYTE))     },
  { "tWORD"         ,  tid_WORD        , "pTYPE_WORD    "    , avpt_word    , AVP_dword(sizeof(tWORD))     },
  { "tDWORD"        ,  tid_DWORD       , "pTYPE_DWORD   "    , avpt_dword   , AVP_dword(sizeof(tDWORD))    },
  { "tQWORD"        ,  tid_QWORD       , "pTYPE_QWORD   "    , avpt_qword   , AVP_dword(sizeof(tQWORD))    },
  { "tBOOL"         ,  tid_BOOL        , "pTYPE_BOOL    "    , avpt_bool    , AVP_dword(sizeof(tBOOL))     },  
  { "tCHAR"         ,  tid_CHAR        , "pTYPE_CHAR    "    , avpt_char    , AVP_dword(sizeof(tCHAR))     },  
  { "tWCHAR"        ,  tid_WCHAR       , "pTYPE_WCHAR   "    , avpt_wchar   , AVP_dword(sizeof(tWCHAR))    },  
  { "tSTRING"       ,  tid_STRING      , "pTYPE_STRING  "    , avpt_str     , AVP_dword(-1)                },  
  { "tWSTRING"      ,  tid_WSTRING     , "pTYPE_WSTRING "    , avpt_wstr    , AVP_dword(-1)                },  
  { "tERROR"        ,  tid_ERROR       , "pTYPE_ERROR   "    , avpt_dword   , AVP_dword(sizeof(tERROR))    },
  { "tPTR"          ,  tid_PTR         , "pTYPE_PTR     "    , avpt_dword   , AVP_dword(sizeof(tPTR))      },  
  { "tINT"          ,  tid_INT         , "pTYPE_INT     "    , avpt_int     , AVP_dword(sizeof(tINT))      },  
  { "tUINT"         ,  tid_UINT        , "pTYPE_UINT    "    , avpt_uint    , AVP_dword(sizeof(tUINT))     },  
  { "tSBYTE"        ,  tid_SBYTE       , "pTYPE_SBYTE   "    , avpt_char    , AVP_dword(sizeof(tSBYTE))    },      
  { "tSHORT"        ,  tid_SHORT       , "pTYPE_SHORT   "    , avpt_short   , AVP_dword(sizeof(tSHORT))    },      
  { "tLONG"         ,  tid_LONG        , "pTYPE_LONG    "    , avpt_long    , AVP_dword(sizeof(tLONG))     },      
  { "tLONGLONG"     ,  tid_LONGLONG    , "pTYPE_LONGLONG"    , avpt_longlong, AVP_dword(sizeof(tLONGLONG)) },      
  { "tIID"          ,  tid_IID         , "pTYPE_IID     "    , avpt_dword   , AVP_dword(sizeof(tIID))      },  
  { "tPID"          ,  tid_PID         , "pTYPE_PID     "    , avpt_dword   , AVP_dword(sizeof(tPID))      },  
  { "tORIG_ID"      ,  tid_ORIG_ID     , "pTYPE_ORIG_ID "    , avpt_dword   , AVP_dword(sizeof(tORIG_ID))  },  
  { "tOS_ID"        ,  tid_OS_ID       , "pTYPE_OS_ID   "    , avpt_dword   , AVP_dword(sizeof(tOS_ID))    },  
  { "tVID"          ,  tid_VID         , "pTYPE_VID     "    , avpt_dword   , AVP_dword(sizeof(tVID))      },  
  { "tPROPID"       ,  tid_PROPID      , "pTYPE_PROPID  "    , avpt_dword   , AVP_dword(sizeof(tPROPID))   },  
  { "tVERSION"      ,  tid_VERSION     , "pTYPE_VERSION "    , avpt_dword   , AVP_dword(sizeof(tVERSION))  },  
  { "tCODEPAGE"     ,  tid_CODEPAGE    , "pTYPE_CODEPAGE"    , avpt_dword   , AVP_dword(sizeof(tCODEPAGE)) },  
  { "tLCID"         ,  tid_LCID        , "pTYPE_LCID    "    , avpt_dword   , AVP_dword(sizeof(tLCID))     },  
  { "tDATA"         ,  tid_DATA        , "pTYPE_DATA    "    , avpt_dword   , AVP_dword(sizeof(tDATA))     },  
  { "tFUNC_PTR"     ,  tid_FUNC_PTR    , "pTYPE_FUNC_PTR"    , avpt_dword   , AVP_dword(sizeof(tFUNC_PTR)) },  
  { "tDATETIME"     ,  tid_DATETIME    , "pTYPE_DATETIME"    , avpt_datetime, AVP_dword(sizeof(tDATETIME)) },  
  { "tDT"           ,  tid_DATETIME    , "pTYPE_DATETIME"    , avpt_datetime, AVP_dword(sizeof(tDT))       },  
  { "tBINARY"       ,  tid_BINARY      , "pTYPE_BINARY  "    , avpt_bin     , AVP_dword(-1)                },  
  { "hOBJECT"       ,  tid_OBJECT      , "pTYPE_OBJECT  "    , avpt_dword   , AVP_dword(sizeof(tPTR))      },  
  { "tTYPE_ID"      ,  tid_TYPE_ID     , "pTYPE_TYPE_ID "    , avpt_dword   , AVP_dword(sizeof(tTYPE_ID))  },  
  { "tTRACE_LEVEL"  ,  tid_TRACE_LEVEL , "pTYPE_TRACE_LEVEL" , avpt_dword   , AVP_dword(sizeof(tTRACE_LEVEL)) },  
  { "tEXPORT"       ,  tid_EXPORT      , "pTYPE_EXPORT     " , avpt_dword   , AVP_dword(sizeof(tEXPORT))   },  
  { "tIMPORT"       ,  tid_IMPORT      , "pTYPE_IMPORT     " , avpt_dword   , AVP_dword(sizeof(tIMPORT))   },  
  { "cSerializable" ,  tid_SERIALIZABLE, "...           "    , avpt_dword   , AVP_dword(sizeof(tPTR))      },  
  { NULL            ,  0               , NULL                , avpt_nothing , AVP_dword(-1)                },
};                                       


// ---
static PropDescr gHardProps[] = {
  { "INTERFACE_VERSION"     ,   pgINTERFACE_VERSION     , "INTERFACE_VERSION    ", avpt_dword    , TRUE , "1", "Interface version", "Interface version", "" },
  { "INTERFACE_COMMENT"     ,   pgINTERFACE_COMMENT     , "INTERFACE_COMMENT    ", avpt_str      , TRUE , "" , "Interface comment", "Interface comment", "" },
	//{ "OBJECT_ID"             ,   pgOBJECT_ID             , "OBJECT_ID            ", avpt_dword    , FALSE, "0", "", "", "" },
//{ "INTERFACE_REVISION"    ,   pgINTERFACE_REVISION    , "INTERFACE_REVISION   ", avpt_dword    , FALSE, "0", "", "", "" },
//{ "PLUGIN_VERSION"        ,   pgPLUGIN_VERSION        , "PLUGIN_VERSION       ", avpt_dword    , FALSE, "0", "", "", "" },
//{ "PLUGIN_COMMENT"        ,   pgPLUGIN_COMMENT        , "PLUGIN_COMMENT       ", avpt_str      , FALSE, "" , "", "", "" },
  { NULL                    ,   0                       , NULL                   , avpt_nothing  , FALSE, "0", "", "", "" },
};


#define PROP_OBJ_AVAILABILITY_SC  "Returns set of features availabe for the implementation. Must return set of ORed availability flags"
#define PROP_OBJ_AVAILABILITY_LC  "It is expected the property to be implemented as shared(static) to let customer evaluate objects behaviour without creating the object"

#define PROP_OBJ_BASED_ON_SC "base for this object"
#define PROP_OBJ_BASED_ON_LC "Object which is became base for the current object.\r\nFor example: OS for IO object or IO for SecIO object"

#define PROP_OBJ_ATTRIBS_SC "attributes of the object. Depends on OS"
#define PROP_OBJ_ATTRIBS_LC "Set of the attributes of the object. Contents must be defined by OS implementation"

// ---
static PropDescr gSoftProps[] = {

  { "NATIVE_ERR"              ,   pgNATIVE_ERR               , "NATIVE_ERR               ", avpt_dword   , FALSE, "0",       "Native error", "Native error of some interfaces", ""  },

  { "PLUGIN_ID"               ,   pgPLUGIN_ID                , "PLUGIN_ID                ", avpt_dword   , FALSE, "0",       "Plugin identificator", "Plugin identificator", ""  },
  { "PLUGIN_VERSION"          ,   pgPLUGIN_VERSION           , "PLUGIN_VERSION           ", avpt_dword   , FALSE, "0",       "Plugin version", "Plugin version", ""  },
  { "PLUGIN_NAME"             ,   pgPLUGIN_NAME              , "PLUGIN_NAME              ", avpt_str     , FALSE, "" ,       "Plugin name"		, "Plugin name", ""  },
  { "PLUGIN_COMMENT"          ,   pgPLUGIN_COMMENT           , "PLUGIN_COMMENT           ", avpt_str     , FALSE, "" ,       "Plugin comment", "Plugin comment", ""  },
  { "PLUGIN_CODEPAGE"         ,   pgPLUGIN_CODEPAGE          , "PLUGIN_CODEPAGE          ", avpt_dword   , FALSE, "0",       "Plugin codepage", "Plugin codepage", ""  },
  { "PLUGIN_EXPORT_COUNT"     ,   pgPLUGIN_EXPORT_COUNT      , "PLUGIN_EXPORT_COUNT      ", avpt_dword   , FALSE, "0",       "Plugin count of exports", "Plugin count of exports", ""  },
  
  { "VENDOR_NAME"             ,   pgVENDOR_NAME              , "VENDOR_NAME              ", avpt_str     , FALSE, "" ,       "Vendor name"		, "Vendor name", ""  },
  { "VENDOR_COMMENT"          ,   pgVENDOR_COMMENT           , "VENDOR_COMMENT           ", avpt_str     , FALSE, "" ,       "Vendor comment", "Vendor comment", ""  },
                                                                                         
  { "INTERFACE_REVISION"      ,   pgINTERFACE_REVISION       , "INTERFACE_REVISION       ", avpt_dword   , FALSE, "0",        "Interface revision"	, "Interface revision", ""  },
  { "INTERFACE_COMMENT_CP"    ,   pgINTERFACE_COMMENT_CP     , "INTERFACE_COMMENT_CP     ", avpt_dword   , FALSE, "cCP_OEM" , "Interface comment code page"		, "Interface comment code page", ""  },
  { "INTERFACE_SUPPORTED"     ,   pgINTERFACE_SUPPORTED      , "INTERFACE_SUPPORTED      ", avpt_dword   , FALSE, "0",        "Interface supported"	, "Interface supported", ""  },
  { "INTERFACE_COMPATIBILITY" ,   pgINTERFACE_COMPATIBILITY  , "INTERFACE_COMPATIBILITY  ", avpt_dword   , FALSE, "0",        "Interface compatibility"	, "Interface compatibility", ""  },

  { "OBJECT_NAME"             ,   pgOBJECT_NAME              , "OBJECT_NAME              ", avpt_str     , FALSE, "" ,        "Object name"                   , "Object name", ""  },
  { "OBJECT_NAME_CP"          ,   pgOBJECT_NAME_CP           , "OBJECT_NAME_CP           ", avpt_dword   , FALSE, "" ,        "Object name code page"         , "Object name code page", ""  },
  { "OBJECT_FULL_NAME"        ,   pgOBJECT_FULL_NAME         , "OBJECT_FULL_NAME         ", avpt_str     , FALSE, "" ,        "Object full name"              , "Object full name", ""  },
  { "OBJECT_FULL_NAME_CP"     ,   pgOBJECT_FULL_NAME_CP      , "OBJECT_FULL_NAME_CP      ", avpt_dword   , FALSE, "" ,        "Object full name code page"    , "Object full name code page", ""  },
  { "OBJECT_COMPLETE_NAME"    ,   pgOBJECT_COMPLETE_NAME     , "OBJECT_COMPLETE_NAME     ", avpt_str     , FALSE, "" ,        "Object complete name"          , "Object complete name", ""  },
  { "OBJECT_COMPLETE_NAME_CP" ,   pgOBJECT_COMPLETE_NAME_CP  , "OBJECT_COMPLETE_NAME_CP  ", avpt_dword   , FALSE, "" ,        "Object complete name code page", "Object complete name code page", ""  },
  { "OBJECT_ALT_NAME"         ,   pgOBJECT_ALT_NAME          , "OBJECT_ALT_NAME          ", avpt_str     , FALSE, "" ,        "Object alt name"               , "Object alt name", ""  },
  { "OBJECT_ALT_NAME_CP"      ,   pgOBJECT_ALT_NAME_CP       , "OBJECT_ALT_NAME_CP       ", avpt_dword   , FALSE, "" ,        "Object alt name code page"     , "Object alt name code page", ""  },
  { "OBJECT_PATH"             ,   pgOBJECT_PATH              , "OBJECT_PATH              ", avpt_str     , FALSE, "" ,        "Object path"                   , "Object path", ""  },
  { "OBJECT_PATH_CP"          ,   pgOBJECT_PATH_CP           , "OBJECT_PATH_CP           ", avpt_dword   , FALSE, "" ,        "Object path code page"         , "Object path code page", ""  },
  { "OBJECT_SIZE"             ,   pgOBJECT_SIZE              , "OBJECT_SIZE              ", avpt_dword   , FALSE, "0",        "Object size"                   , "Object size", ""  },
  { "OBJECT_SIZE_Q"           ,   pgOBJECT_SIZE_Q            , "OBJECT_SIZE_Q            ", avpt_qword   , FALSE, "0",        "Object size Q"                 , "Object size Q", ""  },
  { "OBJECT_SIGNATURE"        ,   pgOBJECT_SIGNATURE         , "OBJECT_SIGNATURE         ", avpt_nothing , FALSE, "0",        "Object signatute"              , "Object signatute", ""  },
  { "OBJECT_SUPPORTED"        ,   pgOBJECT_SUPPORTED         , "OBJECT_SUPPORTED         ", avpt_dword   , FALSE, "0",        "Object supported"              , "Object supported", ""  },
  { "OBJECT_ORIGIN"           ,   pgOBJECT_ORIGIN            , "OBJECT_ORIGIN            ", avpt_dword   , FALSE, "0",        "Object origin"                 , "Object origin", ""  },
  { "OBJECT_OS_TYPE"          ,   pgOBJECT_OS_TYPE           , "OBJECT_OS_TYPE           ", avpt_dword   , FALSE, "0",        "Object OS type"                , "Object OS type", ""  },
  { "OBJECT_OPEN_MODE"        ,   pgOBJECT_OPEN_MODE         , "OBJECT_OPEN_MODE         ", avpt_dword   , FALSE, "0",        "Object open mode"              , "Object open mode", ""  },
  { "OBJECT_NEW"              ,   pgOBJECT_NEW               , "OBJECT_NEW               ", avpt_bool    , FALSE, "0",        "Object new"                    , "Object new", ""  },
  { "OBJECT_CODEPAGE"         ,   pgOBJECT_CODEPAGE          , "OBJECT_CODEPAGE          ", avpt_dword   , FALSE, "0",        "Object code page"              , "Object code page", ""  },
  { "OBJECT_LCID"             ,   pgOBJECT_LCID              , "OBJECT_LCID              ", avpt_dword   , FALSE, "0",        "Object LCID"                   , "Object LCID", ""  },
  { "OBJECT_ACCESS_MODE"      ,   pgOBJECT_ACCESS_MODE       , "OBJECT_ACCESS_MODE       ", avpt_dword   , FALSE, "0",        "Object access mode"            , "Object access mode", ""  },
  { "OBJECT_COMPRESSED_SIZE"  ,   pgOBJECT_COMPRESSED_SIZE   , "OBJECT_COMPRESSED_SIZE   ", avpt_dword   , FALSE, "0",        "Object compressed size"        , "Object compressed size", ""  },
  { "OBJECT_AVAILABILITY"     ,   pgOBJECT_AVAILABILITY      , "pgOBJECT_AVAILABILITY    ", avpt_dword   , FALSE, "0",        PROP_OBJ_AVAILABILITY_SC        , PROP_OBJ_AVAILABILITY_LC, ""  },
  { "OBJECT_BASED_ON"         ,   pgOBJECT_BASED_ON          , "pgOBJECT_BASED_ON        ", avpt_dword   , FALSE, "NULL",     PROP_OBJ_BASED_ON_SC            , PROP_OBJ_BASED_ON_LC, ""  },
  { "OBJECT_ATTRIBUTES"       ,   pgOBJECT_ATTRIBUTES        , "pgOBJECT_ATTRIBUTES      ", avpt_dword   , FALSE, "0",        PROP_OBJ_ATTRIBS_SC             , PROP_OBJ_ATTRIBS_LC, ""  },
  { "OBJECT_HASH"             ,   pgOBJECT_HASH              , "pgOBJECT_HASH            ", avpt_qword   , FALSE, "0",        "Object hash"                   , "Object hash",       "" },
  { "OBJECT_REOPEN_DATA"      ,   pgOBJECT_REOPEN_DATA       , "pgOBJECT_REOPEN_DATA     ", avpt_bin     , FALSE, "0",        "Object reopen data"            , "Object reopen data", "" },
  
  { NULL                      ,    0                         , NULL                       , avpt_nothing , FALSE, "0"  },
};


// ---
struct SMethodParam {
  char      *m_pParamName;
  char      *m_pParamTypeName;
  AVP_dword  m_dwParamType;
  AVP_bool   m_bIsPointer;
  AVP_bool   m_bIsConst;
  char      *m_pLargeComment;
};


// ---
struct SMethod {
  char      *m_pMethodName;
  char      *m_pResultTypeName;
  AVP_dword  m_dwResultType;
  char      *m_pResultDefValue;
  char      *m_pShortComment;
  char      *m_pLargeComment;
  char      *m_pBehaviorComment;
  char      *m_pResultComment;
  SMethodParam m_Params[7];
};

#define Recognize_SC "Returns errOK when can create a new object on base of object pointed by \"this\""
#define Recognize_LC "Static method. Kernel calls this method wihtout any created objects of this interface.\r\nResponds to kernel request is it possible to create an this of the interface on a base of pointed this (\"this\" parameter)\r\nKernel considers absence of the implementation as errOK result for all objects"
#define Recognize_BC "It must be quick (preliminary) analysis"
#define Recognize_RC ""

#define ObjectNew_SC "Creates object of the interface"
#define ObjectNew_LC "Static method. Kernel calls this method wihtout any created objects of this interface. First parameter (\"this\") pointed possible parent of the new object.\r\nKernel asks the interface to create new object. To do it developer must use low-level system methods : HandleCreate and InterfaceSet."
#define ObjectNew_BC "Analog of the operator \"new\""
#define ObjectNew_RC ""

#define ObjectNew_obj_LC "new object retrned"

#define ObjectInit_SC "Notification about creating an object"
#define ObjectInit_LC "Kernel notifies an object about successful creating of object"
#define ObjectInit_BC "Initializes internal object data"
#define ObjectInit_RC "Returns value differ from errOK if object cannot be initialized"

#define ObjectInitDone_SC "Notification about setting all necessary properties"
#define ObjectInitDone_LC "Notification that all necessary properties have been set and object must go to operation state"
#define ObjectInitDone_BC ""
#define ObjectInitDone_RC "Returns value differ from errOK if object cannot function properly"

#define ObjectCheck_SC "Checks object internal structure"
#define ObjectCheck_LC "Kernel asks object to check its internal structure."
#define ObjectCheck_BC ""
#define ObjectCheck_RC "errOK means internal structure is OK"

#define ObjectPreClose_SC "Notification about kernel is going to close all children"
#define ObjectPreClose_LC "Kernel warns object it is going to close all children"
#define ObjectPreClose_BC "Object takes all necessary \"before preclosing\" actions"
#define ObjectPreClose_RC ""

#define ObjectClose_SC "Notification about closing the object"
#define ObjectClose_LC "Kernel warns object it must be closed"
#define ObjectClose_BC "Object takes all necessary \"before closing\" actions"
#define ObjectClose_RC ""

#define ChildNew_SC "Notification about creating child of the object"
#define ChildNew_LC "Kernel asks if the object wants to create a child of certain interface."
#define ChildNew_BC "It is a chance to parent to do some job before creating an new child. Or the parent wants to create \"helper\" instead of an object of implemented interface"
#define ChildNew_RC "New object of requested interface"

#define ChildNew_iid_LC  "Requested child interface id"
#define ChildNew_pid_LC  "Requested child plugin id"
#define ChildNew_subtype_LC  "Requested child subtype"

#define ChildInitDone_SC "Notification about child goes to the operational state"
#define ChildInitDone_LC "Child state changed to the \"operational\". Parent can take considerable actions"
#define ChildInitDone_BC ""
#define ChildInitDone_RC "Child state will not be changed if error code is not errOK"

#define ChildInitDone_child_LC  "Child which initialization is completed"

#define ChildClose_SC "Child is destroyed"
#define ChildClose_LC "Notification about child is going to be destroyed. Parent can take considerable actions"
#define ChildClose_BC ""
#define ChildClose_RC "Child will not be destroyed if error code is not errOK"

#define ChildClose_child_LC  "Child which is destroying"

#define MsgReceive_SC "Receives message"
#define MsgReceive_LC "Receives message sent to the object or to the one of the object parents as broadcast"
#define MsgReceive_BC ""
#define MsgReceive_RC ""

#define MsgReceive_reg_id_LC        "Message class identifier"
#define MsgReceive_msg_id_LC        "Message identifier"
#define MsgReceive_obj_LC           "Object where the message was send initially"
#define MsgReceive_ctx_LC           "Context of the object processing"
#define MsgReceive_recvp_LC         "Point of distributing message (tree top for RegisterMsgHandler call "
#define MsgReceive_par_buff_LC      "Buffer of the parameters"
#define MsgReceive_par_buff_len_LC  "Lenght of the buffer of the parameters"

#define IFaceTransfer_SC "Transfers interface internal data from one subtype to another"
#define IFaceTransfer_LC "Responds to request InterfaceSet sytem method with same id, plugin_id and another subtype."
#define IFaceTransfer_BC "op_code parameters values are IFACE_TRANSFER_QUERY and IFACE_TRANSFER_PERFORM.\r\n  -- for IFACE_TRANSFER_QUERY object must return errOK if it is possible to transfer to new (or from old) subtype\r\n  -- for IFACE_TRANSFER_PERFORM must transfer internal data to new (or from old) subtype"
#define IFaceTransfer_RC ""

#define IFaceTransfer_sub_type_LC  "To (or from) which subtype action is requested"
#define IFaceTransfer_op_code_LC  "What action is requested (IFACE_TRANSFER_QUERY or IFACE_TRANSFER_PERFORM)"


// ---
static SMethod gSMethods[] = {
  // ----------
  { 
    "Recognize", "tERROR", tid_ERROR, "errINTERFACE_INCOMPATIBLE", Recognize_SC, Recognize_LC, Recognize_BC, Recognize_RC, 
    //parameters
    {
      {"type", "tDWORD", tid_DWORD, FALSE, FALSE, "type of recognized object"},
    }
  },
  
  // ----------
  { "ObjectNew", "hOBJECT", tid_OBJECT, "NULL", ObjectNew_SC, ObjectNew_LC, ObjectNew_BC, ObjectNew_RC, 
    {
      {"construct", "tBOOL", tid_BOOL, FALSE, FALSE, "invoke constructor/destructor for the object" }, 
    }
    //parameters
  },
  
  // ----------
  { "ObjectInit", "tERROR", tid_ERROR, "errOK", ObjectInit_SC, ObjectInit_LC, ObjectInit_BC, ObjectInit_RC, 
    //parameters
  },
  
  // ----------
  { "ObjectInitDone", "tERROR", tid_ERROR, "errOK", ObjectInitDone_SC, ObjectInitDone_LC, ObjectInitDone_BC, ObjectInitDone_RC, 
    //parameters
  },
  
  // ----------
  { "ObjectCheck", "tERROR", tid_ERROR, "errOK", ObjectCheck_SC, ObjectCheck_LC, ObjectCheck_BC, ObjectCheck_RC, 
    //parameters
  },
  
  // ----------
  { "ObjectPreClose", "tERROR", tid_ERROR, "errOK", ObjectPreClose_SC, ObjectPreClose_LC, ObjectPreClose_BC, ObjectPreClose_RC, 
    //parameters
  },
  
  // ----------
  { "ObjectClose", "tERROR", tid_ERROR, "errOK", ObjectClose_SC, ObjectClose_LC, ObjectClose_BC, ObjectClose_RC, 
    //parameters
  },
  
  // ----------
  { "ChildNew", "hOBJECT", tid_OBJECT, "NULL", ChildNew_SC, ChildNew_LC, ChildNew_BC, ChildNew_RC, 
    //parameters
    {
      {"iid", "tIID", tid_IID, FALSE, FALSE, ChildNew_iid_LC}, 
      {"pid", "tPID", tid_PID, FALSE, FALSE, ChildNew_pid_LC}, 
      {"subtype", "tDWORD", tid_DWORD, FALSE, FALSE, ChildNew_subtype_LC}, 
    }
  },
  
  // ----------
  { "ChildInitDone", "tERROR", tid_ERROR, "errOK", ChildInitDone_SC, ChildInitDone_LC, ChildInitDone_BC, ChildInitDone_RC, 
    //parameters
    {
      {"child", "hOBJECT", tid_OBJECT, FALSE, FALSE, ChildInitDone_child_LC}, 
    }
  },

  // ----------
  { "ChildClose", "tERROR", tid_ERROR, "errOK", ChildClose_SC, ChildClose_LC, ChildClose_BC, ChildClose_RC, 
    //parameters
    {
      {"child", "hOBJECT", tid_OBJECT, FALSE, FALSE, ChildClose_child_LC}, 
    }
  },

  // ----------
  { "MsgReceive", "tERROR", tid_ERROR, "errOK", MsgReceive_SC, MsgReceive_LC, MsgReceive_BC, MsgReceive_RC, 
    //parameters
    {
      {"msg_cls_id",    "tDWORD",  tid_DWORD,  FALSE, FALSE, MsgReceive_reg_id_LC       },
      {"msg_id",        "tDWORD",  tid_DWORD,  FALSE, FALSE, MsgReceive_msg_id_LC       },
      {"send_point",    "hOBJECT", tid_OBJECT, FALSE, FALSE, MsgReceive_obj_LC          },
      {"ctx",           "hOBJECT", tid_OBJECT, FALSE, FALSE, MsgReceive_ctx_LC          },
			{"receive_point", "hOBJECT", tid_OBJECT, FALSE, FALSE, MsgReceive_recvp_LC        },
      {"par_buf",       "tVOID",   tid_VOID,   TRUE,  FALSE, MsgReceive_par_buff_LC     },
      {"par_buf_len",   "tDWORD",  tid_DWORD,  TRUE,  FALSE, MsgReceive_par_buff_len_LC },
    }
  },
  
  // ----------
  { "IFaceTransfer" , "tERROR" , tid_ERROR,  "errNOT_SUPPORTED", IFaceTransfer_SC , IFaceTransfer_LC , IFaceTransfer_BC , IFaceTransfer_RC , 
    //parameters
    {
      {"sub_type", "tDWORD", tid_DWORD, FALSE, FALSE, IFaceTransfer_sub_type_LC}, 
      {"op_code",  "tDWORD", tid_DWORD, FALSE, FALSE, IFaceTransfer_op_code_LC}, 
      {0}, {0}
    }
  },
                                             
  // ----------
  { NULL, NULL, 0, NULL, NULL, NULL, NULL, NULL, {{0}, {0}, {0}, {0}} },
};


#define MAX_TYPECONST 3

// ---
struct SConstValue {
  char       *m_pValueName;
  AVP_qword    m_dwValue;
};

// ---
struct STypeConst {
  AVP_dword   m_dwPragueType;
  SConstValue  m_Values[MAX_TYPECONST];
};

/*
#define cCP_NULL                       ((tCODEPAGE)(0))
#define cCP_OEM                        ((tCODEPAGE)(((tUINT)-1)-10))
#define cCP_ANSI                       ((tCODEPAGE)(((tUINT)-1)-11))
#define cCP_IBM                        ((tCODEPAGE)(((tUINT)-1)-12))
#define cCP_UTF8                       ((tCODEPAGE)(((tUINT)-1)-13))
#define cCP_UNICODE                    ((tCODEPAGE)(1200))
#define cCP_UNICODE_BIGENDIAN          ((tCODEPAGE)(1201))
*/

// ---
static STypeConst gSTypeConst[] = {
  { tid_BYTE       , {{"cMIN_BYTE"             , cMIN_BYTE}             , {"cMAX_BYTE"      , cMAX_BYTE}      , {0, 0}} },
  { tid_WORD       , {{"cMIN_WORD"             , cMIN_WORD}             , {"cMAX_WORD"      , cMAX_WORD}      , {0, 0}} },
  { tid_BOOL       , {{"cTRUE"                 , cTRUE}                 , {"cFALSE"         , cFALSE}         , {0, 0}} },
  { tid_INT        , {{"cMIN_INT"              , cMIN_INT}              , {"cMAX_INT"       , cMAX_INT}       , {0, 0}} },
  { tid_UINT       , {{"cMIN_UINT"             , cMIN_UINT}             , {"cMAX_UINT"      , cMAX_UINT}      , {0, 0}} },
  { tid_SHORT      , {{"cMIN_SHORT_INT"        , cMIN_SHORT_INT}        , {"cMAX_SHORT_INT" , cMAX_SHORT_INT} , {0, 0}} },
                                                                       
  { tid_CODEPAGE   , {{"cCP_NULL"              , cCP_NULL}              , {0,                 0}              , {0, 0}} },
  { tid_CODEPAGE   , {{"cCP_OEM"               , cCP_OEM}               , {0,                 0}              , {0, 0}} },
  { tid_CODEPAGE   , {{"cCP_ANSI"              , cCP_ANSI}              , {0,                 0}              , {0, 0}} },
  { tid_CODEPAGE   , {{"cCP_IBM"               , cCP_IBM}               , {0,                 0}              , {0, 0}} },
  { tid_CODEPAGE   , {{"cCP_UTF8"              , cCP_UTF8}              , {0,                 0}              , {0, 0}} },
  { tid_CODEPAGE   , {{"cCP_UNICODE"           , cCP_UNICODE}           , {0,                 0}              , {0, 0}} },
  { tid_CODEPAGE   , {{"cCP_UNICODE_BIGENDIAN" , cCP_UNICODE_BIGENDIAN} , {0,                 0}              , {0, 0}} },
  
  { 0              , {{0                       , 0}                     , {0,                 0}              , {0, 0}} },
};


//---
static void PathExtract( const TCHAR * pszFileName, TCHAR * pszDirName, int uicchBufferLen ) {
  if ( pszFileName && pszDirName ) {

    *pszDirName = NULL;

    char fDrive[_MAX_DRIVE];
    char fDir  [_MAX_DIR];

  // выделить диск и каталог
    _splitpath( pszFileName, fDrive, fDir, NULL, NULL);
    int dLen = lstrlen(fDir);
    if ( dLen > 1 && *(fDir + dLen - 1) == L'\\' )
      *((char*)fDir+dLen-1) = 0;

    if ( (lstrlen(fDrive) + lstrlen(fDir)) < (uicchBufferLen - 1) ) {
      lstrcpyn( pszDirName, fDrive, uicchBufferLen );
      int uichLen = lstrlen(pszDirName);
      pszDirName += uichLen;
      lstrcpyn( pszDirName, fDir, uicchBufferLen - uichLen );
    }  
  }
}


// ---
static TCHAR *GetStartupDirectory() {
  CPathStr szPathName( _MAX_PATH );
  CPathStr szExeName( _MAX_PATH );
  ::GetModuleFileName( NULL, szExeName, _MAX_PATH );

  ::PathExtract( szExeName, szPathName, _MAX_PATH );

  return szPathName.Relinquish();
}


// ---
int main(int argc, char* argv[]) {
  ::DATA_Init_Library( malloc, free );
  ::KLDT_Init_Library( malloc, free );
  ::LoadIO();

  HDATA hRootData = ::DATA_Add( NULL, NULL, AVP_PID_NOTHING, 0, 0 );
  if ( hRootData ) {
    HDATA hTTRootData = ::DATA_Add( hRootData, NULL, TT_PID_ROOT, 0, 0 );
    if ( hTTRootData ) {
      HPROP hNameProp     = ::DATA_Find_Prop( hTTRootData, NULL, 0 );
      HPROP hTypeProp     = ::DATA_Add_Prop( hTTRootData, NULL, TT_PID_TYPEID, 0, 0 );
      HPROP hTypeDefProp = ::DATA_Add_Prop( hTTRootData, NULL, TT_PID_TYPEDEFNAME, 0, 0 );
      HPROP hDTTypeProp   = ::DATA_Add_Prop( hTTRootData, NULL, TT_PID_DTTYPEID, 0, 0 );
      HPROP hSizeOfProp   = ::DATA_Add_Prop( hTTRootData, NULL, TT_PID_SIZEOF, 0, 0 );
      if ( hNameProp && hTypeProp && hTypeDefProp && hSizeOfProp ) {
        for ( int i=0; gTypes[i].m_pName; i++ ) {
          ::PROP_Arr_Insert( hNameProp,     PROP_ARR_LAST, (void *)gTypes[i].m_pName, 0 );
          ::PROP_Arr_Insert( hTypeProp,     PROP_ARR_LAST, (void *)&gTypes[i].m_nType, sizeof(AVP_dword) );
          ::PROP_Arr_Insert( hTypeDefProp, PROP_ARR_LAST, (void *)gTypes[i].m_pDefName, 0 );
          ::PROP_Arr_Insert( hDTTypeProp,   PROP_ARR_LAST, (void *)&gTypes[i].m_nDTType, sizeof(AVP_dword) );
          ::PROP_Arr_Insert( hSizeOfProp,   PROP_ARR_LAST, (void *)&gTypes[i].m_nSizeOf, sizeof(AVP_dword) );
        }
      }
    }

    HDATA hPTRootData = ::DATA_Add( hRootData, NULL, PT_PID_ROOT, 0, 0 );
    if ( hPTRootData ) {
      HPROP hNameProp     = ::DATA_Find_Prop( hPTRootData, NULL, 0 );
      HPROP hTypeProp     = ::DATA_Add_Prop( hPTRootData, NULL, PT_PID_TYPEID, 0, 0 );
      HPROP hTypeDefProp = ::DATA_Add_Prop( hPTRootData, NULL, PT_PID_TYPEDEFNAME, 0, 0 );
      HPROP hDTTypeProp   = ::DATA_Add_Prop( hPTRootData, NULL, PT_PID_DTTYPEID, 0, 0 );
      HPROP hSizeOfProp   = ::DATA_Add_Prop( hPTRootData, NULL, PT_PID_SIZEOF, 0, 0 );
      if ( hNameProp && hTypeProp && hTypeDefProp && hSizeOfProp ) {
        for ( int i=0; gPropTypes[i].m_pName; i++ ) {
          ::PROP_Arr_Insert( hNameProp,     PROP_ARR_LAST, (void *)gPropTypes[i].m_pName, 0 );
          ::PROP_Arr_Insert( hTypeProp,     PROP_ARR_LAST, (void *)&gPropTypes[i].m_nType, sizeof(AVP_dword) );
          ::PROP_Arr_Insert( hTypeDefProp, PROP_ARR_LAST, (void *)gPropTypes[i].m_pDefName, 0 );
          ::PROP_Arr_Insert( hDTTypeProp,   PROP_ARR_LAST, (void *)&gPropTypes[i].m_nDTType, sizeof(AVP_dword) );
          ::PROP_Arr_Insert( hSizeOfProp,   PROP_ARR_LAST, (void *)&gPropTypes[i].m_nSizeOf, sizeof(AVP_dword) );
        }
      }
    }

    HDATA hHPRootData = ::DATA_Add( hRootData, NULL, HP_PID_ROOT, 0, 0 );
    if ( hHPRootData ) {
      HPROP hNameProp					= ::DATA_Find_Prop( hHPRootData, NULL, 0 );
      HPROP hTypeProp					= ::DATA_Add_Prop( hHPRootData, NULL, HP_PID_TYPEID, 0, 0 );
      HPROP hTypeDefProp			= ::DATA_Add_Prop( hHPRootData, NULL, HP_PID_TYPEDEFNAME, 0, 0 );
      HPROP hDTTypeProp				= ::DATA_Add_Prop( hHPRootData, NULL, HP_PID_DTTYPEID, 0, 0 );
      HPROP hSharedProp				= ::DATA_Add_Prop( hHPRootData, NULL, HP_PID_SHOULDBESHARED, 0, 0 );
      HPROP hDefValProp				= ::DATA_Add_Prop( hHPRootData, NULL, HP_PID_SHAREDDEFVALUE, 0, 0 );
      HPROP hShortComProp			= ::DATA_Add_Prop( hHPRootData, NULL, HP_PID_SHORTCOMMENT, 0, 0 );
      HPROP hLargeComProp			= ::DATA_Add_Prop( hHPRootData, NULL, HP_PID_LARGECOMMENT, 0, 0 );
      HPROP hBehaviorComProp	= ::DATA_Add_Prop( hHPRootData, NULL, HP_PID_BEHAVIORCOMMENT, 0, 0 );
      if ( hNameProp && hTypeProp && hTypeDefProp ) {
        for ( int i=0; gHardProps[i].m_pName; i++ ) {
          ::PROP_Arr_Insert( hNameProp,					PROP_ARR_LAST, (void *)gHardProps[i].m_pName, 0 );
          ::PROP_Arr_Insert( hTypeProp,					PROP_ARR_LAST, (void *)&gHardProps[i].m_nType, sizeof(AVP_dword) );
          ::PROP_Arr_Insert( hTypeDefProp,			PROP_ARR_LAST, (void *)gHardProps[i].m_pDefName, 0 );
          ::PROP_Arr_Insert( hDTTypeProp,				PROP_ARR_LAST, (void *)&gHardProps[i].m_nDTType, sizeof(AVP_dword) );
          ::PROP_Arr_Insert( hSharedProp,				PROP_ARR_LAST, (void *)&gHardProps[i].m_bShared, sizeof(AVP_bool) );
          ::PROP_Arr_Insert( hDefValProp,				PROP_ARR_LAST, (void *)gHardProps[i].m_pDefValue, 0 );
          ::PROP_Arr_Insert( hShortComProp,			PROP_ARR_LAST, (void *)gHardProps[i].m_pShortComment, 0 );
          ::PROP_Arr_Insert( hLargeComProp,			PROP_ARR_LAST, (void *)gHardProps[i].m_pLargeComment, 0 );
          ::PROP_Arr_Insert( hBehaviorComProp,	PROP_ARR_LAST, (void *)gHardProps[i].m_pBehaviorComment, 0 );
        }
      }
    }

    HDATA hSPRootData = ::DATA_Add( hRootData, NULL, SP_PID_ROOT, 0, 0 );
    if ( hSPRootData ) {
      HPROP hNameProp					= ::DATA_Find_Prop( hSPRootData, NULL, 0 );
      HPROP hTypeProp					= ::DATA_Add_Prop( hSPRootData, NULL, SP_PID_TYPEID, 0, 0 );
      HPROP hTypeDefProp			= ::DATA_Add_Prop( hSPRootData, NULL, SP_PID_TYPEDEFNAME, 0, 0 );
      HPROP hDTTypeProp				= ::DATA_Add_Prop( hSPRootData, NULL, SP_PID_DTTYPEID, 0, 0 );
      HPROP hSharedProp				= ::DATA_Add_Prop( hSPRootData, NULL, SP_PID_SHOULDBESHARED, 0, 0 );
      HPROP hDefValProp				= ::DATA_Add_Prop( hSPRootData, NULL, SP_PID_SHAREDDEFVALUE, 0, 0 );
      HPROP hShortComProp			= ::DATA_Add_Prop( hSPRootData, NULL, SP_PID_SHORTCOMMENT, 0, 0 );
      HPROP hLargeComProp			= ::DATA_Add_Prop( hSPRootData, NULL, SP_PID_LARGECOMMENT, 0, 0 );
      HPROP hBehaviorComProp	= ::DATA_Add_Prop( hSPRootData, NULL, SP_PID_BEHAVIORCOMMENT, 0, 0 );
      if ( hNameProp && hTypeProp && hTypeDefProp ) {
        for ( int i=0; gSoftProps[i].m_pName; i++ ) {
          ::PROP_Arr_Insert( hNameProp,					PROP_ARR_LAST, (void *)gSoftProps[i].m_pName, 0 );
          ::PROP_Arr_Insert( hTypeProp,					PROP_ARR_LAST, (void *)&gSoftProps[i].m_nType, sizeof(AVP_dword) );
          ::PROP_Arr_Insert( hTypeDefProp,			PROP_ARR_LAST, (void *)gSoftProps[i].m_pDefName, 0 );
          ::PROP_Arr_Insert( hDTTypeProp,				PROP_ARR_LAST, (void *)&gSoftProps[i].m_nDTType, sizeof(AVP_dword) );
          ::PROP_Arr_Insert( hSharedProp,				PROP_ARR_LAST, (void *)&gSoftProps[i].m_bShared, sizeof(AVP_bool) );
          ::PROP_Arr_Insert( hDefValProp,				PROP_ARR_LAST, (void *)gSoftProps[i].m_pDefValue, 0 );
          ::PROP_Arr_Insert( hShortComProp,			PROP_ARR_LAST, (void *)gSoftProps[i].m_pShortComment, 0 );
          ::PROP_Arr_Insert( hLargeComProp,			PROP_ARR_LAST, (void *)gSoftProps[i].m_pLargeComment, 0 );
          ::PROP_Arr_Insert( hBehaviorComProp,	PROP_ARR_LAST, (void *)gSoftProps[i].m_pBehaviorComment, 0 );
        }
      }
    }

    HDATA hSMRootData = ::DATA_Add( hRootData, NULL, VE_PID_IF_SYSTEMMETHODS, 0, 0 );
    if ( hSMRootData ) {
      for ( int i=0; gSMethods[i].m_pMethodName; i++ ) {
        HDATA hMethodData = ::DATA_Add( hSMRootData, NULL, VE_PID_IF_METHOD(i), 0, 0 );
        if ( hMethodData ) {
          ::DATA_Add_Prop( hMethodData, NULL, VE_PID_IFM_METHODNAME, AVP_dword(gSMethods[i].m_pMethodName), 0 );
          ::DATA_Add_Prop( hMethodData, NULL, VE_PID_IFM_RESULTTYPE, AVP_dword(gSMethods[i].m_dwResultType), 0 );
          ::DATA_Add_Prop( hMethodData, NULL, VE_PID_IFM_RESULTTYPENAME, AVP_dword(gSMethods[i].m_pResultTypeName), 0 );
          ::DATA_Add_Prop( hMethodData, NULL, VE_PID_IFM_RESULTDEFVALUE, AVP_dword(gSMethods[i].m_pResultDefValue), 0 );
          ::DATA_Add_Prop( hMethodData, NULL, VE_PID_SHORTCOMMENT, AVP_dword(gSMethods[i].m_pShortComment), 0 );
          ::DATA_Add_Prop( hMethodData, NULL, VE_PID_LARGECOMMENT, AVP_dword(gSMethods[i].m_pLargeComment), 0 );
          ::DATA_Add_Prop( hMethodData, NULL, VE_PID_BEHAVIORCOMMENT, AVP_dword(gSMethods[i].m_pBehaviorComment), 0 );
          ::DATA_Add_Prop( hMethodData, NULL, VE_PID_VALUECOMMENT, AVP_dword(gSMethods[i].m_pResultComment), 0 );
          ::DATA_Add_Prop( hMethodData, NULL, VE_PID_NODETYPE, AVP_dword(VE_NT_SYSTEMMETHOD), 0 );

          for ( int j=0; j< sizeof(gSMethods[i].m_Params)/sizeof(gSMethods[i].m_Params[0]) && gSMethods[i].m_Params[j].m_pParamName; j++ ) {
            HDATA hParamData = ::DATA_Add( hMethodData, NULL, VE_PID_IFM_PARAMETER(j), 0, 0 );
            if ( hParamData ) {
              ::DATA_Add_Prop( hParamData, NULL, VE_PID_IFMP_PARAMETERNAME, AVP_dword(gSMethods[i].m_Params[j].m_pParamName), 0 );
              ::DATA_Add_Prop( hParamData, NULL, VE_PID_IFMP_TYPENAME, AVP_dword(gSMethods[i].m_Params[j].m_pParamTypeName), 0 );
              ::DATA_Add_Prop( hParamData, NULL, VE_PID_IFMP_TYPE, AVP_dword(gSMethods[i].m_Params[j].m_dwParamType), 0 );
              ::DATA_Add_Prop( hParamData, NULL, VE_PID_IFMP_ISPOINTER, AVP_dword(gSMethods[i].m_Params[j].m_bIsPointer), 0 );
              ::DATA_Add_Prop( hParamData, NULL, VE_PID_IFMP_ISCONST, AVP_dword(gSMethods[i].m_Params[j].m_bIsConst), 0 );
              ::DATA_Add_Prop( hParamData, NULL, VE_PID_LARGECOMMENT, AVP_dword(gSMethods[i].m_Params[j].m_pLargeComment), 0 );
							::DATA_Add_Prop( hParamData, NULL, VE_PID_NODETYPE, AVP_dword(VE_NT_SYSTEMMETHODPARAM), 0 );
            }
          }
        }
      }
    }


    HDATA hTCRootData = ::DATA_Add( hRootData, NULL, TC_PID_ROOT, 0, 0 );
    if ( hTCRootData ) {
      for ( int i=0; gSTypeConst[i].m_dwPragueType; i++ ) {
        HDATA hTCData = ::DATA_Add( hTCRootData, NULL, TC_PID_TYPEID(i), gSTypeConst[i].m_dwPragueType, 0 );
        if ( hTCData ) {
          HPROP hNameProp    = ::DATA_Add_Prop( hTCData, NULL, TC_PID_CONSTANTNAME, 0, 0 );
          HPROP hValueProp  = ::DATA_Add_Prop( hTCData, NULL, TC_PID_CONSTANTVALUE, 0, 0 );
          for ( int j=0; j<sizeof(gSTypeConst[i].m_Values)/sizeof(gSTypeConst[i].m_Values[0]) && gSTypeConst[i].m_Values[j].m_pValueName; j++ ) {
            ::PROP_Arr_Insert( hNameProp,   PROP_ARR_LAST, (void *)gSTypeConst[i].m_Values[j].m_pValueName, 0 );
            ::PROP_Arr_Insert( hValueProp, PROP_ARR_LAST, (void *)&gSTypeConst[i].m_Values[j].m_dwValue, sizeof(gSTypeConst[i].m_Values[j].m_dwValue) );
          }
        }
      }
    }


    CAPointer<TCHAR> pStartup = ::GetStartupDirectory();
    CPathStr pFileName = pStartup;
    pFileName ^= "PTypeTbl.ptt";

    if ( ::KLDT_SerializeUsingSWM(pFileName, hRootData, "Prague predefined types table") ) 
      printf("The type table file \"%s\" generated succesfully.\r\n", LPCTSTR(pFileName));
    else
      printf("Error writing type table file \"%s\".\r\n", LPCTSTR(pFileName));
      
    ::DATA_Remove( hRootData, NULL );

    return 0;
  }

  printf("Error generating type table.\r\n");
  return 0;
}

