/*!
 * (C) 2003 Kaspersky Lab 
 * 
 * \file	server_ports.h
 * \author	Andrew Kazachkov
 * \date	30.01.2004 16:28:02
 * \brief	
 * 
 */

#ifndef __KL_SERVER_PORTS_H__
#define __KL_SERVER_PORTS_H__

//! ѕорты сервера
#define KLSRV_SP_SERVER_PORTS_ARRAY     L"KLSRV_SP_SERVER_PORTS_ARRAY"
#define KLSRV_SP_SERVER_PORTS_ARRAY_DEFAULT {14000}

//! SSL порты сервера
#define KLSRV_SP_SERVER_SSL_PORTS_ARRAY     L"KLSRV_SP_SERVER_SSL_PORTS_ARRAY"
#define KLSRV_SP_SERVER_SSL_PORTS_ARRAY_DEFAULT {13000}

//! Server UDP-port
#define KLNAG_SERVER_UDP_PORT           L"ServerUdpPort"
#define KLNAG_SERVER_UDP_PORT_DEFAULT   13000

//! Number of UDP-packets to send
#define KLNAG_SERVER_UDP_PACKETS            L"ServerUdpPackets"
#define KLNAG_SERVER_UDP_PACKETS_DEFAULT    5

//! SSL порты сервера
#define KLSRV_SP_SERVER_AKLWNGT_PORTS_ARRAY            L"KLSRV_SP_SERVER_AKLWNGT_PORTS_ARRAY"
#define KLSRV_SP_SERVER_AKLWNGT_PORTS_ARRAY_DEFAULT    {13292}


namespace KLSRV
{
    const int c_pServerPortsDefault[]=KLSRV_SP_SERVER_PORTS_ARRAY_DEFAULT;
    const int c_pServerSslPortsDefault[]=KLSRV_SP_SERVER_SSL_PORTS_ARRAY_DEFAULT;
    const int c_pServerAkLwNgtPortsDefault[]=KLSRV_SP_SERVER_AKLWNGT_PORTS_ARRAY_DEFAULT;
}



#endif //__KL_SERVER_PORTS_H__
