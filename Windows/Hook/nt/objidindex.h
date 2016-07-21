#ifndef __OBJID_INDEX_H_
#define __OBJID_INDEX_H_

#include "fidbox.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct tag_OBJID_CTX OBJID_CTX;

#define OBJID_FLAG_FLUSH			0x001
#define OBJID_FLAG_DONTCLOSE_FILE	0x002

HRESULT
OBJID_Init (
	tHeap* pHeap,
	tFile* pFileData,
	tFile* pFileIndex,
	size_t dwRecordSize,
	OBJID_CTX** ppObjIdCtx,
	ULONG* pCurrentItemSize
	);

HRESULT
OBJID_Done (
	OBJID_CTX* pObjIdCtx,
	size_t dwFlags
	); // OBJID_FLAG_xxx

HRESULT
OBJID_UpdateCurrentDate (
	OBJID_CTX* pObjIdCtx
	);

HRESULT
OBJID_GetObjectData (
	OBJID_CTX* pObjIdCtx,
	const BYTE VolumeId[16],
	const BYTE ObjectId[16],
	PVOID pDataBuffer,
	size_t dwBufferSize,
	size_t* pdwBytesRead
	);

HRESULT
OBJID_SetObjectData (
	OBJID_CTX* pObjIdCtx,
	const BYTE VolumeId[16],
	const BYTE ObjectId[16],
	PVOID pDataBuffer,
	size_t dwBufferSize,
	size_t* pdwBytesWritten
	);

HRESULT
OBJID_Flush (
	OBJID_CTX* pObjIdCtx
	);

#ifdef __cplusplus
} // extern "C"
#endif


#endif // __OBJID_INDEX_H_