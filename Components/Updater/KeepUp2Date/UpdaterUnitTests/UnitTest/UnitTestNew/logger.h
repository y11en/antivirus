#if !defined(TEST_LOGGER_H)
#define TEST_LOGGER_H

#include "stdafx.h"

#define TDTOOL_ROOT L"H:\\Updater-dev\\Components\\Updater\\KeepUp2Date\\UpdaterUnitTests\\TDTool\\"
#define TDIMPORT_FILE L"H:\\Updater-dev\\Components\\Updater\\KeepUp2Date\\UpdaterUnitTests\\TDTool\\import.log"
#define TDIMPORT_SCRIPT L"H:\\Updater-dev\\Components\\Updater\\KeepUp2Date\\UpdaterUnitTests\\TDTool\\td_import.vbs"
#define TDIMPORT_DLL L"H:\\Updater-dev\\Components\\Updater\\KeepUp2Date\\UpdaterUnitTests\\TDTool\\otaclient80.dll"

#define DO_IMPORT false

#include <string>
#include <vector>
#include "windows.h"
#include <boost/shared_ptr.hpp>

using namespace std;
using namespace boost;

class Logger
{
	public:
//--------------------------
		class Step
		{
			friend class Logger;
		public:
			Step ();
			Step (const wstring& name/*, const wstring& result, const wstring& comment*/);
			void Clear ();
			void SetName (const wstring& _name);
			void SetTime ();
			const wchar_t* GetName ();
			void AddPoint (wstring comm);
			bool IsEmpty ();
			
		private:
			wstring name;
			vector<wstring> comments;
			SYSTEMTIME time;
			DWORD tick;
			
		};
		typedef shared_ptr<Step> StepP;
//--------------------------    		
		class Test
		{
			friend class Logger;
		public:
			Test (const wstring& name);
			void AddStep (const Step& step);
			const wchar_t* GetName ();

		private:
			wstring name;
			vector<Step> steps;
			SYSTEMTIME time;
			DWORD tick;
		};
		typedef shared_ptr<Test> TestP;
//--------------------------
		class Case
		{
		public:
			Case (Logger& logger, TestP& test, Step& step, const wstring& name);
			~Case ();

		private:
			Step& m_step;
			TestP& m_test;
			Logger& m_logger;
			
		};
//--------------------------	
	static Logger& GetInstance();

	void AddTest (const TestP& tp);
	void AddStep (const TestP& tp, const Step& sp);
	~Logger ();
	void Initialize ();
	bool Import ();


	private:
		vector<TestP> tests;
		wstring user, host, os;
			
};


#endif