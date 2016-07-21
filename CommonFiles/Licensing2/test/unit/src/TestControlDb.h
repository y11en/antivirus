#ifndef TESTCONTROLDB_H
#define TESTCONTROLDB_H

class TestControlDb : public CPPUNIT_NS::TestFixture
{
	CPPUNIT_TEST_SUITE( TestControlDb );
	CPPUNIT_TEST( testBlistEntries );
	CPPUNIT_TEST( testLoadStatusOk );
	CPPUNIT_TEST( testLoadBadIndex );
	CPPUNIT_TEST( testLoadBadBlistBases );
	CPPUNIT_TEST( testLoadBadBlist );
	CPPUNIT_TEST( testLoadNoRegistry );
	CPPUNIT_TEST_SUITE_END();

	void testBlistEntries();
	void testLoadStatusOk();
	void testLoadBadIndex();
	void testLoadBadBlistBases();
	void testLoadBadBlist();
	void testLoadNoRegistry();
};

#endif // TESTCONTROLDB_H