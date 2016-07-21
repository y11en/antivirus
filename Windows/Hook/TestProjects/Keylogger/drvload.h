#ifndef _DRVLOAD_H
#define _DRVLOAD_H
//-----------------------------------------------------------------------------------------------

class CDrvLoad
{
public:
	CDrvLoad() {};
	~CDrvLoad() {};

	BOOL Start(PTCHAR ServiceName, PTCHAR BinaryPath, PTCHAR DisplayName);
	BOOL Stop(PTCHAR ServiceName);
};
//-----------------------------------------------------------------------------------------------
#endif