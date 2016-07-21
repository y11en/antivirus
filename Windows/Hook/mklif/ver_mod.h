#define VER_INTERNALNAME_STR		"KLIF" 
#define VER_FILEDESCRIPTION_STR		"Klif Mini-Filter"
#define VER_INTERNALEXTENSION_STR	"SYS"

#define VER_FILETYPE    VFT_DRV
#define VER_FILESUBTYPE VFT2_DRV_SYSTEM

#if DBG
#define VER_DEBUG                   VS_FF_DEBUG
#else
#define VER_DEBUG                   0
#endif

#define VER_FILEFLAGSMASK           VS_FFI_FILEFLAGSMASK
#define VER_FILEFLAGS               VER_DEBUG
#define VER_FILEOS                  VOS_NT_WINDOWS32

#define VER_FILEVERSION_HIGH            8
#define VER_FILEVERSION_LOW             0
#define VER_FILEVERSION_BUILD		0
#define VER_FILEVERSION_COMPILATION     60

#include <version/ver_product.h>
