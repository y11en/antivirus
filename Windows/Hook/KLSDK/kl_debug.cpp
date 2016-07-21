#include "kldef.h"
#include "kl_debug.h"
#include "g_thread.h"
#include "kl_file.h"

CKl_DebugInfo*  Dbg;




KLSTATUS
DbgMessage(char* message, ...)
{
    char            buf[512];
    unsigned long   outLen;
    va_list         argptr;
    
    va_start( argptr, message );
    outLen = _vsnprintf( buf, sizeof(buf), message, argptr );
    
    va_end(argptr );
	buf[511] = 0;
    
    Dbg->Print( BIT_PLUGINS, LEVEL_NOTIFY, buf );

    return KL_SUCCESS;
}

CKl_LogMessage::CKl_LogMessage(char* message, size_t Size /* = 0 */)
{    
    m_size = Size ? Size : strlen ( message );
    
    m_message = (char*)KL_MEM_ALLOC ( m_size + 1 );

    if ( m_message )
    {
        RtlCopyMemory ( m_message, message, m_size );
        
        m_message[m_size] = '\0';
    }    
}

CKl_LogMessage::~CKl_LogMessage()
{
    if ( m_message )
        KL_MEM_FREE ( m_message );

    m_message = NULL;
}

//--------------------------------------------------------------------
CKl_DebugInfo::CKl_DebugInfo(char* DrvTag, unsigned long Level /* = 0 */, unsigned long Mask /* = 0 */)
{
    m_ID   = 0;
    RtlZeroMemory ( m_DrvTag, sizeof (m_DrvTag) );
    strcpy (m_DrvTag, DrvTag);
    SetDebugLevel(Level);
    SetDebugMask(Mask);
    

#ifdef _DEBUG
    m_Disabled = false;
#else
    m_Disabled = true;
#endif
}

CKl_DebugInfo::~CKl_DebugInfo()
{
}

void CKl_DebugInfo::SetDebugLevel(unsigned long Level)
{
    m_nLevel = Level;
}

void CKl_DebugInfo::SetDebugMask(unsigned long Mask)
{
    m_nMask = Mask;
}

unsigned long CKl_DebugInfo::DebugLevel()
{
    return m_nLevel;
}

unsigned long CKl_DebugInfo::DebugMask()
{
    return m_nMask;
}

bool CKl_DebugInfo::isMatched(unsigned long Level, unsigned long Mask)
{
    if ( !m_Disabled && 
          Level <= m_nLevel && (Mask & m_nMask) )
        return true;
    
    return false;
}

void
CKl_DebugInfo::Enable()
{
    m_Disabled = false;
}

void
CKl_DebugInfo::Disable()
{
    m_Disabled = true;
}

void
CKl_DebugInfo::Print(unsigned long Mask, unsigned long Level, char* message, ...)
{
    return;
}

void
CKl_DebugInfo::Stop(char* message, ...)
{
    return;
}

/*  ------------------------------------------------------
 *	Класс для вывода отладочной информации в виде trace-ов
 */
CKl_TraceDebugInfo::CKl_TraceDebugInfo(char* DrvTag) : CKl_DebugInfo(DrvTag)
{
    m_ID = DISPATCHER_ID_TRACE;
}

CKl_TraceDebugInfo::CKl_TraceDebugInfo(char* DrvTag, unsigned long Level, unsigned long Mask) : CKl_DebugInfo(DrvTag, Level, Mask)
{
    m_ID = DISPATCHER_ID_TRACE;
}

CKl_TraceDebugInfo::~CKl_TraceDebugInfo()
{

}

#ifdef _DEBUG
inline void CKl_TraceDebugInfo::Print(unsigned long Mask, unsigned long Level, char* message, ...)
{
    char buf[1024];
    unsigned long outLen;
    size_t PrefLen;
    va_list argptr;

    if (!isMatched(Level, Mask))
        return;
    
    strcpy(buf, m_DrvTag);
    PrefLen = strlen(buf);
    va_start( argptr, message );
    outLen = _vsnprintf( buf+PrefLen, sizeof(buf)-PrefLen, message, argptr );

    va_end(argptr );
    
#ifndef ISWIN9X
    DbgPrint(buf);
#else
    nprintf(buf);
#endif // ISWIN9X
}
#else // _DEBUG
inline void CKl_TraceDebugInfo::Print(unsigned long Mask, unsigned long Level, char* message, ...)
{
}
#endif

#ifdef _DEBUG
inline void CKl_TraceDebugInfo::Stop(char* message, ...)
{
    char buf[1024];
    unsigned long outLen;
    size_t PrefLen;
    va_list argptr;

    strcpy(buf, m_DrvTag);
    PrefLen = strlen(buf);
    va_start( argptr, message );
    outLen = _vsnprintf( buf+PrefLen, sizeof(buf)-PrefLen, message, argptr );

    va_end(argptr );
    
#ifndef ISWIN9X
    DbgPrint(buf);
#else
    nprintf(buf);
#endif // ISWIN9X
    DbgBreakPoint();
}
#else // _DEBUG
inline void CKl_TraceDebugInfo::Stop(char* message, ...)
{
}
#endif

/*  ------------------------------------------------------------
 *	Класс для вывода отладочной информации в виде файла с логами
 */
CKl_LogDebugInfo::CKl_LogDebugInfo(char* DrvTag) : CKl_DebugInfo(DrvTag)
{    
    // здесь нужно стартовать поток, который будет периодически сбрасывать логи в файл.
    m_ID = DISPATCHER_ID_LOG;
}

CKl_LogDebugInfo::CKl_LogDebugInfo(char* DrvTag, unsigned long Level, unsigned long Mask) 
                            : CKl_DebugInfo(DrvTag, Level, Mask)
{
    m_ID = DISPATCHER_ID_LOG;
    m_LogList = new CKl_List<CKl_LogMessage>();
    KlInitializeEvent(&m_LogListEvent, SynchronizationEvent , FALSE);
    KlClearEvent(&m_LogListEvent);

    if ( m_OutputFile = (char*)KL_MEM_ALLOC( 512 ) )    
        SetOutputFile( "C:\\Log.txt");

    CKl_Thread*  MyThread = new CKl_Thread();
    if ( MyThread )
        MyThread->Start(LogFlushRoutine, this);
}

CKl_LogDebugInfo::~CKl_LogDebugInfo()
{
    delete m_LogList;
    KL_MEM_FREE ( m_OutputFile );
}

void
CKl_LogDebugInfo::Print(unsigned long Mask, unsigned long Level, char* message, ...)
{
    /*
     *	добавить строку в лист для вывода
     */
    char buf[512];
    unsigned long outLen;
    size_t PrefLen;
    va_list argptr;
    
    if (!isMatched(Level, Mask))
        return;
    
    strcpy(buf, m_DrvTag);
    PrefLen = strlen(buf);
    va_start( argptr, message );
    outLen = _vsnprintf( buf + PrefLen, sizeof(buf)-PrefLen, message, argptr );
    
    va_end(argptr );

#ifndef ISWIN9X
    DbgPrint(buf);
#else
    nprintf(buf);
#endif // ISWIN9X


    CKl_LogMessage* LogMessage = new CKl_LogMessage( buf );

    if ( LogMessage && LogMessage->m_message )
    {
        m_LogList->InsertTail( LogMessage );
        KlSetEvent( &m_LogListEvent );
    }
}


void
CKl_LogDebugInfo::Stop(char* message, ...)
{
#ifdef _DEBUG
    DbgBreakPoint();
#endif
}

void
CKl_LogDebugInfo::SetOutputFile(char* FileName)
{
    RtlZeroMemory ( m_OutputFile, 512 );

    strcpy ((char*)m_OutputFile, (const char*)FileName );
}

void
CKl_LogDebugInfo::LogFlushRoutine(void* Context)
{
    CKl_LogDebugInfo*   Out = (CKl_LogDebugInfo*)Context;
    CKl_LogMessage*     LogMessage = NULL;
    CKl_File            LogFile( Out->m_OutputFile );

    while (TRUE)
    {
        KlWaitEvent ( &(Out->m_LogListEvent) );
        KlClearEvent( &(Out->m_LogListEvent) );

        while ( LogMessage = Out->m_LogList->InterlockedRemoveHead() ) 
        {
#ifdef ISWIN9X
            if ( !K32Initialized )
            {
                KlSleep(5);
                Out->m_LogList->InsertHead(LogMessage);
                continue;
            }
#endif
            if ( KL_SUCCESS == LogFile.Open() )
            {
                if ( KL_SUCCESS == LogFile.Write( LogMessage->m_message, LogFile.GetSize(), (ULONG)LogMessage->m_size, NULL) )
                {
                    delete LogMessage;
                }
                else
                {
                    Out->m_LogList->InsertHead(LogMessage);
                }
                
                LogFile.Close();
            }
            else
            {
                Out->m_LogList->InsertHead(LogMessage);
                break;
            }
        }
    }
}

/*

CKl_PerfCheck::CKl_PerfCheck(char* CheckName) 
		: m_Name( CheckName ), m_Sent(false)
{
}

CKl_PerfCheck::~CKl_PerfCheck()
{
	Send();
}

void
CKl_PerfCheck::Start()
{
	KeQueryPerformanceCounter(&m_StartTime);
}

void
CKl_PerfCheck::Stop()
{
	KeQueryPerformanceCounter(&m_StopTime);
}

VOID
CKl_PerfCheck::Send()
{
	if ( !m_Sent )
	{
		LARGE_INTEGER Delta;
		Delta.QuadPart = ( m_StopTime.QuadPart - m_StartTime.QuadPart );
		
		Dbg->Print(BIT_SHOW_INFO, LEVEL_INTERESTING, "%s was %d ticks\n", m_Name, Delta);

		m_Sent = true;
	}	
}

void
CKl_PerfCheck::Clear()
{	
	m_Sent = false;	
}
*/