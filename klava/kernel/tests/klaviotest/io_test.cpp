// io_test.cpp
//

#include "klaviotest.h"

KLAV_Malloc_Alloc g_malloc;

KLAV_IO_Library * g_io_library = 0;

KLAV_IO_Object_Holder g_io_object;

KLAV_IO_Object * g_vio = 0;
KLAV_IO_Object * g_io = 0;

unsigned long last_result [10];
unsigned int result_cnt = 0;

bool io_lib_init ()
{
	g_io_library = KLAVSYS_Get_IO_Library ();
	if (! g_io_library)
		return false;

	return true;
}

void clear_results ()
{
	result_cnt = 0;
	for (int i = 0; i < sizeof (last_result) / sizeof (last_result [0]); ++i)
	{
		last_result [i] = 0;
	}
}

void set_results (unsigned long r0)
{
	result_cnt = 1;
	last_result [0] = r0;
	printf ("%lu\n", r0);
}

void set_results (unsigned long r0, unsigned long r1)
{
	result_cnt = 2;
	last_result [0] = r0;
	last_result [1] = r1;
	printf ("%lu %lu\n", r0, r1);
}

void set_results (unsigned long r0, unsigned long r1, unsigned long r2)
{
	result_cnt = 3;
	last_result [0] = r0;
	last_result [1] = r1;
	last_result [2] = r2;
	printf ("%lu %lu %lu\n", r0, r1, r2);
}

void do_io_open (const char *fname, bool rdonly)
{
	clear_results ();

	g_io = 0;
	g_io_object.clear ();

	uint32_t access = rdonly ? KLAV_IO_READONLY : KLAV_IO_READWRITE;
	uint32_t flags = rdonly ? KLAV_FILE_OPEN_EXISTING : KLAV_FILE_CREATE_ALWAYS;

	KLAV_ERR err = g_io_library->create_file (0, fname, access, flags, 0, & g_io_object.ptr ());
	set_results (err);

	if (g_io_object.get () != 0)
	{
		g_io = g_io_object;
	}
}

void do_io_close ()
{
	clear_results ();
	g_io_object.clear ();
}

void do_vio (unsigned long offset, bool on)
{
	clear_results ();

	if (g_vio != 0)
	{
		g_vio->destroy ();
		g_vio = 0;
	}

	if (on)
	{
		KLAV_Create_Virtual_IO (&g_malloc, g_io, offset, &g_vio);
		g_io = g_vio;
		set_results (g_vio != 0 ? KLAV_OK : KLAV_EUNKNOWN);
	}
}

void do_io_read (
		unsigned long offset,
		uint32_t size,
		const char *dump_fname,
		bool use_map,
		bool portion,
		bool hex
	)
{
	uint8_t *buf = 0;
	uint32_t nrd = size;

	klav_iomap_t iomap;

	clear_results ();

	if (use_map)
	{
		// using read_iobuf
		uint32_t flags = 0;
		if (portion)
			flags |= KLAV_IOMAP_PARTIAL;

		KLAV_ERR err = g_io->map_data ((klav_filepos_t) offset, size, flags, &iomap);

		if (KLAV_FAILED (err))
		{
			set_results (err, 0, 0);
			nrd = 0;
		}
		else
		{
			set_results (err, iomap.iomap_size, iomap.iomap_flags);
			buf = (uint8_t *) iomap.iomap_data;
			nrd = iomap.iomap_size;
		}
	}
	else
	{
		// using seek_read
		buf = new uint8_t [size];

		KLAV_ERR err = g_io->seek_read ((klav_filepos_t) offset, buf, size, portion ? &nrd : 0);

		if (KLAV_FAILED (err))
			nrd = 0;

		set_results (err, nrd, 0);
	}

	if (dump_fname != 0 && dump_fname[0] != 0)
	{
		if (hex)
		{
			if (! hex_dump_file (dump_fname, buf, nrd))
				printf ("error writing to file: %s\n", dump_fname);
		}
		else
		{
			if (! write_file (dump_fname, buf, nrd))
				printf ("error writing to file: %s\n", dump_fname);
		}
	}

	if (use_map)
	{
		g_io->unmap_data (&iomap);
	}
	else
	{
		delete [] buf;
	}
}

void do_io_write (unsigned long offset, const uint8_t *data, uint32_t size, bool partial)
{
	uint32_t nwr = size;

	clear_results ();

	KLAV_ERR err = g_io->seek_write ((klav_filepos_t) offset, data, size, partial ? &nwr : 0);
	if (KLAV_FAILED (err))
		nwr = 0;

	set_results (err, nwr);
}

void do_io_get_size ()
{
	klav_filepos_t size = 0;

	clear_results ();
	KLAV_ERR err = g_io->get_size (& size);

	set_results (err, (unsigned long)size);
}

void do_io_set_size (unsigned long size)
{
	clear_results ();
	KLAV_ERR err = g_io->set_size ((klav_filepos_t) size);

	set_results (err);
}

bool do_check_results (unsigned int cnt, const unsigned long *r)
{
	if (cnt != result_cnt)
		return false;

	for (unsigned int i = 0; i < cnt; ++i)
	{
		if (r [i] != last_result [i])
			return false;
	}

	return true;
}

bool do_compare_file (const char *fname1, const char *fname2)
{
	buffer_t data1, data2;

	if (! read_file (fname1, data1))
		return false;

	if (! read_file (fname2, data2))
		return false;

	if (data1.size () != data2.size ())
		return false;

	if (memcmp (&data1[0], &data2[0], data1.size ()) != 0)
		return false;

	return true;
}
