/*!
*		
*		
*		(C) 2002 Kaspersky Lab 
*		
*		\file	scoped_handle.hpp
*		\brief	
*		
*		\author Владислав Овчарик
*		\date	21.10.2003 17:13:13
*		
*		
*		
*/		

#ifndef __SCOPED_HANDLE_INCLUDED__
#define __SCOPED_HANDLE_INCLUDED__
///////////////////////////////////////////////////////////////////////////////
namespace util
{
///////////////////////////////////////////////////////////////////////////////
/**
 * \struct	scoped_handle
 * \brief	...
 */
template
<
	typename Handle,
	typename Pfn,
	Pfn close,
	int Unique,
	int null = 0
>
class scoped_handle
{
public:
	scoped_handle()
		:	h_((Handle)null)
	{}

	explicit scoped_handle(Handle h)
		:	h_(h)
	{}

	~scoped_handle()
	{
		if (h_ != (Handle)null)
			close(h_);
	}

	Handle& get_ref()
	{
		return h_;
	}

	Handle get() const
	{
		return h_;
	}

	void reset(Handle h)
	{
		scoped_handle(h).swap(*this);
	}

	Handle release()
	{
		Handle t(h_);
		h_ = (Handle)null;
		return t;
	}

	void swap(scoped_handle& other)
	{
		std::swap(h_, other.h_);
	}

private:
	scoped_handle(const scoped_handle&);
	scoped_handle& operator= (const scoped_handle&);
	typedef Handle(scoped_handle::*unspecified_bool_type)();

public:
	operator unspecified_bool_type() const
	{
		return (Handle)null == h_ ? false : &scoped_handle::release;
	}

private:
	Handle h_;
};
///////////////////////////////////////////////////////////////////////////////
}// namespace util
///////////////////////////////////////////////////////////////////////////////
#endif // __SCOPED_HANDLE_INCLUDED__