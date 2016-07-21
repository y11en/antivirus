#include <Prague/prague.h>
#include "unarj.h"


static tWORD getbits(UnArj_Data* data, tINT n);
static tVOID fillbuf(UnArj_Data* data, tINT n);
static tVOID init_getbits(UnArj_Data* data);


/* Huffman decode routines */

static tINT make_table(UnArj_Data* data, tINT nchar, tBYTE* bitlen, tINT tablebits, tWORD* table, tINT tablesize)
{
tWORD count[17], weight[17], start[18];
tWORD * p;
tUINT i, k, len, ch, jutbits, avail, nextcode, mask;


	for (i = 1; i <= 16; i++)
		count[i] = 0;

	for (i = 0; (tINT)i < nchar; i++)
  {
    if ( bitlen[i] >= 17 )
      return(0);
		count[bitlen[i]]++;
  }

	start[1] = 0;
	for (i = 1; i <= 16; i++)
		start[i + 1] = start[i] + (count[i] << (16 - i));
	if (start[17] != (tWORD) (1 << 16))
	{
#ifdef DEBUG
printf("Error 3");
#endif
		return 0;
	}

	jutbits = 16 - tablebits;
	for (i = 1; (tINT)i <= tablebits; i++)
	{
		start[i] >>= jutbits;
		weight[i] = 1 << (tablebits - i);
	}
	while (i <= 16)
	{
		weight[i] = 1 << (16 - i);
		i++;
	}

  if ( tablebits > 16 )
    return(0);

	i = start[tablebits + 1] >> jutbits;
	if (i != (tWORD) (1 << 16))
	{
		k = 1 << tablebits;
		while (i != k)
		{
			if(i > (tUINT)tablesize)
			{
				return 0;
			}
			table[i++] = 0;
		}
	}

	avail = nchar;
	mask = 1 << (15 - tablebits);
	for (ch = 0; (tINT)ch < nchar; ch++)
	{
		if ((len = bitlen[ch]) == 0)
			continue;
    if ( len >= 17 )
      return(0);
		k = start[len];
		nextcode = k + weight[len];
		if ((tINT)len <= tablebits)
		{
			if (nextcode > (tUINT)tablesize)
			{
//				error(M_BADTABLE, "");
#ifdef DEBUG
printf("Error 4");
#endif
				return 0;
			}

			for (i = start[len]; i < nextcode; i++)
			{
				table[i] = ch;
			}
		}
		else
		{
      if((k >> jutbits) >= (tUINT)tablesize)
        return(0);

			p = &table[k >> jutbits];
			i = len - tablebits;
			while (i != 0)
			{
				if (*p == 0)
				{
          if(avail >= 2 * NC - 1)
            return(0);

					data->right[avail] = data->left[avail] = 0;
					*p = avail++;
				}
				if(*p >= 2 * NC - 1)
				{
					return 0;
				}

				if (k & mask)
					p = &data->right[*p];
				else
					p = &data->left[*p];
				k <<= 1;
				i--;
			}
			*p = ch;
		}
		start[len] = nextcode;
	}
	return 1;
}

static tVOID read_pt_len(UnArj_Data* data, tINT nn, tINT nbit, tINT i_special)
{
tINT i, n;
tSHORT c;
tWORD mask;

    n = getbits(data, nbit);
	if(n > nn)
	{
		data->error = errNOT_OK;
		return;
	}
#ifdef DEBUG
printf("read_pt_len nn=%d nbit=%d n=%d",nn,nbit,n);
#endif
    if (n == 0)
	{
        c = getbits(data, nbit);
        for (i = 0; i < nn; i++)
            data->pt_len[i] = 0;
        for (i = 0; i < 256; i++)
			data->pt_table[i] = c;
    }
    else
	{
        i = 0;
        while (i < n && PR_SUCC(data->error))
        {
            c = data->bitbuf >> (13);
            if (c == 7)
            {
                mask = 1 << (12);
				while (mask & data->bitbuf)
                {
                    mask >>= 1;
					c++;
                }
            }
            fillbuf(data, (c < 7) ? 3 : (tINT)(c - 3));
            data->pt_len[i++] = (tBYTE)c;
			if (i == i_special)
            {
                c = getbits(data, 2);
                while (--c >= 0)
					data->pt_len[i++] = 0;
            }
        }
        while (i < nn)
            data->pt_len[i++] = 0;
		make_table(data, nn, data->pt_len, 8, data->pt_table, PTABLESIZE*sizeof(tWORD));
    }
}

static tVOID read_c_len(UnArj_Data* data)
{
tSHORT i, c, n;
tWORD mask;

#ifdef DEBUG
printf("read_c_len");
#endif

	n = getbits(data, CBIT);
	if (n == 0)
	{
		c = getbits(data, CBIT);
		for (i = 0; i < NC; i++)
			data->c_len[i] = 0;
        for (i = 0; i < CTABLESIZE; i++)
            data->c_table[i] = c;
    }
    else
    {
		i = 0;
        while (i < n && PR_SUCC(data->error))
        {
			c = data->pt_table[data->bitbuf >> (8)];

            mask = 1 << (7);
            while(c >= NT && c)
			{
				if(c > 2*NC) 
				{ 
					data->error = errNOT_OK;
					return; 
				}
                if (data->bitbuf & mask)
                    c = data->right[c];
                else
                    c = data->left[c];
                mask >>= 1;
            }
/*
            if (c >= NT)
            {
                mask = 1 << (7);
                do
				{
                    if (data->bitbuf & mask)
                        c = data->right[c];
                    else
                        c = data->left[c];
                    mask >>= 1;
				} while (c >= NT);
            }
*/
            fillbuf(data, (tINT)(data->pt_len[c]));
            if (c <= 2)
            {
				if (c == 0)
					c = 1;
                else if (c == 1)
					c = getbits(data, 4) + 3;
				else
					c = getbits(data, CBIT) + 20;
                
                if (i + c >= NC)
                {
                    data->error = errNOT_OK;
                    return;
                }

				while (--c >= 0)
					data->c_len[i++] = 0;
			}
			else
            {
                if (i >= NC)
                {
                    data->error = errNOT_OK;
                    return;
                }
				data->c_len[i++] = (tBYTE)(c - 2);
            }
		}
		while (i < NC)
			data->c_len[i++] = 0;
		make_table(data, NC, data->c_len, 12, data->c_table, CTABLESIZE/**sizeof(tWORD)*/);
	}
}

static tWORD decode_c(UnArj_Data* data)
{
tWORD j, mask;

	if (data->blocksize == 0)
    {
        data->blocksize = getbits(data, 16);
        read_pt_len(data, NT, TBIT, 3);
		if(PR_FAIL(data->error))
			return 0;
		read_c_len(data);
		if(PR_FAIL(data->error))
			return 0;
        read_pt_len(data, NP, PBIT, -1);
    }
	if(PR_FAIL(data->error))
		return 0;
    data->blocksize--;
    j = data->c_table[data->bitbuf >> 4];

    mask = 1 << (3);
	while (j >= NC)
    {
		if(j > 2*NC) 
		{ 
			data->error = errNOT_OK;
			return 0; 
		}
		if (data->bitbuf & mask)
			j = data->right[j];
        else
            j = data->left[j];
		mask >>= 1;
	}

/*
    if (j >= NC)
    {
        mask = 1 << (3);
//printf("Do");
		do
        {
			if (data->bitbuf & mask)
				j = data->right[j];
            else
                j = data->left[j];
			mask >>= 1;
        } while (j >= NC);
//printf("End while");
	}
*/
    fillbuf(data, (tINT)(data->c_len[j]));
	return j;
}

static tWORD decode_p(UnArj_Data* data)
{
tWORD j, mask;

	j = data->pt_table[data->bitbuf >> (8)];

	mask = 1 << (7);
	while (j >= NP)
	{
		if(j > 2*NC) 
		{
			data->error = errNOT_OK;
			return 0; 
		}
        if (data->bitbuf & mask)
			j = data->right[j];
        else
            j = data->left[j];
		mask >>= 1;
	}
/*	
	if (j >= NP)
	{
		mask = 1 << (7);
		do
		{
            if (data->bitbuf & mask)
				j = data->right[j];
            else
                j = data->left[j];
			mask >>= 1;
        } while (j >= NP);
	}
*/
    fillbuf(data, (tINT)(data->pt_len[j]));
    if (j != 0)
	{
		j--;
		j = (1 << j) + getbits(data, (tINT)j);
	}
	return j;
}

static tVOID decode_start(UnArj_Data* data)
{
	data->blocksize = 0;
	init_getbits(data);
}

tDWORD  decode_main(UnArj_Data* data)
{
tSHORT i;
tSHORT j;
tSHORT c;
tSHORT r;
tLONG count;

	if((data->c_table = (tWORD*)ArjNew(data, CTABLESIZE*sizeof(tWORD)))==0)
	{
#ifdef DEBUG
printf("Error 5");
#endif
		data->error = errNOT_OK;
		return 0;
	}
	decode_start(data);
	count = 0;
	r = 0;
#ifdef DEBUG
printf("Start");
#endif

	while (count < data->origsize && PR_SUCC(data->error))
	{
		c = decode_c(data);
		if(PR_FAIL(data->error))
			break;
		if (c <= UCHAR_MAX)
		{

			data->text[r] = (tBYTE) c;
			count++;
			if (++r >= DDICSIZ && PR_SUCC(data->error))
			{
				r = 0;
#ifdef DEBUG
printf("Try to write");
#endif
				data->error = data->UnArjWrite(data, data->text, DDICSIZ);
				if(PR_FAIL(data->error))
				{
					return 0;
				}
			}
		}
		else
		{
			j = c - (UCHAR_MAX + 1 - THRESHOLD);
			count += j;
			i = decode_p(data);
			if ((i = r - i - 1) < 0)
				i += DDICSIZ;
			if( i<0 )
			{
				data->error = errNOT_OK;
				return 0;
			}
			if (r > i && r < DDICSIZ - MAXMATCH - 1)
			{
				while (--j >= 0)
					data->text[r++] = data->text[i++];
			}
			else
			{
				while (--j >= 0)
				{
					data->text[r] = data->text[i];
					if (++r >= DDICSIZ && PR_SUCC(data->error))
					{
						r = 0;
						data->error = data->UnArjWrite(data, data->text, DDICSIZ);
						if(PR_FAIL(data->error))
						{
							return 0;
						}
						
					}
					if (++i >= DDICSIZ)
						i = 0;
				}
			}
		}
	}
	if (r != 0 && PR_SUCC(data->error))
	{
		data->error = data->UnArjWrite(data, data->text, r);
		if(PR_FAIL(data->error))
		{
			return 0;
		}
	}
	ArjDelete(data, data->c_table);
	data->c_table = NULL;
	return data->dwDecoded;
}

/* Macros */

#define BFIL {data->getbuf|=data->bitbuf>>data->getlen;fillbuf(data, CODE_BIT-data->getlen);data->getlen=CODE_BIT;}
#define GETBIT(c) {if(data->getlen<=0)BFIL c=(data->getbuf&0x8000)!=0;data->getbuf<<=1;data->getlen--;}
#define BPUL(l) {data->getbuf<<=l;data->getlen-=l;}
#define GETBITS(c,l) {if(data->getlen<l)BFIL c=(tWORD)data->getbuf>>(CODE_BIT-l);BPUL(l)}

static tWORD decode_ptr(UnArj_Data* data)
{
tSHORT c;
tSHORT width;
tSHORT plus;
tSHORT pwr;

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

static tWORD decode_len(UnArj_Data* data)
{
tSHORT c;
tSHORT width;
tSHORT plus;
tSHORT pwr;

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

tDWORD decode_f(UnArj_Data* data)
{
tSHORT i;
tSHORT j;
tSHORT c;
tSHORT r;
tSHORT pos;
tLONG count;

	if((data->c_table=(tWORD *)ArjNew(data, CTABLESIZE*sizeof(tWORD)))==0)
	{
#ifdef DEBUG
printf("Error 6");
#endif
		data->error = errNOT_OK;
		return 0;
	}
	init_getbits(data);
	data->getlen = data->getbuf = 0;
	count = 0;
	r = 0;

	while (count < data->origsize && PR_SUCC(data->error))
	{
		c = decode_len(data);
		if (c == 0)
		{
			GETBITS(c, CHAR_BIT);
			data->text[r] = (tBYTE)c;
			count++;
			if (++r >= DDICSIZ && PR_SUCC(data->error))
			{
				r = 0;
				data->error = data->UnArjWrite(data, data->text, DDICSIZ);
				if(PR_FAIL(data->error))
				{
					return 0;
				}
			}
		}
		else
		{
			j = c - 1 + THRESHOLD;
			count += j;
			pos = decode_ptr(data);
			if ((i = r - pos - 1) < 0)
				i += DDICSIZ;
			while (j-- > 0)
			{
				data->text[r] = data->text[i];
				if (++r >= DDICSIZ && PR_SUCC(data->error))
				{
					r = 0;
					data->error = data->UnArjWrite(data, data->text, DDICSIZ);
					if(PR_FAIL(data->error))
					{
						return 0;
					}
				}
				if (++i >= DDICSIZ)
					i = 0;
			}
		}
	}
	if (r != 0 && PR_SUCC(data->error))
	{
		data->error = data->UnArjWrite(data, data->text, r);
	}
	ArjDelete(data, data->c_table);
	return data->dwDecoded;
}

/* end DECODE.C */



static tVOID init_getbits(UnArj_Data* data)
{
	data->bitbuf = 0;
	data->subbitbuf = 0;
	data->bitcount = 0;
	fillbuf(data, 2 * CHAR_BIT);
}




static tVOID fillbuf(UnArj_Data* data, tINT n)                /* Shift data->bitbuf n bits data->left, read n bits */
{
	if(n > (2 * CHAR_BIT))
	{
#ifdef DEBUG
		printf("Error 1");
#endif
		data->error = errNOT_OK;
	}
	data->bitbuf = (data->bitbuf << n) & 0xFFFF;  /* lose the first n bits */
	while (n > data->bitcount && PR_SUCC(data->error))
	{
		data->bitbuf |= data->subbitbuf << (n -= data->bitcount);
		if (data->compsize != 0)
		{
			data->compsize--;
			if (data->incnt == 0) 
			{
				tUINT rsiz = (data->last_len > INBUFSIZ) ? INBUFSIZ : (tUINT)data->last_len;
				data->error = CALL_SeqIO_Read(data->hInputIo, &data->incnt, data->inbuf, rsiz);
				if(data->incnt == 0)
				{
					data->error = errNOT_OK;
				}
				data->last_len-=data->incnt;
				data->inptr = data->inbuf;
			}
			data->subbitbuf = *data->inptr++;
			--data->incnt;
		}
		else
			data->subbitbuf = 0;
		data->bitcount = CHAR_BIT;
	}
	data->bitbuf |= data->subbitbuf >> (data->bitcount -= n);
}

static tWORD getbits(UnArj_Data* data, tINT n)
{
tWORD x;

	x = data->bitbuf >> (2*CHAR_BIT - n);
	fillbuf(data, n);
	return x;
}


