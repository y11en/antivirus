#ifndef _KL_LOG_H_
#define _KL_LOG_H_

#include "kl_object.h"
#include "kldef.h"
#include "klstatus.h"
#include "g_thread.h"
#include "kl_list.h"

class CKl_LogItem : public CKl_Object
{
public:
	QUEUE_ENTRY;    
	virtual KLSTATUS	Process() = 0;
    
    CKl_LogItem() {};
	virtual ~CKl_LogItem(){};
};


/*
 *	Класс логгирования событий. 
 *  Создается отдельный поток и ведется список событий CKl_LogItem
 *  Когда добавляется событие, то взводится евент m_LogEvent.
 *  Поток выгребает события по-одному и выполняет функцию SaveData для события.
 *  Когда событий нет, то поток ждет на WaitForSingleObject
 *  Когда выгружаемся, то ждем когда выйдем из потока и освобождаем ресурсы.

 
 */
class CKl_Log   : public CKl_Object
{
public:    
    CKl_Log(void* LogMethod);
    virtual ~CKl_Log();

	bool        m_Continue;
    void*		m_LogMethod;	// Контекст, с помощью которого происходит обработка Item.
    CKl_Thread* m_LogThread;	// Поток, который обрабатывает сообщения
    KL_EVENT    m_LogEvent;		// Евент, говоряший, что есть события
	KL_EVENT	m_StopEvent;
	
    CKl_List<CKl_LogItem>* m_LogList;		// Список событий
        
    KLSTATUS    OnThreadStart	( PVOID Context		);
    KLSTATUS    AddItem			( CKl_LogItem* Item	);
    
    static void LogRoutine		( PVOID    Context	);
};







#endif