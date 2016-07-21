#ifndef __DTDROPPER_H__
#define __DTDROPPER_H__


#include "property/property.h"

bool DropDTToFileInitLibrary( void* (*pfMemAlloc)(unsigned int), void (*pfMemFree)(void*) );

bool DropDTToFile( HDATA hRootData, const char *pszFileName );

#endif