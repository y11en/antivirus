#include "precompiled.h"

using namespace KasperskyLicensing;
using namespace KasperskyLicensing::Implementation;

#include <cppunit/extensions/HelperMacros.h>
#include <LicenseContext.h>
#include <LicenseObjectHelper.h>

#include "TestLicenseContext.h"

CPPUNIT_TEST_SUITE_REGISTRATION( TestLicenseContext );


//-----------------------------------------------------------------------------

void TestLicenseContext::setUp()
{
}

//-----------------------------------------------------------------------------

void TestLicenseContext::tearDown()
{
}

//-----------------------------------------------------------------------------

void TestLicenseContext::testCreation()
{
	LicenseContext c;
	CPPUNIT_ASSERT(c.IsEmpty());
	ILicenseContext::ItemEnumerator e = c.GetItems();
	CPPUNIT_ASSERT(e.IsDone());
}

//-----------------------------------------------------------------------------

void TestLicenseContext::testAddition()
{
	// sample context information
	const LicenseObjectId infoList[] = {
		{ 12345, LicenseObjectId::OBJ_TYPE_PRODUCT, 5, 1 },
		{ 7890,	 LicenseObjectId::OBJ_TYPE_APPLICATION, 5, 13 },
		{ 22468, LicenseObjectId::OBJ_TYPE_COMPONENT, 6, 125 }
	};

	size_t infoSize = sizeof(infoList) / sizeof(LicenseObjectId);

	LicenseContext c;
	
	for (size_t i = 0; i < infoSize; i++)
		c.AddItem(infoList[i]);

	// collection is not empty
	CPPUNIT_ASSERT(!c.IsEmpty());
	
	size_t idx = 0;

	ILicenseContext::ItemEnumerator e = c.GetItems();
	CPPUNIT_ASSERT(!e.IsDone());

	// test information identity
	while (!e.IsDone())
	{
		CPPUNIT_ASSERT(infoList[idx] == e.Item());
		idx++;
		e.Next();
	}

	// iterator is done
	CPPUNIT_ASSERT(e.IsDone());
	// counter is equal to items number
	CPPUNIT_ASSERT(idx == infoSize);
}

//-----------------------------------------------------------------------------

void TestLicenseContext::testIdentity()
{
	// sample context information
	const LicenseObjectId infoList[] = {
		{ 231,  LicenseObjectId::OBJ_TYPE_COMPONENT, 7, 11 },
		{ 476,  LicenseObjectId::OBJ_TYPE_PRODUCT, 8, 3 },
		{ 8891, LicenseObjectId::OBJ_TYPE_APPLICATION, 3, 25 }
	};

	size_t infoSize = sizeof(infoList) / sizeof(LicenseObjectId);

	LicenseContext c1;

	for (size_t i = 0; i < infoSize; i++)
		c1.AddItem(infoList[i]);
	
	LicenseContext c2(c1);
	
	// collection is not empty
	CPPUNIT_ASSERT(!c2.IsEmpty());
	
	size_t idx = 0;

	ILicenseContext::ItemEnumerator e = c2.GetItems();
	CPPUNIT_ASSERT(!e.IsDone());

	// test information identity
	while (!e.IsDone())
	{
		CPPUNIT_ASSERT(infoList[idx] == e.Item());
		idx++;
		e.Next();
	}

	// iterator is done
	CPPUNIT_ASSERT(e.IsDone());
	// counter is equal to items number
	CPPUNIT_ASSERT(idx == infoSize);
}

//-----------------------------------------------------------------------------

void TestLicenseContext::testCleaning()
{
	// sample context information
	const LicenseObjectId infoList[] = {
		{ 1, LicenseObjectId::OBJ_TYPE_COMPONENT, 1, 1 },
		{ 2, LicenseObjectId::OBJ_TYPE_APPLICATION, 2, 2 }
	};

	size_t infoSize = sizeof(infoList) / sizeof(LicenseObjectId);

	LicenseContext c;

	for (size_t i = 0; i < infoSize; i++)
		c.AddItem(infoList[i]);

	CPPUNIT_ASSERT(!c.IsEmpty());
	c.Clear();
	CPPUNIT_ASSERT(c.IsEmpty());

}

//-----------------------------------------------------------------------------
