#include "precompiled.h"

using namespace KasperskyLicensing;
using namespace KasperskyLicensing::Implementation;

#include <cppunit/extensions/HelperMacros.h>
#include <utility/WinFileStorageR.h>
#include <utility/pointainer.h>
#include <Serialize/KLDTSer.h>
#include <LicenseKey.h>
#include <LicenseObjectWrapper.h>
#include <ControlDatabase.h>

#include "TestLobjectWrapper.h"

CPPUNIT_TEST_SUITE_REGISTRATION( TestLobjectWrapper );

void TestLobjectWrapper::setUp()
{
	ControlDatabase cdb("");
	WinFileStorageR wfs("..\\..\\..\\data\\licenses", "*.lic");

	key1 = AutoPointer<LicenseKey>(new LicenseKey(*wfs.LoadObject("license6.lic"), cdb));
	key2 = AutoPointer<LicenseKey>(new LicenseKey(*wfs.LoadObject("license7.lic"), cdb));

}

void TestLobjectWrapper::tearDown()
{
}

void TestLobjectWrapper::testMergePlatforms()
{
	PlatformType platforms[] = {PT_WIN_WORKSTATION, PT_WIN_PERSONAL};

	LicenseObjectWrapper low(key1->GetLicenseRoot(), PT_WIN_WORKSTATION);
	low.Merge(key2->GetLicenseRoot());

	ILicenseObject::PlatformEnumerator pe = low.GetSupportedPlatforms();
	CPPUNIT_ASSERT(!pe.IsDone());
	unsigned i = 0;
	for (; !pe.IsDone() && i < 2; pe.Next(), i++)
	{
		CPPUNIT_ASSERT(pe.Item() == platforms[i]);
	}
	CPPUNIT_ASSERT(i == 2 && pe.IsDone());

	ILicenseObject::LicenseObjectEnumerator le = low.GetLicenseObjects();
	CPPUNIT_ASSERT(!le.IsDone());
	for (; !le.IsDone(); le.Next())
	{
		// license object with platform PT_WIN_PERSONAL should be absent
		CPPUNIT_ASSERT(le.Item().GetObjectId().id != 1200);
	}

}