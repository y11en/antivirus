#ifndef _AL_FILE_PRAGUE_H_092307092_3234
#define _AL_FILE_PRAGUE_H_092307092_3234

#include "al_file.h"

AL_DECLARE_INTERFACE(alFilePrague) AL_INTERFACE_BASED(alFile, file)
AL_DECLARE_INTERFACE_BEGIN
AL_DECLARE_INTERFACE_END

EXTERN_C alFilePrague* AL_CALLTYPE new_alFilePrague(hIO hIO, bool bAutoCloseIo);
EXTERN_C alFilePrague* AL_CALLTYPE new_alFilePragueTemp(hOBJECT parent);

#endif // _AL_FILE_PRAGUE_H_092307092_3234
