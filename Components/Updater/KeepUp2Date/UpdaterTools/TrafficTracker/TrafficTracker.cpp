#include "Configuration.h"
#include "NetworkInitializer.h"
#include "TrafficTrackerServer.h"

#include <iostream>


/// this applicaiton tracks traffic to file to provide way to analysis network problems
int main(int argc, char *argv[])
{
    // initialize network
    NetworkInitializer networkInitializer;
    if(!networkInitializer.initialized())
        return false;

    // parse command-line parameters
    if(!ConfigurationManager::instance().parse(argc, argv))
    {
        ConfigurationManager::instance().usage();
        return 0;
    }
    std::cout << "Configuraion parsed: " << ConfigurationManager::instance().m_configuration.toString() << std::endl;


    // run Traffic Tracker Server
    TrafficTrackerServer().run();

    return 0;
}
