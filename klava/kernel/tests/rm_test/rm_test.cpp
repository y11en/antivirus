// rm_test.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"


const std::string PROMPT_STRING (">");

////////////////////////////////////////////////////////////////////////////////
//
// helper functions
//
////////////////////////////////////////////////////////////////////////////////

int hex2bin (const uint8_t *hex_data, size_t sz, std::vector<uint8_t>& bin_data)
{
	if (sz % 2) return -1;
	for (size_t i = 0; i < sz; )
	{
		int val = 0, n = 0;
		uint8_t ch = hex_data[i++];

		if (ch >= '0' && ch <= '9') n = ch - '0';
		else if (ch >= 'A' && ch <= 'F') n = ch -'A' + 10;
		else if (ch >= 'a' && ch <= 'f') n = ch -'a' + 10;
		else return -1;
		val = n;

		ch = hex_data[i++];
		if (ch >= '0' && ch <= '9') n = ch - '0';
		else if (ch >= 'A' && ch <= 'F') n = ch -'A' + 10;
		else if (ch >= 'a' && ch <= 'f') n = ch -'a' + 10;
		else return -1;
		val = (val << 4) | n;

		bin_data.push_back ((uint8_t)val);
	}
	return 0;
}

////////////////////////////////////////////////////////////////////////////////

template <class Ch, class Tr>
std::basic_ostream <Ch,Tr>& formatted_hex (std::basic_ostream<Ch,Tr>& os)
{
	return os << "0x"<< std::setw(8) << std::setfill('0') << std::uppercase << std::hex;
}

template <class Ch, class Tr>
std::basic_ostream <Ch,Tr>& hex_dump (std::basic_ostream<Ch,Tr>& os,
									  const void* buf, size_t sz)
{
	const size_t line_sz = 16;

	for (size_t i = 0; i < sz; )
	{
		uint16_t c = *(reinterpret_cast <const uint8_t*> (buf) + i);

		if ((i % line_sz) == 0) os << formatted_hex << i << ": ";
		os << std::hex << std::setw(2) << std::setfill('0') << c << std::dec << ' ';
		++i;
		if ((i % line_sz) == 0 || i == sz) os << std::endl;
	}
	return os;
}


////////////////////////////////////////////////////////////////////////////////
// test exception

class test_exception : public std::exception
{
public:

	virtual ~test_exception() throw() { delete[] m_msg; }

	const char * what () const throw () { return m_msg; }

	bool fatal() const { return m_fatal; }
	uint32_t error() const { return m_error; }

	static void raise (const char *msg, uint32_t error = 0, bool fatal = false)
	{
		throw test_exception (msg, error, fatal);
	}

	static void raise (const std::string& msg, uint32_t error = 0, bool fatal = false)
	{
		raise(msg.c_str(), error, fatal);
	}

protected:

	test_exception (const char *msg, uint32_t error = 0, bool fatal = false)
		: m_msg (0), m_error (error), m_fatal (fatal)
	{
		if (msg == 0) msg = "";
		size_t len = strlen (msg);

		m_msg = new char [len+1];
		::memcpy (m_msg, msg, len+1);
	}

	char *m_msg;
	bool m_fatal;
	uint32_t m_error;
};

////////////////////////////////////////////////////////////////////////////////
// input checker

template <class Ch, class Tr>
std::basic_istream<Ch,Tr>& check (std::basic_istream<Ch,Tr>* is, const char *msg)
{
	if (! is->good ())
	{
		test_exception::raise (msg);
	}
	return *is;
}

template <class Ch, class Tr>
std::basic_istream<Ch,Tr>& check (std::basic_istream<Ch,Tr>* is, const std::string& msg)
{
	return check (is, msg.c_str ());
}

////////////////////////////////////////////////////////////////////////////////
// read unsigned long

template <class Ch, class Tr>
unsigned long read_ul (std::basic_istream<Ch,Tr>* is, const char *msg)
{
	std::string buf;
	*is >> buf;

	check (is, msg);

	int base = 10;
	if (buf[0] == '0') base = 8;
	if (buf[1] == 'x' || buf[1] == 'X') base = 16;

	const char *ptr = buf.c_str ();
	char *endptr = 0;
	unsigned long  i = strtoul (ptr, &endptr, base);
	if (buf.size() != endptr - ptr) test_exception::raise (msg);
	return i;
}

template <class Ch, class Tr>
unsigned long read_ul (std::basic_istream<Ch,Tr>& is, const std::string& msg)
{
	return read_ul (is, msg.c_str ());
}

////////////////////////////////////////////////////////////////////////////////
// input parameters stream

class parms_stream
{
public:
	parms_stream (std::istream * inp)
		: m_inp (inp) {}

	virtual void read_parms () = 0;

	void check (const char *msg) const
	{
		if (! (*m_inp).good())
		{
			test_exception::raise (msg);
		}
	}

	void prompt (const char* msg) const { std::cout << msg << PROMPT_STRING; }
	void prompt (const std::string& msg) const { prompt (msg.c_str ()); }

protected:
	std::istream * m_inp;
};



////////////////////////////////////////////////////////////////////////////////
//
// input region parameters
//
////////////////////////////////////////////////////////////////////////////////

class region_parms : public parms_stream
{
public:
	region_parms (std::istream * inp)
		: parms_stream (inp), m_type (0), m_id (0) {}

	virtual ~region_parms () {}

	virtual void read_parms ()
		{
			prompt_type (); read_type ();
			prompt_id (); read_id ();
		}

	static region_parms * create_parms (bool interpreter, std::istream * inp);
	static void destroy_parms (region_parms * parms);

	uint32_t get_type () const { return m_type; }
	uint32_t get_id () const { return m_id; }

protected:

	uint32_t m_type;
	uint32_t m_id;

	virtual void prompt_type ()  = 0;
	virtual void prompt_id ()  = 0; 

	void read_type () { m_type = read_ul (m_inp, "error reading region type"); }
	void read_id ()   { m_id   = read_ul (m_inp, "error reading region identifier"); }
};

struct region_parms_interpreter : public region_parms
{
	region_parms_interpreter (std::istream * inp)
		: region_parms (inp) {}

	virtual void prompt_type () { prompt ("input region type      "); }
	virtual void prompt_id ()   { prompt ("input region identifier"); }
};

struct region_parms_cmdline : public region_parms
{
	region_parms_cmdline (std::istream * inp)
		: region_parms (inp) {}

	virtual void prompt_type () {}
	virtual void prompt_id () {}
};

region_parms * region_parms::create_parms (bool interpreter, std::istream * inp)
{
	region_parms * parms = 0;
	if (interpreter)
		parms = new region_parms_interpreter (inp);
	else
		parms = new region_parms_cmdline (inp);
	return parms;
}

void region_parms::destroy_parms (region_parms * parms)
{
	delete parms;
}

////////////////////////////////////////////////////////////////////////////////
//
// input region parameters and flags
//
////////////////////////////////////////////////////////////////////////////////

class region_parms_and_flags : public region_parms
{
public:
	region_parms_and_flags (std::istream * inp)
		: region_parms (inp), m_flags (0) {}

	static region_parms_and_flags * create_parms (bool interpreter, std::istream * inp);
	static void destroy_parms (region_parms_and_flags * parms);

	uint32_t get_flags () const { return m_flags; }

	virtual void prompt_flags () = 0;

	void read_parms ()
		{
			region_parms::read_parms ();
			prompt_flags (); read_flags ();
		}

protected:

	uint32_t m_flags;

	void read_flags () { m_flags = read_ul (m_inp, "error reading flags"); }
};

struct region_parms_and_flags_interpreter : public region_parms_and_flags
{
	region_parms_and_flags_interpreter (std::istream * inp)
		: region_parms_and_flags (inp) {}

	void prompt_type ()  { prompt ("input region type      "); }
	void prompt_id ()    { prompt ("input region identifier"); }
	void prompt_flags () { prompt ("input flags            "); }
};

struct region_parms_and_flags_cmdline : public region_parms_and_flags
{
	region_parms_and_flags_cmdline (std::istream * inp)
		: region_parms_and_flags (inp) {}

	void prompt_type () {}
	void prompt_id () {}
	void prompt_flags () {}
};

region_parms_and_flags * region_parms_and_flags::create_parms (bool interpreter, std::istream * inp)
{
	region_parms_and_flags * parms = 0;
	if (interpreter)
		parms = new region_parms_and_flags_interpreter (inp);
	else
		parms = new region_parms_and_flags_cmdline (inp);
	return parms;
}

void region_parms_and_flags::destroy_parms (region_parms_and_flags * parms)
{
	delete parms;
}


////////////////////////////////////////////////////////////////////////////////
//
// input region parameters and flags and mask
//
////////////////////////////////////////////////////////////////////////////////

class region_parms_and_flags_mask : public region_parms_and_flags
{
public:
	region_parms_and_flags_mask (std::istream * inp)
		: region_parms_and_flags (inp), m_mask (0) {}

	static region_parms_and_flags_mask * create_parms (bool interpreter, std::istream * inp);
	static void destroy_parms (region_parms_and_flags_mask * parms);

	uint32_t get_mask () const { return m_mask; }

	virtual void prompt_mask () = 0;
	void read_parms ()
		{
			region_parms_and_flags::read_parms ();
			prompt_mask (); read_mask ();
		}

protected:

	uint32_t m_mask;

	void read_mask () { m_mask = read_ul (m_inp, "error reading flags"); }
};

struct region_parms_and_flags_mask_interpreter : public region_parms_and_flags_mask
{
	region_parms_and_flags_mask_interpreter (std::istream * inp)
		: region_parms_and_flags_mask (inp) {}

	void prompt_type ()  { prompt ("input region type      "); }
	void prompt_id ()    { prompt ("input region identifier"); }
	void prompt_flags () { prompt ("input flags            "); }
	void prompt_mask ()  { prompt ("input flags mask       "); }
};

struct region_parms_and_flags_mask_cmdline : public region_parms_and_flags_mask
{
	region_parms_and_flags_mask_cmdline (std::istream * inp)
		: region_parms_and_flags_mask (inp) {}

	void prompt_type () {}
	void prompt_id () {}
	void prompt_flags () {}
	void prompt_mask ()  {}
};

region_parms_and_flags_mask * region_parms_and_flags_mask::create_parms (bool interpreter, std::istream * inp)
{
	region_parms_and_flags_mask * parms = 0;
	if (interpreter)
		parms = new region_parms_and_flags_mask_interpreter (inp);
	else
		parms = new region_parms_and_flags_mask_cmdline (inp);
	return parms;
}

void region_parms_and_flags_mask::destroy_parms (region_parms_and_flags_mask * parms)
{
	delete parms;
}

////////////////////////////////////////////////////////////////////////////////
//
// set data file parms
//
////////////////////////////////////////////////////////////////////////////////

class set_data_file_parms : public parms_stream
{
public:
	set_data_file_parms (std::istream * inp)
		: parms_stream (inp) {}

	virtual ~set_data_file_parms () {}

	static set_data_file_parms * create_parms (bool interpreter, std::istream * inp);
	static void destroy_parms (set_data_file_parms * parms);

	virtual void read_parms ()
	{
		prompt_file (); read_file ();
	}

	const std::string& get_file () const { return m_file; }

protected:

	std::string m_file;

	virtual void prompt_file () = 0;
	void read_file () {	*m_inp >> m_file; check ("error reading file name"); }
};

struct set_data_file_parms_interpreter : public set_data_file_parms
{
	set_data_file_parms_interpreter (std::istream * inp)
		: set_data_file_parms (inp) {}

	void prompt_file () { prompt ("input file name "); }
};

struct set_data_file_parms_parms_cmdline : public set_data_file_parms
{
	set_data_file_parms_parms_cmdline (std::istream * inp)
		: set_data_file_parms (inp) {}

	void prompt_file () {};
};

set_data_file_parms * set_data_file_parms::create_parms (bool interpreter, std::istream * inp)
{
	set_data_file_parms * parms = 0;
	if (interpreter)
		parms = new set_data_file_parms_interpreter (inp);
	else
		parms = new set_data_file_parms_parms_cmdline (inp);
	return parms;
}

void set_data_file_parms::destroy_parms (set_data_file_parms * parms)
{
	delete parms;
}


////////////////////////////////////////////////////////////////////////////////
//
// register region params
//
////////////////////////////////////////////////////////////////////////////////

class register_region_parms : public region_parms
{
public:
	register_region_parms (std::istream * inp)
		: region_parms (inp),
		  m_offset (0), m_size_before (0), m_size_after (0), m_flags (0) {}

	virtual ~register_region_parms () {}

	static register_region_parms * create_parms (bool interpreter, std::istream * inp);
	static void destroy_parms (register_region_parms * parms);

	virtual void read_parms ()
		{
			region_parms::read_parms ();
			prompt_offset (); read_offset ();
			prompt_size_before (); read_size_before ();
			prompt_size_after (); read_size_after ();
			prompt_flags (); read_flags ();
		}

	klav_filepos_t get_offset () const { return m_offset; }
	uint32_t get_size_before () const { return m_size_before; }
	uint32_t get_size_after () const { return m_size_after; }
	uint32_t get_flags () const { return m_flags; }

protected:

	klav_filepos_t m_offset;
	uint32_t       m_size_before;
	uint32_t       m_size_after;
	uint32_t       m_flags;

	virtual void prompt_offset () = 0;
	virtual void prompt_size_before () = 0;
	virtual void prompt_size_after () = 0;
	virtual void prompt_flags () = 0;

	void read_offset () { m_offset = read_ul (m_inp, "error reading region offset"); }
	void read_size_before () { m_size_before = read_ul (m_inp, "error reading region size before"); }
	void read_size_after () { m_size_after = read_ul (m_inp, "error reading region size after"); }
	void read_flags () { m_flags = read_ul (m_inp, "error reading flags"); }
};

struct register_region_parms_interpreter : public register_region_parms
{
	register_region_parms_interpreter (std::istream * inp)
		: register_region_parms (inp) {}

	void prompt_type ()        { prompt ("input region type       "); }
	void prompt_id ()          { prompt ("input region identifier "); }
	void prompt_offset ()      { prompt ("input region offset     "); }
	void prompt_size_before () { prompt ("input region size before"); }
	void prompt_size_after ()  { prompt ("input region size after "); }
	void prompt_flags ()       { prompt ("input flags             "); }
};

struct register_region_parms_cmdline : public register_region_parms
{
	register_region_parms_cmdline (std::istream * inp)
		: register_region_parms (inp) {}

	void prompt_type () {}
	void prompt_id ()   {}
	void prompt_offset () {}
	void prompt_size_before () {}
	void prompt_size_after () {}
	void prompt_flags () {}
};

register_region_parms * register_region_parms::create_parms (bool interpreter, std::istream * inp)
{
	register_region_parms * parms = 0;
	if (interpreter)
		parms = new register_region_parms_interpreter (inp);
	else
		parms = new register_region_parms_cmdline (inp);
	return parms;
}

void register_region_parms::destroy_parms (register_region_parms * parms)
{
	delete parms;
}



////////////////////////////////////////////////////////////////////////////////
//
// write data to region
//
////////////////////////////////////////////////////////////////////////////////

class write_data2region_parms : public region_parms
{
public:
	write_data2region_parms (std::istream * inp)
		: region_parms (inp),
		m_offset (0), m_data (std::string()) {}

	virtual ~write_data2region_parms () {}

	static write_data2region_parms * create_parms (bool interpreter, std::istream * inp);
	static void destroy_parms (write_data2region_parms * parms);

	virtual void read_parms ()
		{
			region_parms::read_parms ();
			prompt_offset (); read_offset ();
			prompt_data (); read_data ();
		}

	klav_filepos_t get_offset () const { return m_offset; }
	const std::string& get_data () const { return m_data; }

protected:

	klav_filepos_t m_offset;
	std::string    m_data;

	virtual void prompt_offset () = 0;
	virtual void prompt_data () = 0;

	void read_offset () { m_offset = read_ul (m_inp, "error reading region offset"); }
	void read_data ()   { *m_inp >> m_data; check ("error reading data"); }
};

struct write_data2region_parms_interpreter : public write_data2region_parms
{
	write_data2region_parms_interpreter (std::istream * inp)
		: write_data2region_parms (inp) {}

	void prompt_type ()   { prompt ("input region type       "); }
	void prompt_id ()     { prompt ("input region identifier "); }
	void prompt_offset () { prompt ("input region offset     "); }
	void prompt_data ()   { prompt ("input data              "); }
};

struct write_data2region_parms_cmdline : public write_data2region_parms
{
	write_data2region_parms_cmdline (std::istream * inp)
		: write_data2region_parms (inp) {}

	void prompt_type () {}
	void prompt_id ()   {}
	void prompt_offset () {}
	void prompt_data () {}
};

write_data2region_parms * write_data2region_parms::create_parms (bool interpreter, std::istream * inp)
{
	write_data2region_parms * parms = 0;
	if (interpreter)
		parms = new write_data2region_parms_interpreter (inp);
	else
		parms = new write_data2region_parms_cmdline (inp);
	return parms;
}

void write_data2region_parms::destroy_parms (write_data2region_parms * parms)
{
	delete parms;
}


////////////////////////////////////////////////////////////////////////////////
//
// trace region params
//
////////////////////////////////////////////////////////////////////////////////

class trace_regions_parms : public parms_stream
{
public:
	trace_regions_parms (std::istream * inp)
		: parms_stream (inp), m_flags (0) {}

	virtual ~trace_regions_parms () {}

	static trace_regions_parms * create_parms (bool interpreter, std::istream * inp);
	static void destroy_parms (trace_regions_parms * parms);

	virtual void read_parms ()
		{
			prompt_flags (); read_flags ();
		}

	uint32_t get_flags () const { return m_flags; }

protected:

	uint32_t m_flags;

	void read_flags () { m_flags = read_ul (m_inp, "error reading flags"); }

	virtual void prompt_flags () = 0;
};

struct trace_regions_parms_interpreter : public trace_regions_parms
{
	trace_regions_parms_interpreter (std::istream * inp)
		: trace_regions_parms (inp) {}

	virtual void prompt_flags () { prompt ("input flags"); }
};

struct trace_regions_parms_cmdline : public trace_regions_parms
{
	trace_regions_parms_cmdline (std::istream * inp)
		: trace_regions_parms (inp) {}

	virtual void prompt_flags () {}
};

trace_regions_parms * trace_regions_parms::create_parms (bool interpreter, std::istream * inp)
{
	trace_regions_parms * parms = 0;
	if (interpreter)
		parms = new trace_regions_parms_interpreter (inp);
	else
		parms = new trace_regions_parms_cmdline (inp);
	return parms;
}

void trace_regions_parms::destroy_parms (trace_regions_parms * parms)
{
	delete parms;
}


////////////////////////////////////////////////////////////////////////////////
//
// region manager test
//
////////////////////////////////////////////////////////////////////////////////

static const char* exit_cmd           = "-e";
static const char* quit_cmd           = "-q";
static const char* dfile_cmd          = "-d";
static const char* dbuffile_cmd       = "-b";
static const char* reg_io_cmd         = "-rio";
static const char* reg_mem_cmd        = "-rmem";
static const char* map_cmd            = "-m";
static const char* write_cmd          = "-w";
static const char* commit_cmd         = "-c";
static const char* release_cmd        = "-r";
static const char* file_cmd           = "-p";
static const char* find_cmd           = "-f";
static const char* mark_cmd           = "-k";
static const char* trace_cmd          = "-t";
static const char* trace_all_cmd      = "-a";
static const char* trace_data_cmd     = "-td";
static const char* trace_src_data_cmd = "-tds";
static const char* trace_chg_data_cmd = "-tdc";
static const char* help_cmd           = "-h";

class rm_test
{
public:
	rm_test (KLAV_Alloc * allocator, int argc, char** argv);
	~rm_test ();

	int process ();

private:
	void register_io_region () const;
	void register_io_region (register_region_parms * parms) const;

	void register_mem_region ();
	void register_mem_region (register_region_parms * parms);

	void set_data_file ();
	void set_data_file (set_data_file_parms * parms);

	void set_data_buffer ();
	void set_data_buffer (set_data_file_parms * parms);

	void map_region () const;
	void map_region (region_parms_and_flags * parms) const;

	void commit_region () const;
	void commit_region (region_parms_and_flags * parms) const;

	void release_region () const;
	void release_region (region_parms_and_flags * parms) const;

	void find_region () const;
	void find_region (region_parms * parms) const;
	hKLAV_REGION find_region (uint32_t type, uint32_t id) const;

	void mark_region () const;
	void mark_region (region_parms_and_flags_mask * parms) const;

	void write_data2region () const;
	void write_data2region (write_data2region_parms * parms) const;

	void trace_all_regions () const;
	void trace_regions () const;
	void trace_regions (trace_regions_parms * parms) const;
	void trace_regions (uint32_t flags) const;
	void trace_region (hKLAV_REGION rgn) const;

	void trace_src_region_data () const;
	void trace_chg_region_data () const;
	void trace_region_data () const;
	void trace_region_data (uint32_t type, uint32_t id, uint32_t flags) const;

	void help () const;
	void cmd_line (const char * cmd, const char * comment) const;

	static void welcome();

private:

	void cleanup ();
	hKLAV_REGION search_region (uint32_t type, uint32_t id, uint32_t flags) const;

	enum { UNKNOWN_MODE, INTERPRETER_MODE, FILE_MODE, ARGSTR_MODE } m_mode;
	bool                      m_interpreter;
	std::istream *            m_inp;
	std::ifstream             m_cmd_file;

	KLAV_Region_Manager *     m_rm;

	std::vector<uint8_t>      m_buffer;
	KLAV_Alloc *              m_allocator;
	KLAV_IO_Library *         m_io_library;
	KLAV_IO_Object_Holder     m_io_object;
};

////////////////////////////////////////////////////////////////////////////////

void rm_test::welcome()
{
	std::cout << "Welcome to the region manager test interpreter." << std::endl;
}

////////////////////////////////////////////////////////////////////////////////

rm_test::rm_test (KLAV_Alloc * allocator, int argc, char** argv)
	: m_allocator (allocator), m_io_library (0)
{
	m_interpreter = (argc < 2) ? true : false;

	m_io_library = KLAVSYS_Get_IO_Library ();

	if (argc < 2)
	{
		// processing interactive command
		m_inp = &std::cin;

		m_mode = INTERPRETER_MODE;
	}
	else
	{
		if ((strcmp(argv[1], file_cmd) == 0  || strcmp (argv[1], file_cmd+1) == 0) && argc >= 3)
		{
			// processing command from file
			m_cmd_file.open (argv[2], std::ios::in | std::ios::binary);
			if (! m_cmd_file)
			{
				std::stringstream msg;
				msg << "can't open file " << argv[2];
				test_exception::raise (msg.str ());
			}
			m_inp = &m_cmd_file;

			m_mode = FILE_MODE;
		}
		else
		{
			m_inp = new std::stringstream;

			// processing command from argv[]
			for (int i = 1; i < argc; ++i)
			{
				*(static_cast<std::stringstream*>(m_inp)) << argv[i] << std::endl;
			}

			m_mode = ARGSTR_MODE;
		}
	}

	m_rm = KLAV_Region_Manager_Create (m_allocator, 0);

	if (m_io_object != 0)
	{
		KLAV_Region_Manager_Activate (m_rm, m_io_object);
	}

	if (m_mode == INTERPRETER_MODE) welcome();
}

////////////////////////////////////////////////////////////////////////////////

void rm_test::cleanup ()
{
	m_mode = UNKNOWN_MODE;
	m_interpreter = false;
	m_inp = 0;
	m_rm = 0;
}

////////////////////////////////////////////////////////////////////////////////

rm_test::~rm_test ()
{
	if (m_rm != 0)
	{
		KLAV_Region_Manager_Deactivate (m_rm);
		KLAV_Region_Manager_Destroy (m_rm);
		m_rm = 0;
	}

	m_io_object.clear ();

	if (m_mode == ARGSTR_MODE) delete m_inp;
	if (m_mode == FILE_MODE) m_cmd_file.close ();
	cleanup ();
}

////////////////////////////////////////////////////////////////////////////////
//
// find region
//
////////////////////////////////////////////////////////////////////////////////

void rm_test::find_region () const
{
	std::auto_ptr<region_parms> parms (region_parms::create_parms (m_interpreter, m_inp));
	parms->read_parms ();
	find_region (parms.get ());
}

void rm_test::find_region (region_parms * parms) const
{
	hKLAV_REGION rgn = find_region (parms->get_type (), parms->get_id ());
	trace_region (rgn);
}

hKLAV_REGION rm_test::find_region (uint32_t type, uint32_t id) const
{
	hKLAV_REGION rgn = m_rm->find_region (type, id);

	if (! rgn)
	{
		std::stringstream msg;
		msg << "region not found:" << " type=" << type << " id=" << id;
		test_exception::raise (msg.str ());
	}

	return rgn;
}


////////////////////////////////////////////////////////////////////////////////
//
// set data file
//
////////////////////////////////////////////////////////////////////////////////

void rm_test::set_data_file ()
{
	std::auto_ptr<set_data_file_parms> parms (set_data_file_parms::create_parms (m_interpreter, m_inp));
	parms->read_parms ();
	set_data_file (parms.get ());
}

void rm_test::set_data_file (set_data_file_parms * parms)
{
	bool readOnly = false;

	if (m_io_object.get () != 0)
	{
		KLAV_Region_Manager_Deactivate (m_rm);
		m_io_object.clear ();
	}
	
	uint32_t access = readOnly ? KLAV_IO_READONLY : KLAV_IO_READWRITE;
	uint32_t flags  = KLAV_FILE_OPEN_EXISTING;

	KLAV_ERR err = m_io_library->create_file (0, parms->get_file ().c_str (), access, flags, 0, &m_io_object.ptr ());

	if (KLAV_FAILED (err))
	{
		test_exception::raise ("can't open file " + parms->get_file ());
	}

	KLAV_Region_Manager_Activate (m_rm, m_io_object);
}

////////////////////////////////////////////////////////////////////////////////
//
// set data buffer file
//
////////////////////////////////////////////////////////////////////////////////

void rm_test::set_data_buffer ()
{
	std::auto_ptr<set_data_file_parms> parms (set_data_file_parms::create_parms (m_interpreter, m_inp));
	parms->read_parms ();
	set_data_buffer (parms.get ());
}

void rm_test::set_data_buffer (set_data_file_parms * parms)
{
	std::ifstream fs (parms->get_file ().c_str (), std::ios::in | std::ios::binary);
	if (! fs)
	{
		test_exception::raise ("can't open file " + parms->get_file ());
	}

	std::streamsize fsize = fs.seekg (0, std::ios::end).tellg();
	fs.seekg (0);

	m_buffer.resize (fsize);
	fs.read (reinterpret_cast<char*>(&m_buffer.front()), fsize);
}

////////////////////////////////////////////////////////////////////////////////
//
// register memory region
//
////////////////////////////////////////////////////////////////////////////////

void rm_test::register_mem_region ()
{
	std::auto_ptr<register_region_parms> parms (register_region_parms::create_parms (m_interpreter, m_inp));
	parms->read_parms ();
	register_mem_region (parms.get ());
}

void rm_test::register_mem_region (register_region_parms * parms)
{
	uint8_t* data = m_buffer.size () ? reinterpret_cast <uint8_t*> (&m_buffer[0]) + parms->get_offset () : 0;
	hKLAV_REGION hr = m_rm->register_mem_region (parms->get_type (),
												 parms->get_id (),
												 data,
												 parms->get_size_before (),
												 parms->get_size_after (),
												 parms->get_flags ());
	if (! hr)
	{
		std::stringstream msg;
		msg << "can't register memory region: ";
		msg << "type=" << parms->get_type ();
		msg << "; id=" << parms->get_id ();
		msg << "; offset=" << (long) (parms->get_offset ());
		msg << "; size before=" << parms->get_size_before ();
		msg << "; size after=" << parms->get_size_after ();
		msg << "; flags=" << parms->get_flags ();
		test_exception::raise (msg.str ());
	}
}


////////////////////////////////////////////////////////////////////////////////
//
// register io region
//
////////////////////////////////////////////////////////////////////////////////

void rm_test::register_io_region () const
{
	std::auto_ptr<register_region_parms> parms (register_region_parms::create_parms (m_interpreter, m_inp));
	parms->read_parms ();
	register_io_region (parms.get ());
}

void rm_test::register_io_region (register_region_parms * parms) const
{
	hKLAV_REGION hr = m_rm->register_io_region (parms->get_type (),
												parms->get_id (),
												parms->get_offset (),
												parms->get_size_before (),
												parms->get_size_after (),
												parms->get_flags ());
	if (! hr)
	{
		std::stringstream msg;
		msg << "can't register io region: ";
		msg << "type=" << parms->get_type ();
		msg << "; id=" << parms->get_id ();
		msg << "; offset=" << (long) parms->get_offset ();
		msg << "; size before=" << parms->get_size_before ();
		msg << "; size after=" << parms->get_size_after ();
		msg << "; flags=" << parms->get_flags ();
		test_exception::raise (msg.str ());
	}
}


////////////////////////////////////////////////////////////////////////////////
//
// map region
//
////////////////////////////////////////////////////////////////////////////////

void rm_test::map_region () const
{
	std::auto_ptr<region_parms_and_flags> parms (region_parms_and_flags::create_parms (m_interpreter, m_inp));
	parms->read_parms ();
	map_region (parms.get ());
}

void rm_test::map_region (region_parms_and_flags * parms) const
{
	hKLAV_REGION rgn = find_region (parms->get_type (), parms->get_id ());

	rgn = m_rm->map_region (rgn, parms->get_flags ());
	if (rgn == 0 || rgn == (hKLAV_REGION) (-1))
	{
		std::stringstream msg;
		msg << "error mapping region:" << " type=" << parms->get_type () << " id=" << parms->get_id ();
		test_exception::raise (msg.str ());
	}
}


////////////////////////////////////////////////////////////////////////////////
//
// commit region
//
////////////////////////////////////////////////////////////////////////////////

void rm_test::commit_region () const
{
	std::auto_ptr<region_parms_and_flags> parms (region_parms_and_flags::create_parms (m_interpreter, m_inp));
	parms->read_parms ();
	commit_region (parms.get ());
}

void rm_test::commit_region (region_parms_and_flags * parms) const
{
	hKLAV_REGION rgn = search_region (parms->get_type (), parms->get_id (), KLAV_RGN_F_WRITABLE);

	rgn = m_rm->commit_region (rgn, parms->get_flags ());
	if (! rgn)
	{
		std::stringstream msg;
		msg << "error commiting region:" << " type=" << parms->get_type () << " id=" << parms->get_id ();
		test_exception::raise (msg.str ());
	}
}

////////////////////////////////////////////////////////////////////////////////
//
// release region
//
////////////////////////////////////////////////////////////////////////////////

void rm_test::release_region () const
{
	std::auto_ptr<region_parms_and_flags> parms (region_parms_and_flags::create_parms (m_interpreter, m_inp));
	parms->read_parms ();
	release_region (parms.get ());
}

void rm_test::release_region (region_parms_and_flags * parms) const
{
	hKLAV_REGION rgn = search_region (parms->get_type (), parms->get_id (), KLAV_RGN_F_WRITABLE);

	rgn = m_rm->release_region (rgn, parms->get_flags ());
//	if (! rgn)
//	{
//		std::stringstream msg;
//		msg << "error releasing region:" << " type=" << parms->get_type () << " id=" << parms->get_id ();
//		test_exception::raise (msg.str ());
//	}
}

////////////////////////////////////////////////////////////////////////////////
//
// mark region
//
////////////////////////////////////////////////////////////////////////////////

void rm_test::mark_region () const
{
	std::auto_ptr<region_parms_and_flags_mask> parms (region_parms_and_flags_mask::create_parms (m_interpreter, m_inp));
	parms->read_parms ();
	mark_region (parms.get ());
}

void rm_test::mark_region (region_parms_and_flags_mask * parms) const
{
	hKLAV_REGION rgn = find_region (parms->get_type (), parms->get_id ());

	KLAV_ERR err = m_rm->mark_region (rgn, parms->get_mask (), parms->get_flags ());
	if (KLAV_FAILED (err))
	{
		std::stringstream msg;
		msg << "error marking region:" << " type=" << parms->get_type () << " id=" << parms->get_id ()
			<< " flags=" << parms->get_flags () << " mask=" << parms->get_mask ();
		test_exception::raise (msg.str ());
	}
}


////////////////////////////////////////////////////////////////////////////////
//
// write data to region
//
////////////////////////////////////////////////////////////////////////////////

void rm_test::write_data2region () const
{
	std::auto_ptr<write_data2region_parms> parms (write_data2region_parms::create_parms (m_interpreter, m_inp));
	parms->read_parms ();
	write_data2region (parms.get ());
}

void rm_test::write_data2region (write_data2region_parms * parms) const
{
	hKLAV_REGION rgn = find_region (parms->get_type (), parms->get_id ());
	rgn = m_rm->map_region (rgn, rgn->rgn_flags | KLAV_RGN_F_WRITABLE);

	std::vector<uint8_t> bin_data;
	if (hex2bin (reinterpret_cast<const uint8_t*>(parms->get_data ().data ()), parms->get_data ().size (), bin_data) < 0)
	if (hex2bin (reinterpret_cast<const uint8_t*>(parms->get_data ().data ()), parms->get_data ().size (), bin_data) < 0)
	{
		std::stringstream msg;
		msg << "error writing data to region:" << " type=" << parms->get_type () << " id=" << parms->get_id ();
		test_exception::raise (msg.str ());
	}

	memcpy (rgn->rgn_data - rgn->rgn_size_before, reinterpret_cast<const void*>(&bin_data[0]), bin_data.size ());
}

////////////////////////////////////////////////////////////////////////////////
//
// trace region
//
////////////////////////////////////////////////////////////////////////////////

void rm_test::trace_all_regions () const
{
	trace_regions (uint32_t(0));
}

void rm_test::trace_regions () const
{
	std::auto_ptr<trace_regions_parms> parms (trace_regions_parms::create_parms (m_interpreter, m_inp));
	parms->read_parms ();
	trace_regions (parms.get ());
}

void rm_test::trace_regions (trace_regions_parms * parms) const
{
	trace_regions (parms->get_flags ());
}

void rm_test::trace_regions (uint32_t flags) const
{
	hKLAV_REGION rgn = 0;
	for (size_t i = 0; ; ++i)
	{
		rgn = m_rm->get_next_region (rgn, flags);
		if (rgn)
		{
			if (i) std::cout << std::endl;
			std::cout << "**** region #" << i << std::endl;
			trace_region (rgn);
		}
		else
			break;
	}
}

void rm_test::trace_region (hKLAV_REGION rgn) const
{
	std::cout << std::dec;
	std::cout << "type  :" << rgn->rgn_type << std::endl;
	std::cout << "id    :" << rgn->rgn_id << std::endl;
	std::cout << "offset:" << (long) rgn->rgn_offset << std::endl;
	std::cout << "before:" << rgn->rgn_size_before << std::endl;
	std::cout << "after :" << rgn->rgn_size_after << std::endl;
//	std::cout << "data  :" << formatted_hex << (void *)rgn->rgn_data << std::endl;
	std::cout << "flags :" << formatted_hex << rgn->rgn_flags << std::endl;
}


////////////////////////////////////////////////////////////////////////////////
//
// search region
//
////////////////////////////////////////////////////////////////////////////////

hKLAV_REGION rm_test::search_region (uint32_t type, uint32_t id, uint32_t flags) const
{
	hKLAV_REGION rgn = 0;
	for (size_t i = 0; ; ++i)
	{
		rgn = m_rm->get_next_region (rgn, flags);
		if (rgn == 0 || (rgn->rgn_type == type && rgn->rgn_id == id))
			break;
	}
	return rgn;
}

////////////////////////////////////////////////////////////////////////////////
//
// trace region data
//
////////////////////////////////////////////////////////////////////////////////

void rm_test::trace_src_region_data () const
{
	std::auto_ptr<region_parms> parms (region_parms::create_parms (m_interpreter, m_inp));
	parms->read_parms ();
	trace_region_data (parms->get_type (), parms->get_id (), ~KLAV_RGN_F_WRITABLE);
}

void rm_test::trace_chg_region_data () const
{
	std::auto_ptr<region_parms> parms (region_parms::create_parms (m_interpreter, m_inp));
	parms->read_parms ();
	trace_region_data (parms->get_type (), parms->get_id (), KLAV_RGN_F_WRITABLE);
}

void rm_test::trace_region_data () const
{
	std::auto_ptr<region_parms_and_flags> parms (region_parms_and_flags::create_parms (m_interpreter, m_inp));
	parms->read_parms ();
	trace_region_data (parms->get_type (), parms->get_id (), parms->get_flags ());
}

void rm_test::trace_region_data (uint32_t type, uint32_t id, uint32_t flags) const
{
	hKLAV_REGION rgn = search_region (type, id, flags);

	if (! rgn)
	{
		std::stringstream msg;
		msg << "error: region [" << " type=" << type << " id=" << id
			<< " flags=" << formatted_hex << flags << "] not found";
		test_exception::raise (msg.str ());
	}

	if (! rgn->rgn_data)
	{
		std::stringstream msg;
		msg << "error: region [" << " type=" << type << " id=" << id << "] have not data";
		test_exception::raise (msg.str ());
	}

	hex_dump (std::cout, rgn->rgn_data - rgn->rgn_size_before,
						 rgn->rgn_size_after + rgn->rgn_size_before);
}


////////////////////////////////////////////////////////////////////////////////
//
// process
//
////////////////////////////////////////////////////////////////////////////////

int rm_test::process ()
{
	if (m_interpreter) std::cout << PROMPT_STRING;

	std::string cmd;
	while (*m_inp >> cmd)
	{
		try
		{
			if (cmd == exit_cmd || cmd == exit_cmd+1 || cmd == quit_cmd || cmd == quit_cmd+1)
			{
				if (m_interpreter) std::cout << std::endl;
				break;
			}
			else if (cmd == dfile_cmd || cmd == dfile_cmd+1)
			{
				set_data_file ();
			}
			else if (cmd == dbuffile_cmd || cmd == dbuffile_cmd+1)
			{
				set_data_buffer ();
			}
			else if (cmd == reg_io_cmd || cmd == reg_io_cmd+1)
			{
				register_io_region ();
			}
			else if (cmd == reg_mem_cmd || cmd == reg_mem_cmd+1)
			{
				register_mem_region ();
			}
			else if (cmd == map_cmd || cmd == map_cmd+1)
			{
				map_region ();
			}
			else if (cmd == write_cmd || cmd == write_cmd+1)
			{
				write_data2region ();
			}
			else if (cmd == commit_cmd || cmd == commit_cmd+1)
			{
				commit_region ();
			}
			else if (cmd == release_cmd || cmd == release_cmd+1)
			{
				release_region ();
			}
			else if (cmd == trace_cmd || cmd == trace_cmd+1)
			{
				trace_regions ();
			}
			else if (cmd == trace_all_cmd || cmd == trace_all_cmd+1)
			{
				trace_all_regions ();
			}
			else if (cmd == trace_data_cmd || cmd == trace_data_cmd+1)
			{
				trace_region_data ();
			}
			else if (cmd == trace_src_data_cmd || cmd == trace_src_data_cmd+1)
			{
				trace_src_region_data ();
			}
			else if (cmd == trace_chg_data_cmd || cmd == trace_chg_data_cmd+1)
			{
				trace_chg_region_data ();
			}
			else if (cmd == find_cmd || cmd == find_cmd+1)
			{
				find_region ();
			}
			else if (cmd == mark_cmd || cmd == mark_cmd+1)
			{
				mark_region ();
			}
			else if (cmd == help_cmd || cmd == help_cmd+1)
			{
				help ();
			}
			else
			{
				std::cout << "No such command '" << cmd << "'" << std::endl;
			}
		}
		catch (test_exception& ex)
		{
			std::cerr << ex.what() << std::endl;

			if (m_interpreter)
			{
				m_inp->clear ();

				char ch;
				while (m_inp->good ())
				{
					m_inp->get (ch);
					if( ch == '\n' ) break;
				}
			}
			else
				return 1;
		}

		if (m_interpreter) std::cout << PROMPT_STRING;
	}

	return 0;
}

////////////////////////////////////////////////////////////////////////////////
//
// help
//
////////////////////////////////////////////////////////////////////////////////

void rm_test::cmd_line (const char * cmd, const char * comment) const
{
	std::cerr << "  " << cmd << " " << comment << std::endl;
}

void rm_test::help () const
{
	std::cerr << "usage: rm_test [commands]" << std::endl;
	std::cerr << "commands are:" << std::endl;
	
	cmd_line (         dfile_cmd, "set data file");
	cmd_line (        reg_io_cmd, "register IO region");
	cmd_line (      dbuffile_cmd, "set memory data buffer file");
	cmd_line (       reg_mem_cmd, "register memory region");
	cmd_line (           map_cmd, "map region");
	cmd_line (         write_cmd, "write data to region");
	cmd_line (        commit_cmd, "commit region");
	cmd_line (       release_cmd, "release region");
	cmd_line (         trace_cmd, "trace regions");
	cmd_line (          find_cmd, "find region");
	cmd_line (          mark_cmd, "mark region");
	cmd_line (     trace_all_cmd, "trace all regions");
	cmd_line (    trace_data_cmd, "trace region data");
	cmd_line (trace_src_data_cmd, "trace source region data");
	cmd_line (trace_chg_data_cmd, "trace changing region data");
	cmd_line (          file_cmd, "perform command(s) from file");
	cmd_line (          exit_cmd, "exit");
	cmd_line (          quit_cmd, "quit");
	cmd_line (          help_cmd, "this help");
}
////////////////////////////////////////////////////////////////////////////////

int main (int argc, char** argv)
{
	try
	{
		KLAV_Malloc_Alloc allocator;
		rm_test t (&allocator, argc, argv);
		return t.process ();
	}
	catch (...)
	{
		std::cerr << "Unexpected error." << std::endl;
	}
	return -1;
}
