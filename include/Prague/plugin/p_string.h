// AVP Prague stamp begin( Interface header,  )
// -------- ANSI C++ Code Generator 1.0 --------
// -------- Friday,  23 September 2005,  15:09 --------
// -------------------------------------------
// Copyright © Kaspersky Lab 1996-2004.
// -------------------------------------------
// Project     -- Not defined
// Sub project -- Not defined
// Purpose     -- Not defined
// Author      -- petrovitch
// File Name   -- plugin_string.cpp
// -------------------------------------------
// AVP Prague stamp end



// AVP Prague stamp begin( Header ifndef,  )
#if !defined( __public_plugin_string )
#define __public_plugin_string
// AVP Prague stamp end



// AVP Prague stamp begin( Header includes,  )
#include <Prague/prague.h>
#include <Prague/iface/i_string.h>
// AVP Prague stamp end



// AVP Prague stamp begin( Plugin identifier,  )
#define PID_STRING  ((tPID)(38))
// AVP Prague stamp end



// AVP Prague stamp begin( Vendor identifier,  )
#define VID_PETROVITCH  ((tVID)(100))
// AVP Prague stamp end



// AVP Prague stamp begin( Plugin public definitions, String )
// String interface implementation
// Short comments -- String operations

// constants
#define fSTRING_COMPARE_SLASH_SENSITIVE ((tDWORD)(0x00000000)) //  --
#define fSTRING_COMPARE_SLASH_INSENSITIVE ((tDWORD)(0x08000000)) //  --
#define cSTRING_WHOLE_LENGTH           ((tWORD)(-1)) //  --
#define cSTRING_EMPTY_LENGTH           ((tWORD)(-2)) //  --
// AVP Prague stamp end



// AVP Prague stamp begin( Interface comment,  )

// String interface implementation
// Short comments -- String operations
//    Interface implements string operations
// AVP Prague stamp end



// AVP Prague stamp begin( ,  )

// String declarations

// String declarations

typedef struct tag_String {
  tWCHAR*         m_data;
  unsigned int    m_len;
  unsigned int    m_size;
#ifdef __cplusplus
  tag_String () : m_data ( 0 ), m_len ( 0 ), m_size ( 0 ) {}
#endif
} sString;

// String declarations
// AVP Prague stamp end



// AVP Prague stamp begin( Header endif,  )
#endif // __public_plugin_string
// AVP Prague stamp end



// AVP Prague stamp begin( Plugin export methods,  )
#ifdef IMPEX_TABLE_CONTENT
#ifndef STRING_TABLE_CONTENT
#define STRING_TABLE_CONTENT
{ (tDATA)&CalcExportLen, PID_STRING, 0x8eefa2b8l },
{ (tDATA)&CopyTo, PID_STRING, 0x10a92e27l },
{ (tDATA)&CopyToEx, PID_STRING, 0x941affb2l },
{ (tDATA)&ConvertLen, PID_STRING, 0xab7270acl },
{ (tDATA)&ImportFromProp, PID_STRING, 0xed035ef3l },
{ (tDATA)&CmpStr, PID_STRING, 0xa931f60al },
{ (tDATA)&GetSlashSymbol, PID_STRING, 0x60e03b91l },
{ (tDATA)&ToUpperCase, PID_STRING, 0xdae8a8d0l },
{ (tDATA)&ToLowerCase, PID_STRING, 0xba2d0c6fl },
{ (tDATA)&CompareBuff, PID_STRING, 0xcb15477bl },
{ (tDATA)&FindBuff, PID_STRING, 0x3ee390d3l },
{ (tDATA)&GetSystemDefaultCp, PID_STRING, 0xe641ffe6l },
{ (tDATA)&ReplaceBuffInPos, PID_STRING, 0x0a02700bl },
{ (tDATA)&ReplaceBuff, PID_STRING, 0x0d5e9cc5l },
{ (tDATA)&CompareStr, PID_STRING, 0x65bbab8bl },
{ (tDATA)&FindStr, PID_STRING, 0x9ca854dcl },
{ (tDATA)&ReplaceStrInPos, PID_STRING, 0x9fcbb7ecl },
{ (tDATA)&ReplaceStr, PID_STRING, 0x22b9a7b1l },
{ (tDATA)&FindOneOfBuff, PID_STRING, 0x7ca05251l },
{ (tDATA)&FindOneOfStr, PID_STRING, 0x6df2baf5l },
{ (tDATA)&CalcImportLen, PID_STRING, 0x5c806ee3l },
{ (tDATA)&GetMaxANSICharSize, PID_STRING, 0x51d98467l },
#endif
#elif defined(IMPEX_EXPORT_LIB) || defined(IMPEX_IMPORT_LIB)
#include <Prague/iface/impexhlp.h>
#ifndef STRING_EXPORT_PROTOTYPES
#define STRING_EXPORT_PROTOTYPES
IMPEX_DECL tERROR IMPEX_FUNC(CalcExportLen)( const tPTR p_src_str, tDWORD p_src_len, tCODEPAGE p_src_cp, tCODEPAGE p_dst_cp, tDWORD p_dst_flags, tDWORD* p_dst_len )IMPEX_INIT;
IMPEX_DECL tERROR IMPEX_FUNC(CopyTo)( tPTR p_dst_str, tDWORD p_dst_len, tCODEPAGE p_dst_cp, const tPTR p_src_str, tDWORD p_src_len, tCODEPAGE p_src_cp, tDWORD p_flags, tDWORD* p_output_len )IMPEX_INIT;
IMPEX_DECL tERROR IMPEX_FUNC(CopyToEx)( tPTR p_dst_str, tDWORD p_dst_len, tCODEPAGE p_dst_cp, tDWORD p_dst_flags, const tPTR p_src_str, tDWORD p_src_len, tCODEPAGE p_src_cp, tDWORD p_src_flags, tDWORD* p_output_len )IMPEX_INIT;
IMPEX_DECL tERROR IMPEX_FUNC(ConvertLen)( tDWORD p_src_len, tDWORD* p_dst_len, tCODEPAGE p_src_cp, tCODEPAGE p_dst_cp )IMPEX_INIT;
IMPEX_DECL tERROR IMPEX_FUNC(ImportFromProp)( hOBJECT p_src_obj, tPROPID p_src_propid, tPTR p_dst_buff, tDWORD p_dst_len, tDWORD p_dst_cp, tDWORD* p_result_len )IMPEX_INIT;
IMPEX_DECL tERROR IMPEX_FUNC(CmpStr)( const tVOID* p_str1, tDWORD p_str1_len, tCODEPAGE p_str1_cp, const tVOID* p_str2, tDWORD p_str2_len, tCODEPAGE p_str2_cp, tDWORD p_flags )IMPEX_INIT;
IMPEX_DECL tSYMBOL IMPEX_FUNC(GetSlashSymbol)()IMPEX_INIT;
IMPEX_DECL tERROR IMPEX_FUNC(ToUpperCase)( tVOID* p_str, tDWORD p_len, tCODEPAGE p_cp )IMPEX_INIT;
IMPEX_DECL tERROR IMPEX_FUNC(ToLowerCase)( tVOID* p_str, tDWORD p_len, tCODEPAGE p_cp )IMPEX_INIT;
IMPEX_DECL tERROR IMPEX_FUNC(CompareBuff)( const sString* p_str, tDWORD p_pos, tDWORD p_size, const tVOID* p_other_str, tDWORD p_len, tCODEPAGE p_cp, tDWORD p_flags, tSTR_COMPARE* p_result )IMPEX_INIT;
IMPEX_DECL tERROR IMPEX_FUNC(FindBuff)( const sString* p_str, tDWORD p_pos, tDWORD p_size, const tVOID* p_other_str, tDWORD p_len, tCODEPAGE p_cp, tDWORD p_flags, tDWORD* p_result )IMPEX_INIT;
IMPEX_DECL tCODEPAGE IMPEX_FUNC(GetSystemDefaultCp)()IMPEX_INIT;
IMPEX_DECL tERROR IMPEX_FUNC(ReplaceBuffInPos)( sString* p_str, tDWORD p_pos, tDWORD p_size, const tVOID* p_other_str, tDWORD p_len, tCODEPAGE p_cp )IMPEX_INIT;
IMPEX_DECL tERROR IMPEX_FUNC(ReplaceBuff)( sString* p_str, const tVOID* p_pattern, tDWORD p_pattern_len, tCODEPAGE p_pattern_cp, tDWORD p_pattern_flags, const tVOID* p_replace_with, tDWORD p_replace_with_len, tCODEPAGE p_replace_with_cp )IMPEX_INIT;
IMPEX_DECL tERROR IMPEX_FUNC(CompareStr)( const sString* p_str, tDWORD p_pos, tDWORD p_size, const hSTRING p_other_str, tDWORD p_other_pos, tDWORD p_other_size, tDWORD p_flags, tSTR_COMPARE* p_result )IMPEX_INIT;
IMPEX_DECL tERROR IMPEX_FUNC(FindStr)( const sString* p_str, tDWORD p_pos, tDWORD p_size, hSTRING p_other_str, tDWORD p_other_pos, tDWORD p_other_size, tDWORD p_flags, tDWORD* p_result )IMPEX_INIT;
IMPEX_DECL tERROR IMPEX_FUNC(ReplaceStrInPos)( sString* p_str, tDWORD p_pos, tDWORD p_size, hSTRING p_other_str, tDWORD p_other_pos, tDWORD p_other_size )IMPEX_INIT;
IMPEX_DECL tERROR IMPEX_FUNC(ReplaceStr)( sString* p_str, hSTRING p_pattern, tDWORD p_pattern_pos, tDWORD p_pattern_size, tDWORD p_pattern_flags, hSTRING p_replace_with, tDWORD p_replace_with_pos, tDWORD p_replace_with_size )IMPEX_INIT;
IMPEX_DECL tERROR IMPEX_FUNC(FindOneOfBuff)( const sString* p_str, tDWORD p_pos, tDWORD p_size, const tVOID* p_other_str, tDWORD p_len, tCODEPAGE p_cp, tDWORD p_flags, tBOOL p_is_included, tDWORD* p_result )IMPEX_INIT;
IMPEX_DECL tERROR IMPEX_FUNC(FindOneOfStr)( const sString* p_str, tDWORD p_pos, tDWORD p_size, hSTRING p_other_str, tDWORD p_other_pos, tDWORD p_other_size, tDWORD p_flags, tBOOL p_is_included, tDWORD* p_result )IMPEX_INIT;
IMPEX_DECL tERROR IMPEX_FUNC(CalcImportLen)( const tVOID* p_from, tCODEPAGE p_cp, tDWORD p_flags, tDWORD p_len, tDWORD* p_olen )IMPEX_INIT;
IMPEX_DECL tDWORD IMPEX_FUNC(GetMaxANSICharSize)()IMPEX_INIT;
#endif
#endif
// AVP Prague stamp end



