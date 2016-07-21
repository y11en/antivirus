// kl_test_utils_impl.h
//
// test utilites implementation
//

#include <klava/kl_test_utils.h>
#if defined (_WIN32)
#include <io.h>
#endif
#include <sys/stat.h>

#include <ctype.h>

////////////////////////////////////////////////////////////////

bool Line_Reader::read_line (const char *prompt)
{
	m_buf.resize (0);

	if (prompt != 0 && m_fd_prompt != 0)
	{
		if (is_interactive ())
		{
			fputs (prompt, m_fd_prompt);
		}
	}

	for (;;)
	{
		int c = fgetc (m_fd_in);
		if (c < 0)
		{
			if (m_buf.size () == 0)
				return false;
			break;
		}
		if (c == '\r')
			continue;
		if (c == '\n')
			break;

		m_buf += (char) c;
	}
	return true;
}

bool Line_Reader::is_interactive () const
{
	if (isatty (fileno (m_fd_in)))
		return true;

	return false;
}

////////////////////////////////////////////////////////////////


bool hex_dump_file (const char *fname, const void *buf, size_t size)
{
	FILE *fd = fopen (fname, "wb");
	if (fd == 0)
	{
		perror (fname);
		return false;
	}

	bool f = hex_dump_fd (fd, buf, size);

	if (fclose (fd) < 0)
		f = false;

	return f;
}

#define BYTES_PER_LINE 16

unsigned char byte2asc (unsigned char b)
{
	if (b < 0x20)
		return '.';
	return b;
}

bool hex_dump_fd (FILE *fd, const void *buf, size_t size)
{
	const uint8_t * data = (const uint8_t *)buf;
	size_t off = 0;

	while (size != 0)
	{
		unsigned int bytes = BYTES_PER_LINE;
		if (size < (size_t) bytes)
			bytes = (unsigned int) size;

		// print offset
		fprintf (fd, "%08x:", (unsigned int)off);
		unsigned int i;
		for (i = 0; i < bytes; ++i)
			fprintf (fd, " %02x", data [off + i]);

		for (; i < BYTES_PER_LINE; ++i)
			fprintf (fd, "   ");

		fprintf (fd, " : ");

		for (i = 0; i < bytes; ++i)
			fputc (byte2asc (data [off + i]), fd);

		for (; i < BYTES_PER_LINE; ++i)
			fputc (' ', fd);

		fputc ('\n', fd);

		off += bytes;
		size -= bytes;
	}

	return true;
}

bool read_file (const char *fname, buffer_t& buf)
{
	buf.clear ();
	
	FILE *fd = fopen (fname, "rb");
	if (fd == 0)
		return 0;

	bool f = read_fd (fd, buf);

	fclose (fd);

	return f;
}

bool read_fd (FILE *fd, buffer_t& buf)
{
	buf.resize (0);

	struct stat st;
	if (fstat (fileno (fd), &st) < 0)
		return 0;

	size_t sz = (size_t) st.st_size;

	buf.resize (sz);
	if (buf.size () != sz)
		return false;

	if (sz != 0)
	{
		if (fread ((uint8_t *) &buf [0], sz, 1, fd) != 1)
		{
			buf.clear ();
			return false;
		}
	}

	return true;
}

bool write_file (const char *fname, const void *buf, size_t size)
{
	FILE *fd = fopen (fname, "wb");
	if (fd == 0)
	{
		perror (fname);
		return false;
	}

	bool f = write_fd (fd, buf, size);

	if (fclose (fd) < 0)
		f = false;

	return f;
}

bool write_fd (FILE *fd, const void *buf, size_t size)
{
	if (size != 0)
	{
		if (fwrite (buf, size, 1, fd) != 1)
			return false;
	}
		
	return true;
}

////////////////////////////////////////////////////////////////

char * string_dup (const char *s)
{
	size_t n = (s == 0) ? 0 : strlen (s);
	return string_dupn (s, n);
}

char * string_dupn (const char *s, size_t n)
{
	char *ns = new char [n + 1];
	if (ns == 0)
	{
		throw std::bad_alloc ();
	}

	if (s != 0)
	{
		memcpy (ns, s, n);
	}
	ns [n] = 0;

	return ns;
}

void string_free (char *s)
{
	if (s == 0)
		return;

	delete [] s;
}

int string_cmp (const char *s1, const char *s2)
{
	if (s1 == 0) s1 = "";
	if (s2 == 0) s2 = "";
	return strcmp (s1, s2);
}

////////////////////////////////////////////////////////////////
// tokenizer

// 'windows' mode: paths with spaces are quoted using double quotes
// returns 0 when there are no more tokens
const char * get_next_token (const char *s, std::string& token)
{
	token.resize (0);

	while (isspace (*s)) ++s;
	if (*s == 0)
		return 0;

	const char *pt = s;
	if (*s == '\"')
	{
		s++;
		pt++;
		while (*s != 0 && *s != '\"') ++s;
		token.assign (pt, s - pt);
		if (*s != 0)
			++s;
	}
	else
	{
		while (*s != 0 && !isspace (*s)) ++s;
		token.assign (pt, s - pt);
	}
	return s;
}

////////////////////////////////////////////////////////////////

bool Command_Parser::parse_command (const char *cmdline)
{
	clear_current_cmd ();
	clear_error ();

	// TODO: replace strtok with more sane tokenizer
	const char * s = cmdline;
	std::string cmd, arg;
	
	s = get_next_token (s, cmd);
	if (s == 0)
	{
		format_error ("command expected");
		return false;
	}
	
	const COMMAND_DESCRIPTOR *cmd_desc = 0;

	unsigned int cmd_idx = 0;
	for (; cmd_idx < m_command_count; ++cmd_idx)
	{
		if (strcmp (m_commands[cmd_idx].cmd_name, cmd.c_str ()) == 0)
		{
			cmd_desc = &m_commands[cmd_idx];
			break;
		}
	}

	if (cmd_desc == 0)
	{
		format_error ("invalid command: %s", cmd.c_str ());
		return false;
	}

	m_current_cmd = cmd_desc;

	unsigned int argc = cmd_desc->cmd_argc;
	if (argc == 1 && cmd_desc->cmd_args [0].arg_name == 0)
		argc = 0;

	m_arg_values.resize (argc);

	// calclate number of positional arguments
	unsigned int argc_pos = 0;
	for (; argc_pos < argc; ++argc_pos)
	{
		// bail out on first FLAG argument
		if ((cmd_desc->cmd_args [argc_pos].arg_flags & ARG_FLAG) != 0)
			break;
	}

	unsigned int arg_idx = 0;

	// parse command arguments
	for (; ; arg_idx++)
	{
		s = get_next_token (s, arg);
		if (s == 0)
			break;

		if (arg_idx < argc_pos)
		{
			const COMMAND_ARG_DESCRIPTOR *arg_desc = &(cmd_desc->cmd_args [arg_idx]);

			if (! validate_arg_value (arg_desc, arg.c_str ()))
			{
				format_error ("invalid value for parameter %s", arg_desc->arg_name);
				return false;
			}

			m_arg_values [arg_idx] = arg.c_str ();
		}
		else
		{
			unsigned int idx = 0;
			for (; idx < argc; ++idx)
			{
				const COMMAND_ARG_DESCRIPTOR *arg_desc = &(cmd_desc->cmd_args [idx]);
				if ((arg_desc->arg_flags & ARG_FLAG) != 0)
				{
					if (arg_desc->arg_type == ARG_TYPE_BOOL)
					{
						if (strcmp (arg_desc->arg_name, arg.c_str ()) == 0)
						{
							m_arg_values [idx] = "1";
							break;
						}
					}
				}
			}
			if (idx == argc)
			{
				format_error ("unrecognized argument: %s", arg.c_str ());
				return false;
			}
		}
	}

	if (arg_idx < argc_pos)
	{
		// have all required arguments been processed ?
		if ((cmd_desc->cmd_args [arg_idx].arg_flags & ARG_OPT) == 0)
		{
			format_error ("not enough arguments for command: %s", m_current_cmd->cmd_name);
			return false;
		}
	}
	
	return true;
}

int Command_Parser::process_commands_fd (FILE *fdin, const char *cmd_fname, const char *prompt)
{
	Line_Reader reader (fdin, stdout);

	bool interactive = reader.is_interactive ();

	if (cmd_fname == 0)
		cmd_fname = "(stdin)";

	int exit_code = 0;
	int line_no = 0;

	if (prompt == 0)
		prompt = "> ";

	while (reader.read_line (prompt))
	{
		line_no++;

		const char *cmdline = reader.line_str ();
		if (cmdline [0] == 0)
			continue;

		if (cmdline [0] == '#')
			continue;

		if (! interactive)
		{
			printf ("%s line %d: %s\n", cmd_fname, line_no, cmdline);
		}

		int st = 1;
		if (m_command_handler != 0)
		{
			st = m_command_handler->process_command (this, cmdline);
		}

		if (st == 0)
			break;

		if (st < 0)
		{
			if (! interactive)
			{
				exit_code = 1;
				break;
			}
		}
	}

	return exit_code;
}

int Command_Parser::process_commands_file (const char *fname, const char *prompt)
{
	FILE * fd = 0;
	const char * cmd_fname = 0;
	
	if (fname == 0)
	{
		fd = stdin;
		cmd_fname = "(stdin)";
	}
	else
	{
		fd = fopen (fname, "rb");
		if (fd == 0)
		{
			perror (fname);
			return -1;
		}
		cmd_fname = fname;
	}
	
	int st = process_commands_fd (fd, cmd_fname, prompt);
	
	if (fname != 0)
	{
		fclose (fd);
	}
	
	return st;
}

////////////////////////////////////////////////////////////////

static int get_bool_value (const char *s)
{
	if (strcmp (s, "yes") == 0)
		return 1;
	if (strcmp (s, "no") == 0)
		return 0;
	if (strcmp (s, "true") == 0)
		return 1;
	if (strcmp (s, "false") == 0)
		return 0;
	if (strcmp (s, "on") == 0)
		return 1;
	if (strcmp (s, "off") == 0)
		return 0;

	char *eptr = 0;
	unsigned long n = strtoul (s, &eptr, 0);
	if (eptr == s)
		return -1;

	if (n == 0)
		return 0;

	if (n == 1)
		return 1;

	return -1;
}

bool Command_Parser::validate_arg_value (const COMMAND_ARG_DESCRIPTOR *arg_desc, const char *arg_value)
{
	char *eptr = 0;

	switch (arg_desc->arg_type)
	{
	case ARG_TYPE_STR:
		return true;

	case ARG_TYPE_INT:
		strtol (arg_value, &eptr, 0);
		if (eptr == arg_value)
			return false;
		return true;

	case ARG_TYPE_UINT:
		strtoul (arg_value, &eptr, 0);
		if (eptr == arg_value)
			return false;
		return true;

	case ARG_TYPE_BOOL:
		if (get_bool_value (arg_value) < 0)
			return false;
		return true;

	default:
		break;
	}
	return false;
}

bool Command_Parser::has_arg (const char *name) const
{
	int idx = find_arg (name);
	if (idx < 0)
		return false;

	const char *val = m_arg_values [idx];
	return val != 0;
}

const char * Command_Parser::get_string_arg (const char *name) const
{
	int idx = find_arg (name);
	if (idx < 0)
		return "";

	const char * val = m_arg_values [idx];
	if (val == 0)
		return "";

	return val;
}

long Command_Parser::get_int_arg (const char *name) const
{
	return strtol (get_string_arg (name), 0, 0);
}

unsigned long Command_Parser::get_uint_arg (const char *name) const
{
	return strtoul (get_string_arg (name), 0, 0);
}

bool Command_Parser::get_bool_arg (const char *name) const
{
	int n = get_bool_value (get_string_arg (name));
	return n > 0;
}

int Command_Parser::find_arg (const char *name) const
{
	if (m_current_cmd == 0)
		return -1;

	unsigned int i = 0;
	for (; i < m_current_cmd->cmd_argc; ++i)
	{
		const COMMAND_ARG_DESCRIPTOR * arg_desc = &(m_current_cmd->cmd_args [i]);
		if (arg_desc->arg_name != 0 && strcmp (arg_desc->arg_name, name) == 0)
			return (int) i;
	}

	return -1;
}

void Command_Parser::clear_current_cmd ()
{
	m_current_cmd = 0;
	m_arg_values.resize (0);
}

void Command_Parser::clear_error ()
{
	m_last_error.resize (0);
}

void Command_Parser::format_error (const char *fmt, ...)
{
	va_list ap;
	va_start (ap, fmt);
	format_error_v (fmt, ap);
	va_end (ap);
}

void Command_Parser::format_error_v (const char *fmt, va_list ap)
{
	char buf [1000];
	vsprintf (buf, fmt, ap);
	m_last_error = buf;
}

void Command_Parser::describe_command (unsigned int idx, std::string& buf) const
{
	buf.resize (0);

	const COMMAND_DESCRIPTOR * cmd_desc = &m_commands [idx];
	buf += cmd_desc->cmd_name;

	unsigned int a = 0;
	for (; a < cmd_desc->cmd_argc; ++a)
	{
		const COMMAND_ARG_DESCRIPTOR * arg_desc = &(cmd_desc->cmd_args [a]);
		if (arg_desc->arg_name == 0)
			break;
		buf += ' ';
		if ((arg_desc->arg_flags & ARG_FLAG) != 0)
		{
			buf += '(';
			buf += arg_desc->arg_name;
			buf += ')';
		}
		else if ((arg_desc->arg_flags & ARG_OPT) != 0)
		{
			buf += '[';
			buf += arg_desc->arg_name;
			buf += ']';
		}
		else
		{
			buf += '<';
			buf += arg_desc->arg_name;
			buf += '>';
		}
	}

	if (cmd_desc->cmd_info != 0 && cmd_desc->cmd_info [0] != 0)
	{
		buf += " - ";
		buf += cmd_desc->cmd_info;
	}
}

void Command_Parser::print_command_list (FILE *fd)
{
	unsigned int cmd_cnt = command_count ();

	std::string buf;
	for (unsigned int i = 0; i< cmd_cnt; ++i)
	{
		describe_command (i, buf);
		fprintf (fd, "%s\n", buf.c_str ());
	}
}

