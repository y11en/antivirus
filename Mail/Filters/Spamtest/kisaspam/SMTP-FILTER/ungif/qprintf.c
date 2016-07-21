/*****************************************************************************
*   "Gif-Lib" - Yet another gif library.				     *
*									     *
* Written by:  Gershon Elber			IBM PC Ver 0.1,	Jun. 1989    *
******************************************************************************
* Module to emulate a printf with a possible quiet (disable mode.)           *
* A global variable GifQuietPrint controls the printing of this routine      *
******************************************************************************
* History:								     *
* 12 May 91 - Version 1.0 by Gershon Elber.				     *
*****************************************************************************/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>

#ifdef MEMORY_LEAKS_SEARCH
#ifdef _DEBUG
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif
#endif

#ifdef USE_CUSTOM_HEAP
#include <win32-filter/include/mem_heap.h>
#endif


#include "gif_lib.h"

int GifQuietPrint=0;
/*****************************************************************************
* Same as fprintf to stderr but with optional print.			     *
*****************************************************************************/
void GifQprintf(char *Format, ...)
{
}
