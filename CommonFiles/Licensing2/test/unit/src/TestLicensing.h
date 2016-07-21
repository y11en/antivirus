#ifndef TESTLICENSING_H
#define TESTLICENSING_H

class TestLicensing : public CPPUNIT_NS::TestFixture
{
	CPPUNIT_TEST_SUITE( TestLicensing );
	CPPUNIT_TEST( testInitWarnings );
	CPPUNIT_TEST_SUITE_END();

	void testInitWarnings();
};

#endif // TESTLICENSING_H