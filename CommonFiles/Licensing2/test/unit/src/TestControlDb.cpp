#include "precompiled.h"

using namespace KasperskyLicensing;
using namespace KasperskyLicensing::Implementation;

#include <cppunit/extensions/HelperMacros.h>
#include <utility/WinFileStorageR.h>
#include <ControlDatabase.h>

#include "TestControlDb.h"

CPPUNIT_TEST_SUITE_REGISTRATION( TestControlDb );

void TestControlDb::testBlistEntries()
{
	LicenseKeyInfo::SerialNumber arrSn[] = 
	{ {0, 0, 68710}, {0, 0, 292454}, {0, 0, 297879} };

	const size_t snSize = sizeof(arrSn) / sizeof(arrSn[0]);

	LicenseKeyInfo::SerialNumber  fakeSn = {0, 0, 12345678};

	WinFileStorageR wfs("..\\..\\..\\data\\bases1", "*");
	ControlDatabase cdb("..\\..\\..\\data");
	cdb.LoadDatabase(wfs, CTRL_BLIST_ONLY);

	for (size_t i = 0; i < snSize; i++)
		CPPUNIT_ASSERT(cdb.IsKeyBanned(arrSn[i]));

	CPPUNIT_ASSERT(!cdb.IsKeyBanned(fakeSn));
}
void TestControlDb::testLoadStatusOk()
{
	WinFileStorageR wfs("..\\..\\..\\data\\bases1", "*");
	ControlDatabase cdb("..\\..\\..\\data");
	cdb.LoadDatabase(wfs, CTRL_BLIST_AND_BASES);

	CPPUNIT_ASSERT(cdb.GetIndexStatus()		== ControlDatabase::INDEX_OK);
	CPPUNIT_ASSERT(cdb.GetAvdbStatus()		== ControlDatabase::AVDB_OK);
	CPPUNIT_ASSERT(cdb.GetBlackListStatus()	== ControlDatabase::BLST_OK);
}
void TestControlDb::testLoadBadIndex()
{
	WinFileStorageR wfs("..\\..\\..\\data\\bases2", "*");
	ControlDatabase cdb("..\\..\\..\\data");
	cdb.LoadDatabase(wfs, CTRL_BLIST_AND_BASES);

	CPPUNIT_ASSERT(cdb.GetIndexStatus()		== ControlDatabase::INDEX_PARSING_ERROR);
	CPPUNIT_ASSERT(cdb.GetAvdbStatus()		== ControlDatabase::AVDB_BAD_INDEX);
	CPPUNIT_ASSERT(cdb.GetBlackListStatus()	== ControlDatabase::BLST_BAD_INDEX);
}
void TestControlDb::testLoadBadBlistBases()
{
	WinFileStorageR wfs("..\\..\\..\\data\\bases3", "*");
	ControlDatabase cdb("..\\..\\..\\data");
	cdb.LoadDatabase(wfs, CTRL_BLIST_AND_BASES);

	CPPUNIT_ASSERT(cdb.GetIndexStatus()		== ControlDatabase::INDEX_OK);
	CPPUNIT_ASSERT(cdb.GetAvdbStatus()		== ControlDatabase::AVDB_INCONSISTENT);
	CPPUNIT_ASSERT(cdb.GetBlackListStatus()	== ControlDatabase::BLST_PARSING_ERROR);
}
void TestControlDb::testLoadBadBlist()
{
	WinFileStorageR wfs("..\\..\\..\\data\\bases3", "*");
	ControlDatabase cdb("..\\..\\..\\data");
	cdb.LoadDatabase(wfs, CTRL_BLIST_ONLY);

	CPPUNIT_ASSERT(cdb.GetIndexStatus()		== ControlDatabase::INDEX_OK);
	CPPUNIT_ASSERT(cdb.GetAvdbStatus()		== ControlDatabase::AVDB_OK);
	CPPUNIT_ASSERT(cdb.GetBlackListStatus()	== ControlDatabase::BLST_PARSING_ERROR);
}
void TestControlDb::testLoadNoRegistry()
{
	WinFileStorageR wfs("..\\..\\..\\data\\bases4", "*");
	ControlDatabase cdb("..\\..\\..\\data\\bases4");
	cdb.LoadDatabase(wfs, CTRL_BLIST_AND_BASES);

	CPPUNIT_ASSERT(cdb.GetIndexStatus()		== ControlDatabase::INDEX_INCORRECT_SIGN);
	CPPUNIT_ASSERT(cdb.GetAvdbStatus()		== ControlDatabase::AVDB_BAD_INDEX);
	CPPUNIT_ASSERT(cdb.GetBlackListStatus()	== ControlDatabase::BLST_BAD_INDEX);
}