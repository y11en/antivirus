// kl_fnmatch.h
//

#ifndef kl_fnmatch_h_INCLUDED
#define kl_fnmatch_h_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

enum // flags
{
    KL_FNMATCH_NO_ESCAPE = 0x0001  // do not treat '\\' as escape symbol
};

// returns 1 if string matches pattern, 0 otherwise
int kl_fnmatch (const char *str, const char *pattern, unsigned int flags);

#ifdef __cplusplus
} // extern "C"
#endif // __cplusplus

#ifdef KL_FNMATCH_IMPL
# include "../../klava/kernel/klavcorelib/kl_fnmatch.c"
#endif // KL_FNMATCH_IMPL

#endif // kl_fnmatch_h_INCLUDED

