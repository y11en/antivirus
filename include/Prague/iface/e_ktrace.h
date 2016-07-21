// kernel trace functions

#define IMPEX_CURRENT_HEADER  "e_ktrace.h"
#include <Prague/iface/impexhlp.h>

#if !defined(__e_ktrace_h) || defined(IMPEX_TABLE_CONTENT)
#if ( IMPEX_INTERNAL_MODE == IMPEX_INTERNAL_MODE_DECLARE )
#define __e_ktrace_h

  // Extrnal includes should be placed here. For ex:
  // #include <windows.h>
  #include <stdarg.h>

typedef tINT  (*tHandleEnumFunc)( tVOID* handle, tVOID* params );
typedef tVOID (*tCleanFunc)( tVOID* );


#define pr_trace                                     pr_trace
#define pr_vsprintf(output, length, format, argptr)  pr_vsprintf(output, length, format, argptr)
#define pr_sprintf 	                                 pr_sprintf

#if defined(__cplusplus) 

	typedef tBOOL (pr_call *ftpr_trace_v) ( tVOID* obj, tTRACE_LEVEL level, tSTRING format, va_list argptr );

	template< unsigned int importer_id = 2 >
	class cfpr_trace_v : public cFuncPtrBase<ftpr_trace_v,ECLSID_KERNEL,0x5ec5256cl,importer_id> {
	public:
		tUINT operator() ( tVOID* obj, tTRACE_LEVEL level, tSTRING format, va_list argptr ) {
			if ( PR_SUCC(this->load()) )
				return this->fptr( obj, level, format, argptr );
			return 0;
		}
	};
	extern cfpr_trace_v<2> fppr_trace_v;

	template< unsigned int importer_id = 2 >
	class cfpr_trace : public cFuncPtrBase<ftpr_trace_v,ECLSID_KERNEL,0x5ec5256cl,importer_id> {
	public:
		tUINT operator() ( tVOID* obj, tTRACE_LEVEL level, tSTRING format, ... ) {
			tUINT    ret = 0;
			va_list  argptr;
			va_start( argptr, format );
			if ( PR_SUCC(this->load()) )
				ret = this->fptr( obj, level, format, argptr );
			va_end( argptr );
			return 0;
		}
	};
	extern cfpr_trace<2> fppr_trace;

	typedef tBOOL (pr_call *ftpr_handle_enum) ( tHandleEnumFunc func, tVOID* params );
	template< unsigned int importer_id = 2 >
	class cfpr_handle_enum : public cFuncPtrBase<ftpr_handle_enum,ECLSID_KERNEL,0x3a37bb5bl,importer_id> {
	public:
		tUINT operator() ( tHandleEnumFunc func, tVOID* params ) {
			if ( PR_SUCC(this->load()) )
				return this->fptr( func, params );
			return 0;
		}
	};
	extern cfpr_handle_enum<2> fppr_handle_enum;

	typedef tBOOL (pr_call *ftpr_sizeometer) ( tDWORD type_id, tCleanFunc* func );
	template< unsigned int importer_id = 2 >
	class cfpr_sizeometer : public cFuncPtrBase<ftpr_sizeometer,ECLSID_KERNEL,0xe8e0394cl,importer_id> {
	public:
		tUINT operator() ( tDWORD type_id, tCleanFunc* func ) {
			if ( PR_SUCC(this->load()) )
				return this->fptr( type_id, func );
			return 0;
		}
	};
	extern cfpr_sizeometer<2> fppr_sizeometer;

#if !defined( cfpr_sprintf_defined )
#define cfpr_sprintf_defined
	typedef tUINT (pr_call *ftpr_vsprintf)( tCHAR* output, tINT length, const tCHAR* format, va_list argptr );

	template< unsigned int importer_id = 2 >
	class cfpr_vsprintf : public cFuncPtrBase<ftpr_vsprintf,ECLSID_KERNEL,0x1e98f2eel,importer_id> {
	public:
		tUINT operator() ( tCHAR* output, tINT length, const tCHAR* format, va_list arglist ) {
			if ( PR_SUCC(this->load()) )
				return this->fptr( output, length, format, arglist );
			return 0;
		}
	};
	extern cfpr_vsprintf<2> fppr_vsprintf;

	template< unsigned int importer_id = 2 >
	class cfpr_sprintf : public cFuncPtrBase<ftpr_vsprintf,ECLSID_KERNEL,0x1e98f2eel,importer_id> {
	public:
		tUINT operator() ( tCHAR* output, tINT length, const tCHAR* format, ... ) {
			tUINT    ret = 0;
			va_list  argptr;
			va_start( argptr, format );
			if ( PR_SUCC(this->load()) )
				ret = this->fptr( output, length, format, argptr );
			va_end( argptr );
			return ret;
		}
	};
	extern cfpr_sprintf<2> fppr_sprintf;
#endif

#endif // __cplusplus

#endif
	
	IMPEX_BEGIN
	
	IMPEX_NAME_ID( ECLSID_KERNEL, 0x1e7c5fffl, tBOOL, pr_trace,       (tVOID* obj, tTRACE_LEVEL level, tSTRING format,...) )
	IMPEX_NAME_ID( ECLSID_KERNEL, 0x1e98f2eel, tUINT, pr_vsprintf,    (tCHAR* output, tINT length, const tCHAR* format, va_list argptr) )
  IMPEX_NAME_ID( ECLSID_KERNEL, 0xf9c5b533l, tUINT, pr_sprintf,     (tCHAR* output, tINT length, const tCHAR* format, ... ) )
  IMPEX_NAME_ID( ECLSID_KERNEL, 0x3a37bb5bl, tUINT, pr_handle_enum, ( tHandleEnumFunc func, tVOID* params ) )
  IMPEX_NAME_ID( ECLSID_KERNEL, 0xe8e0394cl, tUINT, pr_sizeometer,  ( tDWORD type_id, tCleanFunc* func ) )
	IMPEX_NAME_ID( ECLSID_KERNEL, 0x5ec5256cl, tBOOL, pr_trace_v,     (tVOID* obj, tTRACE_LEVEL level, tSTRING format, va_list argptr) )
  
	IMPEX_END
	
#endif
