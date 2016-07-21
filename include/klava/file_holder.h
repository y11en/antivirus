// file_holder.h
//

#ifndef file_holder_h_INCLUDED
#define file_holder_h_INCLUDED

#include <stdio.h>
#include <klava/kl_holders.h>

class KLFileTraits {
public:
	typedef FILE * Ptr_t;
	static void init (Ptr_t& p) { p = 0; }
	static bool isValid (const Ptr_t& p)	{ return p != 0; }
	static void destroy (const Ptr_t& p)	{ fclose (p); }
};
KL_HOLDER_DEFINE(KLFileHolder,KLFileTraits);

#endif // file_holder_h_INCLUDED


