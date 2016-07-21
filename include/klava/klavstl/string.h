// string.h
//

#ifndef string_h_INCLUDED
#define string_h_INCLUDED

#include <klava/klavstl/allocator.h>

namespace klavstl {

template <class Allocator>
class string
{
public:
	DECLARE_STD_TYPEDEFS(char)
	typedef char char_type;

 static inline size_type length (const char_type *s)
	{ const char_type *p = s; if (p) while (*p) ++p; return size_type(p - s); }

	explicit string ()
		: m_data (""), m_size (0), m_capacity (0), m_allocator (Allocator ())
		{}

	explicit string (const Allocator& a)
		: m_data (""), m_size (0), m_capacity (0), m_allocator (a)
		{}

	string (const string <Allocator>& str)
		: m_data (""), m_size (0), m_capacity (0), m_allocator (str.get_allocator ())
		{ assign (str.c_str (), str.length ()); }

	string (const char_type *s, size_type n, const Allocator& a)
		: m_data (""), m_size (0), m_capacity (0), m_allocator (a)
		{ assign (s, n); }

	string (const char_type *s, const Allocator& a)
		: m_data (""), m_size (0), m_capacity (0), m_allocator (a)
		{ assign (s); }

//	string (size_type n, char_type c, const Allocator& a)
//		: m_data (""), m_size (0), m_capacity (0), m_allocator (a)
//		{ assign (n, c); }

//	template <class InputIterator>
//	string (InputIterator first, InputIterator last, const Allocator& a = Allocator ())
//		: m_data (""), m_size (0), m_capacity (0), m_allocator (a)
//		{ assign (first, last - first); }

	~string ()
		{ destroy (); }

	const allocator_type& get_allocator () const
		{ return m_allocator; }

	void set_allocator (const Allocator& a)
		{ m_allocator = a; } 

	void swap (string <Allocator>& b) {
			swap (m_data, b.m_data);
			swap (m_size, b.m_size);
			swap (m_capacity, b.m_capacity);
		}

	const char_type * c_str () const
		{ return m_data; }

	size_type size () const
		{ return m_size; }

	size_type length () const
		{ return m_size; }

	bool empty () const
		{ return m_size == 0; }

	size_type capacity () const
		{ return m_capacity; }

	bool reserve (size_type newcap);

	bool resize (size_type newsize)
		{ if (newsize != m_size) {
			if (newsize > m_capacity && ! grow (newsize)) return false;
			m_size = newsize;
			m_data [m_size] = 0;
		  }
		  return true;
		}

	void clear ()
		{ if (m_size != 0) { m_size = 0; m_data[0] = 0; } }

	void destroy ();

	string <Allocator>& operator= (const string <Allocator>& str)
		{ assign (str); return *this; }

	string <Allocator>& operator= (const char_type *str)
		{ assign (str); return *this; }

	string <Allocator>& operator= (char_type c)
		{ assign (1, c); return *this; }

	string <Allocator>& operator+= (const string <Allocator>& str)
		{ append (str); return *this; }

	string <Allocator>& operator+= (const char_type *str)
		{ append (str); return *this; }

	string <Allocator>& operator+= (char_type c)
		{ append (1, c); return *this; }

	iterator begin ()
		{ return (iterator)m_data; }

	const_iterator begin ()	const
		{ return (const_iterator)m_data; }

	iterator end ()
		{ return (iterator)(m_data + m_size); }

	const iterator end () const
		{ return (const_iterator)(m_data + m_size); }

	reference operator[] (size_type pos)
		{ return (reference)m_data [pos]; }

	const_reference operator[] (size_type pos) const
		{ return (const_reference)m_data [pos]; }

	bool assign (const string<Allocator>& str)
		{ return assign (str.c_str (), str.length ()); }

	bool assign (const char_type *s, size_type n);

	bool assign (const char_type *s)
		{ return assign (s, length (s)); }

	bool assign (size_type n, char_type c);

	bool append (const string<Allocator>& str)
		{ return append (str.c_str (), str.length ()); }

	bool append (const char_type *s, size_type n);

	bool append (const char_type *s)
		{ return append (s, length (s)); }

	bool append (size_type n, char_type c);

private:
	pointer   m_data;
	size_type m_size;
	size_type m_capacity;  // if zero - m_data points to static string
	Allocator m_allocator;

	bool grow (size_type newcap);
};

template <typename Allocator>
void string <Allocator>::destroy ()
{
	m_size = 0;
	if (m_capacity != 0) {
		m_allocator.free (m_data);
		m_capacity = 0;
	}
	m_data = 0;
}

template <class Allocator>
bool string<Allocator>::grow (size_type newcap)
{
	if (newcap < m_capacity * 2) newcap = m_capacity * 2;
	return reserve (newcap);
}

template <typename Allocator>
bool string <Allocator>::reserve (size_type newcap)
{
	if (newcap <= m_capacity) return true;
	if (newcap < 0x10) newcap = 0x10;
	pointer p = (m_capacity == 0) 
		? (pointer)m_allocator.alloc ((newcap+1)*sizeof(char_type))
		: (pointer)m_allocator.realloc (m_data, (newcap+1)*sizeof(char_type));
	if (! p) return false;
	m_data = p;
	m_capacity = newcap;
	return true;
}

template <class Allocator>
bool string<Allocator>::assign (const char_type *s, size_type n)
{
	if (! resize (n)) return false;
	memcpy (m_data, s, n * sizeof (char_type));
	return true;
}

template <class Allocator>
bool string<Allocator>::assign (size_type n, char_type c)
{
	if (! resize (n)) return false;
	for (size_type i = 0; i < n; ++i) m_data [i] = c;
	return true;
}

template <class Allocator>
bool string<Allocator>::append (const char_type *s, size_type n)
{
	size_type oldsize = m_size;
	if (! resize (oldsize + n)) return false;
	memcpy (m_data + oldsize, s, n * sizeof (char_type));
	return true;
}

template <class Allocator>
bool string<Allocator>::append (size_type n, char_type c)
{
	size_type oldsize = m_size;
	if (! resize (oldsize + n)) return false;
	for (size_type i = 0; i < n; ++i) m_data [oldsize + i] = c;
	return true;
}

} // namespace klavstl

#endif // string_h_INCLUDED


