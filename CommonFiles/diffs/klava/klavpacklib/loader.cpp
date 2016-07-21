#include <stdlib.h>
#include <memory.h>
#include "loader.h"
#include "types.h"

/* loader binary */
#include "asm_loader.h"

/* ------------------------------------------------------------------------- */

CKlavPackLoader::CKlavPackLoader()
{
  LoaderBody = NULL;
  LoaderBodySize = 0;
}

CKlavPackLoader::~CKlavPackLoader()
{
  if ( LoaderBody )
  {
    free(LoaderBody);
    LoaderBody = NULL;
    LoaderBodySize = 0;
  }
}

/* ------------------------------------------------------------------------- */

tBYTE* CKlavPackLoader::GetLoaderBody(tUINT* LoaderSize)
{
  if ( !LoaderBody )
  {
    /* check structure size */
    ASMLDR_HEADER* LdrHdr = (ASMLDR_HEADER*)(asm_loader_binary);
    if ( LdrHdr->HdrDeltaSize != sizeof(ASMLDR_DATA) )
      return(NULL); /* wrong structures alignment */

    LoaderBodySize = sizeof(asm_loader_binary);
    if ( NULL == (LoaderBody = (tBYTE*)malloc(LoaderBodySize)) )
      return(NULL);

    memcpy(LoaderBody, asm_loader_binary, LoaderBodySize);
  }

  *LoaderSize = LoaderBodySize;
  return(LoaderBody);
}

/* ------------------------------------------------------------------------- */
