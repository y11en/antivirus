#include <Prague/prague.h>
#include <Prague/iface/i_io.h>

#include "unreducei.h"

#include <memory.h>
#include <stdio.h>

/**************************************/
/*  UnReduce Defines, Typedefs, etc.  */
/**************************************/

#define DLE    144

typedef tBYTE f_array[64];       /* for followers[256][64] */

static tERROR LoadFollowers(UnreduceData* data, tBYTE *,f_array *followers);
tVOID Delete(UnreduceData* data, tBYTE * lpdata);
tBYTE* New(UnreduceData* data, tDWORD dwBytes);


#define READBIT(nbits,zdest) {if(nbits > data->bits_left) FillBitBuffer(data);\
zdest=((tWORD)data->zip_bitbuf & mask_bits[nbits]); data->zip_bitbuf>>=nbits; data->bits_left-=nbits;}


static tWORD  mask_bits[] = {
	0x0000,
		0x0001, 0x0003, 0x0007, 0x000f, 0x001f, 0x003f, 0x007f, 0x00ff,
		0x01ff, 0x03ff, 0x07ff, 0x0fff, 0x1fff, 0x3fff, 0x7fff, 0xffff
};

/*******************************/
/*  UnReduce Global Variables  */
/*******************************/

static tINT  L_table[] = {0, 0x7f, 0x3f, 0x1f, 0x0f};
static tINT  D_shift[] = {0, 0x07, 0x06, 0x05, 0x04};
static tINT  D_mask[] = {0, 0x01, 0x03, 0x07, 0x0f};


static int FillBitBuffer(UnreduceData* data)
{
tWORD temp = 0;
//	data->zipeof = 1;
	data->error = errOK;
	while (data->bits_left < 25 && !PR_FAIL(data->error = CALL_SeqIO_ReadByte(data->hInputIO,(tBYTE*)&temp)))
	{
		data->zip_bitbuf |= (tDWORD)((tDWORD)temp << (tBYTE)data->bits_left);
		data->bits_left += 8;
//		data->zipeof = 0;
	}
	return 0;
}


/*************************/
/*  Function unReduce()  */
/*************************/

tERROR unReduce(UnreduceData* data)   /* expand probabilistically reduced data */
{
tINT lchar = 0;
tBYTE* b;
tINT nchar;
tINT ExState = 0;
tINT V = 0;
tINT Len = 0;
tLONG s = (tLONG)data->Zip_ucsize;  /* number of tBYTEs left to decompress */
tWORD w = 0;      /* position in output window data->slide[] */
tWORD u = 1;      /* true if data->slide[] unflushed */
tINT x,i;
tBYTE* Slen;
tCHAR* B_table;
tINT  factor;
f_array *followers=NULL;     /* shared work space */
tDWORD dwCounter;
tQWORD qwSize;
//tDWORD ret;
//tERROR err;

	dwCounter = 0;
	data->bits_left = 0;
	B_table = New(data, 520);
	if(!B_table)
	{
		return 0;
	}
	Slen=B_table+260;
	b=B_table;
	*(b++)=8;
	*(b++)=1;
	for(x=0;x<8;)
	{
		for(i=1<<x++;i;i--)
			*(b++)=x;
	}

	followers = (f_array *) (data->slide + 0x4000);  //??????????!!!!!
	factor = data->compression_method - 1;

	if(errOK!=LoadFollowers(data, Slen,followers))
	{
		return 0;
	}

	while (s > 0 )
	{
/*
		err = CALL_SYS_ErrorCodeGet(data->hInputIO);
		if(PR_FAIL(err))
		{
			return 0;
		}
*/
		if (Slen[lchar] == 0)
		{
//			CALL_SeqIO_ReadBit(data->hInputIO, &ret, 8);   /* ; */
//			nchar=(tBYTE)ret;
			READBIT(8, nchar);
		}
		else
		{
//			CALL_SeqIO_ReadBit(data->hInputIO, &ret, 1);
//			nchar=(tBYTE)ret;
			READBIT(1, nchar);
			if (nchar != 0)
			{
//				CALL_SeqIO_ReadBit(data->hInputIO, &ret,  8);   /* ; */
//				nchar=(tBYTE)ret;
				READBIT(8, nchar);       /* ; */
			}
			else
			{
			tINT follower;
				READBIT((tINT)B_table[Slen[lchar]], follower);
//				CALL_SeqIO_ReadBit(data->hInputIO, &ret, (tINT)B_table[Slen[lchar]]);   /* ; */
//				follower=(tBYTE)ret;
				nchar = followers[lchar][follower];
			}
		}
		if(PR_FAIL(data->error))
		{
			return 0;
		}
	/* expand the resulting tBYTE */
		switch (ExState)
		{
			case 0:
				if (nchar != DLE)
				{
					s--;
					data->slide[w++] = (tBYTE) nchar;
					if (w == SIZEOFSLIDE)
					{
						dwCounter+=w;
						CALL_SeqIO_GetSize(data->hOutputIO, &qwSize, IO_SIZE_TYPE_EXPLICIT);
						CALL_SeqIO_SetSize(data->hOutputIO, qwSize+w);
						CALL_SeqIO_Write(data->hOutputIO,NULL, data->slide,w);
						if(data->dwYeldCounter++ > 0x100)
						{
							data->dwYeldCounter = 0;
							if(errOPERATION_CANCELED == CALL_SYS_SendMsg(data->hOutputIO, pmc_PROCESSING, pm_PROCESSING_YIELD, data->_Me, 0 ,0 ))
								return 0;
						}
						w = u = 0;
					}
				}
				else
					ExState = 1;
				break;

			case 1:
				if (nchar != 0)
				{
					V = nchar;
					Len = V & L_table[factor];
					if (Len == L_table[factor])
						ExState = 2;
					else
						ExState = 3;
				}
				else
				{
					s--;
					data->slide[w++] = DLE;
					if (w == SIZEOFSLIDE)
					{
						dwCounter+=w;
						CALL_SeqIO_GetSize(data->hOutputIO, &qwSize, IO_SIZE_TYPE_EXPLICIT);
						CALL_SeqIO_SetSize(data->hOutputIO, qwSize+w);
						CALL_SeqIO_Write(data->hOutputIO,NULL, data->slide,w);
						if(data->dwYeldCounter++ > 0x100)
						{
							data->dwYeldCounter = 0;
							if(errOPERATION_CANCELED == CALL_SYS_SendMsg(data->hOutputIO, pmc_PROCESSING, pm_PROCESSING_YIELD, data->_Me, 0 ,0 ))
								return 0;
						}
						w = u = 0;
					}
					ExState = 0;
				}
				break;

			case 2:
				Len += nchar;
				ExState = 3;
				break;

			case 3:
			{
			tWORD e;
			tWORD n = Len + 3;
			tWORD d = w - ((((V >> D_shift[factor]) & D_mask[factor]) << 8) + nchar + 1);

				s -= n;
				do
				{
					n -= (e = (e = SIZEOFSLIDE - ((d &= 0x3fff) > w ? d : w)) > n ? n : e);
					if (u && w <= d)
					{
						memset(data->slide + w, 0, e);
						w += e;
						d += e;
					}
					else if (w - d < e)      /* (assume tWORD comparison) */
					{
						do
						{              /* slow to avoid memcpy() overlap */
							data->slide[w++] = data->slide[d++];
						} while (--e);
					}
					else
					{
						memcpy(data->slide + w, data->slide + d, e);
						w += e;
						d += e;
					}
					if (w == SIZEOFSLIDE)
					{
						dwCounter+=w;
						CALL_SeqIO_GetSize(data->hOutputIO, &qwSize, IO_SIZE_TYPE_EXPLICIT);
						CALL_SeqIO_SetSize(data->hOutputIO, qwSize+w);
						CALL_SeqIO_Write(data->hOutputIO,NULL, data->slide,w);
						w = u = 0;
					}
				} while (n );

				ExState = 0;
			}
			break;
		}

		/* store tCHARacter for next iteration */
		lchar = nchar;
	}

	/* flush out data->slide */
	dwCounter+=w;
	CALL_SeqIO_GetSize(data->hOutputIO, &qwSize, IO_SIZE_TYPE_EXPLICIT);
	CALL_SeqIO_SetSize(data->hOutputIO, qwSize+w);

	CALL_SeqIO_Write(data->hOutputIO,NULL,data->slide,w);
	Delete(data, B_table);
	return dwCounter;
}


static tERROR LoadFollowers(UnreduceData* data, tBYTE* Slen, f_array *followers)
{
tINT x;
tINT i;
//tDWORD ret;

	for (x = 255; x >= 0; x--)
	{
//		CALL_SeqIO_ReadBit(data->hInputIO, &ret, 6);
//		Slen[x]=(tBYTE)ret;
		READBIT(6, Slen[x]);
		
		for (i = 0; (tBYTE) i < Slen[x]; i++)
		{
//			CALL_SeqIO_ReadBit(data->hInputIO, &ret,  8);
//			followers[x][i]=(tBYTE)ret;
			READBIT(8, followers[x][i]);
		}
	}
	return data->error;
}

tBYTE* New(UnreduceData* data, tDWORD dwBytes)
{
tBYTE * lpData;

	if(errOK!=CALL_SYS_ObjHeapAlloc(data->_Me, (tPTR*)&lpData, dwBytes))
	{
		data->error = errNOT_ENOUGH_MEMORY;
	}
	return lpData;
}


tVOID Delete(UnreduceData* data, tBYTE * lpdata)
{
	if(lpdata == NULL)
	{
		return ;
	}
	CALL_SYS_ObjHeapFree(data->_Me, lpdata);
}


