#ifndef _cEnvironmentHelperWin32W_H
#define _cEnvironmentHelperWin32W_H

#include "../eventmgr/include/envhelper.h"

class cEnvironmentHelperWin32W : public cEnvironmentHelper 
{
public:
	cEnvironmentHelperWin32W();
	~cEnvironmentHelperWin32W();

	_IMPLEMENT( Alloc );
	_IMPLEMENT( Free );

	// environment
	_IMPLEMENT( ExpandEnvironmentStr );
	
	// Interlocked Variable Access
	_IMPLEMENT( InterlockedInc );
	_IMPLEMENT( InterlockedDec );
	_IMPLEMENT( InterlockedExchAdd );

	// Path 
	_IMPLEMENT( PathGetLong );
	_IMPLEMENT( PathIsNetwork );
	_IMPLEMENT( PathFindExecutable );

	// File operations
	_IMPLEMENT( FileAttr );
	_IMPLEMENT( FileOpen ); // read shared access
	_IMPLEMENT( FileClose );
	_IMPLEMENT( FileRead );
	_IMPLEMENT( FileSizeByHandle );
	_IMPLEMENT( FileSizeByName );

	// Registry
	_IMPLEMENT( Reg_OpenKey );
	_IMPLEMENT( Reg_EnumValue );
	_IMPLEMENT( Reg_QueryValueStr );
	_IMPLEMENT( Reg_CloseKey );

	// Process
	_IMPLEMENT( ProcessVirtualQuery );
	_IMPLEMENT( ProcessGetProcAddr );
};


#endif _cEnvironmentHelper_PDM_H