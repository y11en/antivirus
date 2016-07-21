/*******************************************************************************
  Proj: libosr (Optical Symbols Recognition library)
  --------------------------------------------------------------------------- 
  File: osr_error.cpp
  Created: 2006/09/08
  Desc: see osr_error.h
  --------------------------------------------------------------------------- 
  Evgeny P. Smirnov, Evgeny.Smirnov@kaspersky.com
********************************************************************************/
/*--------------------------------------------------------------------------------------------------------------------*/
#include "osr_error.h"
/*--------------------------------------------------------------------------------------------------------------------*/
const char* ErrorText (const enum ErrCode err)
{
    switch(err)
    {
    case OSR_OK:
        return "no errors";
    case OSR_INVALID_ARGUMENT:
        return "invalid argument";
    case OSR_INVALID_DATA:
        return "invalid data";
    case OSR_INTERNAL_ERROR:
        return "internal error";
    case OSR_MEMORY_FULL:
        return "memory full";
    case OSR_REJECTION:
        return "rejection";
    case OSR_IMAGE_TOO_SMALL:
        return "image too small";
    case OSR_IMAGE_TOO_LARGE:
        return "image too large";
    case OSR_FILE_ERROR:
        return "file error";
    case OSR_FILE_TOO_LARGE:
        return "image file too large";
    case OSR_IMAGE_FORMAT_ERROR:
        return "image format error";
    case OSR_WRONG_FRAME_INDEX:
        return "wrong frame index (of animated image)";
    case OSR_ERROR:
    default:
        return "error";
    }
}
/*--------------------------------------------------------------------------------------------------------------------*/
