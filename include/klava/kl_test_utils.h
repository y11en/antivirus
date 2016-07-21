// test_utils.h
//
// utilites to help develop test applications
//

#ifndef kl_test_utils_h_INCLUDED
#define kl_test_utils_h_INCLUDED

#include <kl_types.h>
#include <kl_byteswap.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include <string>
#include <vector>

////////////////////////////////////////////////////////////////

typedef std::vector <uint8_t> buffer_t;

class Line_Reader
{
public:
	Line_Reader (FILE *fd_in = 0, FILE *fd_prompt = 0)
		: m_fd_in (fd_in), m_fd_prompt (fd_prompt) {}

	~Line_Reader () {}

	void open (FILE *fd_in, FILE *fd_prompt = 0)
		{ m_fd_in = fd_in; m_fd_prompt = fd_prompt; }
	
	bool read_line (const char *prompt = 0);

	const std::string& line () const
		{ return m_buf; }

	const char * line_str () const
		{ return m_buf.c_str (); }

	size_t line_len () const
		{ return m_buf.length (); }

	bool is_interactive () const;

private:
	FILE * m_fd_in;
	FILE * m_fd_prompt;
	std::string m_buf;

	Line_Reader (const Line_Reader&);
	Line_Reader& operator= (const Line_Reader&);
};

bool prompt_line (FILE *fdin, std::string& buf, FILE *fdprompt, const char *prompt);
bool read_line (FILE *fd, std::string& buf, const char *prompt = 0);

////////////////////////////////////////////////////////////////
// file utilites

bool hex_dump_file (const char *fname, const void *buf, size_t size);
bool hex_dump_fd (FILE *fd, const void *buf, size_t size);

bool write_file (const char *fname, const void *buf, size_t size);
bool write_fd (FILE *fd, const void *buf, size_t size);

bool read_file (const char *fname, buffer_t& buf);
bool read_fd (FILE *fd, buffer_t& buf);


////////////////////////////////////////////////////////////////
// string utilities

char * string_dup (const char *s);
char * string_dupn (const char *s, size_t n);
void   string_free (char *s);
int string_cmp (const char *s1, const char *s2);

struct cstring_ref
{
	const char * m_str;

	cstring_ref ()
		: m_str (0) {}
	cstring_ref (const char *s)
		: m_str (s) {}
	cstring_ref (const cstring_ref& s)
		: m_str (s) {}

	cstring_ref& operator= (const char *s)
		{ m_str = s; return *this; }
	cstring_ref& operator= (const cstring_ref& s)
		{ m_str = s; return *this; }

	operator const char * () const
		{ return m_str; }

	bool operator == (const cstring_ref& s) const
		{ return string_cmp (m_str, s.m_str) == 0; }

	bool operator < (const cstring_ref& s) const
		{ return string_cmp (m_str, s.m_str) < 0; }
};

class cstring
{
public:
	cstring ()
		: m_str (0) {}
	cstring (const char *s)
		: m_str (0) { assign (s); }
	cstring (const cstring& s)
		: m_str (0) { assign (s); }
	cstring (const char *s, unsigned int n)
		: m_str (0) { assign (s, n); }
	~cstring ()
		{ clear (); }

	cstring& operator= (const char *s)
		{ assign (s); return *this; }
	cstring& operator= (const cstring& s)
		{ assign (s); return *this; }

	void clear ()
		{ string_free (m_str); m_str = 0; }

	void assign (const char *s)
		{ clear (); if (s != 0) m_str = string_dup (s); }
	void assign (const char *s, unsigned int n)
		{ clear (); if (s != 0) m_str = string_dupn (s, n); }

	operator const char * () const
		{ return m_str; }

	bool operator == (const cstring& s) const
		{ return string_cmp (m_str, s.m_str) == 0; }

	bool operator < (const cstring& s) const
		{ return string_cmp (m_str, s.m_str) < 0; }

private:
	char * m_str;
};

////////////////////////////////////////////////////////////////
// command parser

enum // argument types
{
	ARG_TYPE_STR,   // string
	ARG_TYPE_INT,   // signed number
	ARG_TYPE_UINT,  // unsigned number
	ARG_TYPE_BOOL,  // 0/1, true/false, yes/no, on/off, present/absent
};

enum // argument flags
{
	ARG_REQ  = 0x0000,
	ARG_OPT  = 0x0001,
	ARG_FLAG = 0x0002, // non-positional argument, may be present or absent (BOOL only)
};

struct COMMAND_ARG_DESCRIPTOR
{
	const char * arg_name;
	unsigned int arg_type;
	unsigned int arg_flags;
};

struct COMMAND_DESCRIPTOR
{
	const char *             cmd_name;
	const char *             cmd_info;
	unsigned int             cmd_argc;
	COMMAND_ARG_DESCRIPTOR * cmd_args;
};

#define DEFINE_COMMAND_ARGS(NAME) \
COMMAND_ARG_DESCRIPTOR COMMAND_ARGS_##NAME [] =

#define DEFINE_COMMAND_NO_ARGS(NAME) \
COMMAND_ARG_DESCRIPTOR COMMAND_ARGS_##NAME [] = { 0, 0, 0 }

#define DEFINE_ARG(ANAME,TYPE,FLAGS) \
	{ #ANAME, TYPE, FLAGS }

#define DEFINE_COMMANDS(TNAME) \
COMMAND_DESCRIPTOR COMMANDS_##TNAME [] =

#define COMMAND_COUNT(TNAME) \
	(sizeof(COMMANDS_##TNAME)/sizeof(COMMANDS_##TNAME[0]))

#define COMMANDS(TNAME) \
	COMMANDS_##TNAME, COMMAND_COUNT(TNAME)

typedef const COMMAND_ARG_DESCRIPTOR * COMMAND_ID_TYPE;

#define COMMAND_ID(NAME) (&COMMAND_ARGS_##NAME[0])

#define DEFINE_COMMAND(NAME,INFO) \
	{ #NAME, INFO, sizeof(COMMAND_ARGS_##NAME)/sizeof(COMMAND_ARGS_##NAME[0]), COMMAND_ARGS_##NAME }

class Command_Parser;

struct Command_Handler
{
	// return values:
	// 0 - exit normally,
	// 1 - OK,
	// -1 - error (stops processing if invoked non-interactively)
	virtual int process_command (
				Command_Parser *parser,
				const char *cmdline
			) = 0;
};

class Command_Parser
{
public:
	Command_Parser (const COMMAND_DESCRIPTOR *cmds = 0, unsigned int cnt = 0, Command_Handler * handler = 0)
		: m_commands (cmds), m_command_count (cnt), m_command_handler (handler), m_current_cmd (0) {}

	void init (const COMMAND_DESCRIPTOR *cmds, unsigned int cnt, Command_Handler *handler)
		{ m_commands = cmds; m_command_count = cnt; m_command_handler = handler; }

	int process_commands_fd (FILE *fdin, const char *cmd_fname, const char *prompt);

	// if fname == 0, processes commands from stdin
	int process_commands_file (const char *fname, const char *prompt);

	bool parse_command (const char *cmdline);

	COMMAND_ID_TYPE command_id () const
		{ return m_current_cmd == 0 ? 0 : m_current_cmd->cmd_args; }

	bool has_arg (const char *name) const;
	const char * get_string_arg (const char *name) const;
	long get_int_arg (const char *name) const;
	unsigned long get_uint_arg (const char *name) const;
	bool get_bool_arg (const char *name) const;

	unsigned int command_count () const
		{ return m_command_count; }

	void describe_command (unsigned int idx, std::string& buf) const;

	const char * error_message () const
		{ return m_last_error.c_str (); }

	void print_command_list (FILE *fd);

private:
	const COMMAND_DESCRIPTOR * m_commands;
	unsigned int               m_command_count;
	
	Command_Handler *          m_command_handler;

	const COMMAND_DESCRIPTOR * m_current_cmd;

	typedef std::vector <cstring> str_vector_t;
	str_vector_t               m_arg_values;

	std::string                m_last_error;

	int find_arg (const char *name) const;

	void clear_current_cmd ();

	void clear_error ();
	void format_error (const char *fmt, ...);
	void format_error_v (const char *fmt, va_list ap);

	bool validate_arg_value (const COMMAND_ARG_DESCRIPTOR *arg_desc, const char *arg_value);

	Command_Parser (const Command_Parser&);
	Command_Parser& operator= (const Command_Parser&);
};

////////////////////////////////////////////////////////////////

#endif // kl_test_utils_h_INCLUDED

