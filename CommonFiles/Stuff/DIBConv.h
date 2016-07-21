////////////////////////////////////////////////////////////////////
//
//  DIBConv.h
//  DDB to DIB and DIB to DDB conversion functions definition
//  AVP general purposes stuff
//  Victor Matiouchenkov [victor@avp.ru], Kaspersky Labs. 1999
//  Copyright (c) Kaspersky Labs
//
////////////////////////////////////////////////////////////////////
#ifndef __DIBCONV_H__
#define __DIBCONV_H__

#include <windows.h>

// DDBToDIB				- Creates a DIB from a DDB
// hBitmap				- Device dependent bitmap
// hPalette				- Logical palette
HANDLE DDBToDIB( HBITMAP hBitmap, HPALETTE hPalette, DWORD *pdwDIBSize );

// DIBToDDB	- Creates a DDB from a DIB
// hDIB   	- Device independent bitmap
HBITMAP DIBToDDB( HANDLE hDIB );


// DON'T try to use follow functions for converting small (16x16) icon.
// There is wrong processing for monochrome MaskBitmap of such icons.

// DDIToDIB				- Creates a DIB from a DDI
// hIcon   			  - Device dependent icon
// hPalette				- Logical palette
HANDLE DDIToDIB( HICON hIcon, HPALETTE hPalette, DWORD *pdwDIBSize );

// DIBToDDI	- Creates a DDI from a DIB
// hDIB   	- Device independent bitmap
HICON DIBToDDI( HANDLE hDIB );

#endif //__DIBCONV_H__
