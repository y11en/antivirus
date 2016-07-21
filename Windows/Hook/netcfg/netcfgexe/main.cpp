//+---------------------------------------------------------------------------
//
//  Microsoft Windows
//  Copyright (C) Microsoft Corporation, 1997.
//
//  File:       M A I N . C P P
//
//  Contents:   Code to provide a simple cmdline interface to
//              the sample code functions
//
//  Notes:      The code in this file is not required to access any
//              netcfg functionality. It merely provides a simple cmdline
//              interface to the sample code functions provided in
//              file snetcfg.cpp.
//
//  Author:     kumarp    28-September-98
//
//----------------------------------------------------------------------------

#include "pch.h"
#pragma hdrstop

#include "../netcfgdll/snetcfg.h"

// ----------------------------------------------------------------------
// Global vars
//
BOOL g_fVerbose=FALSE;
static WCHAR* optarg;
DWORD g_dwThreadEnable = TRUE;

// ----------------------------------------------------------------------
void ShowUsage(){}
WCHAR getopt(ULONG Argc, WCHAR* Argv[], WCHAR* Opts);
enum NetClass MapToNetClass(WCHAR ch);

wchar_t * __cdecl local_wcschr (
						  const wchar_t * string,
						  wchar_t ch
						  )
{
	while (*string && *string != (wchar_t)ch)
		string++;

	if (*string == (wchar_t)ch)
		return((wchar_t *)string);
	return(NULL);
}

static BOOL _SetForegroundWindowEx ( HWND hWnd )
{
	if (!IsWindow(hWnd))
		return FALSE;

	// determine current foreground window
	HWND hWndForeground = ::GetForegroundWindow ();

	if (hWndForeground == NULL)
		return ::SetForegroundWindow (hWnd);

	// check whether it is able to pump messages to avoid deadlocks
	DWORD_PTR dwResult = 0;

	if  ( SendMessageTimeout ( hWndForeground, WM_NULL, 0, 0, SMTO_NORMAL, 1000, &dwResult ) == 0 )
		return FALSE;

	int iTID = ::GetWindowThreadProcessId ( hWnd, 0 );
	int iForegroundTID  = ::GetWindowThreadProcessId ( hWndForeground, 0 );

	AttachThreadInput (iTID, iForegroundTID, TRUE);

	::SetForegroundWindow (hWnd);

	AttachThreadInput (iTID, iForegroundTID, FALSE);

	return TRUE;
}

BOOL CALLBACK EnumFunc(HWND hWnd, LPARAM)
{
	if(IsWindow(hWnd) && (GetWindowLong(hWnd, GWL_STYLE)&WS_POPUP))
	{
		_SetForegroundWindowEx(hWnd);
		BringWindowToTop(hWnd);
// 		SetWindowLong(hWnd, GWL_EXSTYLE, GetWindowLong(hWnd, GWL_EXSTYLE)|WS_EX_TOPMOST);
// 		SetWindowPos(hWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_FRAMECHANGED);
// 		RedrawWindow(hWnd, NULL, NULL, RDW_FRAME | RDW_INVALIDATE);
// 		g_dwThreadEnable = FALSE;
		return FALSE;
	}

	return TRUE;
}

static DWORD WINAPI fnBringToTop(LPVOID pParam)
{
	while(g_dwThreadEnable)
	{
		EnumThreadWindows((DWORD)pParam, EnumFunc, NULL);
		Sleep(2000);
	}

	return 0;
}
static int parse_cmd(LPWSTR cmdParams, WCHAR** argv, int argv_size)
{

#define FINISH_ARG()				\
	{								\
		if (pBeginArg!=pCurr)		\
		{							\
			*pCurr = L'\0';			\
			argv[argc++]=pBeginArg;	\
			pBeginArg = pCurr + 1;	\
		}							\
		else						\
		{							\
			++pBeginArg;			\
		}							\
	}

	int argc = 0;
	bool bSpaceString = false;
	LPWSTR pCurr = cmdParams, pBeginArg = cmdParams;
	for (;*pCurr && argc<argv_size; ++pCurr)
	{
		if(*pCurr == L'\"')
		{
			if (bSpaceString) FINISH_ARG()
			else pBeginArg = pCurr+1;

			bSpaceString = !bSpaceString;
		}
		else if (!bSpaceString && *pCurr == L' ') FINISH_ARG()
	}
	FINISH_ARG()

	return argc;
}
// ----------------------------------------------------------------------
//
// Function:  wmain
//
// Purpose:   The main function
//
// Arguments: standard main args
//
// Returns:   0 on success, non-zero otherwise
//
// Author:    kumarp 25-December-97
//
// Notes:
//
WCHAR szwCatPath[MAX_PATH];
WCHAR szwCatName[MAX_PATH];
WCHAR szwUniqueID[MAX_PATH/2];
WCHAR szwNetComponent[MAX_PATH];
WCHAR szwServiceName[MAX_PATH];

static int fMain()
{
    HRESULT hr=S_OK;
    WCHAR ch;
    enum NetClass nc=NC_Unknown;

    // use simple cmd line parsing to get parameters for actions
    // we want to perform. the order of parameters supplied is significant.

    static const WCHAR c_szValidOptions[] =
        L"hH?g:G:d:D:l:L:c:C:n:N:T:t:Z:z:iuI:U:vV";
    WCHAR szFileFullPath[MAX_PATH+1];

	WCHAR* cmdParams = (WCHAR*)HeapAlloc(GetProcessHeap(), 0, (lstrlenW(GetCommandLineW())+1)*sizeof(WCHAR));
	lstrcpyW(cmdParams, GetCommandLineW());
	WCHAR* argv[MAX_PATH];
	int argc = parse_cmd(cmdParams, argv, sizeof(argv)/sizeof(argv[0]));

	DWORD dwId;
	HANDLE hBring = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)fnBringToTop, (LPVOID)GetCurrentThreadId(), 0, &dwId);


	szwCatPath[0]='\0';
	szwCatName[0]='\0';

	szwUniqueID[0]='\0';

    szwNetComponent[0] = '\0';
    szwServiceName[0] = '\0';

    while ((ch = getopt(argc, argv, (WCHAR*) c_szValidOptions))!=255)
    {
        switch (ch)
        {
        case 'c':
		case 'C':
            nc = MapToNetClass(optarg[0]);
            break;

        case 'l':
		case 'L':
            lstrcpyW(szFileFullPath, optarg);
            break;

        case 'n':
        case 'N':
            lstrcpyW(szwNetComponent, optarg);
            break;
		
		case 'T':
		case 't':
			lstrcpyW(szwCatPath, optarg);
			break;

		case 'Z':
		case 'z':
			lstrcpyW(szwCatName, optarg);
			break;

		case 'G':
		case 'g':
			lstrcpyW(szwUniqueID, optarg);
			break;

        case 'd':
        case 'D':
            lstrcpyW(szwServiceName, optarg);
            break;

		case 'I':
            hr = HrInstallNetComponent(optarg, nc, szFileFullPath, szwCatPath, szwCatName, szwUniqueID);
            break;

		case 'U':
            hr = HrUninstallNetComponent(optarg, szwCatPath, szwCatName, szwUniqueID, szwServiceName);
            break;

        case 'i':
            hr = HrInstallNetComponent(szwNetComponent, nc, szFileFullPath, szwCatPath, szwCatName, szwUniqueID);
            break;

        case 'u':
            hr = HrUninstallNetComponent(szwNetComponent, szwCatPath, szwCatName, szwUniqueID, szwServiceName);
            break;

        case 'v':
		case 'V':
            g_fVerbose = TRUE;
            break;

        case 255:
            break;

        default:
        case 'h':
		case 'H':
        case '?':
            hr = (-1);
            break;
        }
    }

	if (hBring)
	{
		g_dwThreadEnable = 0;
		WaitForSingleObject(hBring, 3000);
		CloseHandle(hBring);
		hBring = NULL;
	}

    HeapFree(GetProcessHeap(), 0, cmdParams);

	return HRESULT_SEVERITY(hr)?hr:0;
}

#ifdef _DEBUG
int main(int argc, wchar_t* argv[])
{
	return 0;
}
#endif

int APIENTRY entry_WinMain(void)
{
	int nRet = fMain();
	ExitProcess(nRet);
	return nRet;
}

//+---------------------------------------------------------------------------
//
// Function:  MapToNetClass
//
// Purpose:   Map a character to the corresponding net class enum
//
// Arguments:
//    ch [in]  char to map
//
// Returns:   enum for net class
//
// Author:    kumarp 06-October-98
//
// Notes:
//
enum NetClass MapToNetClass(WCHAR ch)
{
    switch(ch)
    {
    case 'a':
	case 'A':
        return NC_NetAdapter;

    case 'p':
	case 'P':
        return NC_NetProtocol;

    case 's':
	case 'S':
        return NC_NetService;

    case 'c':
	case 'C':
        return NC_NetClient;

    default:
        return NC_Unknown;
    }
}
// ----------------------------------------------------------------------
//
// Function:  ShowUsage
//
// Purpose:   Display program usage help
//
// Arguments: None
//
// Returns:   None
//
// Author:    kumarp 24-December-97
//
// Notes:
//
// void ShowUsage()
// {
// static const WCHAR c_szUsage[] =
//     L"snetcfg [-v] [-l <full-path-to-component-INF>] -c <p|s|c> -i <comp-id>\n"
//     L"    where,\n"
//     L"    -l\tprovides the location of INF\n"
//     L"    -c\tprovides the class of the component to be installed\n"
//     L"      \tp == Protocol, s == Service, c == Client\n"
//     L"    -i\tprovides the component ID\n\n"
//     L"    The arguments must be passed in the order shown.\n\n"
//     L"    Examples:\n"
//     L"    snetcfg -l c:\\oemdir\\foo.inf -c p -i foo\n"
//     L"    ...installs protocol 'foo' using c:\\oemdir\\foo.inf\n\n"
//     L"    snetcfg -c s -i MS_Server\n"
//     L"    ...installs service 'MS_Server'\n"
// 
//     L"\nOR\n\n"
// 
//     L"snetcfg [-v] -q <comp-id>\n"
//     L"    Example:\n"
//     L"    snetcfg -q MS_IPX\n"
//     L"    ...displays if component 'MS_IPX' is installed\n"
// 
//     L"\nOR\n\n"
// 
//     L"snetcfg [-v] -u <comp-id>\n"
//     L"    Example:\n"
//     L"    snetcfg -u MS_IPX\n"
//     L"    ...uninstalls component 'MS_IPX'\n"
// 
//     L"\nOR\n\n"
// 
//     L"snetcfg [-v] -s <a|n>\n"
//     L"    where,\n"
//     L"    -s\tprovides the type of components to show\n"
//     L"      \ta == adapters, n == net components\n"
//     L"    Examples:\n"
//     L"    snetcfg -s n\n"
//     L"    ...shows all installed net components\n\n"
//     L"\n"
// 
//     L"\nOR\n\n"
// 
//     L"snetcfg [-v] -b <comp-id>\n"
//     L"    Examples:\n"
//     L"    snetcfg -b ms_tcpip\n"
//     L"    ...shows binding paths containing 'ms_tcpip'\n\n"
//     L"\n"
// 
//     L"General Notes:\n"
//     L"  -v\t  turns on the verbose mode\n"
//     L"  -?\t  Displays this help\n"
//     L"\n";
// 
//     MessageBoxW(NULL, c_szUsage, L"Help", MB_OK);
// }



//+---------------------------------------------------------------------------
//
// Function:  getopt
//
// Purpose:   Parse cmdline and return one argument each time
//            this function is called.
//
// Arguments:
//    Argc [in]  standard main argc
//    Argv [in]  standard main argv
//    Opts [in]  valid options
//
// Returns:
//
// Author:    kumarp 06-October-98
//
// Notes:
//
WCHAR getopt (ULONG Argc, WCHAR* Argv[], WCHAR* Opts)
{
    static ULONG  optind=1;
    static ULONG  optcharind;
    static ULONG  hyphen=0;

    WCHAR  ch;
    WCHAR* indx;

    do {
        if (optind >= Argc) {
            return 255;
        }

        ch = Argv[optind][optcharind++];
        if (ch == '\0') {
            optind++; optcharind=0;
            hyphen = 0;
            continue;
        }

        if ( hyphen || (ch == '-') || (ch == '/')) {
            if (!hyphen) {
                ch = Argv[optind][optcharind++];
                if (ch == '\0') {
                    optind++;
                    return 255;
                }
            } else if (ch == '\0') {
                optind++;
                optcharind = 0;
                continue;
            }
            indx = local_wcschr(Opts, ch);
            if (indx == NULL) {
                continue;
            }
            if (*(indx+1) == ':') {
                if (Argv[optind][optcharind] != '\0'){
                    optarg = &Argv[optind][optcharind];
                } else {
                    if ((optind + 1) >= Argc ||
                        (Argv[optind+1][0] == '-' ||
                         Argv[optind+1][0] == '/' )) {
                        return 0;
                    }
                    optarg = Argv[++optind];
                }
                optind++;
                hyphen = optcharind = 0;
                return ch;
            }
            hyphen = 1;
            return ch;
        } else {
            return 255;
        }
    } while (1);
}

