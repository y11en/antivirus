#include "unzlx.h"

#include <Prague/pr_err.h>

#include <memory.h>

typedef struct _tag_lzx_bits 
{
	tDWORD bb;	// Bit Buffer
	tINT bl;	// Bits left
//	tBYTE *ip;	// Input Pointer
}lzx_bits ;

//extern "C" tINT  printf(const char*,...);

#define CAB(x)   (decomp_state->x)
#define LZX(x)   (decomp_state->lzx.x)

#ifdef _DEBUG
#define D(x) { if(CAB(bDebugFlag)){ printf x ; printf(" Line is %d\n", __LINE__);} }
#else
#define D(x) {}
#endif

static tINT CAB_LZXinit(LZXData *decomp_state, tBYTE* lpWindow, tINT window) 
{
tDWORD wndsize = 1 << window;
tINT i, j, posn_slots;

	D(("LZX: init wndsize=%ld ", window))

	LZX(window) = lpWindow;
	LZX(window_size) = wndsize;

	// initialise static tables 
	for (i=0, j=0; i <= 50; i += 2) 
	{
		LZX(extra_bits[i]) = LZX(extra_bits[i+1]) = (tBYTE)j; // 0,0,0,0,1,1,2,2,3,3... 
		if ((i != 0) && (j < 17)) 
			j++; // 0,0,1,2,3,4...15,16,17,17,17,17... 
	}
	
	for (i=0, j=0; i <= 50; i++) 
	{
		LZX(position_base[i]) = j; // 0,1,2,3,4,6,8,12,16,24,32,... 
		j += 1 << LZX(extra_bits[i]); // 1,1,1,1,2,2,4,4,8,8,16,16,32,32,... 
	}

	// calculate required position slots 
	if (window == 20)
		posn_slots = 42;
	else if (window == 21)
		posn_slots = 50;
	else 
		posn_slots = window << 1;

  //posn_slots=i=0; while (i < wndsize) i += 1 << extra_bits[posn_slots++]; 
  

	LZX(R0)  =  LZX(R1)  = LZX(R2) = 1;
	LZX(main_elements)   = LZX_NUM_CHARS + (posn_slots << 3);
	LZX(header_read)     = 0;
	LZX(frames_read)     = 0;
	LZX(block_remaining) = 0;
	LZX(block_type)      = LZX_BLOCKTYPE_INVALID;
	LZX(intel_curpos)    = 0;
	LZX(intel_started)   = 0;
	LZX(window_posn)     = 0;

	// initialise tables to 0 (because deltas will be applied to them)
	for (i = 0; i < LZX_MAINTREE_MAXSYMBOLS; i++)
		LZX(MAINTREE_len)[i] = 0;
	for (i = 0; i < LZX_LENGTH_MAXSYMBOLS; i++)
		LZX(LENGTH_len)[i]   = 0;

	return errOK;
}


/* Bitstream reading macros:
 *
 * INIT_BITSTREAM    should be used first to set up the system
 * READ_BITS(var,n)  takes N bits from the buffer and puts them in var
 *
 * ENSURE_BITS(n)    ensures there are at least N bits in the bit buffer
 * PEEK_BITS(n)      extracts (without removing) N bits from the bit buffer
 * REMOVE_BITS(n)    removes N bits from the bit buffer
 *
 * These bit access routines work by using the area beyond the MSB and the
 * LSB as a free source of zeroes. This avoids having to mask any bits.
 * So we have to know the bit width of the bitbuffer variable. This is
 * sizeof(tDWORD) * 8, also defined as ULONG_BITS
 */

/* number of bits in tDWORD. Note: This must be at multiple of 16, and at
 * least 32 for the bitbuffer code to work (ie, it must be able to ensure
 * up to 17 bits - that's adding 16 bits when there's one bit left, or
 * adding 32 bits when there are no bits left. The code should work fine
 * for machines where tDWORD >= 32 bits.
 */

static tBYTE NEXTBYTE(LZXData* decomp_state, tERROR* uiRetVal)
{
tBYTE szBuff[2];
	
	szBuff[0] = 0;
	*uiRetVal=CALL_SeqIO_ReadByte(decomp_state->hInputSeqIo, szBuff);
#ifdef _DEBUG
	if(PR_FAIL(*uiRetVal))
	{
		*uiRetVal=errOBJECT_READ;
	}
	else
		*uiRetVal=errOK;
#endif
	decomp_state->dwRead ++;
	return szBuff[0];
}

static tERROR GetBack2Bytes(LZXData* decomp_state)
{
	decomp_state->dwRead -= 2;
	return CALL_SeqIO_Seek(decomp_state->hInputSeqIo, NULL, 2, cSEEK_BACK);
}

#define ULONG_BITS (sizeof(tDWORD)<<3)

//#define INIT_BITSTREAM do { bitsleft = 0; bitbuf = 0; } while (0)
#define INIT_BITSTREAM bitsleft = 0; bitbuf = 0;

/*
#define ENSURE_BITS(n)							\
  while (bitsleft < (n)) {						\
    bitbuf |= ((inpos[1]<<8)|inpos[0]) << (ULONG_BITS-16 - bitsleft);	\
    bitsleft += 16; inpos+=2;						\
    if((tDWORD)(endinp+2) < (tDWORD)inpos)					\
    {                                                                   \
        D(("Error line %d", __LINE__));					\
		return DECR_ILLEGALDATA;					\
    }                                                                   \
  }

#define ENSURE_BITS_NO_SURE(n)							\
  while (bitsleft < (n)) {						\
    bitbuf |= ((inpos[1]<<8)|inpos[0]) << (ULONG_BITS-16 - bitsleft);	\
    bitsleft += 16; inpos+=2;						\
  }
*/
#define ENSURE_BITS(n)							\
	while (bitsleft < (n)) {						\
	byTemp0 = NEXTBYTE(decomp_state, &error);	\
	byTemp1 = NEXTBYTE(decomp_state, &error);	\
    if(PR_FAIL(error))					\
	{                                                                   \
		D(("Error line %d", __LINE__));					\
		return DECR_ILLEGALDATA;					\
	}                                                                   \
    bitbuf |= ((byTemp1<<8)|byTemp0) << (ULONG_BITS-16 - bitsleft);	\
    bitsleft += 16;						\
}

#define ENSURE_BITS_NO_SURE(n)							\
	while (bitsleft < (n)) {						\
	byTemp0 = NEXTBYTE(decomp_state, &error);	\
	byTemp1 = NEXTBYTE(decomp_state, &error);	\
    if(PR_FAIL(error))					\
{                                                                   \
	D(("Error line %d", __LINE__));					\
	return DECR_ILLEGALDATA;					\
}                                                                   \
    bitbuf |= ((byTemp1<<8)|byTemp0) << (ULONG_BITS-16 - bitsleft);	\
    bitsleft += 16; 						\
	}

#define PEEK_BITS(n)   (bitbuf >> (ULONG_BITS - (n)))
#define REMOVE_BITS(n) ((bitbuf <<= (n)), (bitsleft -= (n)))

#define READ_BITS(v,n) do {						\
  ENSURE_BITS(n);							\
  (v) = PEEK_BITS(n);							\
  REMOVE_BITS(n);							\
} while (0)


#define READ_BITS_NO_SURE(v,n) do {						\
  ENSURE_BITS_NO_SURE(n);							\
  (v) = PEEK_BITS(n);							\
  REMOVE_BITS(n);							\
} while (0)

/* Huffman macros */

#define TABLEBITS(tbl)   (LZX_##tbl##_TABLEBITS)
#define MAXSYMBOLS(tbl)  (LZX_##tbl##_MAXSYMBOLS)
#define SYMTABLE(tbl)    (LZX(tbl##_table))
#define LENTABLE(tbl)    (LZX(tbl##_len))

/* BUILD_TABLE(tablename) builds a huffman lookup table from code lengths.
 * In reality, it just calls make_decode_table() with the appropriate
 * values - they're all fixed by some #defines anyway, so there's no point
 * writing each call out in full by hand.
 */
#define BUILD_TABLE(tbl)						\
  if (make_decode_table(decomp_state,					\
    MAXSYMBOLS(tbl), TABLEBITS(tbl), LENTABLE(tbl), SYMTABLE(tbl)	\
  )) { D(("Error line %d", __LINE__));return DECR_ILLEGALDATA; }


/* READ_HUFFSYM(tablename, var) decodes one huffman symbol from the
 * bitstream using the stated table and puts it in var.
 */
#define READ_HUFFSYM(tbl,var) do {					\
  ENSURE_BITS(16);							\
  hufftbl = SYMTABLE(tbl);						\
  if ((i = hufftbl[PEEK_BITS(TABLEBITS(tbl))]) >= MAXSYMBOLS(tbl)) {	\
    j = 1 << (ULONG_BITS - TABLEBITS(tbl));				\
    do {								\
      j >>= 1; i <<= 1; i |= (bitbuf & j) ? 1 : 0;			\
      if (!j) { D(("Error line %d", __LINE__));return DECR_ILLEGALDATA; }	                        \
    } while ((i = hufftbl[i]) >= MAXSYMBOLS(tbl));			\
  }									\
  j = LENTABLE(tbl)[(var) = i];						\
  REMOVE_BITS(j);							\
} while (0)

#define READ_HUFFSYM_NO_SURE(tbl,var) do {					\
  ENSURE_BITS_NO_SURE(16);							\
  hufftbl = SYMTABLE(tbl);						\
  if ((i = hufftbl[PEEK_BITS(TABLEBITS(tbl))]) >= MAXSYMBOLS(tbl)) {	\
    j = 1 << (ULONG_BITS - TABLEBITS(tbl));				\
    do {								\
      j >>= 1; i <<= 1; i |= (bitbuf & j) ? 1 : 0;			\
      if (!j) { D(("Error line %d", __LINE__));return DECR_ILLEGALDATA; }	                        \
    } while ((i = hufftbl[i]) >= MAXSYMBOLS(tbl));			\
  }									\
  j = LENTABLE(tbl)[(var) = i];						\
  REMOVE_BITS(j);							\
} while (0)


/* READ_LENGTHS(tablename, first, last) reads in code lengths for symbols
 * first to last in the given table. The code lengths are stored in their
 * own special LZX way.
 */
#define READ_LENGTHS(tbl,first,last) do { \
  lb.bb = bitbuf; lb.bl = bitsleft; /*lb.ip = inpos;*/ \
  if (lzx_read_lens(decomp_state, LENTABLE(tbl),(first),(last),&lb)) { \
    D(("Error line %d", __LINE__));return DECR_ILLEGALDATA; \
  } \
  bitbuf = lb.bb; bitsleft = lb.bl; /*inpos = lb.ip;*/ \
} while (0)


/* make_decode_table(nsyms, nbits, length[], table[])
 *
 * This function was coded by David Tritscher. It builds a fast huffman
 * decoding table out of just a canonical huffman code lengths table.
 *
 * nsyms  = total number of symbols in this huffman tree.
 * nbits  = any symbols with a code length of nbits or less can be decoded
 *          in one lookup of the table.
 * length = A table to get code lengths from [0 to syms-1]
 * table  = The table to fill up with decoded symbols and pointers.
 *
 * Returns 0 for OK or 1 for error
 */

static tINT make_decode_table(LZXData *decomp_state, tDWORD nsyms, tDWORD nbits, tBYTE *length, tWORD *table) 
{
tWORD sym;
tDWORD leaf;
tBYTE bit_num = 1;
tDWORD fill;
tDWORD pos         = 0; /* the current position in the decode table */
tDWORD table_mask  = 1 << nbits;
tDWORD bit_mask    = table_mask >> 1; /* don't do 0 length codes */
tDWORD next_symbol = bit_mask; /* base of allocation for long codes */

  /* fill entries for codes short enough for a direct mapping */
	while (bit_num <= nbits) 
	{
		for (sym = 0; sym < nsyms; sym++) 
		{
			if (length[sym] == bit_num) 
			{
				leaf = pos;

				if((pos += bit_mask) > table_mask) 
					return 1; /* table overrun */

				/* fill all possible lookups of this symbol with the symbol itself */
				fill = bit_mask;
				while (fill-- > 0) 
					table[leaf++] = sym;
			}
		}
		bit_mask >>= 1;
		bit_num++;
	}

	/* if there are any codes longer than nbits */
	if (pos != table_mask) 
	{
		/* clear the remainder of the table */
		for (sym = pos; sym < table_mask; sym++) 
			table[sym] = 0;

		/* give ourselves room for codes to grow by up to 16 more bits */
		pos <<= 16;
		table_mask <<= 16;
		bit_mask = 1 << 15;

		while (bit_num <= 16) 
		{
			for (sym = 0; sym < nsyms; sym++) 
			{
				if (length[sym] == bit_num) 
				{
					leaf = pos >> 16;
					for (fill = 0; fill < bit_num - nbits; fill++) 
					{
						/* if this path hasn't been taken yet, 'allocate' two entries */
						if (table[leaf] == 0) 
						{
							table[(next_symbol << 1)] = 0;
							table[(next_symbol << 1) + 1] = 0;
							table[leaf] = next_symbol++;
						}
						/* follow the path and select either left or right for next bit */
						leaf = table[leaf] << 1;
						if ((pos >> (15-fill)) & 1)
							leaf++;
					}
					table[leaf] = sym;

					if ((pos += bit_mask) > table_mask)
						return 1; /* table overflow */
				}
			}
			bit_mask >>= 1;
			bit_num++;
		}
	}

	/* full table? */
	if (pos == table_mask)
		return 0;
	/* either erroneous table, or all elements are 0 - let's find out. */
	for (sym = 0; sym < nsyms; sym++) 
		if (length[sym]) 
			return 1;
	return 0;
}

static tINT lzx_read_lens(LZXData *decomp_state, tBYTE *lens, tDWORD first, tDWORD last, lzx_bits *lb) 
{
tDWORD i,j, x,y;
tINT z;
tDWORD bitbuf = lb->bb;
tINT bitsleft = lb->bl;
//tBYTE *inpos = lb->ip;
tWORD *hufftbl;
tBYTE byTemp0, byTemp1;
tERROR error;

	error = errOK;
	for (x = 0; x < 20; x++) 
	{
		READ_BITS_NO_SURE(y, 4);
		LENTABLE(PRETREE)[x] = y;
	}
	BUILD_TABLE(PRETREE);

	for (x = first; x < last; ) 
	{
		READ_HUFFSYM_NO_SURE(PRETREE, z);
		if (z == 17) 
		{
			READ_BITS_NO_SURE(y, 4); y += 4;
			while (y--) 
				lens[x++] = 0;
		}
		else if (z == 18) 
		{
			READ_BITS_NO_SURE(y, 5); y += 20;
			while (y--) 
				lens[x++] = 0;
		}
		else if (z == 19) 
		{
			READ_BITS_NO_SURE(y, 1); y += 4;
			READ_HUFFSYM_NO_SURE(PRETREE, z);
			z = lens[x] - z; 
			if (z < 0) 
				z += 17;
			while (y--)
				lens[x++] = z;
		}
		else 
		{
			z = lens[x] - z; if (z < 0) z += 17;
			lens[x++] = z;
		}
	}

	lb->bb = bitbuf;
	lb->bl = bitsleft;
//	lb->ip = inpos;
	return 0;
}

static tINT CAB_LZXdecompress(LZXData *decomp_state, tINT outlen) 
{
    //tBYTE *inpos  = CAB(inbuf);
    //tBYTE *endinp = inpos + inlen;
    tBYTE *window = LZX(window);
    tBYTE *runsrc, *rundest;
    tWORD *hufftbl; /* used in READ_HUFFSYM macro as chosen decoding table */

    tDWORD window_posn = LZX(window_posn);
    tDWORD window_size = LZX(window_size);
    tDWORD R0 = LZX(R0);
    tDWORD R1 = LZX(R1);
    tDWORD R2 = LZX(R2);
    tBYTE byRead0, byRead1, byRead2, byRead3;
    tBYTE byTemp0, byTemp1;
    tDWORD bitbuf;
    tINT bitsleft;
    tDWORD match_offset, i,j,k; /* ijk used in READ_HUFFSYM macro */
    lzx_bits lb; /* used in READ_LENGTHS macro */
    tERROR error;

	error = errOK;

	tINT togo = outlen, this_run, main_element, aligned_bits;
	tINT match_length, length_footer, extra, verbatim_bits;

	INIT_BITSTREAM;

	/* read header if necessary */
	if (!LZX(header_read)) 
	{
		i = j = 0;
		READ_BITS(k, 1); 
		if (k) 
		{ 
			READ_BITS(i,16); 
			READ_BITS(j,16); 
		}
		LZX(intel_filesize) = (i << 16) | j; /* or 0 if not encoded */
		LZX(header_read) = 1;
	}

	/* main decoding loop */
	while (togo > 0) 
	{
		/* last block finished, new block expected */
		if (LZX(block_remaining) == 0) 
		{
			if (LZX(block_type) == LZX_BLOCKTYPE_UNCOMPRESSED) 
			{
				if (LZX(block_length) & 1) 
				{
//					inpos++; /* realign bitstream to word */
					NEXTBYTE(decomp_state, &error);
				}
				INIT_BITSTREAM;
			}

			READ_BITS(LZX(block_type), 3);
			READ_BITS(i, 16);
			READ_BITS(j, 8);
			LZX(block_remaining) = LZX(block_length) = (i << 8) | j;

			switch (LZX(block_type)) 
			{
				case LZX_BLOCKTYPE_ALIGNED:
					for (i = 0; i < 8; i++)
					{
						READ_BITS(j, 3); 
						LENTABLE(ALIGNED)[i] = j; 
					}
					BUILD_TABLE(ALIGNED);
				/* rest of aligned header is same as verbatim */

				case LZX_BLOCKTYPE_VERBATIM:
					READ_LENGTHS(MAINTREE, 0, 256);
					READ_LENGTHS(MAINTREE, 256, LZX(main_elements));
					BUILD_TABLE(MAINTREE);
					if (LENTABLE(MAINTREE)[0xE8] != 0) 
						LZX(intel_started) = 1;

					READ_LENGTHS(LENGTH, 0, LZX_NUM_SECONDARY_LENGTHS);
					BUILD_TABLE(LENGTH);
					break;

				case LZX_BLOCKTYPE_UNCOMPRESSED:
					LZX(intel_started) = 1; /* because we can't assume otherwise */
					ENSURE_BITS(16); /* get up to 16 pad bits into the buffer */
					if (bitsleft > 16) 
					{
//						inpos -= 2; /* and align the bitstream! */
						GetBack2Bytes(decomp_state);
					}
					//R0 = inpos[0]|(inpos[1]<<8)|(inpos[2]<<16)|(inpos[3]<<24);
					//R0 = NEXTBYTE(decomp_state, &error)|(NEXTBYTE(decomp_state, &error)<<8)|
					//	(NEXTBYTE(decomp_state, &error)<<16)|(NEXTBYTE(decomp_state, &error)<<24);
                    byRead0 = NEXTBYTE(decomp_state, &error);
                    byRead1 = NEXTBYTE(decomp_state, &error);
                    byRead2 = NEXTBYTE(decomp_state, &error);
                    byRead3 = NEXTBYTE(decomp_state, &error);
					R0 = (byRead0)|(byRead1<<8)|(byRead2<<16)|(byRead3<<24);
                    
					//inpos+=4;
//					R1 = inpos[0]|(inpos[1]<<8)|(inpos[2]<<16)|(inpos[3]<<24);
					//R1 = NEXTBYTE(decomp_state, &error)|(NEXTBYTE(decomp_state, &error)<<8)|
					//	(NEXTBYTE(decomp_state, &error)<<16)|(NEXTBYTE(decomp_state, &error)<<24);
                    byRead0 = NEXTBYTE(decomp_state, &error);
                    byRead1 = NEXTBYTE(decomp_state, &error);
                    byRead2 = NEXTBYTE(decomp_state, &error);
                    byRead3 = NEXTBYTE(decomp_state, &error);
					R1 = (byRead0)|(byRead1<<8)|(byRead2<<16)|(byRead3<<24);

					//inpos+=4;
//					R2 = inpos[0]|(inpos[1]<<8)|(inpos[2]<<16)|(inpos[3]<<24);
					//R2 = NEXTBYTE(decomp_state, &error)|(NEXTBYTE(decomp_state, &error)<<8)|
					//	(NEXTBYTE(decomp_state, &error)<<16)|(NEXTBYTE(decomp_state, &error)<<24);
                    byRead0 = NEXTBYTE(decomp_state, &error);
                    byRead1 = NEXTBYTE(decomp_state, &error);
                    byRead2 = NEXTBYTE(decomp_state, &error);
                    byRead3 = NEXTBYTE(decomp_state, &error);
					R2 = (byRead0)|(byRead1<<8)|(byRead2<<16)|(byRead3<<24);

					//inpos+=4;
					break;

				default:
					D(("Error Unknown Block Type %d ",LZX(block_type)));
					return DECR_ILLEGALDATA;
			}
		}

		/* buffer exhaustion check */
//		if (inpos > endinp) 
//		{
      /* it's possible to have a file where the next run is less than
       * 16 bits in size. In this case, the READ_HUFFSYM() macro used
       * in building the tables will exhaust the buffer, so we should
       * allow for this, but not allow those accidentally read bits to
       * be used (so we check that there are at least 16 bits
       * remaining - in this boundary case they aren't really part of
       * the compressed data)
       */
//			if (inpos > (endinp+2) || bitsleft < 16) 
//			{
//				D(("Error line %d", __LINE__));
//				return DECR_ILLEGALDATA;
//			}
//		}

		while ((this_run = LZX(block_remaining)) > 0 && togo > 0) 
		{
			if (this_run > togo) 
				this_run = togo;
			togo -= this_run;
			LZX(block_remaining) -= this_run;

			/* apply 2^x-1 mask */
			window_posn &= window_size - 1;
			/* runs can't straddle the window wraparound */
			if ((window_posn + this_run) > window_size)
			{
				D(("Error line %d", __LINE__));
				return DECR_DATAFORMAT;
			}

			switch (LZX(block_type)) 
			{

				case LZX_BLOCKTYPE_VERBATIM:
					while (this_run > 0) 
					{
						READ_HUFFSYM(MAINTREE, main_element);

						if (main_element < LZX_NUM_CHARS) 
						{
							/* literal: 0 to LZX_NUM_CHARS-1 */
							window[window_posn++] = main_element;
							this_run--;
						}
						else 
						{
							/* match: LZX_NUM_CHARS + ((slot<<3) | length_header (3 bits)) */
							main_element -= LZX_NUM_CHARS;

							match_length = main_element & LZX_NUM_PRIMARY_LENGTHS;
							if (match_length == LZX_NUM_PRIMARY_LENGTHS) 
							{
								READ_HUFFSYM(LENGTH, length_footer);
								match_length += length_footer;
							}
							match_length += LZX_MIN_MATCH;

							match_offset = main_element >> 3;

							if (match_offset > 2) 
							{
								/* not repeated offset */
								if (match_offset != 3) 
								{
									extra = LZX(extra_bits[match_offset]);
									READ_BITS(verbatim_bits, extra);
									match_offset = LZX(position_base[match_offset]) - 2 + verbatim_bits;
								}
								else 
								{
									match_offset = 1;
								}

								/* update repeated offset LRU queue */
								R2 = R1; R1 = R0; R0 = match_offset;
							}
							else if (match_offset == 0) 
							{
								match_offset = R0;
							}
							else if (match_offset == 1) 
							{
								match_offset = R1;
								R1 = R0; R0 = match_offset;
							}
							else /* match_offset == 2 */ 
							{
								match_offset = R2;
								R2 = R0; R0 = match_offset;
							}

							rundest = window + window_posn;
							runsrc  = rundest - match_offset;
							window_posn += match_length;
							this_run -= match_length;

							/* copy any wrapped around source data */
							while ((runsrc < window) && (match_length-- > 0)) 
							{
								*rundest++ = *(runsrc + window_size); 
								runsrc++;
							}
							/* copy match data - no worries about destination wraps */
							while (match_length-- > 0) 
								*rundest++ = *runsrc++;

						}
					}
					break;

				case LZX_BLOCKTYPE_ALIGNED:
					while (this_run > 0) 
					{
						READ_HUFFSYM(MAINTREE, main_element);
/* do {					
  ENSURE_BITS(16);							
  hufftbl = SYMTABLE(MAINTREE);						
  if ((i = hufftbl[PEEK_BITS(TABLEBITS(MAINTREE))]) >= MAXSYMBOLS(MAINTREE)) {	
    j = 1 << (ULONG_BITS - TABLEBITS(MAINTREE));
    do {								
      j >>= 1; i <<= 1; i |= (bitbuf & j) ? 1 : 0;			
      if (!j) { D(("Error line %d", __LINE__));return DECR_ILLEGALDATA; }	                        
    } while ((i = hufftbl[i]) >= MAXSYMBOLS(MAINTREE));			
  }									
  j = LENTABLE(MAINTREE)[(main_element) = i];						
  REMOVE_BITS(j);							
} while (0);*/

  
						if (main_element < LZX_NUM_CHARS) 
						{
							/* literal: 0 to LZX_NUM_CHARS-1 */
							window[window_posn++] = main_element;
							this_run--;
						}
						else 
						{
							/* match: LZX_NUM_CHARS + ((slot<<3) | length_header (3 bits)) */
							main_element -= LZX_NUM_CHARS;
  
							match_length = main_element & LZX_NUM_PRIMARY_LENGTHS;
							if (match_length == LZX_NUM_PRIMARY_LENGTHS) 
							{
								READ_HUFFSYM(LENGTH, length_footer);
								match_length += length_footer;
							}
							match_length += LZX_MIN_MATCH;
  
							match_offset = main_element >> 3;
  
							if (match_offset > 2) 
							{
								/* not repeated offset */
								extra = LZX(extra_bits[match_offset]);
								match_offset = LZX(position_base[match_offset]) - 2;
								if (extra > 3) 
								{
									/* verbatim and aligned bits */
									extra -= 3;
									READ_BITS(verbatim_bits, extra);
									match_offset += (verbatim_bits << 3);
									READ_HUFFSYM(ALIGNED, aligned_bits);
									match_offset += aligned_bits;
								}
								else if (extra == 3) 
								{
									/* aligned bits only */
									READ_HUFFSYM(ALIGNED, aligned_bits);
									match_offset += aligned_bits;
								}
								else if (extra > 0)  /* extra==1, extra==2 */
								{
									/* verbatim bits only */
									READ_BITS(verbatim_bits, extra);
									match_offset += verbatim_bits;
								}
								else /* extra == 0 */ 
								{
									/* ??? */
									match_offset = 1;
								}
  
								/* update repeated offset LRU queue */
								R2 = R1; 
								R1 = R0; 
								R0 = match_offset;
							}
							else if (match_offset == 0) 
							{
								match_offset = R0;
							}
							else if (match_offset == 1) 
							{
								match_offset = R1;
								R1 = R0; R0 = match_offset;
							}
							else /* match_offset == 2 */ 
							{
								match_offset = R2;
								R2 = R0; 
								R0 = match_offset;
							}

							rundest = window + window_posn;
							runsrc  = rundest - match_offset;
							window_posn += match_length;
							this_run -= match_length;

							/* copy any wrapped around source data */
							while ((runsrc < window) && (match_length-- > 0)) 
							{
								*rundest++ = *(runsrc + window_size); 
								runsrc++;
							}
							/* copy match data - no worries about destination wraps */
							while (match_length-- > 0) 
								*rundest++ = *runsrc++;
						}
					}
					break;

				case LZX_BLOCKTYPE_UNCOMPRESSED:
//					if ((inpos + this_run) > endinp) 
//					{
//						D(("Error line %d", __LINE__));
//						return DECR_ILLEGALDATA;
//					}
					decomp_state->dwRead += this_run;
					error = CALL_SeqIO_Read(decomp_state->hInputSeqIo, NULL, window + window_posn, this_run);
					if(PR_FAIL(error))
					{
						D(("Error line %d", __LINE__));
						return DECR_ILLEGALDATA;
					}
//					memcpy(window + window_posn, inpos, (size_t) this_run);
//					inpos += this_run; 
					window_posn += this_run;
					break;

				default:
					D(("Error line %d", __LINE__));
					return DECR_ILLEGALDATA; /* might as well */
			}

		}
	}

	if (togo != 0) 
	{
		D(("Error line %d", __LINE__));
		return DECR_ILLEGALDATA;
	}
	memcpy(CAB(outbuf), window + ((!window_posn) ? window_size : window_posn) - outlen, (size_t) outlen);

	LZX(window_posn) = window_posn;
	LZX(R0) = R0;
	LZX(R1) = R1;
	LZX(R2) = R2;

	/* intel E8 decoding */
	if ((LZX(frames_read)++ < 32768) && LZX(intel_filesize) != 0) 
	{
		if (outlen <= 6 || !LZX(intel_started)) 
		{
			LZX(intel_curpos) += outlen;
		}
		else 
		{
		tBYTE *data    = CAB(outbuf);
		tBYTE *dataend = data + outlen - 10;
		tLONG curpos    = LZX(intel_curpos);
		tLONG filesize  = LZX(intel_filesize);
		tLONG abs_off, rel_off;

			LZX(intel_curpos) = curpos + outlen;

			while (data < dataend) 
			{
				if (*data++ != 0xE8) 
				{
					curpos++; 
					continue; 
				}
				abs_off = data[0] | (data[1]<<8) | (data[2]<<16) | (data[3]<<24);
				if ((abs_off >= -curpos) && (abs_off < filesize)) 
				{
					rel_off = (abs_off >= 0) ? abs_off - curpos : abs_off + filesize;
					data[0] = (tBYTE) rel_off;
					data[1] = (tBYTE) (rel_off >> 8);
					data[2] = (tBYTE) (rel_off >> 16);
					data[3] = (tBYTE) (rel_off >> 24);
				}
				data += 4;
				curpos += 5;
			}
		}
	}
	return errOK;
}




tERROR LZXInit(LZXData* lcabstate, tBYTE* lpWindow, tDWORD dwWndSize)
{
//	memset((tBYTE*)&lcabstate->lzx, 0, sizeof(LZXstate));
	return CAB_LZXinit(lcabstate, lpWindow, dwWndSize);
}


tINT LZXDecompress(LZXData* decomp_state, tBYTE* lpOutBuffer, tDWORD cbOutBufferSize , tINT Flag)
{
	decomp_state->bDebugFlag = Flag;
//	decomp_state->inbuf = lpInBuffer;
	decomp_state->outbuf = lpOutBuffer;
    decomp_state->err = CAB_LZXdecompress(decomp_state, cbOutBufferSize);
	if( decomp_state->err == 0 )
	{
		return cbOutBufferSize;
	}
	return 0;
}

