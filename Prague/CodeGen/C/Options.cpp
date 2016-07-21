// Options.cpp: implementation of the COptions class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "codegen.h"
#include "Options.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


// ---
bool COptions::header() const { 
  MAKE_ADDR1( a, H___HEADER_FILE_GENERATION_PID );
  return BoolAttr( AVP_PID_VALUE, a ); 
}



// ---
bool COptions::source() const { 
  MAKE_ADDR1( a, C___SOURCE_FILE_GENERATION_PID );
  return BoolAttr( AVP_PID_VALUE, a ); 
}


// ---
bool COptions::ext_comments() const { 
  MAKE_ADDR1( a, GENERATE_EXTENDED_COMMENTS_PID );
  return BoolAttr( AVP_PID_VALUE, a ); 
}



// ---
bool COptions::brace_next_line() const { 
  MAKE_ADDR1( a, OPEN_BRACE_ON_NEXT_LINE_PID );
  return BoolAttr( AVP_PID_VALUE, a ); 
}


// ---
bool COptions::tabs() const {
  MAKE_ADDR1( a, REPLACE_TAB_WITH_SPACES_PID );
  return !BoolAttr( AVP_PID_VALUE, a ); 
}


// ---
DWORD COptions::indent() const {
  BYTE v; 
  MAKE_ADDR2(a,REPLACE_TAB_WITH_SPACES_PID,INDENT_VALUE_PID); 
  if ( Attr(AVP_PID_VALUE,&v,sizeof(v),a) ) 
    return v; 
  else 
    return 2;
}


// ---
bool COptions::delimiter() const { 
  MAKE_ADDR1(a,SECTION_DELIMITER_PID); 
  return 0 != ::DATA_Find(m_curr,a); 
}


// ---
bool COptions::delimiter_page_break() const { 
  MAKE_ADDR2(a,SECTION_DELIMITER_PID,PAGE_BREAK_PID); 
  return BoolAttr(AVP_PID_VALUE,a); 
}


// ---
bool COptions::delimiter_comment() const {
  MAKE_ADDR2(a,SECTION_DELIMITER_PID,COMMENT_PID); 
  return BoolAttr(AVP_PID_VALUE,a); 
}


// ---
bool COptions::private_structure() const {
  MAKE_ADDR1( a, MAKE_INTERFACE_DATA_PUBLIC_PID );
  return !BoolAttr( AVP_PID_VALUE, a ); 
}

// ---
DWORD  COptions::linebreaks_number() const {
  BYTE v; 
  MAKE_ADDR2(a,SECTION_DELIMITER_PID,LINEBREAKS_NUMBER_PID); 
  if ( Attr(AVP_PID_VALUE,&v,sizeof(v),a) ) 
    return v; 
  else 
    return 2;
}

