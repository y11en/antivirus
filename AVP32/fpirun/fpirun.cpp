// fpirun.cpp -- simple program to run and test FPI modules
//
// Copyright (c) 1997-2000 F-Secure Corporation

#include <stdio.h>
#if defined _CONSOLE && !defined UNIX
#include <conio.h>
#endif
#if defined UNIX
#include <unistd.h>
#include <dirent.h>
#endif

#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include <math.h>
#include <ctype.h>
#include <io.h>

#ifdef OS2
#include "myos2.h"
#define FARPROC PFN
#endif

#ifdef _DEBUG
#if _MSC_VER >= 900
//#include "pure.h"
//#define USE_PURIFY
#endif
#endif

#if _MSC_VER >= 900
#define USE_TRY
#endif

extern "C"
{
#include "fpiapi.h"
#include "fpirun.h"
#include "mydir.h"
#include "version.h"

#if defined USE_FM
#if defined(_MSC_VER) && _MSC_VER < 900
#pragma pack(8)
#else
#pragma pack(push,8)
#endif
#include "fmngm.h"
#if defined(_MSC_VER) && _MSC_VER < 900
#pragma pack()
#else
#pragma pack(pop)
#endif
#endif
}

#if !defined(MAKELONG)
#define MAKELONG(low,high)  ((DWORD)((WORD)(low)) | (((DWORD)(WORD)(high))<<16))
#endif

#if !defined(MAKEWORD)
#define MAKEWORD(low,high)  ((WORD)((BYTE)(low)) | (((WORD)(BYTE)(high))<<8))
#endif

#if !defined(HIWORD)
#define HIWORD(dw)		((WORD)(((DWORD)dw) >> 16))
#endif

#if !defined(HIBYTE)
#define HIBYTE(w)		((BYTE)(((WORD)w) >> 8))
#endif

#if !defined(LOBYTE)
#define LOBYTE(w)		((BYTE)(w))
#endif

#ifndef max
#define max(x,y) ((x) > (y) ? (x) : (y))
#endif

#define FSAV_EXIT_NORMAL     0
#define FSAV_EXIT_ABNORMAL   1
#define FSAV_EXIT_SELFTEST   2
#define FSAV_EXIT_INFECTION  3
// 4 is reserved
#define FSAV_EXIT_BREAK      5
#define FSAV_EXIT_DISINF     6
#define FSAV_EXIT_MEMORY     7
#define FSAV_EXIT_SUSPICIOUS 8

#ifdef FSAV
#define FSAV_ACTION_DELETE     2U
#define FSAV_ACTION_RENAME   3U
#endif

#define REFDATA_FILE    0x0100

#define REFDATA_BOOT    0x1000
#define REFDATA_MBR     0x2000
#define REFDATA_MEMORY  0x4000
#define REFDATA_VALIDATEDATABASES  0x5000


#if defined _WIN32
#define DEFAULT_FPI_MODULE  "fpfpi32.dll"
#elif defined OS2
#define DEFAULT_FPI_MODULE  "fpfpios2"
#elif defined UNIX
#define DEFAULT_FPI_MODULE  ""
#else
#define DEFAULT_FPI_MODULE  "fpfpi16.dll"
#endif


// Prototypes for internal functions
void usage();
void print_banner(int argc, char** argv);
void get_options(int argc, char** argv);
void get_interest_option(const char* arg);
void get_debug_option(const char* arg);
void scan_boot(int c, int number, WORD action);
void scan_memory(WORD action);
char* strmaxcpy(char *string1, const char *string2, size_t count);
#ifdef UNIX
void set_prog_dir_by_arg0(const char* arg0);
void check_root();
BOOL is_link(const char* name);
#endif

BOOL touchfile(const char* name, FPICONTAINERHANDLE chandle);
BOOL touchfile(const char* name, FPICONTAINERHANDLE chandle, int readcount, int readsize, int readlinear);

#ifdef ENABLE_MEASURE_STACK
#define MEASURE_STACK_MAX 32000
void BeginMeasureStackUsage(const char* name);
void EndMeasureStackUsage(const char* name);
#endif

#ifdef TRACK_FM
void BeginTrackFMExtra(const char* name);
void EndTrackFMExtra(const char* name);
#endif

const char* logfilename = NULL;
#ifdef _CONSOLE
FILE* logfile = stdout;
#else
FILE* logfile = NULL;
#endif

BOOL displaying_status = FALSE;
BOOL nofpi = FALSE;                             // if TRUE, no actual scanning is done
BOOL fakeread = FALSE;
int readcount;
int readsize;
int readlinear;

const char* fpifilename = DEFAULT_FPI_MODULE;
HMODULE fpi = NULL;

const char* collectdirname;
const char* validatedirname;

const char* statfilename = NULL;
FILE* statfile = NULL;

const char* extensionfilter = NULL;
#ifdef UNIX
const char* stdextensionfilter = "com,exe,sys,ov?,app,pgm,bin,scr,do?,xl?,htm,html,zip,lzh,arj";
#else
const char* stdextensionfilter = "COM,EXE,SYS,OV?,APP,PGM,BIN,SCR,DO?,XL?,RTF,ZIP,LZH,ARJ";
#endif

BOOL isFileClean;
BOOL isFileFailed;
BOOL isFileInfected;
BOOL isFileSuspicious;
BOOL isFileDisinfected;
BOOL isFileDisinfectFailed;


FPIFILEIOFNTABLE io;

struct
{
    LPFPI_GetModuleInformation  FPI_GetModuleInformation;
    LPFPI_Initialize            FPI_Initialize;
    LPFPI_Uninitialize          FPI_Uninitialize;
    LPFPI_ScanFile              FPI_ScanFile;
    LPFPI_ScanBootBlock         FPI_ScanBootBlock;
    LPFPI_ScanMemory            FPI_ScanMemory;
	LPFPI_ValidateDatabases		FPI_ValidateDatabases;
    LPFPI_ReloadDatabases       FPI_ReloadDatabases;
} fpifun;


const char* fpierror[] =
{
    "FPIERROR_SUCCESS",
    "FPIERROR_UNKNOWN",
    "FPIERROR_INIT_MULTIPLE",
    "FPIERROR_NOTINIT",
    "FPIERROR_NOMEM",
    "FPIERROR_NOPGMFILE",
    "FPIERROR_NODBFILE",
    "FPIERROR_BADDBFILE",
    "FPIERROR_FILEOPEN",
    "FPIERROR_FILEENCRYPTED",
    "FPIERROR_FILEREAD",
    "FPIERROR_FILEWRITE",
    "FPIERROR_SECTORREAD",
    "FPIERROR_SECTORWRITE",
    "FPIERROR_WRITEPROTECT",
    "FPIERROR_NOTSUPPORTED",
    "FPIERROR_INVALIDPARAMETER",
    "FPIERROR_WRONGAPI",
    "FPIERROR_ABORTED",
};


DWORD apiversion = 0;
DWORD try_apiversions[] =
{
	MAKELONG(0, MAKEWORD(17, 1)),		// FPI API 1.17
	MAKELONG(0, MAKEWORD(15, 1)),		// FPI API 1.15
	MAKELONG(0, MAKEWORD(12, 1)),		// FPI API 1.12
	MAKELONG(0, MAKEWORD(11, 1)),		// FPI API 1.11
	MAKELONG(0, MAKEWORD(10, 1)),		// FPI API 1.10
};
#define N_TRY_APIVERSIONS	(sizeof try_apiversions / sizeof try_apiversions[0])


FPIMODULEINFO moduleinfo;

#ifdef OS2
HAB g_hab;
#endif



BOOL bExitScan = FALSE;
BOOL bQuit = FALSE;
long nDirectories = 0;
long nFiles = 0;
long nFilesInArchives = 0;

long nFilesClean = 0;           // \ These are mutually exclusive
long nFilesInfected = 0;        // |
long nFilesSuspicious = 0;      // |
long nFilesFailed = 0;          // /

long nFilesDisinfected = 0;     // \ These are mutually exclusive
long nFilesDisinfectFailed = 0; // /

long nInfections = 0;
long nSuspicious = 0;
long nScanErrors = 0;
long nApiErrors = 0;
long nOtherErrors = 0;
long nDisinfections = 0;
long nCollected = 0;
DWORD tickStart;
DWORD tickStartArc;
DWORD tickEnd;
DWORD ticks;
DWORD tickStartFile;
DWORD maxTicksFile;
DWORD sumTicksInfected;
DWORD sumTicksSuspicious;

#ifdef ENABLE_MEASURE_STACK
int biggestStackUsage = 0;
#endif

int nargs = 0;


DWORD ticksMaxClass[5] =  // upper bounds of each counting bin, in ticks.
{
    10,100,1000,10000,0xFFFFFFFF,
};
long nFilesClass[5];

struct fpirunoptions options;


char status[200] = "";


////////////////////////////////////////////////////////////////////
// Declarations for File handling

#ifdef USE_FM

// struct tagFSFP_REFINFO;

typedef struct FPContainerEntry {
    HFMFILE hndl;
    HFMSEARCH sHndl;  // to perform "FindNext"/"FindFirst" inside the
                     // container
    BOOL found; // was the last "FindNext"/"FindFirst" successful ?
    FMFILEINFO fInfo;  // data of the current entry INSIDE the container
    FMFINDDATA fData;
// other necessary items should go here
    LPCSTR	fThisName;	// name of *this* file
    DWORD	tickStart;	// time (ticks) when scanning started
} FPContainerEntry;

FPContainerEntry* cStack = NULL; // stack of nested containers
short onStack; // current number of elements on "cStack"

long myCount;  // testing

static BOOL ProcessEntry(HFMFILE hfmParent, const char *name, WORD action);
static BOOL StartScan(const char *path, WORD action);

#endif // USE_FM


#ifndef USE_FM
char filename[300+1];
#endif // !USE_FM


////////////////////////////////////////////////////////////////////
// Main program

#ifdef _CONSOLE

int main(int argc, char** argv)
{
    return mymain(argc, argv);
}

#endif

int mymain(int argc, char** argv)
{
    int ret = FSAV_EXIT_NORMAL;

#ifdef OS2
    g_hab = WinInitialize(0);
#endif

#ifdef UNIX
    set_prog_dir_by_arg0(argv[0]);
#endif

    get_options(argc, argv);
    print_banner(argc, argv);
    init_io_table();
           
    if (fpi == NULL)
    {
        if (!nofpi)
        {
            if (!load_fpi_module(&fpi, fpifilename))
                return FSAV_EXIT_ABNORMAL;
        }
    }

    ret = begin_scanning();
    if (ret != FSAV_EXIT_NORMAL)
	return ret;

    ret = do_scanning(argc, argv);
    if (ret != FSAV_EXIT_NORMAL)
	return ret;

    ret = end_scanning();
    return ret;
}

void init_numbers()
{
    bExitScan = FALSE;
    bQuit = FALSE;
    nDirectories = 0;
    nFiles = 0;
    nFilesInArchives = 0;

    nFilesClean = 0;
    nFilesInfected = 0;
    nFilesSuspicious = 0;
    nFilesFailed = 0;

    nFilesDisinfected = 0;
    nFilesDisinfectFailed = 0;

    nInfections = 0;
    nSuspicious = 0;
    nScanErrors = 0;
    nApiErrors = 0;
    nOtherErrors = 0;
    nDisinfections = 0;
    nCollected = 0;

    tickStart = 0;
    tickEnd = 0;
    ticks = 0;
    tickStartFile = 0;
    maxTicksFile = 0;
    sumTicksInfected = 0;
    sumTicksSuspicious = 0;
}

//#include "nmapilib.h"
int begin_scanning()
{
   init_numbers();
    
#ifdef USE_FM
    if (options.verbose)
        printboth(">> Initializing File Management module...");

    cStack = (FPContainerEntry*)malloc((options.max_recursion_arc+1) * sizeof(FPContainerEntry));
    if (cStack == NULL)
    {
        printboth("\nFile Management initialization failed (not enough memory for stack).\n");
        nOtherErrors++;
        return FSAV_EXIT_ABNORMAL;
    }
    BOOL bFmInit = fmInitializeModule(0);
    if (!bFmInit)
    {
        printboth("\nFile Management initialization failed.\n");
        nOtherErrors++;
        return FSAV_EXIT_ABNORMAL;
    }
    if (options.verbose)
        printboth("ok\n");
#endif // USE_FM

    if (nofpi)
    {
        printboth(">> Not using any FPI module\n");
    }
    else
    {
        int i;
	for (i=0; i<N_TRY_APIVERSIONS; i++)
	{
	    apiversion = try_apiversions[i];
#ifndef FSAV
		if (!options.terse)
		{
			printboth(">> Getting FPI module information (FPI API %d.%02d)...",
				  HIBYTE(HIWORD(apiversion)),
				  LOBYTE(HIWORD(apiversion)));
		}
#endif
	    moduleinfo.fStructSize = sizeof moduleinfo;
	    if (fpifun.FPI_GetModuleInformation(apiversion, &moduleinfo, &io, FN_CALLBACK))
	        break;		// Succeeded

	    // Otherwise it didn't succeeed.
	    // Error message has already been shown by report callback.
	}
	if (i >= N_TRY_APIVERSIONS)
	{
	    // None of the API versions worked.
	    nOtherErrors++;
	    return FSAV_EXIT_ABNORMAL;
	}

#ifndef FSAV
	if (options.terse)
	{
		printboth(">> Engine:\t%s %d.%02d.%d\n",
			moduleinfo.fModuleName,
			(int)moduleinfo.fVerMajor,
			(int)moduleinfo.fVerMinor,
			(int)moduleinfo.fVerBuild
			);
	}
	else
	{
		printboth("ok\n");
		printboth("\t"
			"id=%hu, "
			"name=\"%s\", "
			"version=%d.%02d.%d, "
			"database=%04hu-%02hu-%02hu"
			"\n\t"
			"vendor=\"%s\"\n",
			moduleinfo.fModuleID,
			moduleinfo.fModuleName,
			(int)moduleinfo.fVerMajor,
			(int)moduleinfo.fVerMinor,
			(int)moduleinfo.fVerBuild,
			moduleinfo.fDatabaseYear,
			(WORD)moduleinfo.fDatabaseMonth,
			(WORD)moduleinfo.fDatabaseDay,
			moduleinfo.fVendorName);
	}
#endif

	if (apiversion >= MAKELONG(0, MAKEWORD(15, 1)))		// the interface is valid for API 1.15 and up.
		fpifun.FPI_ValidateDatabases(NULL, REFDATA_VALIDATEDATABASES);

	if (options.validate
		&& apiversion >= MAKELONG(0, MAKEWORD(15, 1)))		// the interface is valid for API 1.15 and up.
	{
		fpifun.FPI_ValidateDatabases(validatedirname, REFDATA_VALIDATEDATABASES);
	}

	if (options.version_only)
	    exit(0);

#ifdef UNIX
	check_root();
#endif

        if (options.verbose)
            printboth(">> Initializing FPI module...");
		
        if (!fpifun.FPI_Initialize())
        {
            // error message has already been shown by report callback
            printboth("FPI_Initialize failed\n");
            nOtherErrors++;
            return FSAV_EXIT_ABNORMAL;
        }
        if (options.verbose)
            printboth("ok\n");
    }
	

#ifndef FSAV
#ifdef _CONSOLE
	if (!options.terse)
		printboth("\n>> Scanning (press x to stop)...\n\n");
#else
    printboth(">> Scanning...\n\n");
#endif
#endif
    tickStart = GetTickCount();

    return FSAV_EXIT_NORMAL;
}

int do_scanning(int argc, char** argv)
{
    int i;
    char arg[200];

    for (i=1; i<argc; i++)
    {
	if (argv[i][0] == '-')
	{
	    int c = argv[i][1];
	    
	    if (c=='m' || c=='b')
	    {
		int number = atoi(&argv[i][2]);
		if (c=='m')
		    number += 0x80;
                scan_boot(c, number, options.action);
		
                nargs++;
                continue;
            }
            else if (c=='M')
            {
                scan_memory(options.action);

                nargs++;
                continue;
            }
            else
            {
                continue;   // options were handled earlier, just skip them now
            }
        }

        nargs++;

        if (bExitScan)
            break;

        strcpy(arg, argv[i]);
		// remove trailing backslash if any
		if (strlen(arg)>3 && arg[strlen(arg)-1]=='\\')
			arg[strlen(arg)-1] = '\0';
		// add backslash if just a drive letter given
		if (strlen(arg)==2 && isalpha(arg[0]) && arg[1]==':')
            strcat(arg, "\\");
        scan_it(arg, options.action, 0);
    }

    return FSAV_EXIT_NORMAL;
}

const char* seconds_to_ms(double s)
{
    static char buffer[50];

    long sec = (long)s;
    long min = sec/60;
    sec %= 60;

    sprintf(buffer, "%ld:%02ld", min, sec);
    return buffer;
}

int end_scanning()
{
#ifdef _WIN32
    if (nargs == 0)
    {
        printboth(">> Nothing to scan\n\n");
    }
    else
#endif
    {
        tickEnd = GetTickCount();
        ticks = tickEnd - tickStart;

        if (bQuit)
        {
            return FSAV_EXIT_BREAK;   
        }

#ifndef FSAV    
		if (!options.terse)
		{
			printboth("\n>> Scanning finished in %.1f seconds (%s).\n",
				((double)ticks)/1000.0,
				seconds_to_ms(ticks/1000.0));
			printboth("\tDirectories:          %ld\n", nDirectories);
			printboth("\tFiles:                %ld", nFiles);
			if (nFilesInArchives)
			{
				printboth(" + %ld in archives = %ld total", nFilesInArchives, nFiles + nFilesInArchives);
			}
			printboth("\n");

			if (nFilesClean)
				printboth("\t- clean files:        %ld\n", nFilesClean);
			if (nFilesInfected)
				printboth("\t- infected files:     %ld\n", nFilesInfected);
			if (nFilesSuspicious)
				printboth("\t- suspicious files:   %ld\n", nFilesSuspicious);
			if (nFilesFailed)
				printboth("\t- scan failed files:  %ld\n", nFilesFailed);

			if (nFilesDisinfected || nFilesDisinfectFailed)
				printboth("\n");
			if (nFilesDisinfected)
				printboth("\tDisinfected files:    %ld\n", nFilesDisinfected);
			if (nFilesDisinfectFailed)
				printboth("\tDisinf failed files:  %ld\n", nFilesDisinfectFailed);

			long nTotalErrors = nScanErrors + nApiErrors + nOtherErrors;
			if (nInfections || nSuspicious || nTotalErrors)
				printboth("\n");
			if (nInfections)
				printboth("\t# infections:         %ld\n", nInfections);
			if (nSuspicious)
				printboth("\t# suspections:        %ld\n", nSuspicious);
			if (nTotalErrors > 0)
				printboth("\t# errors:             %ld\n", nTotalErrors);
			if (nScanErrors > 0)
				printboth("\t- # scanning errors:  %ld\n", nScanErrors);
			if (nApiErrors > 0)
				printboth("\t- # FPI violations:   %ld\n", nApiErrors);
			if (nOtherErrors > 0)
				printboth("\t- # Other errors:     %ld\n", nOtherErrors);

			if (nDisinfections)
				printboth("\tDisinfections:        %ld\n", nDisinfections);
			if (nCollected)
				printboth("\tCollected:            %ld\n", nCollected);

			updatestatus("");
			printboth("\n>> Status:\n%s", status);

			if (options.statistics)
				print_statistics();
		}

#ifdef ENABLE_MEASURE_STACK
        if (options.measure_stack)
            printboth("Biggest stack usage was %d bytes\n", biggestStackUsage);
#endif
#endif
    }

    if (!nofpi)
    {
        if (options.verbose)
            printboth(">> Uninitializing FPI module...");
        if (!fpifun.FPI_Uninitialize())
        {
            // error message has already been shown by report callback
            printboth("FPI_Uninitialize failed\n");
            nOtherErrors++;
            return FSAV_EXIT_ABNORMAL;
        }
        if (options.verbose)
            printboth("ok\n");
    }

#ifdef USE_FM
    if (options.verbose)
        printboth(">> Unitializing File Management module...");
    BOOL bFmUninit = fmUninitializeModule();
    if (!bFmUninit)
    {
        printboth("\nFile Management uninitialization failed.\n");
        nOtherErrors++;
        return FSAV_EXIT_ABNORMAL;
    }
    free(cStack);
    cStack = NULL;
    if (options.verbose)
        printboth("ok\n");
#endif // USE_FM

#ifndef FSAV
	if (options.terse)
		printboth("\n");
	else
		printboth(">> All done.\n");
#endif

    return FSAV_EXIT_NORMAL;
}

#ifdef UNIX
void scan_directory(const char* name, WORD action, int depth)
{
    if (bExitScan)
        return;

    DIR* dir;
    struct dirent* ent;
    char fullname[300];

    dir = opendir(name);
    if (dir==NULL)
        return;

    while ((ent = readdir(dir)) != NULL)
    {
        if (bExitScan)
	    break;
	if (ent->d_name[0] == '.')
	    continue;

	strcpy(fullname, name);
	strcat(fullname, "/");
	strcat(fullname, ent->d_name);

	scan_it(fullname, action, depth+1);
    }

    closedir(dir);
}

#else

void scan_directory(const char* name, WORD action, int depth)
{
    char pattern[300];
    int pathlen;
    my_finddata_t   info;
    my_findhandle_t handle;
    if (bExitScan)
        return;
        
    strcpy(pattern, name);
    pathlen = strlen(pattern);
    
    if (pathlen<1 || pattern[pathlen-1]!='\\')
    {
        strcpy(pattern+pathlen, "\\");
        pathlen++;
    }

    strcat(pattern, "*.*");

    
    handle = my_findfirst(pattern, &info);
    if (handle != MY_FINDHANDLE_INVALID)
    {
        do
        {
            if (bExitScan)
                break;
            if (info.name[0] == '.')
                continue;

            strcpy(pattern+pathlen, info.name);
            scan_it(pattern, action, depth+1);
        } while (my_findnext(handle, &info));

        my_findclose(handle);
    }
}
#endif


void scan_it(const char* name, WORD action, int depth)
{
    struct stat buffer;

#ifdef UNIX
    if (!options.follow_symlinks)
    {
	// simply don't follow symlinks
	if (is_link(name))
	    return;
    }
    else
    {
	// TODO: follow symlinks but try to avoid loops
    }
#endif

    if (stat(name, &buffer) != -1)
    {
        if (buffer.st_mode & _S_IFDIR)
        {
            if (depth > options.max_recursion_dir)
                return;

            if (options.interest_directories)
                printlog("%s:\tdirectory\n", name);
            scan_directory(name, action, depth);
            nDirectories++;
            return;
        }

#ifdef UNIX
	// skip special files (non-directory, non-regular)
	if (!S_ISREG(buffer.st_mode))
	    return;
#endif
    }

#ifdef USE_FM
    StartScan(name, action);     // using File Management DLL
#else
    strmaxcpy(filename, name, 300);
    if (scan_file(name, FPICONTAINERHANDLE_NULL, action))
        nFiles++;
#endif // USE_FM

}






////////////////////////////////////////////////////////////////////////////
// Internal auxiliary functions


int printstatus(const char* format, ...)
{
#ifdef FSAV
    // No status display in FSAV product
    return 0;
#else
    char msg[300];
    int ret;
    va_list arg;
    va_start(arg,format);
    vsprintf(msg,format,arg);
    va_end(arg);

#ifdef _CONSOLE
    ret = fputs(msg,stderr);
    fflush(stderr);
#else
    winshowstatus(msg);
#endif

    displaying_status = TRUE;

    return ret;
#endif
}

// "printlog" is used for normal messages (infections etc)
// which are shown on stdout or other log file.
int printlog(const char* format, ...)
{
    char msg[500];
    int ret = 0;
    va_list arg;
    va_start(arg,format);
    vsprintf(msg,format,arg);
    va_end(arg);

    if (logfile != NULL)
    {
        if (isatty(fileno(logfile)) && displaying_status)
        {
            printstatus("\r%79s\r", " ");
            displaying_status = FALSE;
        }

        ret = fputs(msg,logfile);
        fflush(logfile);
    }
#ifndef _CONSOLE
    winshowlog(msg);
#endif

    return ret;
}

// "printboth" is used for critical messages (errors etc)
// which must be shown on stderr.
int printboth(const char* format, ...)
{
    char msg[500];
    int ret;
    va_list arg;
    va_start(arg,format);
    vsprintf(msg,format,arg);
    va_end(arg);

    BOOL bScreen = (logfile==NULL || isatty(fileno(logfile)));
    if (bScreen && displaying_status)
    {
        // Logging to console
        printstatus("\r%79s\r", " ");
    }
    displaying_status = FALSE;

#ifdef _CONSOLE
    ret = fputs(msg,stderr);
    fflush(stderr);
#else
    winshowlog(msg);
#endif

    if (!bScreen)
    {
        // Logging to file
        ret = fputs(msg,logfile);
        fflush(logfile);
    }

    return ret;
}

DWORD GetCpuTime()
{
#ifdef _WIN32
    // Return cpu time used (kernel+user), in ticks

    static HANDLE hProcess = NULL;
    FILETIME ftCreation;
    FILETIME ftExit;
    FILETIME ftKernel;
    FILETIME ftUser;

    __int64 kernel;
    __int64 user;

    if (hProcess == NULL)
        hProcess = GetCurrentProcess();
    if (!GetProcessTimes(hProcess, &ftCreation, &ftExit, &ftKernel, &ftUser))
        return 0;

    kernel = ftKernel.dwHighDateTime;
    kernel <<= 32;
    kernel += ftKernel.dwLowDateTime;

    user = ftUser.dwHighDateTime;
    user <<= 32;
    user += ftUser.dwLowDateTime;

    return (DWORD) ((kernel+user) / 10000);
#else
    return 0;   // CPU time not available in Win16
#endif
}

void updatestatus(const char* name)
{
    DWORD ticks = GetTickCount() - tickStart;
    DWORD ticksCpu = GetCpuTime();
    long nFilesTotal = nFiles + nFilesInArchives;

    sprintf(status, "t=%ld/%ld: D%ld F%ld(%.2f %.1f) I%ld #%ld/%ld/%ld/%ld/%ld %.50s",
        ticksCpu/1000,
        ticks/1000,
        nDirectories,
        nFilesTotal,
        (double)ticks / max(1,nFilesTotal) / 1000.0,
        (double)maxTicksFile / 1000.0,
        nInfections,
        nFilesClass[0],
        nFilesClass[1],
        nFilesClass[2],
        nFilesClass[3],
        nFilesClass[4],
        name
        );
}


void scan_boot(int c, int number, WORD action)
{
    WORD blocktype  = (c=='m' ? FPIBOOTBLOCK_MBR : FPIBOOTBLOCK_BOOT);
    char* blockname  = (c=='m' ? "MBR" : "BootBlock");
    WORD blocksize  = 512;  // Note: Might not be actually correct
    BOOL ok;

    DWORD refdata;
    if (c=='m')
        refdata = REFDATA_MBR;
    else
        refdata = REFDATA_BOOT;
    refdata += number;

#ifdef USE_PURIFY
    if (options.purify_leaks)
	{
		PurifyNewLeaks();
		PurifyNewInuse();
		PurifyNewHandlesInuse();
	}
    if (options.purify_printf_files)
        PurifyPrintf("Scanning boot %c%d\n", c, number);
#endif

    if (!nofpi)
    {
#ifdef ENABLE_MEASURE_STACK
        if (options.measure_stack)
            BeginMeasureStackUsage("boot");
#endif
        ok = fpifun.FPI_ScanBootBlock(number, blocktype, blocksize, action, refdata);
#ifdef ENABLE_MEASURE_STACK
        if (options.measure_stack)
            EndMeasureStackUsage("boot");
#endif

        if (ok)
            printlog("%s 0x%X:\tscanned\n", blockname, number);
        else
            printlog("%s 0x%X:\tscan failed\n", blockname, number);
    }
}


void scan_memory(WORD action)
{
    if (!nofpi
        && !fpifun.FPI_ScanMemory(action, REFDATA_MEMORY))
    {
        if (options.interest_errors)
            printlog("memory:\tscan failed\n");
        // TODO: Could check that error callback has been called
    }
    else
    {
        printlog("memory:\tscanned\n");
    }
}

int strnicmpwild(const char* wild, const char* s, int n)
{
    int i;
    for (i=0; i<n; i++)
    {
        // did one of the strings end?
        if (wild[i]=='\0' || s[i]=='\0')
            return wild[i]-s[i];

        // no -- compare chars, unless wild is ?
        if (wild[i]!='?'
            && wild[i]!=s[i])
            return wild[i]-s[i];
    }

    return 0;
}


BOOL matches_filters(const char* name)
{
    if (!options.filter_extension)
        return TRUE;

    const char* ext = strrchr(name, '.');
    if (ext == NULL)
        return FALSE;
    ext++;

    int pos = 0;
    while (extensionfilter[pos] != '\0')
    {
        int len = strcspn(extensionfilter+pos, ",");
        if (strnicmpwild(extensionfilter+pos, ext, len) == 0)
            return TRUE;
        
        pos += len;
        if (extensionfilter[pos] == ',')
            pos++;
    }

    return FALSE;
}


BOOL scan_file(const char* name, FPICONTAINERHANDLE chandle, WORD action)
{
    DWORD tickEndFile;
    DWORD ticksFile;
    int bin;    // index to file counting bin
    DWORD refdata;
	char fullname[300+1];

#ifdef USE_FM
    refdata = REFDATA_FILE + onStack;
    get_fullname_from_refdata(refdata, fullname, sizeof fullname);
	cStack[onStack].tickStart = GetTickCount();
#else
    refdata = 0;
    strmaxcpy(fullname, name, 300);
#endif // USE_FM

#if defined _CONSOLE
#if !defined UNIX
    if (kbhit())
    {
        int c = getch();
        if (c=='x')
        {
            printboth("\n[STOPPED BY USER]\n");
            bExitScan = TRUE;
            return FALSE;
        }
    }
#endif
#else
    if (run_message_loop())
    {
        bExitScan = TRUE;
        bQuit = TRUE;
        return FALSE;
    }

    if (bExitScan)
    {
#ifndef FSAV
        printboth("\n[STOPPED BY USER]\n");
#endif
        return FALSE;
    }
#endif

    if (!matches_filters(name))
        return FALSE;

    updatestatus(fullname);
    printstatus("\r%-79.79s\r", status);

    tickStartFile = GetTickCount();

    BOOL ok = FALSE;

    isFileClean         = TRUE;
    isFileInfected      = FALSE;
    isFileSuspicious    = FALSE;
    isFileDisinfected   = FALSE;
    isFileFailed        = FALSE;
    isFileDisinfectFailed   = FALSE;

    if (nofpi)
    {
        if (fakeread)
        {
            ok = touchfile(name, chandle, readcount, readsize, readlinear);
        }
        else
        {
            ok = touchfile(name, chandle);
        }
    }
    else
    {
#ifdef TRACK_FM
		if (options.track_fm_extra)
			BeginTrackFMExtra(name);
#endif
#ifdef ENABLE_MEASURE_STACK
        if (options.measure_stack)
            BeginMeasureStackUsage(name);
#endif

		try
		{
			ok = fpifun.FPI_ScanFile(name, chandle, action, refdata);
		}
		catch (...)
		{
			if (options.no_catch)
				throw;
			printboth("%s:\t[*** UNCAUGHT EXCEPTION IN SCANNER ***]\n", fullname);
			printlog("\n%-79.79s", status);
			printlog("\n");
		}

#ifdef ENABLE_MEASURE_STACK
        if (options.measure_stack)
            EndMeasureStackUsage(name);
#endif
#ifdef TRACK_FM
		if (options.track_fm_extra)
			EndTrackFMExtra(name);
#endif
    }

	tickEndFile = GetTickCount();
	ticksFile = tickEndFile - tickStartFile;

    if (!ok)
    {
        if (options.interest_time && ticksFile >= options.interest_time_limit*1000UL)
        {
            printlog("%s:\tscan failed in %g s\n", fullname, ticksFile/1000.0);
            collect_sample("slo", chandle, fullname, name);
        }
		else if (options.interest_errors)
            printlog("%s:\tscan failed\n", fullname);

        // TODO: Could check that error callback has been called
        if (isFileDisinfected)
        {
            isFileDisinfected = FALSE;
            isFileDisinfectFailed = TRUE;
        }
        else
        {
            isFileFailed = TRUE;
        }
    }
    else
    {
        const char* clean = (isFileClean ? " clean" : "");

        if (options.interest_time && ticksFile >= options.interest_time_limit*1000UL)
        {
            printlog("%s:\tscanned%s in %g s\n", fullname, clean, ticksFile/1000.0);
            collect_sample("slo", chandle, fullname, name);
        }
        else if (options.interest_filenames
            || (options.interest_cleanfiles && isFileClean))
            printlog("%s:\tscanned%s\n", fullname, clean);

        if (ticksFile > maxTicksFile)
            maxTicksFile = ticksFile;

        for (bin=0; ticksMaxClass[bin] > 0; bin++)
        {
            if (ticksFile < ticksMaxClass[bin])
                break;
        }
        nFilesClass[bin]++;

        add_statistics(fullname, refdata, bin, ticksFile);
    }

	return TRUE;
}


#ifdef FSAV

void invalid_option(const char* arg)
{
    fprintf(stderr, "fsav: invalid option %s\n\n", arg);
    fprintf(stderr, "try \"fsav --help\"\n");
    exit(FSAV_EXIT_ABNORMAL);
}

void usage()
{
    fprintf(stderr, "usage: fsav [options | paths]\n"
	    "\n"
	    "valid options:\n"
	    "--archive   Scan inside .zip, .arj and .lzh archives\n"
	    "--auto      Do not ask before disinfect, delete or rename\n"
	    "--delete    Delete infected files\n"
	    "--disinf    Disinfect infected files\n"
	    "--dumb      \"Dumb\" scan of all files without regard to extension\n"
	    "--extensions=ext,ext,...   Scan files with specified extensions\n"
	    "--help      Display command line options\n"
	    "--list      List all files that are scanned\n"
	    "--rename    Rename infected files to filename.virus\n"
	    "--short     Use short output format\n"
	    "--silent    Do not generate any output (except errors)\n"
	    "--symlink   Follow symbolic links\n"
	    "--version   Show FSAV version number and dates of signature files\n"
	    "\n"
	    "Note: Several action options may be specified, eg. --disinfect --delete\n"
	    "disinfects if possible, otherwise deletes the file.\n"
        );
    exit(FSAV_EXIT_NORMAL);
}


void print_banner(int argc, char** argv)
{
    printf("F-Secure Anti-Virus for Linux %s %u.%02u build %u\n",
	   FPIRUN_MODE, (unsigned)FPIRUN_MAJOR, (unsigned)FPIRUN_MINOR, (unsigned)FPIRUN_BUILD);
}

void get_options(int argc, char** argv)
{
    int     i;

    if (argc <= 1)		// if no options given, just show help
	usage();

    options.ask = TRUE;
    options.action = FPIACTION_NONE;
    options.max_recursion_arc = 0;
    options.max_recursion_dir = 100;
    options.max_time = 30;
    options.follow_symlinks = FALSE;

    // Default settings for interest
    options.interest_errors         = TRUE;
    options.interest_infections     = TRUE;
    options.interest_time           = FALSE;
    options.interest_time_limit     = 0.0;
    options.interest_directories    = FALSE;
    options.interest_filenames      = FALSE;

    for (i=1; i<argc; i++)
    {
        if (strncmp(argv[i], "--", 2) == 0)
        {
	    const char* arg = argv[i]+2;
	    
	    if (strcmp(arg, "archive") == 0)
	    {
		options.max_recursion_arc = 4;
	    }
	    else if (strcmp(arg, "auto") == 0)
	    {
	        options.ask = FALSE;
	    }
	    else if (strcmp(arg, "delete") == 0)
	    {
		if (options.action == FPIACTION_DISINFECT
		    || options.action == FSAV_ACTION_RENAME)
		{
		    invalid_option("multiple action not implemented yet");
		}
		options.action = FSAV_ACTION_DELETE;
	    }
	    else if (strcmp(arg, "disinf") == 0)
	    {
		if (options.action == FSAV_ACTION_DELETE
		    || options.action == FSAV_ACTION_RENAME)
		{
		    invalid_option("multiple action not implemented yet");
		}
		options.action = FPIACTION_DISINFECT;
	    }
	    else if (strcmp(arg, "dumb") == 0)
	    {
	    }
	    else if (strcmp(arg, "extensions") == 0)
	    {
		// TODO: correct parsing (= sign)
	    }
	    else if (strcmp(arg, "help") == 0)
	    {
		usage();
	    }
	    else if (strcmp(arg, "list") == 0)
	    {
	        options.interest_filenames = TRUE;
		options.short_output = FALSE;
	    }
	    else if (strcmp(arg, "rename") == 0)
	    {
		if (options.action == FPIACTION_DISINFECT
		    || options.action == FSAV_ACTION_DELETE)
		{
		    invalid_option("multiple action not implemented yet");
		}
		options.action = FSAV_ACTION_RENAME;
	    }
	    else if (strcmp(arg, "short") == 0)
	    {
		options.short_output = TRUE;
		options.interest_filenames = FALSE;
	    }
	    else if (strcmp(arg, "silent") == 0)
	    {
	    }
	    else if (strcmp(arg, "symlink") == 0)
	    {
		options.follow_symlinks = TRUE;
	    }
	    else if (strcmp(arg, "version") == 0)
	    {
		options.version_only = TRUE;
	    }
	    else
	    {
	        invalid_option(argv[i]);
	    }
        }
    }
}


void get_interest_option(const char* arg)
{
    for (int i=2; arg[i]; i++)
    {
        switch (arg[i])
        {
        case 'e':
            options.interest_errors = TRUE;
            break;
        case 'i':
            options.interest_infections = TRUE;
            break;
        case 't':
            options.interest_time = TRUE;
            if (!isdigit(arg[i+1]))
                invalid_option(arg);
            options.interest_time_limit = atof(&arg[i+1]);
            return;         // Note: No more option letters allowed in this -I option.
            break;
        case 'd':
            options.interest_directories = TRUE;
            break;
        case 'f':
            options.interest_filenames = TRUE;
            break;
        case 'c':
            options.interest_cleanfiles = TRUE;
            break;
        default:
            printboth("invalid option %s\n", arg);
            invalid_option(arg);
            break;
        }
    }
}

#else

void invalid_option(const char* arg)
{
    printboth("Invalid option %s\n\n", arg);
	printboth("Try FPIRUN -? for help\n");
    exit(FSAV_EXIT_ABNORMAL);
}

void usage()
{
    fprintf(stderr, "usage: fpirun [options] files/directories [> logfile]\n"
        "\n"
        "options:\n"
        "-?\tShow this help\n"
        "-v\tVerbose output\n"
		"-s\tTerse output\n"
        "-d\tDisinfect\n"
        "-a\tAsk before disinfecting\n"
        "-f\tDisplay full message for infections, as reported by FPI module\n"
            "\t(not only virus name)\n"
        "-bN\tScan boot block of logical drive N (starting from 0)\n"
        "-mN\tScan MBR of physical hard drive N (starting from 0)\n"
        "-M\tScan memory for resident viruses\n"
        "-rN\tMax depth of archive recursion (0 = no recursion) (default = 7)\n"
        "-RN\tMax depth of directory recursion (0 = no recursion) (default = 100)\n"
		"-tN\tMax time (seconds) for scanning a single component file (0 = no limit) (default = 60)\n"
		"-c\tWrite FTP control files (F-PROT only)\n"
        "-Lxxx\tWrite log to file xxx (default is stdout)\n"
        "-Fxxx\tUse DLL file xxx as the FPI module (default is " DEFAULT_FPI_MODULE ")\n"
        "\t-F-\tDo not use any FPI module (just open and close the files)\n"
        "-Cxxx\tCollect interesting samples to directory xxx\n"
        "-Sxxx\tPrint extra statistics into file xxx\n"
        "-Eext,ext,ext,...\tScan only files with specified extensions.\n"
        "\tSpecial case: just \"-E\" specifies %s\n"
		"-Vxxx\tValidate a database update in directory xxx\n"
        "-I\tDefine which samples are interesting (for collecting and logging):\n"
        "\t-Ie\tErrors in scanning\n"
        "\t-Ii\tInfections\n"
        "\t-ItN\tScanning time more than n seconds (e.g. -It5)\n"
        "\t-Id\tDirectory names (for logging only)\n"
        "\t-If\tAll file names (for logging only)\n"
        "\t-Ic\tNames of all clean files (for logging only)\n"
        "\tNote: -I options can be merged like -Ieif\n"
        "\tIf no -I options are given, default \"-Ieit1\" is used.\n"
        "\n"
        "-Dxxx\tDebug options:\n"
        "\t-Dt\tTrack file read operations and their timing\n"
        "\t-DT\tMore tracking (keep access count for each file byte)\n"
        "\t-DP\tCall PurifyPrintf before scanning each file\n"
        "\t-Dp\tCall PurifyPrintf before scanning each component file\n"
		"\t-DL\tCall PurifyNewLeaks, PurifyNewInUse and PurifyNewHandlesInUse\n"
		"\t-Dc\tDo not catch exceptions during scanning\n"
		"\t\tbefore each file\n"
#ifdef ENABLE_MEASURE_STACK
        "\t-Ds\tMeasure stack space used in each FPI_ScanFile\n"
#endif
        "\t-Dc\tF-PROT only: \"scan virus collection\", including BOO images\n"
        "\tNote: -D options can be merged like -DPp\n",
        stdextensionfilter
        );
    exit(FSAV_EXIT_NORMAL);
}


void print_banner(int argc, char** argv)
{
    int i;
	if (options.terse)
	{
		printboth("FPIRUN %s\n", FPIRUN_VERSION);
	}
	else
	{
		printboth("FPIRUN %s, built %s\n", FPIRUN_VERSION, __DATE__);
		printboth("Copyright (c) 1998-2001 F-Secure Corporation\n");
		printboth("Command line:");
		for (i=0; i<argc; i++)
		{
			printboth(" ");
			if (strchr(argv[i], ' ') != NULL)
				printboth("\"");
			printboth("%s", argv[i]);
			if (strchr(argv[i], ' ') != NULL)
				printboth("\"");
		}

		struct tm *tmtime;
		time_t t;
		time( &t );
		tmtime = localtime( &t );
		printboth("\nStarted %s\n", asctime(tmtime));
	}
}

void get_options(int argc, char** argv)
{
    int     i;
    BOOL    bInterestSpecified = FALSE;

    options.action = FPIACTION_NONE;
    options.max_recursion_arc = 7;
    options.max_recursion_dir = 100;
    options.max_time = 30;

    for (i=1; i<argc; i++)
    {
        if (argv[i][0] == '-')
        {
            switch (argv[i][1])
            {
			case 's':
				options.terse = TRUE;
				break;
            case 'v':
                options.verbose = TRUE;
                break;
            case 'a':
                options.ask = TRUE;
                break;
            case 'd':
                options.disinfect = TRUE;
                options.action = FPIACTION_DISINFECT;
                break;
            case 'f':
                options.fullmessage = TRUE;
                break;
            case 'r':
                if (argv[i][2] == '\0')
                    invalid_option(argv[i]);
                options.max_recursion_arc = atoi(&argv[i][2]);
                break;
            case 'R':
                if (argv[i][2] == '\0')
                    invalid_option(argv[i]);
                options.max_recursion_dir = atoi(&argv[i][2]);
                break;
            case 't':
                if (argv[i][2] == '\0')
                    invalid_option(argv[i]);
                options.max_time = atof(&argv[i][2]);
                break;
            case 'L':
                if (argv[i][2] == '\0')
                    invalid_option(argv[i]);
                logfilename = &argv[i][2];
                logfile = fopen(logfilename, "w+");
                if (logfile == NULL)
                {
                    printboth("Cannot open log file\n");
                    exit(FSAV_EXIT_ABNORMAL);
                }
                break;
            case 'F':
                if (argv[i][2] == '\0')
                    invalid_option(argv[i]);
                fpifilename = &argv[i][2];
                if (strcmp(fpifilename, "-") == 0)
                    nofpi = TRUE;
                else if (fpifilename[0] == '+')
                {
                    sscanf(fpifilename+1, "%d-%d-%d", &readcount, &readsize, &readlinear);
                    fakeread = TRUE;
                    nofpi = TRUE;
                }
                break;
            case 'C':
                options.collect = TRUE;
                if (argv[i][2] == '\0')
                    invalid_option(argv[i]);
                collectdirname = &argv[i][2];
                break;
            case 'V':
                options.validate = TRUE;
                if (argv[i][2] == '\0')
                    invalid_option(argv[i]);
                validatedirname = &argv[i][2];
                break;
			case 'c':
				options.control = TRUE;
				break;
            case 'S':
                options.statistics = TRUE;
                if (argv[i][2] == '\0')
                    invalid_option(argv[i]);
                statfilename = &argv[i][2];
                statfile = fopen(statfilename, "w+");
                if (statfile == NULL)
                {
                    printboth("Cannot open statistics file\n");
                    exit(FSAV_EXIT_ABNORMAL);
                }
                break;
            case 'E':
                options.filter_extension = TRUE;
                if (argv[i][2] == '\0')
                    extensionfilter = stdextensionfilter;
                else
                    extensionfilter = &argv[i][2];
                break;
            case 'D':
                get_debug_option(argv[i]);
                break;
            case 'I':
                get_interest_option(argv[i]);
                bInterestSpecified = TRUE;
                break;
            case 'm':
            case 'b':
            case 'M':
                // will be handled in the scanning loop
                break;
            case '?':
                usage();
                break;
            default:
		invalid_option(argv[i]);
                break;
            }
        }
    }

    if (!bInterestSpecified)
    {
        // Default settings for interest
        options.interest_errors         = TRUE;
        options.interest_infections     = TRUE;
        options.interest_time           = TRUE;
        options.interest_time_limit     = 1.0;
        options.interest_directories    = FALSE;
        options.interest_filenames      = FALSE;
    }
}

void get_debug_option(const char* arg)
{
    for (int i=2; arg[i]; i++)
    {
        switch (arg[i])
        {
		case 'c':
			options.no_catch = TRUE;
			break;
        case 't':
            options.track_fm = TRUE;
            break;
        case 'T':
            options.track_fm_extra = TRUE;
            break;
        case 'P':
            options.purify_printf_files = TRUE;
            break;
        case 'p':
            options.purify_printf_components = TRUE;
            break;
		case 'L':
			options.purify_leaks = TRUE;
			break;
#ifdef ENABLE_MEASURE_STACK
        case 's':
            options.measure_stack = TRUE;
            break;
#endif
        default:
	    invalid_option(arg);
            break;
        }
    }
}

void get_interest_option(const char* arg)
{
    for (int i=2; arg[i]; i++)
    {
        switch (arg[i])
        {
        case 'e':
            options.interest_errors = TRUE;
            break;
        case 'i':
            options.interest_infections = TRUE;
            break;
        case 't':
            options.interest_time = TRUE;
            if (!isdigit(arg[i+1]))
                invalid_option(arg);
            options.interest_time_limit = atof(&arg[i+1]);
            return;         // Note: No more option letters allowed in this -I option.
        case 'd':
            options.interest_directories = TRUE;
            break;
        case 'f':
            options.interest_filenames = TRUE;
            break;
        case 'c':
            options.interest_cleanfiles = TRUE;
            break;
        default:
	    invalid_option(arg);
            break;
        }
    }
}

#endif // FSAV


static FARPROC GetProcAddress_WithUnderscore(HMODULE hModule, LPCSTR lpProcName)
{
	FARPROC p = NULL;

	p = GetProcAddress(hModule, lpProcName+1);
	if (p==NULL)  // try with underscore prefix
		p = GetProcAddress(hModule, lpProcName);
	return p;
}

#define CONCAT(A,B)     A ## B

#ifdef UNIX
#define LOAD_IT(FUN, NAME, MUST)  \
    (fpifun.FUN = FUN)
#else
#define LOAD_IT(FUN, NAME, MUST)  \
    filename = NAME;\
    if ((fpifun.FUN = (CONCAT(LP, FUN)) GetProcAddress_WithUnderscore(*pHandle, filename)) == NULL && MUST)    \
    {   \
        printboth("fpirun: Function %s not found in file \"%s\"\n", filename+1, fpifilename); \
        return FALSE;   \
    }
#endif


BOOL load_fpi_module(HMODULE* pHandle, const char* filename)
{
#ifndef UNIX
    *pHandle = LoadLibrary(filename);
#if defined _WIN32
    if (*pHandle == NULL)
#elif defined OS2
    if (*pHandle == NULLHANDLE)
#else
    if (*pHandle <= HINSTANCE_ERROR)
#endif
    {
        printboth("fpirun: Cannot load FPI module from file \"%s\"\n", fpifilename);
        return FALSE;
    }
#endif

    LOAD_IT(FPI_GetModuleInformation,   "_FPI_GetModuleInformation",    TRUE);
    LOAD_IT(FPI_Initialize,             "_FPI_Initialize",              TRUE);
    LOAD_IT(FPI_Uninitialize,           "_FPI_Uninitialize",            TRUE);
    LOAD_IT(FPI_ScanFile,               "_FPI_ScanFile",                TRUE);
    LOAD_IT(FPI_ScanBootBlock,          "_FPI_ScanBootBlock",           TRUE);
    LOAD_IT(FPI_ScanMemory,             "_FPI_ScanMemory",              TRUE);
    LOAD_IT(FPI_ValidateDatabases,      "_FPI_ValidateDatabases",       FALSE);
    LOAD_IT(FPI_ValidateDatabases,      "_FPI_ValidateDatabases",       FALSE);
    LOAD_IT(FPI_ReloadDatabases,        "_FPI_ReloadDatabases",         TRUE);



    return TRUE;
}

void unload_fpi_module(HMODULE* pHandle)
{
#ifndef UNIX
#if defined _WIN32
    if (*pHandle != NULL)
#elif defined OS2
    if (*pHandle != NULLHANDLE)
#else
    if (*pHandle > HINSTANCE_ERROR)
#endif
    {
        FreeLibrary(*pHandle);
        *pHandle = NULL;
    }
#endif
}

void init_io_table()
{
    io.fFunctionCount   = 14;       // Must be kept correct if you add or remove functions
    io.fFnGetLastError  = FN_GETLASTERROR,
    io.fFnOpen          = FN_OPEN;
    io.fFnRead          = FN_READ;
    io.fFnWrite         = FN_WRITE;
    io.fFnClose         = FN_CLOSE;
    io.fFnGetSize       = FN_GETSIZE;
    io.fFnGetPos        = FN_GETPOS;
    io.fFnSetPos        = FN_SETPOS;
    io.fFnSetEnd        = FN_SETEND;
    io.fFnGetAttr       = FN_GETATTR;
    io.fFnRWBootBlock   = FN_RWBOOTBLOCK;
    io.fFnRWSectorPhys  = FN_RWSECTORPHYS;
    io.fFnRWSectorLogc  = FN_RWSECTORLOGC;
    io.fFnGetDriveGeometry   = FN_GETDRIVEGEOMETRY;
    // Remember to update the function count if you add or remove functions.
}


#ifdef USE_FM

static BOOL StartScan(const char *path, WORD action)
{
    short currNo;
    long size;
    onStack = 0;

    if (bExitScan)
        return FALSE;

    if (!matches_filters(path))
        return FALSE;

#ifdef USE_PURIFY
    if (options.purify_leaks)
	{
		PurifyNewLeaks();
		PurifyNewInuse();
		PurifyNewHandlesInuse();
	}
    if (options.purify_printf_files)
        PurifyPrintf("Scanning %s\n", path);
#endif

	// jmk2000-09-04 {
	// Circumvent FM's bug SCR#9023
	memset(&cStack[0], 0, sizeof cStack[0]);
	// jmk2000-09-04 }

    // NOTE: path is cast into non-const. Let's hope they won't modify it anyway!
    if (!fmOpenFile(&(cStack[0].hndl), 0, (char*)path, FM_READONLY_ACCESS, FALSE, FM_SHARE_COMPAT,
		    0, NULL, &(cStack[0].fInfo)))
    {
        if (options.interest_errors)
            printlog("%s:\tnot scanned (fmOpenFile failed)\n", path);

        nOtherErrors++;
        return(FALSE);
    }

    fmSeekFile(cStack[0].hndl, 0, FILESEEK_END);
    fmTellFile(cStack[0].hndl, &size);
    fmSeekFile(cStack[0].hndl, 0, FILESEEK_SET);

    // if it's a compound file of supported type - put it onto the stack
    if (options.max_recursion_arc > 0
        && cStack[0].fInfo.ulPackerTypeId != 0
        && fmFindFirstFile(&(cStack[0].sHndl), cStack[0].hndl, &(cStack[0].fData)) == TRUE)
    {
        cStack[0].fThisName = path;
        cStack[0].found = TRUE;
        onStack = 1;
    }
    else
    {
        // NOTE: path is cast into non-const. Let's hope they won't modify it anyway!
        cStack[0].fThisName = path;
        fmCloseFile(cStack[0].hndl);
        ProcessEntry(0, path, action);
    }

    while (onStack != 0)
    {
        if (bExitScan)
            return FALSE;

        // number of container to be processed at this pass of the "while" loop
        currNo = onStack - 1;

        // check if the container has still some entries to be processed
        if (cStack[currNo].found == FALSE)   {
            // nothing to do - close its "search", process it, and pop it from the stack
            onStack--;
            fmFindClose(cStack[currNo].sHndl);
            fmCloseFile(cStack[currNo].hndl);
            cStack[currNo].fThisName = (currNo == 0 ? path : cStack[currNo-1].fData.pszFileName);
            ProcessEntry((currNo == 0 ? 0 : cStack[currNo-1].hndl),
                cStack[currNo].fThisName,
                action);
        }
        else
        {
            if (fmOpenFile(&(cStack[onStack].hndl), cStack[currNo].hndl,
                cStack[currNo].fData.pszFileName,
                FM_READONLY_ACCESS, FALSE, FM_SHARE_DENYWR, 0,
                NULL, &(cStack[onStack].fInfo)) == TRUE)
            {
                if (cStack[onStack].fInfo.ulPackerTypeId != 0 &&
                    onStack < options.max_recursion_arc &&
                    (cStack[onStack].found = fmFindFirstFile(
                    &(cStack[onStack].sHndl), cStack[onStack].hndl,
                    &(cStack[onStack].fData))) == TRUE)
                {
                    // put it onto the stack
                    cStack[onStack].fThisName = cStack[onStack-1].fData.pszFileName;
                    onStack++;
                }
                else
                {
                    fmCloseFile(cStack[onStack].hndl);
#ifdef USE_PURIFY
                    if (options.purify_printf_components)
                        PurifyPrintf("Scanning component %s\n", cStack[currNo].fData.pszFileName);
#endif
                    cStack[onStack].fThisName = cStack[currNo].fData.pszFileName;
                    ProcessEntry(cStack[currNo].hndl,
                        cStack[currNo].fData.pszFileName,
                        action);
                }
            }

            // move forward inside the container we started this pass of the "while"
            // loop with
            cStack[currNo].found = 
                fmFindNextFile(cStack[currNo].sHndl, &(cStack[currNo].fData));
        }
    }

    return(TRUE);
}


static BOOL ProcessEntry(HFMFILE hfmParent, const char *name, WORD action)
{
    int len = strlen(name);
    if (len > 0
        && (name[len-1] == '\\'
            || name[len-1] == '/'))
    {
        return FALSE;   // Just skip it, it's a directory name
    }

    BOOL scanned = scan_file(name, hfmParent, action);

    if (scanned)
    {
        if (hfmParent==0)
            nFiles++;
        else
            nFilesInArchives++;

        if (isFileClean)
            nFilesClean++;
        else if (isFileFailed)
            nFilesFailed++;
        else if (isFileInfected)
            nFilesInfected++;
        else if (isFileSuspicious)
            nFilesSuspicious++;

        if (isFileDisinfected)
            nFilesDisinfected++;
        if (isFileDisinfectFailed)
            nFilesDisinfectFailed++;
    }

    return scanned;
}

#endif


BOOL touchfile(const char* name, FPICONTAINERHANDLE chandle)
{
    FPIFILEHANDLE handle;
    handle = FN_OPEN(chandle, name, FPIFILEIO_READ, 0, 0, 0);
    if (handle == FPIFILEHANDLE_ERROR)
        return FALSE;

    if (!FN_CLOSE(handle))
        return FALSE;

    return TRUE;
}

BOOL touchfile(const char* name, FPICONTAINERHANDLE chandle, int readcount, int readsize, int readlinear)
{
    FPIFILEHANDLE handle;
    handle = FN_OPEN(chandle, name, FPIFILEIO_READ, 0, 0, 0);
    if (handle == FPIFILEHANDLE_ERROR)
        return FALSE;

    char* buffer = new char[readsize];
    DWORD bytesread;

    DWORD filesize = FN_GETSIZE(handle);
    
    for (int i=0; i<readcount; i++)
    {
        if (!readlinear)
        {
            LONG pos = ((rand() << 16) + rand()) % (int)filesize;
            FN_SETPOS(handle, pos, FPIFILEIO_BEGIN);
        }
        FN_READ(handle, buffer, readsize, &bytesread);
    }

    delete [] buffer;

    if (!FN_CLOSE(handle))
        return FALSE;

    return TRUE;
}

void get_name_from_refdata(DWORD refdata, char* buffer, int buffersize)
{
    if ((refdata & REFDATA_BOOT) || (refdata & REFDATA_MBR)
        || (refdata & REFDATA_MEMORY) || (refdata & REFDATA_VALIDATEDATABASES)
        || refdata==FPIREFDATA_GETMODULEINFORMATION
        || refdata==FPIREFDATA_INITIALIZE
        || refdata==FPIREFDATA_UNINITIALIZE)
    {
        get_fullname_from_refdata(refdata, buffer, buffersize);
        return;
    }

#ifdef USE_FM
    const char* name = cStack[refdata - REFDATA_FILE].fThisName;
    strmaxcpy(buffer, name, buffersize-1);
#else
    strmaxcpy(buffer, filename, buffersize-1);
#endif
}

DWORD get_ticks_from_refdata(DWORD refdata)
{
#ifdef USE_FM
	if (refdata >= REFDATA_FILE	&& refdata <= REFDATA_FILE + 0x100)
		return cStack[refdata - REFDATA_FILE].tickStart;
	else
		return 0;
#else
	return 0;
#endif
}

void strcatmax(char* buffer, int buffersize, int* plen, const char* add)
{
    int addlen = strlen(add);
    addlen = min(addlen, buffersize-1-*plen);
    if (addlen > 0)
    {
        memcpy(buffer + *plen, add, addlen);
        buffer[*plen + addlen] = '\0';
        *plen += addlen;
    }
}

void get_fullname_from_refdata(DWORD refdata, char* buffer, int buffersize)
{
    int len = 0;
    buffer[0] = '\0';

    if (refdata == FPIREFDATA_GETMODULEINFORMATION)
    {
        strmaxcpy(buffer, "\nFPI_GetModuleInformation", buffersize);
        return;
    }
    else if (refdata == FPIREFDATA_INITIALIZE)
    {
        strmaxcpy(buffer, "\nFPI_Initialize", buffersize);
        return;
    }
    else if (refdata == FPIREFDATA_UNINITIALIZE)
    {
        strmaxcpy(buffer, "\nFPI_Uninitialize", buffersize);
        return;
    }
    if (refdata & REFDATA_BOOT)
    {
        char temp[20];
        sprintf(temp, "BootBlock 0x%lX",
            (refdata & 0xFF));
        strmaxcpy(buffer, temp, buffersize);
        return;
    }
    else if (refdata & REFDATA_MBR)
    {
        char temp[20];
        sprintf(temp, "MBR 0x%lX",
            (refdata & 0xFF));
        strmaxcpy(buffer, temp, buffersize);
        return;
    }
    else if (refdata & REFDATA_MEMORY)
    {
        strmaxcpy(buffer, "memory", buffersize);
        return;
    }
    else if (refdata & REFDATA_VALIDATEDATABASES)
    {
        strmaxcpy(buffer, "db validation", buffersize);
        return;
    }

#ifdef USE_FM
    for (DWORD i=0; i<=refdata-REFDATA_FILE; i++)
    {
        const char* name = cStack[i].fThisName;

        if (i > 0)
            strcatmax(buffer, buffersize, &len, " ");

        if (i < refdata-REFDATA_FILE && len+1+1)
            strcatmax(buffer, buffersize, &len, "[");

        strcatmax(buffer, buffersize, &len, name);

        if (i < refdata-REFDATA_FILE)
            strcatmax(buffer, buffersize, &len, "]");
    }
#else
    strmaxcpy(buffer, filename, buffersize-1);
#endif
}

FPIFILEHANDLE get_handle_from_refdata(DWORD refdata)
{
    if ((refdata & REFDATA_BOOT) || (refdata & REFDATA_MBR)
        || (refdata & REFDATA_MEMORY) || (refdata & REFDATA_VALIDATEDATABASES))
        return FPIFILEHANDLE_ERROR;

#ifdef USE_FM
    if (refdata > (DWORD)(int)onStack)
        return FPIFILEHANDLE_ERROR;
    return cStack[refdata-REFDATA_FILE].hndl;
#else
    return FPIFILEHANDLE_ERROR;
#endif
}

// strmaxcpy:
// Copy at most COUNT characters from STRING2 to STRING1,
// then append the zero byte.
// Note: This means STRING1 must have room for COUNT+1 characters.
// Note: This function is quite different from strncpy.
char* strmaxcpy(char *string1, const char *string2, size_t count)
{
    string1[0] = '\0';
    return strncat(string1, string2, count);
}


#ifdef ENABLE_MEASURE_STACK

void BeginMeasureStackUsage(const char* name)
{
    unsigned char buffer[MEASURE_STACK_MAX];
    int i;

    for (i=0; i<MEASURE_STACK_MAX; i++)
        buffer[i] = 0xDF;
}

void EndMeasureStackUsage(const char* name)
{
    unsigned char buffer[MEASURE_STACK_MAX];
    int i;

    for (i=0; i<MEASURE_STACK_MAX; i++)
    {
        if (buffer[i] != 0xDF)
            break;
    }

    i = MEASURE_STACK_MAX - i;
    printboth("%s: Max stack usage was %s%d bytes\n", name, (i >= MEASURE_STACK_MAX ? "more than " : ""), i);

    if (i > biggestStackUsage)
        biggestStackUsage = i;
}

#endif // ENABLE_MEASURE_STACK

#ifdef UNIX

// currently this is a non-standard extension to FPI API
// but something like this will probably be implemented in the official FPI API
extern "C" void FPIFN FPI_SetProgramDirectory(const char* path_);

void set_prog_dir_by_arg0(const char* arg0)
{
    char path[500+2];
    char newpath[500+2];
    int ret;

    strcpy(path, arg0);

    while ((ret = readlink(path, newpath, 500)) != -1)
    {
	newpath[ret] = '\0';	// null-terminate it
	// printf("%s -> %s\n", path, newpath);
	strcpy(path, newpath);
    }

    // remove name part
    char* last_slash = strrchr(path, '/');
    if (last_slash == NULL)
	path[0] = '\0';
    else
	last_slash[1] = '\0';	// keep last slash

    ret = strlen(path);
    if (ret > 0 && path[ret-1] != '/')
	strcat(path, "/");

    // printf("finally, path = %s\n", path);

    FPI_SetProgramDirectory(path);
}

void check_root()
{
    uid_t euid = geteuid();
    if (euid != 0)
    {
	printf(
	    "\n"
	    "Not running as superuser. F-Secure Anti-Virus may not be able to scan\n"
	    "or disinfect some of the files. To learn how to run as superuser, consult\n"
	    "the manual in section XXX.\n\n");
    }
}

BOOL is_link(const char* name)
{
    char temp[500];

    return readlink(name, temp, sizeof temp) != -1;
}

#endif
