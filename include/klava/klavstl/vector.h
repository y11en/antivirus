// vector.h
//

#ifndef vector_h_INCLUDED
#define vector_h_INCLUDED

#include <klava/klavstl/allocator.h>
#include <klava/klavstl/buffer.h>

namespace klavstl {

////////////////////////////////////////////////////////////////
// trivial_vector: vector for 'movable' objects

template <typename T, class Allocator>
class trivial_vector
{
public:
	DECLARE_STD_TYPEDEFS(T)

	explicit trivial_vector ()
		: m_buffer () {}

	explicit trivial_vector (const Allocator& a)
		: m_buffer (a) {}

	explicit trivial_vector (size_type n, const T& x = T(), const Allocator& a = Allocator ())
		: m_buffer (a) { assign (n, x); }

	template <class InputIterator>
	trivial_vector (InputIterator first, InputIterator last, const Allocator& a = Allocator ())
		: m_buffer (a) { assign (first, last); }

	trivial_vector (const trivial_vector <T, Allocator>& src)
		: m_buffer (src.get_allocator ()) { assign (src.begin (), (size_type)(src.end () - src.begin ())); }

	~trivial_vector ()
		{ destroy (); }

	const Allocator& get_allocator () const
		{ return m_buffer.get_allocator (); }

	void set_allocator (const Allocator& a)
		{ m_buffer.set_allocator (a);}

	void destroy ()
		{ clear (); m_buffer.destroy (); }

	void clear ()
		{ shrink (0); }

	size_type size () const
		{ return (size_type)(end () - begin ()); }

	size_type capacity () const
		{ return m_buffer.capacity () / sizeof (T); }

	bool empty () const
		{ return m_buffer.empty (); }

	void swap (trivial_vector <T, Allocator>& src)
		{ m_buffer.swap (src.m_buffer); }

	// iterators:
	iterator begin ()
		{ return (iterator)m_buffer.begin (); }

	const_iterator begin () const
		{ return (const_iterator)m_buffer.begin (); }

	iterator end ()
		{ return (iterator)m_buffer.end (); }

	const_iterator end() const
		{ return (const_iterator)m_buffer.end (); }

	// element access:
	reference operator [] (size_type n)
		{ return * (begin () + n); }

	const_reference operator [] (size_type n) const
		{ return * (begin () + n); }

	reference at (size_type n)
		{ return * (begin () + n); }

	const_reference at (size_type n) const
		{ return * (begin () + n); }

	reference front()
		{ return * begin (); }
	
	const_reference front() const
		{ return * begin (); }

	reference back()
		{ return * (end () - 1); }

	const_reference back() const
		{ return * (end () - 1); }

	bool reserve (size_type newcap)
		{ return m_buffer.reserve (newcap * sizeof (T)); }

	bool resize (size_type newsize, const T& x = T())
		{ return newsize >= size () ?
		    grow (newsize, x) : shrink (newsize);
		}

	trivial_vector <T,Allocator>& operator= (const trivial_vector <T,Allocator>& src)
		{ assign (src.begin (), (size_type)(src.end () - src.begin ())); return *this; }

	bool assign (size_type n, const T& x)
		{ clear ();
		  if (! m_buffer.resize (n * sizeof(T))) return false;
		  construct (begin(), end(), x);
		  return true;
		}

	// extension
	bool assign (const T* p, size_type n)
		{ clear ();
		  if (! m_buffer.resize (n * sizeof(T))) return false;
		  construct (begin(), end(), p);
		  return true;
		}

	template <class InputIterator>
	bool assign (InputIterator first, InputIterator last)
		{ return assign (first, last - first); }

	iterator insert (iterator position, const T& x)
		{ return insert (position, size_type (1), x); }

	iterator insert (iterator position, size_type n, const T& x)
		{ iterator p = (iterator)m_buffer.insert ((size_type)((position - begin ()) * sizeof (T)), 0, (size_type)(n * sizeof(T)));
		  if (p) construct (p, p+n, x);
		  return p;
		}

	// extension
	iterator insert (size_type idx, const T *src, size_type n)
		{ iterator p = (iterator)m_buffer.insert (idx * sizeof (T), 0, n * sizeof(T));
		  if (p) construct (p, p+n, src);
		  return p;
		}

	template <class InputIterator>
	iterator insert (iterator position, InputIterator first, InputIterator last)
		{ return insert (size_type (position - begin ()), (const T *)first, size_type (last - first)); }

	iterator erase (iterator position)
		{ return erase (position, 1); }

	iterator erase (iterator first, iterator last)
		{ return erase (first, last - first); }

	// extension
	iterator erase (iterator position, size_type n)
		{ destruct (position, position + n);
		  m_buffer.erase ((size_type)((position - begin ()) * sizeof (T)), (size_type)(n * sizeof (T)));
		  return position;
		}

	iterator push_back (const T& x)
		{ return insert (end(), x); }

	void pop_back ()
		{ erase (end() - 1); }

	// comparators
	bool operator== (const trivial_vector <T,Allocator>& src)
		{ size_type cnt = size (); if (cnt != src.size ()) return false;
		  const T * p1 = begin ();
		  const T * p2 = src.begin ();
		  for (size_type i = 0; i < cnt; ++i) {
			if (! (*p1++ != *p2++))
				return false;
		  }
		  return true;
		}

	bool operator!= (const trivial_vector <T,Allocator>& src)
		{ return ! (*this == src); }

	bool operator< (const trivial_vector <T,Allocator>& src)
		{ size_type cnt = size (), cnt2 = src.size ();
		  const T * p1 = begin ();
		  const T * p2 = src.begin ();
		  if (cnt > cnt2) cnt = cnt2;
		  for (size_type i = 0; i < cnt; ++i) {
			if (*p1++ < *p2++)
				return true;
		  }
		  return (size () < src.size ());
		}

	bool operator<= (const trivial_vector <T,Allocator>& src)
		{ size_type cnt = size (), cnt2 = src.size ();
		  const T * p1 = begin ();
		  const T * p2 = src.begin ();
		  if (cnt > cnt2) cnt = cnt2;
		  for (size_type i = 0; i < cnt; ++i) {
			if (*p1++ < *p2++)
				return true;
		  }
		  return (size () <= src.size ());
		}

	bool operator> (const trivial_vector <T,Allocator>& src)
		{ return src < *this; }

	bool operator>= (const trivial_vector <T,Allocator>& src)
		{ return src <= *this; }

protected:
	buffer <Allocator> m_buffer;

	void construct (T *from, T *to, const T& x)
		{ for (T *p = from; p < to; ++p) {
			new (p) T(x);
		  }
		}

	void construct (T *from, T *to, const T *src)
		{ for (T *p = from; p < to; ++p) {
			new (p) T(*src++);
		  }
		}

	void destruct (T *from, T *to)
		{ for (T *p = from; p < to; ++p) {
			p->~T();
		  }
		}

	bool grow (size_type newsize, const T& x)
		{ size_type oldsize = size ();
		  if (! m_buffer.resize (newsize * sizeof(T))) return false;
		  construct (begin() + oldsize, end(), x);
		  return true;
		}

	bool shrink (size_type newsize)
		{ destruct (begin() + newsize, end());
		  return m_buffer.resize (newsize * sizeof(T));
		}

	size_type grow_policy (size_type newcap)
		{ size_type oldcap = capacity ();
		  if (newcap > oldcap)
		  {
			size_type ncap = oldcap * 2;
			if (oldcap < 0x1000 && ncap > newcap) newcap = ncap;
		    newcap = (newcap + 0x0F) & ~((size_type)0x0F);
		  }
		  return newcap;
		}
};

////////////////////////////////////////////////////////////////
// vector

template <typename T, class Allocator>
class vector : public trivial_vector <T, Allocator>
{
public:
	typedef trivial_vector <T, Allocator> base;
	using base::get_allocator;

        DECLARE_PARENT_TYPEDEFS(base)

	explicit vector ()
		: base (Allocator ()) {}

	explicit vector (const Allocator& a)
		: base (a) {}

	explicit vector (size_type n, const T& x = T(), const Allocator& a = Allocator())
		: base (n, x, a) {}

	template <class InputIterator>
	vector (InputIterator first, InputIterator last, const Allocator& a = Allocator ())
		: base (first, last, a) {}

	// extension
	vector (const trivial_vector <T, Allocator>& src)
		: base (src) {}

	vector (const vector <T, Allocator>& src)
		: base (src) {}

	void swap (vector <T, Allocator>& src)
		{ base::swap (src); }

	bool reserve (size_type newcap)
		{ if (newcap > base::capacity ()) {
			trivial_vector <T,Allocator> tmp (get_allocator ());
			if (! tmp.reserve (newcap)
			 || ! tmp.assign (base::begin(), base::end() - base::begin())) 
				return false;
			tmp.swap (*this);
		  }
		  return true;
		}

	bool resize (size_type newsize, const T& x = T())
		{ return (reserve (grow_policy (newsize))
		       && base::resize (newsize, x));
		}

	vector <T,Allocator>& operator= (const vector <T,Allocator>& src)
		{ assign (src.begin (), src.end () - src.begin ()); return *this; }

	bool assign (size_type n, const T& x)
		{ return base::assign (n, x); }

	bool assign (const T* p, size_type n)
		{ return base::assign (p, n); }

	template <class InputIterator>
	bool assign (InputIterator first, InputIterator last)
		{ return assign (first, last - first); }

	iterator insert (iterator position, const T& x)
		{ return insert (position, 1, x); }

	iterator insert (iterator position, size_type n, const T& x)
		{
			size_type idx = position - base::begin ();
			size_type sz = base::size ();
			if (base::capacity () - sz >= n)
			{
// Using bulk resize puts DefaultConstructible restriction upon T, which is not OK
//				resize (sz + n);
//				iterator beg = base::begin ();
//				size_type i;
//				for (i = sz; i > idx; --i)
//					beg [i + n - 1] = beg [i - 1];
//				for (i = 0; i < n; ++i)
//					beg [idx + i] = x;

				iterator beg = base::begin ();

				if (n > sz - idx)
					resize(idx + n, x);

				for (size_type i = n > sz - idx ? n + idx - sz : 0; i < n; ++i)
					resize(sz + i + 1, beg[sz + i - n]);

				for (size_type i = sz; i-- > idx + n;)
					beg[i] = beg[i - n];

				for (size_type i = n > sz - idx ? sz : idx + n; i-- > idx;)
					beg[i] = x;
			}
			else
			{
				trivial_vector <T,Allocator> tmp (get_allocator ());
				if (! tmp.reserve (grow_policy (sz + n))) return 0;
				iterator beg = base::begin ();
				tmp.insert (tmp.begin (), beg, beg + idx);
				tmp.insert (tmp.begin () + idx, n, x);
				tmp.insert (tmp.begin () + (idx + n), beg + idx, base::end ());
				tmp.swap (*this);
			}
			return base::begin () + idx;
		}

	iterator insert (size_type idx, const T *src, size_type n)
		{
			size_type sz = base::size ();
			if (base::capacity () - sz >= n)
			{
				resize (sz + n);
				iterator beg = base::begin ();
				size_type i;
				for (i = sz; i > idx; --i)
					beg [i + n - 1] = beg [i - 1];
				for (i = 0; i < n; ++i)
					beg [idx + i] = src [i];
			}
			else
			{
				trivial_vector <T,Allocator> tmp (get_allocator ());
				if (! tmp.reserve (grow_policy (sz + n))) return 0;
				iterator beg = base::begin ();
				tmp.insert (tmp.begin (), beg, beg + idx);
				tmp.insert (idx, src, n);
				tmp.insert (tmp.begin () + (idx + n), beg + idx, base::end ());
				tmp.swap (*this);
			}
			return base::begin () + idx;
		}

	template <class InputIterator>
	iterator insert (iterator position, InputIterator first, InputIterator last)
		{ return insert (position - base::begin (), first, last - first); }

	iterator erase (iterator position)
		{ return erase (position, 1); }

	iterator erase (iterator position, size_type n)
		{ iterator pend = base::end ();
		  iterator p = position + n;
		  for (; p < pend; ++p) *(p - n) = *p;
		  size_type idx = position - base::begin ();
		  base::shrink(base::size () - n);
		  return base::begin () + idx;
		}

	iterator erase (iterator first, iterator last)
		{ return erase (first, last - first); }

	iterator push_back (const T& x)
		{ return insert (base::end(), x); }

	void pop_back ()
		{ erase (base::end() - 1); }
};

////////////////////////////////////////////////////////////////
// holder_vector

template <class T>
class null_destroyer
{
public:
	inline void operator () (T *) {}
};

template <class T>
class delete_destroyer
{
public:
	inline void operator () (T *p) { delete p; }
};

template <typename T, class Allocator, class Destroyer>
class holder_vector : public trivial_vector <T, Allocator>
{
public:
	typedef trivial_vector <T, Allocator> base;
        DECLARE_PARENT_TYPEDEFS(base)

	explicit holder_vector ()
		: base (Allocator ()), m_destroyer (Destroyer ()) {}

	explicit holder_vector (const Allocator& a)
		: base (a), m_destroyer (Destroyer ()) {}

	explicit holder_vector (const Allocator& a,	const Destroyer& d)
		: base (a), m_destroyer (d) {}

	explicit holder_vector (size_type n, const T& x = T(),
		const Allocator& a = Allocator(),
		const Destroyer& d = Destroyer ())
		: base (n, x, a), m_destroyer (d) {}

	template <class InputIterator>
	holder_vector (InputIterator first, InputIterator last, 
		const Allocator& a = Allocator (),
		const Destroyer& d = Destroyer ())
		: base (first, last, a), m_destroyer (d) {}

	~holder_vector ()
		{ destroy (); }

	void destroy ()
		{ clear (); base::destroy (); }

	void clear ()
		{ replace_items (base::begin (), base::end ()); base::clear (); }

	void clear (iterator position)
		{ replace_items (position, position + 1); }

	void clear (iterator position, size_type n)
		{ replace_items (position, position + n); }

	void release ()
		{ base::clear (); }

	T release (iterator position)
		{ T tmp = *position; *position = T(); return tmp; }

	void release (iterator position, size_type n)
		{ for (; n > 0; --n) *position++ = T(); }

	void swap (holder_vector <T, Allocator, Destroyer>& src)
		{ base::swap (src); }

	bool resize (size_type newsize)
		{ if (newsize < base::size ()) {
			  replace_items (base::begin () + newsize, base::end ());
		  }
		  return base::resize (newsize);
		}

	bool assign (size_type n, const T& x)
		{ clear (); return base::assign (n, x); }

	bool assign (const T *p, size_type n)
		{ clear (); return base::assign (p, n); }

	template <class InputIterator>
	bool assign (InputIterator first, InputIterator last)
		{ return assign (first, last - first); }

	iterator insert (iterator position, const T& x)
		{ return base::insert (position, x); }

	iterator insert (size_type idx, const T *p, size_type n)
		{ return base::insert (idx, p, n); }

	template <class InputIterator>
	iterator insert (iterator position, InputIterator first, InputIterator last)
		{ return base::insert (position, first, last); }

	iterator replace (iterator position, const T& x)
		{ replace_items (position, position + 1, x); return position; }

	iterator replace (size_type idx, const T* p, size_type n)
		{ replace_items (base::begin () + idx, base::begin () + idx + n, p); return p; }

	template <class InputIterator>
	iterator replace (iterator position, InputIterator first, InputIterator last)
		{ return replace_items (position, first, last-first); }

	iterator erase (iterator position)
		{ return erase (position, 1); }

	iterator erase (iterator position, size_type n)
		{ replace_items (position, position + n);
		  return base::erase (position, n);
		}

	iterator erase (iterator first, iterator last)
		{ return erase (first, last - first); }

	iterator push_back (const T& x)
		{ return insert (base::end(), x); }

	void pop_back ()
		{ erase (base::end() - 1); }

protected:
	Destroyer m_destroyer;

	void replace_items (iterator first, iterator last, const T& x = T())
		{ for (; first < last; ++first) {
			m_destroyer (*first);
			*first = x;
		  }
		}

	void replace_items (iterator first, iterator last, const T *p)
		{ for (; first < last; ++first) {
			m_destroyer (*first);
			*first = *p++;
		  }
		}

private:
	holder_vector (const holder_vector <T,Allocator,Destroyer>& src);
	holder_vector <T,Allocator,Destroyer>& operator= (const holder_vector <T,Allocator,Destroyer>& src);
};

} // namespace klavstl

#endif // vector_h_INCLUDED

