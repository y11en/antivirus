/***
*output.c - printf style output to a FILE
*
*       Copyright (c) 1989-1997, Microsoft Corporation. All rights reserved.
*
*Purpose:
*       This file contains the code that does all the work for the
*       printf family of functions.  It should not be called directly, only
*       by the *printf functions.  We don't make any assumtions about the
*       sizes of ints, longs, shorts, or long doubles, but if types do overlap,
*       we also try to be efficient.  We do assume that pointers are the same
*       size as either ints or longs.
*       If CPRFLAG is defined, defines _cprintf instead.
*       **** DOESN'T CURRENTLY DO MTHREAD LOCKING ****
*
*******************************************************************************/

/* temporary work-around for compiler without 64-bit support */


//
//#ifdef _INTEGRAL_MAX_BITS
//  #undef _INTEGRAL_MAX_BITS  
//#endif  /* _INTEGRAL_MAX_BITS */
#include <Prague/prague.h>
#include <Prague/iface/i_string.h>

#define IMPEX_IMPORT_LIB
#include <Prague/iface/e_string.h>

#undef 	IMPEX_IMPORT_LIB
#define IMPEX_EXPORT_LIB
#include <Prague/iface/e_ktime.h>

#include "kernel.h"

#include <limits.h>
#include <stdarg.h>

#define DEF_IDENTIFIER(s,v) { v, #s },

typedef struct tag_NameArr {
	tUINT       id;
	const char* name;
} tNameArr;

#if !defined( NO_PRAGUE_EXCEPT )
	#define PRAGUE_EXCEPT
#endif

#if defined( PRAGUE_EXCEPT )

	#define TRY_ENTER         { tExceptionReg er; \
														  if ( errOK == si.EnterProtectedCode(0,&er) )
	#define EXCEPT_ENTER        else
	#define EXCEPT_LEAVE        si.LeaveProtectedCode( 0, &er ); }

#else
	#include <excpt.h>

	#define TRY_ENTER         __try
	#define EXCEPT_ENTER      __except ( EXCEPTION_EXECUTE_HANDLER )
	#define EXCEPT_LEAVE

#endif

#if defined (_M_X64)
  typedef __int64 textlen_t;
#else
  typedef int textlen_t;
#endif


// ---
#ifndef _DEBUG

	tNameArr g_errs[]  = {{ 0, 0 }};
	tNameArr g_iids[]  = {{ 0, 0 }};
	tNameArr g_pids[]  = {{ 0, 0 }};
	tNameArr g_props[] = {{ 0, 0 }};
	tNameArr g_oids[]  = {{ 0, 0 }};
	tNameArr g_vids[]  = {{ 0, 0 }};
	tNameArr g_pmcs[]  = {{ 0, 0 }};
	tNameArr g_pms[]   = {{ 0, 0 }};
	tNameArr g_ser[]   = {{ 0, 0 }};

#else

	#pragma check_stack

	tNameArr g_errs[] = {
		#include <Prague/pr_d_err.h>
	};
	tNameArr g_iids[] = {
		#include <Prague/pr_d_iid.h>
	};
	tNameArr g_pids[] = {
		#include <Prague/pr_d_pid.h>
	};
	tNameArr g_props[] = {
		#include <Prague/pr_d_pg.h>
	};
	tNameArr g_oids[] = {
		#include <Prague/pr_d_oid.h>
	};
	tNameArr g_vids[] = {
		#include <Prague/pr_d_vid.h>
	};
	tNameArr g_pmcs[] = {
		#include <Prague/pr_d_pmc.h>
	};
	tNameArr g_pms[] = {
		#include <Prague/pr_d_pm.h>
	};
	tNameArr g_ser[] = {
		#include <Prague/pr_d_sid.h>
	};

	// ---
	const char* _find_named_id( tNameArr* arr, tUINT size, tUINT id ) {
		tUINT i;
		for( i=0; i<size; i++, arr++ ) {
			if ( arr->id == id )
				return arr->name;
		}
		return 0;
	}
	#undef DEF_IDENTIFIER
#endif // _DEBUG


// ---
// opcode's
typedef enum tag_FormatOp {
	op_unk,
	op_ansi_hStr,
	//op_unicode_hStr,
	op_local_time,
	op_gmt_time,
	op_local_dt,
	op_gmt_dt,
	op_named,
} tFormatOp;


typedef struct tag_FormatSpec {
	const tCHAR* m_id;
	tFormatOp    m_op;
	tNameArr*    m_names;
	tUINT        m_count;
} tFormatSpec;

tFormatSpec g_format_specs[] = {
	{ "str", op_ansi_hStr,    0, 0 },
//{ "STR", op_unicode_hStr, 0, 0 },
	{ "lt",  op_local_time,   0, 0 },
	{ "gt",  op_gmt_time,     0, 0 },
	{ "ldt", op_local_dt,     0, 0 },
	{ "gdt", op_gmt_dt,       0, 0 },
	{ "err", op_named,        g_errs,  countof(g_errs)  },
	{ "iid", op_named,        g_iids,  countof(g_iids)  },
	{ "pid", op_named,        g_pids,  countof(g_pids)  },
	{ "prop",op_named,        g_props, countof(g_props) },
	{ "oid", op_named,        g_oids,  countof(g_oids)  },
	{ "vid", op_named,        g_vids,  countof(g_vids)  },
	{ "msg", op_named,        g_pmcs,  countof(g_pmcs)  },
	{ "mc",  op_named,        g_pmcs,  countof(g_pmcs)  },
	{ "mid", op_named,        g_pms,   countof(g_pms)   },
	{ "ser", op_named,        g_ser,   countof(g_ser)   },
	{ "",    op_unk,          0, 0 },
};

tFormatSpec g_null_format = { 0, op_unk, 0, 0 };

// ---
static tFormatSpec* _find_format( const tCHAR** format ) {
	tUINT i;
	tFormatSpec* spec = g_format_specs;
	for( i=0; i<countof(g_format_specs); ++i,++spec ) {
		const tCHAR* p1 = spec->m_id;
		const tCHAR* p2 = *format;
		tBOOL found = cTRUE;
		while( *p1 ) {
			if ( *p1 != *p2 ) {
				found = cFALSE;
				break;
			}
			++p1;
			++p2;
		}
		if ( found ) {
			*format = p2;
			return spec;
		}
	}
	return &g_null_format;
}


//char* from_hSTRING_unicode( hSTRING str, int*len, char* buffer );
char* from_hSTRING( hSTRING str, textlen_t* len, char* buffer );

#if !defined( NO_TIME_OUTPUT_IMPL )

	// ---
	char* local_time( int time, textlen_t* len, char* buffer );
	char* gmt_time( int time, textlen_t* len, char* buffer );
	char* local_dt( const tDT* time, textlen_t* len, char* buffer );
	char* gmt_dt( const tDT* time, textlen_t* len, char* buffer );

	#define  _TQW(dt)  (*(tQWORD*)&dt)

#endif

#if (_MSC_VER > 1200 )
	#define __inline
#else
	#define __inline static
#endif

#ifdef _MBCS
  #undef _MBCS
#endif

#if defined (__unix__)
  #define _T(a) (a)
  #include <wchar.h>  
  #define TCHAR char
#else
  #include <tchar.h>
#endif


// ---
tUINT IMPEX_FUNC(pr_sprintf)( tCHAR* stream, tINT length, const tCHAR* format, ... ) {
	tUINT ret;
	va_list argptr;
	va_start( argptr, format );
	ret = pr_vsprintf( stream, length, format, VA_LIST_ADDR(argptr) );
	va_end( argptr );
	return ret;
}



/* this macro defines a function which is private and as fast as possible: */
/* for example, in C 6.0, it might be static _fastcall <type> near. */
#define LOCAL(x) static x __cdecl

/* int/long/short/pointer sizes */

/* the following should be set depending on the sizes of various types */
#define LONG_IS_INT      1      /* 1 means long is same size as int */
#define SHORT_IS_INT     0      /* 1 means short is same size as int */

#if LONG_IS_INT
  #define get_long_arg(x) (long)get_int_arg(x)
#endif  /* LONG_IS_INT */




/* CONSTANTS */

/* size of conversion buffer (ANSI-specified minimum is 509) */

#define BUFFERSIZE    256

/* flag definitions */
#define FL_SIGN       0x00001   /* put plus or minus in front */
#define FL_SIGNSP     0x00002   /* put space or minus in front */
#define FL_LEFT       0x00004   /* left justify */
#define FL_LEADZERO   0x00008   /* pad with leading zeros */
#define FL_LONG       0x00010   /* long value given */
#define FL_SHORT      0x00020   /* short value given */
#define FL_SIGNED     0x00040   /* signed data given */
#define FL_ALTERNATE  0x00080   /* alternate form requested */
#define FL_NEGATIVE   0x00100   /* value is negative */
#define FL_FORCEOCTAL 0x00200   /* force leading '0' for octals */
#define FL_LONGDOUBLE 0x00400   /* long double value given */
#define FL_WIDECHAR   0x00800   /* wide characters */
#define FL_I64        0x08000   /* __int64 value given */

#if defined (_M_X64)
	#define FL_PTR      FL_I64   /* ptr is the same as __int64 */
#elif defined(_Wp64)
	#define FL_PTR      0        /* ptr is the same as int */
#else
	#define FL_PTR      0        /* ptr is the same as int */
#endif

/* state definitions */
enum STATE {
  /* 0 */ ST_NORMAL,          /* normal state; outputting literal chars */
  /* 1 */ ST_PERCENT,         /* just read '%' */
  /* 2 */ ST_FLAG,            /* just read flag character */
  /* 3 */ ST_WIDTH,           /* just read width specifier */
  /* 4 */ ST_DOT,             /* just read '.' */
  /* 5 */ ST_PRECIS,          /* just read precision specifier */
  /* 6 */ ST_SIZE,            /* just read size specifier */
  /* 7 */ ST_TYPE             /* just read type specifier */
};
#define NUMSTATES (ST_TYPE + 1)

/* character type values */
enum CHARTYPE {
	CH_OTHER,           /* character with no special meaning */
	CH_PERCENT,         /* '%' */
	CH_DOT,             /* '.' */
	CH_STAR,            /* '*' */
	CH_ZERO,            /* '0' */
	CH_DIGIT,           /* '1'..'9' */
	CH_FLAG,            /* ' ', '+', '-', '#' */
	CH_SIZE,            /* 'h', 'l', 'L', 'N', 'F', 'w' */
	CH_TYPE             /* type specifying character */
};

/* static data (read only, since we are re-entrant) */
char     __bad_string[] = "(bad str)";  /* string to print on null ptr */
#define  __bad_size (sizeof(__bad_string)-1)

char     __null_string[] = "(null str)";  /* string to print on null ptr */
#define  __null_size (sizeof(__null_string)-1)

wchar_t  __w_null_string[] = L"(null)";/* string to print on null ptr */

/* The state table.  This table is actually two tables combined into one. */
/* The lower nybble of each byte gives the character class of any         */
/* character; while the uper nybble of the byte gives the next state      */
/* to enter.  See the macros below the table for details.                 */
/*                                                                        */
/* The table is generated by maketabc.c -- use this program to make       */
/* changes.                                                               */
/* Table generated by maketabc.c built with -D_WIN32_. Defines additional */
/* format code %Z for counted string.                                     */
//#ifdef   PRAGUE_FORMATS      
  #define TCBF 0x08
//#else
//  #define TCBF 0x00
//#endif

const char _look_up_table[] = {
  0x06, 0x00, 0x00, 0x06, 0x00, 0x01, 0x00, 0x00,
  0x10, 0x00, 0x03, 0x06, 0x00, 0x06, 0x02, 0x10,
  0x04, 0x45, 0x45, 0x45, 0x05, 0x05, 0x05, 0x05,
  0x05, 0x35, 0x30, 0x00, 0x50, 0x00, 0x00, 0x00,
  0x00, 0x20, 0x28, 0x38, 0x50, 0x58, 0x07, 0x08,
  0x00, 0x37, 0x30, 0x30, 0x57, 0x50, 0x07, 0x00,
  0x00, 0x20, 0x20, 0x08, 0x00, 0x00, 0x00, 0x00,
  0x08, 0x60,
  0x68, /* 'Z' (extension for NT development) */
                    0x60, 0x60, 0x60, 0x60, 0x00,
  0x00, 0x70, 0x70, 0x78, 0x78, 0x78, 0x78, 0x08,
  0x07, 0x08, 0x00, 0x00, 0x07, 0x00, 0x08, 0x08,
  0x08, 0x00, 0x00, 0x08, TCBF, 0x08, 0x00,
  0x07, /* 'w' (extension for NT development) */
  0x08
};


#define find_char_class(c)         ((c) < _T(' ') || (c) > _T('x') ? CH_OTHER : _look_up_table[(c)-_T(' ')] & 0xF)
#define find_next_state(cls, st)   (_look_up_table[(cls) * NUMSTATES + (st)] >> 4)


/* prototypes */
LOCAL(void) write_multi_char( int ch, int* size, textlen_t num, char** out, textlen_t* pnumwritten );
LOCAL(void) write_string( char *string, int* size, textlen_t len, char** out, textlen_t* numwritten );

typedef struct {
  va_list argptr;
}va_list_wrapper;

#if defined (_MSC_VER)
	#define VA_COPY(dst,src) dst=src;
#else
	#define VA_COPY(dst,src) va_copy(dst,src)
#endif


#if !SHORT_IS_INT
__inline short __cdecl get_short_arg(va_list_wrapper *pargptr);
#endif  /* !SHORT_IS_INT */

__inline int   __cdecl get_int_arg(va_list_wrapper *pargptr);
__inline void* __cdecl get_ptr_arg(va_list_wrapper *pargptr);

#if !LONG_IS_INT
__inline long __cdecl get_long_arg(va_list_wrapper *pargptr);
#endif  /* !LONG_IS_INT */

//#if _INTEGRAL_MAX_BITS >= 64   
__inline __int64 __cdecl get_int64_arg(va_list_wrapper *pargptr);
//#endif  /* _INTEGRAL_MAX_BITS >= 64    */



/***
*int _output(stream, format, argptr), static int output(format, argptr)
*
*Purpose:
*   Output performs printf style output onto a stream.  It is called by
*   printf/fprintf/sprintf/vprintf/vfprintf/vsprintf to so the dirty
*   work.  In multi-thread situations, _output assumes that the given
*   stream is already locked.
*
*   Algorithm:
*       The format string is parsed by using a finite state automaton
*       based on the current state and the current character read from
*       the format string.  Thus, looping is on a per-character basis,
*       not a per conversion specifier basis.  Once the format specififying
*       character is read, output is performed.
*
*Entry:
*   FILE *stream   - stream for output
*   char *format   - printf style format string
*   va_list argptr - pointer to list of subsidiary arguments
*
*Exit:
*   Returns the number of characters written, or -1 if an output error
*   occurs.
*ifdef _UNICODE
*   The wide-character flavour returns the number of wide-characters written.
*endif
*
*Exceptions:
*
*******************************************************************************/

tUINT IMPEX_FUNC(pr_vsprintf)( tCHAR* stream, tINT length, const tCHAR* format, va_list argptr ) {
	int             hexadd = 0;       /* offset to add to number to get 'a'..'f' */
	char            ch;               /* character just read */
	int             flags = 0;        /* flag word -- see #defines above for flag values */
	enum STATE      state;            /* current state */
	enum CHARTYPE   chclass;          /* class of current character */
	int             radix;            /* current conversion radix */
	textlen_t       charsout;         /* characters currently written so far, -1 = IO error */
	int             fldwidth = 0;     /* selected field width -- 0 means default */
	int             precision = 0;    /* selected precision  -- -1 means default */
	char            prefix[2];        /* numeric prefix -- up to two characters */
	int             prefixlen = 0;    /* length of prefix -- 0 means no prefix */
	int             capexp;           /* non-zero = 'E' exponent signifient, zero = 'e' */
	int             no_output = 0;    /* non-zero = prodcue no output for this specifier */
	union {
		char*    sz;                    /* pointer text to be printed, not zero terminated */
		wchar_t* wz;
	} text;
	textlen_t       textlen;            /* length of the text in bytes/wchars to be printed.
                                         textlen is in multibyte or wide chars if _UNICODE */
	char            buffer[BUFFERSIZE];
	
	wchar_t         wchar;              /* temp wchar_t */
	int             bufferiswide = 0;   /* non-zero = buffer contains wide chars already */
	va_list_wrapper argptr_wrapper;
	tINT            start_length = length;
	tCHAR*          start_point = stream;
	const tCHAR*    start_format = format;
	va_list_wrapper start_argptr;
	tCHAR*          end_point = 0;
	tCHAR*          format_start = (tCHAR*)format; /* prague */
	tFormatSpec*    fspec;

	#if !defined( NO_TIME_OUTPUT_IMPL )
		tDT             dt;
	#endif

	VA_COPY ( start_argptr.argptr,  argptr  );
	VA_COPY ( argptr_wrapper.argptr, argptr );

	charsout = 0;           /* no characters written yet */

	TRY_ENTER {

		if ( stream && length ) {
			end_point = stream + length - 1;
			*end_point = 'z';
		}

		//if ( length < 2 ) {
		//  if ( length == 1 )
		//    *stream = 0;
		//  return length;
		//}
		//length--;

		if ( !stream || (length < 0) )
			length = 0;

		textlen = 0;            /* no text yet */
		charsout = 0;           /* no characters written yet */
		state = ST_NORMAL;      /* starting state */
		
		/* main loop -- loop while format character exist and no I/O errors */
		while( ((ch = *format++) != _T('\0')) && (charsout >= 0) /*&& (length>0)*/ ) {
			chclass = find_char_class(ch);  /* find character class */
			state = find_next_state(chclass, state); /* find next state */
			/* execute code for each state */
			switch (state) {
				
				case ST_NORMAL:
					NORMAL_STATE:
						/* normal state -- just write character */
						bufferiswide = 0;
						// if ( isleadbyte((int)(unsigned char)ch) ) {
						//   *stream++ = ch;
						//   charsout++;
						//   ch = *format++;
						// }
						if ( length > 1 ) {
							*stream++ = ch;
							--length;
						} 
						else if ( length == 1 )
							length = *stream = 0;
						charsout++;
						break;

				case ST_PERCENT:
					/* set default value of conversion parameters */
					prefixlen = fldwidth = no_output = capexp = 0;
					flags = 0;
					precision = -1;
					bufferiswide = 0;   /* default */
					break;

				case ST_FLAG:
					/* set flag based on which flag character */
					switch (ch) {
						case _T('-'):
							flags |= FL_LEFT;   /* '-' => left justify */
							break;
						case _T('+'):
							flags |= FL_SIGN;   /* '+' => force sign indicator */
							break;
						case _T(' '):
							flags |= FL_SIGNSP; /* ' ' => force sign or space */
							break;
						case _T('#'):
							flags |= FL_ALTERNATE;  /* '#' => alternate form */
							break;
						case _T('0'):
							flags |= FL_LEADZERO;   /* '0' => pad with leading zeros */
							break;
					}
					break;

				case ST_WIDTH:
					if ( ch == _T('*') ) { /* update width value */
						fldwidth = get_int_arg(&argptr_wrapper); /* get width from arg list */
						if (fldwidth < 0) { /* ANSI says neg fld width means '-' flag and pos width */
							flags |= FL_LEFT;
							fldwidth = -fldwidth;
						}
					}
					else /* add digit to current field width */
						fldwidth = fldwidth * 10 + (ch - _T('0'));
					break;

				case ST_DOT:
						/* zero the precision, since dot with no number means 0 not default, according to ANSI */
						precision = 0;
						break;

				case ST_PRECIS:
					if ( ch == _T('*') ) { /* update precison value */
						precision = get_int_arg(&argptr_wrapper); /* get precision from arg list */
						if ( precision < 0 )
							precision = -1; /* neg precision means default */
					}
					else /* add digit to current precision */
						precision = precision * 10 + (ch - _T('0'));
					break;
					
				case ST_SIZE:
					/* just read a size specifier, set the flags based on it */
					switch (ch) {
						case _T('l'):
							flags |= FL_LONG;   /* 'l' => long int or wchar_t */
							break;
							
						case _T('I'):
						/* In order to handle the I64 size modifier, we depart from
						* the simple deterministic state machine. The code below
							* scans */
							if ( (*format == _T('6')) && (*(format + 1) == _T('4')) ) {
								format += 2;
								flags |= FL_I64;    /* I64 => __int64 */
							}
							else {
								state = ST_NORMAL;
								goto NORMAL_STATE;
							}
							break;

						#if !SHORT_IS_INT
							case _T('h'):
								flags |= FL_SHORT;  /* 'h' => short int or char */
								break;
						#endif  /* !SHORT_IS_INT */

						case _T('w'): /* UNDONE: support %wc and %ws for now only for compatibility */
							flags |= FL_WIDECHAR;  /* 'w' => wide character */
							break;

					}
					break;

				case ST_TYPE:
					/* we have finally read the actual type character, so we       */
					/* now format and "print" the output.  We use a big switch     */
					/* statement that sets 'text' to point to the text that should */
					/* be printed, and 'textlen' to the length of this text.       */
					/* Common code later on takes care of justifying it and        */
					/* other miscellaneous chores.  Note that cases share code,    */
					/* in particular, all integer formatting is done in one place. */
					/* Look at those funky goto statements!                        */

					switch (ch) {
						case _T('C'):   /* ISO wide character */
							if (!(flags & (FL_SHORT|FL_LONG|FL_WIDECHAR)))
								flags |= FL_WIDECHAR;   /* ISO std. */
							/* fall into 'c' case */

						case _T('c'): {
							
							if (flags & (FL_LONG|FL_WIDECHAR)) { /* print a single character specified by int argument */
								wchar = (wchar_t) get_short_arg(&argptr_wrapper);
								/* convert to multibyte character */
								if ( wchar > 255 ) {/* validate high byte */
									textlen = -1;
									no_output = 1;
								}
								else {
									textlen = sizeof(char);
									*buffer = (char)wchar;
								}
							} 
							else {
								/* format multibyte character: this is an extension of ANSI*/
								unsigned short temp = (unsigned short) get_int_arg(&argptr_wrapper);
								buffer[0] = (char) temp;
								textlen = 1;
							}
							text.sz = buffer;
													}
							break;

						case _T('Z'): {
							/* print a Counted String

							int i;
							char *p;       / * temps */
							struct _count_string {
								short Length;
								short MaximumLength;
								char *Buffer;
							} *pstr;
							
							pstr = get_ptr_arg(&argptr_wrapper);
							if (pstr == NULL || pstr->Buffer == NULL) {
								/* null ptr passed, use special string */
								text.sz = __null_string;
								textlen = __null_size;
							} 
							else {
								if (flags & FL_WIDECHAR) {
									text.wz = (wchar_t *)pstr->Buffer;
									textlen = pstr->Length / sizeof(wchar_t);
									bufferiswide = 1;
								} else {
									bufferiswide = 0;
									text.sz = pstr->Buffer;
									textlen = pstr->Length;
								}
							}
							break;
						}

						case _T('S'):   /* ISO wide character string */
							if (!(flags & (FL_SHORT|FL_LONG|FL_WIDECHAR)))
								flags |= FL_WIDECHAR;

						case _T('s'): {
							/* print a string --                            */
							/* ANSI rules on how much of string to print:   */
							/*   all if precision is default,               */
							/*   min(precision, length) if precision given. */
							/* prints '(null)' if a null string is passed   */
							
							int i;
							char *p;       /* temps */
							wchar_t *pwch;
							
							/* At this point it is tempting to use strlen(), but */
							/* if a precision is specified, we're not allowed to */
							/* scan past there, because there might be no null   */
							/* at all.  Thus, we must do our own scan.           */
							
							i = (precision == -1) ? INT_MAX : precision;
							text.sz = get_ptr_arg(&argptr_wrapper);
							
							/* UNDONE: handle '#' case properly */
							/* scan for null upto i characters */
							if (flags & (FL_LONG|FL_WIDECHAR)) {
								if (text.wz == NULL) /* NULL passed, use special string */
									text.wz = __w_null_string;
								bufferiswide = 1;
								pwch = text.wz;
								while ( i-- && *pwch )
									++pwch;
								textlen = pwch - text.wz; /* textlen now contains length in wide chars */
							} 
							else {
								if (text.sz == NULL) /* NULL passed, use special string */
									text.sz = __null_string;
								p = text.sz;
								while (i-- && *p)
									++p;
								textlen = p - text.sz;    /* length of the string */
							}
													}
							break;

						case _T('n'): {
							/* write count of characters seen so far into */
							/* short/int/long thru ptr read from args */
							
							void *p = get_ptr_arg(&argptr_wrapper); /* temp */
							
							/* store chars out into short/long/int depending on flags */
							#if !LONG_IS_INT
								if (flags & FL_LONG)
									*(long *)p = charsout;
								else
							#endif  /* !LONG_IS_INT */
								
							#if !SHORT_IS_INT
								if ( flags & FL_SHORT )
									*(short *)p = (short)charsout;
								else
							#endif  /* !SHORT_IS_INT */
									*(int *)p = (int)charsout;
								
								no_output = 1;              /* force no output */
							}
							break;

						case _T('d'):
						case _T('i'):
							/* signed decimal output */
							flags |= FL_SIGNED;
							radix = 10;
							goto COMMON_INT;
							
						case _T('u'):
							radix = 10;
							goto COMMON_INT;

						case _T('t') : // prague formats: err,iid,pid... output
							format_start = (tCHAR*)format; /* prague */
							fspec = _find_format( &format );
							switch( fspec->m_op ) {
								case op_ansi_hStr :
									text.sz = from_hSTRING( (hSTRING)get_ptr_arg(&argptr_wrapper), &textlen, buffer );
									break;
								//case op_unicode_hStr :
								//	bufferiswide = 1;
								//	text.sz = from_hSTRING( (hSTRING)get_ptr_arg(&argptr_wrapper), cCP_UNICODE, &textlen, buffer, 0 );
								//	break;
								#if !defined( NO_TIME_OUTPUT_IMPL )
									case op_local_time :
										text.sz = local_time( get_int_arg(&argptr_wrapper), &textlen, buffer );
										break;
									case op_gmt_time :
										text.sz = gmt_time( get_int_arg(&argptr_wrapper), &textlen, buffer );
										break;
									case op_local_dt :
										_TQW(dt) = get_int64_arg(&argptr_wrapper);
										text.sz = local_dt( &dt, &textlen, buffer );
										break;
									case op_gmt_dt :
										_TQW(dt) = get_int64_arg(&argptr_wrapper);
										text.sz = gmt_dt( &dt, &textlen, buffer );
										break;
								#endif

								case op_named:
								default: {
									tUINT id;
									va_list stored_arg_ptr; 
									VA_COPY(stored_arg_ptr,argptr_wrapper.argptr); 
									#if defined(_DEBUG)
										id = get_int_arg(&argptr_wrapper);
										if ( fspec->m_names && (0 != (text.sz=(char*)_find_named_id(fspec->m_names,fspec->m_count,id))) )
											textlen = strlen( text.sz );

										else 
									#endif
										{
											flags |= FL_LEADZERO;
											hexadd = _T('A') - _T('9') - 1;     /* set hexadd for uppercase hex */
											precision = 2 * sizeof(tERROR);     /* number of hex digits needed */
											VA_COPY(argptr_wrapper.argptr,stored_arg_ptr);
											if ( fspec->m_op == op_unk ) {
												write_string( "unk prg=0x", &length, 10, &stream, &charsout );
												goto COMMON_HEX;
											}
											write_string( format_start, &length, format - format_start, &stream, &charsout );
											if ( fspec->m_names == g_ser ) { // its a %tser format
												tUINT l;
												#if !defined( _DEBUG )
													id = get_int_arg(&argptr_wrapper);
												#endif
												l = pr_sprintf(stream, length, "=%tpid:%d", id >> 16, (id >> 8) & 0xFF);
												length -= l;
												stream += l;
												charsout += l;
											}
											else {
												write_string( "=0x", &length, 3, &stream, &charsout );
												goto COMMON_HEX;
											}
										}
									break;
								}
							}
							break;

						case _T('p'):
							/* write a pointer -- this is like an integer or long */
							/* except we force precision to pad with zeros and */
							/* output in big hex. */
							
							precision = 2 * sizeof(void *);     /* number of hex digits needed */
							flags |= FL_PTR;                    /* assume we're converting a right value */
							/* DROP THROUGH to hex formatting */
							
						case _T('X'):
							/* unsigned upper hex output */
							hexadd = _T('A') - _T('9') - 1;     /* set hexadd for uppercase hex */
							goto COMMON_HEX;
							
						case _T('x'):
							/* unsigned lower hex output */
							hexadd = _T('a') - _T('9') - 1;     /* set hexadd for lowercase hex */
							/* DROP THROUGH TO COMMON_HEX */

							COMMON_HEX:
								radix = 16;
								if (flags & FL_ALTERNATE) {
									/* alternate form means '0x' prefix */
									prefix[0] = _T('0');
									prefix[1] = (TCHAR)(_T('x') - _T('a') + _T('9') + 1 + hexadd);  /* 'x' or 'X' */
									prefixlen = 2;
								}
								goto COMMON_INT;

						case _T('o'):
							/* unsigned octal output */
							radix = 8;
							if (flags & FL_ALTERNATE) {
								/* alternate form means force a leading 0 */
								flags |= FL_FORCEOCTAL;
							}
							/* DROP THROUGH to COMMON_INT */

							COMMON_INT: {

								/* This is the general integer formatting routine. */
								/* Basically, we get an argument, make it positive */
								/* if necessary, and convert it according to the */
								/* correct radix, setting text and textlen */
								/* appropriately. */
								
								#if _INTEGRAL_MAX_BITS >= 64           
									unsigned __int64 number;    /* number to convert */
									__int64 l;              /* temp long value */
								#else  /* _INTEGRAL_MAX_BITS >= 64            */
									unsigned long number;   /* number to convert */
									long l;                 /* temp long value */
								#endif  /* _INTEGRAL_MAX_BITS >= 64            */

								/* 1. read argument into l, sign extend as needed */
								#if _INTEGRAL_MAX_BITS >= 64       
									if (flags & FL_I64)
										l = get_int64_arg(&argptr_wrapper);
									else
								#endif  /* _INTEGRAL_MAX_BITS >= 64        */

								#if !LONG_IS_INT
									if (flags & FL_LONG)
										l = get_long_arg(&argptr_wrapper);
									else
								#endif  /* !LONG_IS_INT */

								#if !SHORT_IS_INT
									if (flags & FL_SHORT) {
										if (flags & FL_SIGNED)
											l = (short) get_int_arg(&argptr_wrapper); /* sign extend */
										else
											l = (unsigned short) get_int_arg(&argptr_wrapper);    /* zero-extend*/
									} else
								#endif  /* !SHORT_IS_INT */
									{
										if (flags & FL_SIGNED)
											l = get_int_arg(&argptr_wrapper); /* sign extend */
										else
											l = (unsigned int) get_int_arg(&argptr_wrapper);    /* zero-extend*/
									}

								/* 2. check for negative; copy into number */
								if ( (flags & FL_SIGNED) && l < 0) {
									number = -l;
									flags |= FL_NEGATIVE;   /* remember negative sign */
								} 
								else 
									number = l;

								#if _INTEGRAL_MAX_BITS >= 64       
									if ( (flags & FL_I64) == 0 ) {
									/*
									* Unless printing a full 64-bit value, insure values
									* here are not in cananical longword format to prevent
									* the sign extended upper 32-bits from being printed.
										*/
										number &= 0xffffffff;
									}
								#endif  /* _INTEGRAL_MAX_BITS >= 64        */

								/* 3. check precision value for default; non-default */
								/*    turns off 0 flag, according to ANSI. */
								if ( precision < 0 )
									precision = 1;  /* default precision */
								else
									flags &= ~FL_LEADZERO;

								/* 4. Check if data is 0; if so, turn off hex prefix */
								if ( number == 0 )
									prefixlen = 0;
								
								/* 5. Convert data to ASCII -- note if precision is zero */
								/*    and number is zero, we get no digits at all.       */
								
								text.sz = &buffer[BUFFERSIZE-1];    /* last digit at end of buffer */

								while ( precision-- > 0 || number != 0 ) {
									int digit;              /* ascii value of digit */
									#if 1
										digit = (int)(number % radix) + '0';
										number = number / radix;                /* reduce number */
									#endif
									if ( digit > '9' )              /* a hex digit, make it a letter */
										digit += hexadd;
									*text.sz-- = (char)digit;       /* store the digit */
								}
								
								textlen = (char *)&buffer[BUFFERSIZE-1] - text.sz; /* compute length of number */
								++text.sz;          /* text points to first digit now */
								
								/* 6. Force a leading zero if FORCEOCTAL flag set */
								if ((flags & FL_FORCEOCTAL) && (text.sz[0] != '0' || textlen == 0)) {
									*--text.sz = '0';
									++textlen;      /* add a zero */
								}
							}
							break;
					}

					/* At this point, we have done the specific conversion, and */
					/* 'text' points to text to print; 'textlen' is length.  Now we */
					/* justify it, put on prefixes, leading zeros, and then */
					/* print it. */
					
					if (!no_output) {
						textlen_t padding;    /* amount of padding, negative means zero */
						
						if (flags & FL_SIGNED) {
							if (flags & FL_NEGATIVE) { /* prefix is a '-' */
								prefix[0] = _T('-');
								prefixlen = 1;
							}
							else if (flags & FL_SIGN) { /* prefix is '+' */                        
								prefix[0] = _T('+');
								prefixlen = 1;
							}
							else if (flags & FL_SIGNSP) { /* prefix is ' ' */
								prefix[0] = _T(' ');
								prefixlen = 1;
							}
						}
						
						/* calculate amount of padding -- might be negative, */
						/* but this will just mean zero */
						padding = fldwidth - textlen - prefixlen;
						
						/* put out the padding, prefix, and text, in the correct order */
						
						if (!(flags & (FL_LEFT | FL_LEADZERO))) /* pad on left with blanks */
							write_multi_char( _T(' '), &length, padding, &stream, &charsout );
						
						/* write prefix */
						write_string( prefix, &length, prefixlen, &stream, &charsout );
						
						if ((flags & FL_LEADZERO) && !(flags & FL_LEFT)) /* write leading zeros */
							write_multi_char( _T('0'), &length, padding, &stream, &charsout );
						
						/* write text */
						if (bufferiswide && (textlen > 0)) {
							tWCHAR *p = text.wz;
							textlen_t count = textlen;

							if ( CopyTo ) {
								tINT chunk = count;
								while( (count>0) && *p ) {
									tERROR err;
									tDWORD accepted = 0;
									tINT cnt = (count <= chunk) ? count : chunk;
									if ( cnt <= sizeof(buffer) )
										err = CopyTo( buffer, sizeof(buffer), cCP_ANSI, p, (tDWORD)cnt*sizeof(tWCHAR), cCP_UNICODE, 0, &accepted );
									else
										err = errBUFFER_TOO_SMALL;
									if( err == errBUFFER_TOO_SMALL ) {
										cnt = chunk = (sizeof(buffer) / 5);
										err = CopyTo( buffer, sizeof(buffer), cCP_ANSI, p, (tDWORD)cnt*sizeof(tWCHAR), cCP_UNICODE, 0, &accepted );
									}
									if ( PR_FAIL(err) )
										break;

									p += cnt;
									count -= cnt;
									write_string( buffer, &length, accepted, &stream, &charsout );
								}
							}
							else { // we have no possibilities to convert unicode symbols above 255 unfortunately
								int retval;
								char buffer[MB_LEN_MAX+1];
								while (count--) {
									retval = *p++;
									if ( retval > 255 )/* validate high byte */
										*buffer = '?';
									else
										*buffer = (char)retval;
									retval = sizeof(char);
									write_string( buffer, &length, retval, &stream, &charsout );
								}
							}
						} 
						else 
							write_string( text.sz, &length, textlen, &stream, &charsout );

						if (flags & FL_LEFT) /* pad on right with blanks */
							write_multi_char( _T(' '), &length, padding, &stream, &charsout );

						/* we're done! */
					}
					break;
			}
		}
		if ( length )
			*stream = 0;
		if ( end_point && (*end_point != 'z') ) {
			PR_TRACE(( g_root, prtFATAL, "krn\tstring formatting overlaps endpoint" ));
		}
	}
	EXCEPT_ENTER {

		tCHAR local_buff[0x80];
		tBOOL buffer_valid = cTRUE;
		tBOOL can_output = cTRUE;

		TRY_ENTER {
			if ( length ) {
				tCHAR* p = stream--;
				length--;
				*p = 0;
			}
		}
		EXCEPT_ENTER {
			buffer_valid = cFALSE;
		} EXCEPT_LEAVE

		TRY_ENTER {
			if ( !buffer_valid && length )
				*stream = 0;
		}
		EXCEPT_ENTER {
			can_output = cFALSE;
		} EXCEPT_LEAVE

		TRY_ENTER {
			if ( !buffer_valid ) {
				PR_TRACE(( g_root, prtFATAL, "krn\tstring formatting failed: buffer pointer(offs:0x%08x,len:%d) incorrect", start_point, start_length ));
			}
			if ( can_output ) {
				tBOOL add = 0;
				tINT out_len = start_length - length;
				if ( out_len > (sizeof(local_buff)-4) )
					out_len = (sizeof(local_buff)-4);
				mcpy( local_buff, start_point, out_len );
				if ( add )
					mcpy( local_buff+out_len, "...", 4 );
				else
					*(local_buff+out_len) = 0;
				PR_TRACE(( g_root, prtFATAL, "krn\tstring formatting failed: bad parameters, result at present is:\"%s\"", local_buff ));
			}
		}
		EXCEPT_ENTER {
			PR_TRACE(( g_root, prtFATAL, "krn\tstring formatting failed: bad parameters, cannot output result" ));
		} EXCEPT_LEAVE

	} EXCEPT_LEAVE
	return (tUINT)charsout;        /* return value = number of characters written */
}

/*
 *  Future Optimizations for swprintf:
 *  - Don't free the memory used for converting the buffer to wide chars.
 *    Use realloc if the memory is not sufficient.  Free it at the end.
 */

/***
*Purpose:
*   Writes a single character to the given file/console.  If no error occurs,
*   then *pnumwritten is incremented; otherwise, *pnumwritten is set
*   to -1.
*
*Entry:
*   int ch           - character to write
*   FILE *f          - file to write to
*   int *pnumwritten - pointer to integer to update with total chars written
*
*Exit:
*   No return value.
*
*Exceptions:
*
*******************************************************************************/
/*
LOCAL(void) write_char ( int ch, char** out, int *pnumwritten ) {
  *((*out)++) = ch;
  (*pnumwritten)++;
}
*/


/***
*Purpose:
*   Writes num copies of a character to the given file/console.  If no error occurs,
*   then *pnumwritten is incremented by num; otherwise, *pnumwritten is set
*   to -1.  If num is negative, it is treated as zero.
*
*Entry:
*   int ch           - character to write
*   int num          - number of times to write the characters
*   FILE *f          - file to write to
*   int *pnumwritten - pointer to integer to update with total chars written
*
*Exit:
*   No return value.
*
*Exceptions:
*
*******************************************************************************/
LOCAL(void) write_multi_char( int ch, int* size, textlen_t num, char** out, textlen_t* pnumwritten ) {
  while( (*size > 1) && (num > 0) ) {
    --num;
		--(*size);
		*((*out)++) = ch;
    ++(*pnumwritten);
  }
	if ( num > 0 ) {
		if ( *size )
			*size = **out = 0;
    (*pnumwritten) += num;
	}
}


/***
*Purpose:
*   Writes a string of the given length to the given file.  If no error occurs,
*   then *pnumwritten is incremented by len; otherwise, *pnumwritten is set
*   to -1.  If len is negative, it is treated as zero.
*
*Entry:
*   char *string     - string to write (NOT null-terminated)
*   int len          - length of string
*   FILE *f          - file to write to
*   int *pnumwritten - pointer to integer to update with total chars written
*
*Exit:
*   No return value.
*
*Exceptions:
*
*******************************************************************************/
LOCAL(void) write_string( char *string, int* size, textlen_t len, char** out, textlen_t* pnumwritten ) {
  while( (*size > 1) && (len > 0) ) {
    --len;
		--(*size);
		*((*out)++) = *string++;
    ++(*pnumwritten);
  }
	if ( len > 0 ) {
		if ( *size )
			*size = **out = 0;
    (*pnumwritten) += len;
	}
}


/***
*int get_int_arg(va_list_wrapper *pargptr)
*
*Purpose:
*   Gets an int argument off the given argument list and updates *pargptr.
*
*Entry:
*   va_list_wrapper *pargptr - pointer to argument list; updated by function
*
*Exit:
*   Returns the integer argument read from the argument list.
*
*Exceptions:
*
*******************************************************************************/
__inline int __cdecl get_int_arg ( va_list_wrapper *pargptr ) {
  return va_arg(pargptr->argptr, int);
}

/***
*void* get_ptr_arg(va_list_wrapper *pargptr)
*
*Purpose:
*   Gets an int argument off the given argument list and updates *pargptr.
*
*Entry:
*   va_list_wrapper *pargptr - pointer to argument list; updated by function
*
*Exit:
*   Returns the pointer argument read from the argument list.
*
*Exceptions:
*
*******************************************************************************/
__inline void* __cdecl get_ptr_arg ( va_list_wrapper *pargptr ) {
  return va_arg(pargptr->argptr, void*);
}


/***
*long get_long_arg(va_list_wrapper *pargptr)
*
*Purpose:
*   Gets an long argument off the given argument list and updates *pargptr.
*
*Entry:
*   va_list_wrapper *pargptr - pointer to argument list; updated by function
*
*Exit:
*   Returns the long argument read from the argument list.
*
*Exceptions:
*
*******************************************************************************/
#if !LONG_IS_INT
__inline long __cdecl get_long_arg( va_list_wrapper *pargptr ) {
  return va_arg(pargptr->argptr, long);
}
#endif  /* !LONG_IS_INT */

//#if _INTEGRAL_MAX_BITS >= 64   
//__inline __int64 __cdecl get_int64_arg ( va_list_wrapper *pargptr ) {
//  return va_arg(pargptr->argptr, __int64);
__inline tLONGLONG __cdecl get_int64_arg ( va_list_wrapper *pargptr ) {
  return va_arg(pargptr->argptr, tLONGLONG);
}
//#endif  /* _INTEGRAL_MAX_BITS >= 64    */

/***
*short get_short_arg(va_list_wrapper *pargptr)
*
*Purpose:
*   Gets a short argument off the given argument list and updates *pargptr.
*   *** CURRENTLY ONLY USED TO GET A WCHAR_T, IFDEF _INTL ***
*
*Entry:
*   va_list_wrapper *pargptr - pointer to argument list; updated by function
*
*Exit:
*   Returns the short argument read from the argument list.
*
*Exceptions:
*
*******************************************************************************/

#if !SHORT_IS_INT
__inline short __cdecl get_short_arg( va_list_wrapper *pargptr ) {

	#if defined (__unix__)
		return va_arg(pargptr->argptr, int);
	#else
		return va_arg(pargptr->argptr, short);
	#endif

}
#endif  /* !SHORT_IS_INT */



// ---
char* from_hSTRING( hSTRING str, textlen_t* len, char* buffer ) {
	tDWORD tlen = 0;
	tERROR err;
	
	if ( !str ) {
		if ( len )
			*len = __null_size;
		return __null_string;
	}

	err = CALL_String_ExportToBuff( str, &tlen, cSTRING_WHOLE, buffer, BUFFERSIZE, cCP_ANSI, cSTRING_Z );
	if ( PR_FAIL(err) && (err != errBUFFER_TOO_SMALL) ) {
		*len = __bad_size;
		return __bad_string;
	}
	else
		*len = tlen;
	
	if ( len )
		--(*len);
	return buffer;
}



// ---
//char* from_hSTRING_unicode( hSTRING str, int* len, char* buffer ) {
//	tERROR err;
//
//	if ( !str ) {
//		if ( len )
//			*len = __null_size;
//		return __null_string;
//	}
//	
//	err = CALL_String_ExportToBuff( str, len, cSTRING_WHOLE, buffer, BUFFERSIZE, cCP_UNICODE, cSTRING_Z );
//	if ( PR_FAIL(err) && (err != errBUFFER_TOO_SMALL) ) {
//		*len = __bad_size;
//		return __bad_string;
//	}
//	
//	if ( len ) {
//		*len /= sizeof(wchar_t);
//		--(*len);
//	}
//	return buffer;
//}

#if !defined( NO_TIME_OUTPUT_IMPL )

#define  DATETIME_TIME_T_BASE     (0x563aff9bad170000)
#define  DATETIME_FILETIME_BASE   (0x46120ce758a60000)



char* local_time( int time, textlen_t* len, char* buffer ) {
	tDT dt;
	_TQW(dt) = (DATETIME_TIME_T_BASE + ((tQWORD)time * (100000000)));
	return local_dt( &dt, len, buffer );
}

char* gmt_time( int time, textlen_t* len, char* buffer ) {
	tDT dt;
	_TQW(dt) = (DATETIME_TIME_T_BASE + ((tQWORD)time * (100000000)));
	return gmt_dt( &dt, len, buffer );
}


char* local_dt( const tDT* time, textlen_t* len, char* buffer ) {
	return gmt_dt( time, len, buffer );
}


char* gmt_dt( const tDT* time, textlen_t* len, char* buffer ) {
	tDWORD year, month, day, hour, minute, sec, nsec;
	DTGet( time, &year, &month, &day, &hour, &minute, &sec, &nsec );
	*len = pr_sprintf( buffer, BUFFERSIZE, "%02u.%02u.%04u %02u:%02u:%02u.%03u", day, month, year, hour, minute, sec, nsec/1000000 );
	return buffer;
}

#endif

