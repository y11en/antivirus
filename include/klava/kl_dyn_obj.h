// kl_dyn_obj.h
//

#ifndef kl_dyn_obj_INCLUDED
#define kl_dyn_obj_INCLUDED

#include <kl_platform.h>
#include <klava/kl_holders.h>

struct KLDynObject;

#ifdef __cplusplus

////////////////////////////////////////////////////////////////
// Dynamically-created object, destroyable via destroy() method

struct KL_NO_VTABLE KLDynObject
{
	// implementation of this method should do something like 'delete this;'
	virtual void KL_CDECL destroy () = 0;
};

////////////////////////////////////////////////////////////////
// Dynamic object holder

template <class T>
class KLDynObjectTraits {
public:
 typedef T * Ptr_t;
 static void init (Ptr_t& p) { p = NULL; }
 static bool isValid (const Ptr_t& p)	{ return p != NULL; }
 static void destroy (const Ptr_t& p)	{ p->destroy (); }
};

KL_HOLDER_DEFINE_T(KLDynObjectHolder,KLDynObjectTraits);

////////////////////////////////////////////////////////////////
// Enumerator

template <class T>
class KL_NO_VTABLE KLObjectEnum : public KLDynObject {
public:
	virtual T * getNextItem () = 0;
};
#endif //__cplusplus
#endif // kl_dyn_obj_INCLUDED



