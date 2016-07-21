#ifndef _UTILS_H_
#define _UTILS_H_

#include <Prague/prague.h>
#include <Prague/pr_cpp.h>
#include <Prague/iface/i_io.h>

#include <Mail/structs/s_mailwasher.h>

#include <map>
#pragma warning(disable:4786)
#include "lookahead.h"

tBOOL NeedMoreData(tCHAR* data, tINT size);
tBOOL NeedMoreMultyLineData(tCHAR* data, tINT size);

typedef std::map<tDWORD, cPOP3Message> MessageList_t;

tERROR ParseLIST(MessageList_t* pMessageList, hIO io, cPOP3Session* pPOP3Session, OUT tDWORD* pdwMessageCount, OUT tDWORD* pdwMailboxSize);
//tERROR ParseUIDL(MessageList_t* pMessageList, hIO io);
//tERROR ParseTOP(MessageList_t* pMessageList, tDWORD dwCounter, hIO io);

tBOOL FindPos(lookahead_stream& data, tQWORD begin, tQWORD& end, tCHAR* delimeter);
tERROR GetIntLineParameter(tINT number, lookahead_stream& data, tQWORD begin, tQWORD end, tDWORD& dwParameter);

//////////////////////////////////////////////////////////////////////////
//
//	Ѕезопасна€ посылка пражского сообщени€
//	Ќужна дл€ редкого случа€ быстрого разрыва коннекта, когда контексты протоколлеров уже могут быть уничтожены
//	Ёто вызвано non-thread-safe удалением контекстов {pmSESSION_STOP: cAutoCS _cs_(m_hSessionCS, false)}, 
//	сделанным дл€ ускорени€ работы. 
//	≈стественна€ накладна€ ошибка дл€ многопоточных протоколлеров - таких как MailWasher.
//	Ћюба€ посылка данных в сессию из дочернего потока должна осуществл€тьс€ через функцию, аналогичную SAFE_sysSendMsg
inline tERROR SAFE_sysSendMsg(hOBJECT _this, tDWORD msg_cls, tDWORD msg_id, tPTR ctx, tPTR pbuff, tDWORD* pblen)
{
	__try	{ return _this->sysSendMsg ( msg_cls, msg_id, ctx, pbuff, pblen ); }
	__except(EXCEPTION_EXECUTE_HANDLER) { return E_FAIL; }
}

#endif//_UTILS_H_