#ifndef TESTLICENSECONTEXT_H
#define TESTLICENSECONTEXT_H

class TestLicenseContext : public CPPUNIT_NS::TestFixture
{
  CPPUNIT_TEST_SUITE( TestLicenseContext );
  CPPUNIT_TEST( testCreation );
  CPPUNIT_TEST( testAddition );
  CPPUNIT_TEST( testCleaning );
  CPPUNIT_TEST( testIdentity );
  CPPUNIT_TEST_SUITE_END();

public:
  void setUp();
  void tearDown();

private:
  void testCreation();
  void testAddition();
  void testCleaning();
  void testIdentity();

private:
  void FillContext(LicenseContext& c);
};


#endif // TESTLICENSECONTEXT_H