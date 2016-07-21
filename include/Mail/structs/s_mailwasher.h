/*!
*		
*		
*		(C) 2002 Kaspersky Lab 
*		
*		\file	s_mailwasher.h
*		\brief	Структуры, необходимые для работы майлвошера
*		
*		\author Vitaly DVi Denisov
*		\date	28.03.2005 17:40:04
*		
*		Example:	
*		
		// Начало новой POP3-сессии майлвошера: майлвошер посылает сообщение => GUI отрисовывает окно с пустым списком
		sysSendMsg(
			pmc_MAIL_WASHER, 
			pm_SESSION_BEGIN, 
			NULL, 
			&hPOP3Session,		
			SER_SENDMSG_PSIZE
		);

		// Конец POP3-сессии майлвошера: GUI посылает и ждет от майлвошера подтверждения pm_SESSION_END
		sysSendMsg(
			pmc_MAIL_WASHER, 
			pm_SESSION_CLOSE, 
			NULL, 
			&hPOP3Session,		
			SER_SENDMSG_PSIZE
		);

		// Конец POP3-сессии майлвошера: посылает майлвошер
		sysSendMsg(
			pmc_MAIL_WASHER, 
			pm_SESSION_END, 
			NULL, 
			&hPOP3Session,		
			SER_SENDMSG_PSIZE
		);

		// Запрос порции сообщений: GUI посылает запрос => майлвошер опрашивает сервер
		sysSendMsg(
			pmc_MAIL_WASHER, 
			pm_MESSAGE_GETLIST, 
			NULL, 
			&hPOP3MessageReguest, 
			SER_SENDMSG_PSIZE
		);

		// Обнаружено новое сообщение на сервере: майлвошер посылает сообщение => GUI показывает новую строчку в списке
		sysSendMsg(
			pmc_MAIL_WASHER, 
			pm_MESSAGE_FOUND, 
			NULL, 
			&hPOP3Message, 
			SER_SENDMSG_PSIZE
		);

		// Удалить список писем с сервера: GUI посылает список писем => майлвошер принимает задание на удаление
		sysSendMsg(
			pmc_MAIL_WASHER, 
			pm_MESSAGE_REMOVE, 
			NULL, 
			&hPOP3MessageList, 
			SER_SENDMSG_PSIZE
		);

		// Письмо удалено с сервера: майлвошер посылает сообщение => GUI отмечает соответствующую строчку в списке
		sysSendMsg(
			pmc_MAIL_WASHER, 
			pm_MESSAGE_DELETED, 
			NULL, 
			&hPOP3Message, 
			SER_SENDMSG_PSIZE
		);		
*		
*/		



#ifndef _S_MAILWASHER_H_
#define _S_MAILWASHER_H_

#include <Prague/prague.h>
#include <Prague/pr_cpp.h>
#include <Prague/pr_serializable.h>
#include <Prague/pr_vector.h>

#include <Mail/plugin/p_mailwasher.h>
#include <Mail/structs/s_antispam.h>

enum MailwasherStructId
{
	msid_Session = 0,
	msid_Message,
	msid_MessageList,
	msid_MessageReguest,
	msid_MessageDecode,
	msid_Last,
};

//-----------------------------------------------------------------------------
struct cPOP3Session : public cSerializable
{
	cPOP3Session():
		m_dwGUIContext(0),
		m_dwUserSession(0),
		m_dwPOP3Session(0),
		m_dwMessageCount(0),
		m_dwFirstNewMessage(0),
		m_dwNewMessageCount(0),
		m_dwMailboxSize(0)
		{};
	DECLARE_IID(cPOP3Session, cSerializable, PID_MAILWASHER, msid_Session);
	tDWORD     m_dwGUIContext;      // Контекст GUI майлвошера
	tDWORD     m_dwUserSession;	    // Сессия Windows (== SessionId, идентифицирует декстоп)
	tDWORD     m_dwPOP3Session;	    // Контекст сессии POP3 (м.б. несколько сессий в рамках одного десктопа Windows)
	tDWORD     m_dwMessageCount;    // Количество писем в почтовом ящике
	tDWORD     m_dwFirstNewMessage; // Первое новое сообщение (сообщения с бОльшими номерами, скорее всего, будут тоже новыми для почтового клиента)
	tDWORD     m_dwNewMessageCount; // Количество новых писем в почтовом ящике (m_dwMessageCount - m_dwFirstNewMessage)
	tDWORD     m_dwMailboxSize;     // Совокупный размер всех писем в почтовом ящике
};

IMPLEMENT_SERIALIZABLE_BEGIN( cPOP3Session, 0 )
	SER_VALUE( m_dwGUIContext,     tid_DWORD,      "GUIContext"  )
	SER_VALUE( m_dwUserSession,    tid_DWORD,      "UserSession" )
	SER_VALUE( m_dwPOP3Session,    tid_DWORD,      "POP3Session" )
	SER_VALUE( m_dwMessageCount,   tid_DWORD,      "MessageCount" )
	SER_VALUE( m_dwFirstNewMessage,tid_DWORD,      "FirstNewMessage" )
	SER_VALUE( m_dwNewMessageCount,tid_DWORD,      "NewMessageCount" )
	SER_VALUE( m_dwMailboxSize,    tid_DWORD,      "MailboxSize" )
IMPLEMENT_SERIALIZABLE_END( );
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
struct cPOP3MessageReguest : public cPOP3Session
{
	cPOP3MessageReguest():
		m_dwBegin(1),
		m_dwCount(10),
		m_dwSizeOf(0),
		cPOP3Session()
		{};
	DECLARE_IID(cPOP3MessageReguest, cPOP3Session, PID_MAILWASHER, msid_MessageReguest);
	tDWORD     m_dwBegin;		// Порядковый номер первого затребованного письма
	tDWORD     m_dwCount;		// Количество
	tINT       m_dwSizeOf;      // Количество строк письма для закачки. Если "-1" - закачивается письмо целиком
};

IMPLEMENT_SERIALIZABLE_BEGIN( cPOP3MessageReguest, 0 )
	SER_BASE( cPOP3Session,  0 )
	SER_VALUE( m_dwBegin,        tid_DWORD,       "Begin"  )
	SER_VALUE( m_dwCount,        tid_DWORD,       "Count"  )
	SER_VALUE( m_dwSizeOf,       tid_INT,         "SizeOf"  )
IMPLEMENT_SERIALIZABLE_END( );
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
struct cPOP3Message : public cPOP3Session
{
	cPOP3Message():
		m_dwNumber(-1),
		m_dwSize(0),
		m_bNew(cFALSE),
		m_bMarkToDelete(cFALSE),
		m_dwReason(asdr_NOTHING),
		m_bDeleted(cFALSE),
		m_dwSizeOf(0),
		cPOP3Session()
		{
			m_szCurrentEncode = "WINDOWS-1251";
		};
	DECLARE_IID(cPOP3Message, cPOP3Session, PID_MAILWASHER, msid_Message);
	tDWORD     m_dwNumber;		// Порядковый номер письма
	cStringObj m_szName;		// Имя письма // obvious. need to be deleted.
	cStringObj m_szFrom;
	cStringObj m_szTo;
	cStringObj m_szSubject;
	tDWORD     m_dwSize;		// Размер письма
	cStringObj m_szData;		// Первые несколько строчек письма
	cStringObj m_szDateTime;	// Поле "Date"
	tBOOL      m_bNew;			// Возможно, это новое сообщение для почтового клиента
	tBOOL      m_bMarkToDelete; // Сообщение помечено на удаление
	tDWORD     m_dwReason;      // Почему письмо помечено на удаление? (asDetectReason_t)
	tBOOL      m_bDeleted;      // Сообщение удалено
	cStringObj m_szCurrentEncode;	// Текущая кодировка
	tINT       m_dwSizeOf;      // Количество строк письма для закачки. Если "-1" - закачивается письмо целиком
	cStringObj m_szVirusname;	// Найденный вирус 
};

IMPLEMENT_SERIALIZABLE_BEGIN( cPOP3Message, 0 )
	SER_BASE( cPOP3Session,  0 )
	SER_VALUE( m_dwNumber,        tid_DWORD,       "Number"  )
	SER_VALUE( m_szName,          tid_STRING_OBJ,  "Name"  )
	SER_VALUE( m_szFrom,          tid_STRING_OBJ,  "From"  )
	SER_VALUE( m_szTo,            tid_STRING_OBJ,  "To"  )
	SER_VALUE( m_szSubject,       tid_STRING_OBJ,  "Subject"  )
	SER_VALUE( m_dwSize,          tid_DWORD,       "Size"  )
	SER_VALUE( m_szData,          tid_STRING_OBJ,  "Data"  )
	SER_VALUE( m_szDateTime,      tid_STRING_OBJ,  "DateTime"  )
	SER_VALUE( m_bNew,            tid_BOOL,        "New"  )
	SER_VALUE( m_bMarkToDelete,   tid_BOOL,        "MarkToDelete"  )
	SER_VALUE( m_dwReason,        tid_DWORD,       "Reason"  )
	SER_VALUE( m_bDeleted,        tid_BOOL,        "Deleted"  )
	SER_VALUE( m_dwSizeOf,        tid_INT,         "SizeOf"  )
	SER_VALUE( m_szCurrentEncode, tid_STRING_OBJ,  "CurrentEncode"  )
	SER_VALUE( m_szVirusname,     tid_STRING_OBJ,  "Virusname"  )
IMPLEMENT_SERIALIZABLE_END( );
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
struct cPOP3MessageDecode : public cPOP3Message
{
	cPOP3MessageDecode():
		cPOP3Message()
		{};
	DECLARE_IID(cPOP3MessageDecode, cPOP3Message, PID_MAILWASHER, msid_MessageDecode);
	cStringObj m_DecodeFrom;		// Имя кодировки, из которой следует декодировать письмо
									//	"KOI8-R",		
									//	"WINDOWS-1251",	
									//	"IBM866",		
									//	"ISO8859-5",	
									//	"X-MAC-CYRILLIC"
};

IMPLEMENT_SERIALIZABLE_BEGIN( cPOP3MessageDecode , 0 )
	SER_BASE( cPOP3Message,  0 )
	SER_VALUE( m_DecodeFrom,     tid_STRING_OBJ,  "DecodeFrom"  )
IMPLEMENT_SERIALIZABLE_END( );
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
typedef cVector<cPOP3Message> POP3MessageList_t;
struct cPOP3MessageList : public cPOP3Session
{
	cPOP3MessageList():
		cPOP3Session()
		{};
	DECLARE_IID(cPOP3MessageList, cSerializable, PID_MAILWASHER, msid_MessageList);
	POP3MessageList_t m_POP3MessageList;
};

IMPLEMENT_SERIALIZABLE_BEGIN( cPOP3MessageList, 0 )
	SER_BASE( cPOP3Session,  0 )
	SER_VECTOR(m_POP3MessageList, cPOP3Message::eIID, "MessageList")
IMPLEMENT_SERIALIZABLE_END( );
//-----------------------------------------------------------------------------



#endif//_S_MAILWASHER_H_
