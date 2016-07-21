// main.cpp (avsscan)
//
//
#define PR_IMPEX_DEF

#include <string.h>
#include "avsscan.h"

#define IMPEX_IMPORT_LIB
#include <Prague/iface/e_loader.h> 

#if defined(_MSC_VER)
#	pragma warning (disable : 4311 4312)
#endif

#define IMPEX_TABLE_CONTENT
IMPORT_TABLE(import_table )
#include <Prague/iface/e_loader.h> 
IMPORT_TABLE_END

#if defined(_MSC_VER)
#	pragma warning (default : 4311 4312)
#endif

#include <stdio.h>
#include <fcntl.h>
#include <ctype.h>
#ifdef _WIN32
#else
#include <sys/types.h>
#include <sys/stat.h>
#endif
#include <errno.h>

#ifdef KL_PLATFORM_WINDOWS
# define WIN32_LEAN_AND_MEAN
# include <windows.h>
# include <process.h>
# include <direct.h>
# include <io.h>
# define MKDIR(d) _mkdir(d)
#else
# define MKDIR(d) mkdir(d,S_IRWXU)
#endif

#include <klava/klav_direnum.h>

#include <string>
#include <vector>

#include "asprintf.h"

////////////////////////////////////////////////////////////////
// options

bool opt_file_list = false;
const kl_tchar_t * opt_file_list_fname = 0;
const kl_tchar_t * opt_out_dir = 0;

bool opt_recursive = false;
bool opt_sort_order = false;

unsigned int g_opt_flags = 0;

const kl_tchar_t * opt_log_fname = 0;
bool opt_hide_record_numbers = false;
bool opt_hide_date_time = false;
bool opt_log_normalize = false;
bool opt_log_utf8 = false;

bool opt_prague_trace = false;

bool opt_diff_mode = false;
bool opt_prim_klava = false;  // primary DB is KLAVA
bool opt_sec_klava  = false;  // secondary DB is KLAVA
bool opt_use_imul = false;
bool opt_safe_scan = false;

bool opt_mt_scan = false; // multithread scan

const kl_tchar_t * opt_primary_db = 0;
const kl_tchar_t * opt_secondary_db = 0;
const kl_tchar_t * opt_emul_db = 0;

bool opt_link_debug = false;
bool opt_dsut_mode = false;
int  opt_imul_heur_level = 50;
bool opt_memscan = false;
bool opt_detect_packers = false;
bool opt_async_scan = false;
unsigned opt_time_limit = 0;

bool opt_report_statistics = false;

////////////////////////////////////////////////////////////////
// global vars

hROOT g_root = NULL;

KLAV_Malloc_Alloc g_malloc;

static klav_tstring g_current_file;

////////////////////////////////////////////////////////////////
// logger

class Log_File
{
public:
	Log_File () : m_fd (0) {}
	
	~Log_File () 
	{
		close ();
	}

	void attach (FILE *fd)
	{
		close ();
		m_fd = fd;
	}

	FILE * detach ()
	{
		FILE * fd = m_fd;
		m_fd = 0;
		return fd;
	}

	FILE * fd () const
	{
		return m_fd;
	}

	void close ()
	{
		if (m_fd != 0)
		{
			fclose (m_fd);
			m_fd = 0;
		}
	}

private:
	FILE * m_fd;
};

static Log_File g_log_file;

void print_log (const kl_tchar_t * fmt, ...)
{
	kl_tchar_t * log_buf = 0;
	va_list l2;
	va_start (l2, fmt);
	int n = kl_vastprintf (&log_buf, fmt, l2);
	va_end(l2);

	if (n <= 0 || log_buf == 0)
		return;

#if defined(KL_PLATFORM_WINDOWS) 
	OutputDebugString(log_buf);
#endif
	kl_tprintf(KL_TEXT("%s"), log_buf);

	if (g_log_file.fd () != 0)
	{
#if KL_UNICODE != 0
		if (opt_log_utf8)
		{
			klav_string utf_msg;
			utf_msg.assign (log_buf, & g_malloc);

			fputs (utf_msg.c_str (), g_log_file.fd ());
		}
		else
#endif // KL_UNICODE != 0
		{
			kl_ftprintf (g_log_file.fd (), KL_TEXT("%s"), log_buf);
		}
	}

	free(log_buf);
}

static void prague_trace_fn (tSTRING string)
{
	klav_tstring msg (string, & g_malloc);
	print_log (KL_TEXT ("PRAGUE: %s\n"), msg.c_str ());
}

const kl_tchar_t * format_time (kl_tchar_t *timebuf, size_t buflen, struct tm *tm)
{
    kl_sntprintf (timebuf, buflen, KL_TEXT ("%04u-%02u-%02u %02u:%02u:%02u"),
		tm->tm_year + 1900, tm->tm_mon + 1, tm->tm_mday,
		tm->tm_hour, tm->tm_min, tm->tm_sec);
	return timebuf;
}

const kl_tchar_t * format_time (kl_tchar_t *timebuf, size_t buflen, time_t t)
{
	return format_time (timebuf, buflen, localtime (&t));
}

const kl_tchar_t * format_current_time (kl_tchar_t *timebuf, size_t buflen)
{
	return format_time (timebuf, buflen, time (0));
}

////////////////////////////////////////////////////////////////

class AVS_Dir_Scanner : public KLAV_Process_File_Callback
{
public:
	AVS_Dir_Scanner (AVS_Scanner *scanner, AVS_Scanner *sec_scanner,
		const klav_string& out_dir, bool dsut = false);

	virtual ~AVS_Dir_Scanner ();

	KLAV_ERR KLAV_CALL process_file (const char *path);
	KLAV_ERR KLAV_CALL process_running_processes();

	tERROR msg_handler (
			tDWORD msg_cls,
			const cDetectObjectInfo *info,
			bool bPrimary
		);

	tERROR obj_handler (
				cIO* io,
				const cObjectInfo *info,
				bool bPrimary
		);

	tERROR save_content(cIO* io, const char *fullname, bool bPrimary, bool bCured);
	tERROR log_io_info(cIO* io);

	void set_name_prefix_len (size_t len)
		{ m_name_prefix_len = len; }

	const kl_tchar_t * current_file_name () const
		{ return g_current_file.c_str (); }

	const kl_tchar_t * current_object_name () const
		{ return m_object_name.c_str (); }

private:
	AVS_Scanner * m_scanner1;
	AVS_Scanner * m_scanner2;

	AVS_Scanner::Scan_Options m_options1;
	AVS_Scanner::Scan_Options m_options2;
	AVS_Scanner::Scan_Results m_results1;
	AVS_Scanner::Scan_Results m_results2;

	struct Object_Event
	{
		Object_Event(const cDetectObjectInfo& i)
		{
			m_status = i.m_nObjectStatus;
			m_name = i.m_strObjectName.c_str(cCP_ANSI);
			m_detect = i.m_strDetectName.c_str(cCP_ANSI);
			m_errno = i.m_nDescription;
		}

		enObjectStatus m_status;
		std::string    m_name;
		std::string    m_detect;
		uint32_t       m_errno;
	};

	typedef std::vector <Object_Event> Object_Events;
	Object_Events m_events1;
	Object_Events m_events2;
	
	klav_string m_out_dir;	
	klav_tstring m_object_name;
	size_t      m_name_prefix_len;
	bool        m_dsut_mode;

	void clear_events ()
	{
		m_events1.clear ();
		m_events2.clear();
	}

	void print_event (size_t pos, const Object_Event& evt);
	void print_events (const Object_Events& events);
	uint32_t get_last_errno (const Object_Events& events);

	static tERROR primary_msg_handler_fn (
			void * handler_context,
			tDWORD msg_cls,
			const cDetectObjectInfo *info
		)
	{
		AVS_Dir_Scanner * scanner = (AVS_Dir_Scanner *) handler_context;
		return scanner->msg_handler (msg_cls, info, true);
	}

	static tERROR secondary_msg_handler_fn (
			void * handler_context,
			tDWORD msg_cls,
			const cDetectObjectInfo *info
		)
	{
		AVS_Dir_Scanner * scanner = (AVS_Dir_Scanner *) handler_context;
		return scanner->msg_handler (msg_cls, info, false);
	}

	static tERROR primary_obj_handler_fn (
				void * handler_context,
				cIO* io,
				const cObjectInfo *info
			)
	{
		AVS_Dir_Scanner * scanner = (AVS_Dir_Scanner *) handler_context;
		return scanner->obj_handler(io, info, true);
	}

	static tERROR secondary_obj_handler_fn (
				void * handler_context,
				cIO* io,
				const cObjectInfo *info
			)
	{
		AVS_Dir_Scanner * scanner = (AVS_Dir_Scanner *) handler_context;
		return scanner->obj_handler(io, info, false);
	}

	typedef void (AVS_Dir_Scanner::*Object_Processor)();
	KLAV_ERR process_current_object (Object_Processor object_processor1, Object_Processor object_processor2);

	void do_process_file1 ();
	void do_process_file2 ();

	void do_process_running_processes1 ();
	void do_process_running_processes2 ();

#ifdef KL_PLATFORM_WINDOWS
	static unsigned int __stdcall thread_fn_2 (void *ctx)
	{
		AVS_Dir_Scanner * self = (AVS_Dir_Scanner *) ctx;
		self->do_process_file2 ();
		return 0;
	}
#endif // KL_PLATFORM_WINDOWS
};

AVS_Dir_Scanner::AVS_Dir_Scanner (
		AVS_Scanner *scanner,
		AVS_Scanner *sec_scanner,
		const klav_string& out_dir, 
		bool dsut_mode
	) :
		m_scanner1 (scanner),
		m_scanner2 (sec_scanner),
		m_out_dir (out_dir),
		m_name_prefix_len (0),
		m_dsut_mode (dsut_mode)
{
	if (m_out_dir.length () != 0 || m_dsut_mode)
		m_scanner1->set_obj_handler (this, primary_obj_handler_fn);

	if (m_scanner2 != 0) // using diff mode
	{
		m_scanner1->set_msg_handler (this, primary_msg_handler_fn);
		m_scanner2->set_msg_handler (this, secondary_msg_handler_fn);
		m_scanner2->set_obj_handler (this, secondary_obj_handler_fn);
	}
}

AVS_Dir_Scanner::~AVS_Dir_Scanner ()
{
}

extern const kl_tchar_t* get_status_str(enObjectStatus status, const enDetectStatus* dstatus);

uint32_t AVS_Dir_Scanner::get_last_errno (const Object_Events& events)
{
	uint32_t ec = 0;
	size_t i, cnt = events.size ();
	for (i = 0; i < cnt; ++i)
	{
		const Object_Event& evt = events [i];
		if (evt.m_status == OBJSTATUS_PROCESSING_ERROR)
			ec = evt.m_errno;
	}
	return ec;
}

void AVS_Dir_Scanner::print_event (size_t pos, const Object_Event& evt)
{
	kl_ftprintf (stdout, KL_TEXT ("> %u:\tstatus(%s)\tname(%s)\tobject(%s)\n"),
				(unsigned) pos,
				get_status_str (evt.m_status, NULL),
				klav_tstring(evt.m_detect.data (), &g_malloc).c_str (),
				klav_tstring(evt.m_name.data(), &g_malloc).c_str ()
			);
	fflush(stdout); 
}

void AVS_Dir_Scanner::print_events (const Object_Events& events)
{
	size_t i, cnt = events.size ();
	for (i = 0; i < cnt; ++i)
		print_event(i, events [i]);
}

KLAV_ERR KLAV_CALL AVS_Dir_Scanner::process_file (const char *path)
{
	g_current_file.assign (path, & g_malloc);
	m_object_name.assign (path + m_name_prefix_len, &g_malloc);
	return process_current_object(&AVS_Dir_Scanner::do_process_file1, &AVS_Dir_Scanner::do_process_file2);
}

KLAV_ERR KLAV_CALL AVS_Dir_Scanner::process_running_processes ()
{
	g_current_file.assign ("<MEMSCAN>", & g_malloc);
	m_object_name.assign ("<MEMSCAN>", &g_malloc);
	return process_current_object(&AVS_Dir_Scanner::do_process_running_processes1, &AVS_Dir_Scanner::do_process_running_processes2);
}

KLAV_ERR AVS_Dir_Scanner::process_current_object(Object_Processor object_processor1, Object_Processor object_processor2)
{
	if (opt_log_normalize)
	{
		kl_tchar_t * obj_name_buf = (kl_tchar_t *) (m_object_name.c_str ());

		// 'normalize' object name for tests
		for (kl_tchar_t *p = obj_name_buf; *p != 0; ++p)
		{
			if (*p == '\\')
				*p = '/';
		}
	}

	m_options1.clear ();
	m_options2.clear ();
	m_results1.clear ();
	m_results2.clear ();
	
	m_options1.m_opt_flags  = g_opt_flags;
	m_options1.m_heur_level = opt_imul_heur_level;
	m_options1.m_time_limit = opt_time_limit;

	m_options2.m_opt_flags  = g_opt_flags;
	m_options2.m_heur_level = opt_imul_heur_level;
	m_options2.m_time_limit = opt_time_limit;

	if (m_dsut_mode)
		m_options1.m_opt_flags |= AVS_Scanner::OPT_DSUT_MODE;

	if( opt_async_scan )
		m_options1.m_opt_flags |= AVS_Scanner::OPT_ASYNC_SCAN;

	bool diff_mode = (m_scanner2 != 0);

#ifdef KL_PLATFORM_WINDOWS
	HANDLE h_thread_2 = 0;
#endif

	if (diff_mode)
	{
#ifdef KL_PLATFORM_WINDOWS
		if (opt_mt_scan)
		{
			unsigned int thread_id = 0;
			h_thread_2 = (HANDLE) _beginthreadex (0, 0, thread_fn_2, this, 0, & thread_id);
			if (h_thread_2 == 0)
				(this->*object_processor2) ();
		}
		else
#endif // KL_PLATFORM_WINDOWS
		{
			(this->*object_processor2) ();
		}

//		kl_ftprintf (stdout, KL_TEXT("\n"));
	}

	(this->*object_processor1) ();

#ifdef KL_PLATFORM_WINDOWS
	if (diff_mode && opt_mt_scan && h_thread_2 != 0)
	{
		WaitForSingleObject (h_thread_2, INFINITE);
		CloseHandle (h_thread_2);
	}
#endif // KL_PLATFORM_WINDOWS

	tERROR master_err = m_results1.m_error;

	if (m_dsut_mode)
		return master_err;

	if (! diff_mode)
	{
		if (PR_FAIL (m_results1.m_error))
		{
			kl_ftprintf (stdout, KL_TEXT ("%s\terror\t(%s)\n"), current_object_name (), m_results1.m_error_text.c_str ());
		}
		else
		{
			if (m_results1.m_detected)
			{
				if ((m_options1.m_opt_flags & AVS_Scanner::OPT_CURE_TEST) != 0)
				{
					print_log (KL_TEXT ("%s\tinfected\t%s\t(%s)\n"),
							current_object_name (),
							m_results1.m_detect_name.c_str (),
							m_results1.m_disinfected ? KL_TEXT("disinfected") : KL_TEXT("not disinfected")
						);
				}
				else
				{
					print_log (KL_TEXT ("%s\tinfected\t%s\n"),
							current_object_name (),
							m_results1.m_detect_name.c_str ()
						);
				}
			}
			else
			{
				print_log (KL_TEXT ("%s\tok\n"), current_object_name ());
			}
		}
	}
	else
	{
		size_t num1 = m_events1.size();
		size_t num2 = m_events2.size();
		size_t num = std::min(num1, num2);

		uint32_t last_ec1 = get_last_errno (m_events1);
		uint32_t last_ec2 = get_last_errno (m_events2);

		bool stack_overflow = (last_ec1 == errSTACK_EMPTY && last_ec2 == errSTACK_EMPTY);

		bool bEqual = true;
		size_t i;
		for(i = 0; i < num; i ++)
		{
			Object_Event& i1 = m_events1[i];
			Object_Event& i2 = m_events2[i];

			if( i1.m_status != i2.m_status || 
				i1.m_name != i2.m_name ||
				i1.m_detect != i2.m_detect )
			{
				bEqual = false;
				break;
			}
		}

		if (stack_overflow)
		{
			kl_ftprintf (stdout, KL_TEXT ("%s:ok (stack overflow)\n"), current_object_name ());
		}
		else
		{
			if( bEqual && (num1 != num2) )
			{
				bEqual = false;
				kl_ftprintf (stdout, KL_TEXT ("%s:!!!different events number\n"), current_object_name ());
			}
			else if( bEqual )
			{
				kl_ftprintf (stdout, KL_TEXT ("%s:ok\n"), current_object_name ());
			}
			else
			{
				kl_ftprintf (stdout, KL_TEXT ("%s:!!!diffs!!!\n"), current_object_name ());
				kl_ftprintf (stdout, KL_TEXT ("> primary engine events (%u):\n"), (unsigned) m_events1.size ()),
				print_event(i, m_events1[i]);
//				print_events (m_primary_events);
				kl_ftprintf (stdout, KL_TEXT ("> secondary engine events (%u):\n"), (unsigned) m_events2.size ()),
				print_event(i, m_events2[i]);
//				print_events (m_secondary_events);
			}
			fflush(stdout);
		}

		clear_events ();
	}

	return master_err;
}

void AVS_Dir_Scanner::do_process_file1 ()
{
	m_scanner1->process_file (current_file_name (), m_options1, m_results1);
}

void AVS_Dir_Scanner::do_process_file2 ()
{
	m_scanner2->process_file (current_file_name (), m_options2, m_results2);
}

void AVS_Dir_Scanner::do_process_running_processes1 ()
{
	m_scanner1->process_running_processes (m_options1, m_results1);
}

void AVS_Dir_Scanner::do_process_running_processes2 ()
{
	m_scanner2->process_running_processes (m_options2, m_results2);
}

tERROR AVS_Dir_Scanner::msg_handler (
			tDWORD msg_cls,
			const cDetectObjectInfo *info,
			bool bPrimary
	)
{
	if( bPrimary )
		m_events1.push_back (Object_Event(*info));
	else
		m_events2.push_back (Object_Event(*info));
	return errOK;
}

bool make_path(std::string sDir)
{
	size_t pos = sDir.find_last_of("\\/");
	if( pos == std::string::npos )
		return false;

	sDir.erase(pos);
	if( !make_path(sDir) )
		return true;

	int ret = MKDIR(sDir.c_str());

	if( !ret || errno == EEXIST )
		return true;

	return false;
}

tERROR AVS_Dir_Scanner::obj_handler(cIO* io, const cObjectInfo *info, bool bPrimary)
{
	if( m_out_dir.length() )
		save_content(io, info->m_strObjectName.c_str(cCP_ANSI), bPrimary, false);

	if( m_dsut_mode )
		log_io_info(io);

	return errOK;
}

tERROR AVS_Dir_Scanner::save_content(cIO* io, const char *path, bool bPrimary, bool bCured)
{
	std::string sPath (path);
	std::string sCurrent(cStringObj(current_file_name ()).c_str(cCP_ANSI));
	size_t pos = sPath.find(sCurrent, 0);
	if( pos != std::string::npos )
		sPath.erase(0, pos+sCurrent.size());

	pos = sCurrent.find_last_of("\\/");
	if( pos != std::string::npos )
		sCurrent.erase(0, pos);


	std::string sSubPath;
	bool bWasSlash = false;
	for(size_t i = 0; i < sPath.size(); i++)
	{
		char c = sPath[i];
		switch(c)
		{
		case '\\':
		case '/':
			if( bWasSlash )
			{
				//				sSubPath.insert(sSubPath.size()-1, ".dir");
				continue;
			}
			bWasSlash = true;
			c = PATH_DELIMETER_CHAR;
			break;

//		case '.':
		case ' ':
		case ',':
		case '[':
		case ']':
		case '<':
		case '>':
		case '+':
		case ':':
		case '\"':
		case '\'':
		case '-':
		case '!':
		case '$':
			c = '_';
		default:
			bWasSlash = false;
		}
		sSubPath += c;
	}
	

	std::string sFullPath(m_out_dir.c_str());
	if( m_scanner2 != 0 ) // using diff mode
		sFullPath += (bPrimary)? PATH_DELIMETER "primary" : PATH_DELIMETER "secondary";
	if (bCured)
		sFullPath += PATH_DELIMETER "cured";
	if(opt_file_list)
	{
		sFullPath += PATH_DELIMETER;
		sFullPath += sCurrent;
	}
	sFullPath += PATH_DELIMETER;
	sFullPath += sSubPath;
	sFullPath += ".unp";

	tERROR error = errOK;
	FILE* f = fopen(sFullPath.c_str(), "wb");
	if( !f )
	{
		if( make_path(sFullPath) )
			f = fopen(sFullPath.c_str(), "wb");
		else
			error = errUNEXPECTED;
	}

	if( f )
	{
		tQWORD size;
		io->GetSize(&size, IO_SIZE_TYPE_EXPLICIT);
		tQWORD offset = 0;
		while(size) {
			char buff [4096] = {0};
			tDWORD count = 0;
			error = io->SeekRead(&count, offset, buff, sizeof(buff));
			if(PR_FAIL(error))
				break;
			fwrite(buff, 1, count, f);
			offset += count;
			size -= count;
		}
		fclose(f);
	}

	return error;
}

tERROR AVS_Dir_Scanner::log_io_info(cIO* io)
{
#ifdef _WIN32
extern bool query_io_info(cIO* io);
	query_io_info(io);
#endif
	return errOK;
}

////////////////////////////////////////////////////////////////

void usage ()
{
	kl_ftprintf (stdout, KL_TEXT ("use avsscan <options> <files to scan...>\n\n"));

	kl_ftprintf (stdout, KL_TEXT ("options are:\n"));
	kl_ftprintf (stdout, KL_TEXT ("   -a[k] <path>     path to primary AVP3/KLAVA database\n"));
	kl_ftprintf (stdout, KL_TEXT ("   -x[k] <path>     diff mode, path to secondary database (AVP3/KLAVA)\n"));
	kl_ftprintf (stdout, KL_TEXT ("   -r[s]            scan sub directories recursively (s - case-sensitive sort by name)\n"));
	kl_ftprintf (stdout, KL_TEXT ("   -l  <log file>   log output into file\n"));
	kl_ftprintf (stdout, KL_TEXT ("   -ln <log file>   log output into UTF-8 file, normalize log\n"));
	kl_ftprintf (stdout, KL_TEXT ("   -lu <log file>   log output into UTF-8 file\n"));
	kl_ftprintf (stdout, KL_TEXT ("   -fl <input file> read list of file names to scan from the given file\n")); 
	kl_ftprintf (stdout, KL_TEXT ("   -fc              read list of file names to scan from stdin\n")); 
	kl_ftprintf (stdout, KL_TEXT ("   -bo              use binary stdout\n")); 
	kl_ftprintf (stdout, KL_TEXT ("   -tp              enable Prague tracing\n"));
	kl_ftprintf (stdout, KL_TEXT ("   -ta              trace AVS events\n"));
	kl_ftprintf (stdout, KL_TEXT ("   -m               multithread scan in diff mode\n")); 
	kl_ftprintf (stdout, KL_TEXT ("   -d               use link debugging\n")); 
	kl_ftprintf (stdout, KL_TEXT ("   -imul <basename> enable IMUL\n"));
	kl_ftprintf (stdout, KL_TEXT ("   -h <level>       heuristic level for emulator (default: 50)\n"));
	kl_ftprintf (stdout, KL_TEXT ("   -q               stop object processing on first detect\n"));
	kl_ftprintf (stdout, KL_TEXT ("   -s               display statistics\n"));
	kl_ftprintf (stdout, KL_TEXT ("   -ct              cure mode (non-invasive, file is cured in temp storage)\n"));
	kl_ftprintf (stdout, KL_TEXT ("   -od <dir>        save intermediate files in the <dir> directory (for -fc option <dir>/<file name>/ )\n"));
	kl_ftprintf (stdout, KL_TEXT ("   -dsut            log information about scaned PE files\n"));
	kl_ftprintf (stdout, KL_TEXT ("   -dobj            log sub-object information (emulator-generated objects)\n"));
	kl_ftprintf (stdout, KL_TEXT ("  --memscan         scan running processes\n"));
	kl_ftprintf (stdout, KL_TEXT ("  --time-limit <s>  per-object time limit in seconds (default: none)\n"));
}

int err_invalid_option (const kl_tchar_t *opt)
{
	kl_ftprintf (stderr, KL_TEXT ("Invalid option: %s\n"), opt);
	usage ();
	return 1;
}

int err_missing_optarg (const kl_tchar_t *opt)
{
	kl_ftprintf (stderr, KL_TEXT ("Missing argument for option: %s\n"), opt);
	usage ();
	return 1;
}

int err_duplicate_option (const kl_tchar_t *opt)
{
	kl_ftprintf (stderr, KL_TEXT ("Duplicate value for option: %s\n"), opt);
	return 1;
}

static size_t get_fname_off (const kl_tchar_t *path)
{
	if (path == 0)
		return 0;

	size_t fname_off = kl_tcslen (path);
	for (; fname_off > 0; --fname_off)
	{
		unsigned int ch = path [fname_off - 1];
		if (ch < 0x128 && KLAV_Fname_Is_Path_Separator ((unsigned char) ch))
			break;
	}
	
	return fname_off;
}

////////////////////////////////////////////////////////////////

int kl_main (int argc, kl_tchar_t *argv[])
{
#if defined(_DEBUG) && defined(_MSC_VER)
	_CrtSetDbgFlag ( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
#endif

	int a;

	setlocale (LC_CTYPE, "");

	for (a = 1; a < argc; ++a)
	{
		const kl_tchar_t *arg = argv [a];
		if (arg [0] != '-')
			break;

		switch (arg [1])
		{
		case 'a':
			if (arg [2] == 'k')
				opt_prim_klava = true;
			else if (arg [2] == 's')
			{
				opt_async_scan = true;
				continue;
			}
			else if (arg [2] != 0)
				return err_invalid_option (arg);

			if (argc - a < 2)
				return err_missing_optarg (arg);
			if (opt_primary_db != 0)
				return err_duplicate_option (arg);

			opt_primary_db = argv [++a];
			break;

		case 'x':
			if (arg [2] == 'k')
				opt_sec_klava = true;
			else if (arg [2] != 0)
				return err_invalid_option (arg);

			if (argc - a < 2)
				return err_missing_optarg (arg);
			if (opt_secondary_db != 0)
				return err_duplicate_option (arg);

			opt_secondary_db = argv [++a];
			opt_diff_mode = true;
			break;

		case 'b':
			if (arg [2] == 'o')
			{
#ifdef KL_PLATFORM_WINDOWS
				::_setmode( _fileno( stdout ), _O_BINARY ); 
#endif
			} else {
				kl_ftprintf (stdout, KL_TEXT ("invalid -bX option: %s\n"), arg);
				return 1;
			}
			break;

		case 'r':
			opt_recursive = true;
			if (arg [2] == 's')
			{
				opt_sort_order = true;
			}
			else if (arg [2] != 0)
			{
				kl_ftprintf (stderr, KL_TEXT ("Invalid -rX option: %s\n"), arg);
				return 1;
			}
			break;

		case 'l':
			if (arg [2] == 'n')
			{
				opt_hide_date_time = true;
				opt_log_normalize = true;
				opt_log_utf8 = true;
			}
			else if (arg [2] == 'u')
			{
				opt_log_utf8 = true;
			}
			else if (arg [2] != 0)
			{
				kl_ftprintf (stderr, KL_TEXT ("Invalid -lX option: %s\n"), arg);
				return 1;
			}
			if (argc - a < 2)
				return err_missing_optarg (arg);
			if (opt_log_fname != 0)
				return err_duplicate_option (arg);
			opt_log_fname = argv [++a];
			break;

		case 'q':
			g_opt_flags |= AVS_Scanner::OPT_STOP_ON_DETECT;
			break;

		case 'c':
			if (arg [2] == 't')
			{
				g_opt_flags |= AVS_Scanner::OPT_CURE_TEST;
			}
			else
			{
				kl_ftprintf (stdout, KL_TEXT ("invalid -cX option: %s\n"), arg);
				return 1;
			}
			break;

		case 's':
			if (arg [2] == 's')
				opt_safe_scan = true;
			else
				opt_report_statistics = true;
			break;

		case 'm':
			opt_mt_scan = true;
			break;

		case 'p':
			opt_detect_packers = true;
			break;

		case 't':
			switch (arg [2])
			{
			case 'a':
				g_opt_flags |= AVS_Scanner::OPT_PRINT_EVENTS;
				break;
			case 'p':
				opt_prague_trace = true;
				break;
			default:
				kl_ftprintf (stdout, KL_TEXT ("invalid -tX option: %s\n"), arg);
				return 1;
			} 
			break;

		case 'i':
			if (kl_tcscmp (arg + 1, KL_TEXT ("imul")) == 0)
			{
				if (argc - a < 2)
					return err_missing_optarg (arg);

				g_opt_flags |= AVS_Scanner::OPT_USE_IMUL;
				opt_use_imul = true;
				opt_emul_db = argv [++a];
			}
			else
			{
				kl_ftprintf (stdout, KL_TEXT ("invalid option: %s\n"), arg);
				return 1;
			}
			break;

		case 'h':
			if (argc - a < 2)
				return err_missing_optarg (arg);

			opt_imul_heur_level = (int) kl_tcstoul (argv [++a], 0, 0);
			g_opt_flags |= AVS_Scanner::OPT_USE_IMUL;
			break;

		case 'f': {
				switch (arg[2])
				{
				case 'c':
					opt_file_list = true;
					opt_file_list_fname = 0;
					break;
				case 'l':
					opt_file_list = true;
					if (argc - a < 2)
						return err_missing_optarg (arg);
					opt_file_list_fname = argv [++a];
					break;
				default:
					kl_ftprintf (stderr, KL_TEXT ("Invalid -fX option: %s\n"), arg);
					usage ();
					return 1;
				}
			}
			break;

		case 'o': 
			if(arg[2] != 'd') {
				kl_ftprintf (stdout, KL_TEXT ("invalid option: %s\n"), arg);
				usage();
				return 1;
			}
			if (argc - a < 2)
				return err_missing_optarg (arg);
			opt_out_dir = argv [++a];
			break;

		case 'd':
			if (kl_tcscmp (arg + 1, KL_TEXT ("dsut")) == 0)
			{
				opt_dsut_mode = true;
			}
			else if (kl_tcscmp (arg + 1, KL_TEXT ("dobj")) == 0)
			{
				g_opt_flags |= AVS_Scanner::OPT_LOG_SUBOBJECTS;
			}
			else if (kl_tcscmp (arg + 1, KL_TEXT ("d")) == 0)
			{
				opt_link_debug = true;
			}
			else
				return err_invalid_option (arg);
			break;

		case  '-':
			if (kl_tcscmp (arg + 2, KL_TEXT ("memscan")) == 0)
			{
				opt_memscan = true;
			}
			else if (kl_tcscmp (arg + 2, KL_TEXT ("time-limit")) == 0)
			{
				if (argc - a < 2)
					return err_missing_optarg (arg);

				opt_time_limit = (unsigned) kl_tcstoul (argv [++a], 0, 0);
			}
			else
			{
				return err_invalid_option (arg);
			}
			break;

		default:
			return err_invalid_option (arg);
		};
	}

	if ((a == argc) && !opt_file_list && !opt_memscan)
	{
		kl_ftprintf (stdout, KL_TEXT ("no files to scan\n"));
		return 0;
	}

	if (opt_log_fname != 0 && opt_log_fname [0] != 0)
	{
		const kl_tchar_t * log_open_mode = opt_log_normalize ? KL_TEXT ("wb") : KL_TEXT ("wt");
		FILE * fd = kl_tfopen (opt_log_fname, log_open_mode);
		if (fd != 0)
			g_log_file.attach (fd);
		else
			kl_ftprintf(stdout, KL_TEXT ("Could not open file for logging, no log will be written: %s\n"), opt_log_fname);
	}

	CPrague prague;
	tERROR error = prague.Init (0);
	if (PR_FAIL (error))
	{
		kl_ftprintf (stdout, KL_TEXT ("error initializing Prague: 0x%08X\n"), error);
		return 0;
	}

	tDWORD anCount = 0;
	if ( PR_FAIL ( error = g_root->ResolveImportTable ( &anCount,import_table,PID_LOADER ) ) )
	{
		kl_ftprintf (stdout, KL_TEXT ("error resolving import: 0x%08X\n"), error);
		return 0;
	}

	int st = 0;

	do {
		AVS_Scanner scanner;
		AVS_Scanner sec_scanner;
		
		if (opt_prague_trace)
		{
			CALL_SYS_PropertySetPtr (g_root, pgOUTPUT_FUNC, (void*) prague_trace_fn);
			CALL_SYS_TraceLevelSet (g_root, tlsALL_OBJECTS, prtNOTIFY, prtMIN_TRACE_LEVEL);
		}

		const kl_tchar_t * primary_db = opt_primary_db;
		const kl_tchar_t * secondary_db = opt_secondary_db;

		if (primary_db == 0)
		{
			kl_ftprintf (stdout, KL_TEXT ("database path not specified, use -a option\n"));
			error = errPARAMETER_INVALID;
			st = 1;
			break;
		}

		// initialize primary scanner
		{
			size_t prim_fname_off = get_fname_off (primary_db);
			klav_tstring prim_db_folder (primary_db, prim_fname_off, & g_malloc);

			cAVSSettings settings;
			settings.m_sBaseFolder = prim_db_folder.c_str ();
			settings.m_sEmulBase = opt_emul_db;

			unsigned int engine_mask = 0;
			
			if( opt_safe_scan )
				settings.m_bSafeScan = cFALSE;

			if (opt_prim_klava)
			{
				engine_mask |= ENGINE_KLAV;
				settings.m_sKlavaBase = primary_db + prim_fname_off;
			}
			else
			{
				engine_mask |= ENGINE_AVP3;
				settings.m_sSetName = primary_db + prim_fname_off;
			}

			if (opt_use_imul)
				engine_mask |= ENGINE_EMUL;

			if (opt_link_debug)
				settings.m_bDebugMode = 1;

			if( !opt_detect_packers )
				settings.m_nPackerTypeMask = 0;

//			settings.m_nStackSizeToCheck = 10;

			error = scanner.init_avs (& settings, engine_mask);
			if (PR_FAIL (error))
			{
				print_log (KL_TEXT ("error initializing scanner (db path: %s)\n"), primary_db);
				st = 1;
				break;
			}
		}

		// initialize secondary scanner
		if (opt_diff_mode)
		{
			size_t sec_fname_off = get_fname_off (secondary_db);
			klav_tstring sec_db_folder (secondary_db, sec_fname_off, & g_malloc);

			cAVSSettings settings;
			settings.m_sBaseFolder = sec_db_folder.c_str ();

			unsigned int engine_mask = 0;
			unsigned int opt_flags = g_opt_flags;

			if (opt_link_debug)
				settings.m_bDebugMode = 1;

//			settings.m_nStackSizeToCheck = 10;

			if (opt_sec_klava)
			{
				engine_mask |= ENGINE_KLAV;
				settings.m_sKlavaBase = secondary_db + sec_fname_off;
			}
			else
			{
				engine_mask |= ENGINE_AVP3;
				settings.m_sSetName = secondary_db + sec_fname_off;
			}

			if (opt_use_imul)
				engine_mask |= ENGINE_EMUL;

			if( !opt_detect_packers )
				settings.m_nPackerTypeMask = 0;

			error = sec_scanner.init_avs (& settings, engine_mask);
			if (PR_FAIL (error))
			{
				print_log (KL_TEXT ("error initializing secondary scanner (db path: %s)\n"), secondary_db);
				st = 1;
				break;
			}
		}

		time_t t_start = time (0);

		AVS_Dir_Scanner dir_scanner (& scanner, (opt_diff_mode ? & sec_scanner : 0),
			klav_string(opt_out_dir, &g_malloc), opt_dsut_mode);

		if (opt_file_list)
		{
			FILE * flist_fd = 0;
			if (opt_file_list_fname == 0)
			{
				flist_fd = stdin;
			}
			else
			{
				flist_fd = kl_tfopen (opt_file_list_fname, KL_TEXT("rb"));
				if (flist_fd == 0)
				{
					kl_ftprintf (stderr, KL_TEXT ("Error opening file list to scan: %s\n"), opt_file_list_fname);
					st = 1;
				}
			}

			unsigned int linbuf_size = 4000;
			char * linbuf = (char *) malloc (linbuf_size);
			if (linbuf == 0)
			{
				kl_ftprintf (stderr, KL_TEXT ("fatal error: not enough memory\n"));
				st = 1;
				break;
			}

			for (;;)
			{
				char * fname = fgets (linbuf, linbuf_size - 1, flist_fd);
				if (fname == 0)
					break;

				fname [linbuf_size - 1] = 0;

				for (size_t len = strlen (fname); len > 0 && isspace (fname [len - 1]); --len)
					fname [len - 1] = 0;
					
				if (fname [0] == 0)
					continue; // empty line - skip

				dir_scanner.process_file (fname);
				kl_tprintf(KL_TEXT("*\n"));
				fflush(stdout);
			}

			free (linbuf);

			if (opt_file_list_fname != 0 && flist_fd != 0)
			{
				fclose (flist_fd);
			}
		}
		else if (opt_memscan)
		{
			dir_scanner.process_running_processes();
		}
		else
		{
			for (; a < argc; ++a)
			{
				const kl_tchar_t *filearg = argv [a];
				klav_string filepath (filearg, &g_malloc);

				if (opt_log_normalize)
					dir_scanner.set_name_prefix_len (filepath.length ());

				error = KLAV_Process_Files (filepath.c_str (), opt_recursive, opt_sort_order, & dir_scanner);
				if (PR_FAIL (error))
				{
					kl_ftprintf (stderr, KL_TEXT ("invalid file path: %s\n"), filearg);
					st = 1;
					break;
				}
			}
		}

		time_t t_end = time (0);

		// print statistics
		if (opt_report_statistics)
		{
			kl_tchar_t timebuf[60];
			const AVS_Scanner::Statistics& stats = scanner.statictics ();
			
			print_log(KL_TEXT ("; --- Statistics ---\n"));
			print_log(KL_TEXT ("; Time Start:\t%s\n"), format_time (timebuf, 60, t_start));
			print_log(KL_TEXT ("; Time Finish:\t%s\n"), format_time (timebuf, 60, t_end));
			print_log(KL_TEXT ("; Completion:\t100%%\n"));
			print_log(KL_TEXT ("; Processed objects:\t%u\n"), stats.cnt_scanned);
			print_log(KL_TEXT ("; Total detected:\t%u\n"), stats.cnt_detected);
			print_log(KL_TEXT ("; Detected exact:\t%u\n"), stats.cnt_detected_exact);
			print_log(KL_TEXT ("; Errors:\t%u\n"), stats.cnt_errors);
			print_log(KL_TEXT ("; ------------------\n"));
		}

	} while (0);

	return st;
}

////////////////////////////////////////////////////////////////
// exception support

#if defined(KL_PLATFORM_WINDOWS)

static LONG WINAPI TopLevelExceptionFilter(EXCEPTION_POINTERS *ep)
{
	static PROCESS_INFORMATION pi;
	static STARTUPINFO si;
	memset(&si, 0, sizeof(si));	si.cb = sizeof(si);

	static SYSTEMTIME st;
	static kl_tchar_t strDumpFileName[MAX_PATH];
	GetLocalTime(&st);
	GetModuleFileName(GetModuleHandle(NULL), strDumpFileName, MAX_PATH);
	kl_sntprintf(strDumpFileName + kl_tcslen(strDumpFileName), MAX_PATH, KL_TEXT(".%02d-%02d %02d.%02d.%02d.dmp"), st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);

	static kl_tchar_t strCommandLine[2 * MAX_PATH];
	kl_sntprintf(strCommandLine, 2 * MAX_PATH, KL_TEXT("userdump.exe %u*%p \"%s\""), GetCurrentThreadId(), ep, strDumpFileName);

	bool bDumpCreated = false;
	if( CreateProcess(NULL, strCommandLine, NULL, NULL, 0, 0, NULL, NULL, &si, &pi) )
	{
		WaitForSingleObject(pi.hProcess, INFINITE);
		DWORD nExitCode;
		GetExitCodeProcess(pi.hProcess, &nExitCode);
		if( !nExitCode )
			bDumpCreated = true;
		CloseHandle(pi.hProcess);
		CloseHandle(pi.hThread);
	}

	if( bDumpCreated )
		print_log(KL_TEXT("\nUnhandled exception occured while processing sample %s, dump file - %s\n"), g_current_file, strDumpFileName);
	else
		print_log(KL_TEXT("\nUnhandled exception occured while processing sample %s, dump creation failed (missing userdump.exe?)\n"), g_current_file);

	_flushall();
	TerminateProcess(GetCurrentProcess(), 1);

	return EXCEPTION_EXECUTE_HANDLER;
}

//LPTOP_LEVEL_EXCEPTION_FILTER g_pPrevExceptionFilter = SetUnhandledExceptionFilter(TopLevelExceptionFilter);

#else
# warning "Need core dump generation support on this platform"
#endif
