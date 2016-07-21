#ifndef __crypt_h
#define __crypt_h



#define IZ_PWLEN  80    // input buffer size for reading encryption key 
#ifndef PWLEN           // for compatibility with previous zcrypt release... 
#  define PWLEN IZ_PWLEN
#endif
 
#define RAND_HEAD_LEN  12       // length of encryption random header

#ifndef MAX_PATH
#define MAX_PATH 512
#endif
// --------------------------------------------------------------------------------
// AVP Prague stamp begin( IO, Data structure )
// Interface "IO". Inner data structure
typedef struct tag_DMapData 
{
	tCHAR   szName[MAX_PATH]; // -- 
	tCHAR   szPath[MAX_PATH]; // -- 
	tQWORD  qwStartOffset; // -- 
	tQWORD  qwAreaSize; // -- 
	hIO     hIoOrigin; // -- 
	hOBJECT hPropObject; // -- 
	tUINT   dwAccessMode; // -- 
	tDWORD  dwOpenMode;
	tCHAR*  lpszPassword;
	hHEAP   hHeap;
	tERROR  error;
	tCHAR*  key;
	tDWORD  crc32;
	tWORD   last_mod_file_time;
	tWORD   last_mod_file_date;
	tWORD   general_purpose_bit_flag;
	tQWORD  qwLastPosition;
	tDWORD  keys[3];     /* keys defining the pseudo-random sequence */
	tBYTE   h[RAND_HEAD_LEN];

} DMapData;
// AVP Prague stamp end( IO, Data structure )
// --------------------------------------------------------------------------------

tINT decrypt(DMapData* data, tCHAR* passwrd);
tINT  decrypt_byte (DMapData* data);
tINT  update_keys (DMapData* data, tINT c);
void init_keys (DMapData* data, tCHAR *passwd);


#endif /* !__crypt_h */
