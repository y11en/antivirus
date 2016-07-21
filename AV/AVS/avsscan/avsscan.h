// avsscan.h
//
//

#ifndef avsscan_h_INCLUDED
#define avsscan_h_INCLUDED

#include <kl_platform.h>

#include <klava/kl_tchar.h>

#include <Prague/prague.h>
#include <Prague/pr_loadr.h>

#if defined(_MSC_VER)
#	pragma warning (disable : 4311 4312)
#endif

#include <AV/structs/s_avs.h>

#if defined(_MSC_VER)
#	pragma warning (default : 4311 4312)
#endif

#include <AV/structs/s_scaner.h>

#include <klava/klav_utils.h>

////////////////////////////////////////////////////////////////

extern KLAV_Malloc_Alloc g_malloc;

////////////////////////////////////////////////////////////////

void print_log (const kl_tchar_t * fmt, ...);

typedef tERROR (* PFN_AVS_Msg_Handler)(
			void * handler_context,
			tDWORD msg_cls,
			const cDetectObjectInfo *info
		);

typedef tERROR (* PFN_AVS_Obj_Handler)(
			void * handler_context,
			cIO* io,
			const cObjectInfo *info
		);

////////////////////////////////////////////////////////////////

class AVS_Scanner
{
public:
	AVS_Scanner ();
	virtual ~AVS_Scanner ();

	enum
	{
		OPT_USE_IMUL       = 0x0002,
		OPT_STOP_ON_DETECT = 0x0004,
		OPT_LOG_SUBOBJECTS = 0x0008,
		OPT_PRINT_EVENTS   = 0x0010,
		OPT_CURE_TEST      = 0x0100,
		OPT_DSUT_MODE      = 0x1000,
		OPT_ASYNC_SCAN     = 0x2000,
	};

	struct Statistics
	{
		unsigned int cnt_scanned;
		unsigned int cnt_detected;
		unsigned int cnt_detected_exact;
		unsigned int cnt_disinfected;
		unsigned int cnt_errors;
		unsigned int cnt_disinfect_errors;

		Statistics () { clear (); }
		void clear ()
		{
			cnt_scanned = 0;
			cnt_detected = 0;
			cnt_detected_exact = 0;
			cnt_disinfected = 0;
			cnt_errors = 0;
			cnt_disinfect_errors = 0;
		}
	};

	struct Scan_Options
	{
		unsigned int m_opt_flags;   // options & flags
		unsigned int m_heur_level;  // heuristics level
		unsigned int m_time_limit;  // object processing time limit (sec)

		Scan_Options () { clear (); }
		void clear () { m_opt_flags = 0; m_heur_level = 0; m_time_limit = 0; }
	};

	struct Scan_Results
	{
		hIO          m_cured_io;    // result of disinfection
		klav_tstring m_detect_name; // detection name
		klav_tstring m_error_text;  // error message text
		tERROR       m_error;       // processing error, if any
		bool         m_detected;    // flag: smth was detected
		bool         m_disinfected; // flag: disinfection succeeded

		Scan_Results () : m_cured_io (0), m_detected (false), m_disinfected (false) {}
		~Scan_Results () { clear (); }

		void clear ()
		{
			if (m_cured_io != 0)
			{
				m_cured_io->sysCloseObject ();
				m_cured_io = 0;
			}
			m_detect_name.clear ();
			m_error_text.clear ();
			m_error = errOK;
			m_detected = false;
			m_disinfected = false;
		}
	};

	void set_msg_handler (void * ctx, PFN_AVS_Msg_Handler pfn)
		{ m_msg_handler_ctx = ctx; m_msg_handler_pfn = pfn; }

	void set_obj_handler (void * ctx, PFN_AVS_Obj_Handler pfn)
		{ m_obj_handler_ctx = ctx; m_obj_handler_pfn = pfn; }

	tERROR init_avs (const cAVSSettings * settings, tDWORD engine_flags);

	void process_file (
				const kl_tchar_t * fname,
				const Scan_Options& opts,
				Scan_Results& scan_results
			);

	void process_running_processes (
				const Scan_Options& opts,
				Scan_Results& scan_results
			);

	void clear_statistics ()
		{ m_statistics.clear (); }

	const Statistics& statictics () const
		{ return m_statistics; }

private:
	cAVS        * m_hAVS;
	cAVSSession * m_hAVSSession;
	cObject     * m_hReceiver;
	cObject     * m_hAsyncCtx;

	PFN_AVS_Msg_Handler m_msg_handler_pfn;
	PFN_AVS_Obj_Handler m_obj_handler_pfn;
	void *              m_msg_handler_ctx;
	void *              m_obj_handler_ctx;

	unsigned int  m_opt_flags;
	unsigned int  m_engine_flags;
	unsigned int  m_obj_actions; // object action flags (ACTION_XXX)
	bool          m_cure_mode;   // mode: file cure

	Statistics    m_statistics;

	void process_io (
		cIOObj & hIO,
		const Scan_Options& opts,
		Scan_Results& scan_results
		);

	tERROR msg_receive (hOBJECT _this, tDWORD msg_cls,
			tDWORD msg_id, hOBJECT obj, hOBJECT ctx,
			hOBJECT receive_point, tPTR pbuff, tDWORD* blen);

	static tERROR msg_receive_fn (hOBJECT _this, tDWORD msg_cls,
			tDWORD msg_id, hOBJECT obj, hOBJECT ctx,
			hOBJECT receive_point, tPTR pbuff, tDWORD* blen);

	tERROR ask_object_action (tDWORD msg_id, cAskObjectAction *act);

	tERROR test_cure_io (hOBJECT hIO, Scan_Results& scan_results);
	void print_object_event (const cDetectObjectInfo *info);

	void cleanup ();

	AVS_Scanner (const AVS_Scanner&);
	AVS_Scanner& operator= (const AVS_Scanner&);
};

////////////////////////////////////////////////////////////////

#endif // avsscan_h_INCLUDED

