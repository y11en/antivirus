
/*---------------------------------------------------------------------------

  unshrink.c

  Shrinking is a Dynamic Lempel-Ziv-Welch compression algorithm with partial
  clearing.

  ---------------------------------------------------------------------------*/

#include <Prague/prague.h>
#include "unshrinki.h"
#include <stdio.h>

static tVOID partial_clear(UnshrinkData* data);   /* local prototype */


static tWORD  mask_bits[] = 
{
	0x0000,
	0x0001, 0x0003, 0x0007, 0x000f, 0x001f, 0x003f, 0x007f, 0x00ff,
	0x01ff, 0x03ff, 0x07ff, 0x0fff, 0x1fff, 0x3fff, 0x7fff, 0xffff
};


/*************************************/
/*  UnShrink Defines, Globals, etc.  */
/*************************************/


#ifdef READBIT
#undef READBIT
#endif
#define READBIT(nbits,zdest) {if(nbits>data->bits_left) FillBitBuffer(data);\
  zdest=(int)((tWORD)data->zip_bitbuf & mask_bits[nbits]);data->zip_bitbuf>>=nbits;data->bits_left-=nbits;}


static int FillBitBuffer(UnshrinkData* data)
{
tWORD temp = 0;

	data->zipeof = 1;
	data->error = errOK;
	while (data->bits_left < 25 && !(data->error = CALL_SeqIO_ReadByte(data->hInputIO,(tBYTE*)&temp)))
	{
		data->zip_bitbuf |= (tDWORD)((tDWORD)temp << (tBYTE)data->bits_left);
		data->bits_left += 8;
		data->zipeof = 0;
	}
	return 0;
}

/*************************/
/*  Function unShrink()  */
/*************************/

tQWORD unShrink(UnshrinkData* data)
{
tINT code;
tINT stackp;
//tBOOL zipeof;
tINT finchar;
tINT oldcode;
tINT incode;
tQWORD counter;
tQWORD qwOutSize;
//tDWORD ret;
//tERROR err;

    if (data->zip_corrupt)
    {
        return 0;
    }

	/* decompress the file */
	data->codesize = INIT_BITS;
	data->maxcode = (1 << data->codesize) - 1;
	data->maxcodemax = HSIZE;         /* (1 << MAX_BITS) */
	data->free_ent = FIRST_ENT;

	data->zipeof = cFALSE;
	data->bits_left = 0;
	counter=0;
	qwOutSize=0;

	code = data->maxcodemax;
	do {
		data->area->shrink.Prefix_of[code] = (tWORD)-1;
	} while (--code > 255);


	for (code = 0; code  <256; code++)
	{
		data->area->shrink.Prefix_of[code] = 0;
		data->area->shrink.Suffix_of[code] = (tBYTE) code;
		data->area->shrink.Prefix_of[code] = 0;
	}
	code=0;

//printf("73 Code=%X data->data->area->shrink.Suffix_of[0x4a]=%X",code,data->data->area->shrink.Suffix_of[0x4a]);
	READBIT(data->codesize, oldcode);
	if(data->error)
	{
		return 0;
	}

	//	if(errOK!=CALL_SeqIO_ReadBit(data->hInputIO, &ret, data->codesize))
//	{
//		OUT_DEBUG("Corrupt zipeof");
//		goto loc_exit;
//	}

//	oldcode=(tBYTE)ret;

//printf("75 Code=%X data->data->area->shrink.Suffix_of[0x4a]=%X",code,data->data->area->shrink.Suffix_of[0x4a]);
	finchar = oldcode;

//printf("OutByte  1 %X\n",finchar);
	if(qwOutSize == counter)
	{
		qwOutSize += 0x4000;
		CALL_SeqIO_SetSize(data->hOutputIO, qwOutSize);
	}
	CALL_SeqIO_WriteByte(data->hOutputIO, (tBYTE)finchar);
	counter++;
	stackp = HSIZE;
	while (!data->zipeof)
	{
		READBIT(data->codesize, code);
		if(PR_FAIL(data->error))
		{
			return 0;
		}
//		printf("89 Code=%X nbits %X   %X\n",code,data->codesize,mask_bits[data->codesize]);
//		if(errOK!=CALL_SeqIO_ReadBit(data->hInputIO, &ret, data->codesize))
//		{
//			OUT_DEBUG("Corrupt zipeof");
//			goto loc_exit;
//		}
//		code=(tBYTE)ret;

//printf("90 Code=%X nbits %X   %X\n",code,codesize,mask_bits[codesize]);
		while (code == CLEAR)
		{
/*			if(errOK!=CALL_SeqIO_ReadBit(data->hInputIO, &ret, data->codesize))
			{
//				OUT_DEBUG("Corrupt zipeof");
				goto loc_exit;
			}
			code=(tBYTE) ret;*/
			READBIT(data->codesize, code);
			if(PR_FAIL(data->error))
			{
				return 0;
			}
			switch (code)
			{
				case 1:
					data->codesize++;
					if (data->codesize == MAX_BITS)
						data->maxcode = data->maxcodemax;
					else
						data->maxcode = (1 << data->codesize) - 1;
					break;

				case 2:
					partial_clear(data);
					break;
			}
/*			if(errOK!=CALL_SeqIO_ReadBit(data->hInputIO, &ret, data->codesize))
			{
//				OUT_DEBUG("Corrupt zipeof");
				goto loc_exit;
			}
			code = (tBYTE)ret;*/
			READBIT(data->codesize, code);
			if(PR_FAIL(data->error))
			{
				return 0;
			}
		}

//printf("122 Code=%X data->data->area->shrink.Suffix_of[0x4a]=%X\n",code,data->data->area->shrink.Suffix_of[0x4a]);
		/* special case for KwKwK string */
		incode = code;
		if (data->area->shrink.Prefix_of[code] == (tWORD)-1)
		{
//printf("127 OutByte  6 %X\n",finchar);
      if ( stackp <= 0 )
      {
        data->zip_corrupt=1;
        return 0;
      }
			data->area->shrink.Stack[--stackp] = (tBYTE) finchar;
			code = oldcode;
		}
		/* generate output characters in reverse order */
		while (code >= FIRST_ENT )
		{
			if( code > HSIZE)
			{
//				Printf("130 Code=%d ",code);
				data->zip_corrupt=1;
				return 0;
			}
			if (data->area->shrink.Prefix_of[code] == (tWORD)-1)
			{
//printf("136 OutByte  7 %X\n",finchar);
        if ( stackp <= 0 )
        {
          data->zip_corrupt=1;
          return 0;
        }
				data->area->shrink.Stack[--stackp] = (tBYTE) finchar;
				code = oldcode;
			}
			else if(data->area->shrink.Prefix_of[code] == code)
			{
//				OUT_DEBUG("Corrupt data->area->shrink.Prefix_of[code] == code");
				data->zip_corrupt=1;
				goto loc_exit;
			}
			else
			{
        if ( stackp <= 0 )
        {
          data->zip_corrupt=1;
          return 0;
        }
				data->area->shrink.Stack[--stackp] = data->area->shrink.Suffix_of[code];
				code = data->area->shrink.Prefix_of[code];
			}
		}

		finchar = data->area->shrink.Suffix_of[code];
//printf("154 Code=%X data->area->shrink.Suffix_of[0x4a]=%X\n",code,data->area->shrink.Suffix_of[0x4a]);
//printf("155 OutByte  8 %X\n",finchar);
    if ( stackp <= 0 )
    {
      data->zip_corrupt=1;
      return 0;
    }
		data->area->shrink.Stack[--stackp] = (tBYTE) finchar;


		// and put them out in forward order, block copy 
/*		if ((HSIZE - stackp + Zip_outcnt) < OUTBUFSIZ)
		{
//printf("                      OutByte 4 %d",HSIZE - stackp);

			memcpy(zip_outptr, data->area->shrink.Stack + stackp, HSIZE - stackp);
for(iiii=0;iiii<HSIZE - stackp;iiii++)
{
printf("OutBute 4 %02X",zip_outptr[iiii]);
}
			zip_outptr += HSIZE - stackp;
			Zip_outcnt += HSIZE - stackp;
			stackp = HSIZE;
		}
		// output tBYTE by tBYTE if we can't go by blocks
		else*/
		{
			while (stackp < HSIZE)
			{
//printf("178 OutByte 2 %02X\n",data->area->shrink.Stack[stackp]);
				if(qwOutSize == counter)
				{
					qwOutSize+=0x4000;
					CALL_SeqIO_SetSize(data->hOutputIO, qwOutSize);
				}
				CALL_SeqIO_WriteByte(data->hOutputIO, (data->area->shrink.Stack[stackp++]));
				if(data->dwYeldCounter++ > 0x100)
				{
					data->dwYeldCounter = 0;
					if( errOPERATION_CANCELED == CALL_SYS_SendMsg(data->hOutputIO, pmc_PROCESSING, pm_PROCESSING_YIELD, data->_Me, 0 ,0 ))
						return 0;
				}
				counter++;
			}
		}


		// generate new entry
		code = data->free_ent;
		if (code < data->maxcodemax)
		{

			data->area->shrink.Prefix_of[code] = oldcode;

//printf("191 OutByte  9 %X\n",finchar);
			data->area->shrink.Suffix_of[code] = (tBYTE) finchar;

			do
				code++;
			while ((code < data->maxcodemax) && (data->area->shrink.Prefix_of[code] != (tWORD)-1));

			data->free_ent = code;
		}
		/* remember previous code */
		oldcode = incode;

	}
#ifdef _DEBUG
//	printf("end of unshrink");
#endif
loc_exit:
	if(counter > data->qwSizeOfFile )
	{
		counter = (tDWORD)data->qwSizeOfFile ;
	}
	CALL_SeqIO_SetSize(data->hOutputIO, counter);
	return counter;
}


static tVOID partial_clear(UnshrinkData* data)
{
tINT pr;
tINT cd;

#ifdef _DEBUG
//	printf("partial_clear 1");
#endif

	/* mark all nodes as potentially unused */
	for (cd = FIRST_ENT; cd < data->free_ent; cd++)
		data->area->shrink.Prefix_of[cd] |= 0x8000;

	/* unmark those that are used by other nodes */
	for (cd = FIRST_ENT; cd < data->free_ent; cd++)
	{
		pr = data->area->shrink.Prefix_of[cd] & 0x7fff;    /* reference to another node? */
		if (pr >= FIRST_ENT)    /* flag node as referenced */
			data->area->shrink.Prefix_of[pr] &= 0x7fff;
	}

	/* clear the ones that are still marked */
	for (cd = FIRST_ENT; cd < data->free_ent; cd++)
		if ((data->area->shrink.Prefix_of[cd] & 0x8000) != 0)
			data->area->shrink.Prefix_of[cd] = (tWORD)-1;

	/* find first cleared node as next free_ent */
	cd = FIRST_ENT;
	while ((cd < data->maxcodemax) && (data->area->shrink.Prefix_of[cd] != (tWORD)-1))
		cd++;
	data->free_ent = cd;
#ifdef _DEBUG
//	printf("partial_clear 2");
#endif

}

