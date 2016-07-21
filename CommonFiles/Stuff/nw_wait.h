#ifndef __nw_wait_h
#define __nw_wait_h

#ifndef WAIT_ATOM 
  #define WAIT_ATOM (10)
#endif

#define INFINITE_TIME (~0L)
typedef int (*StopConditionProc)( void* param );



#ifdef __cplusplus
extern "C" {
#endif

// function performs delay for non windows applications
// milliseconds - timeout, may be INFINITE_TIME
// stop         - stop condition
// whole timeout interval is broken down to 10 ms periods
// and on each loop stop condition checked if presented
// result value is TRUE
int Condition_Wait( unsigned milliseconds, StopConditionProc stop, void* param );

#ifdef __cplusplus
}
#endif

#endif

