#ifndef __objs_h
#define __objs_h

#include <prague.h>
#include "../pr_client.h"

// ---
#define ROOT(o)   ((hROOT)OBJ(o))
#define REG(o)    ((hREGISTRY)(o))
#define STRING(o) ((hSTRING)OBJ(o))
#define IFENUM(o) ((hIFACE_ENUM)OBJ(o))
#define IO(o)     ((hIO)OBJ(o))
#define ObjPtr(o) ((hObjPtr)OBJ(o))
#define OS(o)     ((hOS)OBJ(o))
#define ENGINE(o) ((hENGINE)OBJ(o))
#define BL(o)     ((hBL)OBJ(o))

// ---
typedef tERROR (pr_call *t_dummy_func)( hOBJECT );

// ---
tERROR pr_call not_implemented( hOBJECT );
tERROR pr_call not_supported_but_ok( hOBJECT );
tERROR pr_call not_supported( hOBJECT );

  

#endif // __rpc_objs_h