#include <Prague/iface/e_clsid.h>

#undef  IMPEX_FUNC
#undef  IMPEX_DECL
#undef  IMPEX_INIT
#undef  IMPEX_DEF_INT

#if defined(PR_IMPEX_DEF) 
#  define IMPEX_DEF_INT
#else
#  define IMPEX_DEF_INT   extern
#endif

#if defined(IMPEX_EXPORT_LIB)
#  define IMPEX_FUNC(name) pr_call name
#	 ifdef __cplusplus
#    define IMPEX_DECL extern "C"
#  else
#    define IMPEX_DECL
#  endif
#  define IMPEX_INIT 
#else
#  define IMPEX_FUNC(name) (pr_call *name)
#  if defined(__cplusplus)
#    define IMPEX_DECL extern "C" { IMPEX_DEF_INT
#    define IMPEX_INIT ;}
#  else
#    define IMPEX_DECL IMPEX_DEF_INT
#    define IMPEX_INIT
#  endif
#endif

#ifdef IMPEX_CURRENT_HEADER

#	ifndef IMPEX_INTERNAL_MODE_DEFINE
#	  define IMPEX_INTERNAL_MODE_DEFINE  1
#	  define IMPEX_INTERNAL_MODE_DECLARE 2
#	endif

#	undef  IMPEX_BEGIN
#	undef  IMPEX_NAME
#	undef  IMPEX_NAME_ID
#	undef  IMPEX_END

#	ifdef  IMPEX_TABLE_CONTENT
#		define  IMPEX_BEGIN
#		define  IMPEX_NAME(   clsid,    ret_type,name,params)  {{ ((tDATA)&(name)), (tDATA)clsid, (tDATA)FID_##name }},
#		define  IMPEX_NAME_ID(clsid,fid,ret_type,name,params)  {{ ((tDATA)&(name)), (tDATA)(clsid), (tDATA)FID_##name }},
#		define  IMPEX_END
#		undef   IMPEX_CURRENT_HEADER

#	elif !defined(IMPEX_INTERNAL_MODE)
#		define  IMPEX_INTERNAL_MODE IMPEX_INTERNAL_MODE_DEFINE
#		include IMPEX_CURRENT_HEADER
#		undef   IMPEX_INTERNAL_MODE

#		define  IMPEX_INTERNAL_MODE IMPEX_INTERNAL_MODE_DECLARE
#		include IMPEX_CURRENT_HEADER
#		undef   IMPEX_INTERNAL_MODE

#		undef   IMPEX_BEGIN
#		undef   IMPEX_NAME
#		undef   IMPEX_NAME_ID
#		undef   IMPEX_END
#		define  IMPEX_BEGIN
#		define  IMPEX_NAME(clsid,ret_type,name,params)
#		define  IMPEX_NAME_ID(clsid,fid,ret_type,name,params)
#		define  IMPEX_END

#		undef   IMPEX_CURRENT_HEADER
#		undef   IMPEX_EXPORT_LIB

#	elif (IMPEX_INTERNAL_MODE == IMPEX_INTERNAL_MODE_DEFINE)
#		define  IMPEX_BEGIN                                    enum{ 
#		define  IMPEX_NAME(   clsid,    ret_type,name,params)   FID_##name,
#		define  IMPEX_NAME_ID(clsid,fid,ret_type,name,params)   FID_##name = (fid),
#		define  IMPEX_END                                      };
#	elif (IMPEX_INTERNAL_MODE == IMPEX_INTERNAL_MODE_DECLARE)
#		define  IMPEX_BEGIN
#		define  IMPEX_NAME(   clsid,    ret_type,name,params)  IMPEX_DECL ret_type IMPEX_FUNC(name) params IMPEX_INIT;
#		define  IMPEX_NAME_ID(clsid,fid,ret_type,name,params)  IMPEX_DECL ret_type IMPEX_FUNC(name) params IMPEX_INIT;
#		define  IMPEX_END
#	endif
#endif

