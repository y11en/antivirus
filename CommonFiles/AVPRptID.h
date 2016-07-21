#ifndef __AVPRPTID_H__
#define __AVPRPTID_H__

#include "Property/Property.h"
#include "AVPComID.h"

#if ((defined(__WATCOMC__) && (defined(__DOS__)||defined(__OS2__))) || (defined (__unix__)))
#pragma pack(1)
#else
#pragma pack(push,1)
#endif

// Type of cortege
#define RPT_CRTG_GENERIC			0
#define RPT_CRTG_HEADER				1
#define RPT_CRTG_INFORMATION	2

// Type of parsing
#define RPT_PRSR_INTERNAL 0
#define RPT_PRSR_EXTERNAL 1

// Type of character set
#define RPT_CHCR_ANSI 0
#define RPT_CHCR_OEM  1

// Checked type
#define RPT_CHK_UNCHECKED 0
#define RPT_CHK_CHECKED   1

// Type of alignment
#define RPT_ALGN_LEFT   0
#define RPT_ALGN_RIGHT  1
#define RPT_ALGN_CENTER 2

// Type of information
#define RPT_INFO_APP_ID							0
#define RPT_INFO_APP_CTYPE					1
#define RPT_INFO_APP_TASKNUM 				2
#define RPT_INFO_OBJRESULT_COLUMN		3
#define RPT_INFO_OBJRESULT_VALUE		4

// ---
typedef struct {
	union {
		DWORD m_nFlags;
		struct {
			DWORD m_nLevel				: 8; // Number of level in hierarchy
			DWORD m_nType					: 4; // Type of cortege - RPT_CRTG_XXXXXXX
			DWORD m_bExtParsing   : 1; // Type of parsing	-	RPT_PRSR_XXXXXXX
			DWORD m_bOEMCode      : 1; // Type of character set - RPT_CHCR_XXXXXXX
			DWORD m_bChecked      : 1; // Type of checked - RPT_CHK_XXXXXXX
			DWORD m_nReserved     : 1;
			DWORD m_nIconId       : 8; // Identifier of icon
			DWORD m_nAppDefined	  : 8;
		};
	};
#if !defined(__BDSI_PACK__)
} AvpRPTCortegeFlags;
#else
} __attribute ((packed)) AvpRPTCortegeFlags;
#endif

// ---
typedef struct {
	union {
		DWORD m_nFlags;
		union {
			// Generic cortege domain
			struct {
				DWORD m_nLObjectResult : 8;	// Object processing result
				DWORD m_nLReserved		 : 24;
			};
			// Header cortege domain
			struct {
				DWORD m_nHAlignType		 : 2; // Align type - RPT_ALGN_XXXXX
				DWORD m_nHAppDefined	 : 6;
				DWORD m_nHStringId		 : 8; // Identifier of string
				DWORD m_nHIconId       : 8; // Identifier of icon
				DWORD m_nHWidthPercent : 8;	// Width percentage 
			};
			// Information cortege domain
			struct {
				DWORD m_nIType    		 : 8;	// Type of information - RPT_INFO_XXXXXXX
				DWORD m_nIValue   		 : 24;
			};
		};
	};
#if !defined(__BDSI_PACK__)
} AvpRPTDomainFlags;
#else
} __attribute ((packed)) AvpRPTDomainFlags;
#endif

#if !((defined(__WATCOMC__) && (defined(__DOS__)||defined(__OS2__))) || (defined (__unix__)))
#pragma pack(pop)
#else
#pragma pack()
#endif

// Standard symbols in the first position of report cortege
// "#" - Session header 
// ";" - Commentary
// ":" - Statistic line


// Icon identificators

// Standard icon identificators
#define RP_IID_WARNING_ICON						200
#define RP_IID_ERROR_ICON							201
#define RP_IID_INFORMATION_ICON				202
#define RP_IID_SESSION_START_ICON			203
#define RP_IID_SESSION_END_ICON				204

// AVP Scanner/Monitor
#define RP_IID_INFECTED								205
#define RP_IID_ARCHIVE								206
#define RP_IID_CLEAN									207
#define RP_IID_CORRUPTED							208
#define RP_IID_DELETED								209
#define RP_IID_DISINFECTED						210
#define RP_IID_DISK_FULL							211
#define RP_IID_ENCRYPTED							212
#define RP_IID_IOERROR								213
#define RP_IID_LOCKED									214
#define RP_IID_PACKED									215
#define RP_IID_SUSPICION							216
#define RP_IID_UNKNOWN_FORMAT					217
#define RP_IID_WARNING								218
#define RP_IID_DENY_ACCESS						219

// AVP Inspector
#define RP_IID_CHANGED_FILE						220
#define RP_IID_NEW_FILE								221
#define RP_IID_DELETED_FILE						222
#define RP_IID_MOVED_FILE							223
#define RP_IID_RENAMED_FILE						224

#define RP_IID_NEW_DIR								225
#define RP_IID_DELETED_DIR						226
#define RP_IID_MOVED_DIR							227



// Find item flags
#define RFF_MATCHCASE       0x0001
#define RFF_MATCHWHOLEWORD  0x0002
#define RFF_MATCHWHOLESTR   0x0004


//#define RP_APP_ID (3)	Repaced to AVPCommID.h

// Cortege data
// Sent by event RVCParseCortege
// Cortege flags
#define RP_PID_CORTEGE_ID						(10000)
#define RP_PID_CORTEGE							MAKE_AVP_PID(RP_PID_CORTEGE_ID,RP_APP_ID,avpt_dword,0)
#define RP_PID_CORTEGE_ROOT(a)			MAKE_AVP_PID(a,RP_APP_ID,avpt_dword,0)

// Property	of RP_PID_CORTEGE_ROOT
// Domains flags. 
// Array of cortege domains flags 
#define RP_PID_DOMAINS_FLAGS_ID			(1)
#define RP_PID_DOMAINS_FLAGS				MAKE_AVP_PID(RP_PID_DOMAINS_FLAGS_ID,RP_APP_ID,avpt_dword,1)

// Property of RP_PID_CORTEGE_ROOT
// Domains strings. 
// Array of cortege domains strings 
#define RP_PID_DOMAINS_STRS_ID			  (2)
#define RP_PID_DOMAINS_STRS					MAKE_AVP_PID(RP_PID_DOMAINS_STRS_ID,RP_APP_ID,avpt_str,1)

// Report session headlines data
// Received by method GetSessionHeadlines
// Array of strings
#define RP_PID_SESSION_HEADLINES_ID  (10001)
#define RP_PID_SESSION_HEADLINES		MAKE_AVP_PID(RP_PID_SESSION_HEADLINES_ID,RP_APP_ID,avpt_str,1)	


// Report header column widths by percent data
// Received by method GetHeaderColumnWidths
// Array of dwords
#define RP_PID_HEADER_WIDTHS_ID			(10002)
#define RP_PID_HEADER_WIDTHS				MAKE_AVP_PID(RP_PID_HEADER_WIDTHS_ID,RP_APP_ID,avpt_dword,1)	

#endif
