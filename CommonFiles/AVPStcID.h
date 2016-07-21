#ifndef __AVPSTCID_H__
#define __AVPSTCID_H__

#include "Property/Property.h"
#include "AVPComID.h"

#if ((defined(__WATCOMC__) && (defined(__DOS__)||defined(__OS2__))) || (defined (__unix__)))
#pragma pack(1)
#else
#pragma pack(push,1)
#endif

// Type of cortege
#define STC_CRTG_GENERIC 0
#define STC_CRTG_HEADER  1

// Type of character set
#define STC_CHCR_ANSI 0
#define STC_CHCR_OEM  1

// Type of alignment
#define STC_ALGN_LEFT   0
#define STC_ALGN_RIGHT  1
#define STC_ALGN_CENTER 2

// ---
typedef struct {
	union {
		DWORD m_nFlags;
		struct {
			DWORD m_nType					: 4; // Type of cortege - STC_CRTG_GENERIC/STC_CRTG_HEADER
			DWORD m_bOEMCode      : 1; // Type of character set - STC_CHCR_ANSI/STC_CHCR_OEM
			DWORD m_nReserved     : 3;
			DWORD m_nIconId       : 8; // Identifier of icon
		};
	};
#if !defined(__BDSI_PACK__)
} AvpSTCCortegeFlags;
#else
} __attribute ((packed)) AvpSTCCortegeFlags;
#endif

// ---
typedef struct {
	union {
		DWORD m_nFlags;
		union {
			struct {
				DWORD m_nHAlignType		 : 2; // Align type - STC_ALGN_LEFT/STC_ALGN_RIGHT/STC_ALGN_CENTER
				DWORD m_nHAppDefined	 : 6;
				DWORD m_nHStringId		 : 8; // Identifier of string
				DWORD m_nHIconId       : 8; // Identifier of icon
				DWORD m_nHWidthPercent : 8;	// Width percentage 
			};
			struct {
				DWORD m_nLStringId		 : 8; // Identifier of string
				DWORD m_nLReserved		 : 24;
			};
		};
	};
#if !defined(__BDSI_PACK__)
} AvpSTCDomainFlags;
#else
} __attribute ((packed)) AvpSTCDomainFlags;
#endif

#if !((defined(__WATCOMC__) && (defined(__DOS__)||defined(__OS2__))) || (defined (__unix__)))
#pragma pack(pop)
#else
#pragma pack()
#endif


//#define ST_APP_ID (4)	Repaced to AVPCommID.h

// Cortege data
// Cortege flags
#define ST_PID_CORTEGE_ID						(10000)
#define ST_PID_CORTEGE							MAKE_AVP_PID(ST_PID_CORTEGE_ID,ST_APP_ID,avpt_dword,0)
#define ST_PID_CORTEGE_ROOT(a)			MAKE_AVP_PID(a,ST_APP_ID,avpt_dword,0)

// Property	of ST_PID_CORTEGE_ROOT
// Domains flags. 
// Array of cortege domains flags 
#define ST_PID_DOMAINS_FLAGS_ID			(1)
#define ST_PID_DOMAINS_FLAGS				MAKE_AVP_PID(ST_PID_DOMAINS_FLAGS_ID,ST_APP_ID,avpt_dword,1)

// Property of ST_PID_CORTEGE_ROOT
// Domains strings. 
// Array of cortege domains strings 
#define ST_PID_DOMAINS_STRS_ID			(2)
#define ST_PID_DOMAINS_STRS					MAKE_AVP_PID(ST_PID_DOMAINS_STRS_ID,ST_APP_ID,avpt_wstr,1)
#define ST_PID_DOMAINS_STRS_A				MAKE_AVP_PID(ST_PID_DOMAINS_STRS_ID,ST_APP_ID,avpt_str,1)

// Property of ST_PID_CORTEGE_ROOT
// Cleanup window. 
#define ST_PID_CLEARSTATISTIC_ID		  (3)
#define ST_PID_CLEARSTATISTIC					MAKE_AVP_PID(ST_PID_CLEARSTATISTIC_ID,ST_APP_ID,avpt_nothing,0)

// Report header column widths by percent data
// Received by method GetHeaderColumnWidths
// Array of dwords
#define ST_PID_HEADER_WIDTHS_ID			(10001)
#define ST_PID_HEADER_WIDTHS				MAKE_AVP_PID(ST_PID_HEADER_WIDTHS_ID,ST_APP_ID,avpt_dword,1)	


#endif
