#if !defined(__SWENCRYPTER_H__)
#define __SWENCRYPTER_H__

// SWEncrypter.h - definitions for Encrypter serialization wrapper
// 
//

#include "Swmi.h"

extern const unsigned short SW_ENCRYPTER_ID;

#pragma pack(1)

// ---
typedef struct SWEncrypterInitData {
	AVP_word  m_nAlgorithm;
} SWEncrypterInitData;


// ---
typedef struct SWEncrypterWorkData {
	void *m_pAlgorithmData;
} SWEncrypterWorkData;


// ---
typedef struct SWEncrypterData {
	SWEncrypterWorkData m_rcWork;
	SWEncrypterInitData m_rcInit;	
} SWEncrypterData;

#pragma pack()


SWM_PROC AVP_bool SWM_PARAM EDBinary( void **pAlgoData, unsigned char *pCryptBuff,  int nCryptBuffLen );


#endif

