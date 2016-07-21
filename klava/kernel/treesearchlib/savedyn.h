// savedyn.h
//

#ifndef savedyn_h_INCLUDED
#define savedyn_h_INCLUDED

#include "common.h"

typedef struct tag_NODEFLAGS
{
	uint8_t bNodeType    : 3;
	uint8_t bCondition   : 1;
	uint8_t bSignatureID : 1;
	uint8_t bNodeDataPtr : 1;
	uint8_t bReserved1   : 1;
	uint8_t bReserved2   : 1;
}
NODEFLAGS, *PNODEFLAGS;

enum INDEX_STORE_TYPE {
	cBitmap = 1,
	cArray,
	cFull
};

#endif // savedyn_h_INCLUDED

