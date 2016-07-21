#include <windows.h>
#include <Prague/prague.h>
#include "delayed_notifier.h"

http::delayed_notifier::delayed_notifier() :
	hthread_(NULL),
	stopping_(false),
	ref_count_(0),
	running_key_(NULL)
{
	hstop_ = CreateEvent(NULL, FALSE, FALSE, NULL);
	PR_TRACE((0, prtIMPORTANT, "http\tDN: Started"));
}

http::delayed_notifier::~delayed_notifier()
{
	stopping_ = true;
	PR_TRACE((0, prtIMPORTANT, "http\tDN: Exiting..."));
	{
		sync::local_mutex::scoped_lock lock(ref_mutex_);
		wait_thread();
		if(hstop_)
			CloseHandle(hstop_);
	}
	PR_TRACE((0, prtIMPORTANT, "http\tDN: Exited"));
}

void http::delayed_notifier::add_ref()
{
	if(stopping_)
		return;
	PR_TRACE((0, prtIMPORTANT, "http\tDN: Addrefing..."));
	{
		sync::local_mutex::scoped_lock lock(ref_mutex_);
		if(!ref_count_++)
		{
			PR_TRACE((0, prtIMPORTANT, "http\tDN: Creating worker"));
			if(hstop_)
			{
				DWORD threadId;
				hthread_ = CreateThread(NULL, 0, _run, this, 0, &threadId);
			}
		}
	}
	PR_TRACE((0, prtIMPORTANT, "http\tDN: Addrefed (%d)", ref_count_));
}

void http::delayed_notifier::release()
{
	if(stopping_)
		return;
	PR_TRACE((0, prtIMPORTANT, "http\tDN: Releasing..."));
	{
		sync::local_mutex::scoped_lock lock(ref_mutex_);
		if(!--ref_count_)
			wait_thread();
	}
	PR_TRACE((0, prtIMPORTANT, "http\tDN: Released (%d)", ref_count_));
}

void http::delayed_notifier::wait_thread()
{
	PR_TRACE((0, prtIMPORTANT, "http\tDN: Waiting worker..."));
	if(hthread_ && hstop_)
	{
		SetEvent(hstop_);
		DWORD exit_code;
		if(GetExitCodeThread(hthread_, &exit_code) && exit_code == STILL_ACTIVE)
			WaitForSingleObject(hthread_, INFINITE);
		
	}
	if(hthread_)
	{
		CloseHandle(hthread_);
		hthread_ = NULL;
	}
	PR_TRACE((0, prtIMPORTANT, "http\tDN: Waiting worker done"));
}

void http::delayed_notifier::add_event(unsigned timeout, void *key, boost::function0<void> func)
{
	if(stopping_)
		return;
	if(!key)
	{
		PR_ASSERT(0);
		return;
	}
	PR_TRACE((0, prtIMPORTANT, "http\tDN: Adding event with timeout %d, key %p", timeout, key));
	{
		sync::local_mutex::scoped_lock lock(mutex_);
		http::delayed_notifier::evt_t evt;
		evt.invoke_time = time(0) + timeout;
		evt.key = key;
		evt.func = func;
		events_.insert(evt);
	}
	PR_TRACE((0, prtIMPORTANT, "http\tDN: Event added"));
}

class evt_key_eq : public std::binary_function<http::delayed_notifier::evt_t, const void *, bool>
{
public:
	bool operator()(const http::delayed_notifier::evt_t &_L, const void * _R) const
	{
		return _L.key == _R;
	}
};

void http::delayed_notifier::remove_events_by_key(const void *key, bool wait_if_key_is_used /*= false*/)
{
	if(stopping_ || !key)
		return;
	PR_TRACE((0, prtIMPORTANT, "http\tDN: Removing events by key %p", key));
	{
		sync::local_mutex::scoped_lock lock(mutex_);
		events_t::iterator it = std::remove_if(events_.begin(), events_.end(), std::bind2nd(evt_key_eq(), key));
		events_.erase(it, events_.end());
	}
	PR_TRACE((0, prtIMPORTANT, "http\tDN: Events removed"));
	if(wait_if_key_is_used && running_key_ == key)
	{
		PR_TRACE((0, prtIMPORTANT, "http\tDN: Waiting running event %p...", running_key_));
		while(running_key_)
			Sleep(100);
		PR_TRACE((0, prtIMPORTANT, "http\tDN: Waiting done"));
	}
}

DWORD WINAPI http::delayed_notifier::_run(LPVOID param)
{
	return ((http::delayed_notifier *)param)->run();
}

DWORD http::delayed_notifier::run()
{
	if(!hstop_)
		return -1;
	
	PR_TRACE((0, prtIMPORTANT, "http\tDN: Worker started"));
	while(WaitForSingleObject(hstop_, 1000) == WAIT_TIMEOUT)
	{
		while(1)
		{
			time_t cur_time(time(0));
			http::delayed_notifier::evt_t evt;
			{
				sync::local_mutex::scoped_lock lock(mutex_);
				events_t::iterator beg = events_.begin();
				if(events_.empty() || cur_time <= beg->invoke_time)
					break;
				evt = *beg;
				events_.erase(beg);
				running_key_ = evt.key;
			}
			PR_TRACE((0, prtIMPORTANT, "http\tDN: Worker is processing event %p...", evt.key));
			evt.func();
			running_key_ = NULL;
			PR_TRACE((0, prtIMPORTANT, "http\tDN: Event %p processed", evt.key));
			if(WaitForSingleObject(hstop_, 0) != WAIT_TIMEOUT)
				return 0;
		}
	}
	PR_TRACE((0, prtIMPORTANT, "http\tDN: Worker exiting"));

	return 0;
}
