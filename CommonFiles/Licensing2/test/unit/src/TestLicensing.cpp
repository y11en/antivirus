#include "precompiled.h"

using namespace KasperskyLicensing;
using namespace KasperskyLicensing::Implementation;

#include <cppunit/extensions/HelperMacros.h>
#include <utility/WinFileStorageR.h>
#include <utility/WinFileStorageRW.h>
#include <utility/pointainer.h>
#include <Serialize/KLDTSer.h>
#include <LicenseKeyspace.h>
#include <Licensing.h>

#include "TestLicensing.h"

CPPUNIT_TEST_SUITE_REGISTRATION( TestLicensing );

void TestLicensing::testInitWarnings()
{
	ProgramClientInfo pci = 
	{
		{ { 666, LicenseObjectId::OBJ_TYPE_PRODUCT, 6, 0 }, 1, 1, 1, 1 },
		ProgramClientInfo::VER_TYPE_RELEASE,
		PT_WIN_PERSONAL
	};

	WinFileStorageR	bases("..\\..\\..\\data\\bases3", "*");
	WinFileStorageRW keys("..\\..\\..\\data\\licenses", "*.lic");

	Licensing licensing(keys, pci, "..\\..\\..\\data");
	licensing.SetControlMode(bases, CTRL_BLIST_ONLY);
	ILicensing::WarningEnumerator e = licensing.GetInitWarnings();
	for (; !e.IsDone(); e.Next())
	{
		if (e.Item() == ILicensing::WARN_INCORRECT_BLACKLIST)
			break;
	}

	CPPUNIT_ASSERT_MESSAGE("WARN_INCORRECT_BLACKLIST is not reported", !e.IsDone());
}