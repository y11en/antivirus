// klavstl_test.h
//
//

#ifndef klavstl_test_h_INCLUDED
#define klavstl_test_h_INCLUDED

#include <kl_platform.h>
#include <stdio.h>

#ifdef _MSC_VER
# ifdef _DEBUG
#  define KLAV_DBGHEAP_MSC
#  define KLAV_HEAP_DEBUG
# endif // _DEBUG
#endif // _MSC_VER

////////////////////////////////////////////////////////////////
// MSVC implementation

#ifdef KLAV_DBGHEAP_MSC

#include <crtdbg.h>

class KLAV_Heap_Checker
{
public:
	KLAV_Heap_Checker (const char *stderr_msg, int *leak_flag_ptr)
		: m_stderr_msg (stderr_msg), m_leak_flag_ptr (leak_flag_ptr)
	{
		_CrtMemCheckpoint (&m_mem_state);
		if (m_leak_flag_ptr != 0)
			* m_leak_flag_ptr = 0;
	}

	~KLAV_Heap_Checker ()
	{
		if ((m_leak_flag_ptr != 0 || m_stderr_msg != 0) && has_leaks ())
		{
			if (m_leak_flag_ptr != 0)
				* m_leak_flag_ptr = 1;
			fputs (m_stderr_msg, stderr);
		}
		_CrtMemDumpAllObjectsSince (&m_mem_state);
	}

	bool has_leaks ()
	{
		_CrtMemState mem_state_now, mem_state_diff;
		_CrtMemCheckpoint (&mem_state_now);
		return _CrtMemDifference (&mem_state_diff, &m_mem_state, &mem_state_now) != 0;
	}

private:
	_CrtMemState m_mem_state;
	const char * m_stderr_msg;
	int *        m_leak_flag_ptr;

	KLAV_Heap_Checker (const KLAV_Heap_Checker&);
	KLAV_Heap_Checker& operator= (const KLAV_Heap_Checker&);
};

inline void KLAV_Heap_SetAllocBreak (unsigned long n)
{
	_CrtSetBreakAlloc (n);
}

#else  // KLAV_DBGHEAP_MSC

////////////////////////////////////////////////////////////////
// dummy implementation

class KLAV_Heap_Checker
{
public:
	KLAV_Heap_Checker (const char *stderr_msg, int * leak_flag_ptr) {}
	~KLAV_Heap_Checker () {}

private:
	KLAV_Heap_Checker (const KLAV_Heap_Checker&);
	KLAV_Heap_Checker& operator= (const KLAV_Heap_Checker&);
};

inline void KLAV_Heap_SetAllocBreak (unsigned long n) {}

#endif // KLAV_DBGHEAP_MSC

////////////////////////////////////////////////////////////////
// macros

#ifdef KLAV_HEAP_DEBUG

#define KLAV_HEAP_CHECKER(NAME,MSG,PFLAG)  KLAV_Heap_Checker NAME (MSG, PFLAG)
#define KLAV_HEAP_SET_ALLOC_BREAK(N)       KLAV_Heap_SetAllocBreak (N)

#else  // KLAV_HEAP_DEBUG

#define KLAV_HEAP_CHECKER(NAME,MSG,PFLAG)
#define KLAV_HEAP_SET_ALLOC_BREAK(N)

#endif // KLAV_HEAP_DEBUG


#define KLAVSTL_ASSERT(EXPR) \
	do { \
		if (! (EXPR)) { \
			printf ("assertion failed: %s at %s:%d\n", #EXPR, __FILE__, __LINE__); \
			return false; \
		} \
	} while (0)


////////////////////////////////////////////////////////////////
// test procedures

bool buffer_test ();
bool trivial_vector_test ();
bool vector_test ();

////////////////////////////////////////////////////////////////
// test object

class TestObject
{
public:
	TestObject ();
	TestObject (const TestObject&);
	explicit TestObject (int v);
	~TestObject ();

	TestObject& operator= (const TestObject&);
	TestObject& operator= (int v);

	struct Stats
	{
		int n_ctors;   // number of constructors called
		int n_cctors;  // number of copy constructors called
		int n_dtors;   // number of destructors called
		int n_assign;  // number of assignments performed
		int n_total;   // number of distinct objects created
	};

	int value () const
		{ return * m_data; }

	static void clear_stats ();
	static const Stats& get_stats ();

private:
	int * m_data;
};


////////////////////////////////////////////////////////////////

#endif // klavstl_test_h_INCLUDED


