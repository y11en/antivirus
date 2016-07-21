// buffer_test.cpp
//
//

#include "klavstl_test.h"
#include <klava/klavstl/buffer.h>

////////////////////////////////////////////////////////////////
// klavstl::buffer

typedef klavstl::buffer <klavstl::malloc_allocator> test_buffer_t;

static bool check_buffer_contents (const test_buffer_t& buf, uint8_t val, size_t cnt)
{
	KLAVSTL_ASSERT (buf.size () == cnt);
	KLAVSTL_ASSERT (buf.empty () == (cnt == 0));
	if (cnt == 0)
		return true;

	test_buffer_t::pointer ptr = buf.data ();
	KLAVSTL_ASSERT (ptr != 0);

	const uint8_t * p = (const uint8_t *) ptr;

	for (size_t i = 0; i < cnt; ++i)
	{
		KLAVSTL_ASSERT (p [i] == val);
	}

	return true;
}

static bool check_buffer_contents (const test_buffer_t& buf, const char *str)
{
	size_t cnt = 0;
	if (str != 0)
		cnt = strlen (str);

	KLAVSTL_ASSERT (buf.size () == cnt);
	KLAVSTL_ASSERT (buf.empty () == (cnt == 0));
	if (cnt == 0)
		return true;

	test_buffer_t::pointer ptr = buf.data ();
	KLAVSTL_ASSERT (ptr != 0);

	const char * p = (const char *) ptr;

	for (size_t i = 0; i < cnt; ++i)
	{
		KLAVSTL_ASSERT (p [i] == str [i]);
	}

	return true;
}

#define STRDATA_A "A FIRST STRING..."
#define STRDATA_B "B: THE SECOND STRING!"
#define STRDATA_C "C - YET ANOTHER (THIRD) STRING:)"

const char * STR_A = STRDATA_A;
const char * STR_B = STRDATA_B;
const char * STR_C = STRDATA_C;

const char * STR_AB = STRDATA_A STRDATA_B;
const char * STR_AC = STRDATA_A STRDATA_C;
const char * STR_BC = STRDATA_B STRDATA_C;
const char * STR_ABC = STRDATA_A STRDATA_B STRDATA_C;


bool buffer_test ()
{
	int leak_flag = 0;
	{
		KLAV_HEAP_CHECKER (heap_checker, "klavstl::buffer<> : memory leaks detected\n", & leak_flag);

		test_buffer_t buf1;
		KLAVSTL_ASSERT (check_buffer_contents (buf1, 0, 0));

		test_buffer_t buf2 (15, 'a');
		KLAVSTL_ASSERT (check_buffer_contents (buf2, 'a', 15));

		test_buffer_t buf3 (STR_AB, strlen (STR_AB));
		KLAVSTL_ASSERT (check_buffer_contents (buf3, STR_AB));

		test_buffer_t buf4 (STR_BC, STR_BC + strlen (STR_BC));
		KLAVSTL_ASSERT (check_buffer_contents (buf4, STR_BC));

		////////////////////////////////////////////////////////
		// resize

		buf4.resize (strlen (STR_B));
		KLAVSTL_ASSERT (check_buffer_contents (buf4, STR_B));

		////////////////////////////////////////////////////////
		// assign

		buf1.assign (2000, 'A');
		KLAVSTL_ASSERT (check_buffer_contents (buf1, 'A', 2000));

		buf2.clear ();
		KLAVSTL_ASSERT (check_buffer_contents (buf2, ""));

		buf2.assign (STR_AB, strlen (STR_AB));
		KLAVSTL_ASSERT (check_buffer_contents (buf2, STR_AB));

		buf2.assign (STR_BC, STR_BC + strlen (STR_BC));
		KLAVSTL_ASSERT (check_buffer_contents (buf2, STR_BC));

		////////////////////////////////////////////////////////
		// swap

		buf1.swap (buf4);

		KLAVSTL_ASSERT (check_buffer_contents (buf1, STR_B));
		KLAVSTL_ASSERT (check_buffer_contents (buf4, 'A', 2000));

		////////////////////////////////////////////////////////
		// insert & erase

		test_buffer_t buf5;

		buf5.assign (STR_AC, STR_AC + strlen (STR_AC));
		KLAVSTL_ASSERT (check_buffer_contents (buf5, STR_AC));

		// insert in the middle
		buf5.insert (strlen (STR_A), STR_B, strlen (STR_B));
		KLAVSTL_ASSERT (check_buffer_contents (buf5, STR_ABC));

		// erase from the beginning
		buf5.erase (0, strlen (STR_A));
		KLAVSTL_ASSERT (check_buffer_contents (buf5, STR_BC));

		// insert at the beginning
		buf5.insert (buf5.begin (), STR_A, STR_A + strlen (STR_A));
		KLAVSTL_ASSERT (check_buffer_contents (buf5, STR_ABC));

		// erase at the end
		buf5.erase (buf5.begin () + strlen (STR_AB), buf5.end ());
		KLAVSTL_ASSERT (check_buffer_contents (buf5, STR_AB));

		// insert at the end
		buf5.insert (buf5.end (), STR_C, STR_C + strlen (STR_C));
		KLAVSTL_ASSERT (check_buffer_contents (buf5, STR_ABC));

		// erase from the middle
		buf5.erase (buf5.begin () + strlen (STR_A), buf5.begin () + strlen (STR_AB));
		KLAVSTL_ASSERT (check_buffer_contents (buf5, STR_AC));

		////////////////////////////////////////////////////////
		// push-back, pop-back, element access

		test_buffer_t buf6;

		const char *s = STR_AB;
		size_t i,len = strlen (s);

		for (i = 0; i < len; ++i)
		{
			buf6.push_back (s [i]);
		}
		KLAVSTL_ASSERT (check_buffer_contents (buf6, STR_AB));

		for (i = 0; i < len; ++i)
		{
			KLAVSTL_ASSERT (buf6 [i] == s [i]);
		}

		for (i = 0; i < len; ++i)
		{
			KLAVSTL_ASSERT (buf6.size () == len - i);
			buf6.pop_back ();
		}

		KLAVSTL_ASSERT (check_buffer_contents (buf6, ""));
	}

	return leak_flag == 0;
}

