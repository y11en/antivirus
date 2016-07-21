#if !defined (_DEBUGTRACE_H_)
#define _DEBUGTRACE_H_

#include <stdio.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>

//#define DEBUG_TRACE
//#define FRAME_TRACE


#ifdef DEBUG_TRACE
#define TRACE(format, ...) fprintf(stderr, "[%d] %s - ", getpid(), __PRETTY_FUNCTION__); fprintf (stderr, format, ## __VA_ARGS__)
#define TRACE_HEX(address,size) \
{ \
  fprintf(stderr, "[%d] %s - ", getpid(), __PRETTY_FUNCTION__); \
  const unsigned char* l_value = reinterpret_cast<const unsigned char*>(address); \
  for(unsigned int i = 0; i < size; ++i) \
  { \
      fprintf(stderr, "%02X ", *(l_value + i)); \
  } \
  fprintf(stderr, "\n"); \
}
#else // DEBUG_TRACE
#define TRACE(format, ...)
#define TRACE_HEX(value,size) 
#endif // DEBUG_TRACE

#if defined (FRAME_TRACE)
struct EnterExitTracer {
  EnterExitTracer(const char* name) : funcname(name) 
  { fprintf(stderr, "[%d] %s - enter\n", getpid(), funcname); }
  ~EnterExitTracer () 
  { fprintf(stderr, "[%d] %s - exit\n", getpid(), funcname); }
  const char* funcname;
};
#define ENTER EnterExitTracer EnterExitTracerInstance (__PRETTY_FUNCTION__)
#else
#define ENTER
#endif

#endif //_DEBUGTRACE_H_
