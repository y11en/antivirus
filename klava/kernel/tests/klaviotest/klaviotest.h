// klaviotest.h
//
//

#ifndef klaviotest_h_INCLUDED
#define klaviotest_h_INCLUDED

#include "test_utils.h"

#include <klava/klavdef.h>
#include <klava/klavsys.h>
#include <klava/klav_utils.h>

bool io_lib_init ();

void do_io_open (const char *fname, bool rdonly);
void do_io_close ();

void do_vio (unsigned long offset, bool on);

void do_io_read (
		unsigned long offset,
		uint32_t size,
		const char *dump_fname,
		bool use_map,
		bool portion,
		bool hex
	);

void do_io_write (
		unsigned long offset,
		const uint8_t *data,
		uint32_t size,
		bool portion
	);

void do_io_get_size ();
void do_io_set_size (unsigned long size);

bool do_check_results (unsigned int cnt, const unsigned long *r);
bool do_compare_file (const char *fname1, const char *fname2);

#endif // klaviotest_h_INCLUDED


