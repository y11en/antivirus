////////////////////////////////////////////////////////////////////
//
//  Enc2Text.c
//  Light encryption Text-To-Text algorithm implementation
//  AVP general purposes stuff
//  Victor Matiouchenkov [victor@avp.ru], Kaspersky Labs. 2000
//  Copyright (c) Kaspersky Labs
//
////////////////////////////////////////////////////////////////////

#include <Stuff\Enc2Text.h>
#include <Stuff\Text2Bin.h>


// Do not change this value - there are files encrypted with it
#define ENCRYPT_KEY (0x7773 ^ 0x4c4b) //('ws'^'LK')

// ---
static void EDBinary( unsigned short *pCryptBuff,  int nCryptBuffLen ) {
  int            nSize; 
	unsigned int   bOk = 1;
  unsigned short nEncryptKey = ENCRYPT_KEY;

  for ( nSize=nCryptBuffLen >> 1; nSize; nSize-- ) {
    int nBit = (nEncryptKey & 1);
    *((unsigned short *)pCryptBuff) ^= nEncryptKey;
    ((unsigned short *)pCryptBuff)++;
    nEncryptKey >>= 1;
    nEncryptKey |= ((nBit ^ (nEncryptKey & 1)) << 15);
  }

  if ( (nCryptBuffLen % 2) ) 
    *pCryptBuff ^= nEncryptKey;
}

// Converts text to encrypted text
// Uses the same buffer for encrypting
// Uses BinToText function (Text2Bin.c) for converting encrypted binary to text
// pSource			- pointer to source buffer to be converted
// uiSCount			- number of bytes in pSource
// pDestination - pointer to destination buffer for storing resulting text
// uiDCount			- size of destination buffer in bytes. Buffer should be at least E2T_LENGTH bytes 
// Return value - > 0 - number of bytes successfully written to destination buffer
//                0 - error, no data was written
unsigned int EncryptToText(TCHAR *pSource, unsigned int uiSCount, TCHAR *pDestination, unsigned int uiDCount) {
	if ( uiDCount < E2T_LENGTH(uiSCount) )
		return 0;

	EDBinary( (unsigned short *)pSource, uiSCount );

	return BinToText( pSource, uiSCount, pDestination, uiDCount );
}


// Decrypt previously enctypted text to cource text
// Uses the same buffer for decrypting
// Uses TextToBin function (Text2Bin.c) for converting from text to encrypted binary
// pSource			- pointer to source buffer to be converted
// uiSCount			- number of bytes in pSource
// pDestination - pointer to destination buffer for storing resulting text
// uiDCount			- size of destination buffer in bytes. Buffer should be at least D2T_LENGTH bytes 
// Return value - > 0 - number of bytes successfully written to destination buffer
//                0 - error, no data was written
unsigned int DecryptToText(TCHAR *pSource, unsigned int uiSCount, TCHAR *pDestination, unsigned int uiDCount) {
	unsigned int nResult;

	if ( uiDCount < D2T_LENGTH(uiSCount) )
		return 0;

	nResult = TextToBin( pSource, uiSCount, pDestination, uiDCount );

	if ( nResult )
		EDBinary( (unsigned short *)pDestination, nResult );

	return nResult;
}
