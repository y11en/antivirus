#ifndef __DELAYED_NOTIFIER_H__
#define __DELAYED_NOTIFIER_H__

#include <boost/thread.hpp>
#include <set>
#include <utils/local_mutex.hpp>
#include <time.h>

namespace http
{

class delayed_notifier : private boost::noncopyable
{
public:
	delayed_notifier();
	~delayed_notifier();

	struct evt_t
	{
		time_t invoke_time;
		void *key;
		boost::function0<void> func;
	};
	class evt_t_p : public std::binary_function<evt_t, evt_t, bool>
	{
	public:
		bool operator()(const evt_t& _L, const evt_t& _R) const
		{
			return _L.invoke_time < _R.invoke_time;
		}
	};

	void add_ref();
	void release();
	void add_event(unsigned timeout, void *key, boost::function0<void> func);
	void remove_events_by_key(const void *key, bool wait_if_key_is_used = false);

private:
	HANDLE hthread_;
	static DWORD WINAPI _run(LPVOID param);
	DWORD run();
	void wait_thread();

	HANDLE hstop_;
	
	sync::local_mutex mutex_, ref_mutex_;
	typedef std::multiset<evt_t, evt_t_p> events_t;
	events_t events_;

	bool stopping_;
	unsigned ref_count_;

	volatile void *running_key_;
};

}

#endif