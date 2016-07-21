#ifndef _KD3_H
#define _KD3_H
//----------------------------------------------------------------------------------------------

#define MAX_PROCS		1024

struct PROCESS
{
	HMODULE hModule;
	DWORD PID;
};

class KD3
{
private:
// список процессов и хэндлов инжектированных kd3dll.dll
	PROCESS Procs[MAX_PROCS];
	DWORD NumProcs;

// инжекция в АП процесса длл. возвращает ее hmodule.
	HMODULE InjectDLLProcess(DWORD ProcessID, CHAR *DLLName);
// удаление длл из АП процесса
	BOOL EjectDLLProcess(DWORD ProcessID, HMODULE hModule);
// получение SeDebugPrivalage для текущего процесса
	BOOL AcquireDebugPrivelage();
	BOOL CheckProcName(PTCHAR ProcName);
public:
	KD3()
	{
		NumProcs = 0;
	};

	~KD3() {};

	BOOL Start();

	void Stop();
};

//----------------------------------------------------------------------------------------------
#endif