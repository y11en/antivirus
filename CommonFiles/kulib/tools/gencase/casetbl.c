
#include "casetbl.h"

#include <stdio.h>
#include <memory.h>

void casetbl_init( casetbl* tbl )
{
	memset( tbl, 0, sizeof(casetbl) );
	tbl->stage2_size = STAGE2_BLOCK_SIZE;
}

int casetbl_add( casetbl* tbl, ucs4_t s, ucs4_t d )
{
	int32_t index1;
	int32_t delta;

	if( s >= 0x10000 )
	{
		fprintf( stderr, "error: \n" );
		return 0;
	}

	index1 = ( s >> STAGE1_SHIFT ) & STAGE1_MASK;
	
	if( tbl->stage1[ index1 ] == 0 )
	{
		tbl->stage1[ index1 ] = tbl->stage2_size;
		tbl->stage2_size += STAGE2_BLOCK_SIZE;

		if( tbl->stage2_size > STAGE2_MAX_SIZE )
		{
			fprintf( stderr, "error: stage2 overflow.\n" );
			return 0;	
		}
	}

	delta = d - s;
	if( delta < INT16_MIN || delta > INT16_MAX )
	{
		fprintf( stderr, "error: invalid delta\n" );

		return 0;
	}
	
	
	tbl->stage2[ tbl->stage1[ index1 ] + ( s & STAGE2_MASK ) ] = (int16_t) delta;


	return 1;
}


void casetbl_prepare( casetbl* tbl )
{
	int i;

	for( i = 0; i < STAGE1_SIZE; ++i )
	{
		tbl->stage1[ i ] += STAGE1_SIZE;
	}
	
}

void casetbl_write( casetbl* tbl, FILE* fp )
{
	casetbl_prepare( tbl );

	fwrite( tbl->stage1, sizeof(tbl->stage1), 1, fp );
	fwrite( tbl->stage2, sizeof(tbl->stage2[0]) * tbl->stage2_size, 1, fp );
}

uint32_t casetbl_size( casetbl* tbl )
{
	return STAGE1_SIZE + tbl->stage2_size;
}