#ifndef TESTSTORAGE_H
#define TESTSTORAGE_H

class TestStorage : public CPPUNIT_NS::TestFixture
{
	CPPUNIT_TEST_SUITE( TestStorage );
	CPPUNIT_TEST( testReadError );
	CPPUNIT_TEST_SUITE_END();

	void testReadError();
};


#endif // TESTSTORAGE_H