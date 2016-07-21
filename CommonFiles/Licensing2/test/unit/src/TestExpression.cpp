#include "precompiled.h"

#include <stack>
#include <utility/pointainer.h>
#include <expression.h>
#include <cppunit/extensions/HelperMacros.h>

using namespace KasperskyLicensing;
using namespace KasperskyLicensing::Implementation;

#include "TestExpression.h"

CPPUNIT_TEST_SUITE_REGISTRATION( TestExpression );

void TestExpression::testExtraBrackets()
{
	expression e;
	calculation_context c = { 0 };

	e.parse("((1))");
	CPPUNIT_ASSERT_EQUAL(1.0, e.evaluate(c));

	e.clear();

	e.parse("(((1 + (2)) * 3))");
	CPPUNIT_ASSERT_EQUAL(9.0, e.evaluate(c));

	e.clear();

	e.parse("(2)");
	CPPUNIT_ASSERT_EQUAL(2.0, e.evaluate(c));
}

void TestExpression::testUnbalancedBrackets()
{
	expression e;
	e.parse("((1)");
}

void TestExpression::testDivisionByZero()
{
	expression e;
	e.parse("1 / 0");
	calculation_context c  = { 0 };
	e.evaluate(c);
}
