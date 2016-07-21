

#ifndef __CASETBL_H
#define __CASETBL_H

#include <kulib/ku_defs.h>

#include <stdio.h>

#define STAGE1_MASK 0xff
#define STAGE1_SHIFT 8
#define STAGE1_SIZE	0x100

#define STAGE2_MASK 0xff
#define STAGE2_MAX_SIZE	( 0x10000 - STAGE1_SIZE )
#define STAGE2_BLOCK_SIZE 0x100

typedef struct __casetbl
{
	uint16_t stage1[ STAGE1_SIZE ];
	
	int16_t stage2[ STAGE2_MAX_SIZE ];
	int stage2_size;
	
} casetbl;


void casetbl_init( casetbl* tbl );
int casetbl_add( casetbl* tbl, ucs4_t s, ucs4_t d );
uint32_t casetbl_size( casetbl* tbl );
void casetbl_write( casetbl* tbl, FILE* fp );




#endif /* __CASETBL_H */