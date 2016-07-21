#ifdef USING_BOOST
#	include <boost/smart_ptr.hpp>
#elif !defined(__BOOST_SMART_PTR_H)
#	define __BOOST_SMART_PTR_H
/************************************************************************
Этот файл выдернут из boost/smart_ptr.hpp, чтобы создать заново надо:
	в boost/assert.hpp надо поставить #pragma once
	cl /GX /P /I. /MTd smptr_extract.cpp

==8<======================== smptr_extract.cpp ======================8<==
#include <assert.h>             // for std::bad_cast

#include <memory>               // for std::auto_ptr
#include <algorithm>            // for std::swap
#include <functional>           // for std::less
#include <typeinfo>             // for std::bad_cast
#include <iosfwd>               // for std::basic_ostream
#include <limits.h>

==marker1==

#undef assert
#define BOOST_ASSERT assert

#include <boost/config.hpp>   // for broken compiler workarounds
#include <boost/assert.hpp>

#undef assert
#define BOOST_ASSERT assert

#include <boost/smart_ptr.hpp>
==8<=================================================================8<==
Всё что после marker1 и есть этот текст с некотырыми изменениями.
************************************************************************/

#pragma once

#include <assert.h>             // for std::bad_cast

#include <memory>               // for std::auto_ptr
#include <algorithm>            // for std::swap
#include <functional>           // for std::less
#include <typeinfo>             // for std::bad_cast
#include <iosfwd>               // for std::basic_ostream
#include <limits.h>


#pragma warning( disable : 4503 ) 

#pragma warning( disable : 4786 ) 

namespace std { using ::ptrdiff_t; using ::size_t; }
    
namespace boost {

  
  
  template <class T>
  struct type {};

}

namespace boost {

 template <typename T, T n>
 struct non_type { };

}


namespace boost
{

template<class T> inline void checked_delete(T * x)
{
    
    typedef char type_must_be_complete[ sizeof(T)? 1: -1 ];
    (void) sizeof(type_must_be_complete);
    delete x;
}

template<class T> inline void checked_array_delete(T * x)
{
    typedef char type_must_be_complete[ sizeof(T)? 1: -1 ];
    (void) sizeof(type_must_be_complete);
    delete [] x;
}

template<class T> struct checked_deleter
{
    typedef void result_type;
    typedef T * argument_type;

    void operator()(T * x) const
    {
        
        boost::checked_delete(x);
    }
};

template<class T> struct checked_array_deleter
{
    typedef void result_type;
    typedef T * argument_type;

    void operator()(T * x) const
    {
        boost::checked_array_delete(x);
    }
};

} 

namespace boost
{

template<class T> class scoped_ptr 
{
private:

    T * ptr;

    scoped_ptr(scoped_ptr const &);
    scoped_ptr & operator=(scoped_ptr const &);

    typedef scoped_ptr<T> this_type;

public:

    typedef T element_type;

    explicit scoped_ptr(T * p = 0): ptr(p) 
    {

    }

    explicit scoped_ptr(std::auto_ptr<T> p): ptr(p.release()) 
    {

    }

    ~scoped_ptr() 
    {

        boost::checked_delete(ptr);
    }

    void reset(T * p = 0) 
    {
        assert(p == 0 || p != ptr); 
        this_type(p).swap(*this);
    }

    T & operator*() const 
    {
        assert(ptr != 0);
        return *ptr;
    }

    T * operator->() const 
    {
        assert(ptr != 0);
        return ptr;
    }

    T * get() const 
    {
        return ptr;
    }

    typedef T * this_type::*unspecified_bool_type;

    operator unspecified_bool_type() const 
    {
        return ptr == 0? 0: &this_type::ptr;
    }

    bool operator! () const 
    {
        return ptr == 0;
    }

    void swap(scoped_ptr & b) 
    {
        T * tmp = b.ptr;
        b.ptr = ptr;
        ptr = tmp;
    }
};

template<class T> inline void swap(scoped_ptr<T> & a, scoped_ptr<T> & b) 
{
    a.swap(b);
}

template<class T> inline T * get_pointer(scoped_ptr<T> const & p)
{
    return p.get();
}

} 

namespace boost
{

template<class T> class scoped_array 
{
private:

    T * ptr;

    scoped_array(scoped_array const &);
    scoped_array & operator=(scoped_array const &);

    typedef scoped_array<T> this_type;

public:

    typedef T element_type;

    explicit scoped_array(T * p = 0) : ptr(p) 
    {

    }

    ~scoped_array() 
    {

        boost::checked_array_delete(ptr);
    }

    void reset(T * p = 0) 
    {
        assert(p == 0 || p != ptr); 
        this_type(p).swap(*this);
    }

    T & operator[](std::ptrdiff_t i) const 
    {
        assert(ptr != 0);
        assert(i >= 0);
        return ptr[i];
    }

    T * get() const 
    {
        return ptr;
    }

    typedef T * this_type::*unspecified_bool_type;

    operator unspecified_bool_type() const 
    {
        return ptr == 0? 0: &this_type::ptr;
    }

    bool operator! () const 
    {
        return ptr == 0;
    }

    void swap(scoped_array & b) 
    {
        T * tmp = b.ptr;
        b.ptr = ptr;
        ptr = tmp;
    }

};

template<class T> inline void swap(scoped_array<T> & a, scoped_array<T> & b) 
{
    a.swap(b);
}

} 

#pragma once

namespace boost
{

template<class E> inline void throw_exception(E const & e)
{
    throw e;
}

} 

#pragma once

#pragma once

#pragma once

namespace boost
{

namespace detail
{

extern "C" __declspec(dllimport) long __stdcall InterlockedExchange(long volatile *, long);

extern "C" __declspec(dllimport) void __stdcall Sleep(unsigned long);

class lightweight_mutex
{
private:

    long l_;

    lightweight_mutex(lightweight_mutex const &);
    lightweight_mutex & operator=(lightweight_mutex const &);

public:

    lightweight_mutex(): l_(0)
    {
    }

    class scoped_lock;
    friend class scoped_lock;

    class scoped_lock
    {
    private:

        lightweight_mutex & m_;

        scoped_lock(scoped_lock const &);
        scoped_lock & operator=(scoped_lock const &);

    public:

        explicit scoped_lock(lightweight_mutex & m): m_(m)
        {
            while( InterlockedExchange(&m_.l_, 1) )
            {

                Sleep(1);
            }
        }

        ~scoped_lock()
        {
            InterlockedExchange(&m_.l_, 0);

            
            
            
            
            
        }
    };
};

} 

} 

namespace boost
{

class bad_weak_ptr: public std::exception
{
public:

    virtual char const * what() const throw()
    {
        return "boost::bad_weak_ptr";
    }
};

namespace detail
{

class sp_counted_base
{
private:

    typedef detail::lightweight_mutex mutex_type;

public:

    sp_counted_base(): use_count_(1), weak_count_(1)
    {
    }

    virtual ~sp_counted_base() 
    {
    }

    virtual void dispose() = 0; 

    virtual void destruct() 
    {
        delete this;
    }

    virtual void * get_deleter(std::type_info const & ti) = 0;

    void add_ref_copy()
    {

        mutex_type::scoped_lock lock(mtx_);

        ++use_count_;
    }

    void add_ref_lock()
    {

        mutex_type::scoped_lock lock(mtx_);

        if(use_count_ == 0) boost::throw_exception(boost::bad_weak_ptr());
        ++use_count_;
    }

    void release() 
    {
        {

            mutex_type::scoped_lock lock(mtx_);

            long new_use_count = --use_count_;

            if(new_use_count != 0) return;
        }

        dispose();
        weak_release();
    }

    void weak_add_ref() 
    {

        mutex_type::scoped_lock lock(mtx_);

        ++weak_count_;
    }

    void weak_release() 
    {
        long new_weak_count;

        {

            mutex_type::scoped_lock lock(mtx_);

            new_weak_count = --weak_count_;
        }

        if(new_weak_count == 0)
        {
            destruct();
        }
    }

    long use_count() const 
    {

        mutex_type::scoped_lock lock(mtx_);

        return use_count_;
    }

private:

    sp_counted_base(sp_counted_base const &);
    sp_counted_base & operator= (sp_counted_base const &);

    long use_count_;        
    long weak_count_;       

    mutable mutex_type mtx_;

};

template<class P, class D> class sp_counted_base_impl: public sp_counted_base
{
private:

    P ptr; 
    D del; 

    sp_counted_base_impl(sp_counted_base_impl const &);
    sp_counted_base_impl & operator= (sp_counted_base_impl const &);

    typedef sp_counted_base_impl<P, D> this_type;

public:

    sp_counted_base_impl(P p, D d): ptr(p), del(d)
    {

    }

    virtual void dispose() 
    {

        del(ptr);
    }

    virtual void * get_deleter(std::type_info const & ti)
    {
        return ti == typeid(D)? &del: 0;
    }

};

class weak_count;

class shared_count
{
private:

    sp_counted_base * pi_;

    friend class weak_count;

public:

    shared_count(): pi_(0) 

    {
    }

    template<class P, class D> shared_count(P p, D d): pi_(0)

    {

        try
        {
            pi_ = new sp_counted_base_impl<P, D>(p, d);
        }
        catch(...)
        {
            d(p); 
            throw;
        }

    }

    template<class Y>
    explicit shared_count(std::auto_ptr<Y> & r): pi_(new sp_counted_base_impl< Y *, checked_deleter<Y> >(r.get(), checked_deleter<Y>()))

    {
        r.release();
    }

    ~shared_count() 
    {
        if(pi_ != 0) pi_->release();

    }

    shared_count(shared_count const & r): pi_(r.pi_) 

    {
        if(pi_ != 0) pi_->add_ref_copy();
    }

    explicit shared_count(weak_count const & r); 

    shared_count & operator= (shared_count const & r) 
    {
        sp_counted_base * tmp = r.pi_;

        if(tmp != pi_)
        {
            if(tmp != 0) tmp->add_ref_copy();
            if(pi_ != 0) pi_->release();
            pi_ = tmp;
        }

        return *this;
    }

    void swap(shared_count & r) 
    {
        sp_counted_base * tmp = r.pi_;
        r.pi_ = pi_;
        pi_ = tmp;
    }

    long use_count() const 
    {
        return pi_ != 0? pi_->use_count(): 0;
    }

    bool unique() const 
    {
        return use_count() == 1;
    }

    friend inline bool operator==(shared_count const & a, shared_count const & b)
    {
        return a.pi_ == b.pi_;
    }

    friend inline bool operator<(shared_count const & a, shared_count const & b)
    {
        return std::less<sp_counted_base *>()(a.pi_, b.pi_);
    }

    void * get_deleter(std::type_info const & ti) const
    {
        return pi_? pi_->get_deleter(ti): 0;
    }
};

class weak_count
{
private:

    sp_counted_base * pi_;

    friend class shared_count;

public:

    weak_count(): pi_(0) 

    {
    }

    weak_count(shared_count const & r): pi_(r.pi_) 

    {
        if(pi_ != 0) pi_->weak_add_ref();
    }

    weak_count(weak_count const & r): pi_(r.pi_) 

    {
        if(pi_ != 0) pi_->weak_add_ref();
    }

    ~weak_count() 
    {
        if(pi_ != 0) pi_->weak_release();

    }

    weak_count & operator= (shared_count const & r) 
    {
        sp_counted_base * tmp = r.pi_;
        if(tmp != 0) tmp->weak_add_ref();
        if(pi_ != 0) pi_->weak_release();
        pi_ = tmp;

        return *this;
    }

    weak_count & operator= (weak_count const & r) 
    {
        sp_counted_base * tmp = r.pi_;
        if(tmp != 0) tmp->weak_add_ref();
        if(pi_ != 0) pi_->weak_release();
        pi_ = tmp;

        return *this;
    }

    void swap(weak_count & r) 
    {
        sp_counted_base * tmp = r.pi_;
        r.pi_ = pi_;
        pi_ = tmp;
    }

    long use_count() const 
    {
        return pi_ != 0? pi_->use_count(): 0;
    }

    friend inline bool operator==(weak_count const & a, weak_count const & b)
    {
        return a.pi_ == b.pi_;
    }

    friend inline bool operator<(weak_count const & a, weak_count const & b)
    {
        return std::less<sp_counted_base *>()(a.pi_, b.pi_);
    }
};

inline shared_count::shared_count(weak_count const & r): pi_(r.pi_)

{
    if(pi_ != 0)
    {
        pi_->add_ref_lock();
    }
    else
    {
        boost::throw_exception(boost::bad_weak_ptr());
    }
}

} 

} 

#pragma warning(push)
#pragma warning(disable:4284) 

namespace boost
{

template<class T> class weak_ptr;
template<class T> class enable_shared_from_this;

namespace detail
{

struct static_cast_tag {};
struct const_cast_tag {};
struct dynamic_cast_tag {};
struct polymorphic_cast_tag {};

template<class T> struct shared_ptr_traits
{
    typedef T & reference;
};

template<> struct shared_ptr_traits<void>
{
    typedef void reference;
};

template<class T, class Y> void sp_enable_shared_from_this( shared_count const & pn, boost::enable_shared_from_this<T> const * pe, Y const * px )
{
    if(pe != 0) pe->_internal_weak_this._internal_assign(const_cast<Y*>(px), pn);
}

inline void sp_enable_shared_from_this( shared_count const & , ... )
{
}

} 

template<class T> class shared_ptr
{
private:

    
    typedef shared_ptr<T> this_type;

public:

    typedef T element_type;
    typedef T value_type;
    typedef T * pointer;
    typedef typename detail::shared_ptr_traits<T>::reference reference;

    shared_ptr(): px(0), pn() 
    {
    }

    template<class Y>
    explicit shared_ptr(Y * p): px(p), pn(p, checked_deleter<Y>()) 
    {
        detail::sp_enable_shared_from_this( pn, p, p );
    }

    template<class Y, class D> shared_ptr(Y * p, D d): px(p), pn(p, d)
    {
        detail::sp_enable_shared_from_this( pn, p, p );
    }

    template<class Y>
    explicit shared_ptr(weak_ptr<Y> const & r): pn(r.pn) 
    {
        
        px = r.px;
    }

    template<class Y>
    shared_ptr(shared_ptr<Y> const & r): px(r.px), pn(r.pn) 
    {
    }

    template<class Y>
    shared_ptr(shared_ptr<Y> const & r, detail::static_cast_tag): px(static_cast<element_type *>(r.px)), pn(r.pn)
    {
    }

    template<class Y>
    shared_ptr(shared_ptr<Y> const & r, detail::const_cast_tag): px(const_cast<element_type *>(r.px)), pn(r.pn)
    {
    }

    template<class Y>
    shared_ptr(shared_ptr<Y> const & r, detail::dynamic_cast_tag): px(dynamic_cast<element_type *>(r.px)), pn(r.pn)
    {
        if(px == 0) 
        {
            pn = detail::shared_count();
        }
    }

    template<class Y>
    shared_ptr(shared_ptr<Y> const & r, detail::polymorphic_cast_tag): px(dynamic_cast<element_type *>(r.px)), pn(r.pn)
    {
        if(px == 0)
        {
            boost::throw_exception(std::bad_cast());
        }
    }

    template<class Y>
    explicit shared_ptr(std::auto_ptr<Y> & r): px(r.get()), pn()
    {
        Y * tmp = r.get();
        pn = detail::shared_count(r);
        detail::sp_enable_shared_from_this( pn, tmp, tmp );
    }

    template<class Y>
    shared_ptr & operator=(std::auto_ptr<Y> & r)
    {
        this_type(r).swap(*this);
        return *this;
    }

    void reset() 
    {
        this_type().swap(*this);
    }

    template<class Y> void reset(Y * p) 
    {
        assert(p == 0 || p != px); 
        this_type(p).swap(*this);
    }

    template<class Y, class D> void reset(Y * p, D d)
    {
        this_type(p, d).swap(*this);
    }

    reference operator* () const 
    {
        assert(px != 0);
        return *px;
    }

    T * operator-> () const 
    {
        assert(px != 0);
        return px;
    }
    
    T * get() const 
    {
        return px;
    }

    typedef T * this_type::*unspecified_bool_type;

    operator unspecified_bool_type() const 
    {
        return px == 0? 0: &this_type::px;
    }

    bool operator! () const 
    {
        return px == 0;
    }

    bool unique() const 
    {
        return pn.unique();
    }

    long use_count() const 
    {
        return pn.use_count();
    }

    void swap(shared_ptr<T> & other) 
    {
        std::swap(px, other.px);
        pn.swap(other.pn);
    }

    template<class Y> bool _internal_less(shared_ptr<Y> const & rhs) const
    {
        return pn < rhs.pn;
    }

    void * _internal_get_deleter(std::type_info const & ti) const
    {
        return pn.get_deleter(ti);
    }

    T * px;                     
    detail::shared_count pn;    

};  

template<class T, class U> inline bool operator==(shared_ptr<T> const & a, shared_ptr<U> const & b)
{
    return a.get() == b.get();
}

template<class T, class U> inline bool operator!=(shared_ptr<T> const & a, shared_ptr<U> const & b)
{
    return a.get() != b.get();
}

template<class T, class U> inline bool operator<(shared_ptr<T> const & a, shared_ptr<U> const & b)
{
    return a._internal_less(b);
}

template<class T> inline void swap(shared_ptr<T> & a, shared_ptr<T> & b)
{
    a.swap(b);
}

template<class T, class U> shared_ptr<T> static_pointer_cast(shared_ptr<U> const & r)
{
    return shared_ptr<T>(r, detail::static_cast_tag());
}

template<class T, class U> shared_ptr<T> const_pointer_cast(shared_ptr<U> const & r)
{
    return shared_ptr<T>(r, detail::const_cast_tag());
}

template<class T, class U> shared_ptr<T> dynamic_pointer_cast(shared_ptr<U> const & r)
{
    return shared_ptr<T>(r, detail::dynamic_cast_tag());
}

template<class T, class U> shared_ptr<T> shared_static_cast(shared_ptr<U> const & r)
{
    return shared_ptr<T>(r, detail::static_cast_tag());
}

template<class T, class U> shared_ptr<T> shared_dynamic_cast(shared_ptr<U> const & r)
{
    return shared_ptr<T>(r, detail::dynamic_cast_tag());
}

template<class T, class U> shared_ptr<T> shared_polymorphic_cast(shared_ptr<U> const & r)
{
    return shared_ptr<T>(r, detail::polymorphic_cast_tag());
}

template<class T, class U> shared_ptr<T> shared_polymorphic_downcast(shared_ptr<U> const & r)
{
    assert(dynamic_cast<T *>(r.get()) == r.get());
    return shared_static_cast<T>(r);
}

template<class T> inline T * get_pointer(shared_ptr<T> const & p)
{
    return p.get();
}

template<class E, class T, class Y> std::basic_ostream<E, T> & operator<< (std::basic_ostream<E, T> & os, shared_ptr<Y> const & p)

{
    os << p.get();
    return os;
}

template<class D, class T> D * get_deleter(shared_ptr<T> const & p)
{
    return static_cast<D *>(p._internal_get_deleter(typeid(D)));
}

} 

#pragma warning(pop)

namespace boost
{

template<class T> class shared_array
{
private:

    
    typedef checked_array_deleter<T> deleter;
    typedef shared_array<T> this_type;

public:

    typedef T element_type;

    explicit shared_array(T * p = 0): px(p), pn(p, deleter())
    {
    }

    template<class D> shared_array(T * p, D d): px(p), pn(p, d)
    {
    }

    void reset(T * p = 0)
    {
        assert(p == 0 || p != px);
        this_type(p).swap(*this);
    }

    template <class D> void reset(T * p, D d)
    {
        this_type(p, d).swap(*this);
    }

    T & operator[] (std::ptrdiff_t i) const 
    {
        assert(px != 0);
        assert(i >= 0);
        return px[i];
    }
    
    T * get() const 
    {
        return px;
    }

    typedef T * this_type::*unspecified_bool_type;

    operator unspecified_bool_type() const 
    {
        return px == 0? 0: &this_type::px;
    }

    bool operator! () const 
    {
        return px == 0;
    }

    bool unique() const 
    {
        return pn.unique();
    }

    long use_count() const 
    {
        return pn.use_count();
    }

    void swap(shared_array<T> & other) 
    {
        std::swap(px, other.px);
        pn.swap(other.pn);
    }

private:

    T * px;                     
    detail::shared_count pn;    

};  

template<class T> inline bool operator==(shared_array<T> const & a, shared_array<T> const & b) 
{
    return a.get() == b.get();
}

template<class T> inline bool operator!=(shared_array<T> const & a, shared_array<T> const & b) 
{
    return a.get() != b.get();
}

template<class T> inline bool operator<(shared_array<T> const & a, shared_array<T> const & b) 
{
    return std::less<T*>()(a.get(), b.get());
}

template<class T> void swap(shared_array<T> & a, shared_array<T> & b) 
{
    a.swap(b);
}

} 

#pragma warning(push)
#pragma warning(disable:4284) 

namespace boost
{

template<class T> class weak_ptr
{
private:

    
    typedef weak_ptr<T> this_type;

public:

    typedef T element_type;

    weak_ptr(): px(0), pn() 
    {
    }

    template<class Y>
    weak_ptr(weak_ptr<Y> const & r): pn(r.pn) 
    {
        px = r.lock().get();
    }

    template<class Y>
    weak_ptr(shared_ptr<Y> const & r): px(r.px), pn(r.pn) 
    {
    }

    shared_ptr<T> lock() const 
    {

        
        if(expired())
        {
            return shared_ptr<element_type>();
        }

        try
        {
            return shared_ptr<element_type>(*this);
        }
        catch(bad_weak_ptr const &)
        {
            
            
            return shared_ptr<element_type>();
        }

    }

    long use_count() const 
    {
        return pn.use_count();
    }

    bool expired() const 
    {
        return pn.use_count() == 0;
    }

    void reset() 
    {
        this_type().swap(*this);
    }

    void swap(this_type & other) 
    {
        std::swap(px, other.px);
        pn.swap(other.pn);
    }

    void _internal_assign(T * px2, detail::shared_count const & pn2)
    {
        px = px2;
        pn = pn2;
    }

    template<class Y> bool _internal_less(weak_ptr<Y> const & rhs) const
    {
        return pn < rhs.pn;
    }

    T * px;                     
    detail::weak_count pn;      

};  

template<class T, class U> inline bool operator<(weak_ptr<T> const & a, weak_ptr<U> const & b)
{
    return a._internal_less(b);
}

template<class T> void swap(weak_ptr<T> & a, weak_ptr<T> & b)
{
    a.swap(b);
}

template<class T> shared_ptr<T> make_shared(weak_ptr<T> const & r)
{
    return r.lock();
}

} 

#pragma warning(pop)

#pragma warning(push)
#pragma warning(disable:4284) 

namespace boost
{

template<class T> class intrusive_ptr
{
private:

    typedef intrusive_ptr this_type;

public:

    typedef T element_type;

    intrusive_ptr(): p_(0)
    {
    }

    intrusive_ptr(T * p, bool add_ref = true): p_(p)
    {
        if(p_ != 0 && add_ref) intrusive_ptr_add_ref(p_);
    }

    template<class U> intrusive_ptr(intrusive_ptr<U> const & rhs): p_(rhs.get())
    {
        if(p_ != 0) intrusive_ptr_add_ref(p_);
    }

    intrusive_ptr(intrusive_ptr const & rhs): p_(rhs.p_)
    {
        if(p_ != 0) intrusive_ptr_add_ref(p_);
    }

    ~intrusive_ptr()
    {
        if(p_ != 0) intrusive_ptr_release(p_);
    }

    template<class U> intrusive_ptr & operator=(intrusive_ptr<U> const & rhs)
    {
        this_type(rhs).swap(*this);
        return *this;
    }

    intrusive_ptr & operator=(intrusive_ptr const & rhs)
    {
        this_type(rhs).swap(*this);
        return *this;
    }

    intrusive_ptr & operator=(T * rhs)
    {
        this_type(rhs).swap(*this);
        return *this;
    }

    T * get() const
    {
        return p_;
    }

    T & operator*() const
    {
        return *p_;
    }

    T * operator->() const
    {
        return p_;
    }

    typedef T * this_type::*unspecified_bool_type;

    operator unspecified_bool_type () const
    {
        return p_ == 0? 0: &this_type::p_;
    }

    
    bool operator! () const
    {
        return p_ == 0;
    }

    void swap(intrusive_ptr & rhs)
    {
        T * tmp = p_;
        p_ = rhs.p_;
        rhs.p_ = tmp;
    }

private:

    T * p_;
};

template<class T, class U> inline bool operator==(intrusive_ptr<T> const & a, intrusive_ptr<U> const & b)
{
    return a.get() == b.get();
}

template<class T, class U> inline bool operator!=(intrusive_ptr<T> const & a, intrusive_ptr<U> const & b)
{
    return a.get() != b.get();
}

template<class T> inline bool operator==(intrusive_ptr<T> const & a, T * b)
{
    return a.get() == b;
}

template<class T> inline bool operator!=(intrusive_ptr<T> const & a, T * b)
{
    return a.get() != b;
}

template<class T> inline bool operator==(T * a, intrusive_ptr<T> const & b)
{
    return a == b.get();
}

template<class T> inline bool operator!=(T * a, intrusive_ptr<T> const & b)
{
    return a != b.get();
}

template<class T> inline bool operator<(intrusive_ptr<T> const & a, intrusive_ptr<T> const & b)
{
    return std::less<T *>()(a.get(), b.get());
}

template<class T> void swap(intrusive_ptr<T> & lhs, intrusive_ptr<T> & rhs)
{
    lhs.swap(rhs);
}

template<class T> T * get_pointer(intrusive_ptr<T> const & p)
{
    return p.get();
}

template<class T, class U> intrusive_ptr<T> static_pointer_cast(intrusive_ptr<U> const & p)
{
    return static_cast<T *>(p.get());
}

template<class T, class U> intrusive_ptr<T> const_pointer_cast(intrusive_ptr<U> const & p)
{
    return const_cast<T *>(p.get());
}

template<class T, class U> intrusive_ptr<T> dynamic_pointer_cast(intrusive_ptr<U> const & p)
{
    return dynamic_cast<T *>(p.get());
}

template<class E, class T, class Y> std::basic_ostream<E, T> & operator<< (std::basic_ostream<E, T> & os, intrusive_ptr<Y> const & p)

{
    os << p.get();
    return os;
}

} 

#pragma warning(pop)

namespace boost
{

template<class T> class enable_shared_from_this
{
protected:

    enable_shared_from_this()
    {
    }

    enable_shared_from_this(enable_shared_from_this const &)
    {
    }

    enable_shared_from_this & operator=(enable_shared_from_this const &)
    {
        return *this;
    }

    ~enable_shared_from_this()
    {
    }

public:

    shared_ptr<T> shared_from_this()
    {
        shared_ptr<T> p(_internal_weak_this);
        assert(p.get() == this);
        return p;
    }

    shared_ptr<T const> shared_from_this() const
    {
        shared_ptr<T const> p(_internal_weak_this);
        assert(p.get() == this);
        return p;
    }

    typedef T _internal_element_type; 
    mutable weak_ptr<_internal_element_type> _internal_weak_this;
};

} 

#endif