#if !defined(__OPTIONS_H__)
#define __OPTIONS_H__

#include <property/property.h>


#define OPTIONS_PID_ID		(3)
#define OPTIONS_PID		MAKE_AVP_PID(OPTIONS_PID_ID, 8, avpt_nothing, 0)
#define OPTIONS_PID_UID1		(1)

 #define H___HEADER_FILE_GENERATION_PID_ID		(11)
 #define H___HEADER_FILE_GENERATION_PID		MAKE_AVP_PID(H___HEADER_FILE_GENERATION_PID_ID, 1, avpt_bool, 0)
 #define H___HEADER_FILE_GENERATION_PID_UID41		(41)

 #define C___SOURCE_FILE_GENERATION_PID_ID		(10)
 #define C___SOURCE_FILE_GENERATION_PID		MAKE_AVP_PID(C___SOURCE_FILE_GENERATION_PID_ID, 1, avpt_bool, 0)
 #define C___SOURCE_FILE_GENERATION_PID_UID42		(42)

 #define GENERATE_EXTENDED_COMMENTS_PID_ID		(7)
 #define GENERATE_EXTENDED_COMMENTS_PID		MAKE_AVP_PID(GENERATE_EXTENDED_COMMENTS_PID_ID, 1, avpt_bool, 0)
 #define GENERATE_EXTENDED_COMMENTS_PID_UID29		(29)

 #define OPEN_BRACE_ON_NEXT_LINE_PID_ID		(5)
 #define OPEN_BRACE_ON_NEXT_LINE_PID		MAKE_AVP_PID(OPEN_BRACE_ON_NEXT_LINE_PID_ID, 1, avpt_bool, 0)
 #define OPEN_BRACE_ON_NEXT_LINE_PID_UID30		(30)

 #define REPLACE_TAB_WITH_SPACES_PID_ID		(9)
 #define REPLACE_TAB_WITH_SPACES_PID		MAKE_AVP_PID(REPLACE_TAB_WITH_SPACES_PID_ID, 1, avpt_bool, 0)
 #define REPLACE_TAB_WITH_SPACES_PID_UID39		(39)

  #define INDENT_VALUE_PID_ID		(1)
  #define INDENT_VALUE_PID		MAKE_AVP_PID(INDENT_VALUE_PID_ID, 1, avpt_byte, 0)
  #define INDENT_VALUE_PID_UID40		(40)

 #define MAKE_INTERFACE_DATA_PUBLIC_PID_ID		(1)
 #define MAKE_INTERFACE_DATA_PUBLIC_PID		MAKE_AVP_PID(MAKE_INTERFACE_DATA_PUBLIC_PID_ID, 1, avpt_bool, 0)
 #define MAKE_INTERFACE_DATA_PUBLIC_PID_UID45		(45)

 #define SECTION_DELIMITER_PID_ID		(12)
 #define SECTION_DELIMITER_PID		MAKE_AVP_PID(SECTION_DELIMITER_PID_ID, 1, avpt_nothing, 0)
 #define SECTION_DELIMITER_PID_UID46		(46)

  #define PAGE_BREAK_PID_ID		(1)
  #define PAGE_BREAK_PID		MAKE_AVP_PID(PAGE_BREAK_PID_ID, 1, avpt_bool, 0)
  #define PAGE_BREAK_PID_UID47		(47)

  #define COMMENT_PID_ID		(2)
  #define COMMENT_PID		MAKE_AVP_PID(COMMENT_PID_ID, 1, avpt_bool, 0)
  #define COMMENT_PID_UID48		(48)

  #define LINEBREAKS_NUMBER_PID_ID		(3)
  #define LINEBREAKS_NUMBER_PID		MAKE_AVP_PID(LINEBREAKS_NUMBER_PID_ID, 1, avpt_byte, 0)
  #define LINEBREAKS_NUMBER_PID_UID49		(49)



// ---
class COptions : public CTreeInfo {
  
public:
  COptions( HDATA tree, bool own=false ) : CTreeInfo(tree,own) {}
  COptions( DWORD len, const BYTE* stream ) : CTreeInfo(len,stream) {}
  bool  header()               const;
  bool  source()               const;
  bool  ext_comments()         const;
  bool  brace_next_line()      const;
  bool  tabs()                 const;
  DWORD indent()               const;
  bool  delimiter()            const;
  bool  delimiter_page_break() const;
  bool  delimiter_comment()    const;
  bool  private_structure()    const;
  DWORD linebreaks_number()    const;
  
  virtual HDATA Next()         const  { return 0; }
  virtual bool  GoFirst()             { return false; }
  virtual bool  GoNext()              { return false; }
  virtual bool  GoPrev()              { return false; }
  
private:
  virtual bool  ReInit()              { return true; }
};



#endif   // !defined(__OPTIONS_H__)
