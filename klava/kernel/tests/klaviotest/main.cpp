// main.cpp (klaviotest)
//
//

#include "klaviotest.h"

#include <string>

DEFINE_COMMAND_NO_ARGS(exit);
DEFINE_COMMAND_NO_ARGS(help);

DEFINE_COMMAND_ARGS(open)
{
	DEFINE_ARG(filename, ARG_TYPE_STR,  ARG_REQ),
	DEFINE_ARG(write,    ARG_TYPE_BOOL, ARG_FLAG),
};

DEFINE_COMMAND_NO_ARGS(close);

DEFINE_COMMAND_ARGS(vio)
{
	DEFINE_ARG(origin, ARG_TYPE_UINT, ARG_REQ),
	DEFINE_ARG(on,     ARG_TYPE_BOOL, ARG_FLAG),
	DEFINE_ARG(off,    ARG_TYPE_BOOL, ARG_FLAG),
};

DEFINE_COMMAND_ARGS(read)
{
	DEFINE_ARG(offset,    ARG_TYPE_UINT,  ARG_REQ),
	DEFINE_ARG(size,      ARG_TYPE_UINT,  ARG_REQ),
	DEFINE_ARG(dump_file, ARG_TYPE_STR,   ARG_REQ),
	DEFINE_ARG(iobuf,     ARG_TYPE_BOOL,  ARG_FLAG),
	DEFINE_ARG(map,       ARG_TYPE_BOOL,  ARG_FLAG),
	DEFINE_ARG(portion,   ARG_TYPE_BOOL,  ARG_FLAG),
	DEFINE_ARG(hex,       ARG_TYPE_BOOL,  ARG_FLAG),
};

DEFINE_COMMAND_ARGS(write)
{
	DEFINE_ARG(offset,    ARG_TYPE_UINT,  ARG_REQ),
	DEFINE_ARG(data_file, ARG_TYPE_STR,   ARG_REQ),
	DEFINE_ARG(portion,   ARG_TYPE_BOOL,  ARG_FLAG),
};

DEFINE_COMMAND_NO_ARGS(get_size);

DEFINE_COMMAND_ARGS(set_size)
{
	DEFINE_ARG(new_size, ARG_TYPE_UINT, ARG_REQ),
};

DEFINE_COMMAND_ARGS(expect)
{
	DEFINE_ARG(r1, ARG_TYPE_UINT, ARG_REQ),
	DEFINE_ARG(r2, ARG_TYPE_UINT, ARG_OPT),
	DEFINE_ARG(r3, ARG_TYPE_UINT, ARG_OPT),
	DEFINE_ARG(r4, ARG_TYPE_UINT, ARG_OPT),
	DEFINE_ARG(r5, ARG_TYPE_UINT, ARG_OPT),
};

DEFINE_COMMAND_ARGS(compare_file)
{
	DEFINE_ARG(file1, ARG_TYPE_STR, ARG_REQ),
	DEFINE_ARG(file2, ARG_TYPE_STR, ARG_REQ),
};

DEFINE_COMMANDS(klaviotest)
{
	DEFINE_COMMAND(exit,  "exit from the program"),
	DEFINE_COMMAND(help,  "display list of commands"),
	DEFINE_COMMAND(open,  "open file for reading or writing"),
	DEFINE_COMMAND(vio,   "virtual io on/off"),
	DEFINE_COMMAND(close, "close currently opened file"),
	DEFINE_COMMAND(read,  "read file data"),
	DEFINE_COMMAND(write, "write file data"),
	DEFINE_COMMAND(get_size, "get file size"),
	DEFINE_COMMAND(set_size, "set file size"),
	DEFINE_COMMAND(expect, "check results of command"),
	DEFINE_COMMAND(compare_file, "compare files"),
};

Command_Parser cmd_parser (COMMANDS(klaviotest));

void help ()
{
	fprintf (stderr, "Use klaviotest <cmd_file(s)>\n");
	fprintf (stderr, "commands are:\n");
	cmd_parser.print_command_list (stderr);
	fprintf (stderr, "\n");
}

// returns -1 on error, 0 on exit, 1 if OK
int process_command (const char *cmdline)
{
	if (! cmd_parser.parse_command (cmdline))
	{
		fprintf (stderr, "error: %s\n\n", cmd_parser.error_message ());
		help ();
		return 1;
	}

	COMMAND_ID_TYPE cmd_id = cmd_parser.command_id ();

	if (cmd_id == COMMAND_ID(help))
	{
		help ();
	}
	else if (cmd_id == COMMAND_ID (exit))
	{
		return 0;
	}
	else if (cmd_id == COMMAND_ID (open))
	{
		const char *fname = cmd_parser.get_string_arg ("filename");
		bool opt_write = cmd_parser.get_bool_arg ("write");

		do_io_open (fname, opt_write ? false : true);
	}
	else if (cmd_id == COMMAND_ID (close))
	{
		do_io_close ();
	}
	else if (cmd_id == COMMAND_ID (vio))
	{
		unsigned long origin = cmd_parser.get_uint_arg ("origin");
		bool vio_on = true;
		if (cmd_parser.get_bool_arg ("on"))
			vio_on = true;
		if (cmd_parser.get_bool_arg ("ff"))
			vio_on = false;

		do_vio (origin, vio_on);
	}
	else if (cmd_id == COMMAND_ID (read))
	{
		unsigned long offset = cmd_parser.get_uint_arg ("offset");
		uint32_t size = (uint32_t) cmd_parser.get_uint_arg ("size");
		const char * dump_file = cmd_parser.get_string_arg ("dump_file");
//		bool iobuf = cmd_parser.get_bool_arg ("iobuf");
		bool do_map = cmd_parser.get_bool_arg ("map");
		bool portion = cmd_parser.get_bool_arg ("portion");
		bool hex = cmd_parser.get_bool_arg ("hex");

		do_io_read (offset, size, dump_file, do_map, portion, hex);
	}
	else if (cmd_id == COMMAND_ID (write))
	{
		unsigned long offset = cmd_parser.get_uint_arg ("offset");
		const char * data_file = cmd_parser.get_string_arg ("data_file");
		bool portion = cmd_parser.get_bool_arg ("portion");

		buffer_t buf;
		if (! read_file (data_file, buf))
		{
			printf ("error reading from file: %s\n", data_file);
		}
		else
		{
			do_io_write (offset, & buf[0], buf.size (), portion);
		}
	}
	else if (cmd_id == COMMAND_ID (get_size))
	{
		do_io_get_size ();
	}
	else if (cmd_id == COMMAND_ID (set_size))
	{
		unsigned long new_size = cmd_parser.get_uint_arg ("new_size");
		do_io_set_size (new_size);
	}
	else if (cmd_id == COMMAND_ID (expect))
	{
		unsigned long r [5];
		unsigned int rcnt = 0;
		for (; rcnt < 5; ++rcnt)
		{
			char argname [10];
			sprintf (argname, "r%u", rcnt + 1);
			if (! cmd_parser.has_arg (argname))
				break;
			r [rcnt] = cmd_parser.get_uint_arg (argname);
		}
		if (! do_check_results (rcnt, r))
		{
			printf ("command results differ from expected\n");
			return -1;
		}
	}
	else if (cmd_id == COMMAND_ID (compare_file))
	{
		const char * fname1 = cmd_parser.get_string_arg ("file1");
		const char * fname2 = cmd_parser.get_string_arg ("file2");
		if (! do_compare_file (fname1, fname2))
		{
			printf ("files %s and %s are not equal\n", fname1, fname2);
			return -1;
		}
	}
	else
	{
		fprintf (stderr, "command not implemented\n");
	}

	return 1;
}	

int process_commands (FILE *fdin, const char *cmd_fname)
{
	Line_Reader reader (fdin, stdout);

	bool interactive = reader.is_interactive ();

	if (cmd_fname == 0)
		cmd_fname = "(stdin)";

	int exit_code = 0;
	int line_no = 0;

	while (reader.read_line ("klaviotest> "))
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

		int st = process_command (cmdline);

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

int main (int argc, char *argv[])
{
	int a = 1;
	int exit_code = 0;

	if (! io_lib_init ())
	{
		fprintf (stderr, "Error initializing IO library\n");
		return 1;
	}

	if (argc == 1)
	{
		exit_code = process_commands (stdin, "(stdin)");
	}
	else
	{
		for (; a < argc; ++a)
		{
			const char *cmd_fname = argv [a];

			FILE * fdin = fopen (cmd_fname, "rb");
			if (fdin == 0)
			{
				perror (cmd_fname);
				exit_code = 1;
				break;
			}

			exit_code = process_commands (fdin, cmd_fname);

			fclose (fdin);

			if (exit_code != 0)
				break;
		}
	}

	return exit_code;
}

