#include "Logger.h"

#include "include\ver_mod.h"
#include <boost/shared_ptr.hpp>
#include <iostream>

#include <crtdbg.h>
//UPD_VER_MAIN

using namespace boost;

Logger::Step::Step ()
{

}

void Logger::Step::Clear ()
{
	comments.clear();
}

void Logger::Step::SetName (const wstring& _name)
{
	name = _name;
}

void Logger::Step::SetTime ()
{
	GetLocalTime (&time);
	tick = GetTickCount ();
}

const wchar_t* Logger::Step::GetName ()
{
	return name.c_str();
}

void Logger::Step::AddPoint (wstring comm)
{
	int ind = comm.find (L"\n", 0);
	if (ind != wstring::npos) comm.replace (ind, 2, L"");
	comments.push_back (comm);
}

bool Logger::Step::IsEmpty ()
{
	return comments.empty();
}

Logger::Test::Test (const wstring& name): name(name)
{
	GetLocalTime (&time);
	tick = GetTickCount ();
}

void Logger::Test::AddStep (const Step& step)
{
	steps.push_back (step);
};

const wchar_t* Logger::Test::GetName ()
{
	return name.c_str();
}

Logger::Case::Case (Logger& logger, TestP& test, Step& step, const wstring& name): m_step (step), m_test (test), m_logger (logger)
{
	m_step.Clear();
	m_step.SetName (name);
	m_step.SetTime ();
}

Logger::Case::~Case ()
{
	m_logger.AddStep(m_test, m_step);
}
	
Logger& Logger::GetInstance()
{
	static Logger _logger;
	return _logger;
}

void Logger::AddTest (const TestP& tp)
{
	tests.push_back (tp);
}	

void Logger::AddStep (const TestP& tp, const Step& sp)
{
	vector<TestP>::iterator test_it = find (tests.begin(), tests.end(), tp);
	_ASSERT (test_it != tests.end());
	(*test_it)->AddStep (sp);
}

Logger::~Logger ()
{
	Initialize ();
	wchar_t buf[4096];
	FILE* file = _wfopen (TDIMPORT_FILE, L"wb+");
	for (vector<TestP>::iterator it = tests.begin(); it < tests.end(); ++it)
	{
		swprintf (buf, L"TEST\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%02d.%02d.%4d\t%02d:%02d:%02d\x0d\x0a", user.c_str(), os.c_str(),
			UPD_VER_MAIN, host.c_str(), L"Updater 1.3", (*it)->GetName(), os.c_str(),
			(*it)->time.wMonth, (*it)->time.wDay, (*it)->time.wYear, (*it)->time.wHour, (*it)->time.wMinute, (*it)->time.wSecond);			fwrite (buf, 1, wcslen(buf), file);
			
		bool fl = true;
		for (vector<Step>::iterator it2 = (*it)->steps.begin(); it2 < (*it)->steps.end(); ++it2)
		{
			if (it2->comments.empty())
			{
				swprintf (buf, L"STEP\t%s\t%02d.%02d.%4d\t%02d:%02d:%02d\tPassed\tAllDone\x0d\x0a", it2->GetName(),
						 it2->time.wMonth, it2->time.wDay, it2->time.wYear, it2->time.wHour, it2->time.wMinute, it2->time.wSecond);
			}
			else
			{
				wstring reason = *(it2->comments.begin());
				for (vector<wstring>::iterator it3 = it2->comments.begin()+1;
						it3 < it2->comments.end(); ++it3)
						{
							reason.append (L", L"). append (*it3);
						}
				swprintf (buf, L"STEP\t%s\t%02d.%02d.%4d\t%02d:%02d:%02d\tFailed\t%s\x0d\x0a", it2->GetName(), 
						 it2->time.wMonth, it2->time.wDay, it2->time.wYear, it2->time.wHour, it2->time.wMinute, it2->time.wSecond, reason.c_str());
				fl = false;
			}
			fwrite (buf, 1, wcslen(buf), file);		
		}
		wstring verdict;
		if (fl) verdict = L"Passed"; else verdict = L"Failed";
		int duration = 0;
		if (!(*it)->steps.empty()) duration = (((*it)->steps.end()-1)->tick - (*it)->tick) / 1000;
		swprintf (buf, L"END\t%s\t%d\x0d\x0a", verdict.c_str(), duration);
		fwrite (buf, 1, wcslen(buf), file);	
	}
	fclose (file);
	if (DO_IMPORT) Import();
}

void Logger::Initialize ()
{
	DWORD size;
	wchar_t buf[128];
	size = 64;
	GetUserName (buf, &size);
	_wcsupr (buf);
	user = buf;

	size = 64;
	GetComputerName (buf, &size);
	_wcsupr (buf);
	host = buf;

	OSVERSIONINFO osvi;
	ZeroMemory(&osvi, sizeof(OSVERSIONINFO));
	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);

	os = L"Undefined";

	if (GetVersionEx (&osvi))
	{
		if (osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 1) os = L"XP";
		if (osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 0) os = L"2000";
		if (osvi.dwMajorVersion == 4 && osvi.dwMinorVersion == 0) os = L"NT4";
		if (osvi.dwMajorVersion == 4 && osvi.dwMinorVersion == 90) os = L"ME";
		if (osvi.dwMajorVersion == 4 && osvi.dwMinorVersion == 10) os = L"ME";
	};
}

bool Logger::Import ()
{
	wstring comm_line0 = wstring (L"regsvr32").append(L" ").append(TDIMPORT_DLL).append (L" /s");
	wstring comm_line1 = wstring (L"cscript ").append (TDIMPORT_SCRIPT).append(L" updater Kiryukhin 1 ").append (TDIMPORT_FILE);
	wchar_t cl[256];

	STARTUPINFO si;
	PROCESS_INFORMATION pi;

	//register
	ZeroMemory( &si, sizeof(si) );
	si.cb = sizeof(si);
	ZeroMemory( &pi, sizeof(pi) );	

	wcscpy (cl, comm_line0.c_str());
	if (!CreateProcess (NULL, cl, 
					NULL,NULL,FALSE,0,NULL,NULL,&si,&pi)) return false;

	int resWait = WaitForSingleObject (pi.hProcess, 10000);
	CloseHandle (pi.hThread);
	CloseHandle (pi.hProcess);
	
	if (WAIT_TIMEOUT == resWait) return false;
	
	//import
	ZeroMemory( &si, sizeof(si) );
	si.cb = sizeof(si);
	ZeroMemory( &pi, sizeof(pi) );	
	
	wcscpy (cl, comm_line1.c_str());
	std::wcout<<cl<<L"\n";
	if (!CreateProcess (NULL, cl, 
					NULL,NULL,FALSE,0,NULL,NULL,&si,&pi)) return false;

	resWait = WaitForSingleObject (pi.hProcess, 10000);
	CloseHandle (pi.hThread);
	CloseHandle (pi.hProcess);
	
	if (WAIT_TIMEOUT == resWait) return false;

	return true;
		
}
