#include "TestInterface.h"

static const int LINE_BUFFER_SIZE = 100 * 1024;

Ini_Log::Ini_Log(const bool enabled)
 : m_enabled(enabled),
   m_traceFileHandle(0)
{
}

Ini_Log::~Ini_Log()
{
	closeFile();
}

void Ini_Log::print(const char *format, ...)
{
	va_list ap;
	va_start(ap, format);
	reallyPrint(format, ap);
	va_end(ap);
}

bool Ini_Log::openFile(const std::string &fileName)
{
    if(!m_enabled)
        return true;

    if(fileName.empty())
    {
        std::cerr << "Failed to open trace file, because no file name is supplied" << std::endl;
        return false;
    }

    closeFile();

    // create folder for trace file
    const std::string traceFolder = LocalFile::getFolderFromPath(fileName);
    if(!traceFolder.empty() && !mkdirWrapper(traceFolder))
        std::cerr << "Failed to create folder for trace file '" << traceFolder << "'" << std::endl;

    m_traceFileHandle = fopen(fileName.c_str(), "an");
    if(!m_traceFileHandle)
    {
        std::cerr << "Failed to open trace file '" << fileName << "'" << std::endl;
        return false;
    }

    // file is opened for append
    fseek(m_traceFileHandle, 0, SEEK_END);
    
    ////////////////////
    time_t ltime;
    time(&ltime);
    char tmstr[200];
    sprintf(tmstr, "%s", (ltime != -1) ? ctime(&ltime) : "");
    tmstr[strlen(tmstr) - 1] = 0; // erase 0d0a from the end
    print("=========================================================");
    print("=====  Trace log started. %24s  =====", tmstr);
    print("=========================================================");

    return true;
}

void Ini_Log::closeFile()
{
    if(!m_traceFileHandle || !m_enabled)
        return;

    time_t ltime;
    time(&ltime);
    char tmstr[200];
    sprintf(tmstr, "%s", (ltime != -1) ? ctime(&ltime) : "");
    tmstr[strlen(tmstr) - 1] = 0; // erase 0d0a from the end
    print("=========================================================");
    print("=====  Trace log finished. %24s =====", tmstr);
    print("=========================================================");

    fclose(m_traceFileHandle);
    m_traceFileHandle = 0;
}

void Ini_Log::reallyPrint(const char *format, va_list ap) 
{
    if(!m_enabled || !m_traceFileHandle) 
        return;

	int patt_len = strlen(format) + LINE_BUFFER_SIZE + 1;
	int line_len = strlen(format) + 2 * LINE_BUFFER_SIZE + 1;
	char *pattern = new char[patt_len];
	if(!pattern)
        return;
	char *line = (char *)new char[line_len];
	if(!line)
    {
		delete pattern;
        pattern = 0;
		return;
	}
	memset(pattern, 0, patt_len--);
	memset(line, 0, line_len--);


    time_t _tm = time(0);
    struct tm *_tm_p = localtime(&_tm);
    snprintf(pattern, patt_len, "%.2d:%.2d:%.2d:\t%s", 
        _tm_p ? _tm_p->tm_hour : 0, _tm_p ? _tm_p->tm_min : 0, _tm_p ? _tm_p->tm_sec : 0, format);

	if(pattern[strlen(pattern) - 1] != '\n')
		strcat(pattern, "\n");

	vsnprintf(line, line_len, pattern, ap);

    fwrite(line, strlen(line), 1, m_traceFileHandle);
    fflush(m_traceFileHandle);
	delete[] pattern;
	delete[] line;
}


Ini_Journal::Ini_Journal(Log *log)
 : pLog(log)
{
}

void Ini_Journal::publishMessage(const KLUPD::CoreError &code, const STRING &parameter)
{
    if(!pLog)
        return;

    if(parameter.empty())
        printf("\n%s", KLUPD::toString(code).c_str());
    else
        printf("\n%s '%s'", KLUPD::toString(code).c_str(), parameter.to_string().c_str());
}

Command IniCallbacks::checkCallback(const Request &request)
{
    // First show the request
    int req_type = -1;
    for(unsigned _i = 0; _i < sizeof(requests)/sizeof(requests[0]); ++_i)
    {
        if(request == requests[_i].req_id)
        {
            req_type = _i;
            break;
        }
    }
    
    switch(request)
    {
    case REQ_SELECT_UPDATE_OR_CONFIGURE:
        // choice to continue, or cancel
        if(REPLY_EMPTY == requesterWrapper( req_type ) )
            return CMD_CONTINUE_UPDATE;
        return CMD_CANCEL_UPDATE;
        
    // ############ async requests follows ############
        
    case REQ_NET_CONTINUE_DOWNLOAD:
        switch(requesterWrapper(req_type))
        {
        case REPLY_CANCEL:
            return CMD_CANCEL;
        case REPLY_EMPTY:
        default:
            return CMD_EMPTY;
        }
        return CMD_EMPTY;
                        
    case REQ_CHECK_BASES:
        switch(requesterWrapper(req_type))
        {
        case REPLY_EMPTY:
            return CMD_EMPTY;
        
        case REPLY_CANCEL:
        default:
            return CMD_CANCEL;
        }
        break;
    
    case REQ_NO_PRIMARY_INDEX:
    case REQ_NO_SECONDARY_INDEX:
    case REQ_PROXY_LOGIN:
        switch(requesterWrapper(req_type))
        {
        case REPLY_EMPTY:
            return CMD_EMPTY;
        
        case REPLY_CANCEL:
        default:
            return CMD_CANCEL;
        }
        break;
    
    case REQ_PROXY_LOGIN_CONFIRMED:
        return CMD_EMPTY;
        break;
    
    default:
        fprintf(stderr, "\nInvalid request specified (%d), ignoring it\n", (int)request);
        break;
    }

    return CMD_EMPTY;
}

void IniCallbacks::percentCallback(const Message *msg)
{
    printf(".");
}

Reply IniCallbacks::requesterWrapper(const int requestType)
{
    // special case - do not disturb user with these async questions...
    if(REQ_NET_CONTINUE_DOWNLOAD == requests[requestType].req_id)
        return REPLY_EMPTY;
    
    return requests[requestType].def_reply;
}

