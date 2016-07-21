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



#define MEM_CHUNK 0x10
// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Private definitions,  )
#define String_PRIVATE_DEFINITION
// AVP Prague stamp end



//#define RANGE_PROC

// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Includes for interface,  )
//! #include <prague.h>
#include "pstring.h"
// AVP Prague stamp end



#if defined (__unix__)
#include <wctype.h>
#endif


#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#define IMPEX_EXPORT_LIB
#include <Prague/plugin/p_string.h>

#undef  IMPEX_EXPORT_LIB
#define IMPEX_IMPORT_LIB
#include <Prague/iface/e_ktrace.h>


//#define CHECK_STRING_COUNT
#if defined (_DEBUG) && defined(CHECK_STRING_COUNT)
  #include <Prague/iface/e_loader.h>

  #define IMPEX_TABLE_CONTENT
  IMPORT_TABLE(import_table)
    #include <Prague/iface/e_loader.h>
  IMPORT_TABLE_END
#endif


#include "codec.h"

hROOT g_root;

tERROR pr_call i_CalcImportLen( tPTR* p_from, tCODEPAGE p_cp, tDWORD p_flags, tDWORD p_len, tDWORD* p_olen );
// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface comment,  )
// String interface implementation
// Short comments -- String operations
//    Interface implements string operations
// AVP Prague stamp end



#define RES(r)          ((r)?(*(r)):0)

#define BOR(r)          ((tUINT)((r)>>16))
#define LOR(r)          ((tUINT)((r)&0x0000ffff))
#define MR(b,l)         (((b)<<16)|(l))

#define RANGE(b,l,r,c)                 \
  if ((r==cSTRING_WHOLE) /*|| (r==0)*/)\
    b=0, l=d->len;                     \
  else {                               \
    b = BOR(r);                        \
    l = LOR(r);                        \
    if ( b == cSTRING_EMPTY_LENGTH )   \
      {c;}                             \
    else if ( b == 0xffff ) {          \
      if ( d->cp == cCP_UNICODE )      \
        l *= sizeof(tWCHAR);           \
      if ( l > d->len ) {              \
        b = 0;                         \
        l = d->len;                    \
      }                                \
      else                             \
        b = d->len - l;                \
    }                                  \
    else if ( d->cp == cCP_UNICODE ) { \
      b *= sizeof(tWCHAR);             \
      l *= sizeof(tWCHAR);             \
      if ( b > d->len )                \
        {c;}                           \
      else if ((b+l)>d->len)           \
        l = d->len - b;                \
    }                                  \
    else if ( b >= d->len )            \
      {c;}                             \
    else if ((b+l)>d->len)             \
      l = d->len - b;                  \
  } 

#define RANGE_WOCHECK(b,l,r)           \
  if ((r==cSTRING_WHOLE) || (r==0))    \
    b=0, l=d->len;                     \
  else {                               \
    b = BOR(r);                        \
    l = LOR(r);                        \
    if ( b == 0xffff ) {               \
      if ( d->cp == cCP_UNICODE )      \
        l *= sizeof(tWCHAR);           \
      if ( l > d->len ) {              \
        b = 0;                         \
        l = d->len;                    \
      }                                \
      else                             \
        b = d->len - l;                \
    }                                  \
    else if ( d->cp == cCP_UNICODE ) { \
      b *= sizeof(tWCHAR);             \
      l *= sizeof(tWCHAR);             \
    }                                  \
  } 


#if defined( RANGE_PROC )

#undef  RANGE
#define RANGE(b,l,r,c)              if( !range_proc(d,&b,&l,r,cTRUE) ) {c;}

#undef  RANGE_WOCHECK
#define RANGE_WOCHECK(b,l,r)        range_proc(d,&b,&l,r,cFALSE)

tBOOL range_proc( StringData* d, tUINT* b, tUINT* l, tSTR_RANGE r, tBOOL check ) {
  if ((r==cSTRING_WHOLE) || (r==0))    
    *b=0, *l=d->len;                     
  else {                               
    *b = BOR(r);                        
    *l = LOR(r);                        
    if ( *b == 0xffff ) {               
      if ( d->cp == cCP_UNICODE )      
        *l *= sizeof(tWCHAR);           
      if ( *l > d->len ) {              
        *b = 0;                         
        *l = d->len;                    
      }                                
      else                             
        *b = d->len - *l;                
    }                                  
    else if ( d->cp == cCP_UNICODE ) { 
      *b *= sizeof(tWCHAR);             
      *l *= sizeof(tWCHAR);             
      if ( *b > d->len )                
        return check;
      else if ( check && ((*b+*l) > d->len) )
        *l = d->len - *b;
    }                                  
    else if ( *b > d->len )             
      return check;
    else if ( check && ((*b+*l) > d->len) )
      *l = d->len - *b;                  
  } 
  return cTRUE;
}
#endif




// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Data structure,  )
  // data structure "StringData" is described in "o:\prague\string\pstring.c" header file
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Object declaration,  )

typedef struct tag_hi_String {
	const iStringVtbl* vtbl; // pointer to the "String" virtual table
	const iSYSTEMVTBL* sys;  // pointer to the "SYSTEM" virtual table
	StringData*        data; // pointer to the "String" data structure
} *hi_String;
// AVP Prague stamp end



//#if defined(_DEBUG)
//	#define STR_CHECK( content )                                          \
//		if ( _this->data->buff && !wcscmp(UNI(_this->data->buff),content) ) \
//			check_func( _this )
//
//	void check_func( hi_String str ) {
//	}
//
//#else
	#define STR_CHECK( cnt )
//#endif




// ---
static tERROR pr_call CatchMem( hi_String _this, tUINT len ) {
  tERROR      e;
  StringData* d = _this->data;
  tUINT       l = len + sizeof(hHEAP); // kernel will enlarge memory size by sizeof(hHEAP)

  // make it divisible by MEM_CHUNK
  l /= MEM_CHUNK;
  l *= MEM_CHUNK;
  if ( l < (len + sizeof(hHEAP)) )
    l += MEM_CHUNK;
  l -= sizeof(hHEAP);
  if ( PR_SUCC(e=CALL_SYS_ObjHeapRealloc(_this,(tPTR*)&d->buff,d->buff,l)) ) {
    d->blen = l;
    return errOK;
  }
  return e;
}

// ---
/*
tERROR pr_call CheckCP( hOBJECT obj, tPROPID propid, tCODEPAGE* cp ) {
  tERROR error;
  if ( !propid || (errPROPERTY_NOT_FOUND == (error = CALL_SYS_PropertyGet(obj,0,((propid&~pTYPE_MASK)|pTYPE_CODEPAGE),cp,sizeof(tCODEPAGE)))) ) {
    error = CALL_SYS_PropertyGet( obj, 0, pgOBJECT_CODEPAGE, cp, sizeof(tCODEPAGE) );
    if ( error == errPROPERTY_NOT_FOUND ) {
      *cp = cCP_ANSI;
      error = errOK;
    }
  }
  return error;
}
*/




// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Internal method table prototypes,  )
tERROR pr_call String_ObjectInit( hi_String _this );
tERROR pr_call String_ObjectClose( hi_String _this );
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Internal method table,  )
static iINTERNAL i_String_vtbl = {
	(tIntFnRecognize)         NULL,
	(tIntFnObjectNew)         NULL,
	(tIntFnObjectInit)        String_ObjectInit,
	(tIntFnObjectInitDone)    NULL,
	(tIntFnObjectCheck)       NULL,
	(tIntFnObjectPreClose)    NULL,
	(tIntFnObjectClose)       String_ObjectClose,
	(tIntFnChildNew)          NULL,
	(tIntFnChildInitDone)     NULL,
	(tIntFnChildClose)        NULL,
	(tIntFnMsgReceive)        NULL,
	(tIntFnIFaceTransfer)     NULL
};
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface function forward declarations,  )

// ----------- Methods to get string value ----------
typedef   tERROR (pr_call *fnpString_ExportToProp)     ( hi_String _this, tDWORD* result, tSTR_RANGE range, hOBJECT to, tPROPID to_propid ); // -- saves string content to object's property;
typedef   tERROR (pr_call *fnpString_ExportToStr)      ( hi_String _this, tDWORD* result, tSTR_RANGE range, hSTRING to ); // -- saves string content to another string;
typedef   tERROR (pr_call *fnpString_ExportToBuff)     ( hi_String _this, tDWORD* result, tSTR_RANGE range, tPTR to, tDWORD length, tCODEPAGE cp, tDWORD flags ); // -- saves content of string to target buffer;
typedef   tERROR (pr_call *fnpString_ExportToSeq)      ( hi_String _this, tDWORD* result, tSTR_RANGE range, hSEQ_IO to ); // -- saves contents of string to target sequential IO object;

// ----------- Methods to put string value ----------
typedef   tERROR (pr_call *fnpString_ImportFromProp)   ( hi_String _this, tDWORD* result, hOBJECT from, tPROPID propid ); // -- Loads string from object's property;
typedef   tERROR (pr_call *fnpString_ImportFromStr)    ( hi_String _this, tDWORD* result, hSTRING from, tSTR_RANGE range ); // -- loads contents from source string;
typedef   tERROR (pr_call *fnpString_ImportFromBuff)   ( hi_String _this, tDWORD* result, tPTR from, tDWORD length, tCODEPAGE cp, tDWORD flags ); // -- loads content of the source buffer to itself;
typedef   tERROR (pr_call *fnpString_ImportFromSeq)    ( hi_String _this, tDWORD* result, hSEQ_IO from );   // -- loads content from the source sequential IO object;

// ----------- Concatenation methods ----------
typedef   tERROR (pr_call *fnpString_AddFromProp)      ( hi_String _this, tDWORD* result, hOBJECT from, tPROPID propid ); // -- adds content from object property;
typedef   tERROR (pr_call *fnpString_AddFromStr)       ( hi_String _this, tDWORD* result, hSTRING from, tSTR_RANGE range ); // -- concatenates two strings;
typedef   tERROR (pr_call *fnpString_AddFromBuff)      ( hi_String _this, tDWORD* result, tPTR from, tDWORD length, tCODEPAGE cp, tDWORD flags ); // -- adds content from buffer;
typedef   tERROR (pr_call *fnpString_AddFromSeq)       ( hi_String _this, tDWORD* result, hSEQ_IO from );   // -- ;

// ----------- Miscelanous methods ----------
typedef   tERROR (pr_call *fnpString_Fill)             ( hi_String _this, tSTR_RANGE range, tSYMBOL symbol, tCODEPAGE cp ); // -- refill string with the indicated symbol;
typedef   tERROR (pr_call *fnpString_Length)           ( hi_String _this, tDWORD* result );                 // -- returns current length of the string in symbols;
typedef   tERROR (pr_call *fnpString_LengthEx)         ( hi_String _this, tDWORD* result, tSTR_RANGE range, tCODEPAGE cp, tDWORD flags ); // -- returns size of the (sub)string in bytes or in symbols;
typedef   tERROR (pr_call *fnpString_GetCP)            ( hi_String _this, tCODEPAGE* result );                 // -- returns current codepage;
typedef   tERROR (pr_call *fnpString_SetCP)            ( hi_String _this, tCODEPAGE cp );   // -- sets default code page for the string;

// ----------- Compare methods ----------
typedef   tERROR (pr_call *fnpString_Compare)          ( hi_String _this, tSTR_RANGE range, hSTRING other, tSTR_RANGE other_range, tDWORD flags ); // -- compares the string with another one;
typedef   tERROR (pr_call *fnpString_CompareBuff)      ( hi_String _this, tSTR_RANGE range, tPTR buffer, tDWORD size, tCODEPAGE cp, tDWORD flags ); // -- compares string with buffer;
typedef   tERROR (pr_call *fnpString_Compare2)         ( hi_String _this, tSTR_COMPARE* result, tSTR_RANGE range, hSTRING other, tSTR_RANGE other_range, tDWORD flags ); // -- compares the string with another one;
typedef   tERROR (pr_call *fnpString_CompareBuff2)     ( hi_String _this, tSTR_COMPARE* result, tSTR_RANGE range, tPTR buffer, tDWORD size, tCODEPAGE cp, tDWORD flags ); // -- compares string with buffer;

// ----------- Find methods ----------
typedef   tERROR (pr_call *fnpString_Find)             ( hi_String _this, tSTR_RANGE* result, tSTR_RANGE range, hSTRING other, tSTR_RANGE other_range, tDWORD flags ); // -- finds substring;
typedef   tERROR (pr_call *fnpString_FindBuff)         ( hi_String _this, tSTR_RANGE* result, tSTR_RANGE range, tPTR buffer, tDWORD size, tCODEPAGE cp, tDWORD flags ); // -- finds substring given as buffer;
typedef   tERROR (pr_call *fnpString_FindOneOf)        ( hi_String _this, tSTR_RANGE* result, tSTR_RANGE range, hSTRING symbol_set, tDWORD flags ); // -- finds first symbol in string belonged to "symbol_set";
typedef   tERROR (pr_call *fnpString_FindOneOfBuff)    ( hi_String _this, tSTR_RANGE* result, tSTR_RANGE range, tPTR symbol_set, tDWORD size, tCODEPAGE cp, tDWORD flags ); // -- finds one of symbols belonged to "symbol_set";

// ----------- Range concerned methods ----------
typedef   tERROR (pr_call *fnpString_Symbol)           ( hi_String _this, tSYMBOL* result, tSTR_RANGE range, tDWORD index, tCODEPAGE cp, tDWORD p_flags ); // -- returns a symbol indicated by range and index;
typedef   tERROR (pr_call *fnpString_Next)             ( hi_String _this, tSYMBOL* result, tSTR_RANGE* range, tCODEPAGE cp ); // -- advance the range to next symbol and then return it;
typedef   tERROR (pr_call *fnpString_NextEx)           ( hi_String _this, tSYMBOL* result, tSTR_RANGE* range, tCODEPAGE cp ); // -- return the symbol and then advance the range;
typedef   tERROR (pr_call *fnpString_Prev)             ( hi_String _this, tSYMBOL* result, tSTR_RANGE* range, tCODEPAGE cp ); // -- moves range back and then returns symbol;
typedef   tERROR (pr_call *fnpString_PrevEx)           ( hi_String _this, tSYMBOL* result, tSTR_RANGE* range, tCODEPAGE cp ); // -- returns the symbol and then moves range back;
typedef   tERROR (pr_call *fnpString_Left)             ( hi_String _this, tSTR_RANGE* result, tSTR_RANGE range, tDWORD count ); // -- returns range for "count" left symbols of the string;
typedef   tERROR (pr_call *fnpString_Mid)              ( hi_String _this, tSTR_RANGE* result, tSTR_RANGE range, tDWORD begin, tDWORD count ); // -- returns range for "count" symbols begins with "begin";
typedef   tERROR (pr_call *fnpString_Right)            ( hi_String _this, tSTR_RANGE* result, tSTR_RANGE range, tDWORD count ); // -- returns range for "count" right symbols of the string;

// ----------- Modification methods ----------
typedef   tERROR (pr_call *fnpString_ChangeCase)       ( hi_String _this, tSTR_CASE case_code ); // -- converts whole string to given case;
typedef   tERROR (pr_call *fnpString_ChangeCaseEx)     ( hi_String _this, tSTR_RANGE range, tSTR_CASE case_code ); // -- converts substring to given case;
typedef   tERROR (pr_call *fnpString_Trim)             ( hi_String _this, hSTRING symbol_set, tDWORD flags ); // -- removes from left/right/both edges of string all symbols belonged to "symbol_set" string;
typedef   tERROR (pr_call *fnpString_TrimBuff)         ( hi_String _this, tPTR symbol_set, tDWORD size, tCODEPAGE cp, tDWORD flags ); // -- removes from left/right/both edges of string all symbols belonged to "symbol_set";
typedef   tERROR (pr_call *fnpString_Cut)              ( hi_String _this, tSTR_RANGE range, tDWORD flags ); // -- removes/leaves substring pointed by range;
typedef   tERROR (pr_call *fnpString_Replace)          ( hi_String _this, tSTR_RANGE range, hSTRING p_repl, tSTR_RANGE p_repl_range ); // -- replaces substring with the new value;
typedef   tERROR (pr_call *fnpString_ReplaceBuff)      ( hi_String _this, tSTR_RANGE range, tPTR buffer, tDWORD size, tCODEPAGE cp ); // -- replaces substring with the new value;

// ----------- Miscelanous methods ----------
typedef   tERROR (pr_call *fnpString_Reserve)          ( hi_String _this, tDWORD number, tCODEPAGE cp ); // -- reserve space for at least number of symbols;
typedef   tERROR (pr_call *fnpString_ProveLastSlash)   ( hi_String _this );                 // -- check if last symbol in string is slash and add it if not;

// ----------- Find methods ----------
typedef   tERROR (pr_call *fnpString_FindOneNotOfBuff) ( hi_String _this, tSTR_RANGE* result, tSTR_RANGE range, tPTR symbol_set, tDWORD size, tCODEPAGE cp, tDWORD flags ); // -- finds one of symbols belonged to "symbol_set";

// ----------- Format methods ----------
typedef   tERROR (pr_call *fnpString_Format)           ( hi_String _this, tCODEPAGE format_cp, const tVOID* format_str, ... ); // -- make formatted string;
typedef   tERROR (pr_call *fnpString_VFormat)          ( hi_String _this, tCODEPAGE format_cp, const tVOID* format_str, tVOID* args ); // -- ;
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
// AVP Prague stamp begin( External method table prototypes,  )

// ----------  Methods to get string value  ----------
// This methods exports string value to different outputs
tERROR pr_call String_ExportToProp( hi_String _this, tDWORD* result, tSTR_RANGE p_range, hOBJECT p_to, tPROPID p_to_propid );
tERROR pr_call String_ExportToStr( hi_String _this, tDWORD* result, tSTR_RANGE p_range, hSTRING p_to );
tERROR pr_call String_ExportToBuff( hi_String _this, tDWORD* result, tSTR_RANGE p_range, tPTR p_to, tDWORD p_length, tCODEPAGE p_cp, tDWORD p_flags );
tERROR pr_call String_ExportToSeq( hi_String _this, tDWORD* result, tSTR_RANGE p_range, hSEQ_IO p_to );

// ----------  Methods to put string value  ----------
// This methods imports string value from diffeternt sources
tERROR pr_call String_ImportFromProp( hi_String _this, tDWORD* result, hOBJECT p_from, tPROPID p_propid );
tERROR pr_call String_ImportFromStr( hi_String _this, tDWORD* result, hSTRING p_from, tSTR_RANGE p_range );
tERROR pr_call String_ImportFromBuff( hi_String _this, tDWORD* result, tPTR p_from, tDWORD p_length, tCODEPAGE p_cp, tDWORD p_flags );
tERROR pr_call String_ImportFromSeq( hi_String _this, tDWORD* result, hSEQ_IO p_from );

// ----------  Concatenation methods  ----------
// Methods to add value to string from different sources
tERROR pr_call String_AddFromProp( hi_String _this, tDWORD* result, hOBJECT p_from, tPROPID p_propid );
tERROR pr_call String_AddFromStr( hi_String _this, tDWORD* result, hSTRING p_from, tSTR_RANGE p_range );
tERROR pr_call String_AddFromBuff( hi_String _this, tDWORD* result, tPTR p_from, tDWORD p_length, tCODEPAGE p_cp, tDWORD p_flags );
tERROR pr_call String_AddFromSeq( hi_String _this, tDWORD* result, hSEQ_IO p_from );

// ----------  Miscelanous methods  ----------
tERROR pr_call String_Fill( hi_String _this, tSTR_RANGE p_range, tSYMBOL p_symbol, tCODEPAGE p_cp );
tERROR pr_call String_Length( hi_String _this, tDWORD* result );
tERROR pr_call String_LengthEx( hi_String _this, tDWORD* result, tSTR_RANGE p_range, tCODEPAGE p_cp, tDWORD p_flags );
tERROR pr_call String_GetCP( hi_String _this, tCODEPAGE* result );
tERROR pr_call String_SetCP( hi_String _this, tCODEPAGE p_cp );

// ----------  Compare methods  ----------
tERROR pr_call String_Compare( hi_String _this, tSTR_RANGE p_range, hSTRING p_other, tSTR_RANGE p_other_range, tDWORD p_flags );
tERROR pr_call String_CompareBuff( hi_String _this, tSTR_RANGE p_range, tPTR p_buffer, tDWORD p_size, tCODEPAGE p_cp, tDWORD p_flags );
tERROR pr_call String_Compare2( hi_String _this, tSTR_COMPARE* result, tSTR_RANGE p_range, hSTRING p_other, tSTR_RANGE p_other_range, tDWORD p_flags );
tERROR pr_call String_CompareBuff2( hi_String _this, tSTR_COMPARE* result, tSTR_RANGE p_range, tPTR p_buffer, tDWORD p_size, tCODEPAGE p_cp, tDWORD p_flags );

// ----------  Find methods  ----------
tERROR pr_call String_Find( hi_String _this, tSTR_RANGE* result, tSTR_RANGE p_range, hSTRING p_other, tSTR_RANGE p_other_range, tDWORD p_flags );
tERROR pr_call String_FindBuff( hi_String _this, tSTR_RANGE* result, tSTR_RANGE p_range, tPTR p_buffer, tDWORD p_size, tCODEPAGE p_cp, tDWORD p_flags );
tERROR pr_call String_FindOneOf( hi_String _this, tSTR_RANGE* result, tSTR_RANGE p_range, hSTRING p_symbol_set, tDWORD p_flags );
tERROR pr_call String_FindOneOfBuff( hi_String _this, tSTR_RANGE* result, tSTR_RANGE p_range, tPTR p_symbol_set, tDWORD p_size, tCODEPAGE p_cp, tDWORD p_flags );

// ----------  Range concerned methods  ----------
tERROR pr_call String_Symbol( hi_String _this, tSYMBOL* result, tSTR_RANGE p_range, tDWORD p_index, tCODEPAGE p_cp, tDWORD p_flags );
tERROR pr_call String_Next( hi_String _this, tSYMBOL* result, tSTR_RANGE* p_range, tCODEPAGE p_cp );
tERROR pr_call String_NextEx( hi_String _this, tSYMBOL* result, tSTR_RANGE* p_range, tCODEPAGE p_cp );
tERROR pr_call String_Prev( hi_String _this, tSYMBOL* result, tSTR_RANGE* p_range, tCODEPAGE p_cp );
tERROR pr_call String_PrevEx( hi_String _this, tSYMBOL* result, tSTR_RANGE* p_range, tCODEPAGE p_cp );
tERROR pr_call String_Left( hi_String _this, tSTR_RANGE* result, tSTR_RANGE p_range, tDWORD p_count );
tERROR pr_call String_Mid( hi_String _this, tSTR_RANGE* result, tSTR_RANGE p_range, tDWORD p_begin, tDWORD p_count );
tERROR pr_call String_Right( hi_String _this, tSTR_RANGE* result, tSTR_RANGE p_range, tDWORD p_count );

// ----------  Modification methods  ----------
tERROR pr_call String_ChangeCase( hi_String _this, tSTR_CASE p_case_code );
tERROR pr_call String_ChangeCaseEx( hi_String _this, tSTR_RANGE p_range, tSTR_CASE p_case_code );
tERROR pr_call String_Trim( hi_String _this, hSTRING p_symbol_set, tDWORD p_flags );
tERROR pr_call String_TrimBuff( hi_String _this, tPTR p_symbol_set, tDWORD p_size, tCODEPAGE p_cp, tDWORD p_flags );
tERROR pr_call String_Cut( hi_String _this, tSTR_RANGE p_range, tDWORD p_flags );
tERROR pr_call String_Replace( hi_String _this, tSTR_RANGE p_range, hSTRING p_repl, tSTR_RANGE p_repl_range, tSTR_RANGE* p_out_range );
tERROR pr_call String_ReplaceBuff( hi_String _this, tSTR_RANGE p_range, tPTR p_buffer, tDWORD p_size, tCODEPAGE p_cp, tSTR_RANGE* p_out_range );

// ----------  Miscelanous methods  ----------
tERROR pr_call String_Reserve( hi_String _this, tDWORD p_number, tCODEPAGE p_cp );
tERROR pr_call String_ProveLastSlash( hi_String _this );

// ----------  Find methods  ----------
tERROR pr_call String_FindOneNotOfBuff( hi_String _this, tSTR_RANGE* result, tSTR_RANGE p_range, tPTR p_symbol_set, tDWORD p_size, tCODEPAGE p_cp, tDWORD p_flags );

// ----------  Format methods  ----------
tERROR pr_call String_Format( hi_String _this, tCODEPAGE p_format_cp, const tVOID* p_format_str, ... );
tERROR pr_call String_VFormat( hi_String _this, tCODEPAGE p_format_cp, const tVOID* p_format_str, tVOID* p_args );
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External method table,  )
static iStringVtbl e_String_vtbl = {
	(fnpString_ExportToProp)  String_ExportToProp,
	(fnpString_ExportToStr)   String_ExportToStr,
	(fnpString_ExportToBuff)  String_ExportToBuff,
	(fnpString_ExportToSeq)   String_ExportToSeq,
	(fnpString_ImportFromProp) String_ImportFromProp,
	(fnpString_ImportFromStr) String_ImportFromStr,
	(fnpString_ImportFromBuff) String_ImportFromBuff,
	(fnpString_ImportFromSeq) String_ImportFromSeq,
	(fnpString_AddFromProp)   String_AddFromProp,
	(fnpString_AddFromStr)    String_AddFromStr,
	(fnpString_AddFromBuff)   String_AddFromBuff,
	(fnpString_AddFromSeq)    String_AddFromSeq,
	(fnpString_Fill)          String_Fill,
	(fnpString_Length)        String_Length,
	(fnpString_LengthEx)      String_LengthEx,
	(fnpString_GetCP)         String_GetCP,
	(fnpString_SetCP)         String_SetCP,
	(fnpString_Compare)       String_Compare,
	(fnpString_CompareBuff)   String_CompareBuff,
	(fnpString_Compare2)      String_Compare2,
	(fnpString_CompareBuff2)  String_CompareBuff2,
	(fnpString_Find)          String_Find,
	(fnpString_FindBuff)      String_FindBuff,
	(fnpString_FindOneOf)     String_FindOneOf,
	(fnpString_FindOneOfBuff) String_FindOneOfBuff,
	(fnpString_Symbol)        String_Symbol,
	(fnpString_Next)          String_Next,
	(fnpString_NextEx)        String_NextEx,
	(fnpString_Prev)          String_Prev,
	(fnpString_PrevEx)        String_PrevEx,
	(fnpString_Left)          String_Left,
	(fnpString_Mid)           String_Mid,
	(fnpString_Right)         String_Right,
	(fnpString_ChangeCase)    String_ChangeCase,
	(fnpString_ChangeCaseEx)  String_ChangeCaseEx,
	(fnpString_Trim)          String_Trim,
	(fnpString_TrimBuff)      String_TrimBuff,
	(fnpString_Cut)           String_Cut,
	(fnpString_Replace)       String_Replace,
	(fnpString_ReplaceBuff)   String_ReplaceBuff,
	(fnpString_Reserve)       String_Reserve,
	(fnpString_ProveLastSlash) String_ProveLastSlash,
	(fnpString_FindOneNotOfBuff) String_FindOneNotOfBuff,
	(fnpString_Format)        String_Format,
	(fnpString_VFormat)       String_VFormat
};
// AVP Prague stamp end





// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Forwarded property methods declarations,  )
tERROR pr_call String_PROP_CPSet( hi_String _this, tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );
tERROR pr_call String_PROP_LCIDSet( hi_String _this, tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );
tERROR pr_call String_PROP_NameGet( hi_String _this, tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );
tERROR pr_call String_PROP_NameSet( hi_String _this, tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Global property variable declaration,  )
// Interface "String". Static(shared) property table variables
// AVP Prague stamp end



#if defined(ITS_LOADER)
	#define String_VERSION ((tVERSION)2)
#else

// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface version,  )
#define String_VERSION ((tVERSION)1)
// AVP Prague stamp end



#endif

// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Property table,  )
mPROPERTY_TABLE(String_PropTable)
	mSHARED_PROPERTY( pgINTERFACE_VERSION, String_VERSION )
	mSHARED_PROPERTY_PTR( pgINTERFACE_COMMENT, "string", 7 )
	mLOCAL_PROPERTY( pgOBJECT_CODEPAGE, StringData, cp, cPROP_BUFFER_READ, NULL, String_PROP_CPSet )
	mLOCAL_PROPERTY( pgOBJECT_LCID, StringData, lcid, cPROP_BUFFER_READ, NULL, String_PROP_LCIDSet )
	mLOCAL_PROPERTY_FN( pgOBJECT_NAME, String_PROP_NameGet, String_PROP_NameSet )
	mLOCAL_PROPERTY_FN( pgOBJECT_FULL_NAME, String_PROP_NameGet, String_PROP_NameSet )
mPROPERTY_TABLE_END(String_PropTable)
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Registration call,  )
// Interface "String". Register function
tERROR pr_call String_Register( hROOT root ) {
  tERROR error;

  PR_TRACE_A0( root, "Enter String::Register method" );

  error = CALL_Root_RegisterIFace(
    root,                                   // root object
    IID_STRING,                             // interface identifier
    PID_STRING,                             // plugin identifier
    0x00000000,                             // subtype identifier
    String_VERSION,                         // interface version
    VID_PETROVITCH,                         // interface developer
    &i_String_vtbl,                         // internal(kernel called) function table
    (sizeof(i_String_vtbl)/sizeof(tPTR)),   // internal function table size
    &e_String_vtbl,                         // external function table
    (sizeof(e_String_vtbl)/sizeof(tPTR)),   // external function table size
    String_PropTable,                       // property table
    mPROPERTY_TABLE_SIZE(String_PropTable), // property table size
    sizeof(StringData),                     // memory size
    IFACE_SYSTEM                            // interface flags
  );

  #ifdef _PRAGUE_TRACE_
    if ( PR_FAIL(error) )
      PR_TRACE( (root,prtDANGER,"String(IID_STRING) registered [%terr]",error) );
  #endif

    PR_TRACE_A1( root, "Leave String::Register method, ret %terr", error );
  return error;
}
// AVP Prague stamp end






// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Property method implementation, CPSet )
// Interface "String". Method "Set" for property(s):
//  -- OBJECT_CODEPAGE
tERROR pr_call String_PROP_CPSet( hi_String _this, tDWORD* out_size, tPROPID prop, tCHAR* buff, tDWORD size ) {
  tDWORD olen;
  tERROR error = errOK;
  StringData* d;
  tCODEPAGE cp;

  PR_TRACE_A0( _this, "Enter *SET* method String_PROP_CPSet for property pgOBJECT_CODEPAGE" );

  d = _this->data;
  cp = *(tCODEPAGE*)buff;
  MAP_CP( cp );

  if ( d->cp != cp ) {
    tDWORD n; // new length

    switch( cp ) {

      case cCP_NULL:
        error = errCODEPAGE_NOT_SUPPORTED;
        break;

      case cCP_OEM:
        if ( cCP_ANSI == d->cp ) {
            error = PrStringCodec ( d->buff, d->len, cCP_ANSI, d->buff, d->len, cCP_OEM, NULL );
          break;
        }
        else
          goto convert_lbl;
      case cCP_ANSI:
        if ( cCP_OEM == d->cp ) {
            error = PrStringCodec ( d->buff, d->len, cCP_OEM, d->buff, d->len, cCP_ANSI, NULL );
          break;
        }
      case cCP_UNICODE:
convert_lbl:
        if ( d->len )
          error = CalcExportLen( d->buff, d->len, d->cp, cp, cSTRING_CONTENT_ONLY|cSTRING_STRICT_SIZE, &n ); // new length
        else
          n = 0;
        if ( PR_FAIL(error) )
          ;
        else if ( n ) {
          tPTR  b;
          if ( PR_SUCC(error=CALL_SYS_ObjHeapAlloc(_this,&b,n)) ) {
            error = CopyTo( b, n, cp, d->buff, d->len, d->cp, cSTRING_CONTENT_ONLY|cSTRING_STRICT_SIZE, &olen );
            if ( PR_SUCC(error) && ((d->blen >= n) || PR_SUCC(error=CatchMem(_this,n))) ) {
              memcpy( d->buff, b, n );
              d->len = n;
            }
            CALL_SYS_ObjHeapFree( _this, b );
          }
        }
        break;

      case cCP_IBM:
      case cCP_UTF8:
      case cCP_UNICODE_BIGENDIAN:
        error = errCODEPAGE_NOT_SUPPORTED;
        break;
      default:
        error = errPARAMETER_INVALID;
        break;
    }
    if ( error == errOK ) {
      d->cp = cp;
      if ( out_size )
        *out_size = sizeof(tCODEPAGE);
    }
    else if ( out_size )
      *out_size = 0;
  }
  else if ( out_size )
    *out_size = sizeof(tCODEPAGE);

  PR_TRACE_A2( _this, "Leave *SET* method String_PROP_CPSet for property pgOBJECT_CODEPAGE, ret tDWORD = %u(size), %terr", out_size ? *out_size : (error==errOK ? sizeof(tCODEPAGE) : 0), error );
  return error;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Property method implementation, LCIDSet )
// Interface "String". Method "Set" for property(s):
//  -- OBJECT_LCID
tERROR pr_call String_PROP_LCIDSet( hi_String _this, tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size ) {
  tERROR error = errNOT_IMPLEMENTED;
  PR_TRACE_A0( _this, "Enter *SET* method String_PROP_LCIDSet for property pgOBJECT_LCID" );

  *out_size = 0;

  PR_TRACE_A2( _this, "Leave *SET* method String_PROP_LCIDSet for property pgOBJECT_LCID, ret tDWORD = %u(size), %terr", *out_size, error );
  return error;
}
// AVP Prague stamp end




// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Property method implementation, NameGet )
// Interface "String". Method "Set" for property(s):
//  -- OBJECT_NAME
//  -- OBJECT_FULL_NAME
tERROR pr_call String_PROP_NameGet( hi_String _this, tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size ) {
  return String_ExportToBuff( _this, out_size, cSTRING_WHOLE, buffer, size, _this->data->cp, cSTRING_Z );
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Property method implementation, NameGet )
// Interface "String". Method "Set" for property(s):
//  -- OBJECT_NAME
//  -- OBJECT_FULL_NAME
tERROR pr_call String_PROP_NameSet( hi_String _this, tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size ) {
  return String_ImportFromBuff( _this, out_size, buffer, size, _this->data->cp, cSTRING_Z );
}
// AVP Prague stamp end



#if defined (_DEBUG) && defined(CHECK_STRING_COUNT)
  tDWORD g_string_count;
#endif

// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Internal (kernel called) interface method implementation, ObjectInit )
// Extended method comment
//  Kernel notifies an object about successful creating of object
// Behaviour comment
//  Initializes internal object data
// Result comment
//  Returns value differ from errOK if object cannot be initialized
// Parameters are:
tERROR pr_call String_ObjectInit( hi_String _this ) {
  #if defined (_DEBUG) && defined(CHECK_STRING_COUNT)
    tCHAR caller[260];
  #endif

  PR_TRACE_A0( _this, "Enter String::ObjectInit method" );
  _this->data->cp = g_cp_system_default; //cCP_ANSI;

  #if defined (_DEBUG) && defined(CHECK_STRING_COUNT)
    ++g_string_count;
    if ( !PrDbgGetCallerInfo ) {
      CALL_Root_ResolveImportTable( g_root, 0, import_table, PID_STRING );
    }
    if ( !PrDbgGetCallerInfo || PR_FAIL(PrDbgGetCallerInfo("prkernel;prstring",0,caller,sizeof(caller))) )
      *caller = 0;
    PR_TRACE(( _this, prtIMPORTANT, "+String from:\t%04u\t%s", g_string_count, caller ));
  #endif

  PR_TRACE_A0( _this, "Leave String::ObjectInit method, ret tERROR = errOK" );
  return errOK;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Internal (kernel called) interface method implementation, ObjectClose )
// Extended method comment
//    Kernel warns object it must be closed
// Behaviour comment
//    Object takes all necessary "before closing" actions
// Parameters are:
tERROR pr_call String_ObjectClose( hi_String _this ) {
  #if defined (_DEBUG) && defined(CHECK_STRING_COUNT)
    tCHAR caller[260];
  #endif

  PR_TRACE_A0( _this, "Enter String::ObjectClose method" );

  #if defined (_DEBUG) && defined(CHECK_STRING_COUNT)
    --g_string_count;
    if ( !PrDbgGetCallerInfo || PR_FAIL(PrDbgGetCallerInfo("prkernel;prstring",0,caller,sizeof(caller))) )
      *caller = 0;
    PR_TRACE(( _this, prtIMPORTANT, "-String from:\t%04u\t%s", g_string_count, caller ));
  #endif

	STR_CHECK( L"www.kaspersky.com/renew" );

  CALL_SYS_ObjHeapFree( _this, _this->data->buff );
  PR_TRACE_A0( _this, "Leave String::ObjectClose method, ret errOK" );
  return errOK;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, ExportToProp )
// --- Interface "String". Method "ExportToProp"
// Extended method comment
//   Save string content to object's property
// Behaviour comment
//   To save string content to an object it must have two linked properties: binary string content and codepage. Codepage may be assigned to the object or may be default to the object's interface or object's plugin. "Linked" means codepage property idntificator must have the same id part as binary string content has but another (corresponding) type part.
// Parameter "range":
//   range of string to save. Range cSTRING_WHOLE means whole string
// Parameter "to":
//   object to save to
// Parameter "to_propid":
//   propery identificator of string content
tERROR pr_call String_ExportToProp( hi_String _this, tDWORD* result, tSTR_RANGE range, hOBJECT to, tPROPID to_propid ) {
  tDWORD      b, l, nl;
  tERROR      error = errOK;
  tDWORD      ret_val = 0;
  tCODEPAGE   cp;
  StringData* d;
  tPTR        out = 0;
	tPTR        in;
	tDWORD      in_empty;
  PR_TRACE_A0( _this, "Enter String::ExportToProp method" );

  d = _this->data;
  cp = d->cp;

  if ( (pTYPE_STRING != PR_PROP_TYPE(to_propid)) && (pTYPE_WSTRING != PR_PROP_TYPE(to_propid)) )
    error = errPROPERTY_INVALID;
	else {
		error = CALL_SYS_PropertyGetStrCP( to, &cp, to_propid );
		if ( (error == errPROPERTY_NOT_FOUND) && (pRANGE_CUSTOM == PROP_RANGE(to_propid)) )
			error = errOK;
	}

  if ( PR_FAIL(error) )
    goto exit_lbl;

  RANGE( b, l, range, error=errSTR_OUT_OF_RANGE; goto exit_lbl );

	if ( d->buff )
		in = d->buff + b;
	else {
		in_empty = 0;
		in = &in_empty;
	}
  
/*	if ( cp == d->cp ) {
    out = in;
    nl = l;
  }
  else*/ if ( !l ) {
		in_empty = 0;
		out = &in_empty;
		nl = (cp == cCP_UNICODE) ? sizeof(tWCHAR) : sizeof(tCHAR);
  }
	else if ( PR_FAIL(error=CalcExportLen(in,l,d->cp,cp,cSTRING_Z|cSTRING_STRICT_SIZE,&nl)) )
    ;
  else if ( PR_FAIL(error=CALL_SYS_ObjHeapAlloc(_this,&out,nl)) )
		;
  else
    error = CopyTo( out, nl, cp, in, l, d->cp, cSTRING_Z|cSTRING_STRICT_SIZE, &ret_val );

  if ( PR_SUCC(error) )
    error = CALL_SYS_PropertySetStr( to, &ret_val, to_propid, out, nl, cp );

exit_lbl:
  if ( out && (out!= &in_empty) )
    CALL_SYS_ObjHeapFree( _this, out );

  if ( result )
    *result = ret_val;

  PR_TRACE_A2( _this, "Leave String::ExportToProp method, ret tDWORD = %u, %terr", ret_val, error );
  return error;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, ExportToStr )
// --- Interface "String". Method "ExportToStr"
// Extended method comment
//   Saves string content to another string. It is analog of CopyTo functionality
// Parameter "range":
//   range to be copied
// Parameter "to":
//   destination string
tERROR pr_call String_ExportToStr( hi_String _this, tDWORD* result, tSTR_RANGE range, hSTRING to ) {
  tUINT b, l;
  tDWORD ret_val = 0;
  tERROR error = errOK;
  StringData* d = _this->data;
  PR_TRACE_A0( _this, "Enter String::ExportToStr method" );

	if ( !to )
		error = errPARAMETER_INVALID;
	else {
		RANGE( b, l, range, error=errSTR_OUT_OF_RANGE );
		if ( PR_FAIL(error) )
			CALL_String_Cut( (hi_String)to, cSTRING_WHOLE, fSTRING_INNER );
		else if ( !l )
			error = CALL_String_Cut( (hi_String)to, cSTRING_WHOLE, fSTRING_INNER );
		else
			error = CALL_String_ImportFromBuff( (hi_String)to, &ret_val, d->buff+b, l, d->cp, cSTRING_CONTENT_ONLY );
	}

  if ( result )
    *result = ret_val;

  PR_TRACE_A2( _this, "Leave String::ExportToStr method, ret tDWORD = %u, %terr", ret_val, error );
  return error;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, ExportToBuff )
// --- Interface "String". Method "ExportToBuff"
// Extended method comment
//   Saves content of string to target buffer.
// Behaviour comment
//   saves "range" of content to destination buffer using indicated code page
// Parameter "range":
//   range of content to save
// Parameter "to":
//   target buffer
// Parameter "length":
//   length of target buffer
// Parameter "cp":
//   code page of target buffer
// Parameter "flags":
//   determines type of string to be saved: cSTRING_LINE, cSTRING_DLEN, cSTRING_CONTENT_ONLY, etc.
tERROR pr_call String_ExportToBuff( hi_String _this, tDWORD* result, tSTR_RANGE range, tPTR to, tDWORD length, tCODEPAGE cp, tDWORD flags ) {
  tDWORD nl = 0;
  tERROR error;
  StringData* d;
  PR_TRACE_A0( _this, "Enter String::ExportToBuff method" );

  if ( result )
    *result = 0;

  d = _this->data;
  MAP_CP(cp);

  switch( cp ) {

    case cCP_NULL:
      error = errCODEPAGE_NOT_SUPPORTED;
      break;

    case cCP_OEM:
    case cCP_ANSI:
    case cCP_UNICODE:
      error = errOK;
      break;

    case cCP_IBM:
    case cCP_UTF8:
    case cCP_UNICODE_BIGENDIAN:
      error = errCODEPAGE_NOT_SUPPORTED;
      break;
    default:
      error = errPARAMETER_INVALID;
      break;
  }

  if ( PR_SUCC(error) ) {
    tUINT b, l;
    RANGE( b, l, range, l=0 );

    if ( PR_FAIL(error=CalcExportLen(d->buff+b,l,d->cp,cp,flags|cSTRING_STRICT_SIZE,&nl)) )
      ;
    else if ( nl && to )
      error = CopyTo( to, length, cp, d->buff+b, l, d->cp, flags|cSTRING_STRICT_SIZE, &nl );

    if ( result )
      *result = nl;
  }

  PR_TRACE_A2( _this, "Leave String::ExportToBuff method, ret tDWORD = %u, %terr", nl, error );
  return error;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, ExportToSeq )
// --- Interface "String". Method "ExportToSeq"
// Extended method comment
//   Saves content of string to target sequential IO object
// Parameter "range":
//   range of content to save
// Parameter "to":
//   target sequential IO object
// Parameter "cp":
//   code page of target sequential IO object
// Parameter "flags":
//   determines type of string to be saved: cSTRING_LINE, cSTRING_DLEN, cSTRING_CONTENT_ONLY, etc.
tERROR pr_call String_ExportToSeq( hi_String _this, tDWORD* result, tSTR_RANGE range, hSEQ_IO to ) {
	tUINT       b, l;
	tERROR      e;
	StringData* d;
	PR_TRACE_A0( _this, "Enter String::ExportToSeq method" );

	d = _this->data;
	RANGE( b, l, range, l=0 );

	e = errOK;
	if ( to ) {
		tDWORD c, s, t;
		c = s = t = 0;

		if ( PR_FAIL(e=CALL_SeqIO_Write(to,&c,&d->cp,sizeof(d->cp)) ) )
			;
		else if ( PR_SUCC(e=CALL_SeqIO_Write(to,&s,&l,sizeof(l))) )
			;
		else if ( !l )
			;
		else
			e = CALL_SeqIO_Write( to, &t, d->buff+b, l );
		l = c + s + t;
	}
	else
		l += sizeof(d->cp) + sizeof(l);

	if ( result )
		*result = l;

	PR_TRACE_A2( _this, "Leave String::ExportToSeq method, ret tDWORD = %u, %terr", l, e );
	return e;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, ImportFromProp )
// --- Interface "String". Method "ImportFromProp"
// Extended method comment
//   Load string from object's property
// Behaviour comment
//   To load string content from an object it must have two linked properties: string content and codepage. Codepage may be assigned to the object or may be default to the object's interface or object's plugin. "Linked" means codepage property idntificator must have the same id part as string has but another (corresponding) type part.
// Parameter "from":
//   object to load from
// Parameter "propid":
//   propery identificator of the object to load string content
tERROR pr_call String_ImportFromProp( hi_String _this, tDWORD* result, hOBJECT from, tPROPID propid ) {
  tERROR error = errOK;
  tDWORD len = 0;
  tCODEPAGE cp;
  PR_TRACE_A0( _this, "Enter String::ImportFromProp method" );

  if ( ((propid&pTYPE_MASK) != pTYPE_STRING) && ((propid&pTYPE_MASK) != pTYPE_WSTRING) )
    error = errPARAMETER_INVALID;

  else if ( PR_SUCC(error=CALL_SYS_PropertyGetStrCP(from,&cp,propid)) ) {
    StringData* d = _this->data;
    switch( cp ) {
      case cCP_NULL:
        error = errCODEPAGE_NOT_SUPPORTED;
        break;

      case cCP_OEM:
      case cCP_ANSI:
      case cCP_UNICODE:
        if (
          PR_SUCC(error=CALL_SYS_PropertyGetStr(from,&len,propid,0,0,cp)) &&
          ((d->blen >= len) || PR_SUCC(error=CatchMem(_this,len))) &&
          PR_SUCC(error=CALL_SYS_PropertyGetStr(from,&len,propid,d->buff,len,cp))
        ) {
          CalcExportLen( d->buff, len, cp, cp, cSTRING_CONTENT_ONLY|cSTRING_STRICT_SIZE, (tDWORD*) &d->len );
          if ( d->cp == cCP_NULL )
            d->cp = cp;
          else if ( cp != d->cp ) {
            cp    ^= d->cp;
            d->cp ^= cp;
            cp    ^= d->cp;
            error = String_PROP_CPSet(_this, 0, 0, (tPTR)&cp, sizeof(cp) );
          }
        }
        break;

      case cCP_IBM:
      case cCP_UTF8:
      case cCP_UNICODE_BIGENDIAN:
      default:
        error = errCODEPAGE_NOT_SUPPORTED;
        break;
    }
		STR_CHECK( L"www.kaspersky.com/renew" );
  }

  if ( result )
    *result = len;
  PR_TRACE_A2( _this, "Leave String::ImportFromProp method, ret tDWORD = %u, %terr", len, error );
  return error;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, ImportFromStr )
// --- Interface "String". Method "ImportFromStr"
// Extended method comment
//   Receives content from source string ( "from" parameter ) to itself
// Parameter "from":
//   source string
// Parameter "range":
//   range of the source string to be copied
tERROR pr_call String_ImportFromStr( hi_String _this, tDWORD* result, hSTRING from, tSTR_RANGE range ) {
  tERROR      error;
  tDWORD       len;
  StringData* d;
  PR_TRACE_A0( _this, "Enter String::ImportFromStr method" );

  d = _this->data;

  if ( PR_FAIL(error=CALL_String_LengthEx((hi_String)from,&len,range,d->cp,cSTRING_CONTENT_ONLY)) )
    ;

  else if ( (d->blen < len) && PR_FAIL(error=CatchMem(_this,len)) )
    len = 0;

  else if ( PR_FAIL(error = CALL_String_ExportToBuff( (hi_String)from, &len, range,d->buff,len,d->cp,cSTRING_CONTENT_ONLY)) )
    len = 0;

  d->len = len;

	STR_CHECK( L"www.kaspersky.com/renew" );
	
  if ( result )
    *result = len;

  PR_TRACE_A2( _this, "Leave String::ImportFromStr method, ret tDWORD = %u, %terr", len, error );
  return error;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, ImportFromBuff )
// --- Interface "String". Method "ImportFromBuff"
// Extended method comment
//   Loads content from source buffer ( "from" parameter ) to string object
// Parameter "from":
//   source buffer
// Parameter "length":
//   length of the source buffer
// Parameter "cp":
//   code page of the source buffer
// Parameter "flags":
//   determines type of string to be loaded: cSTRING_LINE, cSTRING_DLEN, cSTRING_CONTENT_ONLY, etc.
tERROR pr_call String_ImportFromBuff( hi_String _this, tDWORD* result, tPTR from, tDWORD length, tCODEPAGE cp, tDWORD flags ) {

  tERROR      error;
  StringData* d;
  tUINT       src_len = 0;

  PR_TRACE_A0( _this, "Enter String::ImportFromBuff method" );

  d = _this->data;
  MAP_CP( cp );

  error = i_CalcImportLen( &from, cp, flags, length, (tDWORD*) &src_len );
  if ( PR_SUCC(error) )
    error = CalcExportLen( from, src_len, cp, d->cp, cSTRING_CONTENT_ONLY|cSTRING_STRICT_SIZE, &length );
  if ( PR_SUCC(error) && (d->blen < length) )
    error = CatchMem( _this, length );
  if ( PR_SUCC(error) )
    error=CopyTo( d->buff, d->blen, d->cp, from, src_len, cp, cSTRING_CONTENT_ONLY|cSTRING_STRICT_SIZE, (tDWORD*)&length );
  if ( PR_SUCC(error) )
    d->len = length;
  else
    length = 0;

	STR_CHECK( L"www.kaspersky.com/renew" );

  if ( result )
    *result = length;
  PR_TRACE_A2( _this, "Leave String::ImportFromBuff method, ret tDWORD = %u, %terr", length, error );
  return error;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, ImportFromSeq )
// --- Interface "String". Method "ImportFromSeq"
// Extended method comment
//   Loads content from the source sequential IO object
// Parameter "from":
//   source object
// Parameter "length":
//   length of string in the input stream. May be ((tUCOUNT)-1) if "flags" parameter unambiguously determines length of string content. For example:
// Parameter "flags":
//   determines type of string to be loaded: cSTRING_LINE, cSTRING_DLEN, cSTRING_CONTENT_ONLY, etc.
tERROR pr_call String_ImportFromSeq( hi_String _this, tDWORD* result, hSEQ_IO from ) {
  tERROR e;
  tDWORD  t, c, s;
  StringData* d = _this->data;
  PR_TRACE_A0( _this, "Enter String::ImportFromSeq method" );

  c = s = t = 0;
  if ( from ) {
    tCODEPAGE cp;
    tDWORD    len = 0;
    if ( PR_SUCC(e=CALL_SeqIO_Read(from,&c,&cp,sizeof(cp))) ) {
      if ( PR_SUCC(e=CALL_SeqIO_Read(from,&s,&len,sizeof(len))) ) {
        e = errCODEPAGE_NOT_SUPPORTED;
        if ( (KNOWN_CP(cp) && (d->blen >= len)) || (PR_SUCC(e=CatchMem(_this,len))) ) {
          if (
            PR_SUCC( e = CALL_SeqIO_Read(from,&len,d->buff,len) ) &&
            PR_SUCC( e = CalcExportLen(d->buff,len,cp,cp,cSTRING_CONTENT_ONLY|cSTRING_STRICT_SIZE, (tDWORD*)&d->len) )
          ) {
            if ( d->cp == cCP_NULL )
              d->cp = cp;
            else if ( cp != d->cp ) {
              cp ^= d->cp;
              d->cp ^= cp;
              cp ^= d->cp;
              e = String_PROP_CPSet(_this, 0, 0, (tPTR)&cp, sizeof(cp) );
            }
          }
        }
        else
          CALL_SeqIO_Read( from, &len, 0, len );
      }
      else
        len = 0;
    }
    t = len + c + s;
  }
  else {
    d->len = 0;
    t = 0;
    e = errPARAMETER_INVALID;
  }

  if ( result )
    *result = t;

  PR_TRACE_A2( _this, "Leave String::ImportFromSeq method, ret tDWORD = %u, %terr", t, e );
  return e;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, AddFromProp )
// --- Interface "String". Method "AddFromProp"
// Extended method comment
//   concatenates string and object property
// Result comment
//   returns count of symbols concatenated
// Parameter "from":
//   object to load from
// Parameter "propid":
//   propery identificator of the object to load string content
tERROR pr_call String_AddFromProp( hi_String _this, tDWORD* result, hOBJECT from, tPROPID propid ) {
  tCODEPAGE   cp;
  tERROR      error;
  tDWORD      ret_val = 0;
  tDWORD      prop_len;
  tDWORD      exp_len;
  tPTR        ptr = 0;
  StringData* d;
  PR_TRACE_A0( _this, "Enter String::AddFromProp method" );

  d = _this->data;

  if ( (propid&pTYPE_MASK) != pTYPE_STRING )
    error = errPARAMETER_INVALID;

  else if ( PR_FAIL(error=CALL_SYS_PropertyGetStrCP(from,&cp,propid)) ) // ask prop codepage
    ;

  else if ( !KNOWN_CP(cp) )
    error = errCODEPAGE_NOT_SUPPORTED;

  else if ( PR_FAIL(error=CALL_SYS_PropertyGetStr(from,&prop_len,propid,0,0,cp)) ) // ask prop len for its codepage
    ;

  else if ( cp == d->cp ) {
    if ( (d->blen < (d->len+prop_len)) && PR_FAIL(error=CatchMem(_this,d->len+prop_len)) )
      ;
    else if ( PR_SUCC(error=CALL_SYS_PropertyGetStr(from,&prop_len,propid,d->buff+d->len,prop_len,cp)) ) {
			CalcExportLen( d->buff+d->len, prop_len, cp, d->cp, cSTRING_CONTENT_ONLY|cSTRING_STRICT_SIZE, &prop_len );
			ret_val = prop_len;
      d->len += ret_val;
		}
  }

  else if ( PR_FAIL(error=CALL_SYS_ObjHeapAlloc(_this,&ptr,prop_len)) )
    ;

  else if ( PR_FAIL(error=CALL_SYS_PropertyGetStr(from,&prop_len,propid,ptr,prop_len,cp)) )
    ;

  else if ( PR_FAIL(error=i_CalcImportLen(&ptr,cp,cSTRING_Z,prop_len,&prop_len)) )
    ;

  else if ( PR_FAIL(error=CalcExportLen(ptr,prop_len,cp,d->cp,cSTRING_CONTENT_ONLY|cSTRING_STRICT_SIZE,&exp_len)) )
    ;

  else if ( (d->blen < (d->len+exp_len)) && PR_FAIL(error=CatchMem(_this,d->len+exp_len)) )
    ;

  else if ( PR_FAIL(error=CopyTo(d->buff+d->len,d->blen-d->len,d->cp,ptr,prop_len,cp,cSTRING_CONTENT_ONLY|cSTRING_STRICT_SIZE,&ret_val)) )
    ;

  else
    d->len += ret_val;

  if ( ptr )
    CALL_SYS_ObjHeapFree( _this, ptr );

  if ( result )
    *result = ret_val;
  PR_TRACE_A2( _this, "Leave String::AddFromProp method, ret tDWORD = %u, %terr", ret_val, error );
  return error;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, AddFromStr )
// --- Interface "String". Method "AddFromStr"
// Extended method comment
//   concatenates two strings
// Result comment
//   returns count of symbols concatenated
// Parameter "from":
//   source string
// Parameter "range":
//   range of the source string to be added
tERROR pr_call String_AddFromStr( hi_String _this, tDWORD* result, hSTRING from, tSTR_RANGE range ) {
  tERROR e;
  tDWORD  l;
  StringData* d;
  PR_TRACE_A0( _this, "Enter String::AddFromStr method" );

  d = _this->data;
  if (
    PR_FAIL(e=CALL_String_ExportToBuff((hi_String)from,&l,range,0,0,d->cp,cSTRING_CONTENT_ONLY)) ||
    ( (d->blen < d->len+l) && PR_FAIL(e = CatchMem(_this,d->len+l)) ) ||
    PR_FAIL(e = CALL_String_ExportToBuff((hi_String)from,&l,range,d->buff+d->len,l,d->cp,cSTRING_CONTENT_ONLY) )
  )
    l = 0;
  else
    d->len += l;

  if ( result )
    *result = l;

  PR_TRACE_A2( _this, "Leave String::AddFromStr method, ret tDWORD = %u, %terr", l, e );
  return e;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, AddFromBuff )
// --- Interface "String". Method "AddFromBuff"
// Extended method comment
//   concatenates own and buffer content
// Result comment
//   returns count of symbols concatenated
// Parameter "from":
//   source buffer
// Parameter "length":
//   length of the source buffer
// Parameter "cp":
//   code page of the source buffer
// Parameter "flags":
//   determines type of string to be loaded: cSTRING_LINE, cSTRING_DLEN, cSTRING_CONTENT_ONLY, etc.
tERROR pr_call String_AddFromBuff( hi_String _this, tDWORD* result, tPTR from, tDWORD length, tCODEPAGE cp, tDWORD flags ) {
  tERROR e;
  StringData* d;
  tDWORD      src_len;
  PR_TRACE_A0( _this, "Enter String::AddFromBuff method" );

  d = _this->data;
  MAP_CP( cp );

  if (
    PR_SUCC(e=i_CalcImportLen(&from,cp,flags,length,&src_len)) &&
    PR_SUCC(e=CalcExportLen(from,src_len,cp,d->cp,cSTRING_CONTENT_ONLY|cSTRING_STRICT_SIZE,&length)) &&
    ( (d->blen >= (d->len+length)) || PR_SUCC(e=CatchMem(_this,d->len+length)) ) &&
    PR_SUCC(e=CopyTo(d->buff+d->len,d->blen-d->len,d->cp,from,src_len,cp,cSTRING_CONTENT_ONLY|cSTRING_STRICT_SIZE,&length))
  )
    d->len += length;
  else
    length = 0;

  if ( result )
    *result = length;

  PR_TRACE_A2( _this, "Leave String::AddFromBuff method, ret tDWORD = %u, %terr", length, e );
  return e;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, AddFromSeq )
// --- Interface "String". Method "AddFromSeq"
// Parameter "from":
//   source object
// Parameter "length":
//   length of string in the input stream. May be ((tUCOUNT)-1) if "flags" parameter unambiguously determines length of string content. For example:
// Parameter "flags":
//   determines type of string to be added: cSTRING_LINE, cSTRING_DLEN, cSTRING_CONTENT_ONLY, etc.
tERROR pr_call String_AddFromSeq( hi_String _this, tDWORD* result, hSEQ_IO from ) {
  tERROR e;
  tDWORD  t, c, s;
  PR_TRACE_A0( _this, "Enter String::AddFromSeq method" );

  c = s = t = 0;
  if ( from ) {
    tPTR        p;
    tCODEPAGE   cp;
    tDWORD      len;
    StringData* d = _this->data;
    if (
      PR_SUCC(e=CALL_SeqIO_Read(from,&c,&cp,sizeof(cp))) &&
      PR_SUCC(e=CALL_SeqIO_Read(from,&s,&len,sizeof(len)))
    ) {
      if ( cp == d->cp ) {
        if (
          (d->blen >= d->len+len) || (PR_SUCC(e=CatchMem(_this,len)) &&
          PR_SUCC(e = CALL_SeqIO_Read(from,&t,d->buff+d->len,len)) )
        )
          d->len += t;
      }
      else if ( PR_SUCC(e=CALL_SYS_ObjHeapAlloc(_this,&p,len)) ) {
        if (
          PR_SUCC(e=CALL_SeqIO_Read(from,&t,p,len)) &&
          PR_SUCC(e=String_AddFromBuff(_this,&t,p,len,cp,cSTRING_CONTENT_ONLY))
        )
        CALL_SYS_ObjHeapFree( _this, p );
      }
      t += c + s;
    }
  }
  else {
    t = 0;
    e = errPARAMETER_INVALID;
  }

  if ( result )
    *result = t;
  PR_TRACE_A2( _this, "Leave String::AddFromSeq method, ret tDWORD = %u, %terr", t, e );
  return e;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, Fill )
// --- Interface "String". Method "Fill"
// Extended method comment
//   Clears string content and refill string with the indicated symbol.
// Parameter "range":
//   range of string to fill
// Parameter "symbol":
//   symbol to fill string
// Parameter "cp":
//   code page of the symbol to fill string
tERROR pr_call String_Fill( hi_String _this, tSTR_RANGE range, tSYMBOL symbol, tCODEPAGE cp ) {
  tUINT b, l;
  StringData* d;
  tERROR error = errOK;
  PR_TRACE_A0( _this, "Enter String::Fill method" );

  d = _this->data;
  MAP_CP( cp );
  RANGE_WOCHECK( b, l, range );

  if ( b > d->len )
    error = errSTR_OUT_OF_RANGE;

  else if ( d->blen < (b+l) && PR_FAIL(CatchMem(_this,b+l)) )
    error = errNOT_ENOUGH_MEMORY;

  else if ( cp != d->cp ) {
    tSYMBOL sym = symbol;
    tUINT   sl, dl;
    switch( cp ) {
      default:
      case cCP_ANSI:
      case cCP_OEM:      sl = sizeof(tCHAR);  break;
      case cCP_UNICODE : sl = sizeof(tWCHAR); break;
    }
    switch( d->cp ) {
      default:
      case cCP_ANSI:
      case cCP_OEM:      dl = sizeof(tCHAR);  break;
      case cCP_UNICODE : dl = sizeof(tWCHAR); break;
    }
    error = CopyToEx( &symbol, dl, d->cp, 0, &sym, sl, cp, 0, 0 );
  }
  else
    error = errOK;

  if ( PR_FAIL(error) )
    ;
  else if ( d->cp == cCP_UNICODE ) {
    tWCHAR* p = UNI(d->buff+b);
    tWCHAR* s = UNI(d->buff+b+l);
    while( p < s )
      *(p++) = symbol;
    if ( d->len < (b+l) )
      d->len = b + l;
  }
  else {
    memset( d->buff+b, (tCHAR)symbol, l );
    if ( d->len < (b+l) )
      d->len = b + l;
  }

  PR_TRACE_A1( _this, "Leave String::Fill method, ret %terr", error );
  return error;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, Length )
// --- Interface "String". Method "Length"
// Extended method comment
//   Length of content of the string in symbols
// Behaviour comment
//
tERROR pr_call String_Length( hi_String _this, tDWORD* result ) {
  PR_TRACE_A0( _this, "Enter String::Length method" );
  if ( result )
    *result = _this->data->len;
  PR_TRACE_A1( _this, "Leave String::Length method, ret tDWORD = %u, error = errOK", _this->data->len );
  return errOK;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, LengthEx )
// --- Interface "String". Method "LengthEx"
// Extended method comment
//   (sub)string length for format defined by "code_page" and "flags" parameters, i.e. size of output buffer to put (sub)string in requested format
// Behaviour comment
//
// Parameter "range":
//   substring indicated for calculate length, cSTRING_WHOLE is a whole string
// Parameter "cp":
//   code page of the string to calculate
// Parameter "flags":
//   Format of the return value. May be one of string format constants.
tERROR pr_call String_LengthEx( hi_String _this, tDWORD* result, tSTR_RANGE range, tCODEPAGE cp, tDWORD flags ) {
  tERROR error;
  tDWORD  ret_val = 0;
  StringData* d;
  tUINT b, l;
  PR_TRACE_A0( _this, "Enter String::LengthEx method" );

  d = _this->data;
  MAP_CP( cp );
  RANGE( b, l, range, error=errSTR_OUT_OF_RANGE; goto exit );

  if ( !l ) {
		error = errOK;
		if ( !(flags & cSTRING_Z) )
			ret_val = 0;
		else if ( cCP_UNICODE )
			ret_val = sizeof(tWCHAR);
		else
			ret_val = sizeof(tCHAR);
	}
	else if( PR_FAIL(error=CalcExportLen(d->buff+b,l,d->cp,cp,flags|cSTRING_STRICT_SIZE,&ret_val)) )
    ret_val = 0;

exit:
  if ( result )
    *result = ret_val;
  PR_TRACE_A2( _this, "Leave String::LengthEx method, ret tDWORD = %u, %terr", ret_val, error );
  return error;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, GetCP )
// --- Interface "String". Method "GetCP"
// Extended method comment
//   Returns current string code page
tERROR pr_call String_GetCP( hi_String _this, tCODEPAGE* result ) {
  PR_TRACE_A0( _this, "Enter String::GetCP method" );
  if ( result )
    *result = _this->data->cp;
  PR_TRACE_A1( _this, "Leave String::GetCP method, ret tCODEPAGE = %u, error = errOK", _this->data->cp );
  return errOK;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, SetCP )
// --- Interface "String". Method "SetCP"
// Extended method comment
//   Sets default code page for the string
// Parameter "cp":
//   code page to set as default
tERROR pr_call String_SetCP( hi_String _this, tCODEPAGE cp ) {
  tERROR error;
  tDWORD  size;
  StringData* d = _this->data;
  PR_TRACE_A0( _this, "Enter String::SetCP method" );

  MAP_CP( cp );

  error = String_PROP_CPSet( _this, &size, pgOBJECT_CODEPAGE, MB(&cp), sizeof(tCODEPAGE) );
  PR_TRACE_A1( _this, "Leave String::SetCP method, ret %terr", error );
  return error;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, Compare )
// --- Interface "String". Method "Compare"
// Extended method comment
//   Compares the string with another one.
// Result comment
//   return values are:
// Parameter "other":
//   string to compare
// Parameter "flags":
//   how to compare strings. Must be bitwise combination of COMPARE flags
//     fSTRING_COMPARE_CASE_SENSITIVE
//     fSTRING_COMPARE_LEN_SENSITIVE
//     fSTRING_COMPARE_UPPER
//     fSTRING_COMPARE_LOWER
//
tERROR pr_call String_Compare( hi_String _this, tSTR_RANGE range, hSTRING other, tSTR_RANGE other_range, tDWORD flags ) {
	tERROR error = errOK;
	tUINT b, l;
	StringData* d = _this->data;
	PR_TRACE_A0( _this, "Enter String::Compare method" );
	
	if ( !other ) {
		d = _this->data;
		RANGE( b, l, range, ; );
		if ( l )
			error = errNOT_MATCHED;
	}
	
	else if ( PID_STRING == CALL_SYS_PropertyGetDWord(other,pgPLUGIN_ID) ) {
		d = ((hi_String)other)->data;
		RANGE( b, l, other_range, error=errSTR_OUT_OF_RANGE );
		if ( PR_SUCC(error) ) {
			if ( l )
				error = String_CompareBuff( _this, range, d->buff+b, l, d->cp, flags );
			else if ( !_this->data->len )
				error = errOK;
			else
				error = errNOT_MATCHED;
		}
	}
	else {
		RANGE( b, l, range, error=errSTR_OUT_OF_RANGE );
		if ( PR_SUCC(error) )
			error = CALL_String_CompareBuff( (hi_String)other, other_range, d->buff+b, l, d->cp, flags|cSTRING_CONTENT_ONLY );
	}
	
	PR_TRACE_A1( _this, "Leave String::Compare method, ret tBOOL = see above, %terr", error );
	return error;
}
// AVP Prague stamp end



// ---
tERROR pr_call _adjust_cp( hi_String _this, tStrDsc* f, tStrDsc* s, tSTR_COMPARE* check_len ) {
  tERROR err;
  tPTR   tmp_ptr = 0;
  tDWORD tmp_len = 0;
  tStrDsc*  t;
  
  if ( f->m_cp == s->m_cp ) {
    if ( check_len ) {
      if ( f->m_size < s->m_size ) {
        *check_len = cSTRING_COMP_LT;
        return errNOT_MATCHED;
      }
      else if ( f->m_size > s->m_size ) {
        *check_len = cSTRING_COMP_GT;
        return errNOT_MATCHED;
      }
      else
        *check_len = cSTRING_COMP_EQ;
    }
    return errOK;
  }

  if ( s->m_cp == cCP_UNICODE ) {
    t = f;
    f = s;
    s = t;
  }
  else
    t = 0;

  // first str is UNICODE - convert other to it!!!
  err = CalcExportLen( s->m_ptr, s->m_size, s->m_cp, f->m_cp, cSTRING_CONTENT_ONLY|cSTRING_STRICT_SIZE, &tmp_len );
  if ( PR_SUCC(err) ) { 
    if ( !check_len )
      ;
    else if ( f->m_size == tmp_len )
      *check_len = cSTRING_COMP_EQ;
    else {
      if ( t ) { // has been rotated;
        if ( f->m_size > tmp_len )
          *check_len = cSTRING_COMP_LT;
        else
          *check_len = cSTRING_COMP_GT;
      }
      else if ( f->m_size > tmp_len )
        *check_len = cSTRING_COMP_GT;
      else
        *check_len = cSTRING_COMP_LT;
      return errNOT_MATCHED;
    }
    err = CALL_SYS_ObjHeapAlloc( _this, &tmp_ptr, tmp_len );
  }

  if ( PR_SUCC(err) && tmp_len )
    err = CopyTo( tmp_ptr, tmp_len, f->m_cp, s->m_ptr, s->m_size, s->m_cp, cSTRING_CONTENT_ONLY|cSTRING_STRICT_SIZE, &tmp_len );
  if ( PR_SUCC(err) ) {
    s->m_ptr = tmp_ptr;
    s->m_size = tmp_len;
    s->m_cp  = f->m_cp;
  }
  return err;
}


// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, CompareBuff )
// --- Interface "String". Method "CompareBuff"
// Extended method comment
//   compares string with buffer
// Parameter "buffer":
//   buffer to compare
// Parameter "size":
//   size of buffer to compare
// Parameter "cp":
//   code page of buffer to compare
// Parameter "flags":
//   how to compare strings. Must be bitwise combination of COMPARE flags
//     fSTRING_COMPARE_CASE_SENSITIVE
//     fSTRING_COMPARE_LEN_SENSITIVE
//     fSTRING_COMPARE_UPPER
//     fSTRING_COMPARE_LOWER
//
tERROR pr_call String_CompareBuff( hi_String _this, tSTR_RANGE range, tPTR buffer, tDWORD size, tCODEPAGE cp, tDWORD flags ) {
	tERROR      error;
	StringData* d;
	tUINT       b, l;
	PR_TRACE_A0( _this, "Enter String::CompareBuff method" );

	error = errOK;
	d = _this->data;
	RANGE( b, l, range, error=errSTR_OUT_OF_RANGE; );

	if ( PR_SUCC(error) ) {
		tUINT len;
		MAP_CP( cp );

		if ( PR_SUCC(error=i_CalcImportLen(&buffer,cp,flags,size,&len)) ) {
			tStrDsc me    = { d->buff + b, l, d->cp };
			tStrDsc other = { buffer, len, cp };
			tSTR_COMPARE len_cmp_res;

			if ( _cmpS_ASCII(&len_cmp_res,&me,&other,flags) )
				error = ( (len_cmp_res == cSTRING_COMP_EQ) ? errOK : errNOT_MATCHED );
			else {
				tPTR me_ptr_before_adjust = me.m_ptr;
				tPTR other_ptr_before_adjust = other.m_ptr;

				error = _adjust_cp( _this, &me, &other, &len_cmp_res ); //for BOOL result value can do it always before real compare
				if ( PR_SUCC(error) )
					error = _cmp( me.m_ptr, other.m_ptr, me.m_cp, me.m_size, flags );
				if ( me.m_ptr != me_ptr_before_adjust )
					CALL_SYS_ObjHeapFree( _this, me.m_ptr );
				if ( other.m_ptr != other_ptr_before_adjust )
					CALL_SYS_ObjHeapFree( _this, other.m_ptr );
			}
		}
	}

	PR_TRACE_A1( _this, "Leave String::CompareBuff method, ret %terr", error );
	return error;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, Compare2 )
// --- Interface "String". Method "Compare2"
// Extended method comment
//   Compares the string with another one.
// Result comment
//   return values are:
// Parameter "other":
//   string to compare
// Parameter "flags":
//   how to compare strings. Must be bitwise combination of COMPARE flags
//     fSTRING_COMPARE_CASE_SENSITIVE
//     fSTRING_COMPARE_LEN_SENSITIVE
//     fSTRING_COMPARE_UPPER
//     fSTRING_COMPARE_LOWER
//
tERROR pr_call String_Compare2( hi_String _this, tSTR_COMPARE* result, tSTR_RANGE range, hSTRING other, tSTR_RANGE other_range, tDWORD flags ) {
	StringData* d;
	tERROR error = errOK;
	PR_TRACE_A0( _this, "Enter String::Compare2 method" );
	
	if ( !other ) {
		if ( result ) {
			tUINT b, l;
			d = _this->data;
			RANGE( b, l, range, ; );
			if ( l )
				*result = cSTRING_COMP_GT;
			else
				*result = cSTRING_COMP_EQ;
		}
	}
	
	else if ( PID_STRING == CALL_SYS_PropertyGetDWord(other,pgPLUGIN_ID) ) {
		tUINT b, l;
		d = ((hi_String)other)->data;
		RANGE( b, l, other_range, error=errSTR_OUT_OF_RANGE );
		if ( PR_SUCC(error) ) {
			if ( !l ) {
				if ( _this->data->len )
					*result = cSTRING_COMP_GT;
				else
					*result = cSTRING_COMP_EQ;
				error = errOK;
			}
			else
				error = String_CompareBuff2( _this, result, range, d->buff+b, l, d->cp, flags );
		}
	}
	
	else {
		tBYTE  tmp[100];
		tBYTE* ptmp = tmp;
		tDWORD size = sizeof(tmp);
		d = _this->data;
		
		error = CALL_String_ExportToBuff( (hi_String)other, &size, other_range, ptmp, size, d->cp, 0 );
		if ( (error == errBUFFER_TOO_SMALL) && PR_SUCC(error= CALL_SYS_ObjHeapAlloc(_this,&ptmp,size)) )
			error = CALL_String_ExportToBuff( (hi_String)other, &size, other_range, ptmp, size, d->cp, 0 );
		if ( PR_SUCC(error) )
			error = String_CompareBuff2( _this, result, range, ptmp, size, d->cp, flags );
		if ( ptmp != tmp )
			CALL_SYS_ObjHeapFree( _this, ptmp );
	}
	
	PR_TRACE_A2( _this, "Leave String::Compare2 method, ret tSTR_COMPARE = %p, %terr", ret_val, error );
	return error;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, CompareBuff2 )
// --- Interface "String". Method "CompareBuff2"
// Extended method comment
//   compares string with buffer
// Parameter "buffer":
//   buffer to compare
// Parameter "size":
//   size of buffer to compare
// Parameter "cp":
//   code page of buffer to compare
// Parameter "flags":
//   how to compare strings. Must be bitwise combination of COMPARE flags
//     fSTRING_COMPARE_CASE_SENSITIVE
//     fSTRING_COMPARE_LEN_SENSITIVE
//     fSTRING_COMPARE_UPPER
//     fSTRING_COMPARE_LOWER
//
tERROR pr_call String_CompareBuff2( hi_String _this, tSTR_COMPARE* result, tSTR_RANGE range, tPTR buffer, tDWORD size, tCODEPAGE cp, tDWORD flags ) {

	tERROR      error;
	tDWORD      len;
	StringData* d;
	tUINT       b, l;
	tSTR_COMPARE ret_val = cSTRING_COMP_UNDEF;
	PR_TRACE_A0( _this, "Enter String::CompareBuff2 method" );

	error = errOK;
	d = _this->data;
	RANGE( b, l, range, error=errSTR_OUT_OF_RANGE );

	if ( PR_SUCC(error) ) {
		MAP_CP( cp );

		if ( PR_SUCC(error=i_CalcImportLen(&buffer,cp,flags,size,&len)) ) {
			tStrDsc me    = { d->buff + b, l, d->cp };
			tStrDsc other = { buffer, len, cp };

			if ( !_cmpS_ASCII(&ret_val,&me,&other,flags) ) {
				tPTR me_ptr_before_adjust = me.m_ptr;
				tPTR other_ptr_before_adjust = other.m_ptr;

				error = _adjust_cp( _this, &me, &other, (flags & fSTRING_COMPARE_LEN_SENSITIVE) ? &ret_val : 0 );
				if ( PR_SUCC(error) ) {
					if ( me.m_size <= other.m_size )
						len = me.m_size;
					else
						len = other.m_size;

					ret_val = _cmpS( me.m_ptr, other.m_ptr, me.m_cp, len, flags );

					if ( ret_val != cSTRING_COMP_EQ )
						error = errNOT_MATCHED;
					else if ( me.m_size < other.m_size ) {
						ret_val = cSTRING_COMP_LT;
						error = errNOT_MATCHED;
					}
					else if ( me.m_size > other.m_size ) {
						ret_val = cSTRING_COMP_GT;
						error = errNOT_MATCHED;
					}
				}

				if ( me.m_ptr != me_ptr_before_adjust )
					CALL_SYS_ObjHeapFree( _this, me.m_ptr );
				if ( other.m_ptr != other_ptr_before_adjust )
					CALL_SYS_ObjHeapFree( _this, other.m_ptr );
			}
		}
	}
	if ( result )
		*result = ret_val;

	PR_TRACE_A2( _this, "Leave String::CompareBuff2 method, ret tSTR_COMPARE = %p, %terr", ret_val, error );
	return error;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, Find )
// --- Interface "String". Method "Find"
// Extended method comment
//   finds substring
// Parameter "other":
//   string to find
// Parameter "flags":
//   how to find, must be one of:
//     fSTRING_FORWARD
//     fSTRING_BACKWARD
//
tERROR pr_call String_Find( hi_String _this, tSTR_RANGE* result, tSTR_RANGE range, hSTRING other, tSTR_RANGE other_range, tDWORD flags ) {

	StringData* d;
	tERROR error = errOK;
	PR_TRACE_A0( _this, "Enter String::Find method" );
	
	if ( !other ) {
		if ( result )
			*result = MR( 0, 0 );
	}
	
	else if ( PID_STRING == CALL_SYS_PropertyGetDWord(other,pgPLUGIN_ID) ) {
		tUINT b, l;
		d = ((hi_String)other)->data;
		RANGE( b, l, other_range, error=errSTR_OUT_OF_RANGE );
		if ( PR_SUCC(error) ) {
			if ( !l )
				error = errNOT_FOUND;
			else
				error = String_FindBuff( _this, result, range, d->buff+b, l, d->cp, flags );
		}
	}
	
	else {
		tBYTE  tmp[100];
		tBYTE* ptmp = tmp;
		tDWORD size = sizeof(tmp);
		d = _this->data;
		
		error = CALL_String_ExportToBuff( (hi_String)other, &size, other_range, ptmp, size, d->cp, 0 );
		if ( (error == errBUFFER_TOO_SMALL) && PR_SUCC(error= CALL_SYS_ObjHeapAlloc(_this,&ptmp,size)) )
			error = CALL_String_ExportToBuff( (hi_String)other, &size, other_range, ptmp, size, d->cp, 0 );
		if ( PR_SUCC(error) )
			error = String_FindBuff( _this, result, range, ptmp, size, d->cp, flags );
		if ( ptmp != tmp )
			CALL_SYS_ObjHeapFree( _this, ptmp );
	}
	
	PR_TRACE_A1( _this, "Leave String::Find method, %terr", error );
	return error;


}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, FindBuff )
// --- Interface "String". Method "FindBuff"
// Extended method comment
//   Finds substring given as buffer
// Parameter "buffer":
//   buffer to find
// Parameter "size":
//   size of input buffer
// Parameter "cp":
//   code page of input buffer
// Parameter "flags":
//   how to find, must be one of:
//     fSTRING_FORWARD
//     fSTRING_BACKWARD
//
tERROR pr_call String_FindBuff( hi_String _this, tSTR_RANGE* result, tSTR_RANGE range, tPTR buffer, tDWORD len, tCODEPAGE cp, tDWORD flags ) {
	tERROR      error = errOK;
	tSTR_RANGE  ret_val = cSTRING_EMPTY;
	StringData* d;
	tUINT       start_offset, find_len;
	PR_TRACE_A0( _this, "Enter String::FindBuff method" );

	d = _this->data;
	MAP_CP( cp );

	RANGE( start_offset, find_len, range, error=errSTR_OUT_OF_RANGE );

	if ( PR_FAIL(error) )
		;
	
	else if ( !find_len )
		error = errSTR_OUT_OF_RANGE;

	else if ( PR_FAIL(error=i_CalcImportLen(&buffer,cp,flags,len,&len)) )
		;
	
	else {
		tStrDsc me = { d->buff+start_offset, find_len, d->cp };
		tStrDsc other = { buffer, len, cp };
		error = _adjust_cp( _this, &me, &other, 0 );
		if ( PR_FAIL(error) )
			;
		else if ( me.m_size < other.m_size )
			error = errNOT_FOUND;
		else if ( !me.m_size )
			ret_val = MR( 0, 0 );
		else {
			tINT   c, i, j, dir, step, so = start_offset;
			tCHAR* ps_for_find;
			tCHAR* pf = other.m_ptr; // string to find

			if ( me.m_cp == cCP_UNICODE )
				step = sizeof(tWCHAR);
			else
				step = sizeof(tCHAR);

			if ( d->cp != me.m_cp )
				so *= sizeof(tWCHAR);

			if ( flags & fSTRING_FIND_BACKWARD ) {
				i = me.m_size - other.m_size;
				dir = -step;
			}
			else {
				i = 0;
				dir = step;
			}
			ps_for_find = MB(me.m_ptr) + i;

			error = errNOT_FOUND;
			for( j=0,c=me.m_size-other.m_size+step; j<c; j+=step,i+=dir,ps_for_find+=dir ) {
				if ( PR_SUCC(_cmp(ps_for_find,pf,me.m_cp,other.m_size,flags)) ) {
					ret_val = MR( ((i+so)/step), other.m_size/step );
					error = errOK;
					break;
				}
			}
		}
		if ( me.m_ptr != (d->buff+start_offset) )
			CALL_SYS_ObjHeapFree( _this, me.m_ptr );
		if ( other.m_ptr != buffer )
			CALL_SYS_ObjHeapFree( _this, other.m_ptr );
	}

	/*
	if ( PR_FAIL(error) )
		;
	else if ( find_len < len )
		error = errNOT_FOUND;
	else {
		tCHAR* ps_for_find = 0;
		tINT   c, i, dir;
		tCHAR* pf = buffer; // string to find
		tINT   step;

		if ( d->cp == cCP_UNICODE )
			step = sizeof(tWCHAR);
		else
			step = sizeof(tCHAR);

		if ( flags & fSTRING_FIND_BACKWARD ) {
			dir = -step;
			ps_for_find = d->buff + start_offset + find_len - len;
		}
		else {
			dir = step;
			ps_for_find = d->buff + start_offset;
		}

		if ( cp == d->cp )
			pf = buffer;
		else if ( PR_FAIL(error=CALL_SYS_ObjHeapAlloc(_this,(tPTR*)&pf,len)) )
			;
		else if ( PR_FAIL(error=CopyTo(pf,len,d->cp,buffer,in_len,cp,cSTRING_CONTENT_ONLY|cSTRING_STRICT_SIZE,&len)) )
			;
		else
			flags = (flags & ~mSTRING_FORMAT) | cSTRING_CONTENT_ONLY;

		if ( PR_SUCC(error) ) {
			error = errNOT_FOUND;
			for( i=0,c=find_len-len+step; i<c; i+=step,ps_for_find+=dir ) {
				if ( PR_SUCC(_cmp(ps_for_find,pf,d->cp,len,flags)) ) {
					ret_val = MR( ((i+start_offset)/step), len/step );
					error = errOK;
					break;
				}
			}
		}

		if ( pf && (pf != buffer) )
			CALL_SYS_ObjHeapFree( _this, pf );
	}

	*/
	if ( result )
		*result = ret_val;
	PR_TRACE_A1( _this, "Leave String::FindBuff method, ret tSTR_RANGE = %p, error = errOK", ret_val );
	return error;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, FindOneOf )
// --- Interface "String". Method "FindOneOf"
// Extended method comment
//   Finds one of symbols belonged to "symbol_set"
// Parameter "symbol_set":
//   symbol set to find
// Parameter "flags":
//   how to find, must be one of:
//     fSTRING_FORWARD
//     fSTRING_BACKWARD
//
tERROR pr_call String_FindOneOf( hi_String _this, tSTR_RANGE* result, tSTR_RANGE range, hSTRING symbol_set, tDWORD flags ) {
  tERROR      error = errOK;
  tPTR        ss = 0;
  tDWORD      len;
  tSTR_RANGE  ret_val = cSTRING_EMPTY;
  tUINT       b, l;
  StringData* d = _this->data;
  PR_TRACE_A0( _this, "Enter String::FindOneOf method" );
  RANGE( b, l, range, error=errSTR_OUT_OF_RANGE );

	if ( PR_FAIL(error) )
		;
	else if ( !l )
		error = errSTR_OUT_OF_RANGE;
	else if ( PR_FAIL(error=CALL_String_ExportToBuff((hi_String)symbol_set,&len,cSTRING_WHOLE,0,0,d->cp,cSTRING_CONTENT_ONLY)) ) 
		;
	else if ( PR_FAIL(error=CALL_SYS_ObjHeapAlloc(_this,&ss,len)) )
		;
	else if ( PR_FAIL(error=CALL_String_ExportToBuff((hi_String)symbol_set,&len,cSTRING_WHOLE,ss,len,d->cp,cSTRING_CONTENT_ONLY)) )
		;
	else
    error = String_FindOneOfBuff( _this, &ret_val, range, ss, len, d->cp, (flags & ~mSTRING_FORMAT)|cSTRING_CONTENT_ONLY );

  if ( ss )
    CALL_SYS_ObjHeapFree( _this, ss );
  if ( result )
    *result = ret_val;
  PR_TRACE_A2( _this, "Leave String::FindOneOf method, ret tSTR_RANGE = %p, %terr", ret_val, error );
  return error;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, FindOneOfBuff )
// --- Interface "String". Method "FindOneOfBuff"
// Extended method comment
//   Finds one of symbols belonged to "symbol_set"
// Parameter "symbol_set":
//   symbol set to find one of
// Parameter "size":
//   size of symbol set
// Parameter "cp":
//   code page of the symbol set
// Parameter "flags":
//   how to find, must be one of:
//     fSTRING_FORWARD
//     fSTRING_BACKWARD
//
tERROR pr_call String_FindOneOfBuff( hi_String _this, tSTR_RANGE* result, tSTR_RANGE range, tPTR symbol_set, tDWORD len, tCODEPAGE cp, tDWORD flags ) {
  tERROR     error = errOK;
  tSTR_RANGE ret_val = cSTRING_EMPTY;
  tUINT      b, l;
  StringData* d = _this->data;
	tBYTE      local_set[0x40];
  tPTR       ptr = 0;

  PR_TRACE_A0( _this, "Enter String::FindOneOfBuff method" );
  RANGE( b, l, range, error=errSTR_OUT_OF_RANGE );

  if ( PR_FAIL(error) )
    ;

  else if ( !l )
    error = errNOT_FOUND;

  else if ( !symbol_set )
    error = errPARAMETER_INVALID;

  else {
    tDWORD i, j;
    MAP_CP( cp );

    switch( cp ) {
      case cCP_OEM:
      case cCP_ANSI:
      case cCP_UNICODE:             break;
      case cCP_IBM:
      case cCP_UTF8:
      case cCP_UNICODE_BIGENDIAN:
      default:                      error = errCODEPAGE_NOT_SUPPORTED;break;
    }

    if ( PR_FAIL(error) )
      ;
    else if ( PR_FAIL(error=i_CalcImportLen(&symbol_set,cp,cSTRING_Z,len,&len)) )
      ;
    else if ( !len )
      error = errPARAMETER_INVALID;

    else {
      tCHAR* c;
      tCHAR* p;
      tINT nDir, step;
      tUINT nIndex;

      if ( d->cp == cCP_UNICODE )
        step = sizeof(tWCHAR);
      else
        step = sizeof(tCHAR);

      if ( flags & fSTRING_FIND_BACKWARD ) {
        nDir = -1 ;
        i = l - step;
        nIndex = b + i;
      }
      else {
        nDir = 1 ;
        nIndex = b ;
        i = 0 ;
      }

      if ( d->cp != cp ) {
				tDWORD size = 0;
				if ( PR_FAIL(error=CalcExportLen(symbol_set,len,cp,d->cp,0,&size)) )
					goto exit;
				if ( size <= sizeof(local_set) ) {
					size = sizeof(local_set);
					ptr = local_set;
				}
        else if ( PR_FAIL(error=CALL_SYS_ObjHeapAlloc(_this,&ptr,len)) )
          goto exit;
        if ( PR_FAIL(error=CopyTo(ptr,size,d->cp,symbol_set,len,cp,cSTRING_CONTENT_ONLY|cSTRING_STRICT_SIZE,&len)) )
          goto exit;
        symbol_set = ptr;
      }

      p = d->buff + nIndex;
      for(nIndex=0 ; nIndex<l; p+=nDir*step,nIndex+=step ) {
        for( j=0,c=symbol_set; j<len; j+=step,c+=step ) {
          if ( PR_SUCC(_cmp(p,c,d->cp,step,flags)) ) {
            ret_val = MR((i+b+nDir*nIndex)/step,1);
            goto exit;
          }
        }
      }
    }
  }

exit:
  if ( ptr && (ptr != local_set) )
    CALL_SYS_ObjHeapFree( _this, ptr );
  if ( result )
    *result = ret_val;
  PR_TRACE_A2( _this, "Leave String::FindOneOfBuff method, ret tSTR_RANGE = %p, %terr", ret_val, error );
  return error;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, FindOneNotOfBuff )
// Extended method comment
//    Finds one of symbols belonged to "symbol_set"
// Parameters are:
//   "symbol_set" : symbol set to find one of
//   "size"       : size of symbol set
//   "cp"         : code page of the symbol set
//   "flags"      : how to find, must be one of:
//                    fSTRING_FORWARD
//                    fSTRING_BACKWARD
//
tERROR pr_call String_FindOneNotOfBuff( hi_String _this, tSTR_RANGE* result, tSTR_RANGE p_range, tPTR p_symbol_set, tDWORD p_size, tCODEPAGE p_cp, tDWORD p_flags )
{
  tERROR error = errOK ;
  tSTR_RANGE  ret_val = cSTRING_EMPTY ;
  tPTR        ptr = 0 ;
  tUINT       b, l ;
  StringData* d = _this->data ;
	tBYTE       local_set[0x40];

  PR_TRACE_A0(_this, "Enter String::FindOneNotOfBuff method");

  RANGE(b, l, p_range, error=errSTR_OUT_OF_RANGE) ;

  if ( PR_FAIL(error) )
		;

  else if (!l)
    error = errNOT_FOUND ;

  else if (!p_symbol_set)
    error = errPARAMETER_INVALID ;

  else {
    tDWORD i, j ;
    MAP_CP(p_cp) ;

    switch ( p_cp ) {
			case cCP_OEM:
			case cCP_ANSI:
			case cCP_UNICODE:
				break ;
			case cCP_IBM:
			case cCP_UTF8:
			case cCP_UNICODE_BIGENDIAN:
			default:
				error = errCODEPAGE_NOT_SUPPORTED ;
				break ;
    }

    if ( PR_FAIL(error) )
      ;
    else if (PR_FAIL(error=i_CalcImportLen(&p_symbol_set,p_cp,cSTRING_Z,p_size,&p_size)))
      ;
    else if (!p_size)
      error = errPARAMETER_INVALID ;

    else {
      tCHAR* p;
      tCHAR* c;
      tINT nDir, step;
      tUINT nIndex;

      if (d->cp == cCP_UNICODE)
        step = sizeof(tWCHAR);
      else
        step = sizeof(tCHAR);

      if ( p_flags & fSTRING_FIND_BACKWARD ) {
        nDir = -1 ;
        i = l - step;
        nIndex = b + i;
      }
      else {
        nDir = 1 ;
        i = 0 ;
        nIndex = b;
      }

      if ( d->cp != p_cp ) {
				tDWORD local_size = 0;
				if ( PR_FAIL(error=CalcExportLen(p_symbol_set,p_size,p_cp,d->cp,0,&local_size)) )
					goto exit;
				if ( local_size <= sizeof(local_set) ) {
					local_size = sizeof(local_set);
					ptr = local_set;
				}
        else if (PR_FAIL(error=CALL_SYS_ObjHeapAlloc(_this,&ptr,p_size)))
          goto exit ;
        if (PR_FAIL(error=CopyTo(ptr,local_size,d->cp,p_symbol_set,p_size,p_cp,cSTRING_CONTENT_ONLY|cSTRING_STRICT_SIZE,&p_size)))
          goto exit ;
        p_symbol_set = ptr;
      }


      p = d->buff + nIndex;
      for ( nIndex = 0 ; nIndex < l; p += nDir*step, nIndex += step ) {
        for ( j=0, c=p_symbol_set; j < p_size; j += step, c+=step ) {
          if ( PR_SUCC(_cmp(p,c,d->cp,step,p_flags)) )
            break;
        }
        if ( j == p_size ) {
          ret_val = MR((i + b + nDir*nIndex) / step, 1) ;
          break;
        }
      }
    }
  }

exit:
  if ( ptr && (ptr != local_set) )
    CALL_SYS_ObjHeapFree( _this, ptr );
  if ( result )
    *result = ret_val;

  PR_TRACE_A2(_this, "Leave String::FindOneNotOfBuff method, ret tSTR_RANGE = %p, %terr", ret_val, error) ;

  return error ;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, Format )
// Parameters are:
tERROR pr_call String_Format( hi_String _this, tCODEPAGE p_format_cp, const tVOID* p_format_str, ... ) {
	va_list argptr;
	tERROR error;
	PR_TRACE_A0( _this, "Enter String::Format method" );

	va_start( argptr, p_format_str );
	error = String_VFormat( _this, p_format_cp, p_format_str, VA_LIST_ADDR(argptr) );
	va_end( argptr );

  PR_TRACE_A1( _this, "Leave String::Format method, ret %terr", error );
  return error;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, VFormat )
// Parameters are:
tERROR pr_call String_VFormat( hi_String _this, tCODEPAGE p_format_cp, const tVOID* p_format_str, tVOID* p_args ) {
  tDWORD size;
  StringData* d;
  tERROR error = errOK;
  tPTR ansi_format = 0;
  PR_TRACE_A0( _this, "Enter String::VFormat method" );

  d = _this->data;
  if ( !KNOWN_CP(p_format_cp) )
    error = errCODEPAGE_UNKNOWN;
  else if ( p_format_cp == cCP_UNICODE ) {
    error = CalcExportLen( (tPTR)p_format_str, 0, cCP_UNICODE, cCP_ANSI, cSTRING_Z, &size );
    if ( PR_SUCC(error) )
      error = CALL_SYS_ObjHeapAlloc( _this, &ansi_format, size );
    if ( PR_SUCC(error) )
      error = CopyTo( ansi_format, size, cCP_ANSI, (tPTR)p_format_str, 0, cCP_UNICODE, cSTRING_Z, 0 );
    if ( PR_SUCC(error) ) {
      p_format_str = ansi_format;
      p_format_cp = cCP_ANSI;
    }
  }
	
  if ( PR_SUCC(error) ) {
    tCODEPAGE my_cp = d->cp;
    d->cp = p_format_cp;
    size = pr_vsprintf( d->buff, d->blen, p_format_str, p_args ) + sizeof(tCHAR);
    if ( (size > d->blen) && PR_SUCC(error=CatchMem(_this,size)) )
      size = pr_vsprintf( d->buff, d->blen, p_format_str, p_args ) + sizeof(tCHAR);
		if ( size > d->blen )
			size = d->blen;
		if ( size )
			size -= sizeof(tCHAR);
		d->len = size;
    if ( PR_SUCC(error) && (my_cp != p_format_cp) )
      error = String_SetCP( _this, my_cp );
  }
	
	if ( ansi_format )
		CALL_SYS_ObjHeapFree( _this, ansi_format );

  PR_TRACE_A1( _this, "Leave String::VFormat method, ret %terr", error );
  return error;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, Symbol )
// --- Interface "String". Method "Symbol"
// Extended method comment
//   Returns a symbol from inside the string indicated by begining of range and index in it
// Parameter "range":
//   range to return a char
// Parameter "index":
//   Index of the symbol inside the range.
// Parameter "cp":
//   code page of the symbol to return
tERROR pr_call String_Symbol( hi_String _this, tSYMBOL* result, tSTR_RANGE range, tDWORD index, tCODEPAGE cp, tDWORD flags ) {
	tERROR  error = errOK;
	PR_TRACE_A0( _this, "Enter String::Symbol method" );

	if ( !result )
		error = errPARAMETER_INVALID;
	else {
		tUINT   b, l;
		StringData* d = _this->data;
		MAP_CP( cp );

		if ( (0 == range) || (cSTRING_WHOLE == range) )
			range = STR_RANGE(index,1);
		else
			range = STR_RANGE( STR_RANGE_POS(range)+index, 1 );

		RANGE( b, l, range, error=errSTR_OUT_OF_RANGE;goto exit );

		if (!l) {
			error = errSTR_OUT_OF_RANGE;
			goto exit;
		}

		if ( d->cp == cp ) {
			if ( cp == cCP_UNICODE )
				*result = *UNI(d->buff+b);
			else
				*result = *MB(d->buff+b);
		}
		else {
			*result = 0;
			error = CopyTo( result, sizeof(tSYMBOL), cp, d->buff+b, (d->cp == cCP_UNICODE) ? sizeof(tWCHAR) : sizeof(tCHAR), d->cp, cSTRING_CONTENT_ONLY|cSTRING_STRICT_SIZE, 0 );
		}
	}

	if	( PR_SUCC(error) && flags )
		error = PrStringChCase ( (tBYTE*)result, sizeof(tSYMBOL), cp, (tBYTE*)result, flags );

	exit:
	PR_TRACE_A2( _this, "Leave String::Symbol method, ret tSYMBOL = 0x%x, %terr", (tSYMBOL)(result ? *result : 0), error );
	return error;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, Next )
// --- Interface "String". Method "Next"
// Extended method comment
//   Advance the range to next symbol and then return it.
// Behaviour comment
//
// Result comment
//   If there is no next symbol returns cSTRING_EMPTY
// Parameter "range":
//   range to advance
// Parameter "cp":
//   code page of the symbol to return
tERROR pr_call String_Next( hi_String _this, tSYMBOL* result, tSTR_RANGE* range, tCODEPAGE cp ) {

  tERROR  error;
  tSYMBOL sym;
  PR_TRACE_A0( _this, "Enter String::Next method" );

  if ( !result )
    result = &sym;

  if ( !range ) {
    error = errPARAMETER_INVALID;
    *result = 0;
  }
  else {

    tUINT       b, l;
    StringData* d = _this->data;
    tDWORD      r = MR( BOR(*range)+1, 1 );

    MAP_CP( cp );
    RANGE( b, l, r, error=errSTR_OUT_OF_RANGE; *range=cSTRING_EMPTY; *result=0; goto exit );

    *range = r;

		if ( !l ) {
			error = errSTR_OUT_OF_RANGE;
			*result = 0;
		}

    else if ( cp == d->cp ) {
      if ( cp == cCP_UNICODE )
        *result = *UNI(d->buff + b);
      else
        *result = *MB(d->buff + b);
      error = errOK;
    }
    else {
      *result = 0;
      error = CopyTo( result, sizeof(tSYMBOL), cp, d->buff+b, l, d->cp, cSTRING_CONTENT_ONLY|cSTRING_STRICT_SIZE, 0 );
    }
  }

exit:;
  PR_TRACE_A2( _this, "Leave String::Next method, ret tSYMBOL = 0x%x, %terr", (tSYMBOL)(result ? *result : 0), error );
  return error;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, NextEx )
// --- Interface "String". Method "NextEx"
// Extended method comment
//   Return the symbol and then advance the range to next symbol.
// Behaviour comment
//
// Result comment
//   If there is no next symbol returns cSTRING_EMPTY
// Parameter "range":
//   range to advance
// Parameter "cp":
//   code page of the symbol to return
tERROR pr_call String_NextEx( hi_String _this, tSYMBOL* result, tSTR_RANGE* range, tCODEPAGE cp ) {
  tERROR  error = errOK;
  PR_TRACE_A0( _this, "Enter String::NextEx method" );

  if ( !range ) {
    error = errPARAMETER_INVALID;
    if ( result )
      *result = 0;
  }
  else {
    tUINT       b, l;
    StringData* d = _this->data;
    tSTR_RANGE  r = MR( BOR(*range), 1 );

    MAP_CP( cp );
    RANGE( b, l, r, error=errSTR_OUT_OF_RANGE; *range=cSTRING_EMPTY; if (result) *result=0 );

		if ( !l ) {
			if ( result )
				*result = 0;
		}
    else if ( !result )
      error = errOK;
    else if ( cp == d->cp ) {
      error = errOK;
      if ( cp == cCP_UNICODE )
        *result = *UNI(d->buff + b);
      else
        *result = *MB(d->buff + b);
    }
    else {
      *result = 0;
      error = CopyTo( result, sizeof(tSYMBOL), cp, d->buff+b, l, d->cp, cSTRING_CONTENT_ONLY|cSTRING_STRICT_SIZE, 0 );
    }

    b = BOR(*range) + 1;
    if ( b >= d->len )
      *range = cSTRING_EMPTY;
    else
      *range = MR(b,1);
  }

  PR_TRACE_A2( _this, "Leave String::NextEx method, ret tSYMBOL = 0x%x, %terr", (tSYMBOL)(result ? *result : 0), error );
  return error;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, Prev )
// --- Interface "String". Method "Prev"
// Extended method comment
//   Moves range back to the previous symbol and then return it
// Behaviour comment
//
// Result comment
//   If there is no previous symbol returns cSTRING_EMPTY
// Parameter "range":
//   Range to move back
// Parameter "cp":
//   code page of the symbol to return
tERROR pr_call String_Prev( hi_String _this, tSYMBOL* result, tSTR_RANGE* range, tCODEPAGE cp ) {

  tERROR  error = errOK;
  PR_TRACE_A0( _this, "Enter String::Prev method" );

  if ( !range ) {
    error = errPARAMETER_INVALID;
    if ( result )
      *result = 0;
  }
  else {

    tUINT       b, l;
    StringData* d = _this->data;
    tSTR_RANGE  r = MR( BOR(*range), 1 );

    MAP_CP( cp );
    RANGE( b, l, r, error=errSTR_OUT_OF_RANGE; *range=cSTRING_EMPTY; if (result) *result=0; goto exit );

    if ( b == 0 ) {
      error = errSTR_OUT_OF_RANGE;
      *range = cSTRING_EMPTY;
      if ( result )
        *result = 0;
    }
    else {
      *range = MR( BOR(*range)-1, 1 );
      RANGE( b, l, *range, ; );
      if ( !result )
        error = errOK;
      else if ( cp == d->cp ) {
        error = errOK;
        if ( cp == cCP_UNICODE )
          *result = *UNI(d->buff + b);
        else
          *result = *MB(d->buff + b);
      }
      else {
        *result = 0;
        error = CopyTo( result, sizeof(tSYMBOL), cp, d->buff+b, l, d->cp, cSTRING_CONTENT_ONLY|cSTRING_STRICT_SIZE, 0 );
      }
    }
  }
exit:;
  PR_TRACE_A2( _this, "Leave String::Prev method, ret tSYMBOL = 0x%x, %terr", (tSYMBOL)(result ? *result : 0), error );
  return error;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, PrevEx )
// --- Interface "String". Method "PrevEx"
// Extended method comment
//   Returns the symbol and then moves range back
// Behaviour comment
//
// Result comment
//   If there is no previous symbol returns cSTRING_EMPTY
// Parameter "range":
//   Range to move back
// Parameter "cp":
//   code page of the symbol to return
tERROR pr_call String_PrevEx( hi_String _this, tSYMBOL* result, tSTR_RANGE* range, tCODEPAGE cp ) {

  tERROR  error = errOK;
  PR_TRACE_A0( _this, "Enter String::PrevEx method" );

  if ( !range ) {
    error = errPARAMETER_INVALID;
    if ( result )
      *result = 0;
  }
  else {

    tUINT       b, l;
    StringData* d = _this->data;
    tSTR_RANGE  r = MR( BOR(*range), 1 );

    MAP_CP( cp );
    RANGE( b, l, r, error = errSTR_OUT_OF_RANGE; *range = cSTRING_EMPTY; if (result) *result = 0; goto exit; );

    if ( !result )
      error = errOK;
    else if ( cp == d->cp ) {
      error = errOK;
      if ( cp == cCP_UNICODE )
        *result = *UNI(d->buff + b);
      else
        *result = *MB(d->buff + b);
    }
    else {
      *result = 0;
      error = CopyTo( result, sizeof(tSYMBOL), cp, d->buff+b, l, d->cp, cSTRING_CONTENT_ONLY|cSTRING_STRICT_SIZE, 0 );
    }

    if ( b )
      *range = MR( BOR(*range)-1, 1 );
    else
      *range = cSTRING_EMPTY;
  }

exit:;
  PR_TRACE_A2( _this, "Leave String::PrevEx method, ret tSYMBOL = 0x%x, %terr", (tSYMBOL)(result ? *result : 0), error );
  return error;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, Left )
// --- Interface "String". Method "Left"
// Extended method comment
//   Returns range for "count" left symbols of the string
// Parameter "count":
//   count symbols in the result range
tERROR pr_call String_Left( hi_String _this, tSTR_RANGE* result, tSTR_RANGE range, tDWORD count ) {
  tUINT       b, l;
  StringData* d = _this->data;
  tERROR      e = errOK;
  PR_TRACE_A0( _this, "Enter String::Left method" );

  RANGE( b, l, range, e=errSTR_OUT_OF_RANGE );

  if ( PR_FAIL(e) )
    ;
	else if ( !l )
		e = errSTR_OUT_OF_RANGE;
  else if ( !result )
    ;
  else if ( d->cp == cCP_UNICODE ) {
    if ( (b+l+(count*sizeof(tWCHAR))) > d->len )
      count = (d->len - b - l) / sizeof(tWCHAR);
    *result = MR( b/sizeof(tWCHAR), l/sizeof(tWCHAR)+count );
  }
  else {
    if ( (b+l+count) > d->len )
      count = d->len - b - l;
    *result = MR( b, l+count );
  }

  PR_TRACE_A1( _this, "Leave String::Left method, ret %terr", e );
  return e;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, Mid )
// --- Interface "String". Method "Mid"
// Extended method comment
//   Returns range for "count" symbols begins with "begin"
// Parameter "begin":
//   beginning of the range
// Parameter "count":
//   count symbols in the result range
tERROR pr_call String_Mid( hi_String _this, tSTR_RANGE* result, tSTR_RANGE range, tDWORD begin, tDWORD count ) {
  tSTR_RANGE ret_val;
  StringData* d = _this->data;
  tUINT       b, l;
  tERROR      e = errOK;
  PR_TRACE_A0( _this, "Enter String::Mid method" );

  RANGE( b, l, range, e=errSTR_OUT_OF_RANGE );

  if ( PR_FAIL(e) )
    ret_val = cSTRING_EMPTY;

	else if ( !l )
		e = errSTR_OUT_OF_RANGE;

  else {
    if ( d->cp == cCP_UNICODE ) {
      begin *= sizeof(tWCHAR);
      count *= sizeof(tWCHAR);
    }

    if ( b+begin >= l )
      ret_val = cSTRING_EMPTY;

    else {
      if ( (begin + count) > l )
        count = l - begin;
      if ( d->cp == cCP_UNICODE )
        ret_val = MR( (b+begin)/sizeof(tWCHAR), count/sizeof(tWCHAR) );
      else
        ret_val = MR( b+begin, count );
    }
  }

  if ( result )
    *result = ret_val;
  PR_TRACE_A2( _this, "Leave String::Mid method, ret tSTR_RANGE = 0x%x, %terr", ret_val, e );
  return e;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, Right )
// --- Interface "String". Method "Right"
// Extended method comment
//   Returns range for "count" right symbols of the string
// Parameter "count":
//   count symbols in the result range
tERROR pr_call String_Right( hi_String _this, tSTR_RANGE* result, tSTR_RANGE range, tDWORD count ) {
  tERROR      e = errOK;
  tUINT       b, l;
  StringData* d = _this->data;
  PR_TRACE_A0( _this, "Enter String::Right method" );

  RANGE( b, l, range, e=errSTR_OUT_OF_RANGE );
  if ( PR_FAIL(e) )
    ;
	else if ( !l )
		e = errSTR_OUT_OF_RANGE;
  else if ( !result )
    ;
  else {
    if ( d->cp == cCP_UNICODE ) {
      b /= sizeof(tWCHAR);
      l /= sizeof(tWCHAR);
    }
    if ( count > l )
      count = l;
    if ( (0==range) || (cSTRING_WHOLE==range) )
      *result = MR( -1, count );
    else
      *result = MR( b+l-count, count );
  }

  PR_TRACE_A1( _this, "Leave String::Right method, ret %terr", e );
  return e;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, ChangeCase )
// --- Interface "String". Method "ChangeCase"
// Extended method comment
//   Converts whole string to given case
// Parameter "case_code":
//   case identifier to convert to
tERROR pr_call String_ChangeCase( hi_String _this, tSTR_CASE case_code ) {

  tERROR      error = errUNEXPECTED;
  StringData* d;

  PR_TRACE_A0( _this, "Enter String::ChangeCase method" );

  d = _this->data;

  error = PrStringChCase ( d->buff, d->len, d->cp, d->buff, case_code );

  PR_TRACE_A1( _this, "Leave String::ChangeCase method, ret %terr", error );
  return error;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, ChangeCaseEx )
// --- Interface "String". Method "ChangeCaseEx"
// Extended method comment
//   Converts substring to given case.
// Parameter "case_code":
//   case identifier to convert to
tERROR pr_call String_ChangeCaseEx( hi_String _this, tSTR_RANGE range, tSTR_CASE case_code ) {
  tERROR error = errNOT_IMPLEMENTED;
  PR_TRACE_A0( _this, "Enter String::ChangeCaseEx method" );

  // Place your code here

  PR_TRACE_A1( _this, "Leave String::ChangeCaseEx method, ret %terr", error );
  return error;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, Trim )
// --- Interface "String". Method "Trim"
// Extended method comment
//   removes from left/right/both edges of string
//   all symbols belonged to SYMBOL_SET string
//   area depends on FLAGS parameter and must one of
//     fSTRING_LEFT   -- trim from left
//     fSTRING_RIGHT  -- trim from right
//     fSTRING_OUTER  -- trim from both sides
//
// Behaviour comment
//   if "symbol_set" is 0 or empty removes spaces (default behevior)
// Parameter "symbol_set":
//   symbols to trim. If zero trim spaces
// Parameter "flags":
//   what to trim. Must one of
//     fSTRING_LEFT   -- trim from left
//     fSTRING_RIGHT  -- trim from right
//     fSTRING_OUTER  -- trim from both sides
//
tERROR pr_call String_Trim( hi_String _this, hSTRING symbol_set, tDWORD flags ) {
  StringData* d;
  tERROR      error = errOK;
  PR_TRACE_A0( _this, "Enter String::Trim method" );

  d = _this->data;
  if ( d->len ) {
    tDWORD len;
    tCHAR  buff[20];
    tPTR   symb;
    error = CALL_String_LengthEx( (hi_String)symbol_set, &len, cSTRING_WHOLE, d->cp, cSTRING_Z );
    if ( PR_SUCC(error) ) {
      if ( len <= sizeof(buff) ) {
        symb = buff;
        len = sizeof(buff);
      }
      else
        error = CALL_SYS_ObjHeapAlloc( _this, &symb, len );
      if ( PR_SUCC(error) && PR_SUCC(error=CALL_String_ExportToBuff((hi_String)symbol_set,&len,cSTRING_WHOLE,symb,len,d->cp,cSTRING_Z)) )
        error = String_TrimBuff( _this, symb, len, d->cp, flags );
      if ( symb != buff )
        CALL_SYS_ObjHeapFree( _this, symb );
    }
  }

  PR_TRACE_A1( _this, "Leave String::Trim method, ret %terr", error );
  return error;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, TrimBuff )
// --- Interface "String". Method "TrimBuff"
// Extended method comment
//   removes from left/right/both edges of string
//   all symbols belonged to "symbol_set"
//   what to trim depends on "flags" parameter and must one of
//     fSTRING_LEFT   -- trim from left
//     fSTRING_RIGHT  -- trim from right
//     fSTRING_OUTER  -- trim from both sides
//
// Behaviour comment
//   if "symbol_set" is 0 or empty removes spaces (default behevior)
// Parameter "symbol_set":
//   symbols to trim
// Parameter "cp":
//   code page of the "symbol_set"
// Parameter "flags":
//   what to trim. Must one of
//     fSTRING_LEFT   -- trim from left
//     fSTRING_RIGHT  -- trim from right
//     fSTRING_OUTER  -- trim from both sides
//
tERROR pr_call String_TrimBuff( hi_String _this, tPTR symbol_set, tDWORD size, tCODEPAGE cp, tDWORD flags ) {
  tERROR      error = errOK;
  StringData* d;
  PR_TRACE_A0( _this, "Enter String::TrimBuff method" );

  d = _this->data;
  if ( d->len ) {
    tDWORD  len = 0;
    tCHAR   buff[20];
    tPTR    symb = 0;
    tCHAR*  cptr;
    tWCHAR* wptr;

    MAP_CP( cp );

    if ( cp == d->cp )
      symb = symbol_set;
    else {
      error = CalcExportLen( symbol_set, size, cp, d->cp, cSTRING_Z, &len );
      if ( PR_SUCC(error) ) {
        if ( len <= sizeof(buff) )
          symb = buff;
        else
          error = CALL_SYS_ObjHeapAlloc( _this, &symb, len );
        if ( PR_SUCC(error) )
          error = CopyTo( symb, len, d->cp, symbol_set, size, cp, cSTRING_Z, &len );
      }
    }

    if ( PR_SUCC(error) ) {
      if ( flags & fSTRING_RIGHT ) {
        if ( d->cp == cCP_UNICODE ) {
          wptr = UNI(d->buff + d->len - 1 * sizeof(tWCHAR));
          while( d->len ) {
            if ( wcschr(UNI(symb),*wptr) ) {
              d->len -= sizeof(tWCHAR);
              *wptr = 0;
              wptr--;
            }
            else
              break;
          }
        }
        else {
          cptr = d->buff + d->len - 1;
          while( d->len ) {
            if ( strchr(MB(symb),*cptr) ) {
              d->len--;
              *cptr = 0;
              cptr--;
            }
            else
              break;
          }
        }
      }

      if ( d->len && (flags & fSTRING_LEFT) ) {
        tUINT strip = 0;
        if ( d->cp == cCP_UNICODE ) {
          wptr = UNI(d->buff);
          while( strip < d->len ) {
            if ( wcschr(UNI(symb),*wptr) ) {
              strip += sizeof(tWCHAR);
              wptr++;
            }
            else
              break;
          }
        }
        else {
          cptr = d->buff;
          while( strip < d->len ) {
            if ( strchr(MB(symb),*cptr) ) {
              strip++;
              cptr++;
            }
            else
              break;
          }
        }
        if ( strip ) {
          memmove( d->buff, d->buff+strip, d->len-=strip );
          d->buff[d->len] = 0;
          if ( d->cp == cCP_UNICODE )
            d->buff[d->len+1] = 0;
        }
      }
    }

    if ( (symb != buff) && (symb!=symbol_set) )
      CALL_SYS_ObjHeapFree( _this, symb );
  }

  PR_TRACE_A0( _this, "Leave String::TrimBuff method, ret tERROR = errOK" );
  return errOK;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, Cut )
// --- Interface "String". Method "Cut"
// Extended method comment
//   removes/leaves substring pointed by range
//   cutting area depends on "falgs" parameter and must one of
//     fSTRING_INNER  -- cut inner space
//     fSTRING_OUTER  -- cut edges
//
// Parameter "range":
//   range to cut
// Parameter "flags":
//   what to cat. Must one of
//     fSTRING_INNER  -- cut inner space
//     fSTRING_OUTER  -- cut edges
//
tERROR pr_call String_Cut( hi_String _this, tSTR_RANGE range, tDWORD flags ) {
  tERROR error;
  tDWORD  ret_val = 0;
  StringData* d;
  tUINT b, l, dst/*destination offset*/, src/*src offset*/, len/*copy length*/;

  PR_TRACE_A0( _this, "Enter String::Cut method" );

  d = _this->data;
  RANGE( b, l, range, error=errSTR_OUT_OF_RANGE; goto exit );

	error = errOK;
	switch( flags & fSTRING_OUTER ) {
		case 0             :
			dst = b;
			src = b+l;

			if (d->len > b + l)
				len = d->len - b - l;
			else
				len = 0;
			d->len -= l;
			break; // cut the range
		case fSTRING_LEFT  : dst = 0;     src = b;      len = d->len - b; d->len -= b;   break; // cut all symbols at the left of the range
		case fSTRING_RIGHT : dst = b + l; src = d->len; len = 0;          d->len  = b+l; break; // cut all symbols at the right of the range
		case fSTRING_OUTER : dst = 0;     src = b;      len = l;          d->len  = l;   break; // cut all symbols out of the range
	}

	if ( len && (dst != src) )
		memmove( d->buff+dst, d->buff+src, len );

exit:
  PR_TRACE_A1( _this, "Leave String::Cut method, ret %terr", error );
  return error;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, Replace )
// --- Interface "String". Method "Replace"
// Extended method comment
//   Replaces substring with the new value
// Behaviour comment
//   If "p_repl" is 0 or empty just collapses the string
// Parameter "range":
//   what to replace
// Parameter "p_repl":
//   string to be inserted instead original substring
//   If it is 0 or empty just collapses the substring
// Parameter "p_repl_range":
//   range of source string to be inserted instead original
tERROR pr_call String_Replace( hi_String _this, tSTR_RANGE p_range, hSTRING p_repl, tSTR_RANGE p_repl_range, tSTR_RANGE* p_out_range ) {
  tERROR err = errOK;
	const tVOID* p = 0;
	tUINT        b, l = 0;
	tCODEPAGE    cp;
	tBOOL  free_it = cTRUE;
  PR_TRACE_A0( _this, "Enter String::Replace method" );

	if ( p_out_range )
		*p_out_range = cSTRING_EMPTY;

	if ( !p_repl || (p_repl_range == cSTRING_EMPTY) )
		;
	else if ( PID_STRING == CALL_SYS_PropertyGetDWord(p_repl,pgPLUGIN_ID) ) {
		StringData* d = ((hi_String)p_repl)->data;
		RANGE( b, l, p_repl_range, ; );
		p = d->buff + b;
		cp = d->cp;
		free_it = cFALSE;
	}
	else if ( PR_FAIL(err=CALL_String_LengthEx((hi_String)p_repl,&l,p_repl_range,_this->data->cp,0)) )
		;
	else if ( !l )
		;
	else if ( PR_FAIL(err=CALL_SYS_ObjHeapAlloc((hi_String)p_repl,(tVOID**)&p,l)) )
		;
	else
		err = CALL_String_ExportToBuff( (hi_String)p_repl, &l, p_repl_range, (tVOID*)p, l, cp=_this->data->cp, cSTRING_Z );

	if ( PR_FAIL(err) )
		;
	else if ( l )
		err = String_ReplaceBuff( _this, p_range, (tVOID*)p, l, cp, p_out_range );
	else if ( PR_FAIL(err=String_Cut(_this,p_range,fSTRING_INNER)) )
		;
	else if ( p_out_range )
		*p_out_range = p_range | cSTRING_WHOLE_LENGTH;

	if ( free_it && p )
		CALL_SYS_ObjHeapFree( p_repl, (tVOID*)p );

  PR_TRACE_A1( _this, "Leave String::Replace method, ret %terr", err );
  return err;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, ReplaceBuff )
// --- Interface "String". Method "ReplaceBuff"
// Extended method comment
//   Replaces substring with the new value
// Behaviour comment
//   If "other" is 0 or empty just collapses the string
// Parameter "range":
//   range to be replaced
// Parameter "buffer":
//   buffer to replace original substring. If it is 0 or empty just collapses the substring
// Parameter "size":
//   size of buffer to replace
// Parameter "cp":
//   code page of buffer to replace
tERROR pr_call String_ReplaceBuff( hi_String _this, tSTR_RANGE p_range, tPTR p_buffer, tDWORD p_size, tCODEPAGE p_cp, tSTR_RANGE* p_out_range ) {
  tERROR err = errOK;
	tUINT b, l;
	StringData*  d = _this->data;
  PR_TRACE_A0( _this, "Enter String::ReplaceBuff method" );

	err = String_Cut( _this, p_range, fSTRING_INNER );
	if ( PR_SUCC(err) )
		err = i_CalcImportLen( &p_buffer, p_cp, cSTRING_Z, p_size, &p_size );
	if ( PR_SUCC(err) && p_size ) {
		tVOID* pmem;
		
		MAP_CP( p_cp );
		if ( d->cp == p_cp )
			pmem = p_buffer;
		else {
			tUINT lmem;
			err = CalcExportLen( p_buffer, p_size, p_cp, d->cp, 0, &lmem );
			if ( PR_SUCC(err) )
				err = CALL_SYS_ObjHeapAlloc( _this, &pmem, lmem );
			if ( PR_SUCC(err) )
				err = CopyTo( pmem, lmem, d->cp, p_buffer, p_size, p_cp, 0, &p_size );
		}

		if ( PR_SUCC(err) )
			err = CatchMem( _this, d->len + p_size );

		d = _this->data;
		if ( PR_SUCC(err) ) {
			tUINT rest;
			RANGE( b, l, p_range, b=d->len );
			if ( b < d->len )
				rest = d->len - b;
			else
				rest = 0;
			if ( rest )
				memmove( d->buff+b+p_size, d->buff+b, rest );
			memmove( d->buff+b, pmem, p_size );
			d->len += p_size;
		}

		if ( pmem != p_buffer )
			CALL_SYS_ObjHeapFree( _this, pmem );
	}

	if ( p_out_range ) {
		if ( PR_FAIL(err) )
			*p_out_range = cSTRING_EMPTY;
		else {
			b = BOR(p_range);
			if ( p_size && (d->cp == cCP_UNICODE) )
				p_size /= sizeof(tWCHAR);
			b += p_size; 
			if ( b >= d->len )
				*p_out_range = cSTRING_EMPTY;
			else
				*p_out_range = MR(b,cSTRING_WHOLE_LENGTH);
		}
	}

  PR_TRACE_A1( _this, "Leave String::ReplaceBuff method, ret %terr", err );
  return err;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, Reserve )
// Parameters are:
tERROR pr_call String_Reserve( hi_String _this, tDWORD number, tCODEPAGE cp ) {
  tERROR error;
  PR_TRACE_A0( _this, "Enter String::Reserve method" );

  switch( cp ) {
    case cCP_UNICODE:
      number *= sizeof(tWCHAR);
    case cCP_OEM:
    case cCP_ANSI:
      if ( _this->data->blen >= number )
        error = errOK;
      else
        error = CatchMem( _this, number );
      break;
    case cCP_NULL:
    case cCP_IBM:
    case cCP_UTF8:
    case cCP_UNICODE_BIGENDIAN:
      error = errCODEPAGE_NOT_SUPPORTED;
      break;
    default:
      error = errPARAMETER_INVALID;
      break;
  }

  PR_TRACE_A1( _this, "Leave String::Reserve method, ret %terr", error );
  return error;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, ProveLastSlash )
// Parameters are:
tERROR pr_call String_ProveLastSlash( hi_String _this ) {
  tERROR error = errOK;
  StringData* d;
  PR_TRACE_A0( _this, "Enter String::ProveLastSlash method" );
#if defined (_WIN32)
#define RIGHT_DELIMETER '\\'
#define RIGHT_DELIMETER_WCHAR L'\\'
#define WRONG_DELIMETER '/'
#define WRONG_DELIMETER_WCHAR L'/'
#else
#define RIGHT_DELIMETER '/'
#define RIGHT_DELIMETER_WCHAR L'/'
#define WRONG_DELIMETER '\\'
#define WRONG_DELIMETER_WCHAR L'\\'
#endif
  d = _this->data;
  if ( d->len ) {
    tSYMBOL symb;
    switch( d->cp ) {
      case cCP_UNICODE:
        symb = *UNI(d->buff + d->len - sizeof(tWCHAR));
        if ( symb == WRONG_DELIMETER_WCHAR )
          *UNI(d->buff + d->len - sizeof(tWCHAR)) = RIGHT_DELIMETER_WCHAR;
        else if ( symb != RIGHT_DELIMETER_WCHAR && PR_SUCC(error=CatchMem(_this,d->len+sizeof(tWCHAR))) ) {
          *UNI(d->buff + d->len) = RIGHT_DELIMETER_WCHAR;
          d->len += sizeof(tWCHAR);
        }
        break;
      case cCP_OEM:
      case cCP_ANSI:
        symb = *MB(d->buff + d->len - sizeof(tCHAR));
        if ( symb == WRONG_DELIMETER )
          *MB(d->buff + d->len - sizeof(tCHAR)) = RIGHT_DELIMETER;
        else if ( symb != RIGHT_DELIMETER && PR_SUCC(error=CatchMem(_this,d->len+sizeof(tCHAR))) ) {
          *MB(d->buff + d->len) = RIGHT_DELIMETER;
          d->len += sizeof(tCHAR);
        }
        break;
    }

  }
  PR_TRACE_A1( _this, "Leave String::ProveLastSlash method, ret %terr", error );
  return error;
}
// AVP Prague stamp end



