
#include "stdafx.h"
#include <stuff\TraceLog.h>
#include <stuff\PathStr.h>

#ifdef _MY_TRACE

int  Tracer::g_indent;
char Tracer::g_trace_file[MAX_PATH];

const char* skip_space( const char* p ) {
  while( *p != 0 && *p == ' ' ) p++;
  return p;
}

// ---
Tracer::Tracer( ret_type rt, const char* phrase ) 
  : m_rt(rt),
    m_bp(0),
    m_phrase(phrase), 
    m_ph_len( ::lstrlen(m_phrase) ) /*,
    m_file(file),
    m_fi_len( m_file ? ::lstrlen(m_file) : 0 ) */
{ 
  ::GetVersion();
  Trace("{ "); 
  g_indent++;
}



// ---
Tracer::~Tracer() { 
  CPathStr ret( "} " );
  g_indent--;
  #ifdef _DEBUG
    if ( m_rt != rt_void && m_rt != rt_class ) {

      DWORD res = 0;
      BYTE* code_bytes;
      BOOL  known = FALSE;

      __asm mov eax, [ebp+4]
      __asm mov code_bytes, eax

      if ( *code_bytes == 0x8a ) {
        code_bytes++;
        switch( *code_bytes ) {
          case 0x45 :  // mov eax, [bp-10h]
            known = TRUE;
            res = *(BYTE*)(m_bp+*((char*)(code_bytes+1)));
            break;
          case 0x85 :
            known = TRUE;
            res = *(BYTE*)(m_bp+*((short*)(code_bytes+1)));
            break;
          default :
            code_bytes = code_bytes;
            break;
        }
      }
      else if ( *code_bytes == 0x8b ) {
        code_bytes++;
        switch( *code_bytes ) {
          case 0x45 :  // mov eax, [bp-10h]
            known = TRUE;
            res = *(DWORD*)(m_bp+*((char*)(code_bytes+1)));
            break;
          case 0x85 :
            known = TRUE;
            res = *(DWORD*)(m_bp+*((short*)(code_bytes+1)));
            break;
          default :
            code_bytes = code_bytes;
            break;
        }
      }
      else 
        code_bytes = code_bytes;

      if ( known ) {
        char r[50];
        switch( m_rt ) {
          case rt_num :
            ::wsprintf( r, "%d(0x%x) ", res, res );
            break;
          case rt_str :
            if ( res ) {
              ::lstrcpyn( r, (char*)res, sizeof(r)-1 );
              r[sizeof(r)-1] = 0;
            }
            else 
              ::lstrcpy( r, "null " );
            break;
          case rt_ptr :
            if ( res ) 
              ::wsprintf( r, "0x%x ", res );
            else 
              ::lstrcpy( r, "null " );
            break;
        }
        ret += " = ";
        ret += r;
      }
      else 
        ret += " !!! ";
    }
  #endif
  Trace( ret );   
}

// ---
void Tracer::Trace( const char* pre ) {
	HANDLE trace;

  if ( g_trace_file[0] == 0 ) {
    DWORD len = ::GetModuleFileName( 0, g_trace_file, sizeof(g_trace_file) );
    //DWORD len = GetCurrentDirectory( sizeof(g_trace_file),  g_trace_file );

    if ( len ) {
      char* slh = ::strrchr( g_trace_file, '\\' );
      if ( slh )
        *slh = 0;

      char* dot = ::strrchr( g_trace_file, '.' );
      slh = ::strrchr( g_trace_file, '\\' );
      if ( dot > slh ) 
        *dot = 0;

      ::lstrcat( g_trace_file, "\\" );
      ::lstrcat( g_trace_file, "DTool.log" );
      ::DeleteFile( g_trace_file );

    }
  }
  if ( g_trace_file[0] != 0 ) {
    DWORD written;
		trace = ::CreateFile( g_trace_file, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_ALWAYS, FILE_FLAG_RANDOM_ACCESS, NULL );
    if ( trace != INVALID_HANDLE_VALUE ) {
			::SetFilePointer( trace, 0, NULL, FILE_END );
			/*
			if ( m_fi_len ) {
				trace.Write( m_file, m_fi_len );
				trace.Write( "\t- ", 3 );
			}
			*/
			if ( g_indent > 0 ) {
				char* buff = new char [g_indent*2];
				::memset( buff, ' ', g_indent*2 );
			  ::WriteFile( trace, buff, g_indent*2, &written, NULL );
			}

			::WriteFile( trace, pre, ::lstrlen(pre), &written, NULL );
			::WriteFile( trace, m_phrase, m_ph_len, &written, NULL );
			::WriteFile( trace, "\r\n", 2, &written, NULL );

			::CloseHandle( trace );
		}
  }
}

#endif


