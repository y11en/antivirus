#ifndef __AVPPRPID_H
#define __AVPPRPID_H


#include "Property/Property.h"
#include "AVPComID.h"

//#define PP_APP_ID										(1) Repaced to AVPCommID.h

// AVPPattern IDs and PIDs

// Root ID
#define PP_PID_ROOT_ID							AVP_DC_CONFIG_PATTERN
#define PP_PID_ROOT									MAKE_AVP_PID(PP_PID_ROOT_ID, AVP_APID_GLOBAL, avpt_nothing, 0)

// Root Prop
// Value = version of pattern (4 words)
#define PP_PID_VERSION_ID						(1)
#define PP_PID_VERSION							MAKE_AVP_PID(PP_PID_VERSION_ID,PP_APP_ID,avpt_word,1)

// Pattern data nodes
#define PP_PID_DISPLAY_STR_ID       (1)
#define PP_PID_DISPLAY_STR					MAKE_AVP_PID(PP_PID_DISPLAY_STR_ID,PP_APP_ID,avpt_str,0)

#define PP_PID_EDIT_TYPE_ID         (2)
#define PP_PID_EDIT_TYPE						MAKE_AVP_PID(PP_PID_EDIT_TYPE_ID,PP_APP_ID,avpt_byte,0)

#define PP_PID_MIN_VALUE_ID         (3)

#define PP_PID_MAX_VALUE_ID         (4)

#define PP_PID_FORMAT_STR_IN_ID 		(5)
#define PP_PID_FORMAT_STR_IN        MAKE_AVP_PID(PP_PID_FORMAT_STR_IN_ID,PP_APP_ID,avpt_str,0)

#define PP_PID_FORMAT_STR_OUT_ID    (6)
#define PP_PID_FORMAT_STR_OUT       MAKE_AVP_PID(PP_PID_FORMAT_STR_OUT_ID,PP_APP_ID,avpt_str,0)

#define PP_PID_UPDOWN_STEP_ID       (7)
#define PP_PID_UPDOWN_STEP				  MAKE_AVP_PID(PP_PID_UPDOWN_STEP_ID,PP_APP_ID,avpt_dword,0)

#define PP_PID_LISTCONTENTS_ID			(8)
#define PP_PID_LISTCONTENTS				  MAKE_AVP_PID(PP_PID_LISTCONTENTS_ID,PP_APP_ID,avpt_str,1)

#define PP_PID_COMBOCONTENTS_ID			(9)
#define PP_PID_COMBOCONTENTS				MAKE_AVP_PID(PP_PID_COMBOCONTENTS_ID,PP_APP_ID,avpt_str,1)

#define PP_PID_SUBTYPE_ID						(10)
#define PP_PID_SUBTYPE							MAKE_AVP_PID(PP_PID_SUBTYPE_ID,PP_APP_ID,avpt_byte,0)

#define PP_PID_MNEMONICID_ID				(11)
#define PP_PID_MNEMONICID						MAKE_AVP_PID(PP_PID_MNEMONICID_ID,PP_APP_ID,avpt_str,0)

#define PP_PID_VALUEASINDEX_ID			(12)
#define PP_PID_VALUEASINDEX				  MAKE_AVP_PID(PP_PID_VALUEASINDEX_ID,PP_APP_ID,avpt_bool,0)

#define PP_PID_LASTCATEGORIESLEVEL_ID		(13)
#define PP_PID_LASTCATEGORIESLEVEL			MAKE_AVP_PID(PP_PID_LASTCATEGORIESLEVEL_ID,PP_APP_ID,avpt_nothing,0)

#define PP_PID_STARTRESOURCEID_ID		(14)
#define PP_PID_STARTRESOURCEID			MAKE_AVP_PID(PP_PID_STARTRESOURCEID_ID,PP_APP_ID,avpt_dword,0)

#define PP_PID_DISPSTR_RESID_ID			(15)
#define PP_PID_DISPSTR_RESID				MAKE_AVP_PID(PP_PID_DISPSTR_RESID_ID,PP_APP_ID,avpt_dword,0)

#define PP_PID_STRVAL_RESID_ID			(16)
#define PP_PID_STRVAL_RESID				  MAKE_AVP_PID(PP_PID_STRVAL_RESID_ID,PP_APP_ID,avpt_dword,0)

#define PP_PID_ARRAYCONT_RESID_ID		(17)
#define PP_PID_ARRAYCONT_RESID			MAKE_AVP_PID(PP_PID_ARRAYCONT_RESID_ID,PP_APP_ID,avpt_dword,1)

#define PP_PID_LISTCONT_RESID_ID		(18)
#define PP_PID_LISTCONT_RESID				MAKE_AVP_PID(PP_PID_LISTCONT_RESID_ID,PP_APP_ID,avpt_dword,1)

#define PP_PID_COMBOCONT_RESID_ID		(19)
#define PP_PID_COMBOCONT_RESID			MAKE_AVP_PID(PP_PID_COMBOCONT_RESID_ID,PP_APP_ID,avpt_dword,1)

#define PP_PID_VALUEASPASSWORD_ID		(20)
#define PP_PID_VALUEASPASSWORD			MAKE_AVP_PID(PP_PID_VALUEASPASSWORD_ID,PP_APP_ID,avpt_bool, 0)

#define PP_PID_SCROLLSIZE_ID				(21)
#define PP_PID_SCROLLSIZE						MAKE_AVP_PID(PP_PID_SCROLLSIZE_ID,PP_APP_ID,avpt_dword,0)

#define PP_PID_COMBINELABELANDVALUE_ID	(22)
#define PP_PID_COMBINELABELANDVALUE			MAKE_AVP_PID(PP_PID_COMBINELABELANDVALUE_ID,PP_APP_ID,avpt_bool,0)

#define PP_PID_STARTUNIQUEID_ID			(23)
#define PP_PID_STARTUNIQUEID				MAKE_AVP_PID(PP_PID_STARTUNIQUEID_ID,PP_APP_ID,avpt_dword,0)

#define PP_PID_UNIQUEID_ID					(24)
#define PP_PID_UNIQUEID							MAKE_AVP_PID(PP_PID_UNIQUEID_ID,PP_APP_ID,avpt_dword,0)

#define PP_PID_EXTERNALFORMATTEXT_ID	(25)
#define PP_PID_EXTERNALFORMATTEXT			MAKE_AVP_PID(PP_PID_EXTERNALFORMATTEXT_ID,PP_APP_ID,avpt_bool,0)

#define PP_PID_SHOULDLOCALIZE_ID		(26)
#define PP_PID_SHOULDLOCALIZE				MAKE_AVP_PID(PP_PID_SHOULDLOCALIZE_ID,PP_APP_ID,avpt_bool,0)

#define PP_PID_VALUEASFOLDERNAME_ID		(27)
#define PP_PID_VALUEASFOLDERNAME			MAKE_AVP_PID(PP_PID_VALUEASFOLDERNAME_ID,PP_APP_ID,avpt_bool, 0)

#define PP_PID_VALUEASFILENAME_ID		  (28)
#define PP_PID_VALUEASFILENAME			  MAKE_AVP_PID(PP_PID_VALUEASFILENAME_ID,PP_APP_ID,avpt_bool, 0)

#define PP_PID_FILEDEFEXT_ID				(29)
#define PP_PID_FILEDEFEXT						MAKE_AVP_PID(PP_PID_FILEDEFEXT_ID,PP_APP_ID,avpt_str,0)

#define PP_PID_FILEFILTER_ID				(30)
#define PP_PID_FILEFILTER						MAKE_AVP_PID(PP_PID_FILEFILTER_ID,PP_APP_ID,avpt_str,0)

#define PP_PID_FILEFILTER_RESID_ID		(31)
#define PP_PID_FILEFILTER_RESID				MAKE_AVP_PID(PP_PID_FILEFILTER_RESID_ID,PP_APP_ID,avpt_dword,0)

#define PP_PID_VALUEONLY_ID						(32)
#define PP_PID_VALUEONLY							MAKE_AVP_PID(PP_PID_VALUEONLY_ID,PP_APP_ID,avpt_bool,0)

#define PP_PID_LABELONLY_ID						(33)
#define PP_PID_LABELONLY							MAKE_AVP_PID(PP_PID_LABELONLY_ID,PP_APP_ID,avpt_bool,0)

#define PP_PID_ALLOWEMPTYSTRING_ID		(34)
#define PP_PID_ALLOWEMPTYSTRING				MAKE_AVP_PID(PP_PID_ALLOWEMPTYSTRING_ID,PP_APP_ID,avpt_bool,0)

#define PP_TED_ORDINARY             1
#define PP_TED_UPDOWN               2
#define PP_TED_EDITABLECOMBO        3
#define PP_TED_NONEDITABLECOMBO     4
#define PP_TED_EXTERNAL             5
#define PP_TED_DATEORDINARY         6
#define PP_TED_NONEDITABLE          7
#define PP_TED_NOTVISIBLE           8
#define PP_TED_PASSWORD             9
#define PP_TED_IPADDRESS            10
#define PP_TED_EXCLUSIVEEXTERNAL		11
#define PP_TED_TIMEORDINARY         12
#define PP_TED_DATETIMEORDINARY     13

#define PP_TYPE_NOTHING             avpt_nothing
#define PP_TYPE_CHAR                avpt_char
#define PP_TYPE_WCHAR               avpt_wchar
#define PP_TYPE_SHORT               avpt_short
#define PP_TYPE_LONG                avpt_long
#define PP_TYPE_BYTE                avpt_byte
#define PP_TYPE_WORD                avpt_word
#define PP_TYPE_DWORD               avpt_dword
#define PP_TYPE_INT									avpt_int
#define PP_TYPE_UINT								avpt_uint
#define PP_TYPE_BOOL                avpt_bool
#define PP_TYPE_DATE                avpt_date
#define PP_TYPE_TIME                avpt_time
#define PP_TYPE_DATETIME            avpt_datetime
#define PP_TYPE_STRING              avpt_str
#define PP_TYPE_USTRING             avpt_wstr
#define PP_TYPE_BINARY              avpt_bin
#define PP_TYPE_CHECKBTN            avpt_bool
#define PP_TYPE_RADIOBTN            avpt_bool
#define PP_TYPE_RADIOGROUP          avpt_group
#define PP_TYPE_IPADDRESS           avpt_dword
//#define PP_TYPE_LISTGROUP           avpt_nothing

#define PP_SYBTYPE_NOTHING          0
#define PP_SYBTYPE_CHECKBTN         1
#define PP_SYBTYPE_RADIOBTN         2
#define PP_SYBTYPE_IPADDRESS        3

#define PP_PID_ENABLE_SHIFT         50

#define PP_PID_NODEEXPANDED_ID			(100)
#define PP_PID_NODEEXPANDED					MAKE_AVP_PID(PP_PID_NODEEXPANDED_ID,PP_APP_ID,avpt_bool,0)

#define PP_PID_NODESELECTED_ID			(101)
#define PP_PID_NODESELECTED					MAKE_AVP_PID(PP_PID_NODESELECTED_ID,PP_APP_ID,avpt_bool,0)

#define PP_PID_NODEGROUPSELECTEDIND_ID	(102)
#define PP_PID_NODEGROUPSELECTEDIND			MAKE_AVP_PID(PP_PID_NODEGROUPSELECTEDIND_ID,PP_APP_ID,avpt_dword,0)

#define PP_PID_NODEFIRSTVISIBLE_ID			(103)
#define PP_PID_NODEFIRSTVISIBLE					MAKE_AVP_PID(PP_PID_NODEFIRSTVISIBLE_ID,PP_APP_ID,avpt_bool,0)

#define PP_PID_NODEGROUPFIRSTVISIBLEIND_ID	(104)
#define PP_PID_NODEGROUPFIRSTVISIBLEIND			MAKE_AVP_PID(PP_PID_NODEGROUPFIRSTVISIBLEIND_ID,PP_APP_ID,avpt_dword,0)

#define PP_PID_SCROLLPOS_ID							(105)
#define PP_PID_SCROLLPOS								MAKE_AVP_PID(PP_PID_SCROLLPOS_ID,PP_APP_ID,avpt_dword,0)

#define PP_PID_CURRINDEX_ID							(106)
#define PP_PID_CURRINDEX								MAKE_AVP_PID(PP_PID_CURRINDEX_ID,PP_APP_ID,avpt_dword,0)

#define PP_PID_HEADERPREFIX_ID					(107)
#define PP_PID_HEADERPREFIX							MAKE_AVP_PID(PP_PID_HEADERPREFIX_ID,PP_APP_ID,avpt_str,0)

#define PP_PID_ALREADYLOCALIZED_ID			(108)
#define PP_PID_ALREADYLOCALIZED					MAKE_AVP_PID(PP_PID_ALREADYLOCALIZED_ID,PP_APP_ID,avpt_bool,0)

#define PP_PID_CATEGORY_ID          (1000)
#define PP_PID_CATEGORY							MAKE_AVP_PID(PP_PID_CATEGORY_ID,PP_APP_ID,avpt_dword,0)

#endif