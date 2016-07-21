#if !defined(__OPTIONS_H__)
#define __OPTIONS_H__

#include <property/property.h>


#define OPTIONS_PID_ID		(3)
#define OPTIONS_PID		MAKE_AVP_PID(OPTIONS_PID_ID, 8, avpt_nothing, 0)
#define OPTIONS_PID_UID1		(1)

 #define SOME_OPTION_PID_ID		(7)
 #define SOME_OPTION_PID		MAKE_AVP_PID(SOME_OPTION_PID_ID, 1, avpt_bool, 0)
 #define SOME_OPTION_PID_UID29		(29)


// ---
class COptions : public CTreeInfo {
	
public:
  COptions( HDATA tree, bool own=false ) : CTreeInfo(tree,own) {}
  COptions( DWORD len, const BYTE* stream ) : CTreeInfo(len,stream) {}
  bool  some_option() const;
};


// ---
inline bool COptions::some_option() const {
  MAKE_ADDR2( a, OPTIONS_PID, SOME_OPTION_PID );
  return BoolAttr( AVP_PID_VALUE, a ); 
}


#endif   // !defined(__OPTIONS_H__)
