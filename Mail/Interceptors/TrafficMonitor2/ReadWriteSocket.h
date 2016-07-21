#ifndef _READ_WRITE_SOCKET_H_
#define _READ_WRITE_SOCKET_H_

#define	SOCKETBUFSIZE					0x2000

#ifndef SO_UPDATE_CONNECT_CONTEXT
	#define SO_UPDATE_CONNECT_CONTEXT   0x7010
#endif // SO_UPDATE_CONNECT_CONTEXT

using namespace TrafficProtocoller;

tBOOL ParseKAVSEND( 
                   IN tCHAR* Buf, 
                   IN tDWORD dwSize, 
                   OUT cStringObj& szServerName, 
                   OUT unsigned short& nPort,
                   OUT unsigned short& nMaskedPort,
                   OUT tDWORD& dwClientPID
                   );

SOCKET Connect(	
               IN OUT SOCKET& conn,
               IN tCHAR* lpszServerName, 
               IN tINT nPort, 
               OUT tBOOL& bNeedToAlert
               );

LPCSTR GetDetectCodeString(tDWORD code);

struct ClearCallbackData
{
    ClearCallbackData( data_source_t t = dsUnknown ): data_source(t) {};
    data_source_t data_source;
};

#endif//_READ_WRITE_SOCKET_H_