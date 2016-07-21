// $Id: rcobj.h,v 1.10 2000/04/05 12:36:46 slavikb Exp $
//

#ifndef _RCOBJ_H_
#define	_RCOBJ_H_

#ifdef _WIN32

#ifndef STRICT
#define STRICT
#endif

#include <windows.h>

#else	// _WIN32

// sig_atomic_t definition
#include <signal.h>

#endif	// _WIN32

#include "mballoc.h"

////////////////////////////////////////////////////////////////
// object bases

template<class T>
T * rc_object_create( typename T::_allocator *pa, T * ) {
	void *p = pa->alloc(sizeof(T));
	T *pt = new(p) T();
	if ( pt )
		pt->set_allocator(pa);
	return pt;
}

template<class T>
void	rc_object_destroy( typename T::_allocator *pa, T *p ) {
	if( p!=NULL ) { p->~T(); pa->free(p); }
}

#define	RC_OBJECT_NEW(CLS)	rc_object_create( (CLS::_allocator *)NULL, (CLS *)NULL )

// object base with non-virtual destructor (incurs no overhead on object size,
// but is useful only for objects with no special actions in destructors)
template<class A>
struct	static_object_base {
	typedef	A	_allocator;

	void	duplicate()	{}
	void	release( _allocator * pa )
		{ rc_object_destroy(pa,this); }
};

// object base with virtual destructor
template<class A>
struct	virtual_object_base {
	typedef	A	_allocator;

	virtual	~virtual_object_base()	{}

	void	duplicate()	{}
	void	release( _allocator * pa )
		{ rc_object_destroy(pa,this); }
};

// reference-counting object base
#ifdef _WIN32

typedef	long refcnt_t;

inline refcnt_t	atomic_incr( refcnt_t& n )
	{ return ::InterlockedIncrement(&n); }
inline refcnt_t	atomic_decr( refcnt_t& n )
	{ return ::InterlockedDecrement(&n); }
#else

typedef	sig_atomic_t refcnt_t;

inline refcnt_t	atomic_incr( refcnt_t& n )
	{ return ++n; }
inline refcnt_t	atomic_decr( refcnt_t& n )
	{ return --n; }
#endif

template<class A>
struct	rc_object_base : public virtual_object_base<A> {
		refcnt_t	refcnt;

		rc_object_base() : refcnt(1)	{}
		rc_object_base( const rc_object_base& s ) : refcnt(1)	{}

	rc_object_base& operator=( const rc_object_base& r )
				{ return *this; }

	void	duplicate()	{ atomic_incr(refcnt); }
	void	release( _allocator *pa )
		{ if( atomic_decr(refcnt)==0 ) rc_object_destroy(pa,this); }
};

////////////////////////////////////////////////////////////////
// dynamic (free & managed) objects

// free object (operates on a global allocator, e.g. malloc())
template<class B>
struct	dynamic_object : public B {

	typedef typename B::_allocator _allocator;

	void	set_allocator( _allocator *pa )		{}
	_allocator *get_allocator() const		{ return NULL; }

	void	release()	{ B::release( get_allocator() ); }
};

// managed object (holds pointer to the allocator)
template<class B>
struct	managed_object : public B {

	typedef typename B::_allocator _allocator;

	_allocator	*p_alloc;

		managed_object() : p_alloc(NULL)	{}

	void	set_allocator( _allocator *p )		{ p_alloc=p; }
	_allocator *get_allocator() const		{ return p_alloc; }

	void	release()	{ B::release( get_allocator() ); }
};

////////////////////////////////////////////////////////////////
// self-managed object reference

template<class PT>
class	objref_t : public PT {
public:
	typedef typename PT::_T _T;

	objref_t()	{}

	objref_t( const objref_t& v ) : PT(v)
			{ if(_ptr!=NULL) PT::_duplicate(); }

	objref_t( _T *p )
			{ if(p!=NULL) { PT::_ptr=p; PT::_duplicate(); } }

	~objref_t()	{ release(); }

	void	release() {
			if(PT::_ptr!=NULL) { PT::_release(); PT::_ptr=NULL; }
		}

	void	assign( _T *p )	{
			if( PT::_ptr!=NULL ) PT::_release();
			PT::_ptr = p;
			if( PT::_ptr!=NULL ) PT::_duplicate();
		}

	void	attach( _T *p )	{
			if( PT::_ptr!=NULL ) PT::_release();
			PT::_ptr = p;
		}

	void	detach()	{ PT::_ptr = NULL; }

	objref_t& operator=( _T *p )
			{ assign(p); return *this; }

	objref_t& operator=( const objref_t& v )
			{ assign(v.get()); return *this; }

	bool	is_valid() const 	{ return PT::_ptr!=NULL; }

	bool	is_null() const		{ return PT::_ptr==NULL; }

	bool	operator!() const 	{ return PT::_ptr==NULL; }

	_T *	get() const		{ return PT::_ptr; }

		operator _T *() const	{ return PT::_ptr; }
	_T *	operator->() const	{ return PT::_ptr; }

	bool	operator==( const objref_t& v ) const
			{ return PT::_ptr==v._ptr; }

	bool	operator<( const objref_t& ) const
			{ return false; }

};

////////////////////////////////////////////////////////////////
// reference-counting adaptor

template<class T>
struct	std_rc_adaptor {
	typedef	T	_T;

	_T	*_ptr;

	std_rc_adaptor() : _ptr(NULL)	{}

protected:
	void	_duplicate()	{ _ptr->duplicate(); }
	void	_release()	{ _ptr->release(); }
};

#define	OBJREF_PTR(T)	objref_t<std_rc_adaptor<T> > \


////////////////////////////////////////////////////////////////
// single-instance adaptor

template<class T>
struct	null_rc_adaptor {
	typedef	T	_T;

	_T	*_ptr;

	null_rc_adaptor() : _ptr(NULL)	{}

protected:
	void	_duplicate()	{}
	void	_release()	{ delete _ptr; }
};

#define	AUTO_PTR(T)	objref_t<null_rc_adaptor<T> > \


////////////////////////////////////////////////////////////////
// standard object base

typedef	rc_object_base<static_malloc_allocator>	std_rc_object_base;
typedef	dynamic_object<std_rc_object_base>	std_dynamic_object;


#endif	// _RCOBJ_H_

