////////////////////////////////////////////////////////////////////
//
//  Enc2Text.h
//  Light encryption Text-To-Text algorithm definition
//  AVP general purposes stuff
//  Victor Matiouchenkov [victor@avp.ru], Kaspersky Labs. 2000
//  Copyright (c) Kaspersky Labs
//
////////////////////////////////////////////////////////////////////
#ifndef __ENC2TEXT_H__
#define __ENC2TEXT_H__

#include <windows.h>

#ifdef __cplusplus
extern "C" {
#endif

#define E2T_LENGTH(scount) ( (scount/3)*4 + ( ((scount % 3)==2) ? 3 : (((scount % 3)==1) ? 2 : 0) ) )
#define D2T_LENGTH(scount) (((scount/4)*3) + (scount % 4))

// Converts text to encrypted text
// Uses the same buffer for encrypting
// Uses BinToText function (Text2Bin.c) for converting encrypted binary to text
// pSource			- pointer to source buffer to be converted
// uiSCount			- number of bytes in pSource
// pDestination - pointer to destination buffer for storing resulting text
// uiDCount			- size of destination buffer in bytes. Buffer should be at least E2T_LENGTH bytes 
// Return value - > 0 - number of bytes successfully written to destination buffer
//                0 - error, no data was written
unsigned int EncryptToText(TCHAR *pSource, unsigned int uiSCount, TCHAR *pDestination, unsigned int uiDCount);


// Decrypt previously enctypted text to cource text
// Uses the same buffer for decrypting
// Uses TextToBin function (Text2Bin.c) for converting from text to encrypted binary
// pSource			- pointer to source buffer to be converted
// uiSCount			- number of bytes in pSource
// pDestination - pointer to destination buffer for storing resulting text
// uiDCount			- size of destination buffer in bytes. Buffer should be at least D2T_LENGTH bytes 
// Return value - > 0 - number of bytes successfully written to destination buffer
//                0 - error, no data was written
unsigned int DecryptToText(TCHAR *pSource, unsigned int uiSCount, TCHAR *pDestination, unsigned int uiDCount);


#ifdef __cplusplus
}
#endif


#endif //__ENC2TEXT_H__
