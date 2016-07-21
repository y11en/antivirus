/*!
*		(C) 2002 Kaspersky Lab 
*		
*		\file	Mailer.h
*		\brief	
*		
*		\author Андрей Брыксин
*		\date	26.12.2002 11:55:42
*		
*/		

#ifndef _MAILER_H_
#define _MAILER_H_

#ifdef KLMAIL_EXPORTS
	#define KLMAIL_DECL KLSTD_DLLEXPORT
#else
	#define KLMAIL_DECL KLSTD_DLLIMPORT
#endif

namespace KLMAIL {

	/*!
    * \brief Значения по умолчанию
    */
	const int DEFAULT_SMTP_PORT = 25;
	const wchar_t CONTENT_TYPE_TEXT[] = L"text/plain";
	const wchar_t CONTENT_TYPE_HTML[] = L"text/html";
	const wchar_t CONTENT_TYPE_PNG[] = L"image/png";

	typedef enum TransferEncoding {
		ENCODING_NO,
		ENCODING_BASE64,
		ENCODING_QUOTED_PRINTABLE
	};

	typedef std::vector<std::wstring> ATTACHMENTS;

	/*!
    * \brief Определяет часть MIME сообщения. 
			 Используется для отправки сообщения состоящего из нескольких объектов
			 (например HTML страница с картинками)
    */
	class MessageBodyPart {
	public:
		/*!
		* \brief В зависимости от флага m_bTreatObjectAsFileName определяет либо саму часть сообщения,
				 либо путь к файлу из которого следует ее загрузить
		*/
		std::wstring m_wstrObject;
		bool m_bTreatObjectAsFileName;

		/*!
		* \brief Идентификатор части сообщения. Используется для ссылок на MIME объект 
				(например, на картинку из html-файла : <img="cid:ContentID">)
		*/
		std::wstring m_wstrContentID;

		/*!
		* \brief Тип части сообщения. Используется MIME именование типов 
				(например 'text/plain', 'text/html', 'image/png' и т. д.)
		*/
		std::wstring m_wstrContentType;

		/*!
		* \brief Тип кодирования части сообщения. Для текста используется ENCODING_NO,
				 для файловых данных(картинки) - ENCODING_BASE64
		*/
		TransferEncoding m_Encoding;

		/*!
		* \brief Кодировка части сообщения. Используется MIME именование типов 
				(например 'utf-8', 'windows-1251' и т. д.). По умолчанию: 'windows-XXXX'
		*/
		std::wstring m_wstrCharset;

		MessageBodyPart():
			m_bTreatObjectAsFileName(false),
			m_Encoding(ENCODING_NO)
		{
		}
		MessageBodyPart(const std::wstring& wstrObject,
						bool bTreatObjectAsFileName = false,
						const std::wstring& wstrContentType = CONTENT_TYPE_TEXT,
						const std::wstring& wstrContentID = L"",
						TransferEncoding Encoding = ENCODING_NO,
						const std::wstring& wstrCharset = L""):
			m_wstrObject(wstrObject),
			m_wstrContentID(wstrContentID),
			m_bTreatObjectAsFileName(bTreatObjectAsFileName),
			m_wstrContentType(wstrContentType),
			m_Encoding(Encoding),
			m_wstrCharset(wstrCharset)
		{
		}
	};
	typedef std::vector<MessageBodyPart> MESSAGE_PARTS;

	/*!
    * \brief Интерфейс для отправки почтовых сообщений по SMTP или MAPI
    */
    class Mailer : public KLSTD::KLBase
	{
    public:
		virtual void Init(bool bAsService = false) = 0;
		
		virtual void StartUseSMTP(std::wstring sSMTPServerAddress, 
								  int nSMTPServerPort = DEFAULT_SMTP_PORT) = 0;

	/*!
    * \brief Поставить письмо в очередь на отправку
		\param wstrTo, wstrCC, wstrBCC	- адрес для отправки письма user@domain.net или User Friendly Name<user@domain.net>
					  также можно указать список адресов разделенных ',' или ';'
		\return - идентификатор посылки по которому в дальнейшем можно 
				  получить нотификацию о состоянии отправки
    */
		virtual std::wstring SendMailQueued(std::wstring sTo, 
											std::wstring sCC,
											std::wstring sBCC,
											std::wstring sFrom,
											std::wstring sSubject,
											std::wstring sMessage,
											std::wstring sAttachment = L"") = 0;
		virtual void Done() = 0;

		virtual void InitEx(bool bStartQueuer = true) = 0;
	/*!
    * \brief Поставить письмо в очередь на отправку
		\param wstrTo, wstrCC, wstrBCC	- адрес для отправки письма user@domain.net или User Friendly Name<user@domain.net>
					  также можно указать список адресов разделенных ',' или ';'
		\param bQueued - определяет отправлять сообщения через очеред или синхронно
		\return - идентификатор посылки по которому в дальнейшем можно 
				  получить нотификацию о состоянии отправки
    */
		virtual std::wstring SendMailEx(const std::wstring& wstrTo, 
										const std::wstring& wstrCC, 
										const std::wstring& wstrBCC,
										const std::wstring& wstrFrom,
										const std::wstring& wstrSubject,
										const std::wstring& wstrESMTPUserName,
										const std::wstring& wstrESMTPUserPassword,
										const MESSAGE_PARTS& arMessageParts,
										const ATTACHMENTS& arAttachments,
										bool bQueued) = 0;
	};
};

/*!
  \brief Создает объект Mailer

	\param ppMailer		[out] Указатель на объект
*/
KLMAIL_DECL void KLMAIL_CreateMailer(KLMAIL::Mailer** ppMailer);

/*!
  \brief Создает объект Mailer

	\param sMailerName	[in]  Имя программы - отправителя(прописывается в заголовок X-Mailer письма)
	\param ppMailer		[out] Указатель на объект
*/
KLMAIL_DECL void KLMAIL_CreateMailerEx(std::wstring sMailerName, KLMAIL::Mailer** ppMailer);

#endif
