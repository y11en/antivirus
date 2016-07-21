// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface header,  )
// -------- ANSI C Code Generator 2.0 --------
// -------- Tuesday,  15 March 2005,  19:04 --------
// -------------------------------------------
// Copyright © Kaspersky Labs 1996-2004.
// -------------------------------------------
// Project     -- Not defined
// Sub project -- Not defined
// Purpose     -- Not defined
// Author      -- petrovitch
// File Name   -- pstring.c
// -------------------------------------------
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Header ifndef,  )
#if !defined( __pstring_c__a3f35cc6_5fdf_4dcf_8c22_4b50f4423236 )
#define __pstring_c__a3f35cc6_5fdf_4dcf_8c22_4b50f4423236
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Header includes,  )
#include <Prague/prague.h>
#include <Prague/iface/i_io.h>
#include <Prague/iface/i_root.h>
#include <Prague/iface/i_seqio.h>
#include <Prague/plugin/p_string.h>
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface constants,  )
#define fSTRING_COMPARE_SLASH_SENSITIVE ((tDWORD)(0x00000000)) //  --
#define fSTRING_COMPARE_SLASH_INSENSITIVE ((tDWORD)(0x08000000)) //  --
#define cSTRING_WHOLE_LENGTH           ((tWORD)(-1)) //  --
#define cSTRING_EMPTY_LENGTH           ((tWORD)(-2)) //  --
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface defined messages,  )
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Plugin identifier,  )
//! Section moved to correspondent "plugin_*.c" or "plugin_*.h" file
//! #define PID_STRING  ((tPID)(38))
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Vendor identifier,  )
//! Section moved to correspondent "plugin_*.c" or "plugin_*.h" file
//! #define VID_PETROVITCH  ((tVID)(100))
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface comment,  )
// String interface implementation
// Short comments -- String operations
//    Interface implements string operations
// AVP Prague stamp end



#ifdef __cplusplus
  extern "C"
#endif
// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Registration call prototype,  )
  tERROR pr_call String_Register( hROOT root );
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Data structure,  )
// Interface String. Inner data structure

typedef struct tag_StringData {
  tCODEPAGE cp;      // current code page
  tLCID     lcid;    // current locale id
  tUINT     len;     // current string len
  tUINT     blen;    // allocated buffer len
  tCHAR*    buff;    // string buffer
} StringData;


// to get pointer to the structure StringData from the hOBJECT obj
// get pointer to the structure StringData from the hOBJECT obj
#define CUST_TO_StringData(object)  ((StringData*)( (((tPTR*)object)[2]) ))
// AVP Prague stamp end



#define MAP_CP(p)                                              \
  switch( p ) {                                                \
    case cCP_NULL           : p = cCP_ANSI;            break;  \
    case cCP_SYSTEM_DEFAULT : p = g_cp_system_default; break;  \
    case cCP_STRING_DEFAULT : p = d->cp;               break;  \
  }

#define KNOWN_CP(cp)      ((cp==cCP_ANSI) || (cp==cCP_OEM) || (cp==cCP_UNICODE))
#define WCFL              (WC_COMPOSITECHECK|WC_DISCARDNS|WC_DEFAULTCHAR)  // MB_USEGLYPHCHARS|WC_SEPCHARS|

#define MB(s)    ((tCHAR*)(s))
#define MBR(s)   (*(tBYTE**)&(s))

#define UNI(s)   ((tWCHAR*)(s))
#define UNIR(s)  (*(tWCHAR**)&(s))


#define cSTRING_STRICT_SIZE  ((tDWORD)(0x00100000))

extern
#if defined( __cplusplus )
 "C" 
#endif
 tCODEPAGE g_cp_system_default;

// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Property table,  )
// AVP Prague stamp end



tERROR       _cmp ( tPTR s1, tPTR s2, tCODEPAGE cp, tUINT len, tDWORD flags );
tSTR_COMPARE _cmpS( tPTR s1, tPTR s2, tCODEPAGE cp, tUINT len, tDWORD flags );
tBOOL        _cmpS_ASCII( tSTR_COMPARE* presult, tStrDsc* ps1, tStrDsc* ps2, tDWORD flags);
// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Header endif,  )
#endif // pstring_c
// AVP Prague stamp end





