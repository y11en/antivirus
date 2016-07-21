#if !defined(__SWPACKER_H__)
#define __SWPACKER_H__

// SWPacker.h - definitions for Packer serialization wrapper
// 
//

#include "Swmi.h"

#define LOOKUP_SIZE (0x2000)
#define BUFFER_SIZE (0x4000)


#pragma pack(1)

// ---
typedef struct SWPackerInitData {
	AVP_dword m_nLookUpSize;
	AVP_word  m_nAlgorithm;
} SWPackerInitData;

// ---
typedef struct SWPackerWorkData {
	void *m_pHistory;
	int   m_nHistorySize;
	void *m_pCurrHistory; 
	int   m_nOverHead;
	void *m_pAlgorithmData;
} SWPackerWorkData;


// ---
typedef struct SWPackerData {
	SWPackerWorkData m_rcWork;
	SWPackerInitData m_rcInit;
} SWPackerData;


#pragma pack()


// ---
typedef struct PackContext {
	BYTE   gWorkBuff[36];
	DWORD  gnWrittenBytes;
	BYTE   gnIndexByte;
	BYTE   gnIndexBit;
	DWORD  gnOCount;
	DWORD  gnICount;
	CHAR  *gpOutBuff;
	CHAR  *gpInBuff;
	short *gpLastOccurence;
	short *gpCharList;

	WORD   gnNextWord;
	BYTE   gnCount;
	DWORD *gnIFilled;

	BufferProc  gpBufferProc;
	void       *gpBufferProcData;
} PackContext;


extern const AVP_dword gnGetOutSize;

extern const unsigned short SW_PACKER_ID;



void SWM_PARAM SWPackerCreateSWInstance( SWControl **pSWControl );


#endif

