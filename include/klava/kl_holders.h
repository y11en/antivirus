// kl_holders.h
//
//

#ifndef kl_holders_h_INCLUDED
#define kl_holders_h_INCLUDED

#include <stdlib.h>
#ifdef __cplusplus

////////////////////////////////////////////////////////////////
// 'smart holder' for objects of arbitrary structure
//
// The template class TRAITS should look like:
//
// class XXX_TRAITS {
// public:
// typedef ... Ptr_t;
//   static void init (Ptr_t& p);
//   static bool isValid (const Ptr_t& p);
//   static void destroy (const Ptr_t& p);
// };
//

#ifdef _MSC_VER
# pragma warning(disable:4284)
#endif


template<class TRAITS>
class KLHolder {
public:
	typedef typename TRAITS::Ptr_t Ptr_t;

	KLHolder ()	{ TRAITS::init (m_p); }
	explicit KLHolder (const Ptr_t& p) : m_p (p) {}
	KLHolder (KLHolder& src) : m_p (src.detach ()) {}
	~KLHolder () { if (TRAITS::isValid (m_p)) TRAITS::destroy (m_p); }

	void	clear () {
				if (TRAITS::isValid (m_p)) {
					TRAITS::destroy (m_p);
					TRAITS::init (m_p);
			}
		}

	Ptr_t	detach ()
		{ Ptr_t tmp = m_p; TRAITS::init (m_p); return tmp; }

	KLHolder& operator= (const Ptr_t& p)
		{ clear (); m_p = p; return *this; }

	KLHolder& operator= (KLHolder& src)
		{ clear (); m_p = src.detach (); return *this; }

	operator const Ptr_t& () const   { return m_p; }
	const Ptr_t& operator-> () const { return m_p; }

	const Ptr_t& get () const	{ return m_p; }

	bool isValid () const	{ return TRAITS::isValid (m_p); }
	bool operator! () const	{ return ! isValid (); }

	Ptr_t&  ptr ()	{ return m_p; }

private:
	Ptr_t	m_p;
};

#ifdef _MSC_VER
# pragma warning(default:4284)
#endif

#define KL_HOLDER_DEFINE(NAME,TRAITS) \
typedef KLHolder<TRAITS> NAME;


#define KL_HOLDER_DEFINE_T(NAME,TRAITS) \
template<class T> \
class NAME : public KLHolder <TRAITS<T> > { \
public: \
	typedef KLHolder <TRAITS<T> > Base; \
	NAME () : Base () {} \
	NAME (const typename Base::Ptr_t& p) : Base (p) {} \
	NAME& operator= (const typename Base::Ptr_t& p) \
		{ Base::operator=(p); return *this; } \
};


////////////////////////////////////////////////////////////////
// Holder for memory, allocated by calling operator new(size)

class KLNewMemTraits {
public:
 typedef void * Ptr_t;
 static void init (Ptr_t& p) { p = NULL; }
 static bool isValid (const Ptr_t& p)	{ return p != NULL; }
 static void destroy (const Ptr_t& p)	{ operator delete (p); }
};
KL_HOLDER_DEFINE(KLNewMemHolder,KLNewMemTraits);

////////////////////////////////////////////////////////////////
// Holder for memory buffers, created using malloc()

class KLMallocTraits {
public:
 typedef void * Ptr_t;
 static void init (Ptr_t& p) { p = NULL; }
 static bool isValid (const Ptr_t& p)	{ return p != NULL; }
 static void destroy (const Ptr_t& p)	{ ::free (p); }
};
KL_HOLDER_DEFINE(KLMallocHolder,KLMallocTraits);

////////////////////////////////////////////////////////////////
// Holder for objects, created by calling new T()

template<class T>
class KLObjectTraits {
public:
 typedef T * Ptr_t;
 static void init (Ptr_t& p) { p = NULL; }
 static bool isValid (const Ptr_t& p)	{ return p != NULL; }
 static void destroy (const Ptr_t& p)	{ delete p; }
};
KL_HOLDER_DEFINE_T(KLObjectHolder,KLObjectTraits);

////////////////////////////////////////////////////////////////
// Holder for arrays, created by calling new T[N]

template<class T>
class KLArrayTraits {
public:
 typedef T * Ptr_t;
 static void init (Ptr_t& p) { p = NULL; }
 static bool isValid (const Ptr_t& p)	{ return p != NULL; }
 static void destroy (const Ptr_t& p)	{ delete[] p; }
};
KL_HOLDER_DEFINE_T(KLArrayHolder,KLArrayTraits);

#endif // __cplusplus
#endif // kl_holders_h_INCLUDED
