// main.cpp (klavstl_test)
//
//

#include "klavstl_test.h"
#include <string.h>

////////////////////////////////////////////////////////////////

static TestObject::Stats g_stats;

TestObject::TestObject ()
	: m_data (0)
{
	m_data = new int (++ g_stats.n_total);
	++g_stats.n_ctors;
}

TestObject::TestObject (const TestObject& a)
	: m_data (0)
{
	m_data = new int (* a.m_data);
	++g_stats.n_cctors;
}

TestObject::TestObject (int v)
	: m_data (0)
{
	m_data = new int (v);
	++g_stats.n_ctors;
}

TestObject::~TestObject ()
{
	delete m_data;
	m_data = 0;
	++g_stats.n_dtors;
}

TestObject& TestObject::operator= (const TestObject& a)
{
	delete m_data;
	m_data = new int (* a.m_data);
	++g_stats.n_assign;
	return *this;
}

TestObject& TestObject::operator= (int v)
{
	delete m_data;
	m_data = new int (v);
	++g_stats.n_assign;
	return *this;
}

void TestObject::clear_stats ()
{
	memset (& g_stats, 0, sizeof (g_stats));
}

const TestObject::Stats& TestObject::get_stats ()
{
	return g_stats;
}

////////////////////////////////////////////////////////////////

int main (int argc, char *argv[])
{
	if (! buffer_test ())
	{
		printf ("klavstl::buffer<> test failed\n");
		return 1;
	}

	if (! trivial_vector_test ())
	{
		printf ("klavstl::trivial_vector<> test failed\n");
		return 1;
	}

	if (! vector_test ())
	{
		printf ("klavstl::vector<> test failed\n");
		return 1;
	}

	printf ("klavstl tests OK\n");

	return 0;
}

