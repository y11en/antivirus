// PRTDump.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "scantree.h"
#include "../../AVPPveID.h"
#include <Prague/prague.h>

extern tNodeDsc prt_if_it[];
extern tNodeDsc imp_if_it[];
extern tNodeDsc dsc_if_it[];
extern tNodeDsc data_str_it[];

extern tPropDsc prt_root_props[];
extern tPropDsc imp_root_props[];
extern tPropDsc dsc_root_props[];

extern tPropDsc prt_iface_props[];
extern tPropDsc imp_iface_props[];
extern tPropDsc dsc_iface_props[];

extern tPropDsc prt_property_props[];
extern tPropDsc imp_property_props[];
extern tPropDsc dsc_property_props[];

extern tPropDsc prt_method_props[];
extern tPropDsc imp_method_props[];
extern tPropDsc sys_method_props[];

extern tPropDsc dsc_method_props[];
extern tPropDsc dsc_sys_method_props[];

extern tPropDsc param_props[];
extern tPropDsc dsc_param_props[];

extern tPropDsc type_props[];
extern tPropDsc const_props[];
extern tPropDsc error_props[];
extern tPropDsc msgcl_props[];
extern tPropDsc message_props[];
extern tPropDsc data_str_props[];
extern tPropDsc data_memb_props[];
extern tPropDsc old_sys_meth_props[];

#define VE_PID_IF_OLD_SYSTEMMETHODS       MAKE_AVP_PID(3,VE_APP_ID,avpt_nothing,0)

#define VE_PID_IF_OLD_SYSMETHOD_NAME      MAKE_AVP_PID(401,VE_APP_ID,avpt_str,0)
#define VE_PID_IF_OLD_SYSMETHOD_SELECTED  MAKE_AVP_PID(402,VE_APP_ID,avpt_bool,0)

#define VE_PID_IFDM_OLD_SHORT_COMMENT     MAKE_AVP_PID(603,VE_APP_ID,avpt_str,0)

#define VE_PID_IF_UNK_STR_1               MAKE_AVP_PID(804,VE_APP_ID,avpt_str,0)
#define VE_PID_IF_UNK_STR_2               MAKE_AVP_PID(805,VE_APP_ID,avpt_str,0)

#define VE_PID_IF_COMMENT1                MAKE_AVP_PID(108,VE_APP_ID,avpt_str,0)
#define VE_PID_IF_COMMENT2                MAKE_AVP_PID(109,VE_APP_ID,avpt_str,0)
#define VE_PID_IF_SELECTEDFORIMP          MAKE_AVP_PID(123,VE_APP_ID,avpt_bool,0)

/*
// Interface property shared scope pointer PROP
#define VE_PID_IFPSS_POINTER_ID					(213)
#define VE_PID_IFPSS_POINTER						MAKE_AVP_PID(VE_PID_IFPSS_POINTER_ID,VE_APP_ID,avpt_bool,0)
*/


// ---
extern tSymbDsc prt_method_type;
extern tSymbDsc imp_method_type;
extern tSymbDsc sysmeth_type;
extern tSymbDsc method_param_type;
extern tSymbDsc iface_prop_type;
extern tSymbDsc iface_type_type;
extern tSymbDsc iface_const_type;
extern tSymbDsc iface_error_type;
extern tSymbDsc imp_member_type;


// ---                                                                                                                    hi ar no cp     td op fi
tNodeDsc prt_iface    = { 0, n_IFACE,  VE_PID_IF_INTERFACENAME,      VE_PID_NODETYPE, prt_iface_props,    prt_if_it,      0, 0, 0, ecp_h };
tNodeDsc imp_iface    = { 0, n_IMP,    VE_PID_IF_INTERFACENAME,      0,               imp_iface_props,    imp_if_it,      0, 0, 0, ecp_h };
tNodeDsc dsc_iface    = { 0, n_IMP,    VE_PID_IF_DIGITALID,          VE_PID_NODETYPE, dsc_iface_props,    dsc_if_it,      0, 0, 0, ecp_h };

tNodeDsc prt_if_prp   = { 0, n_PROP,   VE_PID_IFP_PROPNAME,          VE_PID_NODETYPE, prt_property_props, 0,              0, 0, 0, ecp_h, &iface_prop_type,   ";", "" };
tNodeDsc imp_if_prp   = { 0, n_PROP,   VE_PID_IFP_PROPNAME,          VE_PID_NODETYPE, imp_property_props, 0,              0, 0, 0, ecp_h, &iface_prop_type,   ";", "" };
tNodeDsc dsc_if_prp   = { 0, n_PROP,   VE_PID_IFP_DIGITALID,         VE_PID_NODETYPE, dsc_property_props, 0,              0, 0, 0, ecp_h, &iface_prop_type,   ";", "" };

// ---                                                                                                                    hi ar no cp     td op fi
tNodeDsc prt_meth_p   = { 0, n_METH_P, VE_PID_IFMP_PARAMETERNAME,    VE_PID_NODETYPE, param_props,        0,              0, 0, 1, ecp_h, &method_param_type, ",", "" };
tNodeDsc imp_meth_p   = { 0, n_METH_P, VE_PID_IFMP_PARAMETERNAME,    VE_PID_NODETYPE, param_props,        0,              0, 0, 1, ecp_h, &method_param_type, ",", "" };
tNodeDsc sys_meth_p   = { 0, n_METH_P, VE_PID_IFMP_PARAMETERNAME,    VE_PID_NODETYPE, param_props,        0,              1, 0, 1, ecp_h, &method_param_type, ",", "" };
tNodeDsc dsc_meth_p   = { 0, n_METH_P, VE_PID_IFMP_TYPE,             VE_PID_NODETYPE, dsc_param_props,    0,              0, 0, 1, ecp_h, &method_param_type, ",", "" };
tNodeDsc dsc_s_meth_p = { 0, n_METH_P, VE_PID_IFMP_TYPE,             VE_PID_NODETYPE, dsc_param_props,    0,              1, 0, 1, ecp_h, &method_param_type, ",", "" };

// ---                                                                                                                    hi ar no cp     td op fi
tNodeDsc prt_meth     = { 0, n_METH,   VE_PID_IFM_METHODNAME,        VE_PID_NODETYPE, prt_method_props,   &prt_meth_p,    0, 1, 0, ecp_h, &prt_method_type,   "(", ");" };
tNodeDsc imp_meth     = { 0, n_METH,   VE_PID_IFM_METHODNAME,        VE_PID_NODETYPE, imp_method_props,   &imp_meth_p,    0, 1, 0, ecp_h, &imp_method_type,   "(", ");" };
tNodeDsc sys_meth     = { 0, n_METH,   VE_PID_IFM_METHODNAME,        VE_PID_NODETYPE, sys_method_props,   &sys_meth_p,    0, 1, 0, ecp_h, &sysmeth_type,      "",  ";"  };

tNodeDsc dsc_meth     = { 0, n_METH,   VE_PID_IFM_METHOD_ID,         VE_PID_NODETYPE, dsc_method_props,    &dsc_meth_p,   0, 1, 0, ecp_h, &imp_method_type,   "(", ");" };
tNodeDsc dsc_sys_meth = { 0, n_METH,   VE_PID_IFM_METHOD_ID,         VE_PID_NODETYPE, dsc_sys_method_props,&dsc_s_meth_p, 0, 1, 0, ecp_h, &sysmeth_type,      "",  ";" };

// ---                                                                                                                    hi ar no cp     td op fi
tNodeDsc iface_type   = { 0, n_TYPE,   VE_PID_IFT_TYPENAME,          VE_PID_NODETYPE, type_props,         0,              0, 0, 0, ecp_h, &iface_type_type,   ";", "" };
tNodeDsc iface_const  = { 0, n_CONST,  VE_PID_IFC_CONSTANTNAME,      VE_PID_NODETYPE, const_props,        0,              0, 0, 0, ecp_h, &iface_const_type,  ";", "" };
tNodeDsc iface_error  = { 0, n_ERR,    VE_PID_IFE_ERRORCODENAME,     VE_PID_NODETYPE, error_props,        0,              0, 0, 0, ecp_h, &iface_error_type,  ";", "" };

tNodeDsc iface_msg    = { 0, n_MSG,    VE_PID_IFM_MESSAGENAME,       VE_PID_NODETYPE, message_props,      0,              0, 0, 0, ecp_h, 0,                  ";", "" };
tNodeDsc iface_msgcl  = { 0, n_MSG_CL, VE_PID_IFM_MESSAGECLASSNAME,  VE_PID_NODETYPE, msgcl_props,        &iface_msg,     0, 1, 0, ecp_h };

// ---                                                                                                                    hi ar no cp     td op fi
tNodeDsc dsc_message  = { 0, n_MSG,    VE_PID_IFM_MESSAGEID,         VE_PID_NODETYPE, message_props,      0,              0, 0 };
tNodeDsc dsc_msgcl    = { 0, n_MSG_CL, VE_PID_IFM_MESSAGECLASSID,    VE_PID_NODETYPE, msgcl_props,        &dsc_message,   0, 1 };

tNodeDsc imp_data_str = { 0, n_STRUCT, VE_PID_IFD_DATASTRUCTURENAME, VE_PID_NODETYPE, data_str_props,     data_str_it,    0, 0, 0, ecp_h, 0,                 " {", "}" };
tNodeDsc imp_data_memb= { 0, n_MEMB,   VE_PID_IFDM_MEMBERNAME,       VE_PID_NODETYPE, data_memb_props,    0,              0, 0, 0, ecp_h, &imp_member_type,  ";", "" };

tNodeDsc imp_s_o_meth = { 0, n_OSMETH, VE_PID_IF_OLD_SYSMETHOD_NAME, 0,               old_sys_meth_props, 0,              1, 1  };

// ---                                                                                                                    hi ar no cp     td op fi
tNodeDsc prt_root     = { 0, n_PROTO,  0,                            VE_PID_NODETYPE, prt_root_props,     &prt_iface,     0, 1 };
tNodeDsc imp_root     = { 0, n_IMP,    VE_PID_PL_PLUGINNAME,         VE_PID_NODETYPE, imp_root_props,     &imp_iface,     0, 1 };
tNodeDsc dsc_root     = { 0, n_DSC,    VE_PID_PL_DIGITALID,          VE_PID_NODETYPE, dsc_root_props,     &dsc_iface,     0, 1 };



// ---
tSymbDsc prt_method_type = {
	n_METH,
	VE_PID_IFM_RESULTTYPENAME,
	VE_PID_IFM_RESULTTYPE,
};



// ---
tSymbDsc imp_method_type = {
	n_METH,
	VE_PID_IFM_RESULTTYPENAME,
	VE_PID_IFM_RESULTTYPE,
	DSC_END_POINT,
	DSC_END_POINT,
	DSC_END_POINT,
	DSC_END_POINT,
	DSC_END_POINT,
	DSC_END_POINT,
	VE_PID_IFM_EXTENDED,
};



// ---
tSymbDsc sysmeth_type = {
	n_SMETH,
	VE_PID_IFM_RESULTTYPENAME,
	VE_PID_IFM_RESULTTYPE,
	DSC_END_POINT,
	DSC_END_POINT,
	DSC_END_POINT,
	DSC_END_POINT,
	DSC_END_POINT,
	DSC_END_POINT,
	VE_PID_IFM_SELECTED
};



// ---
tSymbDsc method_param_type = {
	0,
	VE_PID_IFMP_TYPENAME,
	VE_PID_IFMP_TYPE,
	VE_PID_IFMP_ISCONST,
	VE_PID_IFMP_ISPOINTER,
	VE_PID_IFMP_IS_DOUBLE_POINTER,
};



// ---
tSymbDsc iface_prop_type = {
	n_PROP,
	VE_PID_IFP_TYPENAME,
	VE_PID_IFP_TYPE,
	0,
	0,
	0,
	VE_PID_IFPSS_VARIABLEDEFVALUE
};



// ---
tSymbDsc iface_type_type = {
	"typedef",
	VE_PID_IFT_BASETYPENAME,
	VE_PID_IFT_BASETYPE,
	DSC_END_POINT,
	DSC_END_POINT,
	DSC_END_POINT,
	DSC_END_POINT
};



// ---
tSymbDsc iface_const_type = {
	n_CONST,
	VE_PID_IFC_BASETYPENAME,
	VE_PID_IFC_BASETYPE,
	DSC_END_POINT,
	DSC_END_POINT,
	DSC_END_POINT,
	VE_PID_IFC_VALUE
};



// ---
tSymbDsc iface_error_type = {
	n_ERR,
	DSC_END_POINT,
	DSC_END_POINT,
	DSC_END_POINT,
	DSC_END_POINT,
	DSC_END_POINT,
	VE_PID_IFE_ERRORCODEVALUE
};



// ---
tSymbDsc imp_member_type = {
	0,
	VE_PID_IFDM_TYPENAME,
	DSC_END_POINT,
	DSC_END_POINT,
	DSC_END_POINT,
	DSC_END_POINT,
	DSC_END_POINT,
	VE_PID_IFDM_ISARRAY,
	VE_PID_IFDM_ARRAYBOUND
};



// ---
tNodeDsc prt_if_it[] = {	
	{ VE_PID_IF_TYPES,      "types",      0, 0, 0, &iface_type,  1, 1 }, 
	{ VE_PID_IF_CONSTANTS,  "constants",  0, 0, 0, &iface_const, 1, 1 }, 
	{ VE_PID_IF_ERRORCODES, "errors",     0, 0, 0, &iface_error, 1, 1 }, 
	{ VE_PID_IF_PROPERTIES, "properties", 0, 0, 0, &prt_if_prp,  1, 1 }, 
	{ VE_PID_IF_METHODS,    "methods",    0, 0, 0, &prt_meth,    1, 1 }, 
	{ VE_PID_PL_MESSAGES,   "messages",   0, 0, 0, &iface_msgcl, 1, 1 }, 
	{ DSC_END_POINT } 
};


// ---
tNodeDsc imp_if_it[] = {	
	{ VE_PID_IF_TYPES,             "types",          0, 0, 0, &iface_type,   1, 1 },
	{ VE_PID_IF_CONSTANTS,         "constants",      0, 0, 0, &iface_const,  1, 1 },
	{ VE_PID_IF_ERRORCODES,        "errors",         0, 0, 0, &iface_error,  1, 1 },
	{ VE_PID_IF_DATASTRUCTURES,    "data structure", 0, 0, 0, &imp_data_str, 1, 1 },
	{ VE_PID_IF_PROPERTIES,        "properties",     0, 0, 0, &imp_if_prp,   1, 1 },
	{ VE_PID_IF_METHODS,           "methods",        0, 0, 0, &imp_meth,     1, 1 },
	{ VE_PID_PL_MESSAGES,          "messages",       0, 0, 0, &iface_msgcl,  1, 1 },
	{ VE_PID_IF_SYSTEMMETHODS,     "system methods", 0, 0, 0, &sys_meth,     1, 1 },
	{ VE_PID_IF_OLD_SYSTEMMETHODS, "old_sysmethods", 0, 0, 0, &imp_s_o_meth, 1, 1 },
	{ DSC_END_POINT } 
};


// ---
tNodeDsc dsc_if_it[] = {	
	{ VE_PID_IF_TYPES,             "types",          0, 0, 0, &iface_type,   1, 1 },
	{ VE_PID_IF_CONSTANTS,         "constants",      0, 0, 0, &iface_const,  1, 1 },
	{ VE_PID_IF_ERRORCODES,        "errors",         0, 0, 0, &iface_error,  1, 1 },
	{ VE_PID_IF_DATASTRUCTURES,    "data structure", 0, 0, 0, &imp_data_str, 1, 1 },
	{ VE_PID_IF_PROPERTIES,        "properties",     0, 0, 0, &dsc_if_prp,   1, 1 },
	{ VE_PID_IF_METHODS,           "methods",        0, 0, 0, &dsc_meth,     1, 1 },
	{ VE_PID_PL_MESSAGES,          "messages",       0, 0, 0, &dsc_msgcl,    1, 1 },
	{ VE_PID_IF_SYSTEMMETHODS,     "system methods", 0, 0, 0, &dsc_sys_meth, 1, 1 },
	{ VE_PID_IF_OLD_SYSTEMMETHODS, "old_sysmethods", 0, 0, 0, &imp_s_o_meth, 1, 1 },
	{ DSC_END_POINT } 
};


// ---
tNodeDsc data_str_it[] = {	
	{ VE_PID_IF_DATASTRUCTURES,       "structures", 0, 0, 0, &imp_data_str,  1, 1 },
	{ VE_PID_IF_DATASTRUCTUREMEMBERS, "members",    0, 0, 0, &imp_data_memb, 1, 1 },
	{ DSC_END_POINT } 
};



// ---
tEnumDsc e_node_type[] = {
	{ 0xffffffff,                n_PROTO  }, // (-1) prototype
	{ VE_NT_PLUGIN,              n_PLUG   }, // (0 ) plugin
	{ VE_NT_INTERFACE,           n_IFACE  }, // (1 ) iface
	{ VE_NT_PROPERTY,            n_PROP   }, // (2 ) property
	{ VE_NT_METHOD,              n_METH   }, // (3 ) method
	{ VE_NT_METHODPARAM,         n_METH_P }, // (4 ) method paremeter
	{ VE_NT_DATASTRUCTURE,       n_STRUCT }, // (5 ) data structure
	{ VE_NT_DATASTRUCTUREMEMBER, n_MEMB   }, // (6 ) data structure member
	{ VE_NT_TYPE,                n_TYPE   }, // (7 ) type
	{ VE_NT_CONSTANT,            n_CONST  }, // (8 ) constant
	{ VE_NT_SYSTEMMETHOD,        n_SMETH  }, // (9 ) system method
	{ VE_NT_SYSTEMMETHODPARAM,   n_METH_P }, // (10) system method  paremeter
	{ VE_NT_ERRORCODE,           n_ERR    }, // (11) error code
	{ VE_NT_MESSAGECLASS,        n_MSG_CL }, // (12) message class
	{ VE_NT_MESSAGE,             n_MSG    }, // (13) message
	{ DSC_END_POINT }
};


// ---
tEnumDsc e_type[] = {
	{ tid_VOID,       "tVOID"     }, // 0x01L,
	{ tid_BYTE,       "tBYTE"     }, // 0x02L,
	{ tid_WORD,       "tWORD"     }, // 0x03L,
	{ tid_DWORD,      "tDWORD"    }, // 0x04L,
	{ tid_QWORD,      "tQWORD"    }, // 0x05L,
	{ tid_BOOL,       "tBOOL"     }, // 0x06L,
	{ tid_CHAR,       "tCHAR"     }, // 0x07L,
	{ tid_WCHAR,      "tWCHAR"    }, // 0x08L,
	{ tid_STRING,     "tSTRING"   }, // 0x09L,
	{ tid_WSTRING,    "tWSTRING"  }, // 0x0aL,
	{ tid_ERROR,      "tERROR"    }, // 0x0bL,
	{ tid_PTR,        "tPTR"      }, // 0x0cL,
	{ tid_INT,        "tINT"      }, // 0x0dL, // native signed integer type
	{ tid_UINT,       "tUINT"     }, // 0x0eL, // native unsigned integer type
	{ tid_SBYTE,      "tSBYTE"    }, // 0x0fL,
	{ tid_SHORT,      "tSHORT"    }, // 0x11L,
	{ tid_LONG,       "tLONG"     }, // 0x12L,
	{ tid_LONGLONG,   "tLONGLONG" }, // 0x13L,
	{ tid_IID,        "tIID"      }, // 0x14L, // plugin interface identifier
	{ tid_PID,        "tPID"      }, // 0x15L, // plugin identifier
	{ tid_ORIG_ID,    "tORIG_ID"  }, // 0x16L, // object origin identifier
	{ tid_OS_ID,      "tOS_ID"    }, // 0x17L, // folder type identifier
	{ tid_VID,        "tVID"      }, // 0x18L, // vendor ideftifier
	{ tid_PROPID,     "tPROPID"   }, // 0x19L, // property identifier
	{ tid_VERSION,    "tVERSION"  }, // 0x1aL, // version of any identifier: High word - version, Low word - subversion
	{ tid_CODEPAGE,   "tCODEPAGE" }, // 0x1bL, // codepage identifier
	{ tid_LCID,       "tLCID"     }, // 0x1cL, // codepage identifier
	{ tid_DATA,       "tDATA"     }, // 0x1dL, // universaL, data storage
	{ tid_DATETIME,   "tDATETIME" }, // 0x1eL, // universaL, date storage
	{ tid_FUNC_PTR,   "tFUNC_PTR" }, // 0x1fL, // universaL, function ptr
	{ tid_DOUBLE,     "tDOUBLE"   }, // 0x20L, // double

	// advanced types
	{ tid_BINARY,       "tBINARY"        }, // 0x30L,
	{ tid_IFACEPTR,     "ifaceptr_t"     }, // 0x31L,
	{ tid_OBJECT,       "hOBJECT"        }, // 0x32L,
	{ tid_EXPORT,       "tEXPORT"        }, // 0x33L,
	{ tid_IMPORT,       "tIMPORT"        }, // 0x34L,
	{ tid_TRACE_LEVEL,  "tTRACE_LEVEL"   }, // 0x35L,
	{ tid_TYPE_ID,      "tTYPE_ID"       }, // 0x36L,
	{ tid_VECTOR,       "vector_t"       }, // 0x37L,
	{ tid_STRING_OBJ,   "cStrObj"        }, // 0x38L,
	{ tid_SERIALIZABLE, "cSerializable" }, // 0x39L,
	{ tid_BUFFER,       "buffer_t"       }, // 0x3aL,
	{ tid_STR_DSC,      "str_dsc_t"      }, // 0x3bL,
	{ tid_LAST_TYPE,    "last_type_t"    }, // 0x3cL,
	{ tid_POINTER,      "pointer"      }, // 0x80000000L,
	{ DSC_END_POINT }
};


// ---
tEnumDsc e_node_namespace[] = {
	{ VE_NTT_GLOBALTYPE, "global" }, // (0 ) // Global type
	{ VE_NTT_USERTYPE,   "user"   }, // (1 ) // User type
	{ VE_NTT_INTERFACE,  n_IFACE  }, // (2 ) // iface
	{ DSC_END_POINT }
};


// ---
tEnumDsc e_prop_shared[] = {
	{ VE_IFP_LOCAL,  "local"  },
	{ VE_IFP_SHARED, "shared" },
	{ DSC_END_POINT }
};


// ---
tEnumDsc e_func_access[] = {
	{ VE_IFPLS_NONE,      "no" },
	{ VE_IFPLS_READ,      "ro" },
	{ VE_IFPLS_WRITE,     "wr" },
	{ VE_IFPLS_READWRITE, "rw" },
	{ DSC_END_POINT }
};


// ---
tEnumDsc e_memb_access[] = {
	{ VE_IFPLS_MEMBER_READ,      "ro" },
	{ VE_IFPLS_MEMBER_WRITE,     "wo" },
	{ VE_IFPLS_MEMBER_READWRITE, "wr" },
	{ DSC_END_POINT }
};

// ---
tEnumDsc e_protected[] = {
	{ VE_IFF_PROTECTED_BY_NONE,  "none" },
	{ VE_IFF_PROTECTED_BY_CS,    "cs  " }, 
	{ VE_IFF_PROTECTED_BY_MUTEX, "mtx " },
	{ DSC_END_POINT }
};


// ---
tEnumDsc e_codepage[] = {
	{ cCP_ANSI,    "ansi"    },
	{ cCP_UNICODE, "unicode" }, 
	{ cCP_OEM,     "oem"     },
	{ DSC_END_POINT }
};


// ---
tEnumDsc e_vendors[] = {
	{ VID_ANY,                 "???any" }, // 0
	{ VID_KASPERSKY_LAB,       "kaspersky_lab"         }, // 1
	{ VID_ANDY,                "andy nikishin"         }, // 65    Andy        Nikishin
	{ VID_ANDREW,              "andrew krukov"         }, // 64    Andrew      Krukov
	{ VID_PETROVITCH,          "petrovitch"            }, // 100   Andrew      Doukhvalow        (Petrovitch)
	{ VID_GRAF,                "graf"                  }, // 66    Alexey      De Mont de Rique  (Graf)
	{ VID_SOBKO,               "sobko"                 }, // 67    Andrew      Sobko
	{ VID_COSTIN,              "costin raiu"           }, // 68    Costin      Raiu
	{ VID_VICTOR,              "victor matioushenkov"  }, // 69    Victor      Matioushenkov
	{ VID_MIKE,                "mike pavlushchik"      }, // 70    Mike        Pavlushchik
	{ VID_EUGENE,              "eugene"                }, // 71    Eugene      Kaspersky
	{ VID_ALEX,                "alex antropov"         }, // 74    Alex        Antropov
	{ VID_TAG,                 "tag (tikhonov)"        }, // 75    Andrew      Tikhonov
	{ VID_MEZHUEV,             "mezhuev"               }, // 79    Pavel       Mezhuev
	{ VID_MONASTYRSKY,         "monastyrsky"           }, // 78    Alexey      Monastyrsky
	{ VID_GUSCHIN,             "guschin"               }, // 90    Denis       Guschin
	{ VID_FEDKO,               "fedko"                 }, // 72    Oleg        Fedko        (old VID_VASILIEV)
	{ VID_DOBROVOLSKY,         "dobrovolsky"           }, // 73    Sergey      Dobrovolsky	(old VID_VALIK)
	{ VID_MELEKH,              "melekh"                }, // 76    Boris       Melekh       (old VID_TIM)
	{ VID_SHANIN,              "shanin"                }, // 77    Shanin
	{ VID_GEN,                 "gen (roschin)"         }, // 80    Eugene      Roschin
	{ VID_GLAV,                "glav"                  }, // 81    Dmitry      Glavatskikh
	{ VID_RUBIN,               "rubin"                 }, // 82    Andrey      Rubin
	{ VID_LYAK,                "lyak (lyakhovich)"     }, // 83    Alexandr    Lyakhovich
	{ VID_BATENIN,             "batenin"               }, // 84    Vyacheslav  Batenin
	{ VID_SYCHEV,              "sychev"                }, // 85    Alexandr    Sychev
	{ VID_SEROGLAZOV,          "seroglazov"            }, // 86    Artur       Seroglazov
	{ VID_ACHP,                "achp (chernyakhovsky)" }, // 87    Andrey      Chernyakhovsky
	{ VID_DENISOV,             "denisov"               }, // 88    Vitaly      Denisov
	{ VID_OVCHARIK,            "ovcharik"              }, // 89    Vladislav   Ovcharik
	{ VID_NEVSTRUEV,           "nevstruev"             }, // 91    Oleg        Nevstruev
	{ VID_KEEN,                "keen (konst lebedev)"  }, // 92    Konstantin  Lebedev
	{ VID_OVCHENKOV,           "ovchenkov"             }, // 93    Petr        Ovchenkov
	{ VID_IVANOV_A,            "ivanov_a"              }, // 94    Andrey      Ivanov
	{ VID_GERASIMOV,           "gerasimov"             }, // 95    Andrey      Gerasimov
	{ VID_VORONKOV,            "voronkov"              }, // 96    Konstantin  Voronkov
	{ VID_GRUZDEV,             "gruzdev"               }, // 97    Andrey      Gruzdev
	{ VID_LEBEDEV,             "lebedev"               }, // 98    Nikolay     Lebedev
	{ DSC_END_POINT }
};


// ---
tPropDsc common_props[] = {
	{ VE_PID_SEPARATOR,       "node_comment", 0, 1 }, // 20000
	{ VE_PID_NODECLIPTYPE,    "cliptype",     0, 1 }, // 65534
	{ VE_PID_NODEEXPANDED,    "expanded",     0, 1 }, // 65333
	{ VE_PID_SHORTCOMMENT,    "comment",      0, 0, 1 }, // 10000
	{ VE_PID_LARGECOMMENT,    "comment_ex",   0, 0, 1 }, // 10001
	{ VE_PID_BEHAVIORCOMMENT, "behv_comment", 0, 1, 1 }, // 10002
	{ VE_PID_VALUECOMMENT,    "valu_comment", 0, 1, 1 }, // 10003
	{ VE_PID_IMPLCOMMENT,     "impl_comment", 0, 1, 1 }, // 10004
	{ VE_PID_UNIQUEID,        "unique_id",    0, 1 }, // 15000
	{ VE_PID_IFM_NATIVE,      "native",       0, 1 }, // 214 - PVE set this property for all entities
	{ VE_PID_NATIVE,          "native2",      0, 1 }, // 3000
	{ VE_PID_NODETYPE,        "node_type",    e_node_type,      1 }, // 22000
	{ VE_PID_NODETYPETYPE,    "namespace",    e_node_namespace, 1 }, // 23000
	{ DSC_END_POINT }
};


// ---
tPropDsc param_common_props[] = {
	{ VE_PID_SEPARATOR,       "node_comment", 0, 1 }, // 20000
	{ VE_PID_NODECLIPTYPE,    "cliptype",     0, 1 }, // 65534
	{ VE_PID_NODEEXPANDED,    "expanded",     0, 1 }, // 65333
	{ VE_PID_SHORTCOMMENT,    "comment",      0, 0, 1 }, // 10000
	{ VE_PID_LARGECOMMENT,    "comment_ex",   0, 0, 1 }, // 10001
	{ VE_PID_BEHAVIORCOMMENT, "behv_comment", 0, 1, 1 }, // 10002
	{ VE_PID_VALUECOMMENT,    "valu_comment", 0, 1, 1 }, // 10003
	{ VE_PID_IMPLCOMMENT,     "impl_comment", 0, 1, 1 }, // 10004
	{ VE_PID_UNIQUEID,        "unique_id",    0, 1 }, // 15000
	{ VE_PID_IFM_NATIVE,      "native",       0, 1 }, // 214 - PVE set this property for all entities
	{ VE_PID_NATIVE,          "native2",      0, 1 }, // 3000
	{ VE_PID_NODETYPE,        "node_type",    e_node_type,      1 }, // 22000
	{ VE_PID_NODETYPETYPE,    "namespace",    e_node_namespace, 1 }, // 23000
	{ DSC_END_POINT }
};


// ---
tPropDsc prt_root_props[] = {
	//{ VE_PID_UNIQUEID, "unique", 0, 1   },
	//{ VE_PID_NODETYPE, "type  ", e_node_type, 1 },
	{ DSC_LINK_POINT, (char*)common_props },
	{ DSC_END_POINT }
};


// ---
tPropDsc imp_root_props[] = {
	{ VE_PID_PL_PLUGINNAME,                "name",         0       }, // 801
	{ VE_PID_PL_DIGITALID,                 "id",           0, 1 }, // 802                        
	{ VE_PID_PL_VERSIONID,                 "version",      0       }, // 803                        
	{ VE_PID_PL_MNEMONICID,                "mnemonic",     0       }, // 806                         
	{ VE_PID_PL_VENDORID,                  "vendor_id",    e_vendors, 1, 1 }, // 807                       
	{ VE_PID_PL_VENDORMNEMONICID,          "vendor_mnem",  0, 1 }, // 808                               
	{ VE_PID_PL_VENDORNAME,                "vendor",       0, 1 }, // 809                         
	{ VE_PID_PL_CODEPAGEID,                "codepage",     e_codepage }, // 810                         
	{ VE_PID_PL_DATEFORMATID,              "date_format",  0, 1 }, // 811                           
	{ VE_PID_PL_AUTHORNAME,                "author",       0, 1 }, // 812                         
	{ VE_PID_PL_PROJECTNAME,               "project",      0, 1 }, // 813                          
	{ VE_PID_PL_SUBPROJECTNAME,            "subprojct",    0, 1 }, // 814                             
	{ VE_PID_PL_AUTOSTART,                 "autostart",    0       }, // 815                        
	{ VE_PID_IF_UNK_STR_1,                 "unk_str_1",    0, 1 }, // 801
	{ VE_PID_IF_UNK_STR_2,                 "unk_str_2",    0, 1 }, // 802
	{ VE_PID_PROTOTYPEFILENAME,            n_PROTO,        0, 1 }, // 21000
	{ VE_PID_PROTOTYPERELATIVEFILENAME,    "proto_rel",    0, 1 }, // 21001

	{ VE_PID_IF_INCLUDEFILE,               "inclde_fname", 0 }, // 106
	{ VE_PID_IF_LASTINTERFACESLEVEL,       "last_if_levl", 0, 1 }, // GET_AVP_PID_ID_MAX_VALUE == GET_AVP_PID_ID(0xffffffff) == ( (0xffff&((AVP_dword)(pid))) )

	{ VE_PID_IF_INCLUDENAMES,              "includes", 0 }, // 116
	{ VE_PID_IF_INCLUDEUIDS,               "include_uids", 0, 1 }, // 129

	{ DSC_LINK_POINT, (char*)common_props },
	{ DSC_END_POINT }
};


// ---
tPropDsc dsc_root_props[] = {
	{ VE_PID_PL_PLUGINNAME,                "name",         0  }, // 801
	{ VE_PID_PL_DIGITALID,                 "id",           0, }, // 802
	{ VE_PID_PL_VERSIONID,                 "version",      0  }, // 803
	{ VE_PID_PL_MNEMONICID,                "mnemonic",     0  }, // 806
	{ VE_PID_PL_VENDORID,                  "vendor_id",    e_vendors, 1, 1 }, // 807
	{ VE_PID_PL_VENDORMNEMONICID,          "vendor_mnem",  0, 1  }, // 808
	{ VE_PID_PL_VENDORNAME,                "vendor",       0, }, // 809
	{ VE_PID_PL_CODEPAGEID,                "codepage",     e_codepage }, // 810
	{ VE_PID_PL_DATEFORMATID,              "date_format",  0, 1 }, // 811
	{ VE_PID_PL_AUTHORNAME,                "author",       0, }, // 812
	{ VE_PID_PL_PROJECTNAME,               "project",      0, 1 }, // 813
	{ VE_PID_PL_SUBPROJECTNAME,            "subprojct",    0, 1 }, // 814
	{ VE_PID_PL_AUTOSTART,                 "autostart",    0  }, // 815
	{ VE_PID_IF_UNK_STR_1,                 "unk_str_1",    0, }, // 801
	{ VE_PID_IF_UNK_STR_2,                 "unk_str_2",    0, }, // 802
	{ VE_PID_PROTOTYPEFILENAME,            "prt_file_nam", 0, }, // 21000
	{ VE_PID_PROTOTYPERELATIVEFILENAME,    "prt_relat_fn", 0, }, // 21001

	{ VE_PID_IF_INCLUDEFILE,               "inclde_fname", 0  }, // 106
	{ VE_PID_IF_LASTINTERFACESLEVEL,       "last_if_levl", 0, }, // GET_AVP_PID_ID_MAX_VALUE == GET_AVP_PID_ID(0xffffffff) == ( (0xffff&((AVP_dword)(pid))) )

	{ VE_PID_IF_INCLUDENAMES,              "includes", 0  }, // 116
	{ VE_PID_IF_INCLUDEUIDS,               "include_uids", 0, }, // 129

	{ DSC_LINK_POINT, (char*)common_props },
	{ DSC_END_POINT }
};


// ---
tPropDsc prt_iface_props[] = {
	{ VE_PID_IF_INTERFACENAME,             "name",          0, 1 }, // 101
	{ VE_PID_IF_COMMENT1,                  "old_comment1",  0, 1 }, // 108
	{ VE_PID_IF_COMMENT2,                  "old_comment2",  0, 1 }, // 109
	{ VE_PID_IF_MNEMONICID,                "mnemonic",      0    }, // 102
	{ VE_PID_IF_DIGITALID,                 "id",            0    }, // 103
	{ VE_PID_IF_REVISIONID,                "revision",      0, 1 }, // 104
	{ VE_PID_IF_CREATIONDATE,              "created",       0, 1 }, // 105
	{ VE_PID_IF_INCLUDEFILE,               "output_header", 0, 0 }, // 106
	{ VE_PID_IF_OBJECTTYPE,                "c_type_name",   0    }, // 107
	{ VE_PID_IF_SELECTED,                  "selected",      0, 1 }, // 110
	{ VE_PID_IF_HEADERGUID,                "header_guid",   0, 1 }, // 111
	{ VE_PID_IF_IMPORTED,                  "imported",      0    }, // 112
	{ VE_PID_IF_IMPORTFILENAME,            "import_fname",  0    }, // 113
	{ VE_PID_IF_SUBTYPEID,                 "sub_type_id ",  0, 1 }, // 114
	{ VE_PID_IF_AUTORNAME,                 "author",        0, 1 }, // 115
	{ VE_PID_IF_INCLUDENAMES,              "includes",      0, 1 }, // 116
	{ VE_PID_IF_STATIC,                    "static",        0, 1 }, // 117
	{ VE_PID_IF_TRANSFERPROPUP,            "trans_prp_up",  0    }, // 118
	{ VE_PID_IF_LASTCALLPROP,              "last_call_pr",  0    }, // 119
	{ VE_PID_IFLCP_READFUNC,               "lastc_r_func",  0    }, // 120
	{ VE_PID_IFLCP_WRITEFUNC,              "lastc_w_func",  0    }, // 121
	{ VE_PID_IF_NONSWAPABLE,               "non_swapable",  0    }, // 122
	{ VE_PID_IF_SELECTEDFORIMP,            "implemented ",  0, 1 }, // 123
	{ VE_PID_IF_PROTECTED_BY,              "protected_by",  e_protected, 1 }, // 130
	{ VE_PID_IF_SYSTEM,                    "system",        0    }, // 131
	{ VE_PID_IF_EXPLICIT_PARENT,           "explct_parnt",  0    }, // 132
	{ VE_PID_IF_VENDORID,                  "vendor_id",     e_vendors, 1, 1 }, // 124
	{ VE_PID_IF_IMPORTRELATIVEFILENAME,    "imprt_rel_fn",  0    }, // 125
	{ VE_PID_IF_OUTPUTFILE,                "output_source", 0    }, // 127
	{ VE_PID_LINKEDUNIQUEID,               "linked_uid",    0, 1 }, // 128
	{ VE_PID_IF_INCLUDEUIDS,               "include_uids",  0, 1 }, // 129
	{ VE_PID_IF_LASTINTERFACESLEVEL,       "last_ifc_lvl",  0, 1 }, // GET_AVP_PID_ID_MAX_VALUE == GET_AVP_PID_ID(0xffffffff) == ( (0xffff&((AVP_dword)(pid))) )
	{ VE_PID_IF_LINKEDBYINTERFACE,         "linked_by_if",  0, 1 }, // GET_AVP_PID_ID_MAX_VALUE - 1
	{ VE_PID_IF_CONTAINERNAME,             "contnr_name",   0, 1 }, // GET_AVP_PID_ID_MAX_VALUE - 2
	{ VE_PID_IF_PLUGIN_STATIC,             "plgn_static",   0    }, // 130

	{ DSC_LINK_POINT, (char*)common_props },
	{ DSC_END_POINT }
};


// ---
tPropDsc imp_iface_props[] = {
	{ VE_PID_IF_INTERFACENAME,             "name",          0, 1 }, // 101
	{ VE_PID_IF_COMMENT1,                  "old_comment1",  0, 1 }, // 108
	{ VE_PID_IF_COMMENT2,                  "old_comment2",  0, 1 }, // 109
	{ VE_PID_IF_MNEMONICID,                "mnemonic",      0, 1 }, // 102
	{ VE_PID_IF_DIGITALID,                 "id",            0, 1 }, // 103
	{ VE_PID_IF_REVISIONID,                "revision",      0, 1 }, // 104
	{ VE_PID_IF_CREATIONDATE,              "created",       0, 1 }, // 105
	{ VE_PID_IF_INCLUDEFILE,               "output_header", 0    }, // 106
	{ VE_PID_IF_OBJECTTYPE,                n_TYPE,          0, 1 }, // 107
	{ VE_PID_IF_SELECTED,                  "selected",      0, 1 }, // 110
	{ VE_PID_IF_HEADERGUID,                "header_guid",   0, 1 }, // 111
	{ VE_PID_IF_IMPORTED,                  "imported",      0    }, // 112
	{ VE_PID_IF_IMPORTFILENAME,            "import_fname",  0    }, // 113
	{ VE_PID_IF_SUBTYPEID,                 "sub_type_id",   0, 1 }, // 114
	{ VE_PID_IF_AUTORNAME,                 "author",        0, 1 }, // 115
	{ VE_PID_IF_INCLUDENAMES,              "includes",      0    }, // 116
	{ VE_PID_IF_STATIC,                    "static",        0, 1 }, // 117
	{ VE_PID_IF_TRANSFERPROPUP,            "trans_prp_up",  0    }, // 118
	{ VE_PID_IF_LASTCALLPROP,              "last_call_pr",  0    }, // 119
	{ VE_PID_IFLCP_READFUNC,               "lastc_r_func",  0    }, // 120
	{ VE_PID_IFLCP_WRITEFUNC,              "lastc_w_func",  0    }, // 121
	{ VE_PID_IF_NONSWAPABLE,               "non_swapable",  0    }, // 122
	{ VE_PID_IF_SELECTEDFORIMP,            "implemented ",  0, 1 }, // 123
	{ VE_PID_IF_PROTECTED_BY,              "protected_by",  e_protected, 1 }, // 130
	{ VE_PID_IF_SYSTEM,                    "system",        0    }, // 131
	{ VE_PID_IF_EXPLICIT_PARENT,           "explct_parnt",  0    }, // 132
	{ VE_PID_IF_VENDORID,                  "vendor_id",     e_vendors, 1, 1 }, // 124
	{ VE_PID_IF_IMPORTRELATIVEFILENAME,    "imprt_rel_fn",  0    }, // 125
	{ VE_PID_IF_OUTPUTFILE,                "output_source", 0    }, // 127
	{ VE_PID_LINKEDUNIQUEID,               "linked_uid",    0, 1 }, // 128
	{ VE_PID_IF_INCLUDEUIDS,               "include_uids",  0, 1 }, // 129
	{ VE_PID_IF_LASTINTERFACESLEVEL,       "last_ifc_lvl",  0, 1 }, // GET_AVP_PID_ID_MAX_VALUE == GET_AVP_PID_ID(0xffffffff) == ( (0xffff&((AVP_dword)(pid))) )
	{ VE_PID_IF_LINKEDBYINTERFACE,         "linked_by_if",  0, 1 }, // GET_AVP_PID_ID_MAX_VALUE - 1
	{ VE_PID_IF_CONTAINERNAME,             "contnr_name",   0, 1 }, // GET_AVP_PID_ID_MAX_VALUE - 2
	{ VE_PID_IF_PLUGIN_STATIC,             "plgn_static",   0    }, // 130

	{ DSC_LINK_POINT, (char*)common_props },
	{ DSC_END_POINT }
};


// ---
tPropDsc dsc_iface_props[] = {
	{ VE_PID_IF_INTERFACENAME,             "name",          0 }, // 101
	{ VE_PID_IF_COMMENT1,                  "old_comment1",  0 }, // 108
	{ VE_PID_IF_COMMENT2,                  "old_comment2",  0 }, // 109
	{ VE_PID_IF_MNEMONICID,                "mnemonic",      0 }, // 102
	{ VE_PID_IF_DIGITALID,                 "id",            0 }, // 103
	{ VE_PID_IF_REVISIONID,                "revision",      0 }, // 104
	{ VE_PID_IF_CREATIONDATE,              "created",       0 }, // 105
	{ VE_PID_IF_INCLUDEFILE,               "output_header", 0 }, // 106
	{ VE_PID_IF_OBJECTTYPE,                n_TYPE,          0 }, // 107
	{ VE_PID_IF_SELECTED,                  "selected",      0 }, // 110
	{ VE_PID_IF_HEADERGUID,                "header_guid",   0 }, // 111
	{ VE_PID_IF_IMPORTED,                  "imported",      0 }, // 112
	{ VE_PID_IF_IMPORTFILENAME,            "import_fname",  0 }, // 113
	{ VE_PID_IF_SUBTYPEID,                 "sub_type_id",   0, 1 }, // 114
	{ VE_PID_IF_AUTORNAME,                 "author",        0 }, // 115
	{ VE_PID_IF_INCLUDENAMES,              "includes",      0 }, // 116
	{ VE_PID_IF_STATIC,                    "static",        0 }, // 117
	{ VE_PID_IF_TRANSFERPROPUP,            "trans_prp_up",  0 }, // 118
	{ VE_PID_IF_LASTCALLPROP,              "last_call_pr",  0 }, // 119
	{ VE_PID_IFLCP_READFUNC,               "lastc_r_func",  0 }, // 120
	{ VE_PID_IFLCP_WRITEFUNC,              "lastc_w_func",  0 }, // 121
	{ VE_PID_IF_NONSWAPABLE,               "non_swapable",  0 }, // 122
	{ VE_PID_IF_SELECTEDFORIMP,            "implemented ",  0 }, // 123
	{ VE_PID_IF_PROTECTED_BY,              "protected_by",  e_protected, 1 }, // 130
	{ VE_PID_IF_SYSTEM,                    "system",        0 }, // 131
	{ VE_PID_IF_EXPLICIT_PARENT,           "explct_parnt",  0 }, // 132
	{ VE_PID_IF_VENDORID,                  "vendor_id",     e_vendors, 1, 1 }, // 124
	{ VE_PID_IF_IMPORTRELATIVEFILENAME,    "imprt_rel_fn",  0 }, // 125
	{ VE_PID_IF_OUTPUTFILE,                "output_source", 0 }, // 127
	{ VE_PID_LINKEDUNIQUEID,               "linked_uid",    0 }, // 128
	{ VE_PID_IF_INCLUDEUIDS,               "include_uids",  0 }, // 129
	{ VE_PID_IF_LASTINTERFACESLEVEL,       "last_ifc_lvl",  0 }, // GET_AVP_PID_ID_MAX_VALUE == GET_AVP_PID_ID(0xffffffff) == ( (0xffff&((AVP_dword)(pid))) )
	{ VE_PID_IF_LINKEDBYINTERFACE,         "linked_by_if",  0 }, // GET_AVP_PID_ID_MAX_VALUE - 1
	{ VE_PID_IF_CONTAINERNAME,             "contnr_name",   0 }, // GET_AVP_PID_ID_MAX_VALUE - 2
	{ VE_PID_IF_PLUGIN_STATIC,             "plgn_static",   0 }, // 130

	{ DSC_LINK_POINT, (char*)common_props },
	{ DSC_END_POINT }
};


// ---
tPropDsc prop_variable_defs[] = {
	{ VE_PID_IFPSS_VARTYPIFIEDDEFVALUE(avpt_dword),    "v_def(dw)", 0, 1 },
	{ VE_PID_IFPSS_VARTYPIFIEDDEFVALUE(avpt_str),      "v_def(st)", 0, 1 },
	{ VE_PID_IFPSS_VARTYPIFIEDDEFVALUE(avpt_bool),     "v_def(bo)", 0, 1 },
	{ VE_PID_IFPSS_VARTYPIFIEDDEFVALUE(avpt_byte),     "v_def(by)", 0, 1 },
	{ VE_PID_IFPSS_VARTYPIFIEDDEFVALUE(avpt_word),     "v_def(wo)", 0, 1 },
	{ VE_PID_IFPSS_VARTYPIFIEDDEFVALUE(avpt_bin),      "v_def(bi)", 0, 1 },
	{ VE_PID_IFPSS_VARTYPIFIEDDEFVALUE(avpt_qword),    "v_def(qw)", 0, 1 },
	{ VE_PID_IFPSS_VARTYPIFIEDDEFVALUE(avpt_datetime), "v_def(dt)", 0, 1 },
	{ VE_PID_IFPSS_VARTYPIFIEDDEFVALUE(avpt_int),      "v_def(in)", 0, 1 },
	{ VE_PID_IFPSS_VARTYPIFIEDDEFVALUE(avpt_uint),     "v_def(ui)", 0, 1 },
	{ VE_PID_IFPSS_VARTYPIFIEDDEFVALUE(avpt_char),     "v_def(ch)", 0, 1 },
	{ VE_PID_IFPSS_VARTYPIFIEDDEFVALUE(avpt_wstr),     "v_def(ws)", 0, 1 },
	{ DSC_END_POINT }
};


// ---
tPropDsc prt_property_props[] = {
	{ VE_PID_IFP_PROPNAME,                 "name",         0,             1 }, // 201
	{ VE_PID_IFP_DIGITALID,                "id",           0,               }, // 202
	{ VE_PID_IFP_TYPE,                     "type_id",      e_type,        1 }, // 203
	{ VE_PID_IFP_TYPENAME,                 n_TYPE,         0,             1 }, // 204
	{ VE_PID_IFP_SCOPE,                    "scope",        e_prop_shared, 1 }, // 205
	{ VE_PID_IFPLS_REQUIRED,               "required",     0,             1 }, // 206
	{ VE_PID_IFPLS_MEMBER,                 "member_ref",   0,             1 }, // 207
	{ VE_PID_IFPLS_MODE,                   "func_imp_mod", e_func_access, 1 }, // 208
	{ VE_PID_IFPLS_READFUNC,               "read_func",    0,             1 }, // 209
	{ VE_PID_IFPLS_WRITEFUNC,              "write_func",   0,             1 }, // 210
	{ VE_PID_IFPSS_VARIABLENAME,           "var_name",     0,             1 }, // 211
	{ VE_PID_IFPSS_VARIABLEDEFVALUE,       "var_default",  0,             1, 1 }, // 212
	{ VE_PID_IFPLS_MEMBER_MODE,            "mmbr_imp_mod", e_memb_access, 1 }, // 215
	{ VE_PID_IFP_PREDEFINED,               "predefined",   0,             1 }, // 216
	{ VE_PID_IFP_HARDPREDEFINED,           "predefined_2", 0,             1 }, // 217
	{ VE_PID_IFPLS_WRITABLEDURINGINIT,     "write_on_init",0,             1 }, // 219

	{ DSC_LINK_POINT, (char*)prop_variable_defs},
	{ DSC_LINK_POINT, (char*)common_props },
	{ DSC_END_POINT }
};


// ---
tPropDsc imp_property_props[] = {
	{ VE_PID_IFP_PROPNAME,                 "name",         0,             1 }, // 201
	{ VE_PID_IFP_DIGITALID,                "id",           0,             0 }, // 202
	{ VE_PID_IFP_TYPE,                     "type_id",      e_type,        1 }, // 203
	{ VE_PID_IFP_TYPENAME,                 n_TYPE,         0,             1 }, // 204
	{ VE_PID_IFP_SCOPE,                    "scope",        e_prop_shared, 1 }, // 205
	{ VE_PID_IFPLS_REQUIRED,               "required",     0,             1 }, // 206
	{ VE_PID_IFPLS_MEMBER,                 "member_ref",   0,             1 }, // 207
	{ VE_PID_IFPLS_MODE,                   "func_imp_mod", e_func_access, 1 }, // 208
	{ VE_PID_IFPLS_READFUNC,               "read_func",    0,             1 }, // 209
	{ VE_PID_IFPLS_WRITEFUNC,              "write_func",   0,             1 }, // 210
	{ VE_PID_IFPSS_VARIABLENAME,           "var_name",     0,             1 }, // 211
	{ VE_PID_IFPSS_VARIABLEDEFVALUE,       "var_default",  0,             1, 1 }, // 212
	{ VE_PID_IFPLS_MEMBER_MODE,            "mmbr_imp_mod", e_memb_access, 1 }, // 215
	{ VE_PID_IFP_PREDEFINED,               "predefined",   0,             1 }, // 216
	{ VE_PID_IFP_HARDPREDEFINED,           "predefined_2", 0,             1 }, // 217
	{ VE_PID_IFPLS_WRITABLEDURINGINIT,     "write_on_init",0,             1 }, // 219

	{ DSC_LINK_POINT, (char*)prop_variable_defs},
	{ DSC_LINK_POINT, (char*)common_props },
	{ DSC_END_POINT }
};


// ---
tPropDsc dsc_property_props[] = {
	{ VE_PID_IFP_PROPNAME,                 "name",         0 }, // 201
	{ VE_PID_IFP_DIGITALID,                "id",           0 }, // 202
	{ VE_PID_IFP_TYPE,                     "type_id",      e_type }, // 203
	{ VE_PID_IFP_TYPENAME,                 n_TYPE,         0 }, // 204
	{ VE_PID_IFP_SCOPE,                    "scope",        e_prop_shared }, // 205
	{ VE_PID_IFPLS_REQUIRED,               "required",     0 }, // 206
	{ VE_PID_IFPLS_MEMBER,                 "member_ref",   0 }, // 207
	{ VE_PID_IFPLS_MODE,                   "func_imp_mod", e_func_access }, // 208
	{ VE_PID_IFPLS_READFUNC,               "read_func",    0 }, // 209
	{ VE_PID_IFPLS_WRITEFUNC,              "write_func",   0 }, // 210
	{ VE_PID_IFPSS_VARIABLENAME,           "var_name",     0 }, // 211
	{ VE_PID_IFPSS_VARIABLEDEFVALUE,       "var_default",  0, 0, 1 }, // 212
	{ VE_PID_IFPLS_MEMBER_MODE,            "mmbr_imp_mod", e_memb_access }, // 215
	{ VE_PID_IFP_PREDEFINED,               "predefined",   0 }, // 216
	{ VE_PID_IFP_HARDPREDEFINED,           "predefined_2", 0 }, // 217
	{ VE_PID_IFPLS_WRITABLEDURINGINIT,     "write_on_init",0 }, // 219

	{ DSC_LINK_POINT, (char*)prop_variable_defs},
	{ DSC_LINK_POINT, (char*)common_props },
	{ DSC_END_POINT }
};


// ---
tPropDsc prt_method_props[] = {
	{ VE_PID_IFM_METHODNAME,     "name",        0, 1 }, // 301
	{ VE_PID_IFM_RESULTTYPE,     "res_type_id", e_type, 1 }, // 306
	{ VE_PID_IFM_RESULTTYPENAME, "res_type",    0, 1 }, // 307
	{ VE_PID_IFM_RESULTDEFVALUE, "res_def_val", 0, 1 }, // 308
	{ VE_PID_IFM_SELECTED,       "selected",    0, 1 }, // 309
	{ VE_PID_IFM_METHOD_ID,      "method_id",   0, 1 }, // 310
	{ VE_PID_IFM_EXTENDED,       "extended",    0, 1 }, // 311

	{ DSC_LINK_POINT, (char*)common_props },
	{ DSC_END_POINT }
};


// ---
tPropDsc imp_method_props[] = {
	{ VE_PID_IFM_METHODNAME,     "name",        0, 1 }, // 301
	{ VE_PID_IFM_RESULTTYPE,     "res_type_id", e_type, 1 }, // 306
	{ VE_PID_IFM_RESULTTYPENAME, "res_type",    0, 1 }, // 307
	{ VE_PID_IFM_RESULTDEFVALUE, "res_def_val", 0, 1 }, // 308
	{ VE_PID_IFM_SELECTED,       "selected",    0, 1 }, // 309
	{ VE_PID_IFM_METHOD_ID,      "method_id",   0, 1 }, // 310
	{ VE_PID_IFM_EXTENDED,       "extended",    0, 1 }, // 311

	{ DSC_LINK_POINT, (char*)common_props },
	{ DSC_END_POINT }
};


// ---
tPropDsc dsc_method_props[] = {
	{ VE_PID_IFM_METHODNAME,     "name",        0 }, // 301
	{ VE_PID_IFM_RESULTTYPE,     "res_type_id", e_type }, // 306
	{ VE_PID_IFM_RESULTTYPENAME, "res_type",    0 }, // 307
	{ VE_PID_IFM_RESULTDEFVALUE, "res_def_val", 0 }, // 308
	{ VE_PID_IFM_SELECTED,       "selected",    0 }, // 309
	{ VE_PID_IFM_METHOD_ID,      "method_id",   0 }, // 310
	{ VE_PID_IFM_EXTENDED,       "extended",    0 }, // 311

	{ DSC_LINK_POINT, (char*)common_props },
	{ DSC_END_POINT }
};


// ---
tPropDsc dsc_sys_method_props[] = {
	{ VE_PID_IFM_METHODNAME,     "name",        0 }, // 301
	{ VE_PID_IFM_RESULTTYPE,     "res_type_id", e_type }, // 306
	{ VE_PID_IFM_RESULTTYPENAME, "res_type",    0 }, // 307
	{ VE_PID_IFM_RESULTDEFVALUE, "res_def_val", 0 }, // 308
	{ VE_PID_IFM_SELECTED,       "selected",    0, 1 }, // 309
	{ VE_PID_IFM_METHOD_ID,      "method_id",   0 }, // 310
	{ VE_PID_IFM_EXTENDED,       "extended",    0 }, // 311

	{ DSC_LINK_POINT, (char*)common_props },
	{ DSC_END_POINT }
};


// ---
tPropDsc sys_method_props[] = {
	{ VE_PID_IFM_METHODNAME,     "name",        0, 1 }, // 301
	{ VE_PID_IFM_RESULTTYPE,     "res_type_id", e_type, 1 }, // 306
	{ VE_PID_IFM_RESULTTYPENAME, "res_type",    0, 1 }, // 307
	{ VE_PID_IFM_RESULTDEFVALUE, "res_def_val", 0, 1 }, // 308
	{ VE_PID_IFM_SELECTED,       "selected",    0, 1 }, // 309
	{ VE_PID_IFM_METHOD_ID,      "method_id",   0, 1 }, // 310
	{ VE_PID_IFM_EXTENDED,       "extended",    0, 1 }, // 311

	{ DSC_LINK_POINT, (char*)common_props },
	{ DSC_END_POINT }
};



// ---
tPropDsc param_props[] = {
	{ VE_PID_IFMP_PARAMETERNAME,     "name",        0, 1 }, // 401
	{ VE_PID_IFMP_TYPENAME,          n_TYPE,        0, 1 }, // 402
	{ VE_PID_IFMP_TYPE,              "type_id",     e_type, 1 }, // 403
	{ VE_PID_IFMP_ISCONST,           n_CONST,       0, 1 }, // 405
	{ VE_PID_IFMP_ISPOINTER,         "pointer",     0, 1 }, // 406
	{ VE_PID_IFMP_IS_DOUBLE_POINTER, "dbl_pointer", 0, 1 }, // 407

	{ DSC_LINK_POINT, (char*)param_common_props },
	{ DSC_END_POINT }
};


// ---
tPropDsc dsc_param_props[] = {
	{ VE_PID_IFMP_PARAMETERNAME,     "name",        0 }, // 401
	{ VE_PID_IFMP_TYPENAME,          n_TYPE,        0 }, // 402
	{ VE_PID_IFMP_TYPE,              "type_id",     e_type }, // 403
	{ VE_PID_IFMP_ISCONST,           n_CONST,       0 }, // 405
	{ VE_PID_IFMP_ISPOINTER,         "pointer",     0 }, // 406
	{ VE_PID_IFMP_IS_DOUBLE_POINTER, "dbl_pointer", 0 }, // 407

	{ DSC_LINK_POINT, (char*)param_common_props },
	{ DSC_END_POINT }
};


// ---
tPropDsc type_props[] = {
	{ VE_PID_IFT_TYPENAME,     "name",         0, 1 }, // 901
	{ VE_PID_IFT_BASETYPENAME, "base_type_nm", 0, 1 }, // 902
	{ VE_PID_IFT_BASETYPE,     "base_type_id", e_type, 1 }, // 903
	{ VE_PID_IFT_NATIVE,       "native",       0, 1 }, // 904

	{ DSC_LINK_POINT, (char*)common_props },
	{ DSC_END_POINT }
};


// ---
tPropDsc const_props[] = {
	{ VE_PID_IFC_CONSTANTNAME, "name",         0, 1 }, // 1001
	{ VE_PID_IFC_BASETYPENAME, "base_type_nm", 0, 1 }, // 1002
	{ VE_PID_IFC_BASETYPE,     "base_type_id", e_type, 1 }, // 1003
	{ VE_PID_IFC_VALUE,        "value",        0, 1 }, // 1004
	{ VE_PID_IFC_NATIVE,       "native",       0, 1 }, // 1005

	{ DSC_LINK_POINT, (char*)common_props },
	{ DSC_END_POINT }
};



// ---
tPropDsc error_props[] = {
	{ VE_PID_IFE_ERRORCODENAME,  "name",    0, 1 }, // 1101
	{ VE_PID_IFE_ERRORCODEVALUE, "value",   0, 1 }, // 1102
	{ VE_PID_IFE_ISWARNING,      "warning", 0       }, // 1103
	{ VE_PID_IFE_NATIVE,         "native",  0, 1 }, // 1104

	{ DSC_LINK_POINT, (char*)common_props },
	{ DSC_END_POINT }
};



// ---
tPropDsc msgcl_props[] = {
	{ VE_PID_IFM_MESSAGECLASSNAME, "name",  0, 1 }, // 1151
	{ VE_PID_IFM_MESSAGECLASSID,   "id",    0    }, // 1152

	{ DSC_LINK_POINT, (char*)common_props },
	{ DSC_END_POINT }
};



// ---
tPropDsc message_props[] = {
	{ VE_PID_IFM_MESSAGENAME,        "name",         0 }, // 1201
	{ VE_PID_IFM_MESSAGEID,          "id",           0 }, // 1202
	{ VE_PID_IFM_MESSAGECONTEXT,     "ctx_prm_dsc",  0, 1, 1 }, // 1203
	{ VE_PID_IFM_MESSAGEDATA,        "data_prm_dsc", 0, 1, 1 }, // 1204
	{ VE_PID_IFM_MESSAGE_SEND_POINT, "send_pnt_dsc", 0, 1, 1 }, // 1205

	{ DSC_LINK_POINT, (char*)common_props },
	{ DSC_END_POINT }
};



// ---
tPropDsc dsc_message_props[] = {
	{ VE_PID_IFM_MESSAGENAME,        "name",         0 }, // 1201
	{ VE_PID_IFM_MESSAGEID,          "id",           0 }, // 1202
	{ VE_PID_IFM_MESSAGECONTEXT,     "ctx_prm_dsc",  0, 1, 1 }, // 1203
	{ VE_PID_IFM_MESSAGEDATA,        "data_prm_dsc", 0, 1, 1 }, // 1204
	{ VE_PID_IFM_MESSAGE_SEND_POINT, "send_pnt_dsc", 0, 1, 1 }, // 1205

	{ DSC_LINK_POINT, (char*)common_props },
	{ DSC_END_POINT }
};



// ---
tPropDsc data_str_props[] = {
	{ VE_PID_IFD_DATASTRUCTURENAME, "name", 0, 1 }, // 501

	{ DSC_LINK_POINT, (char*)common_props },
	{ DSC_END_POINT }
};



// ---
tPropDsc data_memb_props[] = {
	{ VE_PID_IFDM_MEMBERNAME,        "name",        0, 1 }, // 601
	{ VE_PID_IFDM_TYPENAME,          n_TYPE,        0, 1 }, // 602
	{ VE_PID_IFDM_OLD_SHORT_COMMENT, "old_comment", 0, 1 }, // 603
	{ VE_PID_IFDM_ISARRAY,           "array",       0, 1 }, // 604
	{ VE_PID_IFDM_ARRAYBOUND,        "array_bound", 0, 1 }, // 605

	{ DSC_LINK_POINT, (char*)common_props },
	{ DSC_END_POINT }
};


// ---
tPropDsc old_sys_meth_props[] = {
	{ VE_PID_IF_OLD_SYSMETHOD_NAME,     "name",     0, 1 }, // 401
	{ VE_PID_IF_OLD_SYSMETHOD_SELECTED, "selected", 0, 1 }, // 402

	{ DSC_LINK_POINT, (char*)common_props },
	{ DSC_END_POINT }
};


const char* g_known_props[] = {
	"NATIVE_ERR",
	"OBJECT_NAME",
	"OBJECT_NAME_CP",
	"OBJECT_FULL_NAME",
	"OBJECT_FULL_NAME_CP",
	"OBJECT_COMPLETE_NAME",
	"OBJECT_COMPLETE_NAME_CP",
	"OBJECT_ALT_NAME",
	"OBJECT_ALT_NAME_CP",
	"OBJECT_PATH",
	"OBJECT_PATH_CP",
	"OBJECT_SIZE",
	"OBJECT_SIZE_Q",
	"OBJECT_SIGNATURE",
	"OBJECT_SUPPORTED",
	"OBJECT_ORIGIN",
	"OBJECT_OS_TYPE",
	"OBJECT_OPEN_MODE",
	"OBJECT_NEW",
	"OBJECT_CODEPAGE",
	"OBJECT_LCID",
	"OBJECT_ACCESS_MODE",
	"OBJECT_USAGE_COUNT",
	"OBJECT_COMPRESSED_SIZE",
	"OBJECT_HEAP",
	"OBJECT_AVAILABILITY",
	"OBJECT_BASED_ON",
	"OBJECT_ATTRIBUTES",
	"OBJECT_HASH",
	"OBJECT_REOPEN_DATA",
	"OBJECT_INHERITABLE_HEAP",
	"OBJECT_NATIVE_IO",
	"OBJECT_PROP_SYNCHRONIZED",
	"OBJECT_MEM_SYNCHRONIZED",
	"OBJECT_NATIVE_PATH",
};

// ---
bool _known_prop( const char* prop ) {
	for( uint i=0; i<countof(g_known_props); ++i )
		if ( !strcmp(prop,g_known_props[i]) )
			return true;
	return false;
}

