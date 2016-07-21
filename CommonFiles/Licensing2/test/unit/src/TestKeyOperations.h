#ifndef TESTKEYOPERATIONS_H
#define TESTKEYOPERATIONS_H

class TestKeyOperations : public CPPUNIT_NS::TestFixture
{
	CPPUNIT_TEST_SUITE( TestKeyOperations );
	CPPUNIT_TEST( testParsing );
	CPPUNIT_TEST( testInstallInvalidSignature );
	CPPUNIT_TEST( testInstallWithSameSerial );
	CPPUNIT_TEST( testWrongAppVersionType );
	CPPUNIT_TEST( testUninstall );
	CPPUNIT_TEST( testGetByContext );
	CPPUNIT_TEST_SUITE_END();
public:
	void setUp();
	void tearDown();

private:
	void testParsing();
	void testInstallWithSameSerial();
	void testWrongAppVersionType();
	void testUninstall();
	void testInstallInvalidSignature();
	void testGetByContext();

private:
	WinFileStorageRW* wfs;
	AutoPointer<ILicensing> licensing;
	AutoPointer<ILicenseUtility> utility;
};

#endif // TESTKEYOPERATIONS_H