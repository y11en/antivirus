// $Id: mballoc.h,v 1.2 2000/06/27 04:01:01 slavikb Exp $
//

#ifndef	_MBALLOC_H_
#define	_MBALLOC_H_

#include <stdlib.h>

///////////////////////////////////////////////////////////////
// allocator: standard 'malloc' allocator
//
// common allocator interface is:
//
//	void *	alloc( size_t sz );
//
//	void *	realloc( void *p, size_t old_sz, size_t sz );
//
//	void	free( void *p );
//

// virtual interface
class	mb_allocator {
public:
	virtual void *	alloc( size_t sz ) = 0;
	virtual void *	realloc( void *p, size_t old_sz, size_t sz ) = 0;
	virtual void	free( void *p ) = 0;
};

// standard 'malloc' allocator
class	static_malloc_allocator {
public:
 static	void *	alloc( size_t sz )
			{ return ::malloc(sz); }

 static	void *	realloc( void *p, size_t old_sz, size_t sz )
			{ return ::realloc(p,sz); }

 static	void	free( void *p )
			{ ::free(p); }
};

// static-to-non-static adapter
template<class A>
class	static_allocator_t {
public:
	void *	alloc( size_t sz )
			{ return A::alloc(sz); }

	void *	realloc( void *p, size_t old_sz, size_t sz )
			{ return A::realloc(p,old_sz,sz); }

	void	free( void *p )
			{ A::free(p); }
};

// non-virtual-to-virtual adapter
template<class A>
class	mb_allocator_t : public mb_allocator {
public:
	void *	alloc( size_t sz )
			{ return imp.alloc(sz); }

	void *	realloc( void *p, size_t old_sz, size_t sz )
			{ return imp.realloc(p,old_sz,sz); }

	void	free( void *p )
			{ imp.free(p); }

	A	imp;
};

// static-to-virtual adapter
template<class A>
class	mb_static_allocator_t : public mb_allocator {
public:
	void *	alloc( size_t sz )
			{ return A::alloc(sz); }

	void *	realloc( void *p, size_t old_sz, size_t sz )
			{ return A::realloc(p,old_sz,sz); }

	void	free( void *p )
			{ A::free(p); }
};

// object allocator based on the specified memory allocator
template<class T>
class	object_allocator_t {
public:
	typedef	T	VALUE;

	virtual T *	alloc() = 0;
	virtual void	free( T * ) = 0;
};

// object allocator based upon the specified memory allocator
template<class T, class A>
class	mb_object_allocator_t : public object_allocator_t<T> {
public:
	typedef	A	allocator;

		mb_object_allocator_t( A *p=NULL ) : palloc(p)	{}

	void	set_allocator( A *p )	{ palloc=p; }
	A *	get_allocator() const	{ return palloc; }

	T *	alloc()	{
			void *p = palloc->alloc(sizeof(T));
			if( p==NULL ) return NULL;
			return new(p) T();
		}

	void	free( T *p )	{
			if( p==NULL ) return;
			p->~T();
			palloc->free(p);
		}

	A *	palloc;
};

// recyclable object pool based on the specified memory allocator
template<class T, class A>
class	mb_object_pool_t : public object_allocator_t<T> {
public:
	typedef	A	allocator;

	struct blk {
		struct blk_hdr {
		  blk	*next;
		  blk	*prev;
		}	hdr;
		T	value;
	};

		mb_object_pool_t( A *p=NULL ) : palloc(p),
					freepool(NULL), last(NULL)	{}

		~mb_object_pool_t()		{ clear(); drain(); }

	void	set_allocator( A *p )	{ palloc=p; }
	A *	get_allocator() const	{ return palloc; }

	T *	alloc()	{
			blk *p = freepool;
			if( p==NULL ) {
				void *pp = palloc->alloc(sizeof(blk));
				if( pp==NULL ) return NULL;
				p = new(pp) blk();
			} else {
				freepool = p->hdr.next;
			}
			if (p!=NULL ) {
				p->hdr.next = NULL;
				p->hdr.prev = last;
			}
			if( last!=NULL ) last->hdr.next = p;
			last = p;
			if ( p!=NULL )
				return &(p->value);
			else
				return NULL;
		}

	void	free( T *pt ) {
			blk *p = (blk *)(((unsigned char *)pt)-sizeof(blk::blk_hdr));
			if( p->hdr.next!=NULL ) (p->hdr.next)->hdr.prev = p->hdr.prev;
			if( p->hdr.prev!=NULL ) (p->hdr.prev)->hdr.next = p->hdr.next;
			if( last==p ) last = p->hdr.prev;
			p->hdr.next = freepool;
			freepool = p;
		}

	// deallocate all allocated blocks (move to the free list)
	void	clear()	{
			while( last!=NULL ) {
				last->hdr.next = freepool;
				freepool = last;
				last = last->hdr.prev;
			}
		}

	// free the free list
	void	drain() {
			while( freepool!=NULL ) {
				blk *p = freepool;
				freepool = p->hdr.next;
				p->~blk();
				palloc->free(p);
			}
		}

protected:
	A	*palloc;
	blk	*freepool;
	blk	*last;
};

// specializations for the standard malloc allocator:
//
// static_malloc_allocator: static allocator
// std_malloc_allocator: non-static malloc allocator
// mb_malloc_allocator: malloc allocator with virtual interface

typedef	static_allocator_t<static_malloc_allocator> std_malloc_allocator;
typedef	mb_static_allocator_t<static_malloc_allocator> mb_malloc_allocator;

template<class T>
class malloc_object_pool : public mb_object_pool_t<T,std_malloc_allocator> {};

// instance of the standard malloc allocator
extern	mb_malloc_allocator	std_malloc_allocator_imp;

////////////////////////////////////////////////////////////////
// WIN32-specific allocators

#ifdef _WIN32

#ifndef	STRICT
#define	STRICT
#endif	// STRICT

#include <windows.h>
#include <objbase.h>

// standard 'CoTaskMemAlloc' allocator
class	static_ole_allocator {
public:
 static	void *	alloc( size_t sz )
			{ return ::CoTaskMemAlloc(sz); }

 static	void *	realloc( void *p, size_t old_sz, size_t sz )
			{ return ::CoTaskMemRealloc(p,sz); }

 static	void	free( void *p )
			{ ::CoTaskMemFree(p); }
};

#endif	// _WIN32


#endif	// _MBALLOC_H_

