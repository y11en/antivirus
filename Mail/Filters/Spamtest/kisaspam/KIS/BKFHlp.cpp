#include <Prague/prague.h>
#include <string>

#include "BKFHlp.h"

// получает MESSAGEPART_INFO html или plain
static MESSAGEPART_INFO *BKF_GetBodyMessagePart(MESSAGE_INFO *pMsg, bool bIsHTML)
{
	MESSAGEPART_INFO* pAtt = NULL;
	unsigned long i;
	for(i = 0; pAtt = pMsg->GetMessagePart(i); ++i)
	{
		const char *pObjName = pAtt->GetPropStr(pgOBJECT_NAME);
		if(bIsHTML && (strcmp(pObjName, MSG_CONTENT_HTML_BODY) == 0 || _stricmp(pObjName, "text/html") == 0) ||
			!bIsHTML && (strcmp(pObjName, MSG_CONTENT_PLAIN_BODY) == 0 || _stricmp(pObjName, "text/plain") == 0))
		{
			return pAtt;
		}
		pMsg->ReleaseMessagePart(pAtt);
	}
	return NULL;
}

// получает html или plain тело
static std::string BKF_GetBodyString(MESSAGE_INFO *pMsg, bool bIsHTML)
{
	std::string strMessageBody;
	MESSAGEPART_INFO* pAtt = NULL;
	pAtt = BKF_GetBodyMessagePart(pMsg, bIsHTML);
	if(pAtt)
	{
		IStream* pData = pAtt->GetData();
		if(pData)
		{
			char buffer[1024];
			ULONG read_bytes;
			while((read_bytes = 0, SUCCEEDED(pData->Read(buffer, sizeof(buffer), &read_bytes))) && read_bytes)
				strMessageBody += std::string(buffer, read_bytes);
		}
		pMsg->ReleaseMessagePart(pAtt);
	}
	else
	{
		PR_TRACE((0, prtIMPORTANT, "spamtst\t%s body not found", bIsHTML ? "html" : "plain"));
	}

	return strMessageBody;
}

// строит MIME из заголовков и тел
static std::string BKF_BuildMIME(const char *pInetHeaders,
								 const std::string &strPlainBody,
								 const std::string &strHTMLBody)
{
	std::string strMIME;

	if(!pInetHeaders || strPlainBody.empty() && strHTMLBody.empty())
		return "";

	// создадим временное IO
	cAutoObj<cIO> TempIO;
	tDWORD dwWritten = 0;
	tERROR err = g_root->sysCreateObjectQuick(TempIO, IID_IO, PID_TMPFILE);
	// пропишем туда заголовки
	if(PR_SUCC(err))
		TempIO->SeekWrite(&dwWritten, 0, (tPTR)pInetHeaders, strlen(pInetHeaders));
	if(PR_SUCC(err))
	{
		cAutoObj<cOS> MessageOS;
		// создаем MSOE OS
		err = TempIO->set_pgOBJECT_ORIGIN(OID_MAIL_MSG_MIME);
		err = TempIO->sysCreateObjectQuick(MessageOS, IID_OS, PID_MSOE);
		if(PR_SUCC(err))
		{
			cAutoObj<cObjPtr> MessageObjPtr;
			{
				err = MessageOS->PtrCreate(MessageObjPtr, NULL);
				if(PR_SUCC(err))
				{
					bool bPlainInserted = false;
					bool bHtmlInserted = false;
					// бежим по телам и вставляем пришедшие в аргументах
					while(PR_SUCC(err = MessageObjPtr->Next()))
					{
						cAutoObj<cIO> BodyIO;
						if(PR_SUCC(err))
							err = MessageObjPtr->IOCreate(BodyIO, 0, fACCESS_RW, fOMODE_CREATE_IF_NOT_EXIST);
						if(PR_SUCC(err))
						{
							cStringObj sContentType;
							sContentType.assign(BodyIO, pgMESSAGE_PART_CONTENT_TYPE);
							if(sContentType == "text/plain" && !strPlainBody.empty() &&
								PR_SUCC(err = BodyIO->SeekWrite(&dwWritten, 0, (tPTR)strPlainBody.c_str(), strPlainBody.size())))
							{
								bPlainInserted = true;
							}
							else if(sContentType == "text/html" && !strHTMLBody.empty() &&
								PR_SUCC(err = BodyIO->SeekWrite(&dwWritten, 0, (tPTR)strHTMLBody.c_str(), strHTMLBody.size())))
							{
								bHtmlInserted = true;
							}
						}
					}
					// если хотя бы одно тело было, но вставить ничего не удалось,
					// не будем ничего возвращать
					if(!bPlainInserted && !bHtmlInserted)
						return "";
				}
			}
		}
	}

	// забираем из IO полученный MIME
	if((hIO)TempIO)
	{
		tQWORD size = 0;
		if(PR_SUCC(err = TempIO->GetSize(&size, IO_SIZE_TYPE_EXPLICIT)))
		{
			char *p = (char *)malloc((tDWORD)size + 1);
			if(p)
			{
				tDWORD read = 0, chunk_size = min(1024, (tDWORD)size);
				tQWORD offset = 0;
				while(PR_SUCC(TempIO->SeekRead(&read, offset, (tPTR)(p + offset), chunk_size)) && read)
					offset += read;
				strMIME += std::string(p, (size_t)size);
				free(p);
			}
		}
	}

	return strMIME;
}

// проверяет объект PID_MAILMSG
unsigned int BKF_CheckMAILMSG(MESSAGE_INFO *pMsg, CBKFBase &BKFBase)
{
	unsigned int spam_relevant_index = -1;

	PR_TRACE((0, prtIMPORTANT, "spamtst\tChecking MAILMSG-message..."));

	// получаем тела
	std::string strPlainBody = BKF_GetBodyString(pMsg, false);
	std::string strHTMLBody = BKF_GetBodyString(pMsg, true);

	std::string strMessageBody;

	// если есть интернет заголовки, пытаемся строить MIME
	const char *pInetHeaders = pMsg->GetPropStr(pgMESSAGE_INTERNET_HEADERS);
	if(pInetHeaders && *pInetHeaders)
		strMessageBody = BKF_BuildMIME(pInetHeaders, strPlainBody, strHTMLBody);
	// если нет заголовков или не удалось построить MIME, пробуем отдать библиотеке отдельно тему и тело,
	// иначе обрабатываем MIME
	if(strMessageBody.empty())
	{
		if(BKFBase.ProcessMessage(pMsg->GetPropStr(pgMESSAGE_SUBJECT),
					!strHTMLBody.empty() ? strHTMLBody.c_str() : strPlainBody.c_str(), &spam_relevant_index))
		{
			PR_TRACE((0, prtIMPORTANT, "spamtst\tMessage spam relevance index is %d", spam_relevant_index));
		}
	}
	else
	{
		PR_TRACE((0, prtIMPORTANT, "spamtst\tChecking MIME built from MAILMSG..."));
		if(BKFBase.ProcessMIMEMessage(strMessageBody.c_str(), &spam_relevant_index))
		{
			PR_TRACE((0, prtIMPORTANT, "spamtst\tMessage spam relevance index is %d", spam_relevant_index));
		}
	}

	return spam_relevant_index;
}

// проверяет объект PID_MSOE
unsigned int BKF_CheckMSOE(IN MESSAGE_INFO *pMsg, CBKFBase &BKFBase)
{
	unsigned int spam_relevant_index = -1;

	PR_TRACE((0, prtIMPORTANT, "spamtst\tChecking MIME-message..."));

	// получаем MIME-сообщение
	const char *pMessage = pMsg->GetRawMessage();
	// и обрабатываем его
	if(pMessage)
	{
		if(BKFBase.ProcessMIMEMessage(pMessage, &spam_relevant_index))
		{
			PR_TRACE((0, prtIMPORTANT, "spamtst\tMessage spam relevance index is %d", spam_relevant_index));
		}
		free((void *)pMessage);
	}
	else
	{
		PR_TRACE((0, prtERROR, "spamtst\tFailed to get message"));
	}

	return spam_relevant_index;
}
