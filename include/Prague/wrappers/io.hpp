/*!
*		
*		
*		(C) 2003 Kaspersky Lab 
*		
*		\file	IO.hpp
*		\brief	шаблонная обертка, над интерфейсом hIO в стиле STL
*				обладает так же поведением a.k.a auto_ptr
*				в настоящий момент реализация итератов не может быть
*				использована в модифицирующих алгоритмах.
*		
*		\author Владислав Овчарик
*		\date	20.02.2005 9:41:56
*		
*		
*/		
#pragma once
#include <Prague/iface/i_io.h>

#include <boost/type_traits.hpp>
#include <iterator>
///////////////////////////////////////////////////////////////////////////////
namespace prague
{
//////////////////////////////////////////////////////////////////////////////
using ::hIO;
//////////////////////////////////////////////////////////////////////////////
template<typename T> class IO;
//////////////////////////////////////////////////////////////////////////////
/**
 * Ссылка на IO
 */
template<typename T>
struct IORef
{
	IORef(IO<T>& ref)
		: _ref(ref)
	{
	}
	IO<T>&	_ref;
};
//////////////////////////////////////////////////////////////////////////////
template<typename T>
struct WeakIO
{
	explicit WeakIO(hIO hio)
		: io_(hio)
	{
	}
	~WeakIO()
	{
		io_.release();
	}
	WeakIO(WeakIO<T> const& other)
		: io_(other.io_.get())
	{
	}
	WeakIO<T>& operator = (WeakIO<T> const& other)
	{
		if(this != &other)
		{
			io_.release();
			io_.reset(other.io_.get());
		}
		return *this;
	}
	IO<T>& io() { return io_; }
	IO<T> const& io() const { return io_; }
private:
	IO<T>	io_;
};
//////////////////////////////////////////////////////////////////////////////
/**
 * IO - обертка, над интерфейсом hIO в стиле STL
 * 		обладает так же поведением a.k.a auto_ptr
 * T  - должен быть представлен строго как POD структура
 */
template<typename T>
class IO
{
public:
	typedef unsigned __int64	size_type;
	typedef T					value_type;
	typedef size_type			difference_type;
	typedef T					reference;
	typedef T*					pointer;
	class const_iterator;
	friend class const_iterator;
	class const_iterator
#if _MSC_VER < 1300
		: public std::_Ranit<value_type, difference_type>
#else
		: public std::_Ranit<value_type, difference_type, pointer, reference>
#endif
	{
		friend class IO<value_type>;
	public:
		const_iterator()
			{}
		const_iterator(const_iterator const& other)
			: pos_(other.pos_)
			, io_(other.io_)
			{}
		value_type operator * () const
			{ return IO<T>::do_create(io_, pos_); }
		const_iterator& operator ++ ()
			{ ++pos_; return *this;}
		const_iterator operator ++ (int)
			{
				const_iterator tmp(*this);
				++(*this);
				return tmp;
			}
		const_iterator& operator -- ()
			{ --pos_; return *this; }
		const_iterator operator -- (int)
			{
				const_iterator tmp(*this);
				--(*this);
				return tmp;
			}
		const_iterator& operator -= (size_type offset)
			{ pos_ -= offset; return *this; }
		const_iterator& operator += (size_type offset)
			{ pos_ += offset; return *this; }
		const_iterator operator + (size_type pos) const
			{
				const_iterator tmp(*this);
				tmp += pos;
				return tmp;
			}
		const_iterator operator - (size_type pos) const
			{
				const_iterator tmp(*this);
				tmp -= pos;
				return tmp;
			}
		size_type operator - (const_iterator const& rhs) const
			{ return this->position() - rhs.position(); }
		bool operator == (const_iterator const& other) const
			{	return other.pos_ == this->pos_ && other.io_ == this->io_; }
		bool operator != (const_iterator const& other) const
			{ return !(*this == other); }
		bool operator < (const const_iterator& other) const
			{ return this->pos_ < other.pos_; }
		bool operator > (const const_iterator& other) const
			{ return this->pos_ > other.pos_; }
		bool operator <= (const const_iterator& other) const
			{ return !(*this > other); }
		bool operator >= (const const_iterator& other) const
			{ return !(*this < other); }
		size_type position() const { return pos_; }
	protected:
		const_iterator(size_type pos, IO<T>* io)
			: pos_(pos)
			, io_(io)
		{}
	private:
		size_type	pos_;
		IO			*io_;
	};
	class iterator
		: public const_iterator
	{
		friend class IO<value_type>;
	public:
		iterator(){}
		iterator(iterator const& other)
			: const_iterator(other)
		{}
		value_type operator * () const
			{ return **(static_cast<const_iterator const*>(this)); }
		iterator& operator ++ ()
			{ ++*static_cast<const_iterator*>(this); return *this;}
		iterator operator ++ (int)
			{
				iterator tmp(*this);
				++(*this);
				return tmp;
			}
		iterator& operator -- ()
			{ --*static_cast<const_iterator*>(this); return *this;}
		iterator operator -- (int)
			{
				iterator tmp(*this);
				--(*this);
				return tmp;
			}
		iterator& operator -= (size_type offset)
			{ *static_cast<const_iterator*>(this) -= offset; return *this; }
		iterator& operator += (size_type offset)
			{ *static_cast<const_iterator*>(this) += offset; return *this; }
		iterator operator + (size_type pos) const
			{
				iterator tmp(*this);
				tmp += pos;
				return tmp;
			}
		iterator operator - (size_type pos) const
			{
				iterator tmp(*this);
				tmp -= pos;
				return tmp;
			}
		size_type operator - (iterator const& rhs) const
			{ return this->position() - rhs.position(); }
	protected:
		iterator(size_type pos, IO<T>* io)
			: const_iterator(pos, io)
		{}
	};
#if _MSC_VER > 1200
	typedef std::reverse_iterator<iterator>				reverse_iterator;
	typedef std::reverse_iterator<const_iterator>		const_reverse_iterator;
#else
	typedef std::reverse_iterator<iterator, T, T>		reverse_iterator;
	typedef std::reverse_iterator<const_iterator,T, T>	const_reverse_iterator;
#endif
	//!
	explicit IO(hIO hio)
		: hio_(hio)
	{
		BOOST_STATIC_ASSERT(boost::is_pod<T>::value);
	}
#if _MSC_VER > 1200
	IO(IO<T>& other)
		: hio_(other.release())
	{
	}
#else
	IO(IO<T>& other)
		: hio_(other.release())
	{
	}
#endif
	IO(IORef<T>& other)
		: hio_(other._ref.release())
	{
	}
#if _MSC_VER > 1200
	template<typename Other>
	IO(IO<Other>& other)
		: hio_(other.release())
	{
	}
#endif
	template<typename Other>
	operator IORef<Other> ()
	{
		return IORef<Other>(*this);
	}
	template<typename Other>
	operator IO<Other> ()
	{
		return IO<Other>(*this);
	}
	IO& operator = (IO& other)
	{
		this->reset(other.release());
	}
	~IO()
	{
		if(hio_)
			hio_->sysCloseObject();
	}
	hIO release()
	{
		hIO tmp(hio_);
		hio_ = 0;
		return tmp;
	}
	hIO get() const { return hio_; }
	void reset(hIO hio)
	{
		if(hio != hio_ && hio_ != 0)
			hio_->sysCloseObject();
		hio_ = hio;
	}	
	//! return number of object of type T
	size_type size() const
	{
		return size(hio_);
	}
	iterator begin() { return iterator(0, this); }
	iterator end() { return iterator(this->size(), this); }
	const_iterator begin() const { return const_iterator(0, const_cast<IO*>(this)); }
	const_iterator end() const { return const_iterator(this->size(), const_cast<IO*>(this)); }
	reverse_iterator rbegin() { return reverse_iterator(this->end()); }
	reverse_iterator rend() { return reverse_iterator(this->begin()); }
	const_reverse_iterator rbegin() const { return const_reverse_iterator(this->end()); }
	const_reverse_iterator rend() const { return const_reverse_iterator(this->begin()); }
	bool empty() const { return this->size() == 0; }
	value_type operator [](size_type pos) const { return *(this->begin() + pos); }
	value_type at(size_type pos) const
	{
		if(pos > this->size())
			throw std::out_of_range("position out of range");
		return *(this->begin() + pos);
	}
	value_type front() { return *this->begin(); }
	value_type back() { return *(this->end() - 1); }
	void push_back(T const& val) { this->insert(this->end(), val); }
	void pop_back() { this->erase(this->end() - 1); }
	void pop_front() { this->erase(this->begin()); }
	template<typename _It>
	void assign(_It f, _It l)
	{
		this->erase(this->begin(), this->end());
		this->insert(this->end(), f, l);
	}
	void assign(size_type n, T const& val = T())
	{
		this->erase(this->begin(), this->end());
		this->insert(this->end(), n, val);
	}
	void write(iterator p, T const& val)
	{
		this->do_write(this, p.position(), val);
	}
	iterator insert(iterator p, T const& val)
	{
		size_type sz(p - this->begin());
		this->insert(p, 1, val);
		return this->begin() + sz;
	}
	void insert(iterator p, size_type n, T const& val)
	{
		iterator _end(this->end());
		size_type last(_end - p);
		if(last > n)
		{
			//1 копируем последние sz объектов в конец IO
			this->copy2end(_end - n, _end);
			//2 используем алгоритм copy_backward для перемещения блока [p, _end - n)
			this->copy_backward(p, _end - n, _end);
		}
		else
		{
			size_type sz(this->size());
			do_setsize(this, sz + n - last);//выдкляем необходимую память
			this->copy2end(p, _end);
		}
		//3 заполняем интервал [p, p + sz) значением val
		this->fill(p, p + n, val);
	}
	template<typename _It>
	void insert(iterator p, _It f, _It l)
	{
		this->do_insert(p, f, l, static_cast<_It*>(0));
	}
	iterator erase(iterator p)
	{
		size_type sz(this->size());
		this->copy(p + 1, this->end(), p);
		do_setsize(this, sz - 1);
		return p;
	}
	iterator erase(iterator f, iterator l)
	{
		size_type sz(this->size());
		size_type dist(std::distance(f, l));
		this->copy(l, this->end(), f);
		do_setsize(this, sz - dist);
		return f;
	}
	void clear() { this->erase(this->begin(), this->end()); }
	void swap(IO<T>& other)
	{
		std::swap(hio_, other.hio_);
	}
	static size_type size(hIO hio)
	{
		size_type bytes(0);
		hio->GetSize(&bytes, IO_SIZE_TYPE_EXPLICIT);
		return bytes / sizeof(T);
	}
	friend class iterator;
private:
	class insert_mode_normal  {};
	class insert_mode_chunk  {};
	//! копирует интервал в конец IO
	void copy2end(iterator f, iterator l)
	{
		for(; f != l; ++f)
		{
			size_type _end_pos(this->size());
			IO<T>::do_write(this, _end_pos, *f);
		}
	}
	iterator copy_backward(iterator f, iterator l, iterator e)
	{
		while(f != l)
			IO<T>::do_write(this, (--e).position(), *--l);
		return e;
	}
	void fill(iterator f, iterator l, T const& val)
	{
		for(; f != l; ++f)
			IO<T>::do_write(this, f.position(), val);
	}
	template<typename _It>
	void copy(_It f, _It l, iterator p)
	{
		for(;f != l; ++f, ++p)
			do_write(this, p.position(), *f);
	}
	template <>
	void copy(char const* f, char const* l, iterator p)
	{
		do_write(this, p.position(), f, l);
	}
	static void do_write(IO* io, size_type pos, char const* f, char const* l)
	{
		size_t result(0);
		void const* buffer = f;
		size_type offset(pos * sizeof(T));
		io->hio_->SeekWrite(&result, offset, const_cast<void*>(buffer), l - f);
	}
	static void do_write(IO* io, size_type pos, T const& value)
	{
		size_t result(0);
		void const* buffer = &value;
		size_type offset(pos * sizeof(T));
		io->hio_->SeekWrite(&result, offset, const_cast<void*>(buffer), sizeof(T));	
	}
	static T do_create(IO* io, size_type pos)
	{
		size_type bytes(pos * sizeof(T));
		size_t result(0);
		T value;
		char buffer[sizeof(T)] = { 0 };
		//todo: проверить код возврата
		io->hio_->SeekRead(&result, pos, &value, sizeof(T));
		return value;
	}
	static void do_setsize(IO* io, size_type sz)
	{
		size_type new_size(sz * sizeof(T));
		io->hio_->SetSize(new_size);
	}
	template<typename _It>
	void do_insert(iterator p, _It f, _It l, _It*)
	{
		typename std::iterator_traits<_It>::distance_type dist(std::distance(f, l));
		iterator _end(this->end());
		size_type last(std::distance(p, _end));
		if(last > dist)
		{
			this->copy2end(_end - dist, _end);
			this->copy_backward(p, _end - dist, _end);	
		}
		else
		{
			size_type sz(this->size());
			do_setsize(this, sz + dist - last);//выдкляем необходимую память
			this->copy2end(p, _end);
		}
		this->copy(f, l, p);
	}
	//template<>
	void do_insert(iterator p, char const* f, char const* l, const char**)
	{
		ptrdiff_t dist(l - f);
		iterator _end(this->end());
		size_type last(std::distance(p, _end));
		if(last > dist)
		{
			this->copy2end(_end - dist, _end);
			this->copy_backward(p, _end - dist, _end);	
		}
		else
		{
			size_type sz(this->size());
			do_setsize(this, sz + dist - last);//выдкляем необходимую память
			this->copy2end(p, _end);
		}
		this->copy(f, l, p);	
	}
private:
	hIO	hio_;
};
///////////////////////////////////////////////////////////////////////////////
}//namespace prague
///////////////////////////////////////////////////////////////////////////////
