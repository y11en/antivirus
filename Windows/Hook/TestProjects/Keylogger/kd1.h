#ifndef _KD1_H
#define _KD1_H
//----------------------------------------------------------------------------------------------
#include "kd1dll\kd1dll.h"

class KD1
{
public:
	KD1() {};

	~KD1() {};

	BOOL Start(HMODULE hKD1DLL)
	{
// тут все просто..
		fSetGlobalHook p_fSetGlobalHook = (fSetGlobalHook)GetProcAddress(hKD1DLL, "SetGlobalHook");
		if (!p_fSetGlobalHook)
			return FALSE;

		return p_fSetGlobalHook(hKD1DLL);
	}

	void Stop(HMODULE hKD1DLL)
	{
		fUnsetGlobalHook p_fUnsetGlobalHook = (fUnsetGlobalHook)GetProcAddress(hKD1DLL, "UnsetGlobalHook");
		if (p_fUnsetGlobalHook)
			p_fUnsetGlobalHook();
	}
};

//----------------------------------------------------------------------------------------------
#endif