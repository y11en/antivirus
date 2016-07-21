#ifndef _NPR_HASH_H_
#define _NPR_HASH_H_

#if !defined(_NO_PRAGUE_)

#define CALL_Hash_Create(parent,phHash) CALL_SYS_ObjectCreateQuick(parent, phHash, IID_HASH, PID_HASH_MD5, SUBTYPE_ANY)
#define CALL_Hash_Close(hHash)          CALL_SYS_ObjectClose(hHash)

#else // _NO_PRAGUE_

#include "../../hash/hash_api/hash_api.h"
#include "../../hash/hash_api/md5/hash_md5.h"

typedef unsigned char * hHASH;

#define CALL_Hash_Create(parent,phHash) \
        ( *(phHash) = (hHASH)malloc(hash_api_md5.ContextSize), \
	  hash_api_md5.Init(0, *(phHash), hash_api_md5.ContextSize, 16, NULL) ? \
		errOK : errUNEXPECTED \
	)

#define CALL_Hash_Close(hHash) \
        ( free(hHash) )

#define CALL_Hash_Update(hHash, buffer, size) \
	( hash_api_md5.Update(hHash, buffer, size) ? \
		errOK : errUNEXPECTED \
	)

#define CALL_Hash_GetHash(hHash, buffer, size) \
	( hash_api_md5.GetCurrentHash(hHash, &size, buffer) ? \
		errOK : errUNEXPECTED \
        )


#endif // _NO_PRAGUE_

#endif //_NPR_HASH_H_
