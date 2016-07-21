/** 
 * @file
 * @brief	Implementation of InternetHandle class.
 * @author	Andrey Guzhov
 * @date	18.04.2007
 */

#ifndef _AG_INTERNETHANDLE_H_
#define _AG_INTERNETHANDLE_H_

#include <stdexcept>
#include <boost/intrusive_ptr.hpp>
#include <utils/local_mutex.hpp>

namespace boost
{
	template <typename T> inline void intrusive_ptr_add_ref(T* p) 
		{ p->addref(); }
	template <typename T> inline void intrusive_ptr_release(T* p) 
		{ p->release(); }
}  // namespace boost

namespace OnlineActivation
{
/**
 * Reference counted internet handle.
 * This wrapper class provides a way of storing reference counted WinInet handles hierarchy.
 */
class InternetHandle
{
	friend class WinInternet;
	typedef DWORD (__stdcall *unspecified_bool_type)();

public:

	/**
	 * Constructs NULL-handle object.
	 */
	InternetHandle(){}

	/**
	 * Returns NULL when handle is NULL-handle.
	 */
    operator unspecified_bool_type() const
		{ return child ? &GetLastError : 0; }

private:

	/**
	 * Constructs root internet handle.
	 */
	explicit InternetHandle(HINTERNET hInternet)
		: child(new handle(hInternet)){}

	/**
	 * Constructs child internet handle.
	 */
	InternetHandle(const InternetHandle& hParent, HINTERNET hInternet)
		: parent(hParent.child)
		, child(new handle(hInternet)){}

	/**
	 * Returns WinInet handle value.
	 */
	operator HINTERNET() const
		{ return child ? child->get() : 0; }

	/**
	 * Forcibly closes internet handle.
	 */
	void Close()
		{ child ? child->reset() : 0; }

private:

	/**
	 * Reference counted WinInet handle wrapper.
	 */
	class handle
	{
	public:

		/**
		 * Constructs unassigned handle object.
		 */
		handle()
			: h_(0)
			, c_(0){}

		/**
		 * Constructs handle object and assigns it to WinInet handle.
		 */
		handle(HINTERNET h)
			: h_(h)
			, c_(0){}

		/**
		 * Destroys object and closes WinInet handle if set.
		 */
		~handle()
			{ h_ ? InternetCloseHandle(h_) : void(0); }

		/**
		 * Intrusive pointer add reference.
		 */
		long addref()
			{ return InterlockedIncrement(&c_); }

		/**
		 * Intrusive pointer release.
		 */
		long release()
		{ 
			long c = InterlockedDecrement(&c_);
			if (c == 0)
				delete this;
			return c;
		}

		/**
		 * Returns WinInet handle value.
		 */
		HINTERNET get() const
		{ 
			return InterlockedCompareExchangePointer(const_cast<HINTERNET*>(&h_), 0, 0); 
		}

		/**
		 * Resets WinInternet handle value.
		 */
		void reset()
		{ 
			if (HINTERNET h = InterlockedExchangePointer(&h_, 0))
				// close internet handle
				InternetCloseHandle(h);
		}
	private:
		// internet handle
		volatile HINTERNET h_;
		// reference counter
		volatile long c_;
	};

	/**
	 * Parent internet handle.
	 */
	boost::intrusive_ptr<handle> parent;

	/**
	 * Child (current) internet handle.
	 */
	boost::intrusive_ptr<handle> child;
};

} // namespace OnlineActivation

#endif //_AG_INTERNETHANDLE_H_
