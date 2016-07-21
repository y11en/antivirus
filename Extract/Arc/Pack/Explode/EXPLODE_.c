#include <Prague/prague.h>

#include "explodei.h"

#include <memory.h>
#include <stdio.h>

#ifdef _DEBUG
  #define OUT_DEBUG(a) printf(a)
#else
  #define OUT_DEBUG(a) 
#endif


#ifdef _DEBUG
static tWORD hufts;         /* track memory usage */
#endif

#pragma pack(1)
typedef struct _tag_huft
{
	tBYTE e;               /* number of extra bits or operation */
	tBYTE b;               /* number of bits in this code or subcode */
	union
	{
		tDWORD n;            /* literal, length base, or distance base */
		struct _tag_huft  *t;     /* pointer to next level of table */
	} v;
}huft;
#pragma pack()


static tINT  get_tree(ExplodeData*, tWORD *, tWORD);
static tINT  explode_lit(ExplodeData*,huft*,huft*,huft*, tINT,tINT,tINT,tINT,tINT);
static tINT  huft_free(ExplodeData*, huft* t);
static tINT  huft_build_(ExplodeData*, tWORD * b,tWORD n,tWORD s,tWORD * d,tWORD * e,huft** t,tINT* m);
tVOID Delete(ExplodeData* data, tBYTE * lpdata);
tBYTE* New(ExplodeData* data, tDWORD dwBytes);


/* The implode algorithm uses a sliding 4K or 8K tBYTE window on the
   uncompressed stream to find repeated tBYTE strings.  This is implemented
   here as a circular buffer.  The index is updated simply by incrementing
   and then and'ing with 0x0fff (4K-1) or 0x1fff (8K-1).  Here, the 32K
   buffer of inflate is used, and it works just as well to always have
   a 32K circular buffer, so the index is anded with 0x7fff.  This is
   done to allow the window to also be used as the output buffer. */
/* This must be supplied in an external module useable like "tBYTE slide[8192];"
   or "tBYTE *slide;", where the latter would be malloc'ed.  In unzip, slide[]
   is actually a 32K area for use by inflate, which uses a 32K sliding window.
 */

/* Tables for length and distance */

static tWORD  cplen[] = {2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17,
                18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34,
                35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51,
                52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66};

static tWORD  extra[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                8};
static tWORD  cpdist4[] = {1, 65, 129, 193, 257, 321, 385, 449, 513, 577, 641, 705,
                769, 833, 897, 961, 1025, 1089, 1153, 1217, 1281, 1345, 1409, 1473,
                1537, 1601, 1665, 1729, 1793, 1857, 1921, 1985, 2049, 2113, 2177,
                2241, 2305, 2369, 2433, 2497, 2561, 2625, 2689, 2753, 2817, 2881,
                2945, 3009, 3073, 3137, 3201, 3265, 3329, 3393, 3457, 3521, 3585,
                3649, 3713, 3777, 3841, 3905, 3969, 4033};
static tWORD  cpdist8[] = {1, 129, 257, 385, 513, 641, 769, 897, 1025, 1153, 1281,
                1409, 1537, 1665, 1793, 1921, 2049, 2177, 2305, 2433, 2561, 2689,
                2817, 2945, 3073, 3201, 3329, 3457, 3585, 3713, 3841, 3969, 4097,
                4225, 4353, 4481, 4609, 4737, 4865, 4993, 5121, 5249, 5377, 5505,
                5633, 5761, 5889, 6017, 6145, 6273, 6401, 6529, 6657, 6785, 6913,
                7041, 7169, 7297, 7425, 7553, 7681, 7809, 7937, 8065};

static tWORD mask_bits[] = {
	0x0000,
	0x0001, 0x0003, 0x0007, 0x000f, 0x001f, 0x003f, 0x007f, 0x00ff,
	0x01ff, 0x03ff, 0x07ff, 0x0fff, 0x1fff, 0x3fff, 0x7fff, 0xffff
};


#define NEEDBITS(n) {\
while(k<(n))\
{\
b|=((tDWORD)iReadByte(data->hInputIO)<<k);\
k+=8;\
}\
}

#define DUMPBITS(n) {b>>=(n);k-=(n);}


tBYTE iReadByte(hSEQ_IO io)
{
tBYTE ret;
	CALL_SeqIO_ReadByte(io, &ret);
	return ret;
}


static tINT  get_tree(ExplodeData* data, tWORD *l, tWORD n)
//l;            /* bit lengths */
//n;             /* number expected */
/* Get the bit lengths for a code representation from the compressed
   stream.  If get_tree() returns 4, then there is an error in the data.
   Otherwise zero is returned. */
{
tWORD i;           /* bytes remaining in list */
tWORD k;           /* lengths entered */
tWORD j;           /* number of codes */
tWORD b;           /* bit length for those codes */
tWORD  bytebuf;


	bytebuf = 0;
  /* get bit lengths */
	CALL_SeqIO_ReadByte(data->hInputIO,(tBYTE*)&bytebuf);
	i = bytebuf + 1;                      /* length/count pairs to read */
	k = 0;                                /* next code */
	do
	{
		CALL_SeqIO_ReadByte(data->hInputIO,(tBYTE*)&bytebuf);
		b = ((j = bytebuf) & 0xf) + 1;      /* bits in code (1..16) */
		j = ((j & 0xf0) >> 4) + 1;          /* codes with those bits (1..16) */
		if (k + j > n)
			return 4;                         /* don't overflow l[] */
		do
		{
			l[k++] = b;
		} while (--j);
	} while (--i);
	return k != n ? 4 : 0;                /* should have read n of them */
}



/* Decompress the imploded data using coded literals and an 8K sliding
   window. */

static tINT  explode_lit(ExplodeData* data,huft* tb,huft* tl,huft* td,tINT bb,tINT bl,tINT bd,tINT l,tINT lit)
{
tLONG s;            /* bytes to decompress */
tDWORD e;  /* table entry flag/number of extra bits */
tDWORD n, d;        /* length and index for copy */
tDWORD w;           /* current window position */
huft* t;       /* pointer to table entry */
tWORD mb, ml, md;  /* masks for bb, bl, and bd bits */
tDWORD b;     /* bit buffer */
tDWORD k;  /* number of bits in bit buffer */
tWORD u;           /* true if unflushed */
tQWORD qwSize;

  /* explode the coded data */
	b = k = w = 0;                /* initialize bit buffer, window */
	u = 1;                        /* buffer unflushed */
	if(lit)
		mb = mask_bits[bb];           /* precompute masks for speed */
	ml = mask_bits[bl];
	md = mask_bits[bd];
	s = (tLONG)data->Zip_ucsize;
	while (s > 0 )                 /* do until data->Zip_ucsize bytes uncompressed */
	{
		NEEDBITS(1)
		if (b & 1)                  /* then literal--decode it */
		{
			DUMPBITS(1)
			s--;
			if(lit)
			{
				NEEDBITS((tWORD)bb)    /* get coded literal */
				if ((e = (t = tb + ((~(tWORD)b) & mb))->e) > 16)
				{
					do
					{
						if (e == (tWORD)99)
							return 1;
						DUMPBITS(t->b)
						e -= 16;
						NEEDBITS(e)
					} while ((e = (t = t->v.t + ((~(tWORD)b) & mask_bits[e]))->e) > 16);
				}
				DUMPBITS(t->b)
				data->slide[w++] = (tBYTE)t->v.n;
				if (w == WSIZE)
				{
					CALL_SeqIO_GetSize(data->hOutputIO, &qwSize, IO_SIZE_TYPE_EXPLICIT);
					CALL_SeqIO_SetSize(data->hOutputIO, qwSize+w);
					CALL_SeqIO_Write(data->hOutputIO,NULL, data->slide,w);
//					flush(w);
					w = u = 0;
				}
			}
			else
			{
				NEEDBITS(8)
				data->slide[w++] = (tBYTE)b;
				if (w == WSIZE)
				{
					CALL_SeqIO_GetSize(data->hOutputIO, &qwSize, IO_SIZE_TYPE_EXPLICIT);
					CALL_SeqIO_SetSize(data->hOutputIO, qwSize+w);
					CALL_SeqIO_Write(data->hOutputIO,NULL,data->slide,w);
//					flush(w);
					w = u = 0;
				}
				DUMPBITS(8)
			}
		}
		else                        /* else distance/length */
		{
			DUMPBITS(1)
			if(l)
			{
				NEEDBITS(7)               /* get distance low bits */
				d = (tWORD)b & 0x7f;
				DUMPBITS(7)
			}
			else
			{
				NEEDBITS(6)               /* get distance low bits */
				d = (tWORD)b & 0x3f;
				DUMPBITS(6)
			}

			NEEDBITS((tWORD)bd)    /* get coded distance high bits */
			if ((e = (t = td + ((~(tWORD)b) & md))->e) > 16)
			{
				do
				{
					if (e == 99)
						return 1;
					DUMPBITS(t->b)
					e -= 16;
					NEEDBITS(e)
				} while ((e = (t = t->v.t + ((~(tWORD)b) & mask_bits[e]))->e) > 16);
			}
			DUMPBITS(t->b)
			d = w - d - t->v.n;       /* construct offset */
			NEEDBITS((tWORD)bl)    /* get coded length */
			if ((e = (t = tl + ((~(tWORD)b) & ml))->e) > 16)
			{
				do
				{
					if (e == 99)
						return 1;
					DUMPBITS(t->b)
					e -= 16;
					NEEDBITS(e)
				} while ((e = (t = t->v.t + ((~(tWORD)b) & mask_bits[e]))->e) > 16 );
			}
			DUMPBITS(t->b)
			n = t->v.n;
			if (e)                    /* get length extra bits */
			{
				NEEDBITS(8)
				n += (tWORD)b & 0xff;
				DUMPBITS(8)
			}

			/* do the copy */
			s -= n;
			do
			{
				n -= (e = (e = WSIZE - ((d &= WSIZE-1) > w ? d : w)) > n ? n : e);
				if (u && w <= d)
				{
					memset(data->slide + w, 0, e);
					w += e;
					d += e;
				}
				else if ((tWORD)(w - d) >= (tWORD)e)       /* (this test assumes tWORD comparison) */
				{
					memcpy(data->slide + w, data->slide + d, e);
					w += e;
					d += e;
				}
				else                  /* do it slow to avoid memcpy() overlap */
				{
					do
					{
						data->slide[w++] = data->slide[d++];
					} while (--e);
				}
				if (w == WSIZE)
				{
					CALL_SeqIO_GetSize(data->hOutputIO, &qwSize, IO_SIZE_TYPE_EXPLICIT);
					CALL_SeqIO_SetSize(data->hOutputIO, qwSize+w);
					
					CALL_SeqIO_Write(data->hOutputIO,NULL, data->slide,w);
//					flush(w);
					w = u = 0;
				}
			} while (n && !data->zip_corrupt );
		}
	}

	/* flush out data->slide */
	CALL_SeqIO_GetSize(data->hOutputIO, &qwSize, IO_SIZE_TYPE_EXPLICIT);
	CALL_SeqIO_SetSize(data->hOutputIO, qwSize+w);
	CALL_SeqIO_Write(data->hOutputIO,NULL, data->slide,w);
//	flush(w);
//	return data->Zip_csize ? 5 : 0;         /* should have read data->Zip_csize bytes */
	return (int)qwSize+w;
}



// Explode an imploded compressed stream.  Based on the general purpose
//   bit flag, decide on coded or uncoded literals, and an 8K or 4K sliding
//   window.  Construct the literal (if any), length, and distance codes and
//   the tables needed to decode them (using huft_build_() from inflate.c),
//   and call the appropriate routine for the type of data in the remainder
//   of the stream.  The four routines are NEARly identical, differing only
//   in whether the literal is decoded or simply read in, and in how many
//   bits are read in, uncoded, for the low distance bits. 

tDWORD  explode(ExplodeData* data)
{
tDWORD r=1;           // return codes 
huft  *tb;      // literal code table 
huft  *tl;      // length code table 
huft  *td;      // distance code table 
tINT bb;               // bits for tb 
tINT bl;               // bits for tl 
tINT bd;               // bits for td 
tWORD l[256];      // bit lengths for codes 



// Tune base table sizes.  Note: I thought that to truly optimize speed,
//         I would have to select different bl, bd, and bb values for different
//     compressed file sizes.  I was suprised to find out the the values of
//     7, 7, and 9 worked best over a very wide range of sizes, except that
//     bd = 8 worked marginally better for large compressed sizes. 
	bl = 7;
	bd = data->Zip_csize > 200000L ? 8 : 7;


  // With literal tree--minimum match length is 3 
#ifdef _DEBUG
	hufts = 0;                    // initialze huft's malloc'ed 
#endif
	if (data->general_purpose_bit_flag & 4)
	{
		bb = 9;                     // base table size for literals 
		if ((r = get_tree(data, l, 256)) != 0)
			goto ret;
		if ((r = huft_build_(data, l, 256, 256, NULL, NULL, &tb, &bb)) != 0)
		{
			if (r == 1)
				huft_free(data, tb);
			goto ret;
		}
		if ((r = get_tree(data, l, 64)) != 0)
			goto ret;
		if ((r = huft_build_(data, l, 64, 0, cplen+1, extra, &tl, &bl)) != 0)
		{
			if (r == 1)
				huft_free(data, tl);
			huft_free(data, tb);
			goto ret;
		}
		if ((r = get_tree(data, l, 64)) != 0)
			goto ret;
		if (data->general_purpose_bit_flag & 2)      // true if 8K 
		{
			if ((r = huft_build_(data, l, 64, 0, cpdist8, extra, &td, &bd)) != 0)
			{
				if (r == 1)
					huft_free(data, td);
				huft_free(data, tl);
				huft_free(data, tb);
				goto ret;
			}
			r = explode_lit(data, tb, tl, td, bb, bl, bd, 1, 1);
		}
		else                                        // else 4K 
		{
			if ((r = huft_build_(data, l, 64, 0, cpdist4, extra, &td, &bd)) != 0)
			{
				if (r == 1)
					huft_free(data, td);
				huft_free(data, tl);
				huft_free(data, tb);
				goto ret;
			}
			r = explode_lit(data, tb, tl, td, bb, bl, bd, 0, 1);
		}
		huft_free(data, td);
		huft_free(data, tl);
		huft_free(data, tb);
	}
	else
  // No literal tree--minimum match length is 2 
	{
		if ((r = get_tree(data, l, 64)) != 0)
			goto ret;
		if ((r = huft_build_(data, l, 64, 0, cplen, extra, &tl, &bl)) != 0)
		{
			if (r == 1)
				huft_free(data, tl);
			goto ret;
		}
		if ((r = get_tree(data, l, 64)) != 0)
			goto ret;
		if (data->general_purpose_bit_flag & 2)      // true if 8K 
		{
			if ((r = huft_build_(data, l, 64, 0, cpdist8, extra, &td, &bd)) != 0)
			{
				if (r == 1)
					huft_free(data, td);
				huft_free(data, tl);
				goto ret;
			}
			r = explode_lit(data, NULL, tl, td, 0, bl, bd, 1, 0);
		}
		else                                        // else 4K 
		{
			if ((r = huft_build_(data, l, 64, 0, cpdist4, extra, &td, &bd)) != 0)
			{
				if (r == 1)
					huft_free(data, td);
				huft_free(data, tl);
				goto ret;
			}
			r = explode_lit(data, NULL, tl, td, 0, bl, bd, 0, 0);
		}
		huft_free(data, td);
		huft_free(data, tl);
	}
#ifdef _DEBUG
//	printf( "<%u > \n", hufts);
#endif // _DEBUG
ret:
	if(r)
		r=(tDWORD)data->Zip_ucsize;
	return r;
}

#define BMAX 16         /* maximum bit length of any code (16 for explode) */
#define N_MAX 288       /* maximum number of codes in any set */



// b code lengths in bits (all assumed <= BMAX) 
// n number of codes (assumed <= N_MAX) 
// s number of simple-valued codes (0..s-1) 
// d list of base values for non-simple codes 
// e list of extra bits for non-simple codes 
// t result: starting table 
// m maximum lookup bits, returns actual 
// Given a list of code lengths and a maximum table size, make a set of
//   tables to decode that set of codes.  Return zero on success, one if
//   the given code set is incomplete (the tables are still built in this
//   case), two if the input is invalid (all zero length codes or an
//   oversubscribed set of lengths), and three if not enough memory. 

static tINT  huft_build_(ExplodeData* data,tWORD* b,tWORD n,tWORD s,tWORD* d,tWORD* e,huft** t,tINT* m)
{
tWORD c[BMAX+1];           // bit length count table 
tWORD x[BMAX+1];           // bit offsets, then code stack 
tWORD v[N_MAX];            // values in order of bit length 

huft  *u[BMAX];         // table stack 
//static struct huft  *u;

tWORD  *p;         // pointer into c[], b[], or v[] 
huft  *q;      // points to current table 
huft r;                // table entry for structure assignment 
tWORD  *xp;                 // pointer into x 
tWORD a;                   // counter for codes of length k 
tWORD f;                   // i repeats in table every f entries 
tINT g;                        // maximum code length 
tINT h;                        // table level 
tWORD i;          // counter, current code 
tWORD j;          // counter 
tINT k;               // number of bits in current code 
tINT l;                        // bits per table (returned in m) 
tINT w;               // bits before this table == (l * h) 
tSHORT y;                        // number of dummy codes added 
tWORD z;                   // number of entries in current table 
tINT RET=2;

#ifdef _DEBUG
//	printf("huft_build_( %d, %d, %d, %X, %X, %X, %d )\n",*b,n,s,d,e,t,*m);
#endif

  // Generate counts for each bit length 
	if( n > N_MAX )
	{
		data->zip_corrupt=1;
		RET=2;
#ifdef _DEBUG
		OUT_DEBUG("Corrupt\n");
//		printf("n > N_MAX %x",n);
#endif
		goto ret;
	}
	memset((tBYTE *)c, 0, (BMAX+1)*sizeof(tWORD));
	p = b;
	i = n;
	do
	{
	tWORD d=*p++;
		if( d > BMAX )
		{
			data->zip_corrupt=1;
			RET=2;
#ifdef _DEBUG
			OUT_DEBUG("Corrupt\n");
//			printf("d > BMAX %x",d);
#endif
			goto ret;
		}
		c[d]++;                  // assume all entries <= BMAX 
	} while (--i);

	if (c[0] == n)                // null input--all zero length codes
	{
//  It's OK!
		*t = (huft  *)NULL;
		*m = 0;
		 RET=0;
#ifdef _DEBUG
//		printf("c[0] == n %x\n",n);
#endif
		goto ret;
	}


  // Find minimum and maximum length, bound *m by those 
	l = *m;
	for (j = 1; j <= BMAX; j++)
		if (c[j])
			break;
	k = j;                        // minimum code length 
	if ((tWORD)l < j)
		l = j;
	for (i = BMAX; i; i--)
		if (c[i])
			break;
	g = i;                        // maximum code length 
	if ((tWORD)l > i)
		l = i;
	*m = l;


  // Adjust last length count to fill out codes, if needed 
	for (y = 1 << j; j < i; j++, y <<= 1)
	{
		if ((y -= c[j]) < 0)
		{
//			data->zip_corrupt=1;
			RET=2;
#ifdef _DEBUG
			printf("(y -= c[j]) < 0  %d\n",y);
#endif
			goto ret;
		}
	}

	if ((y -= c[i]) < 0)
	{
//		data->zip_corrupt=1;
		RET=2;
#ifdef _DEBUG
		printf("(y -= c[j]) < 0  %d\n",y);
#endif
		goto ret;
	}

	c[i] += y;


	// Generate starting offsets into the value table for each length 
	x[1] = j = 0;
	p = c + 1;  xp = x + 2;
	while (--i)                 // note that i == g from above 
	{
		*xp++ = (j += *p++);
	}


	// Make a table of values in order of bit lengths 
	p = b;  i = 0;
	do
	{
		if ((j = *p++) != 0)
			v[x[j]++] = i;
	} while (++i < n);


	// Generate the Huffman codes and for each, make the table entries 
	x[0] = i = 0;                 // first Huffman code is zero 
	p = v;                        // grab values in bit order 
	h = (tINT)-1;                       // no tables yet--level -1 
	w = (tINT)-l;                       // bits decoded == (l * h) 
	u[0] = ( huft*)NULL;   // just to keep compilers happy 
	q = ( huft*)NULL;      // ditto 
	z = 0;                        // ditto 

	// go through the bit lengths (k already is bits in shortest code) 
	for (; k <= g; k++)
	{
		a = c[k];
        while (a--)
		{
			// here i is the Huffman code of length k bits for value *p 
			// make tables up to required level 
			while (k > w + l)
			{
				h++;
				w += l;                 // previous table always l bits 

				// compute minimum size table less than or equal to l bits 
				z = (z = g - w) > (tWORD)l ? l : z;  // upper limit on table size 
				if ((f = 1 << (j = k - w)) > a + 1)     // try a k-w bit table 
				{                       // too few codes for k-w bit table 
					f -= a + 1;           // deduct codes from patterns left 
					xp = c + k;
					while (++j < z)       // try smaller tables up to z bits 
					{
						if ((f <<= 1) <= *++xp)
							break;            // enough codes to use up j bits 
						f -= *xp;           // else deduct codes from patterns 
					}
				}
				z = 1 << j;             // table entries for j-bit table 

				// allocate and link in new table 
				if ((q = ( huft  *)New(data, (z + 1)*sizeof( huft))) == (huft*)NULL)
				{
					if (h)
						huft_free(data, u[0]);
					OUT_DEBUG("Corrupt");
					data->zip_corrupt=1;
					RET=3;
					goto ret;
				}
#ifdef DEBUGMEM
				//printf("New: %08lX size %d",q,(z + 1)*sizeof( huft));
#endif
#ifdef _DEBUG
				hufts += z + 1;         // track memory usage 
#endif
				*t = q + 1;             // link to list for huft_free() 
				*(t = &(q->v.t)) = ( huft  *)NULL;
				u[h] = ++q;             // table starts after link 

				// connect to last table, if there is one 
				if (h)
				{
					x[h] = i;             // save pattern for backing up 
					r.b = (tBYTE)l;         // bits to dump before this table 
					r.e = (tBYTE)(16 + j);  // bits in this table 
//printf("e=%x\n",r.e);
					r.v.t = (huft*)q;            // pointer to this table 
					j = i >> (w - l);     // (get around Turbo C bug) 
					memcpy((tBYTE *)&u[h-1][j],(tBYTE *)&r,sizeof(r));
				}
			}

			// set up table entry in r 
			r.b = (tBYTE)(k - w);
			if (p >= v + n)
				r.e = 99;               // out of values--invalid code 
			else if (*p < s)
			{
				r.e = (tBYTE)(*p < 256 ? 16 : 15);    // 256 is end-of-block code 
//printf("e=%x\n",r.e);
				r.v.n = *p++;           // simple code is just the value 
			}
			else
			{
			tUINT o=*p - s;

				if(d==NULL || e==NULL)
				{
#ifdef _DEBUG
					printf("*p=%d o=%d n=%d s=%d   e=%X d=%X\n",*p,o,n,s,e,d);
#endif

					huft_free(data, q);
					data->zip_corrupt=1;
					OUT_DEBUG("Corrupt");
					RET=2;
					goto ret;
				}

				if(*p >= n)
				{
					r.e = 0;
					r.v.n = 0;
				}
				else
				{
					r.e = (tBYTE)e[o];   // non-simple--look up in lists 
					r.v.n = d[o];
				}
//printf("e=%x\n",r.e);
				p++;
			}

	// fill code-like entries with r 
			f = 1 << (k - w);
			for (j = i >> w; j < z; j += f)
			{
				memcpy((tBYTE *)&q[j],(tBYTE *)&r,sizeof(r));
			}

	// backwards increment the k-bit code i 
			for (j = 1 << (k - 1); i & j; j >>= 1)
				i ^= j;
			i ^= j;

			// backup over finished tables 
			while ((i & ((1 << w) - 1)) != x[h])
			{
				h--;                    // don't need to update q 
				w -= l;
			}
		}
	}


	// Return true (1) if we were given an incomplete table 
	RET=y != 0 && g != 1;
ret:

#ifdef _DEBUG
//	printf("huft_build_ return %d : y=%d g=%d\n",RET,y,g);
#endif

  return RET;
}


// Free the malloc'ed tables built by huft_build__(), which makes a linked
//   list of the tables it made, with the links in a dummy first entry of
//   each table. 

static tINT  huft_free(ExplodeData* data,  huft  * t)
{
huft  *p;
huft  *q;

  // Go through linked list, freeing from the malloced (t[-1]) address. 
	p = t;
	while (p != ( huft  *)NULL)
	{
		q = (--p)->v.t;
#ifdef DEBUGMEM
		printf("Delete: %08lX ",p);
#endif
		if(p)Delete(data,(tBYTE *)p);
		p = q;
	}
	return 0;
}


tBYTE* New(ExplodeData* data, tDWORD dwBytes)
{
tBYTE * lpData;

	if(errOK != CALL_Heap_Alloc(data->hHeap, (tPTR*)&lpData, dwBytes))
	{
		data->error = errNOT_ENOUGH_MEMORY;
		OUT_DEBUG("Error allocate memory");
	}
	return lpData;
}


tVOID Delete(ExplodeData* data, tBYTE * lpdata)
{
	if(lpdata==NULL)
	{
		return ;
	}
	CALL_Heap_Free(data->hHeap, lpdata);
}


