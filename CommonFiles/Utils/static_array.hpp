/*!
*		
*		
*		(C) 2003 Kaspersky Lab 
*		
*		\file	static_array.hpp
*		\brief	
*		
*		\author Владислав Овчарик
*		\date	14.02.2004 14:51:24
*		
*		
*/		
#if !defined(_STATIC_ARRAY_INCLUDE_H_)
#define _STATIC_ARRAY_INCLUDE_H_
#include <iterator>
///////////////////////////////////////////////////////////////////////////////
namespace util
{
///////////////////////////////////////////////////////////////////////////////
template<typename T>
struct static_array
{
	typedef T		value_type;
	typedef T*		iterator;
	typedef const T*	const_iterator;
	typedef T&		reference;
	typedef const T&	const_reference;
	typedef size_t    	size_type;
	typedef ptrdiff_t 	difference_type;

#if _MSC_VER > 1200
	typedef std::reverse_iterator<iterator> reverse_iterator;
	typedef std::reverse_iterator<const_iterator> const_reverse_iterator;
#else
	typedef std::reverse_iterator<iterator,T> reverse_iterator;
	typedef std::reverse_iterator<const_iterator,T> const_reverse_iterator;
#endif

	static_array()
		:	elems_(0)
		,	size_(0)
	{
	}
	static_array(T* f, T* l)
		:	elems_(f)
		,	size_(l - f)
	{
	}
	static_array(T* p, size_t size)
		:	elems_(p)
		,	size_(size)
	{
	}
	T* reset(T* f, T* l)
	{
		T* p = elems_;
		elems_ = f;
		size_ = l - f;
		return p;
	} 
	iterator begin() { return elems_; }
	const_iterator begin() const { return elems_; }
	iterator end() { return elems_ + size_; }
	const_iterator end() const { return elems_ + size_; }
	
	reverse_iterator rbegin() { return reverse_iterator(end()); }
	const_reverse_iterator rbegin() const
	{
		return const_reverse_iterator(end());
	}
	reverse_iterator rend() { return reverse_iterator(begin()); }
	const_reverse_iterator rend() const
	{
		return const_reverse_iterator(begin());
	}

	reference front() { return elems_[0]; }
	const_reference front() const { return elems_[0]; }
	reference back() { return elems_[size_ - 1]; }
	const_reference back() const { return elems_[size_ - 1]; }

	size_type size() const { return size_; }
	bool empty() const { return size() == 0; }
	size_type max_size() const { return size_; }
	reference operator [] (size_t pos) { return elems_[pos]; }
	const_reference operator [] (size_t pos) const { return elems_[pos]; }
	reference at(size_t pos)
	{
		if(pos > size_)
			throw std::out_of_range("pos exceeds the size of static array");
		return elems_[pos];
	}
	const_reference at(size_t pos) const
	{
		if(pos > size_)
			throw std::out_of_range("pos exceeds the size of static array");
		return elems_[pos];
	}
private:
	T*		elems_;
	size_t	size_;
};
///////////////////////////////////////////////////////////////////////////////
}//namespace util
///////////////////////////////////////////////////////////////////////////////
#endif//!defined(_STATIC_ARRAY_INCLUDE_H_)