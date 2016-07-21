#include <string>

#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/TextOutputter.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/TestRunner.h>


int main(int argc, char* argv[])
{

  // Create the event manager and test controller
  CPPUNIT_NS::TestResult controller;

  // Add a listener that collects test result
  CPPUNIT_NS::TestResultCollector result;
  controller.addListener( &result );        

  // Add a listener that print dots as test run.
  CPPUNIT_NS::BriefTestProgressListener progress;
  controller.addListener( &progress );      

  // Add the top suite to the test runner
  CPPUNIT_NS::TestRunner runner;
  runner.addTest( CPPUNIT_NS::TestFactoryRegistry::getRegistry().makeTest() );
  runner.run( controller );

  std::ostringstream os;

  // Print test in a compiler compatible format.
  CPPUNIT_NS::TextOutputter outputter(&result, os);
  outputter.write(); 

  std::cout << os.str();

  return result.wasSuccessful() ? 0 : 1;

	return 0;
}

