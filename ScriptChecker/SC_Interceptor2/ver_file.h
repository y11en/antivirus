////////////////////////////////////////////////////////////////////
//
//  VER_FILE.H
//  File Version definitions
//  Mike Pavlyushchik [mike@kaspersky.com], Kaspersky Labs. 2001
//  Copyright (c) Kaspersky Labs.
//
////////////////////////////////////////////////////////////////////

#ifndef __VER_FILE_H
#define __VER_FILE_H

#include "VER_PROD.H"

#define VER_FILEDESCRIPTION_STR     "Kaspersky Anti-Virus Script Checker Main Module"

#define VER_INTERNALNAME_STR        "AVPSCRCH"
#define VER_ORIGINALFILENAME_STR    "AVPSCRCH.DLL"

// #define VER_SPECIALBUILD_STR     ""
// #define VER_PRIVATEBUILD_STR     ""
// #define VER_COMMENTS_STR         ""

//// If file version undefined (commented out) then it will be equal product version, either file or product version must be defined
#define VER_FILEVERSION_HIGH          1
#define VER_FILEVERSION_LOW           0
#define VER_FILEVERSION_BUILD         3
#define VER_FILEVERSION_COMPILATION   342

///* ----- VS_VERSION.dwFileFlags ----- */
//#define VER_FILEFLAGS 
//                      VS_FF_DEBUG
//                      VS_FF_PRERELEASE
//                      VS_FF_PATCHED
//                      VS_FF_PRIVATEBUILD
//                      VS_FF_INFOINFERRED
//                      VS_FF_SPECIALBUILD

///* ----- VS_VERSION.dwFileOS ----- */
//#define VER_FILEOS
//                      VOS_UNKNOWN      
//                      VOS_DOS          
//                      VOS_OS216        
//                      VOS_OS232        
//                      VOS_NT           
//                      
//                      VOS__BASE        
//                      VOS__WINDOWS16   
//                      VOS__PM16        
//                      VOS__PM32        
//                      VOS__WINDOWS32   
//                      
//                      VOS_DOS_WINDOWS16
//                      VOS_DOS_WINDOWS32
//                      VOS_OS216_PM16   
//                      VOS_OS232_PM32   
//                      VOS_NT_WINDOWS32 

///* ----- VS_VERSION.dwFileType ----- */
#define VER_FILETYPE	VFT_DLL
//                      VFT_UNKNOWN
//                      VFT_APP
//                      VFT_DLL
//                      VFT_DRV
//                      VFT_FONT
//                      VFT_VXD
//                      VFT_STATIC_LIB

///* ----- VS_VERSION.dwFileSubtype for VFT_WINDOWS_DRV ----- */
//#define VER_FILESUBTYPE     
//                      -- for VFT_WINDOWS_DRV --
//                      VFT2_UNKNOWN        
//                      VFT2_DRV_PRINTER    
//                      VFT2_DRV_KEYBOARD   
//                      VFT2_DRV_LANGUAGE   
//                      VFT2_DRV_DISPLAY    
//                      VFT2_DRV_MOUSE      
//                      VFT2_DRV_NETWORK    
//                      VFT2_DRV_SYSTEM     
//                      VFT2_DRV_INSTALLABLE
//                      VFT2_DRV_SOUND      
//                      VFT2_DRV_COMM       
//                      VFT2_DRV_INPUTMETHOD
//                      -- for VFT_WINDOWS_FONT --
//                      VFT2_FONT_RASTER  
//                      VFT2_FONT_VECTOR  
//                      VFT2_FONT_TRUETYPE

#endif // __VER_FILE_H
