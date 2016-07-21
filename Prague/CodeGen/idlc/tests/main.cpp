// Boost.Test
#include <boost/test/framework.hpp>
#include <boost/test/results_collector.hpp>
#include <boost/test/unit_test_suite.hpp>
#include <boost/test/results_reporter.hpp>

#include <boost/test/detail/unit_test_parameters.hpp>

// Boost
#include <boost/cstdlib.hpp>

// STL
#include <stdexcept>
#include <iostream>

#include <boost/test/detail/suppress_warnings.hpp>

//____________________________________________________________________________//

// ************************************************************************** //
// **************                 unit test main               ************** //
// ************************************************************************** //

extern void init();
extern void term();


int BOOST_TEST_CALL_DECL main(int argc, char* argv[]) {
    using namespace boost::unit_test;

    init();
       
    try {
        framework::init( argc, argv );

        framework::run();

        results_reporter::make_report();

        return runtime_config::no_result_code() 
                    ? boost::exit_success 
                    : results_collector.results( framework::master_test_suite().p_id ).result_code();
    }
    catch( std::logic_error const& ex ) {
        std::cerr << "Boost.Test internal framework error: " << ex.what() << std::endl;
        
        return boost::exit_exception_failure;
    }
    catch( ... ) {
        std::cerr << "Boost.Test internal framework error: unknown reason" << std::endl;
        
        return boost::exit_exception_failure;
    }
    
    term();
    return 0;
}

//____________________________________________________________________________//

#include <boost/test/detail/enable_warnings.hpp>
