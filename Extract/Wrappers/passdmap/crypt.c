#include <Prague/prague.h>

#include "dmap.h"
#include "crypt.h"

#include <string.h>

   /* For the encoding task used in Zip (and ZipCloak), we want to initialize
      the crypt algorithm with some reasonably unpredictable bytes, see
      the crypthead() function. The standard rand() library function is
      used to supply these `random' bytes, which in turn is initialized by
      a srand() call. The srand() function takes an "unsigned" (at least 16bit)
      seed value as argument to determine the starting point of the rand()
      pseudo-random number generator.
      This seed number is constructed as "Seed = Seed1 .XOR. Seed2" with
      Seed1 supplied by the current time (= "(unsigned)time()") and Seed2
      as some (hopefully) nondeterministic bitmask. On many (most) systems,
      we use some "process specific" number, as the PID or something similar,
      but when nothing unpredictable is available, a fixed number may be
      sufficient.
      NOTE:
      1.) This implementation requires the availability of the following
          standard UNIX C runtime library functions: time(), rand(), srand().
          On systems where some of them are missing, the environment that
          incorporates the crypt routines must supply suitable replacement
          functions.
      2.) It is a very bad idea to use a second call to time() to set the
          "Seed2" number! In this case, both "Seed1" and "Seed2" would be
          (almost) identical, resulting in a (mostly) "zero" constant seed
          number passed to srand().

      The implementation environment defined in the "zip.h" header should
      supply a reasonable definition for ZCR_SEED2 (an unsigned number; for
      most implementations of rand() and srand(), only the lower 16 bits are
      significant!). An example that works on many systems would be
           "#define ZCR_SEED2  (unsigned)getpid()".
      The default definition for ZCR_SEED2 supplied below should be regarded
      as a fallback to allow successful compilation in "beta state"
      environments.
    */
//#include <time.h>     /* time() function supplies first part of crypt seed */

   /* "last resort" source for second part of crypt seed pattern */
#define ZCR_SEED2 (unsigned)3141592654L     /* use PI as default pattern */

tINT testp (DMapData* data, tBYTE *h);
tINT testkey (DMapData* data, tBYTE *h,  tCHAR *key);
tVOID undefer_input(tVOID);
void defer_leftover_input(tVOID);

static tDWORD crc_32_tab[256] =
{
	0x00000000L, 0x77073096L, 0xee0e612cL, 0x990951baL, 0x076dc419L,
	0x706af48fL, 0xe963a535L, 0x9e6495a3L, 0x0edb8832L, 0x79dcb8a4L,
	0xe0d5e91eL, 0x97d2d988L, 0x09b64c2bL, 0x7eb17cbdL, 0xe7b82d07L,
	0x90bf1d91L, 0x1db71064L, 0x6ab020f2L, 0xf3b97148L, 0x84be41deL,
	0x1adad47dL, 0x6ddde4ebL, 0xf4d4b551L, 0x83d385c7L, 0x136c9856L,
	0x646ba8c0L, 0xfd62f97aL, 0x8a65c9ecL, 0x14015c4fL, 0x63066cd9L,
	0xfa0f3d63L, 0x8d080df5L, 0x3b6e20c8L, 0x4c69105eL, 0xd56041e4L,
	0xa2677172L, 0x3c03e4d1L, 0x4b04d447L, 0xd20d85fdL, 0xa50ab56bL,
	0x35b5a8faL, 0x42b2986cL, 0xdbbbc9d6L, 0xacbcf940L, 0x32d86ce3L,
	0x45df5c75L, 0xdcd60dcfL, 0xabd13d59L, 0x26d930acL, 0x51de003aL,
	0xc8d75180L, 0xbfd06116L, 0x21b4f4b5L, 0x56b3c423L, 0xcfba9599L,
	0xb8bda50fL, 0x2802b89eL, 0x5f058808L, 0xc60cd9b2L, 0xb10be924L,
	0x2f6f7c87L, 0x58684c11L, 0xc1611dabL, 0xb6662d3dL, 0x76dc4190L,
	0x01db7106L, 0x98d220bcL, 0xefd5102aL, 0x71b18589L, 0x06b6b51fL,
	0x9fbfe4a5L, 0xe8b8d433L, 0x7807c9a2L, 0x0f00f934L, 0x9609a88eL,
	0xe10e9818L, 0x7f6a0dbbL, 0x086d3d2dL, 0x91646c97L, 0xe6635c01L,
	0x6b6b51f4L, 0x1c6c6162L, 0x856530d8L, 0xf262004eL, 0x6c0695edL,
	0x1b01a57bL, 0x8208f4c1L, 0xf50fc457L, 0x65b0d9c6L, 0x12b7e950L,
	0x8bbeb8eaL, 0xfcb9887cL, 0x62dd1ddfL, 0x15da2d49L, 0x8cd37cf3L,
	0xfbd44c65L, 0x4db26158L, 0x3ab551ceL, 0xa3bc0074L, 0xd4bb30e2L,
	0x4adfa541L, 0x3dd895d7L, 0xa4d1c46dL, 0xd3d6f4fbL, 0x4369e96aL,
	0x346ed9fcL, 0xad678846L, 0xda60b8d0L, 0x44042d73L, 0x33031de5L,
	0xaa0a4c5fL, 0xdd0d7cc9L, 0x5005713cL, 0x270241aaL, 0xbe0b1010L,
	0xc90c2086L, 0x5768b525L, 0x206f85b3L, 0xb966d409L, 0xce61e49fL,
	0x5edef90eL, 0x29d9c998L, 0xb0d09822L, 0xc7d7a8b4L, 0x59b33d17L,
	0x2eb40d81L, 0xb7bd5c3bL, 0xc0ba6cadL, 0xedb88320L, 0x9abfb3b6L,
	0x03b6e20cL, 0x74b1d29aL, 0xead54739L, 0x9dd277afL, 0x04db2615L,
	0x73dc1683L, 0xe3630b12L, 0x94643b84L, 0x0d6d6a3eL, 0x7a6a5aa8L,
	0xe40ecf0bL, 0x9309ff9dL, 0x0a00ae27L, 0x7d079eb1L, 0xf00f9344L,
	0x8708a3d2L, 0x1e01f268L, 0x6906c2feL, 0xf762575dL, 0x806567cbL,
	0x196c3671L, 0x6e6b06e7L, 0xfed41b76L, 0x89d32be0L, 0x10da7a5aL,
	0x67dd4accL, 0xf9b9df6fL, 0x8ebeeff9L, 0x17b7be43L, 0x60b08ed5L,
	0xd6d6a3e8L, 0xa1d1937eL, 0x38d8c2c4L, 0x4fdff252L, 0xd1bb67f1L,
	0xa6bc5767L, 0x3fb506ddL, 0x48b2364bL, 0xd80d2bdaL, 0xaf0a1b4cL,
	0x36034af6L, 0x41047a60L, 0xdf60efc3L, 0xa867df55L, 0x316e8eefL,
	0x4669be79L, 0xcb61b38cL, 0xbc66831aL, 0x256fd2a0L, 0x5268e236L,
	0xcc0c7795L, 0xbb0b4703L, 0x220216b9L, 0x5505262fL, 0xc5ba3bbeL,
	0xb2bd0b28L, 0x2bb45a92L, 0x5cb36a04L, 0xc2d7ffa7L, 0xb5d0cf31L,
	0x2cd99e8bL, 0x5bdeae1dL, 0x9b64c2b0L, 0xec63f226L, 0x756aa39cL,
	0x026d930aL, 0x9c0906a9L, 0xeb0e363fL, 0x72076785L, 0x05005713L,
	0x95bf4a82L, 0xe2b87a14L, 0x7bb12baeL, 0x0cb61b38L, 0x92d28e9bL,
	0xe5d5be0dL, 0x7cdcefb7L, 0x0bdbdf21L, 0x86d3d2d4L, 0xf1d4e242L,
	0x68ddb3f8L, 0x1fda836eL, 0x81be16cdL, 0xf6b9265bL, 0x6fb077e1L,
	0x18b74777L, 0x88085ae6L, 0xff0f6a70L, 0x66063bcaL, 0x11010b5cL,
	0x8f659effL, 0xf862ae69L, 0x616bffd3L, 0x166ccf45L, 0xa00ae278L,
	0xd70dd2eeL, 0x4e048354L, 0x3903b3c2L, 0xa7672661L, 0xd06016f7L,
	0x4969474dL, 0x3e6e77dbL, 0xaed16a4aL, 0xd9d65adcL, 0x40df0b66L,
	0x37d83bf0L, 0xa9bcae53L, 0xdebb9ec5L, 0x47b2cf7fL, 0x30b5ffe9L,
	0xbdbdf21cL, 0xcabac28aL, 0x53b39330L, 0x24b4a3a6L, 0xbad03605L,
	0xcdd70693L, 0x54de5729L, 0x23d967bfL, 0xb3667a2eL, 0xc4614ab8L,
	0x5d681b02L, 0x2a6f2b94L, 0xb40bbe37L, 0xc30c8ea1L, 0x5a05df1bL,
	0x2d02ef8dL
};


#define CRC32(c, b) (crc_32_tab[((tINT)(c) ^ (b)) & 0xff] ^ ((c) >> 8))

/***********************************************************************
 * Return the next byte in the pseudo-random sequence
 */

tINT decrypt_byte(DMapData* data)
    
{
tUINT temp;

	temp = ((tUINT)data->keys[2] & 0xffff) | 2;
	return (tINT)(((temp * (temp ^ 1)) >> 8) & 0xff);
}

tBYTE* New(DMapData* data, tDWORD dwBytes)
{
tBYTE * lpData;

	if(errOK!=CALL_Heap_Alloc(data->hHeap, (tPTR*)&lpData, dwBytes))
	{
		data->error=errNOT_ENOUGH_MEMORY;
	}
	return lpData;
}


tVOID Free(DMapData* data, tBYTE * lpdata)
{
	if(lpdata==NULL)
	{
		return ;
	}
	CALL_Heap_Free(data->hHeap, lpdata);
}


/***********************************************************************
 * Update the encryption keys with the next byte of plain text
    tINT c -- byte of plain text 
 */
tINT update_keys(DMapData* data,tINT  c)
{
tINT keyshift;
	data->keys[0] = CRC32(data->keys[0], c);
	data->keys[1] += data->keys[0] & (tDWORD)0xff;
	data->keys[1] = data->keys[1] * 134775813L + 1;
	keyshift = (tINT)(data->keys[1] >> 24);
	data->keys[2] = CRC32(data->keys[2], keyshift);
	return c;
}


/***********************************************************************
 * Initialize the encryption keys and the random header according to
 * the given password.
 passwd password string with which to modify keys 
 */
tVOID init_keys(DMapData* data,tCHAR * passwd)
{
	data->keys[0] = 305419896L;
	data->keys[1] = 591751049L;
	data->keys[2] = 878082192L;
	while (*passwd != '\0')
	{
		update_keys(data, (tINT)*passwd);
		passwd++;
	}
}


/***********************************************************************
 * Get the password and set up keys for current zipfile member.
 * Return PK_ class error.
 */
tINT decrypt(DMapData* data, tCHAR* passwrd)
{
tERROR error;

	error=errDECRIPT_WARNING;
	if (passwrd != (tCHAR *)NULL)
	{ // user gave password on command line 
		if (!data->key)
		{
			tDWORD keylen = _toui32(strlen(passwrd)+1);
			if ((data->key = (tCHAR *)New(data, keylen)) == (tCHAR *)NULL)
				return errNOT_ENOUGH_MEMORY;
			strcpy_s (data->key, keylen, passwrd);
		}
	}

	if (data->key)
	{
		if (!testp(data, data->h))
			error=errOK;
	}
	if (data->key)
	{
		Free(data, data->key);
		data->key = (tCHAR *)NULL;
	}
	return error;
}


/* codepage conversion setup for testp() in crypt.c */
/*
#ifdef CRTL_CP_IS_ISO
#  ifndef STR_TO_CP2
#    define STR_TO_CP2  STR_TO_OEM
#  endif
#else
#  ifdef CRTL_CP_IS_OEM
#    ifndef STR_TO_CP2
#      define STR_TO_CP2  STR_TO_ISO
#    endif
#  else // native internal CP is neither ISO nor OEM 
#    ifndef STR_TO_CP1
#      define STR_TO_CP1  STR_TO_ISO
#    endif
#    ifndef STR_TO_CP2
#      define STR_TO_CP2  STR_TO_OEM
#    endif
#  endif
#endif
*/

//#  define STR_TO_CP2(dst, src)  (AnsiToOem(src, dst), dst)

/***********************************************************************
 * Test the password.  Return -1 if bad, 0 if OK.
 */
tINT testp(DMapData* data, tBYTE * h)
{
tINT r;
//tCHAR *key_translated;

    /* On systems with "obscure" native character coding (e.g., EBCDIC),
     * the first test translates the password to the "main standard"
     * character coding. */

#ifdef STR_TO_CP1
	/* allocate buffer for translated password */
	if ((key_translated = New(data, strlen(data->key) + 1)) == (tCHAR *)NULL)
		return -1;
	/* first try, test password translated "standard" charset */
	r = testkey(data, h, STR_TO_CP1(key_translated, data->key));
#else /* !STR_TO_CP1 */
	/* first try, test password as supplied on the extractor's host */
	r = testkey(data, h, data->key);
#endif /* ?STR_TO_CP1 */

#ifdef STR_TO_CP2
	if (r != 0)
	{
#ifndef STR_TO_CP1
		/* now prepare for second (and maybe third) test with translated pwd */
		if ((key_translated = malloc(strlen(key) + 1)) == (tCHAR *)NULL)
			return -1;
#endif
		/* second try, password translated to alternate ("standard") charset */
		r = testkey(data, h, STR_TO_CP2(key_translated, data->key));
#ifdef STR_TO_CP3
		if (r != 0)
		/* third try, password translated to another "standard" charset */
			r = testkey(data, h, STR_TO_CP3(key_translated, data->key));
#endif
#ifndef STR_TO_CP1
		free(key_translated);
#endif
	}
#endif /* STR_TO_CP2 */

#ifdef STR_TO_CP1
	Free(data, key_translated);
	if (r != 0)
	{
		/* last resort, test password as supplied on the extractor's host */
		r = testkey(data, h, data->key);
	}
#endif /* STR_TO_CP1 */

	return r;

}


tINT testkey(DMapData* data, tBYTE * h,tCHAR * key)
{
tWORD b;
tINT n;
//tDWORD date;
tBYTE hh[RAND_HEAD_LEN]; // decrypted header

    // set keys and save the encrypted header
	init_keys(data,  data->key);
	memcpy(hh, h, RAND_HEAD_LEN);

	// check password
	for (n = 0; n < RAND_HEAD_LEN; n++)
	{
		update_keys(data,hh[n] ^= decrypt_byte(data));
//		Trace((stdout, " %02x", hh[n]));
	}

//	Trace((stdout,	"\n  lrec.crc= %08lx  crec.crc= %08lx  pInfo->ExtLocHdr= %s\n",lrec.crc32, pInfo->crc,pInfo->ExtLocHdr ? "true":"false"));
//	Trace((stdout, "  incnt = %d  unzip offset into zipfile = %ld\n",incnt,cur_zipfile_bufstart+(inptr-inbuf)));


	b = hh[RAND_HEAD_LEN-1];
//	Trace((stdout, "  b = %02x  (crc >> 24) = %02x  (lrec.time >> 8) = %02x\n",b, (tWORD)(lrec.crc32 >> 24),((tWORD)lrec.last_mod_dos_datetime >> 8) & 0xff));
//	if (b != (pInfo->ExtLocHdr ? ((tWORD)data->header.last_mod_dos_datetime >> 8) & 0xff : (tWORD)(data->header.crc32 >> 24)))
	//date=(((tDWORD)data->last_mod_file_time)<<16)+data->last_mod_file_date;
	if(data->general_purpose_bit_flag & 8)
	{
		if (b != (((tWORD)data->last_mod_file_time >> 8) & 0xff))
		//if (b != (((tWORD)date >> 8) & 0xff))
			return -1;
	}
	else
	{
		if (b!=(tWORD)(data->crc32 >> (tWORD)24))
			return -1;  // bad 
	}
	return errOK; 
}


