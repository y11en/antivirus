#if !defined(KL_OEANTISPAM_H__BB3E97E4_0F3E_48F1_BFDF_AFE78FB6A870__INCLUDED_)
#define KL_OEANTISPAM_H__BB3E97E4_0F3E_48F1_BFDF_AFE78FB6A870__INCLUDED_

/*
 *	This is a global hook DLL for OutlookExpress
 */

#ifdef OEANTISPAM_EXPORTS
	#define OEANTISPAM_API __declspec(dllexport)
#else
	#define OEANTISPAM_API __declspec(dllimport)
#endif

STDAPI DllRegisterServer();
STDAPI DllUnregisterServer();

OEANTISPAM_API HHOOK InstallHook();
OEANTISPAM_API void UninstallHook();

typedef HHOOK (*InstallHook_t)();
typedef void (*UninstallHook_t)();

#endif	// KL_OEANTISPAM_H__BB3E97E4_0F3E_48F1_BFDF_AFE78FB6A870__INCLUDED_
