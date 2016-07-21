#if !defined(__OPTIONS_H__)
#define __OPTIONS_H__

#include <property/property.h>


#define OPTIONS_PID_ID		(3)
#define OPTIONS_PID		MAKE_AVP_PID(OPTIONS_PID_ID, 8, avpt_nothing, 0)
#define OPTIONS_PID_UID1		(1)

 #define LANGUAGE_PID_ID		(2)
 #define LANGUAGE_PID		MAKE_AVP_PID(LANGUAGE_PID_ID, 1, avpt_nothing, 0)
 #define LANGUAGE_PID_UID50		(50)

  #define LANGUAGEID_PID_ID		(1)
  #define LANGUAGEID_PID		MAKE_AVP_PID(LANGUAGEID_PID_ID, 1, avpt_group, 0)
  #define LANGUAGEID_PID_UID51		(51)

 #define FULL_INFO_PID_ID		(8)
 #define FULL_INFO_PID		MAKE_AVP_PID(FULL_INFO_PID_ID, 1, avpt_bool, 0)
 #define FULL_INFO_PID_UID52		(52)

 #define BRICK_STYLE_PID_ID		(100)
 #define BRICK_STYLE_PID		MAKE_AVP_PID(BRICK_STYLE_PID_ID, 1, avpt_bool, 0)
 #define BRICK_STYLE_PID_UID53		(53)




#endif   // !defined(__OPTIONS_H__)
