#pragma once

class Virt
{
public:
	Virt(void);
	~Virt(void);

	HRESULT Init();
	
	HRESULT 
	AddApplToSB(
		__in PWCHAR dosAppPath,
		__in PWCHAR dosSBPath
	);

	HRESULT Apply();

	HRESULT Reset();

private:
	char* pClientContext;
	ULONG ApiVersion;
	ULONG AppId;
	ULONG FltId ;
	HANDLE ThHandle;
	DWORD dwThreadID;
};
