// buffer.h
//

#ifndef klavstl_membuf_h_INCLUDED
#define klavstl_membuf_h_INCLUDED

#include <klava/klavstl/allocator.h>

namespace klavstl {

#ifdef KLAVA
#	define klavstl_memset  klav_memset
#	define klavstl_memcpy  klav_memcpy
#	define klavstl_memmove klav_memmove
#else // KLAVA
#	define klavstl_memset  memset
#	define klavstl_memcpy  memcpy
#	define klavstl_memmove memmove
#endif // KLAVA

////////////////////////////////////////////////////////////////
// buffer

template<class Allocator>
class buffer
{
public:
	DECLARE_STD_TYPEDEFS(uint8_t)

	explicit buffer ()
		: m_data (0), m_size (0), m_capacity (0), m_allocator (Allocator ()) {}

	explicit buffer (const Allocator& a)
		: m_data (0), m_size (0), m_capacity (0), m_allocator (a) {}

	explicit buffer (size_type n, const value_type& x, const Allocator& a = Allocator())
		: m_data (0), m_size (0), m_capacity (0), m_allocator (a)
			{ assign (n, x); }

	explicit buffer (const void *p, size_type n, const Allocator& a = Allocator ())
		: m_data (0), m_size (0), m_capacity (0), m_allocator (a)
			{ assign (p, n); }

	explicit buffer (const void *pbeg, const void *pend, const Allocator& a = Allocator ())
		: m_data (0), m_size (0), m_capacity (0), m_allocator (a)
			{ assign (pbeg, pend); }

	buffer (const buffer <Allocator>& b)
		: m_data (0), m_size (0), m_capacity (0), m_allocator (b.m_allocator)
			{ assign (b.data(), b.size()); }

	~buffer ()
		{ destroy (); }

	const allocator_type& get_allocator () const
		{ return m_allocator; }

	void set_allocator (const Allocator& a)
		{ m_allocator = a; } 

	void clear ()
		{ m_size = 0; }

	bool empty () const
		{ return m_size == 0; }

	void destroy ();

	void swap (buffer <Allocator>& b) {
			klavstl::swap (m_data, b.m_data);
			klavstl::swap (m_size, b.m_size);
			klavstl::swap (m_capacity, b.m_capacity);
		}

	bool reserve (size_type newcap);

	bool resize (size_type newsize) {
			if (newsize > m_capacity && ! grow (newsize)) return false;
			m_size = newsize;
			return true;
		}

	buffer <Allocator>& operator= (const buffer <Allocator>& src)
		{ assign (src.data (), src.size ()); return *this; }

	bool assign (size_type size, const value_type& x);
	bool assign (const void *data, size_type size);

	bool assign (const void *pbeg, const void *pend)
		{ return assign (pbeg, (const uint8_t *)pend - (const uint8_t *)pbeg); }

	pointer data () const
		{ return m_data; }

	size_type size () const
		{ return m_size;}

	size_type capacity () const
		{ return m_capacity;}

	// iterators:
	iterator begin ()
		{ return m_data; }

	const_iterator begin () const
		{ return m_data; }

	iterator end ()
		{ return m_data + m_size; }

	const_iterator end() const
		{ return m_data + m_size; }

	// element access:
	reference operator[] (size_type n)
		{ return m_data[n]; }

	const_reference operator[] (size_type n) const
		{ return m_data[n]; }

	reference front()
		{ return m_data [0]; }
	
	const_reference front() const
		{ return m_data [0]; }

	reference back()
		{ return m_data [m_size - 1]; }

	const_reference back() const		
		{ return m_data [m_size - 1]; }

	// modifiers:
	iterator push_back(const value_type& x)
		{ if (! resize (size () + 1)) return 0;
		  iterator p = end () - 1;
		  *p = x;
		  return p;
		}

	void pop_back()
		{ resize (size () - 1); }

	iterator insert (size_type idx, const void *data, size_type size);

	iterator insert (iterator pos, const void *pbeg, const void *pend)
		{ return insert (pos - begin (), pbeg, (const uint8_t *)pend - (const uint8_t *)pbeg); }

	iterator insert(iterator pos, const value_type& x)
		{ return insert (pos, 1, x); }
		
	iterator insert(iterator pos, size_type n, const value_type& x);

	void erase (size_type pos, size_type size);

	iterator erase (iterator position)
		{ erase (position - m_data, 1); return position; }
		
	iterator erase (iterator first, iterator last)
		{ erase (first - m_data, last - first); return first; }

private:
	pointer   m_data;
	size_type m_size;
	size_type m_capacity;
	Allocator m_allocator;

	bool grow (size_type newcap);
};

template <typename Allocator>
void buffer <Allocator>::destroy ()
{
	m_size = 0;
	if (m_capacity != 0) {
		m_allocator.free (m_data);
		m_capacity = 0;
		m_data = 0;
	}
}

template <typename Allocator>
bool buffer <Allocator>::grow (size_type newcap)
{
	if (newcap < m_capacity * 2) newcap = m_capacity * 2;
	return reserve (newcap);
}

template <typename Allocator>
bool buffer <Allocator>::reserve (size_type newcap)
{
	if (newcap <= m_capacity) return true;
	if (newcap < 0x10) newcap = 0x10;
	pointer p = (m_capacity == 0)
		? (pointer)m_allocator.alloc (newcap)
		: (pointer)m_allocator.realloc (m_data, newcap);
	if (! p) return false;
	m_data = p;
	m_capacity = newcap;
	return true;
}

template <typename Allocator>
typename buffer <Allocator>::iterator
buffer <Allocator>::insert (size_type pos, const void *data, size_type size)
{
	if (! resize (m_size + size)) return 0;
	klavstl_memmove (m_data + pos + size, m_data + pos, m_size - pos - size);
	if (data) klavstl_memcpy (m_data + pos, data, size);
	return m_data + pos;
}

template <typename Allocator>
void buffer <Allocator>::erase (size_type pos, size_type size)
{
	klavstl_memcpy (m_data + pos, m_data + pos + size, m_size - pos - size);
	resize (m_size - size);
}

template <typename Allocator>
bool buffer <Allocator>::assign (const void *data, size_type n)
{
	if (! resize (n))
		return false;
	if (m_data != data)
		klavstl_memcpy (m_data, data, n);
	return true;
}

template <typename Allocator>
bool buffer <Allocator>::assign (size_type n, const value_type& x)
{
	if (! resize (n))
		return false;
	klavstl_memset (m_data, x, n);
	return true;
}

template <typename Allocator>
typename buffer <Allocator>::iterator
buffer <Allocator>::insert (iterator pos, size_type n, const value_type& x)
{
	iterator p = insert (pos - m_data, 0, n);
	if (p != 0)
		klavstl_memset (p, x, n);
	return p;
}

} // namespace klavstl

#endif // klavstl_membuf_h_INCLUDED
