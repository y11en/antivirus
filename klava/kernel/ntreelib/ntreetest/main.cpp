// main.cpp
//
//

#include <tns_mgr.h>
#include <tns_write.h>
#include <tns_db.h>

#include <stdio.h>
#include <string.h>
#include <exception>
#include <vector>

#include <klava/klav_utils.h>
#include <klava/klav_sig.h>
#include <klava/kdb_utils.h>

#include <klava/kl_test_utils.h>

static KLAV_Malloc_Alloc g_alloc;

#define DEFAULT_HASH_LEVEL 16

bool opt_autocheck = false;
bool opt_autocommit = false;
bool opt_trace = false;

////////////////////////////////////////////////////////////////

struct Printer_Impl : public TNS_Printer
{
public:
	Printer_Impl (FILE *fd, int indent)
		: m_fd (fd), m_indent (indent), m_eol (true) {}

	virtual void print (const char * fmt, va_list ap);
	virtual void endl ();

	virtual void indent (int level)
		{ m_indent += (level * 4); }

private:
	FILE * m_fd;
	int    m_indent;
	bool   m_eol;
};

void Printer_Impl::print (const char * fmt, va_list ap)
{
	if (m_eol)
	{
		m_eol = false;
		for (int i = 0; i < m_indent; ++i)
		{
			fputc (' ', m_fd);
		}
	}

	vfprintf (m_fd, fmt, ap);
}


void Printer_Impl::endl ()
{
	fputc ('\n', m_fd);
	m_eol = true;
}

////////////////////////////////////////////////////////////////

struct Tree_Writer_Impl : public TNS_Writer
{
	Tree_Writer_Impl ();
	virtual ~Tree_Writer_Impl ();

	virtual uint32_t    KLAV_CALL size ();
	virtual klav_bool_t KLAV_CALL append (const void *data, uint32_t size);
	virtual klav_bool_t KLAV_CALL read  (uint32_t pos, void *data, uint32_t size);
	virtual klav_bool_t KLAV_CALL write (uint32_t pos, const void *data, uint32_t size);

	void save (const char *fname);

	std::vector <uint8_t> m_data;
};

Tree_Writer_Impl::Tree_Writer_Impl ()
{
}

Tree_Writer_Impl::~Tree_Writer_Impl ()
{
}

uint32_t Tree_Writer_Impl::size ()
{
	return (uint32_t) m_data.size ();
}

klav_bool_t Tree_Writer_Impl::append (const void *data, uint32_t size)
{
	m_data.insert (m_data.end (), (const uint8_t *)data, (const uint8_t *)data + size);
	return true;
}

klav_bool_t Tree_Writer_Impl::read (uint32_t pos, void *data, uint32_t size)
{
	if (pos >= m_data.size () || (m_data.size () - pos < size))
		return false;

	memcpy (data, & m_data [pos], size);
	return true;
}

klav_bool_t Tree_Writer_Impl::write (uint32_t pos, const void *data, uint32_t size)
{
	if (pos >= m_data.size () || (m_data.size () - pos < size))
		return false;

	memcpy (& m_data [pos], data, size);
	return true;
}

void Tree_Writer_Impl::save (const char *fname)
{
	FILE *fd = fopen (fname, "w+b");
	if (fd == 0)
	{
		throw std::runtime_error ("error writing file");
	}

	size_t n = fwrite (& m_data [0], 1, m_data.size (), fd);
	if (n != m_data.size ())
	{
		tns_raise_error (KLAV_EWRITE, "error writing file data");
	}

	fclose (fd);
}

////////////////////////////////////////////////////////////////
// Merged tree reader

struct Merged_Tree_Hdr
{
	uint32_t m_root_node;
	uint32_t m_reserved1;
	uint32_t m_reserved2;
	uint32_t m_reserved3;
};

class Merged_Tree_Reader
{
public:
	Merged_Tree_Reader ()
		: m_data (0), m_size (0) {}
	~Merged_Tree_Reader ()
		{ cleanup (); }

	KLAV_ERR open (const char *fname);

	void close ()
		{ cleanup (); }

	const uint8_t * data () const
		{ return m_data + sizeof (Merged_Tree_Hdr); }

	size_t size () const
		{ return m_size - sizeof (Merged_Tree_Hdr); }

	nodeid_t root_node () const
		{ return ((Merged_Tree_Hdr *)(m_data))->m_root_node; }

private:
	uint8_t * m_data;
	size_t    m_size;

	KLAV_ERR do_open (FILE *fd);
	void cleanup ();
};

KLAV_ERR Merged_Tree_Reader::open (const char *fname)
{
	FILE * fd = fopen (fname, "rb");
	if (fd == 0)
		return KLAV_ENOTFOUND;

	KLAV_ERR err = do_open (fd);
	if (KLAV_FAILED (err))
		cleanup ();

	fclose (fd);

	return err;
}

KLAV_ERR Merged_Tree_Reader::do_open (FILE *fd)
{
	fseek (fd, 0, SEEK_END);

	m_size = ftell (fd);
	if (m_size < sizeof (Merged_Tree_Hdr))
		return KLAV_ECORRUPT;

	fseek (fd, 0, SEEK_SET);

	m_data = (uint8_t *) malloc (m_size);
	if (m_data == 0)
		return KLAV_ENOMEM;

	size_t nrd = fread (m_data, 1, m_size, fd);
	if (nrd != m_size)
		return KLAV_EREAD;

	return KLAV_OK;
}

void Merged_Tree_Reader::cleanup ()
{
	if (m_data != 0)
		::free (m_data);
	m_data = 0;
	m_size = 0;
}

////////////////////////////////////////////////////////////////
// Line reader

class Text_File_Reader
{
public:
	enum { IO_BUFSIZE = 0x1000 };

	Text_File_Reader ()
		: m_fd (0),
		  m_iobuf (new char [IO_BUFSIZE]),
		  m_bptr (0),
		  m_eptr (0)
	{
	}

	~Text_File_Reader ()
	{
		close ();
		delete[] m_iobuf;
	}

	bool open (const char *fname)
	{
		close ();

		m_fd = fopen (fname, "rb");
		if (m_fd == 0)
			return false;

		return true;
	}

	void close ()
	{
		if (m_fd != 0)
		{
			fclose (m_fd);
			m_fd = 0;
		}

		m_line.resize (0);
		m_bptr = 0;
		m_eptr = 0;
	}

	const char * getline ();

private:
	FILE * m_fd;
	char * m_iobuf;
	int    m_bptr;
	int    m_eptr;
	std::string m_line;
};


const char * Text_File_Reader::getline ()
{
	m_line.resize (0);

	for (;;)
	{
		while (m_bptr < m_eptr)
		{
			unsigned char c = m_iobuf [m_bptr++];
			if (c == '\n')
			{
				size_t len = m_line.length ();
				for (; len > 0; --len)
				{
					if ((unsigned char)m_line [len - 1] >= 0x20)
						break;
				}
				m_line.resize (len);
				return m_line.c_str ();
			}

			m_line += c;
		}

		m_bptr = 0;
		m_eptr = 0;

		int n = (int) fread (m_iobuf, 1, IO_BUFSIZE, m_fd);
		if (n < 0)
			return 0;

		if (n == 0)
			break;

		m_eptr = n;
	}

	if (m_line.length () == 0)
		return 0;

	size_t len = m_line.length ();
	for (; len > 0; --len)
	{
		if ((unsigned char)m_line [len - 1] >= 0x20)
			break;
	}
	m_line.resize (len);
	return m_line.c_str ();
}

////////////////////////////////////////////////////////////////

int compare_files (const char *fname1, const char *fname2)
{
	FILE *fd1 = fopen (fname1, "rb");
	if (fd1 == 0)
	{
		perror (fname1);
		return -1;
	}

	FILE *fd2 = fopen (fname2, "rb");
	if (fd2 == 0)
	{
		perror (fname2);
		fclose (fd1);
		return -1;
	}

	uint8_t buf1 [4096];
	uint8_t buf2 [4096];
	int n = 0, st = 1;

	for (;;)
	{
		n = (int) fread (buf1, 1, sizeof (buf1), fd1);
		if (n < 0)
		{
			perror (fname1);
			st = -1;
		}
		int n2 = (int) fread (buf2, 1, sizeof (buf2), fd2);
		if (n2 != n)
		{
			st = -1;  // mismatch (file size different)
			break;
		}
		if (n == 0)
		{
			st = 1;
			break;
		}

		if (memcmp (buf1, buf2, n) != 0)
		{
			st = -1;
			break;
		}
	}

	fclose (fd1);
	fclose (fd2);

	return st;
}

////////////////////////////////////////////////////////////////
// Command processor

DEFINE_COMMAND_ARGS(create)
{
	DEFINE_ARG(name, ARG_TYPE_STR,   ARG_REQ),
};

DEFINE_COMMAND_ARGS(open)
{
	DEFINE_ARG(name,     ARG_TYPE_STR,   ARG_REQ),
	DEFINE_ARG(readonly, ARG_TYPE_BOOL,  ARG_FLAG),
	DEFINE_ARG(merged,   ARG_TYPE_BOOL,  ARG_FLAG),
};

DEFINE_COMMAND_ARGS(close)
{
	DEFINE_ARG(merge, ARG_TYPE_BOOL,  ARG_FLAG),
};

DEFINE_COMMAND_ARGS(add)
{
	DEFINE_ARG(id,   ARG_TYPE_UINT,  ARG_REQ),
	DEFINE_ARG(text, ARG_TYPE_STR,   ARG_REQ),
};

DEFINE_COMMAND_ARGS(del)
{
	DEFINE_ARG(text, ARG_TYPE_STR,   ARG_REQ),
};

DEFINE_COMMAND_ARGS(addsigs)
{
	DEFINE_ARG(file, ARG_TYPE_STR,  ARG_REQ),
};

DEFINE_COMMAND_ARGS(delsigs)
{
	DEFINE_ARG(file, ARG_TYPE_STR,   ARG_REQ),
};

DEFINE_COMMAND_ARGS(print)
{
	DEFINE_ARG(norm,   ARG_TYPE_BOOL,  ARG_FLAG),
	DEFINE_ARG(merged, ARG_TYPE_BOOL,  ARG_FLAG),
};

DEFINE_COMMAND_ARGS(printf)
{
	DEFINE_ARG(file,   ARG_TYPE_STR,   ARG_REQ),
	DEFINE_ARG(norm,   ARG_TYPE_BOOL,  ARG_FLAG),
	DEFINE_ARG(merged, ARG_TYPE_BOOL,  ARG_FLAG),
};

DEFINE_COMMAND_ARGS(dumpb)
{
	DEFINE_ARG(data, ARG_TYPE_BOOL,  ARG_FLAG),
};

DEFINE_COMMAND_ARGS(dumpbf)
{
	DEFINE_ARG(file, ARG_TYPE_STR,   ARG_REQ),
	DEFINE_ARG(data, ARG_TYPE_BOOL,  ARG_FLAG),
};

DEFINE_COMMAND_ARGS(compare)
{
	DEFINE_ARG(file,   ARG_TYPE_STR,   ARG_REQ),
	DEFINE_ARG(norm,   ARG_TYPE_BOOL,  ARG_FLAG),
	DEFINE_ARG(merged, ARG_TYPE_BOOL,  ARG_FLAG),
};

DEFINE_COMMAND_ARGS(write)
{
	DEFINE_ARG(file,   ARG_TYPE_STR,   ARG_REQ),
	DEFINE_ARG(merged, ARG_TYPE_BOOL,  ARG_FLAG),
};

DEFINE_COMMAND_ARGS(autocheck)
{
	DEFINE_ARG(on, ARG_TYPE_BOOL,   ARG_REQ),
};

DEFINE_COMMAND_ARGS(autocommit)
{
	DEFINE_ARG(on, ARG_TYPE_BOOL,   ARG_REQ),
};

DEFINE_COMMAND_ARGS(hash_level)
{
	DEFINE_ARG(level, ARG_TYPE_UINT,   ARG_OPT),
};

DEFINE_COMMAND_ARGS(trace)
{
	DEFINE_ARG(on, ARG_TYPE_BOOL,   ARG_REQ),
};

DEFINE_COMMAND_NO_ARGS(clear);

DEFINE_COMMAND_ARGS(check)
{
	DEFINE_ARG(merged, ARG_TYPE_BOOL,  ARG_FLAG),
};

DEFINE_COMMAND_NO_ARGS(commit);
DEFINE_COMMAND_NO_ARGS(rollback);

DEFINE_COMMAND_NO_ARGS(exit);
DEFINE_COMMAND_NO_ARGS(help);

DEFINE_COMMANDS(ntreetest)
{
	DEFINE_COMMAND(create,    "create database"),
	DEFINE_COMMAND(open,      "open database"),
	DEFINE_COMMAND(close,     "close database"),
	DEFINE_COMMAND(clear,     "clear signature tree"),
	DEFINE_COMMAND(add,       "add signature"),
	DEFINE_COMMAND(del,       "delete signature"),
	DEFINE_COMMAND(addsigs,   "add signatures from file"),
	DEFINE_COMMAND(delsigs,   "delete signatures from file"),
	DEFINE_COMMAND(print,     "print signature tree"),
	DEFINE_COMMAND(printf,    "print signature tree to file"),
	DEFINE_COMMAND(dumpb,     "dump database blocks"),
	DEFINE_COMMAND(dumpbf,    "dump database blocks to file"),
	DEFINE_COMMAND(compare,   "compares printed tree with the sample"),
	DEFINE_COMMAND(check,     "validates signature tree"),
	DEFINE_COMMAND(commit,    "commit database"),
	DEFINE_COMMAND(rollback,  "rollback database"),
	DEFINE_COMMAND(write,     "write runtime tree"),
	DEFINE_COMMAND(autocheck, "validate signature tree after each modification"),
	DEFINE_COMMAND(autocommit, "commit database after each modification"),
	DEFINE_COMMAND(hash_level, "prints or sets hash level"),
	DEFINE_COMMAND(trace,     "turns tracing on/off"),
	DEFINE_COMMAND(exit,      "exit from the program"),
	DEFINE_COMMAND(help,      "display list of commands"),
};

////////////////////////////////////////////////////////////////

struct NtreeTest : public Command_Handler
{
	NtreeTest ();
	virtual ~NtreeTest ();

	virtual int process_command (
				Command_Parser *parser,
				const char *cmdline
			);

	int do_create (Command_Parser *cmd_parser);
	int do_open (Command_Parser *cmd_parser);
	int do_close (Command_Parser *cmd_parser);
	int do_add (Command_Parser *cmd_parser);
	int do_del (Command_Parser *cmd_parser);
	int do_addsigs (Command_Parser *cmd_parser);
	int do_delsigs (Command_Parser *cmd_parser);
	int do_clear (Command_Parser *cmd_parser);
	int do_print (Command_Parser *cmd_parser);
	int do_printf (Command_Parser *cmd_parser);
	int do_print_file (const char *fname, bool norm, bool merged);
	int do_dumpb (Command_Parser *cmd_parser);
	int do_dumpbf (Command_Parser *cmd_parser);
	int do_dump_blocks (const char *fname, bool data);
	int do_compare (Command_Parser *cmd_parser);
	int do_check (Command_Parser *cmd_parser);
	int do_commit (Command_Parser *cmd_parser);
	int do_rollback (Command_Parser *cmd_parser);
	int do_write (Command_Parser *cmd_parser);
	int do_help (Command_Parser *cmd_parser);

	int merge_sig (const char *sig, size_t sig_size, uint32_t evid);
	bool process_sig_file (const char *fname, bool del);

	File_Block_Device       m_blk_device;
	File_Block_Manager      m_blk_mgr;
	Tree_Node_Block_Manager m_node_mgr;

	Merged_Tree_Reader      m_merged_tree;
	Tree_Node_Reader        m_merged_rdr;

	Printer_Impl            m_printer;

	nodeid_t  m_nid_root;
	uint32_t  m_hash_level;

	void clear ();
};

NtreeTest::NtreeTest () :
		m_blk_mgr (& g_alloc),
		m_node_mgr (& g_alloc),
		m_merged_rdr (& g_alloc),
		m_printer (stdout, 0),
		m_nid_root (NODEID_INVALID),
		m_hash_level (DEFAULT_HASH_LEVEL)
{
}

NtreeTest::~NtreeTest ()
{
}

int NtreeTest::process_command (
				Command_Parser *parser,
				const char *cmdline
			)
{
	if (! parser->parse_command (cmdline))
	{
		fprintf (stderr, "error: %s\n\n", parser->error_message ());
		do_help (parser);
		return -1;
	}

	COMMAND_ID_TYPE cmd_id = parser->command_id ();

	try
	{
		if (cmd_id == COMMAND_ID(create))
		{
			return do_create (parser);
		}
		else if (cmd_id == COMMAND_ID(open))
		{
			return do_open (parser);
		}
		else if (cmd_id == COMMAND_ID(close))
		{
			return do_close (parser);
		}
		else if (cmd_id == COMMAND_ID(add))
		{
			return do_add (parser);
		}
		else if (cmd_id == COMMAND_ID(del))
		{
			return do_del (parser);
		}
		else if (cmd_id == COMMAND_ID(addsigs))
		{
			return do_addsigs (parser);
		}
		else if (cmd_id == COMMAND_ID(delsigs))
		{
			return do_delsigs (parser);
		}
		else if (cmd_id == COMMAND_ID(clear))
		{
			return do_clear (parser);
		}
		else if (cmd_id == COMMAND_ID(print))
		{
			return do_print (parser);
		}
		else if (cmd_id == COMMAND_ID(printf))
		{
			return do_printf (parser);
		}
		else if (cmd_id == COMMAND_ID(dumpb))
		{
			return do_dumpb (parser);
		}
		else if (cmd_id == COMMAND_ID(dumpbf))
		{
			return do_dumpbf (parser);
		}
		else if (cmd_id == COMMAND_ID(compare))
		{
			return do_compare (parser);
		}
		else if (cmd_id == COMMAND_ID(check))
		{
			return do_check (parser);
		}
		else if (cmd_id == COMMAND_ID(commit))
		{
			return do_commit (parser);
		}
		else if (cmd_id == COMMAND_ID(rollback))
		{
			return do_rollback (parser);
		}
		else if (cmd_id == COMMAND_ID(write))
		{
			return do_write (parser);
		}
		else if (cmd_id == COMMAND_ID(autocheck))
		{
			opt_autocheck = parser->get_bool_arg ("on");
			return 1;
		}
		else if (cmd_id == COMMAND_ID(autocommit))
		{
			opt_autocommit = parser->get_bool_arg ("on");
			return 1;
		}
		else if (cmd_id == COMMAND_ID(hash_level))
		{
			if (parser->has_arg ("level"))
			{
				m_hash_level = parser->get_int_arg ("level");
				fprintf (stdout, "Hash level set to %u\n", m_hash_level);
			}
			else
			{
				fprintf (stdout, "Hash level is %u\n", m_hash_level);
			}
			return 1;
		}
		else if (cmd_id == COMMAND_ID(trace))
		{
			opt_trace = parser->get_bool_arg ("on");
			return 1;
		}
		else if (cmd_id == COMMAND_ID(help))
		{
			return do_help (parser);
		}
		else if (cmd_id == COMMAND_ID (exit))
		{
			return 0;
		}
		else
		{
			fprintf (stderr, "command not implemented: %s\n", cmdline);
			return -1;
		}
	}
	catch (const std::exception& ex)
	{
		fprintf (stdout, "error: %s\n", ex.what ());
		m_node_mgr.rollback ();
		return -1;
	}
	
	return 1;
}

int NtreeTest::do_create (Command_Parser *cmd_parser)
{
	const char *fname = cmd_parser->get_string_arg ("name");

	m_node_mgr.close ();
	m_blk_mgr.close ();
	m_blk_device.close ();

	KLAV_ERR err = m_blk_device.open (fname, File_Block_Device::ALLOW_CREATE|File_Block_Device::CREATE_ALWAYS, 0, 0);
	if (KLAV_FAILED (err))
	{
		fprintf (stdout, "error creating database file: %s\n", fname);
		return -1;
	}

	err = m_blk_mgr.open (& m_blk_device);
	if (KLAV_FAILED (err))
	{
		fprintf (stdout, "error creating database on file: %s\n", fname);
		m_blk_device.close ();
		return -1;
	}

	err = m_node_mgr.open (& m_blk_mgr);
	if (KLAV_FAILED (err))
	{
		fprintf (stdout, "error opening node manager on file: %s\n", fname);
		m_blk_mgr.close ();
		m_blk_device.close ();
		return -1;
	}

	return 1;
}

int NtreeTest::do_open (Command_Parser *cmd_parser)
{
	const char *fname = cmd_parser->get_string_arg ("name");
	bool readonly = cmd_parser->get_bool_arg ("readonly");
	bool merged = cmd_parser->get_bool_arg ("merged");

	KLAV_ERR err = KLAV_OK;

	if (! merged)
	{
		m_node_mgr.close ();
		m_blk_mgr.close ();
		m_blk_device.close ();

		uint32_t open_mode = 0;
		if (readonly != 0)
			open_mode |= File_Block_Device::OPEN_READONLY;

		err = m_blk_device.open (fname, open_mode, 0, 0);
		if (KLAV_FAILED (err))
		{
			fprintf (stdout, "error opening database file: %s\n", fname);
			return -1;
		}

		err = m_blk_mgr.open (& m_blk_device);
		if (KLAV_FAILED (err))
		{
			fprintf (stdout, "error opening database on file: %s\n", fname);
			m_blk_device.close ();
			return -1;
		}

		err = m_node_mgr.open (& m_blk_mgr);
		if (KLAV_FAILED (err))
		{
			fprintf (stdout, "error opening node manager on file: %s\n", fname);
			m_blk_mgr.close ();
			m_blk_device.close ();
			return -1;
		}

		m_nid_root = m_blk_mgr.get_root_node ();
		m_hash_level = m_blk_mgr.get_hash_level ();
	}
	else
	{
		m_merged_rdr.close ();
		m_merged_tree.close ();

		err = m_merged_tree.open (fname);
		if (KLAV_FAILED (err))
		{
			fprintf (stdout, "error opening merged file: %s\n", fname);
			return -1;
		}

		m_merged_rdr.open (m_merged_tree.data (), m_merged_tree.size ());
	}

	return 1;
}

int NtreeTest::do_close (Command_Parser *cmd_parser)
{
	bool merged = cmd_parser->get_bool_arg ("merged");

	if (! merged)
	{
		m_node_mgr.close ();
		m_blk_mgr.close ();
		m_blk_device.close ();
	}
	else
	{
		m_merged_rdr.close ();
		m_merged_tree.close ();
	}

	return 1;
}

int NtreeTest::do_add (Command_Parser *cmd_parser)
{
	uint32_t sig_id = cmd_parser->get_uint_arg ("id");
	if (sig_id == 0 || sig_id == EVID_DELETED)
	{
		fprintf (stdout, "invalid signature ID\n");
		return -1;
	}

	const char * sig = cmd_parser->get_string_arg ("text");

	int st = merge_sig (sig, strlen (sig), sig_id);
	if (st != 1)
		return st;

	if (opt_autocheck)
	{
		st = do_check (cmd_parser);
	}

	if (st == 1 && opt_autocommit)
	{
		st = do_commit (cmd_parser);
	}

	return st;
}

int NtreeTest::do_del (Command_Parser *cmd_parser)
{
	const char * sig = cmd_parser->get_string_arg ("text");

	int st = merge_sig (sig, strlen (sig), EVID_DELETED);

	if (opt_autocheck)
	{
		st = do_check (cmd_parser);
	}

	if (st == 1 && opt_autocommit)
	{
		st = do_commit (cmd_parser);
	}

	return st;
}

bool NtreeTest::process_sig_file (const char *fname, bool del)
{
	Text_File_Reader reader;

	if (! reader.open (fname))
	{
		fprintf (stdout, "error reading file: %s\n", fname);
		return false;
	}

	for (;;)
	{
		const char * line = reader.getline ();
		if (line == 0)
			break;

		if (line [0] == 0)
			continue;

		if (! isdigit (line [0]))
			continue;

		KDBParserImpl parser (line, strlen (line));
		
		uint32_t sig_id = 0;
		if (! parser.scanInt (& sig_id) || sig_id == 0)
			continue;

		const char *sig_text = 0;
		size_t sig_text_len = 0;
		if (! parser.scanStr (& sig_text, & sig_text_len))
			continue;

		if (del)
			sig_id = EVID_DELETED;

		int st = merge_sig (sig_text, sig_text_len, sig_id);
		if (st != 1)
			return false;
	}

	return true;
}

int NtreeTest::do_addsigs (Command_Parser *cmd_parser)
{
	const char * fname = cmd_parser->get_string_arg ("file");

	if (! process_sig_file (fname, false))
	{
		fprintf (stdout, "error processing file: %s\n", fname);
		return -1;
	}

	int st = 1;

	if (opt_autocheck)
	{
		st = do_check (cmd_parser);
	}

	if (st == 1 && opt_autocommit)
	{
		st = do_commit (cmd_parser);
	}

	return st;
}

int NtreeTest::do_delsigs (Command_Parser *cmd_parser)
{
	const char * fname = cmd_parser->get_string_arg ("file");

	if (! process_sig_file (fname, true))
	{
		fprintf (stdout, "error processing file: %s\n", fname);
		return -1;
	}

	int st = 1;

	if (opt_autocheck)
	{
		st = do_check (cmd_parser);
	}

	if (st == 1 && opt_autocommit)
	{
		st = do_commit (cmd_parser);
	}

	return st;
}

int NtreeTest::merge_sig (const char *sig, size_t sig_size, uint32_t evid)
{
	std::vector <uint8_t> sig_data;
	KLAV_Buffer_Writer <std::vector <uint8_t> > writer (sig_data);
			
	const char *errmsg = 0;
	if (! KLAV_Sig_Parse (& g_alloc, sig, sig_size, 0, & writer, & errmsg))
	{
		fprintf (stdout, "error parsing signature: %s\n", errmsg);
		return -1;
	}

	uint32_t flags = 0;
	if (opt_trace)
		flags |= TNS_F_TRACE;

	m_nid_root = KLAV_Tree_Merge_Signature (
			& m_node_mgr, m_nid_root,
			& sig_data [0], sig_data.size (), evid, flags, m_hash_level);

//	m_node_mgr.commit ();

	return 1;
}

int NtreeTest::do_clear (Command_Parser *cmd_parser)
{
	m_nid_root = NODEID_INVALID;
	m_node_mgr.clear_tree ();
	return 1;
}

int NtreeTest::do_print (Command_Parser *cmd_parser)
{
	bool norm = cmd_parser->get_bool_arg ("norm");
	bool merged = cmd_parser->get_bool_arg ("merged");

	if (! merged)
	{
		KLAV_Tree_Print (& m_node_mgr, m_nid_root, & m_printer, norm);
	}
	else
	{
		KLAV_Tree_Print (& m_merged_rdr, m_merged_tree.root_node (), & m_printer, norm);
	}
	return 1;
}

int NtreeTest::do_printf (Command_Parser *cmd_parser)
{
	const char *fname = cmd_parser->get_string_arg ("file");
	bool norm = cmd_parser->get_bool_arg ("norm");
	bool merged = cmd_parser->get_bool_arg ("merged");

	return do_print_file (fname, norm, merged);
}

int NtreeTest::do_print_file (const char *fname, bool norm, bool merged)
{
	FILE * fd = fopen (fname, "w+t");
	if (fd == 0)
	{
		perror (fname);
		return -1;
	}

	Printer_Impl printer (fd, 0);

	if (! merged)
	{
		KLAV_Tree_Print (& m_node_mgr, m_nid_root, & printer, norm);
	}
	else
	{
		KLAV_Tree_Print (& m_merged_rdr, m_merged_tree.root_node (), & printer, norm);
	}

	fclose (fd);

	return 1;
}

int NtreeTest::do_dumpb (Command_Parser *cmd_parser)
{
	bool data = cmd_parser->get_bool_arg ("data");

	return do_dump_blocks (0, data);
}

int NtreeTest::do_dumpbf (Command_Parser *cmd_parser)
{
	const char *fname = cmd_parser->get_string_arg ("file");
	bool data = cmd_parser->get_bool_arg ("data");

	return do_dump_blocks (fname, data);
}

int NtreeTest::do_dump_blocks (const char *fname, bool data)
{
	FILE * fd = stdout;
	if (fname != 0)
	{
		fd = fopen (fname, "w+t");
		if (fd == 0)
		{
			perror (fname);
			return -1;
		}
	}

	// dump file header/blocks

	Printer_Impl printer (fd, 0);

	KLAV_Tree_File_Dump (&m_blk_device, &printer, data);

	if (fname != 0)
	{
		fclose (fd);
	}

	return 1;
}

int NtreeTest::do_compare (Command_Parser *cmd_parser)
{
	const char * cmp_fname = cmd_parser->get_string_arg ("file");
	const char * tmp_fname = "tree.tmp";
	bool norm = cmd_parser->get_bool_arg ("norm");
	bool merged = cmd_parser->get_bool_arg ("merged");
	
	int st = do_print_file (tmp_fname, norm, merged);
	if (st != 1)
		return st;

	st = compare_files (tmp_fname, cmp_fname);
	if (st < 0)
	{
		fprintf (stderr, "file %s differs from the sample: %s\n", tmp_fname, cmp_fname);
	}

	return st;
}

struct Leak_Reporter : public Tree_Node_Reporter
{
	virtual ~Leak_Reporter ()
	{
	}

	virtual void report_node (nodeid_t nid)
	{
		fprintf (stdout, "LEAKED NODE: 0x%08X\n", nid);
	}
};

int NtreeTest::do_check (Command_Parser *cmd_parser)
{
	bool merged = cmd_parser->get_bool_arg ("merged");

	klav_bool_t ok = 0;

	if (! merged)
	{
		ok = KLAV_Tree_Validate (
				& m_node_mgr,
				m_nid_root,
				& m_printer,
				0,
				m_hash_level,
				KLAV_TREE_VALIDATE_MARK_USED);
		if (ok)
		{
			Leak_Reporter leak_reporter;
			uint32_t leak_cnt = m_node_mgr.report_nodes (& leak_reporter, TREE_NODE_REPORT_UNUSED);

			if (leak_cnt != 0)
				fprintf (stdout, "LEAKED NODES DETECTED (%u)\n", leak_cnt);

			KLAV_Tree_Validate (
				& m_node_mgr,
				m_nid_root,
				0,
				0,
				m_hash_level,
				KLAV_TREE_VALIDATE_CLEAR_USED);
		}
	}
	else
	{
		ok = KLAV_Tree_Validate (
				& m_merged_rdr,
				m_merged_tree.root_node (),
				& m_printer,
				0,
				m_hash_level,
				KLAV_TREE_VALIDATE_FINAL_NORM);
	}

	return ok ? 1 : -1;
}

int NtreeTest::do_commit (Command_Parser *cmd_parser)
{
	m_node_mgr.commit (m_nid_root, m_hash_level);
//	if (KLAV_FAILED (err))
//	{
//		fprintf (stdout, "error committing database: 0x%08X\n", err);
//		return -1;
//	}

	return 1;		
}

int NtreeTest::do_rollback (Command_Parser *cmd_parser)
{
	m_node_mgr.rollback ();

	m_nid_root = m_blk_mgr.get_root_node ();
	m_hash_level = m_blk_mgr.get_hash_level ();

	return 1;		
}

int NtreeTest::do_write (Command_Parser *cmd_parser)
{
	const char *fname = cmd_parser->get_string_arg ("file");
	bool merged = cmd_parser->get_bool_arg ("merged");

	Tree_Writer_Impl writer;

	TNS_Write_Options opts;
	opts.m_flags = 0;
	opts.m_hash_level = m_hash_level;
	opts.m_hash_size = 64;

	if (! merged)
	{
		KLAV_Tree_Write (& m_node_mgr, m_nid_root, & writer, & opts, 0);
	}
	else
	{
		KLAV_Tree_Write (& m_merged_rdr, m_merged_tree.root_node (), & writer, & opts, 0);
	}

	writer.save (fname);

	return 1;
}

int NtreeTest::do_help (Command_Parser *cmd_parser)
{
	fprintf (stdout, "Use ntreetest <cmd_file(s)>\n");
	fprintf (stdout, "commands are:\n");
	cmd_parser->print_command_list (stdout);
	fprintf (stdout, "\n");
	return 1;
}

////////////////////////////////////////////////////////////////

int main (int argc, char *argv[])
{
//	KLAV_ERR err = KLAV_OK;

	NtreeTest ntree_test;

	Command_Parser cmd_parser (COMMANDS(ntreetest), & ntree_test);

	int a = 1;
	int exit_code = 0;

	if (argc == 1)
	{
		exit_code = cmd_parser.process_commands_file (0, "ntreetest> ");
	}
	else
	{
		for (; a < argc; ++a)
		{
			const char *cmd_fname = argv [a];

			exit_code = cmd_parser.process_commands_file (cmd_fname, "ntreetest> ");

			if (exit_code != 0)
				break;
		}
	}

	return exit_code;
}

#include <klava/kl_test_utils_impl.h>

