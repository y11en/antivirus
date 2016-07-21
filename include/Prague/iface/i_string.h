// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface header,  )
// -------- Tuesday,  15 March 2005,  18:58 --------
// File Name   -- (null)i_string.c
// Copyright © Kaspersky Labs 1996-2004.
// -------------------------------------------
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Header ifndef,  )
#if !defined( __i_string__37b1402e_ae7b_11d4_b757_00d0b7170e50 )
#define __i_string__37b1402e_ae7b_11d4_b757_00d0b7170e50
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Header includes,  )
// AVP Prague stamp end



#include <Prague/pr_vtbl.h>

// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface comment,  )
// String interface implementation
// Short comments -- String operations
//    Interface implements string operations
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface menonic identificator,  )
#define IID_STRING  ((tIID)(100))
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface typedefs,  )
typedef tDWORD                         tSTR_RANGE; //  --
typedef tDWORD                         tSTR_CASE; //  --
typedef tINT                           tSTR_COMPARE; //  --
typedef tDWORD                         tSYMBOL; //  --
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface constants,  )

// ----------  String format constants  ----------
#define cSTRING_Z                      ((tDWORD)(0x00010000)) // string is zero terminated
#define cSTRING_LINE                   ((tDWORD)(0x00020000)) // string is "\r\n" terminated
#define cSTRING_BLEN                   ((tDWORD)(0x00030000)) // string is byte prefixed
#define cSTRING_WLEN                   ((tDWORD)(0x00040000)) // string is word prefixed
#define cSTRING_DLEN                   ((tDWORD)(0x00050000)) // string is dword prefixed
#define cSTRING_CONTENT_ONLY           ((tDWORD)(0x00000000)) // string without any prefix and terminated elements
#define mSTRING_PR_FORMAT              ((tDWORD)(0x00030000)) // mask for prefix part of format field
#define mSTRING_TERM_FORMAT            ((tDWORD)(cSTRING_Z|cSTRING_LINE)) // mask for postfix part of format field
#define mSTRING_FORMAT                 ((tDWORD)(0x00070000)) // mask for string format

// ----------  String unit constants  ----------
#define fSTRING_BYTES                  ((tDWORD)(0x00000000)) //  --
#define fSTRING_SYMBOLS                ((tDWORD)(0x10000000)) //  --
#define fSTRING_F_EMPTY                ((tDWORD)(0x20000000)) //  --
#define mSTRING_UNITS                  ((tDWORD)(0x20000000)) //  --

// ----------  Compare constants  ----------
#define fSTRING_COMPARE_CASE_SENSITIVE ((tDWORD)(0x00000000)) //  --
#define fSTRING_COMPARE_LEN_SENSITIVE  ((tDWORD)(0x01000000)) //  --
#define fSTRING_COMPARE_UPPER          ((tDWORD)(0x02000000)) //  --
#define fSTRING_COMPARE_LOWER          ((tDWORD)(0x04000000)) //  --
#define fSTRING_COMPARE_SLASH_SENSITIVE ((tDWORD)(0x00000000)) //  --
#define fSTRING_COMPARE_SLASH_INSENSITIVE ((tDWORD)(0x08000000)) //  --
#define fSTRING_COMPARE_CASE_INSENSITIVE ((tDWORD)(fSTRING_COMPARE_UPPER | fSTRING_COMPARE_LOWER)) //  --
#define mSTRING_COMPARE                ((tDWORD)(0x0f000000)) //  --

// ----------  Convert constants  ----------
#define cSTRING_TO_UPPER               ((tSTR_CASE)(0x01000000)) //  --
#define cSTRING_TO_LOWER               ((tSTR_CASE)(0x02000000)) //  --

// ----------  Find constants  ----------
#define fSTRING_FIND_FORWARD           ((tDWORD)(0x00000000)) //  --
#define fSTRING_FIND_BACKWARD          ((tDWORD)(0x00100000)) //  --

// ----------  Catting constants  ----------
#define fSTRING_INNER                  ((tDWORD)(0x00000000)) //  --
#define fSTRING_LEFT                   ((tDWORD)(0x00010000)) //  --
#define fSTRING_RIGHT                  ((tDWORD)(0x00020000)) //  --
#define fSTRING_OUTER                  ((tDWORD)(fSTRING_LEFT|fSTRING_RIGHT)) //  --

// ----------  Range constants  ----------
#define cSTRING_WHOLE_LENGTH           ((tWORD)(-1)) //  --
#define cSTRING_EMPTY_LENGTH           ((tWORD)(-2)) //  --
#define cSTRING_WHOLE                  ((tSTR_RANGE)(STR_RANGE(0,cSTRING_WHOLE_LENGTH))) //  --
#define cSTRING_EMPTY                  ((tSTR_RANGE)(STR_RANGE(cSTRING_EMPTY_LENGTH,0))) //  --

// ----------  Compare return values  ----------
#define cSTRING_COMP_LT                ((tSTR_COMPARE)(-1)) //  --
#define cSTRING_COMP_EQ                ((tSTR_COMPARE)(0)) //  --
#define cSTRING_COMP_GT                ((tSTR_COMPARE)(+1)) //  --
#define cSTRING_COMP_UNDEF             ((tSTR_COMPARE)(2)) //  --
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface defined messages,  )
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface defined errors,  )
#define errSTR_OUT_OF_RANGE                      PR_MAKE_DECL_ERR(IID_STRING, 0x001) // 0x80064001,-2147074047 (1) -- range is out of string content
// AVP Prague stamp end



#if defined( __cplusplus ) && !defined(_USE_VTBL)
  struct cSeqIO;
  typedef cSeqIO* hSEQ_IO;
#else
  #if !defined( SEQIO_DEFINED )
    #define SEQIO_DEFINED
      
    typedef struct tag_hSEQ_IO *hSEQ_IO;
  #endif
#endif // if defined( __cplusplus )


// --------------------------------------------------------------------------------
// AVP Prague stamp begin( RPC start declaration,  )
#if defined( __RPC__ )
  typedef tUINT hSTRING;
#else
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface forward declaration,  )
struct iStringVtbl;
typedef struct iStringVtbl iStringVtbl;
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Object declaration,  )
#if defined( __cplusplus ) && !defined(_USE_VTBL)
  struct cString;
  typedef cString* hSTRING;
#else
  typedef struct tag_hSTRING {
    const iStringVtbl* vtbl; // pointer to the "String" virtual table
    const iSYSTEMVtbl* sys;  // pointer to the "SYSTEM" virtual table
  } *hSTRING;
#endif // if defined( __cplusplus )
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( RPC_ end declaration,  )
#endif // if defined( __RPC__ )
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Private definitions begin,  )
#if !defined( String_PRIVATE_DEFINITION )
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface function forward declarations,  )



  // ----------- Methods to get string value ----------
  typedef   tERROR (pr_call *fnpString_ExportToProp)     ( hSTRING _this, tDWORD* result, tSTR_RANGE range, hOBJECT to, tPROPID to_propid ); // -- saves string content to object's property;
  typedef   tERROR (pr_call *fnpString_ExportToStr)      ( hSTRING _this, tDWORD* result, tSTR_RANGE range, hSTRING to ); // -- saves string content to another string;
  typedef   tERROR (pr_call *fnpString_ExportToBuff)     ( hSTRING _this, tDWORD* result, tSTR_RANGE range, tPTR to, tDWORD length, tCODEPAGE cp, tDWORD flags ); // -- saves content of string to target buffer;
  typedef   tERROR (pr_call *fnpString_ExportToSeq)      ( hSTRING _this, tDWORD* result, tSTR_RANGE range, hSEQ_IO to ); // -- saves contents of string to target sequential IO object;

  // ----------- Methods to put string value ----------
  typedef   tERROR (pr_call *fnpString_ImportFromProp)   ( hSTRING _this, tDWORD* result, hOBJECT from, tPROPID propid ); // -- Loads string from object's property;
  typedef   tERROR (pr_call *fnpString_ImportFromStr)    ( hSTRING _this, tDWORD* result, hSTRING from, tSTR_RANGE range ); // -- loads contents from source string;
  typedef   tERROR (pr_call *fnpString_ImportFromBuff)   ( hSTRING _this, tDWORD* result, tPTR from, tDWORD length, tCODEPAGE cp, tDWORD flags ); // -- loads content of the source buffer to itself;
  typedef   tERROR (pr_call *fnpString_ImportFromSeq)    ( hSTRING _this, tDWORD* result, hSEQ_IO from );   // -- loads content from the source sequential IO object;

  // ----------- Concatenation methods ----------
  typedef   tERROR (pr_call *fnpString_AddFromProp)      ( hSTRING _this, tDWORD* result, hOBJECT from, tPROPID propid ); // -- adds content from object property;
  typedef   tERROR (pr_call *fnpString_AddFromStr)       ( hSTRING _this, tDWORD* result, hSTRING from, tSTR_RANGE range ); // -- concatenates two strings;
  typedef   tERROR (pr_call *fnpString_AddFromBuff)      ( hSTRING _this, tDWORD* result, tPTR from, tDWORD length, tCODEPAGE cp, tDWORD flags ); // -- adds content from buffer;
  typedef   tERROR (pr_call *fnpString_AddFromSeq)       ( hSTRING _this, tDWORD* result, hSEQ_IO from );   // -- ;

  // ----------- Miscelanous methods ----------
  typedef   tERROR (pr_call *fnpString_Fill)             ( hSTRING _this, tSTR_RANGE range, tSYMBOL symbol, tCODEPAGE cp ); // -- refill string with the indicated symbol;
  typedef   tERROR (pr_call *fnpString_Length)           ( hSTRING _this, tDWORD* result );                 // -- returns current length of the string in symbols;
  typedef   tERROR (pr_call *fnpString_LengthEx)         ( hSTRING _this, tDWORD* result, tSTR_RANGE range, tCODEPAGE cp, tDWORD flags ); // -- returns size of the (sub)string in bytes or in symbols;
  typedef   tERROR (pr_call *fnpString_GetCP)            ( hSTRING _this, tCODEPAGE* result );                 // -- returns current codepage;
  typedef   tERROR (pr_call *fnpString_SetCP)            ( hSTRING _this, tCODEPAGE cp );   // -- sets default code page for the string;

  // ----------- Compare methods ----------
  typedef   tERROR (pr_call *fnpString_Compare)          ( hSTRING _this, tSTR_RANGE range, hSTRING other, tSTR_RANGE other_range, tDWORD flags ); // -- compares the string with another one;
  typedef   tERROR (pr_call *fnpString_CompareBuff)      ( hSTRING _this, tSTR_RANGE range, tPTR buffer, tDWORD size, tCODEPAGE cp, tDWORD flags ); // -- compares string with buffer;
  typedef   tERROR (pr_call *fnpString_Compare2)         ( hSTRING _this, tSTR_COMPARE* result, tSTR_RANGE range, hSTRING other, tSTR_RANGE other_range, tDWORD flags ); // -- compares the string with another one;
  typedef   tERROR (pr_call *fnpString_CompareBuff2)     ( hSTRING _this, tSTR_COMPARE* result, tSTR_RANGE range, tPTR buffer, tDWORD size, tCODEPAGE cp, tDWORD flags ); // -- compares string with buffer;

  // ----------- Find methods ----------
  typedef   tERROR (pr_call *fnpString_Find)             ( hSTRING _this, tSTR_RANGE* result, tSTR_RANGE range, hSTRING other, tSTR_RANGE other_range, tDWORD flags ); // -- finds substring;
  typedef   tERROR (pr_call *fnpString_FindBuff)         ( hSTRING _this, tSTR_RANGE* result, tSTR_RANGE range, tPTR buffer, tDWORD size, tCODEPAGE cp, tDWORD flags ); // -- finds substring given as buffer;
  typedef   tERROR (pr_call *fnpString_FindOneOf)        ( hSTRING _this, tSTR_RANGE* result, tSTR_RANGE range, hSTRING symbol_set, tDWORD flags ); // -- finds first symbol in string belonged to "symbol_set";
  typedef   tERROR (pr_call *fnpString_FindOneOfBuff)    ( hSTRING _this, tSTR_RANGE* result, tSTR_RANGE range, tPTR symbol_set, tDWORD size, tCODEPAGE cp, tDWORD flags ); // -- finds one of symbols belonged to "symbol_set";

  // ----------- Range concerned methods ----------
  typedef   tERROR (pr_call *fnpString_Symbol)           ( hSTRING _this, tSYMBOL* result, tSTR_RANGE range, tDWORD index, tCODEPAGE cp, tDWORD flags ); // -- returns a symbol indicated by range and index;
  typedef   tERROR (pr_call *fnpString_Next)             ( hSTRING _this, tSYMBOL* result, tSTR_RANGE* range, tCODEPAGE cp ); // -- advance the range to next symbol and then return it;
  typedef   tERROR (pr_call *fnpString_NextEx)           ( hSTRING _this, tSYMBOL* result, tSTR_RANGE* range, tCODEPAGE cp ); // -- return the symbol and then advance the range;
  typedef   tERROR (pr_call *fnpString_Prev)             ( hSTRING _this, tSYMBOL* result, tSTR_RANGE* range, tCODEPAGE cp ); // -- moves range back and then returns symbol;
  typedef   tERROR (pr_call *fnpString_PrevEx)           ( hSTRING _this, tSYMBOL* result, tSTR_RANGE* range, tCODEPAGE cp ); // -- returns the symbol and then moves range back;
  typedef   tERROR (pr_call *fnpString_Left)             ( hSTRING _this, tSTR_RANGE* result, tSTR_RANGE range, tDWORD count ); // -- returns range for "count" left symbols of the string;
  typedef   tERROR (pr_call *fnpString_Mid)              ( hSTRING _this, tSTR_RANGE* result, tSTR_RANGE range, tDWORD begin, tDWORD count ); // -- returns range for "count" symbols begins with "begin";
  typedef   tERROR (pr_call *fnpString_Right)            ( hSTRING _this, tSTR_RANGE* result, tSTR_RANGE range, tDWORD count ); // -- returns range for "count" right symbols of the string;

  // ----------- Modification methods ----------
  typedef   tERROR (pr_call *fnpString_ChangeCase)       ( hSTRING _this, tSTR_CASE case_code ); // -- converts whole string to given case;
  typedef   tERROR (pr_call *fnpString_ChangeCaseEx)     ( hSTRING _this, tSTR_RANGE range, tSTR_CASE case_code ); // -- converts substring to given case;
  typedef   tERROR (pr_call *fnpString_Trim)             ( hSTRING _this, hSTRING symbol_set, tDWORD flags ); // -- removes from left/right/both edges of string all symbols belonged to "symbol_set" string;
  typedef   tERROR (pr_call *fnpString_TrimBuff)         ( hSTRING _this, tPTR symbol_set, tDWORD size, tCODEPAGE cp, tDWORD flags ); // -- removes from left/right/both edges of string all symbols belonged to "symbol_set";
  typedef   tERROR (pr_call *fnpString_Cut)              ( hSTRING _this, tSTR_RANGE range, tDWORD flags ); // -- removes/leaves substring pointed by range;
  typedef   tERROR (pr_call *fnpString_Replace)          ( hSTRING _this, tSTR_RANGE range, hSTRING repl, tSTR_RANGE repl_range, tSTR_RANGE* out_range ); // -- replaces substring with the new value;
  typedef   tERROR (pr_call *fnpString_ReplaceBuff)      ( hSTRING _this, tSTR_RANGE range, tPTR buffer, tDWORD size, tCODEPAGE cp, tSTR_RANGE* out_range ); // -- replaces substring with the new value;

  // ----------- Miscelanous methods ----------
  typedef   tERROR (pr_call *fnpString_Reserve)          ( hSTRING _this, tDWORD number, tCODEPAGE cp ); // -- reserve space for at least number of symbols;
  typedef   tERROR (pr_call *fnpString_ProveLastSlash)   ( hSTRING _this );                 // -- check if last symbol in string is slash and add it if not;

  // ----------- Find methods ----------
  typedef   tERROR (pr_call *fnpString_FindOneNotOfBuff) ( hSTRING _this, tSTR_RANGE* result, tSTR_RANGE range, tPTR symbol_set, tDWORD size, tCODEPAGE cp, tDWORD flags ); // -- finds one of symbols belonged to "symbol_set";

  // ----------- Format methods ----------
  typedef   tERROR (pr_call *fnpString_Format)           ( hSTRING _this, tCODEPAGE format_cp, const tVOID* format_str, ... ); // -- make formatted string;
  typedef   tERROR (pr_call *fnpString_VFormat)          ( hSTRING _this, tCODEPAGE format_cp, const tVOID* format_str, tVOID* args ); // -- ;


// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface declaration,  )
struct iStringVtbl {

// ----------- Methods to get string value ----------
  fnpString_ExportToProp      ExportToProp;
  fnpString_ExportToStr       ExportToStr;
  fnpString_ExportToBuff      ExportToBuff;
  fnpString_ExportToSeq       ExportToSeq;

// ----------- Methods to put string value ----------
  fnpString_ImportFromProp    ImportFromProp;
  fnpString_ImportFromStr     ImportFromStr;
  fnpString_ImportFromBuff    ImportFromBuff;
  fnpString_ImportFromSeq     ImportFromSeq;

// ----------- Concatenation methods ----------
  fnpString_AddFromProp       AddFromProp;
  fnpString_AddFromStr        AddFromStr;
  fnpString_AddFromBuff       AddFromBuff;
  fnpString_AddFromSeq        AddFromSeq;

// ----------- Miscelanous methods ----------
  fnpString_Fill              Fill;
  fnpString_Length            Length;
  fnpString_LengthEx          LengthEx;
  fnpString_GetCP             GetCP;
  fnpString_SetCP             SetCP;

// ----------- Compare methods ----------
  fnpString_Compare           Compare;
  fnpString_CompareBuff       CompareBuff;
  fnpString_Compare2          Compare2;
  fnpString_CompareBuff2      CompareBuff2;

// ----------- Find methods ----------
  fnpString_Find              Find;
  fnpString_FindBuff          FindBuff;
  fnpString_FindOneOf         FindOneOf;
  fnpString_FindOneOfBuff     FindOneOfBuff;

// ----------- Range concerned methods ----------
  fnpString_Symbol            Symbol;
  fnpString_Next              Next;
  fnpString_NextEx            NextEx;
  fnpString_Prev              Prev;
  fnpString_PrevEx            PrevEx;
  fnpString_Left              Left;
  fnpString_Mid               Mid;
  fnpString_Right             Right;

// ----------- Modification methods ----------
  fnpString_ChangeCase        ChangeCase;
  fnpString_ChangeCaseEx      ChangeCaseEx;
  fnpString_Trim              Trim;
  fnpString_TrimBuff          TrimBuff;
  fnpString_Cut               Cut;
  fnpString_Replace           Replace;
  fnpString_ReplaceBuff       ReplaceBuff;

// ----------- Miscelanous methods ----------
  fnpString_Reserve           Reserve;
  fnpString_ProveLastSlash    ProveLastSlash;

// ----------- Find methods ----------
  fnpString_FindOneNotOfBuff  FindOneNotOfBuff;

// ----------- Format methods ----------
  fnpString_Format            Format;
  fnpString_VFormat           VFormat;
}; // "String" external virtual table prototype
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Private definitions end,  )
#endif // if !defined( String_PRIVATE_DEFINITION )
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface declared properties,  )
//! #define pgINTERFACE_VERSION mPROPERTY_MAKE_GLOBAL( pTYPE_VERSION , 0x00000082 )
//! #define pgINTERFACE_COMMENT mPROPERTY_MAKE_GLOBAL( pTYPE_STRING  , 0x00000083 )
//! #define pgOBJECT_CODEPAGE   mPROPERTY_MAKE_GLOBAL( pTYPE_DWORD   , 0x0000004c )
//! #define pgOBJECT_LCID       mPROPERTY_MAKE_GLOBAL( pTYPE_DWORD   , 0x0000004d )
//! #define pgOBJECT_NAME       mPROPERTY_MAKE_GLOBAL( pTYPE_STRING  , 0x00000040 )
//! #define pgOBJECT_FULL_NAME  mPROPERTY_MAKE_GLOBAL( pTYPE_STRING  , 0x00000041 )
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Defines to simplify interface method calls,  )
#if !defined( __cplusplus ) || defined(_USE_VTBL)
  #define CALL_String_ExportToProp( _this, result, range, to, to_propid )                      ((_this)->vtbl->ExportToProp( (_this), result, range, to, to_propid ))
  #define CALL_String_ExportToStr( _this, result, range, to )                                  ((_this)->vtbl->ExportToStr( (_this), result, range, to ))
  #define CALL_String_ExportToBuff( _this, result, range, to, length, cp, flags )              ((_this)->vtbl->ExportToBuff( (_this), result, range, to, length, cp, flags ))
  #define CALL_String_ExportToSeq( _this, result, range, to )                                  ((_this)->vtbl->ExportToSeq( (_this), result, range, to ))
  #define CALL_String_ImportFromProp( _this, result, from, propid )                            ((_this)->vtbl->ImportFromProp( (_this), result, from, propid ))
  #define CALL_String_ImportFromStr( _this, result, from, range )                              ((_this)->vtbl->ImportFromStr( (_this), result, from, range ))
  #define CALL_String_ImportFromBuff( _this, result, from, length, cp, flags )                 ((_this)->vtbl->ImportFromBuff( (_this), result, from, length, cp, flags ))
  #define CALL_String_ImportFromSeq( _this, result, from )                                     ((_this)->vtbl->ImportFromSeq( (_this), result, from ))
  #define CALL_String_AddFromProp( _this, result, from, propid )                               ((_this)->vtbl->AddFromProp( (_this), result, from, propid ))
  #define CALL_String_AddFromStr( _this, result, from, range )                                 ((_this)->vtbl->AddFromStr( (_this), result, from, range ))
  #define CALL_String_AddFromBuff( _this, result, from, length, cp, flags )                    ((_this)->vtbl->AddFromBuff( (_this), result, from, length, cp, flags ))
  #define CALL_String_AddFromSeq( _this, result, from )                                        ((_this)->vtbl->AddFromSeq( (_this), result, from ))
  #define CALL_String_Fill( _this, range, symbol, cp )                                         ((_this)->vtbl->Fill( (_this), range, symbol, cp ))
  #define CALL_String_Length( _this, result )                                                  ((_this)->vtbl->Length( (_this), result ))
  #define CALL_String_LengthEx( _this, result, range, cp, flags )                              ((_this)->vtbl->LengthEx( (_this), result, range, cp, flags ))
  #define CALL_String_GetCP( _this, result )                                                   ((_this)->vtbl->GetCP( (_this), result ))
  #define CALL_String_SetCP( _this, cp )                                                       ((_this)->vtbl->SetCP( (_this), cp ))
  #define CALL_String_Compare( _this, range, other, other_range, flags )                       ((_this)->vtbl->Compare( (_this), range, other, other_range, flags ))
  #define CALL_String_CompareBuff( _this, range, buffer, size, cp, flags )                     ((_this)->vtbl->CompareBuff( (_this), range, buffer, size, cp, flags ))
  #define CALL_String_Compare2( _this, result, range, other, other_range, flags )              ((_this)->vtbl->Compare2( (_this), result, range, other, other_range, flags ))
  #define CALL_String_CompareBuff2( _this, result, range, buffer, size, cp, flags )            ((_this)->vtbl->CompareBuff2( (_this), result, range, buffer, size, cp, flags ))
  #define CALL_String_Find( _this, result, range, other, other_range, flags )                  ((_this)->vtbl->Find( (_this), result, range, other, other_range, flags ))
  #define CALL_String_FindBuff( _this, result, range, buffer, size, cp, flags )                ((_this)->vtbl->FindBuff( (_this), result, range, buffer, size, cp, flags ))
  #define CALL_String_FindOneOf( _this, result, range, symbol_set, flags )                     ((_this)->vtbl->FindOneOf( (_this), result, range, symbol_set, flags ))
  #define CALL_String_FindOneOfBuff( _this, result, range, symbol_set, size, cp, flags )       ((_this)->vtbl->FindOneOfBuff( (_this), result, range, symbol_set, size, cp, flags ))
  #define CALL_String_Symbol( _this, result, range, index, cp, flags )                         ((_this)->vtbl->Symbol( (_this), result, range, index, cp, flags ))
  #define CALL_String_Next( _this, result, range, cp )                                         ((_this)->vtbl->Next( (_this), result, range, cp ))
  #define CALL_String_NextEx( _this, result, range, cp )                                       ((_this)->vtbl->NextEx( (_this), result, range, cp ))
  #define CALL_String_Prev( _this, result, range, cp )                                         ((_this)->vtbl->Prev( (_this), result, range, cp ))
  #define CALL_String_PrevEx( _this, result, range, cp )                                       ((_this)->vtbl->PrevEx( (_this), result, range, cp ))
  #define CALL_String_Left( _this, result, range, count )                                      ((_this)->vtbl->Left( (_this), result, range, count ))
  #define CALL_String_Mid( _this, result, range, begin, count )                                ((_this)->vtbl->Mid( (_this), result, range, begin, count ))
  #define CALL_String_Right( _this, result, range, count )                                     ((_this)->vtbl->Right( (_this), result, range, count ))
  #define CALL_String_ChangeCase( _this, case_code )                                           ((_this)->vtbl->ChangeCase( (_this), case_code ))
  #define CALL_String_ChangeCaseEx( _this, range, case_code )                                  ((_this)->vtbl->ChangeCaseEx( (_this), range, case_code ))
  #define CALL_String_Trim( _this, symbol_set, flags )                                         ((_this)->vtbl->Trim( (_this), symbol_set, flags ))
  #define CALL_String_TrimBuff( _this, symbol_set, size, cp, flags )                           ((_this)->vtbl->TrimBuff( (_this), symbol_set, size, cp, flags ))
  #define CALL_String_Cut( _this, range, flags )                                               ((_this)->vtbl->Cut( (_this), range, flags ))
  #define CALL_String_Replace( _this, range, repl, repl_range, out_range )                     ((_this)->vtbl->Replace( (_this), range, repl, repl_range, out_range ))
  #define CALL_String_ReplaceBuff( _this, range, buffer, size, cp, out_range )                 ((_this)->vtbl->ReplaceBuff( (_this), range, buffer, size, cp, out_range ))
  #define CALL_String_Reserve( _this, number, cp )                                             ((_this)->vtbl->Reserve( (_this), number, cp ))
  #define CALL_String_ProveLastSlash( _this )                                                  ((_this)->vtbl->ProveLastSlash( (_this) ))
  #define CALL_String_FindOneNotOfBuff( _this, result, range, symbol_set, size, cp, flags )    ((_this)->vtbl->FindOneNotOfBuff( (_this), result, range, symbol_set, size, cp, flags ))
  #define CALL_String_Format( _this, format_cp, format_str )                              ((_this)->vtbl->Format( (_this), format_cp, format_str ))
  #define CALL_String_VFormat( _this, format_cp, format_str, args )                            ((_this)->vtbl->VFormat( (_this), format_cp, format_str, args ))
#else // if !defined( __cplusplus )
  #define CALL_String_ExportToProp( _this, result, range, to, to_propid )                      ((_this)->ExportToProp( result, range, to, to_propid ))
  #define CALL_String_ExportToStr( _this, result, range, to )                                  ((_this)->ExportToStr( result, range, to ))
  #define CALL_String_ExportToBuff( _this, result, range, to, length, cp, flags )              ((_this)->ExportToBuff( result, range, to, length, cp, flags ))
  #define CALL_String_ExportToSeq( _this, result, range, to )                                  ((_this)->ExportToSeq( result, range, to ))
  #define CALL_String_ImportFromProp( _this, result, from, propid )                            ((_this)->ImportFromProp( result, from, propid ))
  #define CALL_String_ImportFromStr( _this, result, from, range )                              ((_this)->ImportFromStr( result, from, range ))
  #define CALL_String_ImportFromBuff( _this, result, from, length, cp, flags )                 ((_this)->ImportFromBuff( result, from, length, cp, flags ))
  #define CALL_String_ImportFromSeq( _this, result, from )                                     ((_this)->ImportFromSeq( result, from ))
  #define CALL_String_AddFromProp( _this, result, from, propid )                               ((_this)->AddFromProp( result, from, propid ))
  #define CALL_String_AddFromStr( _this, result, from, range )                                 ((_this)->AddFromStr( result, from, range ))
  #define CALL_String_AddFromBuff( _this, result, from, length, cp, flags )                    ((_this)->AddFromBuff( result, from, length, cp, flags ))
  #define CALL_String_AddFromSeq( _this, result, from )                                        ((_this)->AddFromSeq( result, from ))
  #define CALL_String_Fill( _this, range, symbol, cp )                                         ((_this)->Fill( range, symbol, cp ))
  #define CALL_String_Length( _this, result )                                                  ((_this)->Length( result ))
  #define CALL_String_LengthEx( _this, result, range, cp, flags )                              ((_this)->LengthEx( result, range, cp, flags ))
  #define CALL_String_GetCP( _this, result )                                                   ((_this)->GetCP( result ))
  #define CALL_String_SetCP( _this, cp )                                                       ((_this)->SetCP( cp ))
  #define CALL_String_Compare( _this, range, other, other_range, flags )                       ((_this)->Compare( range, other, other_range, flags ))
  #define CALL_String_CompareBuff( _this, range, buffer, size, cp, flags )                     ((_this)->CompareBuff( range, buffer, size, cp, flags ))
  #define CALL_String_Compare2( _this, result, range, other, other_range, flags )              ((_this)->Compare2( result, range, other, other_range, flags ))
  #define CALL_String_CompareBuff2( _this, result, range, buffer, size, cp, flags )            ((_this)->CompareBuff2( result, range, buffer, size, cp, flags ))
  #define CALL_String_Find( _this, result, range, other, other_range, flags )                  ((_this)->Find( result, range, other, other_range, flags ))
  #define CALL_String_FindBuff( _this, result, range, buffer, size, cp, flags )                ((_this)->FindBuff( result, range, buffer, size, cp, flags ))
  #define CALL_String_FindOneOf( _this, result, range, symbol_set, flags )                     ((_this)->FindOneOf( result, range, symbol_set, flags ))
  #define CALL_String_FindOneOfBuff( _this, result, range, symbol_set, size, cp, flags )       ((_this)->FindOneOfBuff( result, range, symbol_set, size, cp, flags ))
  #define CALL_String_Symbol( _this, result, range, index, cp, flags )                         ((_this)->Symbol( result, range, index, cp, flags ))
  #define CALL_String_Next( _this, result, range, cp )                                         ((_this)->Next( result, range, cp ))
  #define CALL_String_NextEx( _this, result, range, cp )                                       ((_this)->NextEx( result, range, cp ))
  #define CALL_String_Prev( _this, result, range, cp )                                         ((_this)->Prev( result, range, cp ))
  #define CALL_String_PrevEx( _this, result, range, cp )                                       ((_this)->PrevEx( result, range, cp ))
  #define CALL_String_Left( _this, result, range, count )                                      ((_this)->Left( result, range, count ))
  #define CALL_String_Mid( _this, result, range, begin, count )                                ((_this)->Mid( result, range, begin, count ))
  #define CALL_String_Right( _this, result, range, count )                                     ((_this)->Right( result, range, count ))
  #define CALL_String_ChangeCase( _this, case_code )                                           ((_this)->ChangeCase( case_code ))
  #define CALL_String_ChangeCaseEx( _this, range, case_code )                                  ((_this)->ChangeCaseEx( range, case_code ))
  #define CALL_String_Trim( _this, symbol_set, flags )                                         ((_this)->Trim( symbol_set, flags ))
  #define CALL_String_TrimBuff( _this, symbol_set, size, cp, flags )                           ((_this)->TrimBuff( symbol_set, size, cp, flags ))
  #define CALL_String_Cut( _this, range, flags )                                               ((_this)->Cut( range, flags ))
  #define CALL_String_Replace( _this, range, repl, repl_range, out_range )                     ((_this)->Replace( range, repl, repl_range, out_range ))
  #define CALL_String_ReplaceBuff( _this, range, buffer, size, cp, out_range )                 ((_this)->ReplaceBuff( range, buffer, size, cp, out_range ))
  #define CALL_String_Reserve( _this, number, cp )                                             ((_this)->Reserve( number, cp ))
  #define CALL_String_ProveLastSlash( _this )                                                  ((_this)->ProveLastSlash( ))
  #define CALL_String_FindOneNotOfBuff( _this, result, range, symbol_set, size, cp, flags )    ((_this)->FindOneNotOfBuff( result, range, symbol_set, size, cp, flags ))
  #define CALL_String_Format( _this, format_cp, format_str )                              ((_this)->Format( format_cp, format_str ))
  #define CALL_String_VFormat( _this, format_cp, format_str, args )                            ((_this)->VFormat( format_cp, format_str, args ))
#endif // if !defined( __cplusplus )
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( C++ interface declaration,  )
#if defined (__cplusplus) && !defined(_USE_VTBL)
  struct pr_abstract iString {
    virtual tERROR pr_call ExportToProp( tDWORD* result, tSTR_RANGE range, hOBJECT to, tPROPID to_propid ) = 0; // -- saves string content to object's property
    virtual tERROR pr_call ExportToStr( tDWORD* result, tSTR_RANGE range, hSTRING to ) = 0; // -- saves string content to another string
    virtual tERROR pr_call ExportToBuff( tDWORD* result, tSTR_RANGE range, tPTR to, tDWORD length, tCODEPAGE cp, tDWORD flags ) = 0; // -- saves content of string to target buffer
    virtual tERROR pr_call ExportToSeq( tDWORD* result, tSTR_RANGE range, hSEQ_IO to ) = 0; // -- saves contents of string to target sequential IO object
    virtual tERROR pr_call ImportFromProp( tDWORD* result, hOBJECT from, tPROPID propid ) = 0; // -- Loads string from object's property
    virtual tERROR pr_call ImportFromStr( tDWORD* result, hSTRING from, tSTR_RANGE range ) = 0; // -- loads contents from source string
    virtual tERROR pr_call ImportFromBuff( tDWORD* result, tPTR from, tDWORD length, tCODEPAGE cp, tDWORD flags ) = 0; // -- loads content of the source buffer to itself
    virtual tERROR pr_call ImportFromSeq( tDWORD* result, hSEQ_IO from ) = 0; // -- loads content from the source sequential IO object
    virtual tERROR pr_call AddFromProp( tDWORD* result, hOBJECT from, tPROPID propid ) = 0; // -- adds content from object property
    virtual tERROR pr_call AddFromStr( tDWORD* result, hSTRING from, tSTR_RANGE range ) = 0; // -- concatenates two strings
    virtual tERROR pr_call AddFromBuff( tDWORD* result, tPTR from, tDWORD length, tCODEPAGE cp, tDWORD flags ) = 0; // -- adds content from buffer
    virtual tERROR pr_call AddFromSeq( tDWORD* result, hSEQ_IO from ) = 0;
    virtual tERROR pr_call Fill( tSTR_RANGE range, tSYMBOL symbol, tCODEPAGE cp ) = 0; // -- refill string with the indicated symbol
    virtual tERROR pr_call Length( tDWORD* result ) = 0; // -- returns current length of the string in symbols
    virtual tERROR pr_call LengthEx( tDWORD* result, tSTR_RANGE range, tCODEPAGE cp, tDWORD flags ) = 0; // -- returns size of the (sub)string in bytes or in symbols
    virtual tERROR pr_call GetCP( tCODEPAGE* result ) = 0; // -- returns current codepage
    virtual tERROR pr_call SetCP( tCODEPAGE cp ) = 0; // -- sets default code page for the string
    virtual tERROR pr_call Compare( tSTR_RANGE range, hSTRING other, tSTR_RANGE other_range, tDWORD flags ) = 0; // -- compares the string with another one
    virtual tERROR pr_call CompareBuff( tSTR_RANGE range, tPTR buffer, tDWORD size, tCODEPAGE cp, tDWORD flags ) = 0; // -- compares string with buffer
    virtual tERROR pr_call Compare2( tSTR_COMPARE* result, tSTR_RANGE range, hSTRING other, tSTR_RANGE other_range, tDWORD flags ) = 0; // -- compares the string with another one
    virtual tERROR pr_call CompareBuff2( tSTR_COMPARE* result, tSTR_RANGE range, tPTR buffer, tDWORD size, tCODEPAGE cp, tDWORD flags ) = 0; // -- compares string with buffer
    virtual tERROR pr_call Find( tSTR_RANGE* result, tSTR_RANGE range, hSTRING other, tSTR_RANGE other_range, tDWORD flags ) = 0; // -- finds substring
    virtual tERROR pr_call FindBuff( tSTR_RANGE* result, tSTR_RANGE range, tPTR buffer, tDWORD size, tCODEPAGE cp, tDWORD flags ) = 0; // -- finds substring given as buffer
    virtual tERROR pr_call FindOneOf( tSTR_RANGE* result, tSTR_RANGE range, hSTRING symbol_set, tDWORD flags ) = 0; // -- finds first symbol in string belonged to "symbol_set"
    virtual tERROR pr_call FindOneOfBuff( tSTR_RANGE* result, tSTR_RANGE range, tPTR symbol_set, tDWORD size, tCODEPAGE cp, tDWORD flags ) = 0; // -- finds one of symbols belonged to "symbol_set"
    virtual tERROR pr_call Symbol( tSYMBOL* result, tSTR_RANGE range, tDWORD index, tCODEPAGE cp, tDWORD flags ) = 0; // -- returns a symbol indicated by range and index
    virtual tERROR pr_call Next( tSYMBOL* result, tSTR_RANGE* range, tCODEPAGE cp ) = 0; // -- advance the range to next symbol and then return it
    virtual tERROR pr_call NextEx( tSYMBOL* result, tSTR_RANGE* range, tCODEPAGE cp ) = 0; // -- return the symbol and then advance the range
    virtual tERROR pr_call Prev( tSYMBOL* result, tSTR_RANGE* range, tCODEPAGE cp ) = 0; // -- moves range back and then returns symbol
    virtual tERROR pr_call PrevEx( tSYMBOL* result, tSTR_RANGE* range, tCODEPAGE cp ) = 0; // -- returns the symbol and then moves range back
    virtual tERROR pr_call Left( tSTR_RANGE* result, tSTR_RANGE range, tDWORD count ) = 0; // -- returns range for "count" left symbols of the string
    virtual tERROR pr_call Mid( tSTR_RANGE* result, tSTR_RANGE range, tDWORD begin, tDWORD count ) = 0; // -- returns range for "count" symbols begins with "begin"
    virtual tERROR pr_call Right( tSTR_RANGE* result, tSTR_RANGE range, tDWORD count ) = 0; // -- returns range for "count" right symbols of the string
    virtual tERROR pr_call ChangeCase( tSTR_CASE case_code ) = 0; // -- converts whole string to given case
    virtual tERROR pr_call ChangeCaseEx( tSTR_RANGE range, tSTR_CASE case_code ) = 0; // -- converts substring to given case
    virtual tERROR pr_call Trim( hSTRING symbol_set, tDWORD flags ) = 0; // -- removes from left/right/both edges of string all symbols belonged to "symbol_set" string
    virtual tERROR pr_call TrimBuff( tPTR symbol_set, tDWORD size, tCODEPAGE cp, tDWORD flags ) = 0; // -- removes from left/right/both edges of string all symbols belonged to "symbol_set"
    virtual tERROR pr_call Cut( tSTR_RANGE range, tDWORD flags ) = 0; // -- removes/leaves substring pointed by range
    virtual tERROR pr_call Replace( tSTR_RANGE range, hSTRING repl, tSTR_RANGE repl_range, tSTR_RANGE* out_range ) = 0; // -- replaces substring with the new value
    virtual tERROR pr_call ReplaceBuff( tSTR_RANGE range, tPTR buffer, tDWORD size, tCODEPAGE cp, tSTR_RANGE* out_range ) = 0; // -- replaces substring with the new value
    virtual tERROR pr_call Reserve( tDWORD number, tCODEPAGE cp ) = 0; // -- reserve space for at least number of symbols
    virtual tERROR pr_call ProveLastSlash() = 0; // -- check if last symbol in string is slash and add it if not
    virtual tERROR pr_call FindOneNotOfBuff( tSTR_RANGE* result, tSTR_RANGE range, tPTR symbol_set, tDWORD size, tCODEPAGE cp, tDWORD flags ) = 0; // -- finds one of symbols belonged to "symbol_set"
    virtual tERROR pr_call Format( tCODEPAGE format_cp, const tVOID* format_str, ... ) = 0; // -- make formatted string
    virtual tERROR pr_call VFormat( tCODEPAGE format_cp, const tVOID* format_str, tVOID* args ) = 0;
  };

  struct pr_abstract cString : public iString, public iObj {

		OBJ_IMPL( cString );

    operator hOBJECT() { return (hOBJECT)this; }
    operator hSTRING()   { return (hSTRING)this; }

    tDWORD pr_call get_pgOBJECT_CODEPAGE() { return (tDWORD)getDWord(pgOBJECT_CODEPAGE); }
    tERROR pr_call set_pgOBJECT_CODEPAGE( tDWORD value ) { return setDWord(pgOBJECT_CODEPAGE,(tDWORD)value); }

    tDWORD pr_call get_pgOBJECT_LCID() { return (tDWORD)getDWord(pgOBJECT_LCID); }
    tERROR pr_call set_pgOBJECT_LCID( tDWORD value ) { return setDWord(pgOBJECT_LCID,(tDWORD)value); }

    tERROR pr_call get_pgOBJECT_NAME( tPTR buff, tDWORD size, tCODEPAGE cp=cCP_ANSI ) { return getStr(pgOBJECT_NAME,buff,size,cp); }
    tERROR pr_call set_pgOBJECT_NAME( tPTR buff, tDWORD size, tCODEPAGE cp=cCP_ANSI ) { return setStr(pgOBJECT_NAME,buff,size,cp); }

    tERROR pr_call get_pgOBJECT_FULL_NAME( tPTR buff, tDWORD size, tCODEPAGE cp=cCP_ANSI ) { return getStr(pgOBJECT_FULL_NAME,buff,size,cp); }
    tERROR pr_call set_pgOBJECT_FULL_NAME( tPTR buff, tDWORD size, tCODEPAGE cp=cCP_ANSI ) { return setStr(pgOBJECT_FULL_NAME,buff,size,cp); }

  };

#endif // if defined (__cplusplus) && !defined(_USE_VTBL)
// AVP Prague stamp end



// range defines
#define STR_RANGE( offset, len )            ((tSTR_RANGE)((((tWORD)(offset))<<16)|((tWORD)(len))))
#define STR_RANGE_LEFT( len )               STR_RANGE(  0, len )
#define STR_RANGE_RIGHT( len )              STR_RANGE( cSTRING_WHOLE_LENGTH, len )
  
#define STR_RANGE_POS(range)                ((tUINT)((range)>>16))
#define STR_RANGE_LEN(range)                ((tUINT)((range)&0x0000ffff))
#define STR_RANGE_END(range)                (STR_RANGE_POS(range)+STR_RANGE_LEN(range))

#define STR_RANGE_SHIFT(range,step)         STR_RANGE( (((STR_RANGE_POS(range)+(step))>=0) ? (STR_RANGE_POS(range)+(step)) : STR_RANGE_POS(range)), STR_RANGE_LEN(range) )
#define STR_RANGE_MOVE_POS(range,step)      STR_RANGE( (((STR_RANGE_POS(range)+(step))>=0) ? (STR_RANGE_POS(range)+(step)) : STR_RANGE_POS(range)), (((STR_RANGE_LEN(range)-(step)) >= 1) ? ((STR_RANGE_LEN(range)-(step)) : 1)) )
#define STR_RANGE_ENLARGE(range,add)        STR_RANGE( STR_RANGE_POS(range), (((STR_RANGE_LEN(range)-(add)) >= 1) ? ((STR_RANGE_LEN(range)-(add)) : 1)) )


#if defined (__cplusplus)
  struct tStrRange;

  struct tStrPos {
    tINT m_pos;
    tStrPos() : m_pos(0) {}
    tStrPos( tSTR_RANGE pos ) : m_pos(STR_RANGE_POS(pos)) {}
    tStrPos( const tStrPos& o ) : m_pos(o.m_pos) {}
    tStrPos( const tStrRange& o );

    operator tSTR_RANGE () const { return STR_RANGE(m_pos,1); }
    operator tStrRange () const;

    tINT operator  + ( tINT steps ) const { return m_pos+steps; }
    tINT operator  - ( tINT steps ) const { return m_pos-steps; }
    tINT operator += ( tINT steps )       { return m_pos+=steps; }
    tINT operator -= ( tINT steps )       { return m_pos-=steps; }
    tINT operator ++ ()                   { return m_pos++; }
    tINT operator ++ (int)                { return ++m_pos; }
    tINT operator -- ()                   { return m_pos--; }
    tINT operator -- (int)                { return --m_pos; }
  };

  struct tStrRange {
    tSTR_RANGE m_range;

    tStrRange() : m_range(cSTRING_WHOLE) {}
    tStrRange( tSTR_RANGE range )   : m_range(range) {}
    tStrRange( tINT pos, tINT len ) : m_range(STR_RANGE(pos,len)) {}
    tStrRange( const tStrPos& o )   : m_range(STR_RANGE(o.m_pos,1)) {}
    tStrRange( const tStrRange& o ) : m_range(o.m_range) {}

    tSTR_RANGE operator=( tSTR_RANGE o ) { return m_range=o; }
    tStrRange  set( tSTR_RANGE o )       { tStrRange ret(*this); m_range=STR_RANGE_POS(o); return ret; }
    tStrRange  set( tINT pos, tINT len ) { tStrRange ret(*this); m_range=STR_RANGE(pos,len); return ret; }

    tINT pos() const { return STR_RANGE_POS(m_range); }
    tINT len() const { return STR_RANGE_LEN(m_range); }

    operator tSTR_RANGE () const { return m_range; }
    operator tSTR_RANGE*()       { return &m_range; }
    operator tStrPos()     const { return tStrPos(STR_RANGE_POS(m_range)); }

    tStrRange  operator  + ( tINT steps ) const { return tStrRange(STR_RANGE_POS(m_range)+steps,STR_RANGE_LEN(m_range)); }
    tStrRange  operator  - ( tINT steps ) const { return tStrRange(STR_RANGE_POS(m_range)-steps,STR_RANGE_LEN(m_range)); }
    tStrRange& operator += ( tINT steps )       { m_range=STR_RANGE(STR_RANGE_POS(m_range)+steps,STR_RANGE_LEN(m_range)); return *this; }
    tStrRange& operator -= ( tINT steps )       { m_range=STR_RANGE(STR_RANGE_POS(m_range)+steps,STR_RANGE_LEN(m_range)); return *this; }
    tStrRange  operator ++ ()                   { tStrRange ret = *this; m_range=STR_RANGE(STR_RANGE_POS(m_range)+1,STR_RANGE_LEN(m_range)); return ret; }
    tStrRange& operator ++ (int)                { m_range=STR_RANGE(STR_RANGE_POS(m_range)+1,STR_RANGE_LEN(m_range)); return *this; }
    tStrRange  operator -- ()                   { tStrRange ret = *this; m_range=STR_RANGE(STR_RANGE_POS(m_range)-1,STR_RANGE_LEN(m_range)); return ret; }
    tStrRange& operator -- (int)                { m_range=STR_RANGE(STR_RANGE_POS(m_range)-1,STR_RANGE_LEN(m_range)); return *this; }

    tStrRange& shift( tINT steps )              { return operator += (steps); }
    tStrRange& shift_pos( tINT add );
    tStrRange& enlarge( tINT add )              { tINT len = STR_RANGE_LEN(m_range); if ( len != cSTRING_WHOLE_LENGTH ) m_range = STR_RANGE( STR_RANGE_POS(m_range), len+add ); return *this; }
    tStrRange& step()                           { tINT len = STR_RANGE_LEN(m_range); if ( len != cSTRING_WHOLE_LENGTH ) m_range = STR_RANGE( STR_RANGE_POS(m_range)+len, len ); return *this; }
    tStrRange& enlarge_to_the_end()             { m_range |= cSTRING_WHOLE_LENGTH; return *this; }
    tStrRange& step_and_enlarge_to_the_end()    { tUINT pos = STR_RANGE_POS(m_range); tINT len = STR_RANGE_LEN(m_range); if ( len != cSTRING_WHOLE_LENGTH ) m_range = STR_RANGE(pos+len,cSTRING_WHOLE_LENGTH); return *this; }
  };

  inline tStrPos::tStrPos( const tStrRange& o ) : m_pos(STR_RANGE_POS(o.m_range)) {}
  inline tStrPos::operator tStrRange () const { return tStrRange(m_pos,1); }
  inline tStrRange& tStrRange::shift_pos( tINT add ) { 
    tINT pos = STR_RANGE_POS(m_range); 
    tINT len = STR_RANGE_LEN(m_range);
    if ( len == cSTRING_WHOLE_LENGTH )
      pos += add;
    else if ( add < len ) {
      pos += add;
      len -= add;
    }
    else {
      pos += len-1;
      len = 1;
    }
    return *this; 
  }

#endif // #if defined (__cplusplus)

// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Header endif,  )
#endif // __i_string__37b1402e_ae7b_11d4_b757_00d0b7170e50
// AVP Prague stamp end



#if defined(__PROXY_DECLARATION) && !defined(String_PROXY_DEFINITION)
#define String_PROXY_DEFINITION

PR_PROXY_BEGIN(String)
	PR_PROXY(String, ExportToProp,   PR_ARG4(tid_DWORD|tid_POINTER,tid_DWORD,tid_OBJECT,tid_PROPID))
	PR_PROXY(String, ExportToStr,    PR_ARG3(tid_DWORD|tid_POINTER,tid_DWORD,tid_OBJECT))
	PR_PROXY(String, ExportToBuff,   PR_ARG6(tid_DWORD|tid_POINTER,tid_DWORD,tid_BINARY|prf_OUT,tid_DWORD,tid_CODEPAGE,tid_DWORD))
	PR_PROXY(String, ImportFromProp, PR_ARG3(tid_DWORD|tid_POINTER,tid_OBJECT,tid_PROPID))
	PR_PROXY(String, ImportFromStr,  PR_ARG3(tid_DWORD|tid_POINTER,tid_OBJECT,tid_DWORD))
	PR_PROXY(String, ImportFromBuff, PR_ARG4(tid_DWORD|tid_POINTER,tid_BINARY|prf_IN,tid_CODEPAGE,tid_DWORD))
	PR_PROXY(String, Length,         PR_ARG1(tid_DWORD|tid_POINTER))
	PR_PROXY(String, LengthEx,       PR_ARG4(tid_DWORD|tid_POINTER,tid_DWORD,tid_CODEPAGE,tid_DWORD))
	PR_PROXY(String, GetCP,          PR_ARG1(tid_CODEPAGE|tid_POINTER))
	PR_PROXY(String, SetCP,          PR_ARG1(tid_CODEPAGE))
PR_PROXY_END(String, IID_STRING)

#endif // __PROXY_DECLARATION
