#ifndef TESTLOBJECTWRAPPER_H
#define TESTLOBJECTWRAPPER_H

class TestLobjectWrapper : public CPPUNIT_NS::TestFixture
{
	CPPUNIT_TEST_SUITE( TestLobjectWrapper );
	CPPUNIT_TEST( testMergePlatforms );
	CPPUNIT_TEST_SUITE_END();

public:
	void setUp();
	void tearDown();

private:
	void testMergePlatforms();

	AutoPointer<LicenseKey> key1, key2;
};

#endif // TESTLOBJECTWRAPPER_H