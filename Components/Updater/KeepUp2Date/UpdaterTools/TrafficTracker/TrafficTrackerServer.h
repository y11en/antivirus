#if !defined(AFX_TRAFFICTRACKERSERVER_H__004EEEE8_B42C_4DA9_9366_3405562E6576__INCLUDED_)
#define AFX_TRAFFICTRACKERSERVER_H__004EEEE8_B42C_4DA9_9366_3405562E6576__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Socket.h"

#include <fstream>


/// the Traffic Tracker Server listens incoming TCP connectoin
///  and runs thread-per-connection to serve client requests
class TrafficTrackerServer  
{
public:
	TrafficTrackerServer();
	virtual ~TrafficTrackerServer();

    // listen incomning connection and create handler thread to serve connections
    void run();


private:
    // Handler serves connection and log its session in separate thread
    class Handler
    {
    public:
        // creates a handler which starts thread, which processes connection
        //  @param client [in] - client connection
        //  @param port [in] - port number to form log file
        Handler(SOCKET client, const unsigned short port);

        // run thread function within context of Handler class
        //  deletes Handler object when thread finishes
        static unsigned int __stdcall threadRunner(void *handler);
    private:
        // disabling creation of Handler in stack, because it deletes itself after thread function finishes
        ~Handler();
        void threadFunction();
        std::string logFileName()const;

        // establish connection to server
        SOCKET connectToServer();

        // forwards data from one socket to other
        // @return false in case input socket is closed or other error happened
        bool forwardData(Socket &input, Socket &output);

        std::ofstream m_logFile;
        Socket m_connectionToClient;
        const unsigned short m_port;
    };

};

#endif // !defined(AFX_TRAFFICTRACKERSERVER_H__004EEEE8_B42C_4DA9_9366_3405562E6576__INCLUDED_)
