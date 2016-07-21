#include "precompiled.h"

#include <ControlDatabase.h>
#include <utility/WinFileStorageR.h>
#include <cppunit/extensions/HelperMacros.h>

using namespace KasperskyLicensing;
using namespace KasperskyLicensing::Implementation;

#include "TestIndex.h"

CPPUNIT_TEST_SUITE_REGISTRATION( TestIndex );

void TestIndex::setUp()
{
	WinFileStorageR wfs("..\\..\\..\\data", "*");
	image1 = wfs.LoadObject("kavset1.xml");
	image2 = wfs.LoadObject("kavset2.xml");
}

void TestIndex::tearDown()
{
}

void TestIndex::testParsing()
{
	ControlDatabase::Index index(*image1);
}

void TestIndex::testParsingThrow()
{
	ControlDatabase::Index index(*image2);
}


void TestIndex::testGetters()
{
	ControlDatabase::Index index(*image1);

	tm mytm = { 0 };
	mytm.tm_mday = 15; mytm.tm_mon = 7-1; mytm.tm_year = 2005-1900; mytm.tm_isdst = -1;
	time_t nExpectedDate = mktime(&mytm);

	CPPUNIT_ASSERT(!strcmp(index.GetBlistName(), "blackjack.lst"));
	CPPUNIT_ASSERT(index.GetUpdateDate() == nExpectedDate);

	std::vector<std::basic_string<char_t> > aExpectedNames;
	aExpectedNames.push_back("avcmhk4.dll");
	aExpectedNames.push_back("avp.klb");

	CPPUNIT_ASSERT(index.GetAvdbNames() == aExpectedNames);
}
