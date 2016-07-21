#ifndef _DPRINTF_H_
#ifdef __cplusplus
extern "C" {
#endif

#ifdef _DEBUG
	int dprintf(const char* msg, ...);
#else
#define dprintf //
#endif

#ifdef __cplusplus
}
#endif

#endif//_DPRINTF_H_
