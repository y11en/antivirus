#include "precompiled.h"
#include <windows.h>

using namespace KasperskyLicensing;
using namespace KasperskyLicensing::Implementation;

#include <cppunit/extensions/HelperMacros.h>
#include <utility/WinFileStorageR.h>

#include "TestStorage.h"

CPPUNIT_TEST_SUITE_REGISTRATION( TestStorage );

void TestStorage::testReadError()
{
	HANDLE hDir = CreateFile(
		"..\\..\\..\\data\\licenses\\license1.lic",
		GENERIC_READ, 
		0, 
		NULL, 
		OPEN_EXISTING, 
		FILE_ATTRIBUTE_NORMAL, 
		0
	);

	WinFileStorageR wfs("..\\..\\..\\data\\licenses\\", "*.lic");

	try
	{
		wfs.LoadObject("license1.lic");
	}
	catch(ILicensingError& e)
	{
		CloseHandle(hDir);
		CPPUNIT_ASSERT(e.GetErrorCode() == ILicensingError::LICERR_STORAGE_READ);
		return;
	}
	CloseHandle(hDir);

	CPPUNIT_FAIL("LICERR_STORAGE_READ is not reported");
	
}