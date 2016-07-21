// main.cpp
//

#include <stdio.h>
#include <string.h>
#include <klava/klaveng.h>
#include <klava/klavsys.h>
#include <klava/stream_packet.h>

#ifdef _WIN32
# include <windows.h>
#define RTLD_NOW 1
#define dlopen(NAME,FLAGS) LoadLibrary(NAME)
#define dlclose(HDL) FreeLibrary((HMODULE)HDL)
#define dlsym(HDL,NAME) GetProcAddress((HMODULE)HDL,NAME)
#elif defined __unix__
# include <unistd.h>
# include <dlfcn.h>
#endif

#define KLAVENG_NAME "klaveng.kdl"

struct Allocator : public KLAV_IFACE_IMPL(KLAV_Alloc)
{
public:
	Allocator () {}
	virtual ~Allocator () {}

	uint8_t * KLAV_CALL alloc (size_t size)
	{
		return (uint8_t *)::calloc (1, size);
	}

	uint8_t * KLAV_CALL realloc (void *ptr, size_t newsize)
	{
		return (uint8_t *)::realloc (ptr, newsize);
	}

	void KLAV_CALL free (void *ptr)
	{
		if (ptr != 0)
			::free (ptr);
	}
};

Allocator g_alloc;

////////////////////////////////////////////////////////////////

struct AV_Callbacks :
	public KLAV_IFACE_IMPL(KLAV_Message_Printer),
	public KLAV_IFACE_IMPL(KLAV_Callback_Handler)
{
	virtual ~AV_Callbacks ();
	
	virtual uint32_t KLAV_CALL print_message_v (
				KLAV_CONTEXT_TYPE  context,
				uint32_t level,
				const char * fmt,
				va_list ap
			);

	virtual uint32_t KLAV_CALL handle_callback (
				KLAV_CONTEXT_TYPE  context,
				KLAV_CALLBACK_CODE code,
				KLAV_Properties *  props
			);
};

AV_Callbacks::~AV_Callbacks ()
{
}

uint32_t KLAV_CALL AV_Callbacks::print_message_v (
				KLAV_CONTEXT_TYPE context,
				uint32_t trace_level,
				const char * fmt,
				va_list ap
			)
{
	const char * level_name = "";
	switch (trace_level)
	{
	case KLAV_TRACE_LEVEL_DEBUG:
		level_name = "DEBUG";
		break;
	case KLAV_TRACE_LEVEL_PERF:
		level_name = "PERF";
		break;
	case KLAV_TRACE_LEVEL_INFO:
		level_name = "INFO";
		break;
	case KLAV_TRACE_LEVEL_WARNING:
		level_name = "WARNING";
		break;
	case KLAV_TRACE_LEVEL_ERROR:
		level_name = "ERROR";
		break;
	}

	fprintf (stdout, "%s: ", level_name);
	vfprintf (stdout, fmt, ap);
	fputc ('\n', stdout);

	return 0;
}

uint32_t KLAV_CALL AV_Callbacks::handle_callback (
				KLAV_CONTEXT_TYPE  context,  
				KLAV_CALLBACK_CODE code,
				KLAV_Properties *  props
			)
{
	switch (code)
	{
	case KLAV_CALLBACK_EXCEPTION: {
		unsigned int error_code = KLAV_Get_Property_Int (props, KLAV_PROPID_ERROR_CODE);
		const char * error_info = KLAV_Get_Property_Str (props, KLAV_PROPID_ERROR_INFO);
		if (error_info == 0)
			error_info = "";

		fprintf (stdout, "! ERROR: 0x%x: %s\n", error_code, error_info);
		} break;
	case KLAV_CALLBACK_TRACEPOINT: {
		unsigned int trace_id = KLAV_Get_Property_Int (props, KLAV_PROPID_TRACE_ID);
		const char * trace_info = KLAV_Get_Property_Str (props, KLAV_PROPID_TRACE_INFO);
		if (trace_info == 0)
			trace_info = "";

		fprintf (stdout, "- TRACE: 0x%x: %s\n", trace_id, trace_info);
		} break;
	case KLAV_CALLBACK_OBJECT_DETECT: {
		const char * detect_name = KLAV_Get_Property_Str (props, KLAV_PROPID_VERDICT_NAME);
		if (detect_name == 0)
			detect_name = "(unknown)";

		fprintf (stdout, "* DETECT: %s\n", detect_name);
		} break;
	default:
		break;
	}
	
	return 0;
}

////////////////////////////////////////////////////////////////

static bool scan_file_data (KLAV_Engine *engine, KLAV_Context *ctx, FILE *fd)
{
#define PKT_BUF_SIZE 4096

	char buf[PKT_BUF_SIZE];
	KLAV_ERR err = KLAV_OK;

//	err = engine->process_action (ctx, "BEGIN_STREAM", 0, 0);
//	if (KLAV_FAILED (err))
//	{
//		fprintf (stdout, "error processing stream (BEGIN_STREAM) (error code 0x%x)\n", err);
//		return false;
//	}

	uint32_t pkt_flags = KLAV_STREAM_PACKET_F_BEGIN;

	for (;;)
	{
		int n = (int) fread (buf, 1, PKT_BUF_SIZE, fd);
		if (n < 0)
		{
			fprintf (stdout, "error reading file\n");
			return false;
		}

		if (n < PKT_BUF_SIZE)
			pkt_flags |= KLAV_STREAM_PACKET_F_END;

		KLAV_Stream_Packet pkt (buf, n, KLAV_STREAM_FORMAT_RAW, pkt_flags);
		KLAV_ERR err = engine->process_action (ctx, "PROCESS_PACKET", &pkt, 0);
		if (KLAV_FAILED (err))
		{
			fprintf (stdout, "error scanning file data (error code 0x%x)\n", err);
			return false;
		}

		pkt_flags &= ~KLAV_STREAM_PACKET_F_BEGIN;

		if (n < PKT_BUF_SIZE)
			break;
	}

//	err = engine->process_action (ctx, "END_STREAM", 0, 0);
//	if (KLAV_FAILED (err))
//	{
//		fprintf (stdout, "error processing stream (END_STREAM) (error code 0x%x)\n", err);
//		return false;
//	}

	return true;
}

static bool do_scan_file (KLAV_Engine *engine, FILE *fd)
{
	KLAV_Context *ctx = 0;
	KLAV_ERR err = KLAV_OK;
	
	err = engine->create_context (0, & ctx);
	if (KLAV_FAILED (err))
	{
		fprintf (stderr, "error creating context (error code 0x%x)\n", err);
		return false;
	}

	err = engine->activate_context (ctx, 0, 0);
	if (KLAV_FAILED (err))
	{
		fprintf (stderr, "error activating context (error code 0x%x)\n", err);
		engine->destroy_context (ctx);
		return false;
	}
	
	bool ok = scan_file_data (engine, ctx, fd);

	engine->deactivate_context (ctx);
	engine->destroy_context (ctx);

	return ok;
}

bool scan_file (KLAV_Engine *engine, const char *file_name)
{
	FILE *fd = fopen (file_name, "rb");
	if (fd == 0)
	{
		perror (file_name);
		return false;
	}

	bool f = do_scan_file (engine, fd);

	fclose (fd);
	return f;
}

////////////////////////////////////////////////////////////////

int main (int argc, char *argv[])
{
	if (argc < 2)
	{
		fprintf (stderr, "Use klaveng_test.exe <database_path> <files_to_scan...>\n");
		return 1;
	}

	const char *db_path = argv[1];

	KLAV_Engine *engine = 0;
	KLAV_DB_Loader *db_loader = 0;
	KLAV_Proc_Resolver *proc_resolver = 0;
	KLAV_ERR err = KLAV_OK;
	int st = 0;

	// create engine object
	void * engine_dll = dlopen (KLAVENG_NAME,RTLD_NOW);
	if (engine_dll == 0)
	{
		fprintf (stderr, "error loading engine dynamic library: %s\n", KLAVENG_NAME);
		st = 1;
	}

	if (st == 0)
	{
		PFN_KLAV_Engine_Create pfn_create = (PFN_KLAV_Engine_Create) dlsym (engine_dll, KLAV_ENGINE_CREATE_NAME);
		if (pfn_create == 0)
		{
			fprintf (stderr, "could not locate export symbol: %s in library: %s\n",
				KLAV_ENGINE_CREATE_NAME, KLAVENG_NAME);
			st = 1;
		}
		else
		{
			err = pfn_create (KLAV_ENGINE_VERSION_CURRENT, & g_alloc, & engine);
			if (KLAV_FAILED (err))
			{
				fprintf (stderr, "error creating engine object (error code 0x%x)\n", err);
				st = 1;
			}
		}
	}

	AV_Callbacks callbacks;

	if (st == 0)
	{
		KLAV_SET_IFACE (engine, Callback_Handler, & callbacks);
		KLAV_SET_IFACE (engine, Message_Printer, & callbacks);

		err = KLAVSYS_Open_DB_File (db_path, & db_loader);

		KLAV_SET_IFACE (engine, DB_Loader, db_loader);

		proc_resolver = KLAVSYS_Create_Proc_Resolver (0);
		KLAV_SET_IFACE (engine, Proc_Resolver, proc_resolver);

		if (KLAV_FAILED (err))
		{
			fprintf (stderr, "error opening database file: %s (error code 0x%x)\n", db_path, err);
			st = 1;
		}

		if (st == 0)
		{
			err = engine->load (0);

			if (KLAV_FAILED (err))
			{
				fprintf (stderr, "error loading database from %s (error code 0x%x)\n", db_path, err);
				st = 1;
			}
		}
	}

	if (st == 0)
	{
		int a;
		for (a = 2; a < argc; ++a)
		{
			const char *file_name = argv[a];

			fprintf (stdout, "scanning %s\n", file_name);

			if (! scan_file (engine, file_name))
			{
				fprintf (stderr, "error scanning file: %s\n", file_name);
				st = 1;
			}
		}
	}

	if (engine != 0)
	{
		engine->destroy ();
		engine = 0;
	}

	if (db_loader != 0)
	{
		KLAVSYS_Close_DB_File (db_loader);
		db_loader = 0;
	}

	if (proc_resolver != 0)
	{
		KLAVSYS_Destroy_Proc_Resolver (proc_resolver);
		proc_resolver = 0;
	}

	if (engine_dll != 0)
	{
		dlclose (engine_dll);
		engine_dll = 0;
	}

	return st;
}

