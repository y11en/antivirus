#ifndef __TRACELOG_H
#define __TRACELOG_H

#ifdef _MY_TRACE

  enum ret_type { rt_void, rt_num, rt_str, rt_ptr, rt_class };

  struct Tracer {
    static int  g_indent;
    static char g_trace_file[MAX_PATH];

    ret_type      m_rt;
    unsigned long m_bp;
    const char*   m_phrase;
    int           m_ph_len;
    /*
    const char*   m_file;
    int           m_fi_len;
    */

    Tracer( ret_type rt, const char* phrase );
    ~Tracer();
    void Trace( const char* pre );

  };

  #ifdef _DEBUG
    #define TRACE_THE_PROC(a,b) \
      Tracer _m_t_(a,b);\
      __asm { mov _m_t_.m_bp, ebp }
  #else
    #define TRACE_THE_PROC(a,b) \
      Tracer _m_t_(a,b);\
      __asm { mov _m_t_.m_bp, ebp }
  #endif
  #define TRACE_POINT(a) _m_t_.Trace(a);

#else

  #define TRACE_THE_PROC(a,b)
  #define TRACE_POINT(a)

#endif


#endif


