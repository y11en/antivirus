// fpirun.h

extern "C" {
#include "fpiapi.h"
}

#if 0
#define TRACK_FM
#endif

#ifdef UNIX
#include "myunix.h"
#endif

#ifdef UNIX
typedef void* HMODULE;
#define TRUE 1
#define FALSE 0
#define GetTickCount() (time(NULL) * 1000)
#endif

#ifndef min
#define min(a,b) ((a) < (b) ? (a) : (b))
#endif

// FPI callback

BOOL  FPIFN FN_CALLBACK (WORD /*event_*/, WORD /*param_*/, DWORD /*flags_*/, LPCSTR /*message_*/, DWORD /*refdata_*/);

// FPI file and sector I/O callback functions

WORD  FPIFILEIOFN FN_GETLASTERROR (void);
FPIFILEHANDLE  FPIFILEIOFN FN_OPEN (FPICONTAINERHANDLE /*parent_*/, LPCSTR /*filename_*/, DWORD /*accessmode_*/, DWORD /*reservedA_*/, DWORD /*reservedB_*/, DWORD /*reservedC_*/);
BOOL  FPIFILEIOFN FN_READ (FPIFILEHANDLE /*handle_*/, LPVOID /*buf_*/, DWORD /*bytestoread_*/, LPDWORD /*bytesread_*/);
BOOL  FPIFILEIOFN FN_WRITE (FPIFILEHANDLE /*handle_*/, LPCVOID /*buf_*/, DWORD /*bytestowrite_*/, LPDWORD /*byteswritten_*/);
BOOL  FPIFILEIOFN FN_CLOSE (FPIFILEHANDLE /*handle_*/);
DWORD  FPIFILEIOFN FN_GETSIZE (FPIFILEHANDLE /*handle_*/);
DWORD  FPIFILEIOFN FN_GETPOS (FPIFILEHANDLE /*handle_*/);
DWORD  FPIFILEIOFN FN_SETPOS (FPIFILEHANDLE /*handle_*/, LONG /*distance_*/, WORD /*fromwhere_*/);
BOOL  FPIFILEIOFN FN_SETEND (FPIFILEHANDLE /*handle_*/);
DWORD FPIFILEIOFN FN_GETATTR (FPIFILEHANDLE /*handle_*/);
BOOL  FPIFILEIOFN FN_RWBOOTBLOCK (WORD /*drive_*/, WORD /*blocktype_*/, BOOL /*write_*/, LPVOID /*buf_*/);
BOOL  FPIFILEIOFN FN_RWSECTORPHYS (WORD /*physdrive_*/, WORD /*head_*/, WORD /*cylinder_*/, WORD /*physsector_*/, BOOL /*write_*/, LPVOID /*buf_*/, WORD /*size_*/);
BOOL  FPIFILEIOFN FN_RWSECTORLOGC (WORD /*drive_*/, DWORD /*sector_*/, BOOL /*write_*/, LPVOID /*buf_*/, WORD /*size_*/);
BOOL  FPIFILEIOFN FN_GETDRIVEGEOMETRY (WORD /*drive_*/, LPFPI_DISK_GEOMETRY /*geom_*/);


extern BOOL bIgnoreOle;
extern BOOL bExitScan;
extern long nDirectories;
extern long nFiles;
extern long nFilesInArchives;
extern long nInfections;
extern long nSuspicious;
extern long nDisinfections;
extern long nScanErrors;
extern long nApiErrors;
extern long nOtherErrors;
extern long nCollected;
extern DWORD tickStart;
extern DWORD tickEnd;
extern DWORD ticks;
extern DWORD tickStartFile;
extern DWORD maxTicksFile;
extern DWORD sumTicksInfected;
extern DWORD sumTicksSuspicious;

extern long nFilesClass[];
extern DWORD ticksMaxClass[]; // upper bounds of each counting bin

extern char status[200];

struct fpirunoptions
{
    BOOL    short_output;		/* short output (show infected file names only) */

	BOOL	terse;			    // terse output -- reduce banners etc.
    BOOL    verbose;
    BOOL    disinfect;
    BOOL    ask;
    BOOL    fullmessage;
    WORD    action;

    BOOL    interest_errors;
    BOOL    interest_infections;
    BOOL    interest_time;
    double  interest_time_limit;
    BOOL    interest_directories;
    BOOL    interest_filenames;
    BOOL    interest_cleanfiles;

    BOOL    collect;
    BOOL    statistics;

    BOOL    track_fm;
    BOOL    track_fm_extra;
    BOOL    purify_printf_files;
    BOOL    purify_printf_components;
	BOOL	purify_leaks;
    BOOL    measure_stack;
	BOOL	no_catch;
	BOOL	control;
	BOOL	validate;

    BOOL    filter_extension;

    int     max_recursion_dir;
    int     max_recursion_arc;
    double  max_time;

    BOOL    version_only;
    BOOL    follow_symlinks;
};

extern struct fpirunoptions options;
// extern WORD action;

extern const char* fpierror[];


extern const char* fpifilename;
extern HMODULE fpi;

extern const char* logfilename;
extern FILE* logfile;

extern const char* collectdirname;

extern const char* statfilename;
extern FILE* statfile;

// extern const char* file_being_scanned;

extern const char* extensionfilter;


int printstatus(const char* format, ...);
int printlog(const char* format, ...);
int printboth(const char* format, ...);

void updatestatus(const char* name);

int mymain(int argc, char** argv);
int begin_scanning();
int do_scanning(int argc, char** argv);
int end_scanning();
void scan_it(const char* name, WORD action, int depth);
BOOL scan_file(const char* name, FPICONTAINERHANDLE chandle, WORD action);
void scan_directory(const char* name, WORD action, int depth);
BOOL load_fpi_module(HMODULE* pHandle, const char* filename);
void unload_fpi_module(HMODULE* pHandle);
void init_io_table();
void collect_sample(const char* target, FPICONTAINERHANDLE chandle, const char* fullname, const char* name, FPIFILEHANDLE handle = FPIFILEHANDLE_ERROR);

void get_name_from_refdata(DWORD refdata, char* buffer, int buffersize);
void get_fullname_from_refdata(DWORD refdata, char* buffer, int buffersize);
FPIFILEHANDLE get_handle_from_refdata(DWORD refdata);
DWORD get_ticks_from_refdata(DWORD refdata);

#ifndef _CONSOLE
int winshowstatus(const char* msg);
int winshowlog(const char* msg);
int run_message_loop();
#endif


void add_statistics(const char* fullname, int arclevel, int bin, int nTicks);
void print_statistics();

