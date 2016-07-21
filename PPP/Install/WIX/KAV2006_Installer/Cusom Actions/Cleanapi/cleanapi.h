///////////////////////////////////////////////////////////////////////////////
#if !defined(__CLEANAPI_H_7BEB01DA_EA42_4916_9D55_41549A60A6A9__)
#	define __CLEANAPI_H_7BEB01DA_EA42_4916_9D55_41549A60A6A9__
///////////////////////////////////////////////////////////////////////////////
#if !defined(CLEAN_API)
#	define CLEAN_API __declspec(dllimport)
#endif//EXPORT
///////////////////////////////////////////////////////////////////////////////
//если нужно прервать обработку нужно вернуть 0
typedef int (__stdcall *fn_callback_enumerator)(char const* software_name
												, void* user_data);
///////////////////////////////////////////////////////////////////////////////
//return 1 if all OK else 0
CLEAN_API int __stdcall enum_competitor_software(char const* ini_path
												, void* data
												, fn_callback_enumerator fn);
///////////////////////////////////////////////////////////////////////////////
//return 1 if all OK else 0
CLEAN_API int __stdcall remove_competitor_software(char const* ini_path
												, void* data
												, fn_callback_enumerator fn);
///////////////////////////////////////////////////////////////////////////////
//return 1 if all OK else 0
//[out] software_names - is a list of software names were detected. names separated by ;
CLEAN_API int __stdcall list_competitor_software(char const* ini_path
												 , char* software_names
												 , size_t size);
///////////////////////////////////////////////////////////////////////////////
//returns names from parameter fullname of main section
//return 1 if all OK else 0
//[out] software_names - is a list of software names were detected. names separated by ;
CLEAN_API int __stdcall list_competitor_software_unique(char const* ini_path
												 , char* software_names
												 , size_t size);
//return 1 if all OK else 0
//[in] software_names is zero termoated string with software names that is separeted by ;
CLEAN_API int __stdcall remove_all_competitor_software(char const* ini_path);
///////////////////////////////////////////////////////////////////////////////
#endif//__CLEANAPI_H_7BEB01DA_EA42_4916_9D55_41549A60A6A9__
///////////////////////////////////////////////////////////////////////////////
