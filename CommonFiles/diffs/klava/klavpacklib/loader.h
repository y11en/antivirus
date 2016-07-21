#ifndef _KLAVPACK_LOADER_H_
#define _KLAVPACK_LOADER_H_  "Assembly loader wrapper"

#include <stdlib.h>
#include <memory.h>
#include "types.h"

/* ------------------------------------------------------------------------- */
#include <kl_pushpack.h>
/* ------------------------------------------------------------------------- */

typedef struct _ASMLDR_SECTION {
  tDWORD  SectionRVA;
  tDWORD  PackedSize;
  tDWORD  UnpackedSize;
} KL_PACKED ASMLDR_SECTION;

#define ASMLDR_MAX_SECTIONS 15

typedef struct _ASMLDR_DATA {
  tWORD   LoaderVersion;
  tWORD   LoaderFlags;
  tDWORD  ImageBase;
  tDWORD  ModuleInstance;
  tDWORD  LoaderRVA;
  tDWORD  MaxPackedSize;
  tDWORD  HeaderDataRVA;
  tDWORD  HeaderDataPLen;
  tDWORD  HeaderDataULen;
  tDWORD  TLSDataRVA;
  tDWORD  TLSDataLen;
  tDWORD  TLSDataVal;
  tDWORD  OriginalEntryRVA;
  tDWORD  OriginalImportRVA;
  tDWORD  OriginalFixUpsRVA;
  tDWORD  OriginalFixUpsLen;
  tDWORD  Reserved[4];
  ASMLDR_SECTION Section[ASMLDR_MAX_SECTIONS+1];
} KL_PACKED ASMLDR_DATA;

typedef struct _ASMLDR_HEADER {
  tBYTE   Prologue[4];
  tDWORD  HdrDeltaSize;
  ASMLDR_DATA  LdrData;
} KL_PACKED ASMLDR_HEADER;

/* ------------------------------------------------------------------------- */
#include <kl_poppack.h>
/* ------------------------------------------------------------------------- */

class CKlavPackLoader
{
public:
  CKlavPackLoader();
 ~CKlavPackLoader();

  tBYTE* GetLoaderBody(tUINT* LoaderSize);

private:
  tBYTE* LoaderBody;
  tUINT  LoaderBodySize;
};

/* ------------------------------------------------------------------------- */

#endif /* _KLAVPACK_LOADER_H_ */
