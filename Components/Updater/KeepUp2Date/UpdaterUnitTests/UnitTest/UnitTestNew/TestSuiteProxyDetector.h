#ifndef TESTSUITE_PROXY_DETECTOR_H
#define TESTSUITE_PROXY_DETECTOR_H

#include "stdafx.h"

#include "boost.h"
#include "TestSuiteBase.h"
#include "TestSuiteTransaction.h"
#include "journal.h"

using namespace ProxyDetectorNamespace;

class TestSuiteProxyDetector: public TestSuiteBase
{
public:

	TestSuiteProxyDetector(): TestSuiteBase (L"ProxyDetector"), s6c (&testLog), callback (&testLog) {};

	//tests
	void TestWPAD ();
	void TestWPAD_IpV6 ();
	void TestWPADCorrupted ();

	class ProxyCallback: public CallbackInterface
	{
	public:
		ProxyCallback (Log* pLog): CallbackInterface (/*pLog*/) {};
		virtual UpdaterConfiguration &updaterConfiguration()
		{
			static UpdaterConfiguration upd_config (1, 1, L"");
			return upd_config;
		}

		virtual bool checkFilesToDownload(FileVector &, const NoCaseString &updateDate, const bool retranslationMode)
		{
			return true;
		}

		virtual FileInfo getPrimaryIndex(const bool retranslationMode)
		{
			return FileInfo ();
		}

		virtual FileInfo getUpdaterConfigurationXml()
		{
			return FileInfo ();
		}

		virtual FileInfo getSitesFileXml()
		{
			return FileInfo ();
		}

		//now abstract in CallbackInterface

		virtual bool removeRollbackSection(const bool retranslationMode)
		{
			return true;
		}
		virtual bool readRollbackFiles(FileVector &, const bool retranslationMode)
		{
			return false;
		}

		virtual bool processReceivedFiles(const FileVector &, const bool createRollbackNeeded, const Path &rollbackFolder, const bool retranslationMode, const NoCaseString&)
		{
			return true;
		}

		virtual bool expandEnvironmentString(const NoCaseString &input, NoCaseString &output, const StringParser::ExpandOrder &)
		{
			return false;
		}

	};

	class ProxyJournal: public TestJournalInterface
	{
		
	public:

		ProxyJournal (wstring fake_wpad): m_updater (NULL), m_fakewpad (fake_wpad), m_detector (NULL) {};
		
		void SetUpdater (Updater* upd)
		{
			m_updater = upd;
		}

		void SetProxyDetector (ProxyDetector* proxy_det)
		{
			m_detector = proxy_det;
		}

		virtual void publishMessage(const KLUPD::CoreError & event, const NoCaseString &parameter = NoCaseString(), const NoCaseString &parameter2 = NoCaseString())
		{
			wstring file = parameter.toWideChar();
			transform(file.begin(), file.end(), file.begin(), tolower);


			if (event == EVENT_FILE_DOWNLOADED && 
				file.find(L"wpad.dat") != wstring::npos && !m_fakewpad.empty()) 
				{
					_helper::CloneFile(m_fakewpad, WPADName(), false);
				}
		};

		const std::wstring WPADName()
		{
			if (m_updater) return m_updater->m_proxyDetector.m_winInetReader.m_wpad_LocalFullFileName.toWideChar();
			if (m_detector) return m_detector->m_winInetReader.m_wpad_LocalFullFileName.toWideChar();
			_ASSERTE ("Cannot determine WPAD name\n");
		}


	private:

		//HANDLE hWPADDownloaded;
		wstring m_fakewpad;
		Updater* m_updater;
		ProxyDetector* m_detector;

	};

	TestLog testLog;
	ProxyCallback callback;
	Signature6Checker s6c;

	FileVector m_files;

};

#endif