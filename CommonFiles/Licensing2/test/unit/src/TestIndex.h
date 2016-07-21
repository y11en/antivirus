#ifndef TESTINDEX_H
#define TESTINDEX_H

class TestIndex : public CPPUNIT_NS::TestFixture
{
	CPPUNIT_TEST_SUITE( TestIndex );
	CPPUNIT_TEST( testParsing );
	CPPUNIT_TEST_EXCEPTION( testParsingThrow, KasperskyLicensing::ILicensingError );
	CPPUNIT_TEST( testGetters );
	CPPUNIT_TEST_SUITE_END();
public:
	void setUp();
	void tearDown();

private:
	void testParsing();
	void testParsingThrow();
	void testGetters();

private:
	AutoPointer<IObjectImage> image1, image2;
};


#endif // TESTINDEX_H