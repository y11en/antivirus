
/* Huffman decode routines */

static int NEAR make_table(nchar, bitlen, tablebits, table, tablesize)
int    nchar;
uchar  *bitlen;
int    tablebits;
ushort FAR* table;
int    tablesize;
{
static	ushort count[17], weight[17], start[18];
	ushort FAR* p;
	uint i, k, len, ch, jutbits, avail, nextcode, mask;

	for (i = 1; i <= 16; i++)
		count[i] = 0;

	for (i = 0; (int)i < nchar; i++)
		count[bitlen[i]]++;

	start[1] = 0;
	for (i = 1; i <= 16; i++)
		start[i + 1] = start[i] + (count[i] << (16 - i));
	if (start[17] != (ushort) (1 << 16)){
#ifdef _DEBUG
Printf("Error 3");
#endif
		corrupt=1;
		return 0;
	}

	jutbits = 16 - tablebits;
	for (i = 1; (int)i <= tablebits; i++)
	{
		start[i] >>= jutbits;
		weight[i] = 1 << (tablebits - i);
	}
	while (i <= 16)
	{
		weight[i] = 1 << (16 - i);
		i++;
	}

	i = start[tablebits + 1] >> jutbits;
	if (i != (ushort) (1 << 16))
	{
		k = 1 << tablebits;
		while (i != k)
			table[i++] = 0;
	}

	avail = nchar;
	mask = 1 << (15 - tablebits);
	for (ch = 0; (int)ch < nchar; ch++)
	{
		if ((len = bitlen[ch]) == 0)
			continue;
		k = start[len];
		nextcode = k + weight[len];
		if ((int)len <= tablebits)
		{
			if (nextcode > (uint)tablesize){
//				error(M_BADTABLE, "");
#ifdef _DEBUG
Printf("Error 4");
#endif
				corrupt=1;
				return 0;
			}

			for (i = start[len]; i < nextcode; i++)
				table[i] = ch;
		}
		else
		{
			p = &table[k >> jutbits];
			i = len - tablebits;
			while (i != 0)
			{
				if (*p == 0)
				{
					right[avail] = left[avail] = 0;
					*p = avail++;
				}
				if(*p > 2*NC){
					corrupt=1;
					return 0;
				}

				if (k & mask)
					p = &right[*p];
				else
					p = &left[*p];
				k <<= 1;
				i--;
			}
			*p = ch;
		}
		start[len] = nextcode;
	}
	return 1;
}

static void NEAR read_pt_len(nn, nbit, i_special)
int nn;
int nbit;
int i_special;
{
	int i, n;
	short c;
	ushort mask;
    n = getbits(nbit);
	if(n > nn){
		corrupt=1;
		return;
	}
#ifdef _DEBUG
Printf("read_pt_len nn=%d nbit=%d n=%d",nn,nbit,n);
#endif
    if (n == 0)
	{
        c = getbits(nbit);
        for (i = 0; i < nn; i++)
            pt_len[i] = 0;
        for (i = 0; i < 256; i++)
			pt_table[i] = c;
    }
    else
	{
        i = 0;
        while (i < n && !corrupt)
        {
            c = bitbuf >> (13);
            if (c == 7)
            {
                mask = 1 << (12);
				while (mask & bitbuf)
                {
                    mask >>= 1;
					c++;
                }
            }
            fillbuf((c < 7) ? 3 : (int)(c - 3));
            pt_len[i++] = (uchar)c;
			if (i == i_special)
            {
                c = getbits(2);
                while (--c >= 0)
					pt_len[i++] = 0;
            }
        }
        while (i < nn)
            pt_len[i++] = 0;
		make_table(nn, pt_len, 8, pt_table, PTABLESIZE*sizeof(ushort));
    }
}

static void NEAR
read_c_len()
{
	short i, c, n;
	ushort mask;
#ifdef _DEBUG
Printf("read_c_len");
#endif

	n = getbits(CBIT);
	if (n == 0)
	{
		c = getbits(CBIT);
		for (i = 0; i < NC; i++)
			c_len[i] = 0;
        for (i = 0; i < CTABLESIZE; i++)
            c_table[i] = c;
    }
    else
    {
		i = 0;
        while (i < n && !corrupt)
        {
			c = pt_table[bitbuf >> (8)];

            mask = 1 << (7);
            while(c >= NT && c)
			{
				if(c > 2*NC) { corrupt=1;return; }
                if (bitbuf & mask)
                    c = right[c];
                else
                    c = left[c];
                mask >>= 1;
            }
/*
            if (c >= NT)
            {
                mask = 1 << (7);
                do
				{
                    if (bitbuf & mask)
                        c = right[c];
                    else
                        c = left[c];
                    mask >>= 1;
				} while (c >= NT);
            }
*/
            fillbuf((int)(pt_len[c]));
            if (c <= 2)
            {
				if (c == 0)
					c = 1;
                else if (c == 1)
					c = getbits(4) + 3;
				else
					c = getbits(CBIT) + 20;
				while (--c >= 0)
					c_len[i++] = 0;
			}
			else
				c_len[i++] = (uchar)(c - 2);
		}
		while (i < NC)
			c_len[i++] = 0;
		make_table(NC, c_len, 12, c_table, CTABLESIZE*sizeof(ushort));
	}
}

static ushort NEAR
decode_c()
{
	ushort j, mask;

	if (blocksize == 0)
    {
        blocksize = getbits(16);
        read_pt_len(NT, TBIT, 3);
		if(corrupt)return 0;
		read_c_len();
		if(corrupt)return 0;
        read_pt_len(NP, PBIT, -1);
    }
	if(corrupt)return 0;
    blocksize--;
    j = c_table[bitbuf >> 4];

    mask = 1 << (3);
	while (j >= NC)
    {
		if(j > 2*NC) { corrupt=1;return 0; }
		if (bitbuf & mask)
			j = right[j];
        else
            j = left[j];
		mask >>= 1;
	}

/*
    if (j >= NC)
    {
        mask = 1 << (3);
//Printf("Do");
		do
        {
			if (bitbuf & mask)
				j = right[j];
            else
                j = left[j];
			mask >>= 1;
        } while (j >= NC);
//Printf("End while");
	}
*/
    fillbuf((int)(c_len[j]));
	return j;
}

static ushort NEAR
decode_p()
{
	ushort j, mask;

	j = pt_table[bitbuf >> (8)];

	mask = 1 << (7);
	while (j >= NP)
	{
		if(j > 2*NC) { corrupt=1;return 0; }
        if (bitbuf & mask)
			j = right[j];
        else
            j = left[j];
		mask >>= 1;
	}
/*	
	if (j >= NP)
	{
		mask = 1 << (7);
		do
		{
            if (bitbuf & mask)
				j = right[j];
            else
                j = left[j];
			mask >>= 1;
        } while (j >= NP);
	}
*/
    fillbuf((int)(pt_len[j]));
    if (j != 0)
	{
		j--;
		j = (1 << j) + getbits((int)j);
	}
	return j;
}

static void   NEAR
decode_start()
{
	blocksize = 0;
	init_getbits();
}

static void  NEAR
decode_main()
{
	short i;
	short j;
	short c;
	short r;
	long count;

	if((c_table=(ushort FAR*)New(8192))==0){
#ifdef _DEBUG
Printf("Error 5");
#endif
		corrupt=1;
		return;
	}
	if(disp_clock())corrupt=17;
	decode_start();
	count = 0;
	r = 0;
#ifdef _DEBUG
Printf("Start");
#endif

	while (count < origsize && !corrupt)
	{
		c = decode_c();
		if(corrupt)break;
		if (c <= UCHAR_MAX)
		{

			text[r] = (uchar) c;
			count++;
			if (++r >= DDICSIZ && !corrupt)
			{
				r = 0;
#ifdef _DEBUG
Printf("Try to write");
#endif
				if(disp_clock())corrupt=17;
				else fwrite_txt_crc(text, DDICSIZ);
			}
		}
		else
		{
			j = c - (UCHAR_MAX + 1 - THRESHOLD);
			count += j;
//#ifdef _DEBUG
//Printf("Try to decode_p");
//#endif
			i = decode_p();
//#ifdef _DEBUG
//Printf("decode_p reurns %d",i);
//#endif
			if ((i = r - i - 1) < 0)
				i += DDICSIZ;
			if( i<0 ){
				corrupt=1;
				return;
			}
			if (r > i && r < DDICSIZ - MAXMATCH - 1)
			{
				while (--j >= 0)
					text[r++] = text[i++];
			}
			else
			{
				while (--j >= 0)
				{
					text[r] = text[i];
					if (++r >= DDICSIZ && !corrupt)
					{
						r = 0;
#ifdef _DEBUG
Printf("Try to write");
#endif
						if(disp_clock())corrupt=17;
						else fwrite_txt_crc(text, DDICSIZ);
					}
					if (++i >= DDICSIZ)
						i = 0;
				}
			}
		}
	}
	if (r != 0 && !corrupt){
#ifdef _DEBUG
Printf("Try to write");
#endif
		fwrite_txt_crc(text, r);
	}
	Delete((BYTE FAR*)c_table);
}

/* Macros */

#define BFIL {getbuf|=bitbuf>>getlen;fillbuf(CODE_BIT-getlen);getlen=CODE_BIT;}
#define GETBIT(c) {if(getlen<=0)BFIL c=(getbuf&0x8000)!=0;getbuf<<=1;getlen--;}
#define BPUL(l) {getbuf<<=l;getlen-=l;}
#define GETBITS(c,l) {if(getlen<l)BFIL c=(ushort)getbuf>>(CODE_BIT-l);BPUL(l)}

static short NEAR
decode_ptr()
{
	short c;
	short width;
	short plus;
	short pwr;

	plus = 0;
	pwr = 1 << (STRTP);
	for (width = (STRTP); width < (STOPP) ; width++)
	{
		GETBIT(c);
		if (c == 0)
			break;
		plus += pwr;
		pwr <<= 1;
	}
	if (width != 0)
        GETBITS(c, width);
	c += plus;
	return c;
}

static short NEAR
decode_len()
{
	short c;
	short width;
    short plus;
    short pwr;

    plus = 0;
    pwr = 1 << (STRTL);
	for (width = (STRTL); width < (STOPL) ; width++)
    {
        GETBIT(c);
		if (c == 0)
            break;
        plus += pwr;
		pwr <<= 1;
    }
    if (width != 0)
        GETBITS(c, width);
	c += plus;
	return c;
}

static void NEAR
decode_f()
{
	short i;
	short j;
	short c;
	short r;
	short pos;
	long count;

	if((c_table=(ushort FAR*)New(8192))==0){
#ifdef _DEBUG
Printf("Error 6");
#endif
		corrupt=1;
		return;
	}
	if(disp_clock())corrupt=17;
	init_getbits();
	getlen = getbuf = 0;
	count = 0;
	r = 0;

	while (count < origsize && !corrupt)
	{
		c = decode_len();
		if (c == 0)
		{
			GETBITS(c, CHAR_BIT);
			text[r] = (uchar)c;
			count++;
			if (++r >= DDICSIZ && !corrupt)
			{
				r = 0;
#ifdef _DEBUG
Printf("Try to write");
#endif
				if(disp_clock())corrupt=17;
				else fwrite_txt_crc(text, DDICSIZ);
			}
		}
		else
		{
			j = c - 1 + THRESHOLD;
			count += j;
			pos = decode_ptr();
			if ((i = r - pos - 1) < 0)
				i += DDICSIZ;
			while (j-- > 0)
			{
				text[r] = text[i];
				if (++r >= DDICSIZ && !corrupt)
				{
					r = 0;
#ifdef _DEBUG
Printf("Try to write");
#endif
					if(disp_clock())corrupt=17;
					else fwrite_txt_crc(text, DDICSIZ);
				}
				if (++i >= DDICSIZ)
					i = 0;
			}
		}
	}
	if (r != 0 && !corrupt){
#ifdef _DEBUG
Printf("Try to write");
#endif
		fwrite_txt_crc(text, r);
	}
	Delete((BYTE FAR*)c_table);
}

/* end DECODE.C */