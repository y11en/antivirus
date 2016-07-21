#ifndef _AL_FILE_WIN_H_4541231843434_2158123
#define _AL_FILE_WIN_H_4541231843434_2158123

#include "al_file.h"

AL_DECLARE_INTERFACE(alFileStd) AL_INTERFACE_BASED(alFile, file)
AL_DECLARE_INTERFACE_BEGIN
AL_DECLARE_METHOD3(bool, fsopen, const char *filename, const char *mode, int shflag);
AL_DECLARE_METHOD3(bool, wfsopen, const wchar_t *filename, const wchar_t *mode, int shflag);
AL_DECLARE_INTERFACE_END

EXTERN_C alFileStd* AL_CALLTYPE new_alFileStd();

#endif // _AL_FILE_WIN_H_4541231843434_2158123
