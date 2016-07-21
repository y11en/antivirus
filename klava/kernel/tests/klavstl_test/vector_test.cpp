// vector_test.cpp
//

#include "klavstl_test.h"
#include <klava/klavstl/vector.h>

typedef klavstl::vector <TestObject, klavstl::malloc_allocator> test_vector_t;

static int VDATA_1[] = { 1, 1, 1, 1, 1, 1, 1, 1, 0 };
static int VDATA_A[] = { 1, 2, 3, 4, 5, 6, 7, 8, 0 };

static size_t vdlen (const int *vd)
{
	size_t cnt = 0;
	if (vd != 0) {
		while (vd [cnt] != 0) ++cnt;
	}
	return cnt;
}

static bool check_vector_contents (test_vector_t& vec, const int * data)
{
	size_t len = vdlen (data);
	KLAVSTL_ASSERT (vec.size () == len);

	for (size_t i = 0; i < len; ++i)
	{
		KLAVSTL_ASSERT (vec [i].value () == data [i]);
	}

	return true;
}

static bool check_vector_contents_n (test_vector_t& vec, size_t len)
{
	KLAVSTL_ASSERT (vec.size () == len);

	for (size_t i = 0; i < len; ++i)
	{
		KLAVSTL_ASSERT (vec [i].value () == (int)(i + 1));
	}

	return true;
}

static bool check_vector_contents_nc (test_vector_t& vec, size_t len, int val)
{
	KLAVSTL_ASSERT (vec.size () == len);

	for (size_t i = 0; i < len; ++i)
	{
		KLAVSTL_ASSERT (vec [i].value () == val);
	}

	return true;
}

//static void print_vector_contents (test_vector_t& vec, const char *name)
//{
//	printf ("vector contents (%s):\n", name);
//	size_t cnt = vec.size ();
//	for (size_t i = 0; i < cnt; ++i)
//	{
//		printf ("name [%d] = %d\n", i, vec [i].value ());
//	}
//}

bool vector_test ()
{
	int leak_flag = 0;
	{
		KLAV_HEAP_CHECKER (heap_checker, "klavstl::vector<> : memory leaks detected\n", & leak_flag);


		TestObject::clear_stats ();

		// constructors

		test_vector_t vec1;
		KLAVSTL_ASSERT (vec1.empty ());
		KLAVSTL_ASSERT (check_vector_contents (vec1, 0));

		// creating default element (value == 1), call copy constructor 8 times
		test_vector_t vec2 (8);
		KLAVSTL_ASSERT (check_vector_contents (vec2, VDATA_1));
		KLAVSTL_ASSERT (TestObject::get_stats ().n_ctors == 1);
		KLAVSTL_ASSERT (TestObject::get_stats ().n_cctors == 8);
		KLAVSTL_ASSERT (TestObject::get_stats ().n_dtors == 1);

		TestObject::clear_stats ();

		// fill vector using push_back
		size_t i;
		for (i = 0; i < 8; ++i)
		{
			vec1.push_back (TestObject ((int)i + 1));
			KLAVSTL_ASSERT (vec1.size () == i + 1);
		}
//		print_vector_contents (vec1, "vec1");
		KLAVSTL_ASSERT (check_vector_contents (vec1, VDATA_A));
//		KLAVSTL_ASSERT (TestObject::get_stats ().n_ctors == 8);
//		KLAVSTL_ASSERT (TestObject::get_stats ().n_dtors == 8);
//		KLAVSTL_ASSERT (TestObject::get_stats ().n_cctors == 8);

		TestObject::clear_stats ();

		// create copy of vector, using pair of iterators
		test_vector_t vec3 (vec1.begin (), vec1.end ()); // WAS: ctor using pair of iterators
		KLAVSTL_ASSERT (check_vector_contents (vec3, VDATA_A));
		KLAVSTL_ASSERT (TestObject::get_stats ().n_ctors == 0);
		KLAVSTL_ASSERT (TestObject::get_stats ().n_cctors == 8);
		KLAVSTL_ASSERT (TestObject::get_stats ().n_dtors == 0);

		TestObject::clear_stats ();

		// create copy of vector, using vector object
		test_vector_t vec4 (vec1);
		KLAVSTL_ASSERT (check_vector_contents (vec4, VDATA_A));
		KLAVSTL_ASSERT (TestObject::get_stats ().n_ctors == 0);
		KLAVSTL_ASSERT (TestObject::get_stats ().n_cctors == 8);
		KLAVSTL_ASSERT (TestObject::get_stats ().n_dtors == 0);

		// test large reallocations
		TestObject::clear_stats ();

		test_vector_t vec5;

		size_t CNT = 10000;

		for (i = 0; i < CNT; ++i)
		{
			vec5.push_back (TestObject (i + 1));
		}
		KLAVSTL_ASSERT (check_vector_contents_n (vec5, CNT));
//		KLAVSTL_ASSERT (TestObject::get_stats ().n_ctors == CNT);
//		KLAVSTL_ASSERT (TestObject::get_stats ().n_cctors == CNT);
//		KLAVSTL_ASSERT (TestObject::get_stats ().n_dtors == CNT);

		// test large reallocations using resize ()
		TestObject::clear_stats ();

		test_vector_t vec6;
		vec6.resize (CNT);
		KLAVSTL_ASSERT (check_vector_contents_nc (vec6, CNT, 1));
		KLAVSTL_ASSERT (TestObject::get_stats ().n_ctors == 1);
		KLAVSTL_ASSERT (TestObject::get_stats ().n_cctors == CNT);
		KLAVSTL_ASSERT (TestObject::get_stats ().n_dtors == 1);

		// grow...
		TestObject::clear_stats ();

		vec6.resize (CNT * 2);
		KLAVSTL_ASSERT (check_vector_contents_nc (vec6, CNT * 2, 1));
		KLAVSTL_ASSERT (TestObject::get_stats ().n_ctors == 1);
		KLAVSTL_ASSERT (TestObject::get_stats ().n_cctors == CNT * 2);
		KLAVSTL_ASSERT (TestObject::get_stats ().n_dtors == CNT + 1);

		// shrink...
		TestObject::clear_stats ();
		vec6.resize (CNT);
		KLAVSTL_ASSERT (check_vector_contents_nc (vec6, CNT, 1));
		KLAVSTL_ASSERT (TestObject::get_stats ().n_ctors == 1);
		KLAVSTL_ASSERT (TestObject::get_stats ().n_cctors == 0);
		KLAVSTL_ASSERT (TestObject::get_stats ().n_dtors == CNT + 1);

		// clear...
		TestObject::clear_stats ();
		vec6.clear ();
		KLAVSTL_ASSERT (check_vector_contents (vec6, 0));
		KLAVSTL_ASSERT (TestObject::get_stats ().n_ctors == 0);
		KLAVSTL_ASSERT (TestObject::get_stats ().n_cctors == 0);
		KLAVSTL_ASSERT (TestObject::get_stats ().n_dtors == CNT);

		// insert/erase
		// prepare source vector (1..999)
		for (i = 1; i < 1000; ++i)
			vec6.push_back (TestObject (i));

		KLAVSTL_ASSERT (check_vector_contents_n (vec6, 999));

		TestObject::clear_stats ();

		// construct vector: (100...199)
		test_vector_t vec7 (vec6.begin () + 99, vec6.begin () + 199);
		vec7.insert (vec7.begin (), vec6.begin (), vec6.begin () + 99);

		KLAVSTL_ASSERT (check_vector_contents_n (vec7, 199));

		vec7.insert (vec7.begin () + 199, vec6.begin () + 199, vec6.begin () + 899);
		KLAVSTL_ASSERT (check_vector_contents_n (vec7, 899));

		vec7.erase (vec7.begin () + 599, vec7.begin () + 799);
		KLAVSTL_ASSERT (vec7.size () == 699);

		vec7.erase (vec7.begin () + 599, vec7.end ());
		KLAVSTL_ASSERT (check_vector_contents_n (vec7, 599));

		vec7.insert (vec7.begin (), vec6.begin (), vec6.begin () + 100);
		KLAVSTL_ASSERT (vec7.size () == 699);
		
		vec7.erase (vec7.begin () + 10, vec7.begin () + 100);
		vec7.erase (vec7.begin (), vec7.begin () + 10);
		KLAVSTL_ASSERT (check_vector_contents_n (vec7, 599));
	}

	return leak_flag == 0;
}

