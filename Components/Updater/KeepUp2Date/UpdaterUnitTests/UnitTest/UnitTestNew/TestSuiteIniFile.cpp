#include "util.h"

#include "log.h"
#include "callback.h"
#include "journal.h"
#include "core/updater.h"
#include "helper/local_file.h"
#include "IniFile/IniFile.h"

#include "TestSuiteIniFile.h"
#include "main.h"

using namespace KLUPD;
using namespace std;

bool operator==(const IniFile::Key& rev1, const IniFile::Key& rev2)
{
	return
		(
		//rev1.m_comment == rev2.m_comment &&
		rev1.m_key == rev2.m_key &&
		rev1.m_value == rev2.m_value
		);
}

#define INIFILE_NAME L"d:\\inifile.ini"

#define PREPARE_FILE(filename)\
	_helper::DeleteDirectory(ROOT);\
	KLUPD::createFolder (ROOT, &testLog);\
	_helper::CloneFile (wstring(COLL_ROOT).append(L"\\Inifiles\\").append(filename).c_str(),\
						wstring (ROOT).append (L"\\inifile.ini"), false);\
	IniFile ifile (wstring (ROOT).append (L"\\inifile.ini").c_str(), &testLog);


typedef vector<IniFile::Key>::iterator keys_iter;

void TestSuiteIniFile::TestCommon ()
{
	std::wcout<<L"TestCommon\n";
	Logger::Case _case (m_logger, m_testp, m_step, L"TestCommon");

	IniFile::Key key1 (L"key_name1", L"key_value1", L"key_comment1");
	IniFile::Key key2 (L"key_name2", L"key_value2", L"");
	IniFile::Key key3 (L"key_name3", L"", L"key_comment3");
	IniFile::Key key4 (L"", L"key_value4", L"key_comment4");

	IniFile::Key key1a (L"key_name1", L"key_value1a", L"key_comment1a");
	IniFile::Key key2a (L"key_name2", L"key_value2a", L"");
	IniFile::Key key3a (L"key_name3", L"", L"key_comment3a");

	vector<IniFile::Key> keys1, keys1a;
	keys1.push_back(key1);
	keys1.push_back(key2);
	keys1.push_back(key3);
	keys1.push_back(key4);

	keys1a.push_back(key1a);
	keys1a.push_back(key2a);
	keys1a.push_back(key3a);

	_helper::DeleteFile (INIFILE_NAME);

	//write
	{
		IniFile ifile (INIFILE_NAME, &testLog);
		ifile.createSection(L"Section1", L"Comment1", keys1);
		QC_BOOST_CHECK_MESSAGE (ifile.keyCount() == 3, L"Error");
	}
	//read
	{
		IniFile ifile (INIFILE_NAME, &testLog);
		IniFile::Section* sec = ifile.getSection(L"Section1");
		
		//getkey
		for (keys_iter it = keys1.begin(); it != keys1.end()-1; ++it)
			QC_BOOST_CHECK_MESSAGE (*ifile.getKey (it->m_key, L"Section1") == *it, L"Error");
		QC_BOOST_CHECK_MESSAGE (ifile.getKey (key4.m_key, L"Section1") == NULL, L"Error4\n") ;

		//getvalue
		for (keys_iter it = keys1.begin(); it != keys1.end()-1; ++it)
			QC_BOOST_CHECK_MESSAGE (ifile.getValue(it->m_key, L"Section1") == it->m_value, L"Error");

		//setvalue
		for (keys_iter it = keys1.begin(), it2 = keys1a.begin(); it != keys1.end()-1; ++it, ++it2)
			QC_BOOST_CHECK_MESSAGE (ifile.setValue(it->m_key, it2->m_value, L"Section1", it2->m_comment), L"Error");

		//getvalue
		for (keys_iter it = keys1a.begin(); it != keys1a.end()-1; ++it)
			QC_BOOST_CHECK_MESSAGE (ifile.getValue(it->m_key, L"Section1") == it->m_value, L"Error");
	}

	//read
	{
		IniFile ifile (INIFILE_NAME, &testLog);
		IniFile::Section* sec = ifile.getSection(L"Section1");

		//getvalue
		for (keys_iter it = keys1a.begin(); it != keys1a.end()-1; ++it)
			QC_BOOST_CHECK_MESSAGE (ifile.getValue(it->m_key, L"Section1") == it->m_value, L"Error");

		//getstring
		QC_BOOST_CHECK_MESSAGE (ifile.getValue(key1a.m_key, L"Section1") == key1a.m_value, L"Error");

		//setint/getint
		QC_BOOST_CHECK_MESSAGE (ifile.setInt(key1a.m_key, 4, L"Section1"), L"Error" );
		QC_BOOST_CHECK_MESSAGE (4 == ifile.getInt(key1a.m_key, L"Section1"), L"Error" );

		//setint/getint
		QC_BOOST_CHECK_MESSAGE (ifile.setInt(key1a.m_key, 5, L"Section1"), L"Error" );
		QC_BOOST_CHECK_MESSAGE (5 == ifile.getInt(key1a.m_key, L"Section1"), L"Error" );

		//setbool/getbool
		QC_BOOST_CHECK_MESSAGE (ifile.setBool(key1a.m_key, true, L"Comment1", L"Section1"), L"Error" );
		QC_BOOST_CHECK_MESSAGE (true == ifile.getBool(false, key1a.m_key, L"Section1"), L"Error" );

		//setbool/getbool
		QC_BOOST_CHECK_MESSAGE (ifile.setBool(key1a.m_key, false, L"Comment1", L"Section1"), L"Error" );
		QC_BOOST_CHECK_MESSAGE (false == ifile.getBool(true, key1a.m_key, L"Section1"), L"Error" );

		//setkeycomment
		QC_BOOST_CHECK_MESSAGE (ifile.setKeyComment(key1a.m_key, L"Comment2", L"Section1"), L"Error" );
		QC_BOOST_CHECK_MESSAGE (ifile.getKey(key1a.m_key, L"Section1")->m_comment == L"Comment2", L"Error");

		//setsectioncomment
		QC_BOOST_CHECK_MESSAGE (ifile.setSectionComment(L"Section1", L"Comment2section"), L"Error" );
		QC_BOOST_CHECK_MESSAGE (ifile.getSection(L"Section1")->m_comment == L"Comment2section", L"Error" );

		//createsection/deletesection
		ifile.createSection(L"Section4", L"Comment4", keys1);
		ifile.createSection(L"Section5", L"Comment5");
		QC_BOOST_CHECK_MESSAGE (ifile.getSection (L"Section4"), L"Error");
		QC_BOOST_CHECK_MESSAGE (ifile.getSection (L"Section5"), L"Error");
		QC_BOOST_CHECK_MESSAGE (ifile.getKey (L"key_name1", L"Section4"), L"Error");
		QC_BOOST_CHECK_MESSAGE (ifile.getKey (L"key_name2", L"Section4"), L"Error");
		//counters
	QC_BOOST_CHECK_MESSAGE (ifile.sectionCount() == 4, L"Error");
	QC_BOOST_CHECK_MESSAGE (ifile.keyCount() == 6, L"Error");
		
		//delete key
		QC_BOOST_CHECK_MESSAGE (ifile.deleteKey (L"key_name2", L"Section4"), L"Error");
		QC_BOOST_CHECK_MESSAGE (!ifile.getKey (L"key_name2", L"Section4"), L"Error");
		ifile.deleteSection(L"Section4");
		ifile.deleteSection(L"Section5");
		QC_BOOST_CHECK_MESSAGE (!ifile.getSection (L"Section4"), L"Error");
		QC_BOOST_CHECK_MESSAGE (!ifile.getSection (L"Section5"), L"Error");
		QC_BOOST_CHECK_MESSAGE (!ifile.getKey (L"key_name1", L"Section4"), L"Error");

	}


}

void TestSuiteIniFile::TestIncorrectFormat ()
{
	std::wcout<<L"TestIncorrectFormat\n";
	Logger::Case _case (m_logger, m_testp, m_step, L"TestIncorrectFormat");

	//ini1
	{
		PREPARE_FILE(L"ini1.ini")

		QC_BOOST_CHECK_MESSAGE ( !ifile.getSection(L"Section1") , L"Error");
		QC_BOOST_CHECK_MESSAGE ( ifile.getSection(L"Section2") , L"Error");
		QC_BOOST_CHECK_MESSAGE ( ifile.getSection(L"Section3") , L"Error");
	}

	//ini2
	{
		PREPARE_FILE(L"ini2.ini")

		QC_BOOST_CHECK_MESSAGE ( ifile.getSection(L"Section1") , L"Error");
		QC_BOOST_CHECK_MESSAGE ( !ifile.getSection(L"Section2") , L"Error");
		QC_BOOST_CHECK_MESSAGE ( ifile.getSection(L"Section3") , L"Error");
	}

	//ini3
	{
		PREPARE_FILE(L"ini3.ini")

		QC_BOOST_CHECK_MESSAGE ( !ifile.getSection(L"Section1") , L"Error");
		QC_BOOST_CHECK_MESSAGE ( !ifile.getSection(L"Section2") , L"Error");
		QC_BOOST_CHECK_MESSAGE ( !ifile.getSection(L"Section3") , L"Error");
		QC_BOOST_CHECK_MESSAGE ( !ifile.getSection(L"Section4") , L"Error");
		QC_BOOST_CHECK_MESSAGE ( ifile.getSection(L"Section5") , L"Error");
	}

	//ini4
	{
		PREPARE_FILE(L"ini4.ini")

		QC_BOOST_CHECK_MESSAGE ( ifile.getSection(L"Section1") , L"Error");
		QC_BOOST_CHECK_MESSAGE ( ifile.getSection(L"Section2") , L"Error");
		QC_BOOST_CHECK_MESSAGE ( ifile.getSection(L"Section3") , L"Error");
		QC_BOOST_CHECK_MESSAGE ( ifile.getSection(L"Section4") , L"Error");
		QC_BOOST_CHECK_MESSAGE ( ifile.getSection(L"Section5") , L"Error");
	}

	//ini5
	{
		PREPARE_FILE(L"ini5.ini")

		QC_BOOST_CHECK_MESSAGE ( ifile.getSection(L"Section1") , L"Error");
		QC_BOOST_CHECK_MESSAGE ( !ifile.getSection(L"Section2") , L"Error");
		QC_BOOST_CHECK_MESSAGE ( ifile.getSection(L"Section3") , L"Error");
		QC_BOOST_CHECK_MESSAGE ( ifile.getSection(L"Section4") , L"Error");
		QC_BOOST_CHECK_MESSAGE ( ifile.getSection(L"Section5") , L"Error");
	}

	
}

void TestSuiteIniFile::TestRandFormat ()
{
	std::wcout<<L"TestRandFormat\n";
	Logger::Case _case (m_logger, m_testp, m_step, L"TestRandFormat");

	_helper::DeleteDirectory(ROOT);
	KLUPD::createFolder (ROOT, &testLog);
	char* symbols = "[]~!@#$%^&*()<>,.'/\\{}abcdefghi\x0d\x0a\x0d\x0a";

	string filebuf;
	FILE* file = _wfopen (wstring(COLL_ROOT).append(L"\\Inifiles\\ini.ini").c_str(), L"rb");
	char buffer[1024];
	int count = 1;
	while (count)
	{
		count = fread (buffer, 1, 1024, file);
		if (count != 0) filebuf.append (buffer, buffer + count);
	}
	fclose (file);

	srand( (unsigned)time( NULL ) );
	for (int i = 0; i <= 100; ++i)
	{
		vector<char> buf;
		buf.assign ( filebuf.c_str(), filebuf.c_str() + filebuf.length() );

		for (int j = 0; j <= filebuf.length(); ++j)
		{
			int num = rand()*(strlen(symbols)-1)/RAND_MAX;
			int pos = rand()*(buf.size()-1)/RAND_MAX;
			buf[pos] = * (symbols + num);
			FILE* file2 = _wfopen (wstring (ROOT).append (L"\\inifile.ini").c_str(), L"wb");
			fwrite (&buf[0], 1, buf.size(), file2);
			fclose(file2);
	
			IniFile ifile (wstring (ROOT).append (L"\\inifile.ini").c_str(), &testLog);
			ifile.getSection(L"Section1");
			ifile.getSection(L"Section2");
			ifile.getSection(L"Section3");
			ifile.getSection(L"Section4");
			ifile.getSection(L"Section5");
		}
	}
	


}
