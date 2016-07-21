#include "Configuration.h"

#include <iostream>

#include <sstream>

//////////////////////////////////////////////////////
///// ConfigurationManager::Configuration
ConfigurationManager::Configuration::Configuration()
 // set default port number
 : m_port(2000),
   m_logFileName("TrafficTracker.log")
{
}

std::string ConfigurationManager::Configuration::toString()const
{
    std::stringstream stream;
    const char *serverAddressName = inet_ntoa(m_serverAddress.sin_addr);
    stream << "server address " << (serverAddressName ? serverAddressName : "")
        << ":" << ntohs(m_serverAddress.sin_port)
        << ", Traffic Tracker uses port " << m_port
        << ", log file name is 'NNNN_" << m_logFileName << "'";
    return stream.str();
}



//////////////////////////////////////////////////////
///// ConfigurationManager

ConfigurationManager &ConfigurationManager::instance()
{
    static ConfigurationManager configurationManager;
    return configurationManager;
}
ConfigurationManager::~ConfigurationManager()
{
}

bool ConfigurationManager::parse(int argc, char *argv[])
{
    for(int i = 0; i < argc; ++i)
    {
        std::string currentParameter(argv[i]);

        // remote address
        if(currentParameter == "-s")
        {
            if(i == argc)
            {
                std::cerr << "Error: '-s' parameter presents, but address is absent" << std::cout;
                return false;
            }

            const std::string address = argv[i + 1];

            std::string nodename;
            std::string servname;
            if(address.find(":") != std::string::npos)
            {
                nodename.assign(address.c_str(), address.find(":"));
                servname.assign(address.c_str() + address.find(":") + 1);
            }
            else
            {
                // port number is not specified
                nodename = address;
                // default port number
                servname = "80";
            }

            addrinfo hints;
            memset(&hints, 0, sizeof addrinfo);
            hints.ai_family = AF_INET;
            hints.ai_socktype = SOCK_STREAM;
            hints.ai_protocol = IPPROTO_TCP;
            addrinfo *addressList = 0;

            const int result = getaddrinfo(nodename.c_str(), servname.c_str(), &hints, &addressList);
            // save address information
            if(addressList && (result == 0))
                memcpy(&m_configuration.m_serverAddress, addressList->ai_addr, sizeof sockaddr);

            // free resource
            if(addressList)
                freeaddrinfo(addressList);

            if(result != 0)
            {
                std::cerr << "Error: unable to parse address '" << address << "', last error " << WSAGetLastError() << ", result " << result << std::endl;
                return false;
            }

            // skipping already parsed server address command-line parameter
            ++i;
            continue;
        }
  
        // port number
        if(currentParameter == "-p")
        {
            if(i == argc)
            {
                std::cerr << "Error: '-p' parameter presents, but port number is absent" << std::cout;
                return false;
            }

            m_configuration.m_port = atoi(argv[i + 1]);
            if(!m_configuration.m_port)
            {
                std::cerr << "Error: unable to port number " << argv[i + 1] << std::endl;
                return false;
            }

            // skipping already parsed port number command-line parameter
            ++i;
            continue;
        }

        // trace file
        if(currentParameter == "-f")
        {
            if(i == argc)
            {
                std::cerr << "Error: '-p' parameter presents, but port number is absent" << std::cout;
                return false;
            }

            m_configuration.m_logFileName = argv[i + 1];
            if(m_configuration.m_logFileName.empty())
            {
                std::cerr << "Error: unable to log file name " << argv[i + 1] << std::endl;
                return false;
            }

            // skipping already parsed log file name command-line parameter
            ++i;
            continue;
        }
    }

    return requiredParametersParsed();
}

void ConfigurationManager::usage()const
{
    std::cerr << "Traffic Tracker intercepts all traffic to specified server and traces" << std::endl
        << "   session to file. Traffic Tracker acts as 'transperent' proxy server" << std::endl
        << "  and is needed for analysis of problems" << std::endl
        << std::endl
        << "Usage:" << std::endl
        << "TrafficTracker.exe -s <server address[:port]> [-p <port number>] [-f <log file>]" << std::endl
        << "  <server address[:port]> - server address to use for interception of traffic." << std::endl
        << "     All traffic received by Traffic Tracker will be forwarded to given" << std::endl
        << "    resource and logged to file. Default remote port number is 80" << std::endl
        << "  <port number> - port number to use by Traffic Tracker. Default is " << Configuration().m_port << std::endl
        << "  <log file> - the name of file to append dump of network session. Default" << std::endl
        << "     is NNNN_" << Configuration().m_logFileName << ". The client port number NNNN" << std::endl
        << "    is appended to the name of log file to distinguish sessions" << std::endl
        << std::endl
        << "Sample: TrafficTracker.exe -s 192.168.0.1:80 -p 2000 -f dumpFile.log" << std::endl;
}

bool ConfigurationManager::requiredParametersParsed()const
{
    // port number of server is not 0
    return m_configuration.m_serverAddress.sin_port != 0;
}

ConfigurationManager::ConfigurationManager()
{
}
