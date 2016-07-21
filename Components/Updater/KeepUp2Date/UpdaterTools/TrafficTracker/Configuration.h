#if !defined(AFX_CONFIGURATION_H__3049F698_2DBA_496B_99AB_23BD8D99FD0E__INCLUDED_)
#define AFX_CONFIGURATION_H__3049F698_2DBA_496B_99AB_23BD8D99FD0E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <string>

#include <winsock2.h>
#include <ws2tcpip.h>


/// the ConfigurationManager class parses input arguments
class ConfigurationManager
{
public:
    /// the Configuration structure groups and holds parameters
    struct Configuration
    {
        /// initialize configuration parameters with default values
        Configuration();
        /// outputs configuration as user-readable string
        std::string toString()const;

        // server address to connect to
        sockaddr_in m_serverAddress;



        // port number to run Traffic Tracker
        unsigned short m_port;
        std::string m_logFileName;
    };



	virtual ~ConfigurationManager();
    /// return a single instance of ConfigurationManager class
    static ConfigurationManager &instance();

    /// parse parameters
    bool parse(int argc, char *argv[]);
    /// output applicaiton usage
    void usage()const;


    Configuration m_configuration;

private:
    /// checks whether all required parameters are parsed
    bool requiredParametersParsed()const;
    /// disable to create other instances of ConfigurationManager other then singleton instance
  	ConfigurationManager();

};

#endif // !defined(AFX_CONFIGURATION_H__3049F698_2DBA_496B_99AB_23BD8D99FD0E__INCLUDED_)
