/** 
 * @file
 * @brief	Implementation of AuthToken interface.
 * @author	Andrey Guzhov
 * @date	19.04.2007
 */

#ifndef _AG_AUTHTOKENIMP_H_
#define _AG_AUTHTOKENIMP_H_

#include "../include/AuthToken.h"

namespace OnlineActivation
{
/**
 * Secure allocator class.
 * Fills memory with zeros before deallocation.
 */
template<typename T> class secure_allocator
{
public:
	typedef typename std::allocator<T>::value_type		value_type;
	typedef typename std::allocator<T>::pointer			pointer;
	typedef typename std::allocator<T>::reference		reference;
	typedef typename std::allocator<T>::const_pointer	const_pointer;
	typedef typename std::allocator<T>::const_reference	const_reference;

	typedef typename std::allocator<T>::size_type		size_type;
	typedef typename std::allocator<T>::difference_type	difference_type;

	template<typename U> struct rebind
	{
		typedef secure_allocator<U> other;
	};
	pointer address(reference value) const
	{ 
		return m_alloc.address(value); 
	}
	const_pointer address(const_reference value) const
	{ 
		return m_alloc.address(value); 
	}
	secure_allocator() throw()
	{
	}
	secure_allocator(const secure_allocator<T>& other) throw()
		: m_alloc(other.m_alloc)
	{
	}
	template<typename C>
	secure_allocator(const secure_allocator<C>& other) throw()
		: m_alloc(other.m_alloc)
	{
	}
	template<typename C>
	secure_allocator<T>& operator= (const secure_allocator<C>& other)
	{
		m_alloc = other.m_alloc;
		return (*this);
	}
	void deallocate(pointer ptr, size_type count)
	{
		// zero unused memory
		SecureZeroMemory(ptr, count * sizeof(T));
		m_alloc.deallocate(ptr, count);
	}
	pointer allocate(size_type count)
	{	
		return m_alloc.allocate(count);
	}
	pointer allocate(size_type count, const void* hint)
	{
		return m_alloc.allocate(count, hint);
	}
	void construct(pointer ptr, const T& value)
	{
		m_alloc.construct(ptr, value);
	}
	void destroy(pointer ptr)
	{
		m_alloc.destroy(ptr);
	}
	size_type max_size() const throw()
	{
		return m_alloc.max_size();
	}
private:
	std::allocator<T>	m_alloc;
};

template<typename T, typename C>
inline bool operator== (const secure_allocator<T>&, const secure_allocator<C>&) throw()
{
	return true;
}

template<typename T, typename C>
inline bool operator!= (const secure_allocator<T>&, const secure_allocator<C>&) throw()
{
	return false;
}

/**
 * Secure buffer, based on vector.
 */
typedef std::vector<unsigned char, secure_allocator<unsigned char> > secure_buffer;

/**
 * Implementation of AuthToken interface.
 */
class AuthTokenImp : public AuthToken
{
public:

	/**
	 * Sets credentials.
	 */
	virtual void SetCredentials(const char* userName, const char* passWord);

	/**
	 * Returns true if no credentials are set.
	 */
	bool IsEmpty() const;

	/**
	 * Returns user name in a secure data buffer.
	 */
	void GetUserName(secure_buffer& buf) const;

	/**
	 * Returns password in a secure data buffer.
	 */
	void GetPassword(secure_buffer& buf) const;

private:

	/**
	 * User name stored in the encrypted form.
	 */
	std::vector<unsigned char> m_user;

	/**
	 * Password stored in the encrypted form.
	 */
	std::vector<unsigned char> m_pass;
};

} // namespace OnlineActivation

#endif // _AG_AUTHTOKENIMP_H_
