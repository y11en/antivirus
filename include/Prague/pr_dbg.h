#if !defined( __pr_dbg_h )
#define __pr_dbg_h

///////////////////////////////////////////////////////////////////////////
// Assertion macros
///////////////////////////////////////////////////////////////////////////

/*
*	Usage
*/
// PR_ASSERT(a > 0);
// PR_VERIFY( PR_SUCC(obj->do()) );
// PR_VERIFY_SUCC( obj->do() );
// PR_ASSERT_MSG(a > 0, "Bad thing happened!");
// PR_RPT(("a = %i", a));

#if defined(_DEBUG)

#	include <assert.h>
#	include <crtdbg.h>
#	include <stdio.h>
#	include <stdarg.h>

#	include <Prague/pr_err.h>

#	if defined(ITS_LOADER)
#		define CHECK_L(name)      (1)
#		define CHECK_K(name)      (name)
#	elif defined(ITS_KERNEL)
#		define CHECK_L(name)      (name)
#		define CHECK_K(name)      (1)
#	else
#		define CHECK_L(name)      (name)
#		define CHECK_K(name)      (name)
#	endif

#if defined(__cplusplus)
#	define PR_DBG_REP fpPrDbgReport
#	define PR_PRINTF  fppr_vsprintf
#else
#	define PR_DBG_REP PrDbgReport_v
#	define PR_PRINTF  pr_vsprintf
#endif

#if defined(_MSC_VER) && defined(_M_IX86)
# undef  _CrtDbgBreak
# define _CrtDbgBreak()  Int3()
  __inline void Int3() { __asm { int 3 } }
#endif

#	define PR_VERIFY(expr)      PR_ASSERT(expr)
#	define PR_VERIFY_SUCC(expr) PR_VERIFY(PR_SUCC(expr))

#if defined(PR_ASSERT_SYMB)

#	define PR_ASSERT(expr)                                                                                  \
	do {                                                                                                    \
		if ( !(expr) && CHECK_L(PR_DBG_REP) && (1 == PR_DBG_REP(_CRT_ASSERT,__FILE__,__LINE__,NULL,#expr))) { \
			_CrtDbgBreak();                                                                                     \
		}                                                                                                     \
	} while (0)

#	define PR_ASSERT_MSG( expr, msg )                                                                       \
	(void)(	                                                                                                \
		(!!(expr)) ||                                                                                         \
		(!CHECK_L(PR_DBG_REP) || (1 != PR_DBG_REP(_CRT_ASSERT, __FILE__, __LINE__, NULL, "%s", msg)) ) ||     \
		(_CrtDbgBreak(), 0)                                                                                   \
	)

#else

#	define PR_ASSERT(expr)                                                                                  \
	do {                                                                                                    \
		if ( !(expr) && (1 == _CrtDbgReport(_CRT_ASSERT,__FILE__,__LINE__,NULL,#expr))) {                     \
			_CrtDbgBreak();                                                                                     \
		}                                                                                                     \
	} while (0)

#	define PR_ASSERT_MSG(expr, msg)                                                                         \
	(void)(	                                                                                                \
		(!!(expr)) ||                                                                                         \
		(1 != _CrtDbgReport(_CRT_ASSERT, __FILE__, __LINE__, NULL, "%s", msg)) ||                             \
		(_CrtDbgBreak(), 0)                                                                                   \
	)

#endif

#if defined(PR_ASSERT_SYMB)
#if defined(__cplusplus) 

#if !defined( cFuncPtrBase_declared )
#	define cFuncPtrBase_declared
	template< typename tfPTR, unsigned int cls_id, unsigned int func_id, unsigned int importer_id >
	class cFuncPtrBase {
	protected:
		tfPTR fptr;
	public:
		cFuncPtrBase() : fptr(0) {}
		tERROR load();
		operator bool() { return PR_SUCC(load()); }
	};
#endif

	typedef tINT  (pr_call *ftPrDbgReport)( tINT nRptType, const tCHAR* szFile, tINT nLine, const tCHAR* szModule, const tCHAR* szFormat, va_list arglist );

	template< unsigned int importer_id = 2 >
	class cfPrDbgReport : public cFuncPtrBase<ftPrDbgReport,3,0x8d487b78l,importer_id> {
	public:
		tINT operator() ( tINT nRptType, const tCHAR* szFile, tINT nLine, const tCHAR* szModule, const tCHAR* szFormat, ... ) {
			if ( PR_SUCC(load()) ) {
				va_list argptr;
				va_start( argptr, szFormat );
				tINT retval = fptr( nRptType, szFile, nLine, szModule, szFormat, argptr );
				va_end( argptr );
				return retval;
			}
			return 0;
		}
	};
	extern cfPrDbgReport<2> fpPrDbgReport;

#if !defined( cfpr_sprintf_defined )
#define cfpr_sprintf_defined
	
	typedef tUINT (pr_call *ftpr_vsprintf)( tCHAR* output, tINT length, const tCHAR* format, va_list argptr );

	template< unsigned int importer_id = 2 >
	class cfpr_vsprintf : public cFuncPtrBase<ftpr_vsprintf,1,0x1e98f2eel,importer_id> {
	public:
		tUINT operator() ( tCHAR* output, tINT length, const tCHAR* format, va_list arglist ) {
			if ( PR_SUCC(load()) )
				return fptr( output, length, format, arglist );
			return 0;
		}
	};
	extern cfpr_vsprintf<2> fppr_vsprintf;

	template< unsigned int importer_id = 2 >
	class cfpr_sprintf : public cFuncPtrBase<ftpr_vsprintf,1,0x1e98f2eel,importer_id> {
	public:
		tUINT operator() ( tCHAR* output, tINT length, const tCHAR* format, ... ) {
			tUINT    ret = 0;
			va_list  argptr;
			va_start( argptr, format );
			if ( PR_SUCC(load()) )
				ret = fptr( output, length, format, argptr );
			va_end( argptr );
			return ret;
		}
	};
	extern cfpr_sprintf<2> fppr_sprintf;
#endif

#else // !__cplusplus
#	if defined(ITS_LOADER)
#		define IMPEX_FUNC_L(name) pr_call name
#		define IMPEX_FUNC_K(name) (pr_call *name)
#		ifdef __cplusplus
#			define IMPEX_DECL_L  extern "C"
#			define IMPEX_INIT_L
#			define IMPEX_DECL_K  extern "C" { extern
#			define IMPEX_INIT_K  ;}
#		else
#			define IMPEX_DECL_L
#			define IMPEX_INIT_L
#			define IMPEX_DECL_K extern
#			define IMPEX_INIT_K
#		endif
#	elif defined(ITS_KERNEL)
#		define IMPEX_FUNC_L(name) (pr_call *name)
#		define IMPEX_FUNC_K(name) pr_call name
#		ifdef __cplusplus
#			define IMPEX_DECL_L  extern "C" { extern
#			define IMPEX_INIT_L  ;}
#			define IMPEX_DECL_K  extern "C"
#			define IMPEX_INIT_K
#		else
#			define IMPEX_DECL_L extern
#			define IMPEX_INIT_L
#			define IMPEX_DECL_K
#			define IMPEX_INIT_K
#		endif
#	else
#		define IMPEX_FUNC_L(name) (pr_call *name)
#		define IMPEX_FUNC_K(name) (pr_call *name)
#		ifdef __cplusplus
#			define IMPEX_DECL_L  extern "C" { extern
#			define IMPEX_INIT_L  ;}
#			define IMPEX_DECL_K  extern "C" { extern
#			define IMPEX_INIT_K	;}
#		else
#			define IMPEX_DECL_L extern
#			define IMPEX_INIT_L
#			define IMPEX_DECL_K extern
#			define IMPEX_INIT_K
#		endif
#	endif

	IMPEX_DECL_K tUINT IMPEX_FUNC_K(pr_vsprintf)( tCHAR* output, tINT length, const tCHAR* format, va_list argptr ) IMPEX_INIT_K;
	IMPEX_DECL_L tINT IMPEX_FUNC_L(PrDbgReport_v)( tINT nRptType, const tCHAR* szFile, tINT nLine, const tCHAR* szModule, const tCHAR* szFormat, va_list argptr ) IMPEX_INIT_L;
#endif
#endif

__inline int PrDbgReportMsg( const char *msg, ... ) {
	int result = 0;
	char buf[1024];
	va_list argptr;
	va_start( argptr, msg );
#	if defined(PR_ASSERT_SYMB)
	if ( CHECK_K(PR_PRINTF) && CHECK_L(PR_DBG_REP) ) {
		PR_PRINTF( buf, sizeof(buf), msg, argptr );
		result = PR_DBG_REP( _CRT_WARN, 0, 0, 0, buf, 0 );
	}
#	else
#		if defined(_MSC_VER) && (_MSC_VER >= 1400)
			_vsnprintf_s( buf, sizeof(buf), _TRUNCATE, msg, argptr );
#		elif defined(_MSC_VER)
			_vsnprintf( buf, sizeof(buf) - 1, msg, argptr );
#		else
			vsnprintf( buf, sizeof(buf) - 1, msg, argptr );
#		endif
		result = _CrtDbgReport( _CRT_WARN, 0, 0, 0, "%s", buf );
#	endif
	va_end( argptr );
	return result;
}

#	define PR_RPT(params) (void)((1 != PrDbgReportMsg params) || (_CrtDbgBreak(), 0))

#else // defined(_DEBUG)

#	define PR_ASSERT(expr) ((void)0)
#	define PR_VERIFY(expr) ((void)(expr))
#	define PR_VERIFY_SUCC(expr) (PR_SUCC(expr))
#	define PR_ASSERT_MSG(expr, msg) ((void)0)
#	define PR_RPT(params) ((void)0)

#endif // defined(_DEBUG)

#if defined(__cplusplus) && defined(cFuncPtrBase_declared) && !defined(cFuncPtrBase_implemented)
#	include <iface/i_root.h>
#if !defined(cFuncPtrBase_implemented)
#define cFuncPtrBase_implemented
template< typename tfPTR, unsigned int cls_id, unsigned int func_id, unsigned int importer_id = 2 >
inline tERROR cFuncPtrBase<tfPTR,cls_id,func_id,importer_id>::load() {
	if ( fptr )
		return errOK;
	if ( !::g_root )
		return errUNEXPECTED;
	return ::g_root->ResolveImportFunc( (tFUNC_PTR*)&fptr, cls_id, func_id, importer_id );
}
#endif

#endif // defined(__cplusplus) && defined(cFuncPtrBase_d_defined)


///////////////////////////////////////////////////////////////////////////
// Compile-time report macros
///////////////////////////////////////////////////////////////////////////

#define	__pr_dbg_makestr(x)   #x
#define __pr_dbg_makestr2(x)  __pr_dbg_makestr(x)

/*
*	Usage
*/
// #pragma PR_BUILD_MSG("Hi Mom")
// #pragma PR_TODO("Finish this Off")
#define PR_BUILD_MSG(desc)    message(__FILE__ "(" __pr_dbg_makestr2(__LINE__) "): " desc)
#define PR_TODO(desc)         message(__FILE__ "(" __pr_dbg_makestr2(__LINE__) "): TODO: " desc)

#endif // __pr_dbg_h

