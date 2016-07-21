#include "precompiled.h"

#include <LicenseContext.h>
#include <utility/WinFileStorageR.h>
#include <cppunit/extensions/HelperMacros.h>

using namespace KasperskyLicensing;
using namespace KasperskyLicensing::Implementation;

#include "TestKeyOperations.h"

CPPUNIT_TEST_SUITE_REGISTRATION( TestKeyOperations );

void TestKeyOperations::setUp()
{
	ProgramClientInfo pci = 
	{
		{ { 666, LicenseObjectId::OBJ_TYPE_PRODUCT, 6, 0 }, 1, 1, 1, 1 },
		ProgramClientInfo::VER_TYPE_RELEASE,
		PT_WIN_PERSONAL
	};
	LicenseContext context;
	wfs = new WinFileStorageRW("..\\..\\..\\data\\licenses", "*.key");
	licensing = MakeLicensing(*wfs, pci, "..\\..\\..\\data");
	utility = licensing->MakeLicenseUtility(context);
}

void TestKeyOperations::tearDown()
{
	IStorageR::NameEnumerator e = wfs->GetObjectNames();

	// cleanup installed keys
	for (; !e.IsDone(); e.Next())
		wfs->RemoveObject(e.Item());

	delete wfs;
}

void TestKeyOperations::testParsing()
{
	AutoPointer<IObjectImage> image = 
		wfs->LoadObject("license1.lic");
	AutoPointer<ILicenseKey> key =  
		licensing->ParseLicenseKey(image->GetObjectImage(), image->GetObjectSize());
}

void TestKeyOperations::testInstallInvalidSignature()
{
	AutoPointer<IObjectImage> image = wfs->LoadObject("license1.xml");
	try
	{
		utility->InstallLicenseKey("license1.xml", image->GetObjectImage(), image->GetObjectSize());
	}
	catch(ILicensingError& e)
	{
		CPPUNIT_ASSERT(e.GetErrorCode() == ILicensingError::LICERR_INVALID_SIGNATURE);
		return;
	}
	
	CPPUNIT_FAIL("Key with ivalid signature can not be installed");
}

void TestKeyOperations::testInstallWithSameSerial()
{
	{
		AutoPointer<IObjectImage> image = wfs->LoadObject("license2.lic");
		utility->InstallLicenseKey("license2.key", image->GetObjectImage(), image->GetObjectSize());
	}

	try
	{
		AutoPointer<IObjectImage> image = wfs->LoadObject("license3.lic");
		utility->InstallLicenseKey("license3.key", image->GetObjectImage(), image->GetObjectSize());
	}
	catch(ILicensingError& e)
	{
		CPPUNIT_ASSERT(e.GetErrorCode() == ILicensingError::LICERR_DUPLICATE_KEY);
		return;
	}

	CPPUNIT_FAIL("Keys with identical SN can not be installed");
}

void TestKeyOperations::testWrongAppVersionType()
{
	// test release key for beta product
	{
		ProgramClientInfo pci = 
		{
			{ { 666, LicenseObjectId::OBJ_TYPE_PRODUCT, 6, 0 }, 1, 1, 1, 1 },
			ProgramClientInfo::VER_TYPE_BETA,
			PT_WIN_PERSONAL
		};

		LicenseContext context;
		AutoPointer<ILicensing> licensing = MakeLicensing(*wfs, pci, "..\\..\\..\\data");
		AutoPointer<IObjectImage> image = wfs->LoadObject("license5.lic");
		AutoPointer<ILicenseKey> key = licensing->ParseLicenseKey(image->GetObjectImage(), image->GetObjectSize());
		AutoPointer<ILicenseUtility> utility = licensing->MakeLicenseUtility(context);
		CPPUNIT_ASSERT(utility->GetLicenseKeyStatus(*key) == ILicenseUtility::KEY_INCOMPATIBLE_VERSION_TYPE);
	}

	// test beta key for release product
	try
	{
		AutoPointer<IObjectImage> image = wfs->LoadObject("license4.lic");
		AutoPointer<ILicenseKey> key = licensing->ParseLicenseKey(image->GetObjectImage(), image->GetObjectSize());
		CPPUNIT_ASSERT(utility->GetLicenseKeyStatus(*key) == ILicenseUtility::KEY_INCOMPATIBLE_VERSION_TYPE);
		utility->InstallLicenseKey("license4.key", image->GetObjectImage(), image->GetObjectSize());
	}
	catch(ILicensingError& e)
	{
		CPPUNIT_ASSERT(e.GetErrorCode() == ILicensingError::LICERR_KEY_MISMATCH);
		return;
	}

	CPPUNIT_FAIL("Keys with incompatible version type can not be installed");
}

void TestKeyOperations::testUninstall()
{
	LicenseKeyInfo::SerialNumber sn = { 0 };
	{
		AutoPointer<IObjectImage> image = wfs->LoadObject("license5.lic");
		const ILicenseKey& key =
			utility->InstallLicenseKey("license5.key", image->GetObjectImage(), image->GetObjectSize());
		sn = key.GetKeyInfo().serial_number;
	}

	utility->UninstallLicenseKey(sn);
}

void TestKeyOperations::testGetByContext()
{
	{
		AutoPointer<IObjectImage> image = wfs->LoadObject("license8.lic");
		utility->InstallLicenseKey("license8.key", image->GetObjectImage(), image->GetObjectSize());
	}

	{
		AutoPointer<IObjectImage> image = wfs->LoadObject("license9.lic");
		utility->InstallLicenseKey("license9.key", image->GetObjectImage(), image->GetObjectSize());
	}

	// context from second license key (license9)
	LicenseObjectId id1 = {1300, LicenseObjectId::OBJ_TYPE_APPLICATION, 6, 0 };
	LicenseContext context;
	context.AddItem(id1);
	utility = licensing->MakeLicenseUtility(context);
	ILicenseUtility::KeyEnumerator ke = utility->GetInstalledKeys();
	CPPUNIT_ASSERT_MESSAGE("License license9.key is not included", !ke.IsDone());
	
	ILicenseObject::LicenseObjectEnumerator le = ke.Item().GetLicenseObject().GetLicenseObjects();
	CPPUNIT_ASSERT(!le.IsDone());
	for (; le.IsDone(); le.Next())
	{
		if (le.Item().GetObjectId().id == id1.id)
			break;
	}
	// license object is found
	CPPUNIT_ASSERT_MESSAGE("License object with id 1330 is not found", !le.IsDone());

	// wrong context
	LicenseObjectId id2 = {999, LicenseObjectId::OBJ_TYPE_APPLICATION, 6, 0 };
	context.AddItem(id2);
	utility = licensing->MakeLicenseUtility(context);

	ke = utility->GetInstalledKeys();
	CPPUNIT_ASSERT_MESSAGE("Installed key list must be empty", ke.IsDone());

	// root context
	context.Clear();
	utility = licensing->MakeLicenseUtility(context);
	ke = utility->GetInstalledKeys();
	CPPUNIT_ASSERT_MESSAGE("Installed key list must not be empty", !ke.IsDone());
	// first key (license8)
	CPPUNIT_ASSERT(ke.Item().GetKeyInfo().serial_number.sequence_number == 21);
	ke.Next();
	CPPUNIT_ASSERT_MESSAGE("Installed key list must not be empty", !ke.IsDone());
	// first key (license9)
	CPPUNIT_ASSERT(ke.Item().GetKeyInfo().serial_number.sequence_number == 22);
	ke.Next();
	CPPUNIT_ASSERT_MESSAGE("More than two keys (license8, license9) returned", ke.IsDone());
}