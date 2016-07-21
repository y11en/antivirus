// IDSTest.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "../../../klsdk/klstatus.h"
#include "../../../klsdk/PlugAPI.h"
#include "../../include/logger_api.h"
#include "../../../../../include/CKAH/ckahum.h"
#include "../../../../../include/CKAH/ckahids.h"
#include "../../../Hook/Klick_api.h"
#include "../kldump/logformat.h"
#include "Unicodeconv.h"

#pragma warning(disable : 4786)
#include <vector>
#include <string>

static std::string g_szBasesPath;

typedef	std::vector<std::string> FILENAME_LIST; 
static FILENAME_LIST g_Files;

class logfile
{
    FILE * m_file;
    int count;
public:

    logfile() : m_file(0), count(0)
    {
    }

    ~logfile() 
    { 
        if (m_file) fclose(m_file);
    }

    void open(const char *name) 
    { 
        m_file = fopen(name, "wt"); 
    }

    void printf(const char *format...)
    {
        if (m_file)
        {
            va_list list;
            va_start(list, format);
            vfprintf(m_file, format, list);
            va_end(list);

            fflush(m_file);
        }
    }
};

static bool g_bRunning = true;
static std::string g_szCurrentFile;
static bool g_bLog = false;
static std::string g_szLog;
static logfile g_log;
static bool g_bCurrentFileVirusDetect = false;
static HANDLE g_hAttackSemaphore = CreateSemaphore (NULL, 0, 1, NULL);
static bool g_bRecursively = false;
static bool g_bHeuristics = false;
static bool g_bReload = true;
static bool g_bExitAfterReload = false;
static bool g_bSilent = false;
static bool g_bVerbose = false;
static int  g_nVer = 0;

static BOOL CtrlHandler( DWORD fdwCtrlType ) 
{ 
	switch( fdwCtrlType ) 
	{ 
    case CTRL_C_EVENT: 
    case CTRL_CLOSE_EVENT: 
    case CTRL_BREAK_EVENT: 
    case CTRL_LOGOFF_EVENT: 
    case CTRL_SHUTDOWN_EVENT:
		g_bRunning = false;	
		return TRUE; 
		
    default: 
		return FALSE; 
	} 
} 

static bool IsWin9x ()
{
    OSVERSIONINFO osvi;
    osvi.dwOSVersionInfoSize = sizeof ( osvi );
    GetVersionEx ( &osvi );
    
    return ( osvi.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS );
}

static HANDLE FindPluginDriver()
{
	const TCHAR szName [] = _T("\\\\.\\klick");
	HANDLE hPlugin = INVALID_HANDLE_VALUE;
	
	if (IsWin9x())
		hPlugin = CreateFile (szName, 0, 0, NULL, 0, 0, NULL);
	else
		hPlugin = CreateFile (szName, 0, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	
	return hPlugin;
}

const char * format_time (char *timebuf, struct tm *tm)
{
    sprintf (timebuf, "%04u-%02u-%02u %02u:%02u:%02u",
		tm->tm_year + 1900, tm->tm_mon + 1, tm->tm_mday,
		tm->tm_hour, tm->tm_min, tm->tm_sec);
	return timebuf;
}

const char * format_time (char *timebuf, time_t t)
{
	return format_time (timebuf, localtime (&t));
}

const char * format_current_time (char *timebuf)
{
	return format_time (timebuf, time (0));
}

static void OutLog(const char *filename, const char *verdict, const char *verdictname)
{
	char timebuf[60];
	format_current_time (timebuf);

    if (!g_bSilent)
    {
        if (verdictname)
	        printf("%s\t%s\t%s\t%s\n", timebuf, filename, verdict, verdictname);
        else
	        printf("%s\t%s\t%s\n", timebuf, filename, verdict);
    }

    if (g_bLog)
    {
        if (verdictname)
	        g_log.printf("%s\t%s\t%s\t%s\n", timebuf, filename, verdict, verdictname);
        else
	        g_log.printf("%s\t%s\t%s\n", timebuf, filename, verdict);
    }
}

static void OutLog(const char *comment)
{
    if (g_bLog)
    {
        g_log.printf("%s\n", comment);
    }
}


static void CALLBACK OnAttackNotify (LPVOID lpContext, const CKAHIDS::AttackNotify *pAttackNotify)
{
	if (pAttackNotify->AttackID == 0x0000000B)
	{
		//stop event
		LONG uCount;
		if (!ReleaseSemaphore (g_hAttackSemaphore, 1, &uCount) || uCount!=0)
		{
			fprintf(stderr, "Error: Count semaphore mismatch.\n");
		}
	}
	else
	{
        OutLog(g_szCurrentFile.c_str (), "Detected", __LPCTSTR(pAttackNotify->AttackDescription));

		g_bCurrentFileVirusDetect = true;
	}
}

static void ShowUsage()
{
    printf("Usage: idstest [options] -b <bases> -f <file(s)> [-f <file(s)>...] [-l <log>]\n"
           "options: -n - do not reload bases\n"
           "         -x - exit after base reload\n"
           "         -r - scan directories recursively\n"
           "         -h - use heuristic records\n"
           "         -s - silent mode (no output to stdout)\n"
           "         -v - verbose mode\n"
           "         -5 -6 -7 -8 - product version\n" 
          );
}

static void ShowError()
{
}


static bool parse_commandline(char* cmdstart)
{
	std::vector<std::string> argv;
	std::string cmd = cmdstart;
	std::string arg;
	bool bDBLQ = false;

	for (int offset = 0; cmdstart[offset]!='\0'; ++offset)
	{
		char sym = cmdstart[offset];

		if (!bDBLQ && (sym == ' ' || sym == '\t'))
		{
			if (!arg.empty ())
				argv.push_back (arg);

			arg = "";

			continue;
		}
		else
		{
			if (sym == '\"')
			{
				if (bDBLQ)
				{
					if (!arg.empty ())
						argv.push_back (arg);
					
					arg = "";
					bDBLQ = false;
				}
				else
				{
					bDBLQ = true;
				}
			}
			else
				arg+=sym;
		}
	}

	if (!arg.empty ())
		argv.push_back (arg);

	
	int argc = argv.size (); 
	int i = 1;
    if (argc < 2)
    {
        ShowUsage();
        return false;
    }

	while ( i<argc )
	{
		if (argv[i][0] == '/' || argv[i][0] == '-')
		{
			if (argv[i].size () != 2)
			{
				ShowError ();
				return false;
			}
			
			switch(argv[i][1])
			{
            case '?':
                ShowUsage();
                return false;

            case '5': g_nVer = 5; i++; break;
            case '6': g_nVer = 6; i++; break;
            case '7': g_nVer = 7; i++; break;
            case '8': g_nVer = 8; i++; break;

			case 'n':
                g_bReload = false;
                i++;
                break;

			case 'x':
                g_bExitAfterReload = true;
                i++;
                break;

			case 'r':
                g_bRecursively = true;
                i++;
                break;

			case 'h':
                g_bHeuristics = true;
                i++;
                break;

            case 's':
                g_bSilent = true;
                i++;
                break;

            case 'v':
                g_bVerbose = true;
                i++;
                break;

			case 'b':
				{
					int ckah_path_len = 0;
					if (i+1<argc && (ckah_path_len = argv[i+1].size ()))
					{
						g_szBasesPath = argv[i+1] + (argv[i+1][ckah_path_len-1]=='\\'?"ckah.set":"\\ckah.set");
						i+=2;
					}
					else
					{
						ShowError ();
						return false;
					}
				}
				break;
				
			case 'f':
				if (i+1<argc && argv[i+1].size ())
				{
					g_Files.push_back(argv[i+1]);
					i+=2;
				}
				else
				{
					ShowError ();
					return false;
				}
				break;

            case 'l':
				if (i+1<argc && argv[i+1].size ())
				{
                    g_bLog = true;
					g_szLog = argv[i+1];
					i+=2;
				}
				else
				{
					ShowError ();
					return false;
				}
				break;

                
            default:
                ShowUsage ();
                return false;
			}
		}
		else
		{
			ShowError ();
			return false;
		}
	}
	
	return true;
}

void __stdcall LogCKAHUM(CKAHUM::LogLevel loglevel, const char *str )
{
    if (!g_bSilent && g_bVerbose)
    {
        printf("[CKAHUM] %s\n", str);
    }
}

static bool InitCKAHUM(int ver)
{
    const wchar_t *reg;
    switch(ver)
    {
    case 5:  reg = L"SOFTWARE\\KasperskyLab\\Components\\10a"; break;
    case 6:  reg = L"SOFTWARE\\KasperskyLab\\AVP6\\CKAHUM"; break;
    default:
    case 7:  reg = L"SOFTWARE\\KasperskyLab\\protected\\AVP7\\CKAHUM"; break;
    case 8:  reg = L"SOFTWARE\\KasperskyLab\\protected\\AVP8\\CKAHUM"; break;
    }
	CKAHUM::OpResult result = CKAHUM::Initialize ( HKEY_LOCAL_MACHINE, reg, LogCKAHUM );
	
	if (result != CKAHUM::srOK)
	{
		fprintf(stderr, "Error: CKAHUN initialization failed (%d)\n", result);
		return false;
    }
    return true;
}

static bool ReloadCKAHUM(bool& needreboot)
{
	CKAHUM::OpResult result = CKAHUM::Reload (__LPCWSTR(g_szBasesPath));
	if (result == CKAHUM::srNeedReboot)
	{
        needreboot = true;
		return true;
	}
	
    needreboot = false;

	if (result != CKAHUM::srOK)
	{
        fprintf(stderr, "Error: IDS Bases reload failed (%d)\n", result);
		return false;
	}

    return true;
}

static bool StartCKAHUM()
{
	CKAHUM::OpResult result = CKAHIDS::Start (OnAttackNotify, NULL);

	if (result != CKAHUM::srOK)
	{
		fprintf(stderr, "Error: IDS start failed (%d)\n", result);
		return false;
	}

	result = CKAHIDS::SetIDSAttackNotifyParam(0, FLAG_ATTACK_NOTIFY, 0);
	if (result != CKAHUM::srOK)
	{
		fprintf(stderr, "Error: SetIDSAttackNotifyParam failed (%d)\n", result);
		return false;
	}

	result = CKAHIDS::SetIDSAttackNotifyPeriod(0);
	if (result != CKAHUM::srOK)
	{
		fprintf(stderr, "Error: SetIDSAttackNotifyPeriod failed (%d)\n", result);
		return false;
	}

    if (!g_bHeuristics)
    {
        #define VERDICT_TCPPORTSCAN		0x00000001
        #define VERDICT_UDPPORTSCAN		0x00000002
        #define VERDICT_ICMPFLOOD		0x00000003
        #define VERDICT_TCPSYNFLOOD		0x00000004

        CKAHIDS::SetIDSParam ( VERDICT_TCPPORTSCAN, 2, 0 ); // expire time = 0;
        CKAHIDS::SetIDSParam ( VERDICT_UDPPORTSCAN, 2, 0 ); // expire time = 0;
        CKAHIDS::SetIDSParam ( VERDICT_ICMPFLOOD,   0, 0 ); // expire time = 0;
        CKAHIDS::SetIDSParam ( VERDICT_TCPSYNFLOOD, 0, 0 ); // expire time = 0;
    }
	
	result = CKAHIDS::Resume ();
	if (result != CKAHUM::srOK)
	{
		fprintf(stderr, "Error: IDS resume failed (%d)\n", result);
		return false;
	}

	return true;
}

static bool DeinitCKAHUM()
{
	CKAHUM::OpResult result = CKAHIDS::Stop ();
	if (result != CKAHUM::srOK)
	{
		fprintf(stderr, "Error: IDS stop failed (%d)\n", result);
		return false;
	}
	
	result = CKAHUM::Deinitialize ( LogCKAHUM );
	if (result != CKAHUM::srOK)
	{
		fprintf(stderr, "Error: CKAHUM deinitialization failed (%d)\n", result);
		return false;
	}

	return true;
}

static bool ProcessFile(HANDLE hDriver, HANDLE hStorage)
{
	LOGGER_PACKET Packet;
	ULONG uOutBuffSize = 0;

	while(g_bRunning)
	{
		
		DWORD dwReaded;
		if ( !ReadFile(hStorage, &Packet, 2 * sizeof(ULONG), &dwReaded, NULL) || dwReaded != 2 * sizeof(ULONG) )
		{
			//EOF
			if (dwReaded == 0)
				break;

            OutLog(g_szCurrentFile.c_str (), "read error", NULL);
			return false;
		}
		
		if (Packet.PacketSize<=0 || Packet.PacketSize>1600)
		{
            OutLog(g_szCurrentFile.c_str (), "corrupted", NULL);
			return false;
		}
		
		if ( !ReadFile(hStorage, Packet.PacketBuffer, Packet.PacketSize, &dwReaded, NULL) || dwReaded != Packet.PacketSize )
		{
			//EOF должны получить при чтении заголовка
            OutLog(g_szCurrentFile.c_str (), "read error", NULL);
			return false;
		}
		
		ULONG uRet;
		if (!DeviceIoControl(hDriver, KLICK_PROCESS_PACKET, (IDS_PACKET*)&Packet, Packet.PacketSize + 2 * sizeof(ULONG), &uRet, sizeof (uRet), &uOutBuffSize, NULL))
		{
			fprintf(stderr, "Error: KLICK_PROCESS_PACKET failed.\n");
			return false;
		}

	}

	return true;
}

static bool ClearStreams(HANDLE hDriver)
{
	ULONG uOutBuffSize = 0;
    ULONG uRet = 0;
	if (!DeviceIoControl(hDriver, KLICK_RESET_STREAMS, NULL, 0, &uRet, sizeof (uRet), &uOutBuffSize, NULL))
	{
		//fprintf(stderr, "Error: KLICK_RESET_STREAMS failed.\n");
		return false;
	}

    return true;
}

static bool SendStopPacket(HANDLE hDriver)
{
#if 0
	static unsigned char buff[] = {	0x01, 0x00, 0x00, 0x00, /*packet size*/0x4A, 0x00, 0x00, 0x00/*end packet size*/, 
									0x00, 0x80, 0x48, 0xB3, 0x54, 0x3F, 0x00, 0x12, 
									0x7F, 0xB5, 0x2E, 0xC0, 0x08, 0x00, 0x45, 0x78, 
									0x00, 0x3C, 0xA2, 0x98, 0x00, 0x00, 0x39, 0x01, 
									0x6E, 0x21, 0xC2, 0x57, 0x00, 0x32, 0xAC, 0x10, 
									0x01, 0xF6, 0x00, 0x00, 0x2D, 0x5C, 0x04, 0x00, 
									0x24, 0x00, /*sig*/0x1B, 0x03, 0x7D, 0x35, 0xDB, 0x15, 
									0x41, 0xE7, 0xB0, 0x81, 0x61, 0x84, 0x8C, 0x33, 
									0xF1, 0xC1, 0x3F, 0x57, 0xD1, 0x7B,	0xC7, 0x94, 
									0x45, 0x20, 0xBC, 0x2E, 0x65, 0x7A, 0xBC, 0xFF, 
									0xEF, 0xE4 /*sig end*/ };
#endif
    // xored by 0xA5
	static unsigned char buff[] = {	0xA4, 0xA5, 0xA5, 0xA5, /*packet size*/0xEF, 0xA5, 0xA5, 0xA5/*end packet size*/, 
                                    0xA5, 0x25, 0xED, 0x16, 0xF1, 0x9A, 0xA5, 0xB7, 
                                    0xDA, 0x10, 0x8B, 0x65, 0xAD, 0xA5, 0xE0, 0xDD, 
                                    0xA5, 0x99, 0x07, 0x3D, 0xA5, 0xA5, 0x9C, 0xA4, 
                                    0xCB, 0x84, 0x67, 0xF2, 0xA5, 0x97, 0x09, 0xB5, 
                                    0xA4, 0x53, 0xA5, 0xA5, 0x88, 0xF9, 0xA1, 0xA5, 
                                    0x81, 0xA5, /*sig*/0xBE, 0xA6, 0xD8, 0x90, 0x7E, 0xB0, 
                                    0xE4, 0x42, 0x15, 0x24, 0xC4, 0x21, 0x29, 0x96, 
                                    0x54, 0x64, 0x9A, 0xF2, 0x74, 0xDE, 0x62, 0x31, 
                                    0xE0, 0x85, 0x19, 0x8B, 0xC0, 0xDF, 0x19, 0x5A, 
                                    0x4A, 0x41 /*sig end*/ }; 

    unsigned char tempbuff[sizeof(buff)];

    for (int i = 0; i < sizeof(buff); ++i)
        tempbuff[i] = buff[i] ^ 0xA5;

	ULONG uOutBuffSize = 0;
	ULONG uRet;
	if (!DeviceIoControl(hDriver, KLICK_PROCESS_PACKET, (IDS_PACKET*)tempbuff, sizeof(buff), &uRet, sizeof (uRet), &uOutBuffSize, NULL))
	{
		fprintf(stderr, "Error: KLICK_PROCESS_PACKET send stop packet failed.\n");
		return false;
	}

	return true;
}

int ScanFile(HANDLE hDriver, const TCHAR *file)
{
	HANDLE hStorage = CreateFile(file, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hStorage == INVALID_HANDLE_VALUE)
	{
        OutLog(file, "open error", NULL);
		return 0;
	}

    KLDUMP_LOG_HEADER Header;
    DWORD dwReaded;
	if ( !ReadFile(hStorage, &Header, sizeof(Header), &dwReaded, NULL) || dwReaded != sizeof(Header) )
	{
        CloseHandle(hStorage);
		return 0;
	}

    if (Header.sig != KLDUMP_LOG_SIG)
    {
        CloseHandle(hStorage);
        return 0;
    }

	g_szCurrentFile = file;

    ClearStreams(hDriver);

	if (!ProcessFile(hDriver, hStorage))
		return -1;

	
	if (!SendStopPacket(hDriver))
		return -1;

	if (WaitForSingleObject(g_hAttackSemaphore, 2*60*1000) != WAIT_OBJECT_0)
	{
        OutLog(g_szCurrentFile.c_str (), "stop signature timed out", NULL);
		return -1;
	}
	
	if ( !g_bCurrentFileVirusDetect )
        OutLog(g_szCurrentFile.c_str (), "Ok", NULL);
	
	g_bCurrentFileVirusDetect = false;
	g_szCurrentFile = "";

	CloseHandle (hStorage);
	hStorage = INVALID_HANDLE_VALUE;
    return 0;
}

int ScanDir(HANDLE hDriver, const TCHAR *drive, const TCHAR *dir, const TCHAR *mask, const TCHAR *maskext, bool recursively)
{
    TCHAR szPath[_MAX_PATH];
    _tmakepath ( szPath, drive, dir, mask, maskext );

	WIN32_FIND_DATA	 find_data;

	HANDLE hSearch;

    // scan for directories
    if (recursively)
    {
        hSearch= FindFirstFile( szPath, &find_data );
	    
	    if( hSearch != INVALID_HANDLE_VALUE ) 
	    {
		    BOOL	found = TRUE;
		    
		    do 
            {
			    if( _tcscmp( find_data.cFileName, _T(".") ) != 0 && _tcscmp( find_data.cFileName, _T("..") ) != 0 ) 
			    { 
				    if( find_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
                    {
                        std::string newdir = dir;
                        newdir += find_data.cFileName;
                        newdir += '\\';

                        if ( ScanDir ( hDriver, drive, newdir.c_str(), mask, maskext, recursively ) !=0 )
                            return -1;
                    }
			    }
			    
			    found = FindNextFile( hSearch, & find_data );
		    } 
            while( found && g_bRunning );
		    
		    FindClose( hSearch );
	    }
    }

    // scan for files
    {
	    hSearch = FindFirstFile( szPath, &find_data );
	    
	    if( hSearch != INVALID_HANDLE_VALUE ) 
	    {
		    BOOL	found = TRUE;
		    
		    do 
            {
			    if( _tcscmp( find_data.cFileName, _T(".") ) != 0 && _tcscmp( find_data.cFileName, _T("..") ) != 0 ) 
			    { 
				    if( !(find_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) ) 
				    {
                        _tmakepath ( szPath, drive, dir, find_data.cFileName, NULL);

                        if ( ScanFile ( hDriver, szPath ) !=0 )
                            return -1;
				    }
			    }
			    
			    found = FindNextFile( hSearch, & find_data );
		    } 
            while( found && g_bRunning );
		    
		    FindClose( hSearch );
	    }
    }
    return 0;
}

int main(int argc, char* argv[])
{
	if (!parse_commandline (GetCommandLine ()))
		return 2;


	SetConsoleCtrlHandler( (PHANDLER_ROUTINE) CtrlHandler, TRUE );

	if (!InitCKAHUM(g_nVer))
		return 2;

    bool needreboot = false;

    if (g_bReload)
    {
	    if (!ReloadCKAHUM(needreboot))
		    return 2;

        if (needreboot)
        {
            if (!g_bSilent)
		        printf("IDS Bases need reboot.\n");
            return 1;
        }

        if (g_bExitAfterReload)
        {
            if (!g_bSilent)
	            printf("IDS Bases reloaded successfully. No reboot needed.\n");
            return 0;
        }
    }
    

	HANDLE hDriver = FindPluginDriver();
	
	if (hDriver == INVALID_HANDLE_VALUE)
	{
		fprintf(stderr, "Error: Cannot find plugin driver\n");
		return 2;
	}

	DWORD dwOutRet;
	if (!DeviceIoControl (hDriver, KLICK_PROCESS_INIT, NULL, 0, NULL, 0, &dwOutRet, NULL))
	{
		fprintf(stderr, "Error: Klick init error\n");
		return 2;
	}

	if (!StartCKAHUM())
		return 2;

    if (g_bLog)
    {
        g_log.open(g_szLog.c_str());
    }

	for(FILENAME_LIST::iterator it = g_Files.begin (); it != g_Files.end () && g_bRunning; ++it)
	{
		const char* szFileName = it->c_str ();

	    TCHAR szDrive [ _MAX_DRIVE ], szDir [ _MAX_DIR ], szName [ _MAX_FNAME ], szExt [ _MAX_EXT ];
	    _tsplitpath ( szFileName, szDrive, szDir, szName, szExt );

        if (strchr(szName, '*') || strchr(szName, '?') || strchr(szExt, '*') || strchr(szExt, '?'))
        {
            if ( ScanDir ( hDriver, szDrive, szDir, szName, szExt, g_bRecursively ) != 0 )
                return 2;
        }
        else
        {
            DWORD fa = GetFileAttributes( szFileName );
            if (fa == -1) continue;

            if (fa & FILE_ATTRIBUTE_DIRECTORY)
            {
                std::string newdir = szDir;
                newdir += szName;
                newdir += szExt;
                newdir += '\\';

                if ( ScanDir ( hDriver, szDrive, newdir.c_str(), "*", NULL, g_bRecursively ) != 0 )
                    return 2;
            }
            else
            {
                if ( ScanFile ( hDriver, szFileName ) != 0 )
                    return 2;
            }
        }
	}

    OutLog(";  -------- END -------");

	DeinitCKAHUM ();

	if (!DeviceIoControl (hDriver, KLICK_PROCESS_DONE, NULL, 0, NULL, 0, &dwOutRet, NULL))
	{
		fprintf (stderr, "Error: Klick deinit error!\n");
		return 2;
	}

	return 0;
}

