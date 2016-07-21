#include <iostream>

#define _USE_VTBL

#include "sampleplugin.h"
#include "sampleimpl.h"

extern "C" {
DATA_STRUCT_BEGIN(SampleImpl)
	int userField;
DATA_STRUCT_END(SampleImpl)

hROOT  g_root = NULL;
}

#include "sampleimpl.c"

tERROR pr_call SampleImpl_ObjectInit(hSampleImpl _this ) {
	return 0;
}

tERROR pr_call SampleImpl_baseMethod (hi_SampleImpl _this, hSampleI prm ) {
	std::cout << "> SampleImpl_baseMethod called" << std::endl;

	if (_this->data->field0 == 1234) {
		return -1;
	}
	return 0;
}

tERROR pr_call SampleImpl_iMethod (hi_SampleImpl _this) {
	return 0;
}

tERROR pr_call SampleImpl_addMethod (hi_SampleImpl _this) {
	return 0;
}

tERROR pr_call SampleImpl_getImplProp( hi_SampleImpl _this, tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size ) {
	return 0;
}

tERROR pr_call SampleImpl_setImplProp( hi_SampleImpl _this, tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size ) {
	return 0;
}

extern "C" tDWORD pr_vsprintf( tCHAR* buffer, tDWORD length, const tSTRING format, tPTR arg_ptr ) {
	return 0;
}

tBOOL __stdcall DllMain( tPTR hInstance, tDWORD dwReason, tERROR* pError ) {
	// tDWORD count;

	switch( dwReason ) {
		case DLL_PROCESS_ATTACH:
			PR_TRACE(( g_root, prtERROR, "attach \"SampleI\" interface"));
			//printf("test attach\n");
		case DLL_PROCESS_DETACH:
		case DLL_THREAD_ATTACH :
		case DLL_THREAD_DETACH :
			break;

		case PRAGUE_PLUGIN_LOAD :
			g_root = (hROOT)hInstance;
			*pError = errOK;

			//__asm int 3
			
			PR_TRACE(( g_root, prtERROR, "try to register \"SampleI\" interface"));
			// register my interfaces
			if ( PR_FAIL(*pError=SampleImpl_Register(g_root)) ) {
				PR_TRACE(( g_root, prtERROR, "cannot register \"SampleI\" interface"));
				return cFALSE;
			}
			break;

		case PRAGUE_PLUGIN_UNLOAD :
			// free system resources
			// unregister my custom property ids -- you can drop it, kernel do it by itself
			// release    my imports		         -- you can drop it, kernel do it by itself
			// unregister my exports		         -- you can drop it, kernel do it by itself
			// unregister my interfaces          -- you can drop it, kernel do it by itself
			g_root = NULL;
			break;
	}
	return cTRUE;
}
