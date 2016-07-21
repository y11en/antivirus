#ifndef __VER_MOD_H
#define __VER_MOD_H

#define VER_INTERNALNAME_STR        "FSSYNC"
#define VER_INTERNALEXTENSION_STR   "DLL"    //Optional - EXE or DLL will be added automatically.
//#define VER_FILEDESCRIPTION_STR     "Anti-Virus Test Application" //Optional - otherwise will be taken from VER_ORIGINALFILENAME_STR.
#define VER_FILEVERSION_BUILD        5     //Do not change this value 
                                           //until VER_PRODUCTVERSION_BUILD 
                                           //will be bigger than 5! 
                                           //
                                           //Because of the bug, fssync.dll 
                                           //version was 6.0.5.0 for a very 
                                           //long time. So in order for other 
                                           //products to replace it during 
                                           //the installation, the version for 
                                           //this dll is needed to be higher 
                                           //than 6.0.5


#include <version\ver_product.h>        //product version definitions

#endif// __VER_MOD_H
