/*!
*		
*		
*		(C) 2006 Kaspersky Lab 
*		
*		\file	cond_var.hpp
*		\brief	condition variable
*
*				Has some weaknesses:
*				1. busy-waiting problem - if a waiter has the highest priority thread.
*										  the problem is that once call notify_all() the manual-reset event_
*										  remains signaled. the highest priority thread may cycle
*										  endlessly through the 'for loop' in wait() and never sleep on the event.
*				2. unfairness  problem  - the 'for loop' in wait() releases the mutex before
*										  calling WaitForSingleObject. in this case, the waiting thread can steal
*										  a release that was intended for another thread and fairness will break.
*		
*		\author Vladislav Ovcharik
*		\date	13.02.2006 9:36:10
*		
*		
*/		
#if !defined(_CONDITION_VARIABLE_HPP_INCLUDED)
#define _CONDITION_VARIABLE_HPP_INCLUDED
#include "local_mutex.hpp"
///////////////////////////////////////////////////////////////////////////////
namespace sync
{
///////////////////////////////////////////////////////////////////////////////
class condition;
///////////////////////////////////////////////////////////////////////////////
namespace detail
{
class condition_impl
		: private boost::noncopyable
{
	friend class sync::condition;

	typedef util::scoped_handle<HANDLE, int (__stdcall *)(HANDLE), &::CloseHandle, 1> event_t;
public:
	condition_impl()
		: event_(::CreateEvent(0, 1, 0, 0))
		, waiters_count_(0)
		, release_count_(0)
		, wait_generation_count_(0)
	{
	}
private:
	void do_notify_one()
	{
		sync::local_mutex::scoped_lock  lock(mutex_);
		if(waiters_count_ > release_count_)
		{
			SetEvent(event_.get());
			release_count_++;
			wait_generation_count_++;//! start a new generation.
		}
	}
	void do_notify_all()
	{
		sync::local_mutex::scoped_lock  lock(mutex_);
		if (waiters_count_ > 0)
		{
			SetEvent(event_.get());
			release_count_ = waiters_count_;
			wait_generation_count_++;//! start a new generation.
		}
	}
	long do_enter_wait()
	{
		sync::local_mutex::scoped_lock  lock(mutex_);
		++waiters_count_;
		return wait_generation_count_;
	}
	void do_wait(long curr_generation)
	{
		bool wait_done = false;
		while(!wait_done)
		{
			::WaitForSingleObject(event_.get(), INFINITE);
			sync::local_mutex::scoped_lock  lock(mutex_);
			wait_done = (release_count_ > 0) && (wait_generation_count_ != curr_generation);
		}
	}
	void do_leave_wait()
	{
		bool last_waiter(false);
		do
		{
			sync::local_mutex::scoped_lock  lock(mutex_);
			waiters_count_--;
			release_count_--;
			last_waiter = (release_count_ == 0);
		}while(0);

		if(last_waiter)
			ResetEvent(event_.get());
	}
private:
	/** to block and release waiting thread*/
	event_t				event_;
	long				waiters_count_;
	long				release_count_;
	long				wait_generation_count_;
	sync::local_mutex	mutex_;

};
///////////////////////////////////////////////////////////////////////////////
}//namespace detail
///////////////////////////////////////////////////////////////////////////////
class condition
		: private boost::noncopyable
{
public:
	void notify_all() { impl_.do_notify_all(); }
	void notify_one() { impl_.do_notify_one(); }
	template<typename T>
	void wait(T& lock)
	{
		if(!lock)
			throw std::runtime_error("not locked");
		do_wait(lock.mutex_);
	}
	template<typename T, typename P>
	void wait(T& lock, P pred)
	{
		if(!lock)
			throw std::runtime_error("not locked");
		while(!pred())
			do_wait(lock.mutex_);
	}
private:
	template<typename M>
	void do_wait(M& mutex)
	{
		long curr_generation = impl_.do_enter_wait();
		details::lock_ops<M>::unlock(mutex);
		impl_.do_wait(curr_generation);
		details::lock_ops<M>::lock(mutex);
		impl_.do_leave_wait();
	}
private:
	detail::condition_impl	impl_;
};
///////////////////////////////////////////////////////////////////////////////
}//namespace sync
#endif//!defined(_CONDITION_VARIABLE_HPP_INCLUDED)