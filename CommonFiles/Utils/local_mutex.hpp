#if !defined(_LOCAL_MUTEX_HPP_INCLUEDE)
#define _LOCAL_MUTEX_HPP_INCLUEDE
#include <boost/utility.hpp>
#include "prague/scoped_handle.hpp"
/**
 * \brief	Пространство имен для объектов синхронизации
 */
namespace sync
{
///////////////////////////////////////////////////////////////////////////////
class condition;
///////////////////////////////////////////////////////////////////////////////
enum rw_policy
{
	single_writer_many_reads
};
///////////////////////////////////////////////////////////////////////////////
enum rw_lock_state
{
    NO_LOCK			= 0,
    SHARED_LOCK		= 1,
    EXCLUSIVE_LOCK	= 2
};
///////////////////////////////////////////////////////////////////////////////
namespace details
{
///////////////////////////////////////////////////////////////////////////////
/**
 * \struct	scoped_lock
 * \brief	Идиома scoped lock, см. Дуглас Шмидт.
 */
template<typename MutexT>
struct scoped_lock
	: private boost::noncopyable
{
	friend class sync::condition;
	typedef MutexT	mutex_type;
	//! По умолчанию блокирует объект синхонизации
	explicit scoped_lock(MutexT& mx, bool initially_locked = true)
        :	mutex_(mx)
		,	locked_(false)
    {
        if (initially_locked)
			lock();
    }
	//! В деструкторе разблокируем
    ~scoped_lock()
    {
        if (locked_)
			unlock();
    }
	//! Блокирует объект синхронизации
    void lock()
    {
        if (locked_)
			throw std::runtime_error("mutex has been already locked");
        mutex_.do_lock();
        locked_ = true;
    }
	//! Разблокирует объект синхронизации
    void unlock()
    {
        if (!locked_)
			throw std::runtime_error("mutex has been already unlocked");
        mutex_.do_unlock();
        locked_ = false;
    }
	//! Возвращает состояние объекта синхронизации
    bool locked() const { return m_locked; }
	//! оператор используется для написамни выражений if(lock) ...
	//! используется правило приведения void* к bool
    operator const void*() const { return locked_ ? this : 0; }

private:
    MutexT&			mutex_;
    bool			locked_;
};
///////////////////////////////////////////////////////////////////////////////
template<typename RWMutexT>
class scoped_rw_lock
		:	private boost::noncopyable
{
public:
    typedef RWMutexT mutex_type;

    explicit scoped_rw_lock(RWMutexT& m
							,	rw_lock_state initial_state = SHARED_LOCK)
							:	mutex_(m)
							,	locked_(NO_LOCK)
    {
        if (initial_state == SHARED_LOCK)
            rdlock();
        else if (initial_state == EXCLUSIVE_LOCK)
            wrlock();
    }
    ~scoped_rw_lock()
    {
        if (locked_ != NO_LOCK)
            unlock();
    }

    void rdlock()
    {
        if (locked_ != NO_LOCK) 
			throw std::runtime_error("rw_mutex has been already locked");
        mutex_.do_rdlock();
        locked_ = SHARED_LOCK;
    }
    void wrlock()
    {
        if (locked_ != NO_LOCK)
			throw std::runtime_error("rw_mutex has been already locked");
        mutex_.do_wrlock();
        locked_ = EXCLUSIVE_LOCK;
    }
    void unlock()
    {
        if (locked_ == NO_LOCK)
			throw std::runtime_error("rw_mutex has been already unlocked");
        if (locked_ == SHARED_LOCK)
            mutex_.do_rdunlock();
        else
            mutex_.do_wrunlock();

        locked_ = NO_LOCK;
    }
    bool locked() const
    {
        return locked_ != NO_LOCK;
    }
    operator const void*() const
    {
        return (locked_ != NO_LOCK) ? this : 0;
    }
    rw_lock_state state() const
    {
        return locked_;
    }

private:
    RWMutexT& mutex_;
    rw_lock_state locked_;
};
///////////////////////////////////////////////////////////////////////////////
template <typename Mutex>
class lock_ops
			: private boost::noncopyable
{
	friend class sync::condition;
private:
    lock_ops() { }
public:
    static void lock(Mutex& m) { m.do_lock(); }
    static void unlock(Mutex& m) { m.do_unlock(); }
};
}//namespace details
///////////////////////////////////////////////////////////////////////////////
/**
 *	\class	local_mutex
 *	\brief	Объект синхронизации mutex. Сериализует доступ
 *			к разделяемым ресурсам между конкурирующими потоками.
 *			Основное ограничение - внутри процессовая синхронизация.
 */
class local_mutex
	: private boost::noncopyable
{
	friend details::scoped_lock<local_mutex>;
	friend details::lock_ops<local_mutex>;
public:
	typedef details::scoped_lock<local_mutex>	scoped_lock;

	local_mutex() { ::InitializeCriticalSection(&object_); }
	~local_mutex() { ::DeleteCriticalSection(&object_); }
private:
	//! Блокироовать объект синхронизации
	void do_lock() { ::EnterCriticalSection(&object_); }
	//! Блокироовать объект синхронизации
    void do_unlock() { ::LeaveCriticalSection(&object_); }
	//! Объект критическая секция для внутрипроцессовой синхронизации
	CRITICAL_SECTION		object_;
};
///////////////////////////////////////////////////////////////////////////////
class local_rw_mutex
	: private boost::noncopyable
{
	typedef util::scoped_handle<HANDLE, int (__stdcall *)(HANDLE), &::CloseHandle, 0, 0> semaphore_t;
public:
	 explicit local_rw_mutex(rw_policy sp)
				 : waitingReaders_(0)
				 , waitingWriters_(0)
				 , active_(0)
				 , readers_(::CreateSemaphore(NULL, 0, MAXLONG, NULL))
				 , writers_(::CreateSemaphore(NULL, 0, MAXLONG, NULL))
	 {
	 }
    ~local_rw_mutex()
	{
	}

    friend class details::scoped_rw_lock<local_rw_mutex>;
    typedef details::scoped_rw_lock<local_rw_mutex> scoped_rw_lock;

private:
    void do_wrlock()
	{
		if(is_resource_owned())
			::WaitForSingleObject(writers_.get(), INFINITE);
	}
    void do_rdlock()
	{
		if(is_writer_wait())
			::WaitForSingleObject(readers_.get(), INFINITE);
	}
    void do_wrunlock()
	{
		unlock();
	}
    void do_rdunlock()
	{
		unlock();
	}
	bool is_writer_wait()
	{
		local_mutex::scoped_lock lock(local_);

		bool resource_write_pending = (waitingWriters_ || (active_ < 0));

		resource_write_pending
			? ++waitingReaders_
			: ++active_;
		return resource_write_pending;
	}
	bool is_resource_owned()
	{
		local_mutex::scoped_lock lock(local_);

		bool resource_owned = (active_ != 0);

		resource_owned
			? waitingWriters_++
			: active_ = -1;

		return resource_owned;
	}
	std::pair<void*, int> done()
	{
		local_mutex::scoped_lock lock(local_);
		
		active_ > 0
			? --active_
			: ++active_;

		void *sem = 0;
		int count = 1;

		if(active_ == 0)
		{
			if(waitingWriters_ > 0)
			{
				active_ = -1;
				--waitingWriters_;
				sem = writers_.get();
			}
			else if(waitingReaders_ > 0)
			{
				active_ = waitingReaders_;
				waitingReaders_ = 0;
				sem = readers_.get();
				count = active_;
			}
		}

		return std::make_pair(sem, count);
	}
	void unlock()
	{
		std::pair<void*, int> const& unlck = done();
		if(unlck.first)
			::ReleaseSemaphore(unlck.first, unlck.second, 0);
	}
private:
	int		waitingReaders_;
	int		waitingWriters_;
	int		active_;
	semaphore_t	readers_;
	semaphore_t	writers_;
	local_mutex	local_;
};
///////////////////////////////////////////////////////////////////////////////
}//namespace sync
#endif//!defined(_LOCAL_MUTEX_HPP_INCLUEDE)