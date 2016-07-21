/*!
*		
*		
*		(C) 2005 Kaspersky Lab 
*		
*		\file	msiapi.hpp
*		\brief	
*		
*		\author Vladislav Ovcharik
*		\date	25.10.2006 9:55:28
*		
*		
*/		
#if !defined(MSI_API_HPP_INCLUDED)
#define MSI_API_HPP_INCLUDED
#include <string>
#include <vector>
#include <msi.h>
#include <msiquery.h>
#include "scoped_handle.hpp"
///////////////////////////////////////////////////////////////////////////////
extern "C"
{
///////////////////////////////////////////////////////////////////////////////
typedef unsigned int (__stdcall *fn_MsiGetProductInfoA)(char const*		product_code
													, char const*		prop
													, char*				buffer
													, unsigned long*	size);
///////////////////////////////////////////////////////////////////////////////
typedef unsigned int (__stdcall *fn_MsiGetProductInfoW)(wchar_t const*	product_code
													, wchar_t const*	prop
													, wchar_t*			buffer
													, unsigned long*	size);
///////////////////////////////////////////////////////////////////////////////
typedef unsigned int (__stdcall *fn_MsiCloseHandle)(MSIHANDLE);
///////////////////////////////////////////////////////////////////////////////
typedef unsigned int (__stdcall *fn_MsiOpenDatabaseA)(char const* db_name
													  , char const * mode
													  , MSIHANDLE* db_handle);
///////////////////////////////////////////////////////////////////////////////
typedef unsigned int (__stdcall *fn_MsiOpenDatabaseW)(wchar_t const* db_name
													  , wchar_t const * mode
													  , MSIHANDLE* db_handle);
///////////////////////////////////////////////////////////////////////////////
typedef unsigned int (__stdcall *fn_MsiRecordGetStringA)(MSIHANDLE h
														 , unsigned int n
														 , char * buf
														 , unsigned long *size);
///////////////////////////////////////////////////////////////////////////////
typedef unsigned int (__stdcall *fn_MsiRecordGetStringW)(MSIHANDLE h
														 , unsigned int n
														 , wchar_t * buf
														 , unsigned long *size);
///////////////////////////////////////////////////////////////////////////////
typedef unsigned int (__stdcall *fn_MsiRecordReadStream)(MSIHANDLE h
														 , unsigned int n
														 , char * buf
														 , unsigned long *size);
///////////////////////////////////////////////////////////////////////////////
typedef unsigned int (__stdcall *fn_MsiViewExecute)(MSIHANDLE view, MSIHANDLE rec);
///////////////////////////////////////////////////////////////////////////////
typedef unsigned int (__stdcall *fn_MsiViewClose)(MSIHANDLE view);
///////////////////////////////////////////////////////////////////////////////
typedef unsigned int (__stdcall *fn_MsiViewFetch)(MSIHANDLE view, MSIHANDLE *rec);
///////////////////////////////////////////////////////////////////////////////
typedef unsigned int (__stdcall *fn_MsiViewModify)(MSIHANDLE view
												   , MSIMODIFY mode
												   , MSIHANDLE rec);
///////////////////////////////////////////////////////////////////////////////
typedef unsigned int (__stdcall *fn_MsiDatabaseOpenViewA)(MSIHANDLE hdb
														  , char const* query
														  , MSIHANDLE *hvw);
///////////////////////////////////////////////////////////////////////////////
typedef unsigned int (__stdcall *fn_MsiDatabaseOpenViewW)(MSIHANDLE hdb
														  , wchar_t const* query
														  , MSIHANDLE *hvw);
///////////////////////////////////////////////////////////////////////////////
typedef unsigned int (__stdcall *fn_MsiDatabaseCommit)(MSIHANDLE hdb);
///////////////////////////////////////////////////////////////////////////////
typedef unsigned long (__stdcall *fn_MsiRecordDataSize)(MSIHANDLE h, unsigned int n);
///////////////////////////////////////////////////////////////////////////////
}//extern "C"
///////////////////////////////////////////////////////////////////////////////
namespace msi
{
///////////////////////////////////////////////////////////////////////////////
enum db_open_mode
{
	read_only_mode		= 0,
	transact_mode		= 1,
	direct_mode			= 2,
	create_mode			= 3,	//transact mode
	create_direct_mode	= 4		//direct mode	
};
///////////////////////////////////////////////////////////////////////////////
template<typename _E> struct property;
template<> struct property<char>
{
	static char const* local_package() { return "LocalPackage"; }
};
///////////////////////////////////////////////////////////////////////////////
template<> struct property<wchar_t>
{
	static wchar_t const* local_package() { return L"LocalPackage"; }
};
///////////////////////////////////////////////////////////////////////////////
//
inline unsigned int close_handle(MSIHANDLE h)
{
	HMODULE hmsi = GetModuleHandleA("msi");
	if(!hmsi)
		throw std::runtime_error("msi module was not loaded or alreade loaded");
	::fn_MsiCloseHandle fn =
		reinterpret_cast<fn_MsiCloseHandle>(GetProcAddress(hmsi, "MsiCloseHandle"));
	return fn(h);
}
///////////////////////////////////////////////////////////////////////////////
typedef MSIHANDLE	handle_t;
///////////////////////////////////////////////////////////////////////////////
typedef util::scoped_handle
			<
				handle_t,
				unsigned int (*)(handle_t),
				&close_handle,
				0
			> msi_handle_t;
///////////////////////////////////////////////////////////////////////////////
namespace detail
{
///////////////////////////////////////////////////////////////////////////////
//!throw std::exception
inline void check_error(unsigned int error)
{
	switch(error)//work up error code
	{
	case ERROR_BAD_CONFIGURATION:
		throw std::runtime_error("The configuration data is corrupt");
	case ERROR_INVALID_PARAMETER:
		throw std::invalid_argument("An invalid parameter was passed to the function");
	case ERROR_UNKNOWN_PRODUCT:
		throw std::runtime_error("The product is unadvertised or uninstalled");
	case ERROR_UNKNOWN_PROPERTY:
		throw std::runtime_error("The property is unrecognized");
	case ERROR_CREATE_FAILED:
		throw std::runtime_error("can not create database");
	case ERROR_OPEN_FAILED:
		throw std::runtime_error("can not open database");
	case ERROR_FUNCTION_FAILED:
		throw std::runtime_error("A view could not be executed");
	case ERROR_INVALID_HANDLE:
		throw std::runtime_error("An invalid or inactive handle was supplied");
	case ERROR_ACCESS_DENIED:
		throw std::runtime_error("Access was not permitted");
	case ERROR_INVALID_DATA:
		throw std::runtime_error("A validation was requested and the data did not pass");
	case ERROR_INVALID_HANDLE_STATE:
		throw std::runtime_error("The handle is in an invalid state");
	case ERROR_BAD_QUERY_SYNTAX:
		throw std::runtime_error("An invalid SQL query string was passed to the function");
	}
}
///////////////////////////////////////////////////////////////////////////////
inline unsigned int MsiRecordGetString_helper(MSIHANDLE h, unsigned int n
											, char * buf, unsigned long *size)
{
	HMODULE hmsi = GetModuleHandleA("msi");
	if(!hmsi)
		throw std::runtime_error("msi module was not loaded or alreade loaded");
	::fn_MsiRecordGetStringA fn =
		reinterpret_cast<::fn_MsiRecordGetStringA>(GetProcAddress(hmsi, "MsiRecordGetStringA"));
	return fn(h, n, buf, size);
}
///////////////////////////////////////////////////////////////////////////////
inline unsigned int MsiRecordGetString_helper(MSIHANDLE h, unsigned int n
											, wchar_t * buf, unsigned long *size)
{
	HMODULE hmsi = GetModuleHandleA("msi");
	if(!hmsi)
		throw std::runtime_error("msi module was not loaded or alreade loaded");
	::fn_MsiRecordGetStringW fn =
		reinterpret_cast<::fn_MsiRecordGetStringW>(GetProcAddress(hmsi, "MsiRecordGetStringW"));
	return fn(h, n, buf, size);
}
///////////////////////////////////////////////////////////////////////////////
inline unsigned int MsiRecordReadStream_helper(MSIHANDLE h, unsigned int n
											, char * buf, unsigned long *size)
{
	HMODULE hmsi = GetModuleHandleA("msi");
	if(!hmsi)
		throw std::runtime_error("msi module was not loaded or alreade loaded");
	::fn_MsiRecordReadStream fn =
		reinterpret_cast<::fn_MsiRecordReadStream>(GetProcAddress(hmsi, "MsiRecordReadStream"));
	return fn(h, n, buf, size);
}
///////////////////////////////////////////////////////////////////////////////
inline unsigned long MsiRecordDataSize_helper(MSIHANDLE h, unsigned int n)
{
	HMODULE hmsi = GetModuleHandleA("msi");
	if(!hmsi)
		throw std::runtime_error("msi module was not loaded or alreade loaded");
	::fn_MsiRecordDataSize fn =
		reinterpret_cast<::fn_MsiRecordDataSize>(GetProcAddress(hmsi, "MsiRecordDataSize"));
	return fn(h, n);
}
///////////////////////////////////////////////////////////////////////////////
inline unsigned int MsiViewExecute_helper(MSIHANDLE view, MSIHANDLE rec)
{
	HMODULE hmsi = GetModuleHandleA("msi");
	if(!hmsi)
		throw std::runtime_error("msi module was not loaded or alreade loaded");
	::fn_MsiViewExecute fn =
		reinterpret_cast<::fn_MsiViewExecute>(GetProcAddress(hmsi, "MsiViewExecute"));
	return fn(view, rec);
}
///////////////////////////////////////////////////////////////////////////////
inline unsigned int MsiViewFetch_helper(MSIHANDLE view, MSIHANDLE *rec)
{
	HMODULE hmsi = GetModuleHandleA("msi");
	if(!hmsi)
		throw std::runtime_error("msi module was not loaded or alreade loaded");
	::fn_MsiViewFetch fn =
		reinterpret_cast<::fn_MsiViewFetch>(GetProcAddress(hmsi, "MsiViewFetch"));
	return fn(view, rec);
}
///////////////////////////////////////////////////////////////////////////////
inline unsigned int MsiViewModify_helper(MSIHANDLE view, MSIMODIFY mode, MSIHANDLE rec)
{
	HMODULE hmsi = GetModuleHandleA("msi");
	if(!hmsi)
		throw std::runtime_error("msi module was not loaded or alreade loaded");
	::fn_MsiViewModify fn =
		reinterpret_cast<::fn_MsiViewModify>(GetProcAddress(hmsi, "MsiViewModify"));
	return fn(view, mode, rec);
}
///////////////////////////////////////////////////////////////////////////////
inline unsigned int MsiViewClose_helper(MSIHANDLE hvw)
{
	HMODULE hmsi = GetModuleHandleA("msi");
	if(!hmsi)
		throw std::runtime_error("msi module was not loaded or alreade loaded");
	::fn_MsiViewClose fn =
		reinterpret_cast<::fn_MsiViewClose>(GetProcAddress(hmsi, "MsiViewClose"));
	return fn(hvw);
}
///////////////////////////////////////////////////////////////////////////////
inline unsigned int MsiDatabaseOpenView_helper(MSIHANDLE hdb, char const* query, MSIHANDLE *hvw)
{
	HMODULE hmsi = GetModuleHandleA("msi");
	if(!hmsi)
		throw std::runtime_error("msi module was not loaded or alreade loaded");
	::fn_MsiDatabaseOpenViewA fn =
		reinterpret_cast<::fn_MsiDatabaseOpenViewA>(GetProcAddress(hmsi, "MsiDatabaseOpenViewA"));
	return fn(hdb, query, hvw);
}
///////////////////////////////////////////////////////////////////////////////
inline unsigned int MsiDatabaseOpenView_helper(MSIHANDLE hdb, wchar_t const* query, MSIHANDLE *hvw)
{
	HMODULE hmsi = GetModuleHandleA("msi");
	if(!hmsi)
		throw std::runtime_error("msi module was not loaded or alreade loaded");
	::fn_MsiDatabaseOpenViewW fn =
		reinterpret_cast<::fn_MsiDatabaseOpenViewW>(GetProcAddress(hmsi, "MsiDatabaseOpenViewW"));
	return fn(hdb, query, hvw);
}
///////////////////////////////////////////////////////////////////////////////
inline unsigned int MsiGetProductInfo_helper(char const*		product_code
											, char const*		prop
											, char*			buffer
											, unsigned long*	size)
{
	HMODULE hmsi = GetModuleHandleA("msi");
	if(!hmsi)
		throw std::runtime_error("msi module was not loaded or alreade loaded");
	::fn_MsiGetProductInfoA fn =
		reinterpret_cast<::fn_MsiGetProductInfoA>(GetProcAddress(hmsi, "MsiGetProductInfoA"));
	return fn(product_code, prop, buffer, size);
}
///////////////////////////////////////////////////////////////////////////////
inline unsigned int MsiGetProductInfo_helper(wchar_t const*		product_code
											, wchar_t const*	prop
											, wchar_t*		buffer
											, unsigned long*	size)
{
	HMODULE hmsi = GetModuleHandleA("msi");
	if(!hmsi)
		throw std::runtime_error("msi module was not loaded or alreade loaded");
	::fn_MsiGetProductInfoW fn =
		reinterpret_cast<::fn_MsiGetProductInfoW>(GetProcAddress(hmsi, "MsiGetProductInfoW"));
	return fn(product_code, prop, buffer, size);
}
///////////////////////////////////////////////////////////////////////////////
//!open existence database
inline unsigned int MsiOpenDatabase_helper(char const* db_name, db_open_mode mode, handle_t* db_handle)
{
	HMODULE hmsi = GetModuleHandleA("msi");
	if(!hmsi)
		throw std::runtime_error("msi module was not loaded or alreade loaded");
	::fn_MsiOpenDatabaseA fn =
		reinterpret_cast<::fn_MsiOpenDatabaseA>(GetProcAddress(hmsi, "MsiOpenDatabaseA"));
	return fn(db_name, reinterpret_cast<char const*>(mode), db_handle);
}
///////////////////////////////////////////////////////////////////////////////
inline unsigned int MsiDatabaseCommit_helper(MSIHANDLE hdb)
{
	HMODULE hmsi = GetModuleHandleA("msi");
	if(!hmsi)
		throw std::runtime_error("msi module was not loaded or alreade loaded");
	::fn_MsiDatabaseCommit fn =
		reinterpret_cast<::fn_MsiDatabaseCommit>(GetProcAddress(hmsi, "MsiDatabaseCommit"));
	return fn(hdb);
}
///////////////////////////////////////////////////////////////////////////////
inline unsigned int MsiOpenDatabase_helper(wchar_t const* db_name, db_open_mode mode, handle_t* db_handle)
{
	HMODULE hmsi = GetModuleHandleA("msi");
	if(!hmsi)
		throw std::runtime_error("msi module was not loaded or alreade loaded");
	::fn_MsiOpenDatabaseW fn =
		reinterpret_cast<::fn_MsiOpenDatabaseW>(GetProcAddress(hmsi, "MsiOpenDatabaseW"));
	return fn(db_name, reinterpret_cast<wchar_t const*>(mode), db_handle);
}
///////////////////////////////////////////////////////////////////////////////
}//namespace detail
///////////////////////////////////////////////////////////////////////////////
//!return path to MSI database
template<typename _E>
inline std::basic_string<_E> database_path(_E const* product_code)
{
	std::vector<_E> path(MAX_PATH + 1, _E('\x0'));
	unsigned long size(MAX_PATH);
	unsigned long error(detail::MsiGetProductInfo_helper(product_code
							, property<_E>::local_package()
							, &path[0]
							, &size));
	if(ERROR_MORE_DATA == error)
	{
		path.resize(size + 1);
		error = detail::MsiGetProductInfo_helper(product_code, property<_E>::local_package()
										, &path[0], &size);

	}
	detail::check_error(error);//Workup error code
	return std::string(&path[0], &path[size]);
}
//////////////////////////////////////////////////////////////////////////////
template<typename _E>
inline std::basic_string<_E> database_path(std::basic_string<_E> const& product_code)
{
	return database_path(product_code.c_str());
}
///////////////////////////////////////////////////////////////////////////////
template<typename _E>
inline handle_t open_database(_E const* db_name, db_open_mode mode)
{
	handle_t db_handle(0);
	detail::check_error(detail::MsiOpenDatabase_helper(db_name, mode, &db_handle));
	return db_handle;
}
///////////////////////////////////////////////////////////////////////////////
template<typename _E>
inline handle_t open_database(std::basic_string<_E> const& db_name, db_open_mode mode)
{
	return open_database(db_name.c_str(), mode);
}
///////////////////////////////////////////////////////////////////////////////
}//namespace msi
///////////////////////////////////////////////////////////////////////////////
#endif// !defined(MSI_API_HPP_INCLUDED)