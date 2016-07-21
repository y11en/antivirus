#if !defined(__SWCRCCHECKER_H__)
#define __SWCRCCHECKER_H__

// SWCRCChecker.h - definitions for CRCChecker serialization wrapper
// 
//

#include "Swmi.h"

extern const unsigned short SW_CRCCHECKER_ID;

#define LOOKUP_SIZE (0x400)

#pragma pack(1)

// ---
typedef struct SWCRCCheckerInitData {
	AVP_dword m_nLookUpSize;
	AVP_word  m_nAlgorithm;
}  SWCRCCheckerInitData;

// ---
typedef struct SWCRCCheckerWorkData {
	AVP_dword m_nCRCCheckCount;
	AVP_dword m_nCRCCheckCRC;
} SWCRCCheckerWorkData;


// ---
typedef struct SWCRCCheckerData {
	SWCRCCheckerWorkData m_rcWork;
	SWCRCCheckerInitData m_rcInit;
} SWCRCCheckerData;


#pragma pack()


SWM_PROC AVP_dword SWM_PARAM CountCRC( AVP_byte* pBuffer, AVP_dword nSize, AVP_dword nOldCrc32 );


#endif

