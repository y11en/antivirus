#ifndef __AVPNSTID_H
#define __AVPNSTID_H


#include "property/property.h"
#include "avpcomid.h"

//#define NS_APP_ID (2)	Repaced to AVPCommID.h

// ID object node in NS-tree
// a - unique uint value
// b - product ID
#define NS_PID_OBJECT(a,b)					MAKE_AVP_PID(a,b,avpt_wstr,0)

// Property	of data NS_PID_OBJECT
// Pattern ID. 
// ID corresponding Pattern subtree in Pattern tree
#define NS_PID_PATTERN_ID_ID				(1)
#define NS_PID_PATTERN_ID						MAKE_AVP_PID(NS_PID_PATTERN_ID_ID,NS_APP_ID,avpt_dword,0)

// Property	of data NS_PID_OBJECT
// Display string 
// String showed on display
#define NS_PID_DISPLAY_STR_ID				(2)
#define NS_PID_DISPLAY_STR					MAKE_AVP_PID(NS_PID_DISPLAY_STR_ID,NS_APP_ID,avpt_str,0)

// Property	of data NS_PID_OBJECT
// First icon
// Handle "normal" icon for NS-tree
// Handle "large" icon for NS-list
#define NS_PID_FIRST_ICON_ID				(3)
#define NS_PID_FIRST_ICON			  		MAKE_AVP_PID(NS_PID_FIRST_ICON_ID,NS_APP_ID,avpt_dword,0)

// Property	of data NS_PID_OBJECT
// Second icon
// Handle "selected (opened)" icon for NS-tree
// Handle "small" icon for NS-list
#define NS_PID_SECOND_ICON_ID				(4)
#define NS_PID_SECOND_ICON			 		MAKE_AVP_PID(NS_PID_SECOND_ICON_ID,NS_APP_ID,avpt_dword,0)

// Property	of data NS_PID_OBJECT
// First icon image list index
// Handle "normal" icon for NS-tree
// Handle "large" icon for NS-list
#define NS_PID_FIRST_ICON_IND_ID		(5)
#define NS_PID_FIRST_ICON_IND			  MAKE_AVP_PID(NS_PID_FIRST_ICON_IND_ID,NS_APP_ID,avpt_dword,0)

// Property	of data NS_PID_OBJECT
// Second icon image list index
// "Selected (opened)" icon image list index for NS-tree
// "Small" icon image list index for NS-list
#define NS_PID_SECOND_ICON_IND_ID		(6)
#define NS_PID_SECOND_ICON_IND			MAKE_AVP_PID(NS_PID_SECOND_ICON_IND_ID,NS_APP_ID,avpt_dword,0)

// Property	of data NS_PID_OBJECT
// Header names
// String array containig strings for naming header columns in NS-list
#define NS_PID_LIST_HEADER_NAMES_ID	(7)
#define NS_PID_LIST_HEADER_NAMES		MAKE_AVP_PID(NS_PID_LIST_HEADER_NAMES_ID,NS_APP_ID,avpt_str,1)

// Property	of data NS_PID_OBJECT
// Header names
// String array containig strings for naming header columns in NS-list
#define NS_PID_LIST_HEADER_NAME_IDS_ID	(8)
#define NS_PID_LIST_HEADER_NAME_IDS		  MAKE_AVP_PID(NS_PID_LIST_HEADER_NAMES_ID,NS_APP_ID,avpt_dword,1)

// Header colemns alignment types	for PROP NS_PID_LIST_HEADER_ALIGN
#define NS_HEADER_ALIGN_LEFT        0
#define NS_HEADER_ALIGN_RIGHT       1
#define NS_HEADER_ALIGN_CENTER      2

// Property	of data NS_PID_OBJECT
// Header columns alignment
#define NS_PID_LIST_HEADER_ALIGN_ID	(9)
#define NS_PID_LIST_HEADER_ALIGN		MAKE_AVP_PID(NS_PID_LIST_HEADER_ALIGN_ID,NS_APP_ID,avpt_byte,1)

// Property	of data NS_PID_OBJECT
// Object properties values strings
#define NS_PID_LIST_PROP_VALUES_ID	(10)
#define NS_PID_LIST_PROP_VALUES		  MAKE_AVP_PID(NS_PID_LIST_PROP_VALUES_ID,NS_APP_ID,avpt_str,1)

// NS-objects types	for PROP NS_PID_OBJECT_TYPE
#define NS_OBJECT_TYPE_FOLDER							(1) 
#define NS_OBJECT_TYPE_FILE								(2) 
#define NS_OBJECT_TYPE_MYCOMPUTER		      (3) 
#define NS_OBJECT_TYPE_NWROOT	            (4) 
#define NS_OBJECT_TYPE_NWRESOURCE	        (5) 

// Property	of data NS_PID_OBJECT
// NS-object type
#define NS_PID_OBJECT_TYPE_ID						(11)
#define NS_PID_OBJECT_TYPE							MAKE_AVP_PID(NS_PID_OBJECT_TYPE_ID,NS_APP_ID,avpt_dword,0)

// Property	of data NS_PID_OBJECT
// NS-object type
#define NS_PID_FOLDER_HASSUBFOLDERS_ID	(12)
#define NS_PID_FOLDER_HASSUBFOLDERS			MAKE_AVP_PID(NS_PID_FOLDER_HASSUBFOLDERS_ID,NS_APP_ID,avpt_nothing, 0)


// Property	of data NS_PID_OBJECT
// Should be immediatelly expanded
#define NS_PID_SHOULDBEEXPANDED_ID     	(13)
#define NS_PID_SHOULDBEEXPANDED					MAKE_AVP_PID(NS_PID_SHOULDBEEXPANDED_ID,NS_APP_ID,avpt_nothing, 0)


// Property	of data NS_PID_OBJECT
// Display string ID
// ID of string showed on display
#define NS_PID_DISPLAY_STRID_ID					(14)
#define NS_PID_DISPLAY_STRID						MAKE_AVP_PID(NS_PID_DISPLAY_STRID_ID,NS_APP_ID,avpt_dword,0)


// Property	of data NS_PID_OBJECT
// First icon
// DIB    "normal" icon for NS-tree
// DIB    "large" icon for NS-list
#define NS_PID_FIRST_ICON_DIB_ID				(15)
#define NS_PID_FIRST_ICON_DIB 		  		MAKE_AVP_PID(NS_PID_FIRST_ICON_DIB_ID,NS_APP_ID,avpt_bin,0)

// Property	of data NS_PID_OBJECT
// Second icon
// DIB    "selected (opened)" icon for NS-tree
// DIB    "small" icon for NS-list
#define NS_PID_SECOND_ICON_DIB_ID				(16)
#define NS_PID_SECOND_ICON_DIB			 		MAKE_AVP_PID(NS_PID_SECOND_ICON_DIB_ID,NS_APP_ID,avpt_bin,0)

// Property	of data NS_PID_OBJECT
// Should be immediatelly selected
#define NS_PID_SHOULDBESELECTED_ID     	(17)
#define NS_PID_SHOULDBESELECTED					MAKE_AVP_PID(NS_PID_SHOULDBESELECTED_ID,NS_APP_ID,avpt_nothing, 0)

// ---
typedef struct { 
	DWORD  dwScope;
	DWORD  dwType; 
  DWORD  dwDisplayType;
	DWORD  dwUsage;
	TCHAR  tchLocalName[_MAX_PATH]; 
  TCHAR  tchRemoteName[_MAX_PATH];
	TCHAR  tchComment[_MAX_PATH];
	TCHAR  tchProvider[_MAX_PATH]; 
} NS_NETRESOURCE_A; 

// ---
typedef struct { 
	DWORD  dwScope;
	DWORD  dwType; 
  DWORD  dwDisplayType;
	DWORD  dwUsage;
	WCHAR  tchLocalName[_MAX_PATH]; 
  WCHAR  tchRemoteName[_MAX_PATH];
	WCHAR  tchComment[_MAX_PATH];
	WCHAR  tchProvider[_MAX_PATH]; 
} NS_NETRESOURCE_W; 


// Property	of data NS_PID_OBJECT
// NetWork resource descriptor
#define NS_PID_NETWORKRESOURCE_ID     	(18)
#define NS_PID_NETWORKRESOURCE					MAKE_AVP_PID(NS_PID_NETWORKRESOURCE_ID,NS_APP_ID,avpt_bin, 0)

// Property	of data NS_PID_OBJECT
// Access denied
#define NS_PID_ACCESSDENIED_ID     			(19)
#define NS_PID_ACCESSDENIED							MAKE_AVP_PID(NS_PID_ACCESSDENIED_ID,NS_APP_ID,avpt_nothing, 0)

// Property	of data NS_PID_OBJECT
// Not checkable
#define NS_PID_NOCHECKABLE_ID     			(20)
#define NS_PID_NOCHECKABLE							MAKE_AVP_PID(NS_PID_NOCHECKABLE_ID,NS_APP_ID,avpt_nothing, 0)

// Property	of data NS_PID_OBJECT
// Rule not expected
#define NS_PID_RULENOTEXPECTED_ID     	(21)
#define NS_PID_RULENOTEXPECTED					MAKE_AVP_PID(NS_PID_RULENOTEXPECTED_ID,NS_APP_ID,avpt_nothing, 0)

// Property	of data NS_PID_OBJECT
// Should be immediatelly collapsed
#define NS_PID_SHOULDBECOLLAPSED_ID     (22)
#define NS_PID_SHOULDBECOLLAPSED				MAKE_AVP_PID(NS_PID_SHOULDBECOLLAPSED_ID,NS_APP_ID,avpt_nothing, 0)

// Property	of data NS_PID_OBJECT
// Should be renamed later
#define NS_PID_SHOULDCHANGEDISPLAYNAME_ID				(23)
#define NS_PID_SHOULDCHANGEDISPLAYNAME					MAKE_AVP_PID(NS_PID_SHOULDCHANGEDISPLAYNAME_ID,NS_APP_ID,avpt_nothing, 0)

// Property	of data NS_PID_OBJECT
// Number of folder	objects
#define NS_PID_NUMBER_OF_FOLDERS_ID						(24)
#define NS_PID_NUMBER_OF_FOLDERS							MAKE_AVP_PID(NS_PID_NUMBER_OF_FOLDERS_ID,NS_APP_ID,avpt_dword,0)

// Property	of data NS_PID_OBJECT
// Number of file	objects
#define NS_PID_NUMBER_OF_FILES_ID						(25)
#define NS_PID_NUMBER_OF_FILES							MAKE_AVP_PID(NS_PID_NUMBER_OF_FILES_ID,NS_APP_ID,avpt_dword,0)

// Property	of data NS_PID_OBJECT
// NetWork resource descriptor
#define NS_PID_NETWORKRESOURCE_W_ID     	(26)
#define NS_PID_NETWORKRESOURCE_W					MAKE_AVP_PID(NS_PID_NETWORKRESOURCE_W_ID,NS_APP_ID,avpt_bin, 0)

// Property	of data NS_PID_OBJECT
// Display string 
// String showed on display
#define NS_PID_DISPLAY_STR_W_ID						(27)
#define NS_PID_DISPLAY_STR_W							MAKE_AVP_PID(NS_PID_DISPLAY_STR_W_ID,NS_APP_ID,avpt_wstr,0)

// Property	of data NS_PID_OBJECT
// Object properties values strings
#define NS_PID_LIST_PROP_VALUES_W_ID			(28)
#define NS_PID_LIST_PROP_VALUES_W					MAKE_AVP_PID(NS_PID_LIST_PROP_VALUES_W_ID,NS_APP_ID,avpt_wstr,1)



// For internal using only!

// Property	of data NS_PID_OBJECT
// Object has rule
#define NS_PID_HAS_RULE_ID										(1000)
#define NS_PID_HAS_RULE												MAKE_AVP_PID(NS_PID_HAS_RULE_ID,NS_APP_ID,avpt_nothing,0)

// Property	of data NS_PID_OBJECT
// Object has strict rule
#define NS_PID_HAS_STRICT_RULE_ID							(1001)
#define NS_PID_HAS_STRICT_RULE								MAKE_AVP_PID(NS_PID_HAS_STRICT_RULE_ID,NS_APP_ID,avpt_nothing,0)

// Property	of data NS_PID_OBJECT
// Number of children with rules
#define NS_PID_CHILDREN_WITH_RULES_ID					(1002)
#define NS_PID_CHILDREN_WITH_RULES						MAKE_AVP_PID(NS_PID_CHILDREN_WITH_RULES_ID,NS_APP_ID,avpt_dword,0)

// Property	of data NS_PID_OBJECT
// Number of children with strict rules
#define NS_PID_CHILDREN_WITH_STRICT_RULES_ID  (1003)
#define NS_PID_CHILDREN_WITH_STRICT_RULES			MAKE_AVP_PID(NS_PID_CHILDREN_WITH_STRICT_RULES_ID,NS_APP_ID,avpt_dword,0)

// Property	of data NS_PID_OBJECT
// Handles of node icons
#define NS_PID_ICONHANDLES_ID									(1004)
#define NS_PID_ICONHANDLES										MAKE_AVP_PID(NS_PID_ICONHANDLES_ID,NS_APP_ID,avpt_dword,1)

// Property	of data NS_PID_OBJECT
// Ownership of node icons
#define NS_PID_ICONOWNERSHIP_ID									(1005)
#define NS_PID_ICONOWNERSHIP										MAKE_AVP_PID(NS_PID_ICONOWNERSHIP_ID,NS_APP_ID,avpt_bool,0)

// Property	of data NS_PID_OBJECT
// NST subtree has been loaded
#define NS_PID_NSTSUBTREE_LOADED_ID						  (2000)
#define NS_PID_NSTSUBTREE_LOADED								MAKE_AVP_PID(NS_PID_NSTSUBTREE_LOADED_ID,NS_APP_ID,avpt_nothing,0)

// Property	of data NS_PID_OBJECT
// NSL subtree has been loaded
#define NS_PID_NSLSUBTREE_LOADED_ID						  (2001)
#define NS_PID_NSLSUBTREE_LOADED								MAKE_AVP_PID(NS_PID_NSLSUBTREE_LOADED_ID,NS_APP_ID,avpt_nothing,0)

// Data - Rules tree root
// Object rule tree root
#define NS_PID_RULE_ROOT_ID			    (GET_AVP_PID_ID_MAX_VALUE)
#define NS_PID_RULE_ROOT						MAKE_AVP_PID(NS_PID_RULE_ROOT_ID,NS_APP_ID,avpt_nothing,0)

// Property	of data NS_PID_RULE_ROOT
// Rule selected
#define NS_PID_RULE_SELECTED_ID			(1)
#define NS_PID_RULE_SELECTED				MAKE_AVP_PID(NS_PID_RULE_SELECTED_ID,NS_APP_ID,avpt_nothing,0)

// Property	of data NS_PID_RULE_ROOT
// Rule strict
#define NS_PID_RULE_STRICT_ID				(2)
#define NS_PID_RULE_STRICT					MAKE_AVP_PID(NS_PID_RULE_STRICT_ID,NS_APP_ID,avpt_nothing,0)

// Property	of data NS_PID_RULE_ROOT
// Rule tree host PID
#define NS_PID_RULE_HOST_PID_ID			(3)
#define NS_PID_RULE_HOST_PID				MAKE_AVP_PID(NS_PID_RULE_HOST_PID_ID,NS_APP_ID,avpt_dword,0)

// Property	of data NS_PID_RULE_ROOT
// Rule is host
#define NS_PID_RULE_IS_HOST_ID			(4)
#define NS_PID_RULE_IS_HOST					MAKE_AVP_PID(NS_PID_RULE_IS_HOST_ID,NS_APP_ID,avpt_nothing,0)


#endif
