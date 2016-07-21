// ProxySession.h: interface for the CProxySession class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PROXYSESSION_H__1E87F3B9_3C38_49E7_81D9_34DB62CBAF45__INCLUDED_)
#define AFX_PROXYSESSION_H__1E87F3B9_3C38_49E7_81D9_34DB62CBAF45__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <mail/structs/s_mc_trafficmonitor.h>
#include <prague/pr_wrappers.h>
#include "TransparentSocket.h"
#include "SimpleBufferWindow.h"
#include "ReadWriteSocket.h"
#include "ProxySessionManager.h"

using namespace TrafficProtocoller;
class CProxySessionManager;

// послана команда start протоколлерам
#define     FPS_START_SENT              0x1
// послана команда stop протоколлерам
#define     FPS_STOP_SENT               0x2
// установлено соединение с сервером
#define     FPS_SERVER_CONNECTED        0x4
// установлено соединение с клиентом
#define     FPS_CLIENT_CONNECTED        0x8
// разорвано соединение с сервером
#define     FPS_SERVER_DISCONNECTED     0x10
// разорвано соединение с клиентом
#define     FPS_CLIENT_DISCONNECTED     0x20
// у клиента есть поток, в котором он работает.
#define     FPS_HAS_THREAD              0x40
// первого селекта не было
#define     FPS_FIRST_SELECT            0x80
// выделены контексты для протоколлеров
#define     FPS_CTX_ALLOCATED           0x100
// сессия переключена в ssl режим
#define     FPS_SWITCHED_SSL            0x200

#define     FPS_GOT_KAVSEND             0x400
#define     FPS_GOT_KAVSVC              0x800
#define     FPS_GOT_KAVCONN_ID          0x1000
// сессия полностью проинициализирована и готова к работе
#define     FPS_ESTABLISHED             0x2000
// сессия в состоянии разрыва
#define     FPS_FINISH                  0x4000
// соединения порваны. можно удалять.
#define     FPS_DONE                    0x8000



class CProxySession
{
public:	
    CProxySession( hOBJECT hParent = NULL , CProxySessionManager* manager = NULL, HANDLE StopEvent = NULL );
	virtual ~CProxySession();

    ULONG                   m_Flags;    // флаги сессии

    DWORD                   m_dwCDCount; // client data count;

    bool                    m_bInWaitSessionList;

    CProxySessionManager*   m_manager;
    cStringObj              m_szSSLCacheFolder;    
    HANDLE                  m_hStopEvent;
    hOBJECT                 m_hParent;

    DWORD                   m_ThreadID;
    HANDLE                  m_ThreadHandle;

    CTransparentSocket      m_server;            // Сокет сервера
    CTransparentSocket      m_client;            // Сокет клиента

    tINT                    m_ServerPort;
    SOCKET                  m_AcceptedSock;
    SOCKET                  m_ConnectSocket;

    char                    m_rIp[50];              // удаленный адрес серверного соединения
    char                    m_lIp[50];              // локальный адрес серверного соединения
    char                    m_Localhost[50];        // локальный адрес клиентского соединения

    CSimpleBufferWindow*    m_Buf_Client;
    CSimpleBufferWindow*    m_Buf_Server;

    ClearCallbackData*      m_ccdClient;
    ClearCallbackData*      m_ccdServer;

    __int64                 m_ID;                   // ID для редиректора
    ULONG                   m_StartTime;
    

    tERROR  ParseKavsendBuffer  ( char* Buf, int nRecv );
    tERROR  ParseKavsvcBuffer   ( char* Buf, int nRecv );
    tERROR  ParseKavconnIdBuffer( char* Buf, int nRecv );

    tERROR  StartProtocollers();
    tERROR  StopProtocollers();    
	
	tERROR ReceiveProtocollerCommand(
        IN hOBJECT p_send_point, 
        IN process_msg_t* ppmCommand
        );
	tERROR SendProtocollerDetect(
		IN data_source_t dsDataSource,   // Источник данных
		IN tVOID* pLastReceivedData,     // Буфер с данными для оценки
		IN tULONG ulLastReceivedData     // Размер буфера
		);
	tERROR SendProtocollerDetectEx(
		IN data_source_t dsDataSource,   // Источник данных
		IN tVOID* pDataBuffer,           // Полный буфер с данными для оценки (все данные, переданные в направлении dsDataSource в состоянии dcProtocolLikeDetected)
		IN tULONG ulDataBuffer,          // Размер полного буфера
		IN tVOID* pLastReceivedData,     // Буфер с данными для оценки (расположен в конце полного буфера)
		IN tULONG ulLastReceivedData     // Размер буфера
		);
	tERROR SendProtocollerProcess(
		IN data_source_t dsDataSource,   // Источник данных
		IN tVOID* pLastReceivedData,     // Буфер с данными для обработки
		IN tULONG ulLastReceivedData     // Размер буфера
		);
	tERROR PseudoStop(
		IN hOBJECT hProtocoller          // Стопим сессию у конкретного протоколлера
		);

    static void ClearCallback(CSimpleBufferWindow* _this, tDWORD context, tVOID* data);


    tERROR          ReadKAVSEND();
    tERROR          DoConnect();    
    tINT            ProcessClient();
    
    void            MakeAlert();
    void            ProcessFirstSelect();

    bool            StartClient();    
    void            WaitExitThread();
    void            StopSession();
    void            ProcessExit();
    void            BreakKavsvcConnect();

    void            RemoveFromWaitSessionList();
    
	hOBJECT			m_hProtocoller;			// Указатель на активный протоколлер
	detect_code_t	m_dcDetectCode;			// Текущее состояние протокола
	session_t		m_hSessionCtx;			// Контекст сессии
	HANDLE			m_hCurrentUser;			// Хендл текущего пользователя Windows (который запустил процесс с указанным PID)

	tBOOL NeedToSwitchToSSL(cTrafficSelfAskAction& szSSLCacheFolder);

private:
	struct send_buf_t {
        tBYTE*  buf;		// Указатель на буфер
        tSIZE_T	fullsize;	// Аллоцированный размер для буфера

		send_buf_t();
		~send_buf_t();
		tBOOL ready();
		tBOOL alloc_more(hIO hData);
		tBOOL alloc_more(tSIZE_T size);
		tBOOL clear();
	};
	
	send_buf_t m_send_buf;	// Динамический буфер для отправки данных
	PRLocker   m_send_buf_cs; // Объект синхронизации для m_send_buf
	SOCKET	   m_socketFromSave;		// запоминаем сокет для определения смены контекста данных
};

// для уничтожения CProxySession в контексте потока сервера.
struct CDoneSession
{
    bool operator ()(CProxySession* pSession) 
    {
        if ( pSession->m_Flags & FPS_DONE )
        {
            if ( pSession->m_bInWaitSessionList )
            {
                pSession->RemoveFromWaitSessionList();
            }
            delete pSession;
            return true;
        }

        // Мы законнектились на сервер. А KAVCONN_ID не пришел. ( драйвер должен это делать моментально )
        // В этом случае будем считать, что что-то пошло не так ( клиент закрыл порт ), порвем коннект и завершимся.
        if ( 
            ( pSession->m_Flags & FPS_GOT_KAVSVC ) && 
            ( pSession->m_Flags & FPS_SERVER_CONNECTED ) &&
            !( pSession->m_Flags & FPS_GOT_KAVCONN_ID ) )
        {   
#ifdef _DEBUG
			const int seconds_timeout = 30;
#else
			const int seconds_timeout = 3;
#endif
            if ( GetTickCount() - pSession->m_StartTime > 1000 * seconds_timeout )
            {
                pSession->m_Flags |= FPS_FINISH;
            }
        }

        if ( pSession->m_Flags & FPS_FINISH )
        {
            pSession->WaitExitThread();
            pSession->ProcessExit();
            if ( pSession->m_bInWaitSessionList )
            {
                pSession->RemoveFromWaitSessionList();
            }

            delete pSession;
            return true;
        }

        return false; 
    } 
};

int
AssociateCR( SOCKET client, SOCKET server );

tBOOL GetParam(cStringObj& sBuf, cStringObj& sRes, tINT& nStart);

tBOOL ParseKAVSEND( 
                   IN tCHAR* Buf, 
                   IN tDWORD dwSize, 
                   OUT cStringObj& szServerName, 
                   OUT unsigned short& nPort,
                   OUT unsigned short& nMaskedPort,
                   OUT tDWORD& dwClientPID
                   );

LPCSTR GetDetectCodeString(tDWORD code);

#endif // !defined(AFX_PROXYSESSION_H__1E87F3B9_3C38_49E7_81D9_34DB62CBAF45__INCLUDED_)
