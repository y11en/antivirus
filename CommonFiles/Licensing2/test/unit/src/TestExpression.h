#ifndef TESTEXPRESSION_H
#define TESTEXPRESSION_H

class TestExpression : public CPPUNIT_NS::TestFixture
{
	CPPUNIT_TEST_SUITE( TestExpression);
	CPPUNIT_TEST( testExtraBrackets );
	CPPUNIT_TEST_EXCEPTION( testUnbalancedBrackets, std::runtime_error );
	CPPUNIT_TEST_EXCEPTION( testDivisionByZero, std::overflow_error );
	CPPUNIT_TEST_SUITE_END();

	void testExtraBrackets();
	void testUnbalancedBrackets();
	void testDivisionByZero();

};

#endif // TESTEXPRESSION_H