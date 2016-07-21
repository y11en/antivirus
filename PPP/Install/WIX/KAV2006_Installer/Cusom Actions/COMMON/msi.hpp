/*!
*		
*		
*		(C) 2005 Kaspersky Lab 
*		
*		\file	msi.hpp
*		\brief	
*		
*		\author Vladislav Ovcharik
*		\date	25.10.2006 9:54:47
*		
*		
*/		
#if !defined(MSI_HPP_INCLUDED)
#define MSI_HPP_INCLUDED
#include "msiapi.hpp"
#include <boost/utility.hpp>
///////////////////////////////////////////////////////////////////////////////
namespace msi
{
///////////////////////////////////////////////////////////////////////////////
class record
		: private boost::noncopyable
{
	typedef msi_handle_t record_handle_t;
public:
	explicit record(MSIHANDLE h)
		: record_(h)
	{
	}
	std::string get_string(int field) const
	{
		unsigned long size(MAX_PATH);
		detail::MsiRecordGetString_helper(record_.get(), 1, (char*)0, &size);
		std::vector<char> buffer(++size);
		detail::MsiRecordGetString_helper(record_.get(), 1, &buffer[0], &size);
		return std::string(&buffer[0]);
	}
	bool get_stream(int field, char *buffer, unsigned long& size)
	{
		bool result = detail::MsiRecordReadStream_helper(record_.get(), field, buffer, &size) == ERROR_SUCCESS;
		return result && (size != 0);
	}
private:
	record_handle_t	record_;
};
///////////////////////////////////////////////////////////////////////////////
/**
 *	Describes view object for a database
 */
class view
	: private boost::noncopyable
{
	typedef msi_handle_t record_handle_t;
	typedef msi_handle_t view_handle_t;
public:
	explicit view(handle_t handle)
		: handle_(handle)
	{
		detail::check_error(detail::MsiViewExecute_helper(handle_.get(), 0));
	}
	~view()
	{
	}
	void delete_record(std::string const& key)
	{
		record_handle_t r;
		char buffer[MAX_PATH] = { 0 };
		while(ERROR_SUCCESS == detail::MsiViewFetch_helper(handle_.get(), &r.get_ref()))
		{
			unsigned long size(MAX_PATH);
			detail::MsiRecordGetString_helper(r.get(), 1, buffer, &size);
			buffer[size] = '\x0';
			if(strcmp(buffer, key.c_str()) == 0)
			{
				detail::check_error(detail::MsiViewModify_helper(handle_.get(), MSIMODIFY_DELETE, r.get()));
			}
			record_handle_t().swap(r);
		}
	}
	std::auto_ptr<record> get_record(std::string const& key)
	{
		std::vector<char> buffer(64);
		record_handle_t r;
		while(ERROR_SUCCESS == detail::MsiViewFetch_helper(handle_.get(), &r.get_ref()))
		{
			unsigned long size(MAX_PATH);
			detail::MsiRecordGetString_helper(r.get(), 1, (char*)0, &size);
			buffer.resize(++size);
			detail::MsiRecordGetString_helper(r.get(), 1, &buffer[0], &size);
			if(_stricmp(&buffer[0], key.c_str()) == 0)
				return std::auto_ptr<record>(new record(r.release()));
			record_handle_t().swap(r);
		}
		return std::auto_ptr<record>();
	}
private:
	view_handle_t	handle_;
};
///////////////////////////////////////////////////////////////////////////////
class database
	: private boost::noncopyable
{
	typedef msi_handle_t database_handle_t;
public:
	template<typename _E>
	database(std::basic_string<_E> const& db_name, db_open_mode mode)
		: handle_(open_database(db_name, mode))
		, mode_(mode)
	{
	}
	explicit database(MSIHANDLE h)
		: handle_(h)
		, mode_(read_only_mode)
	{
	}
	~database()
	{
		if(mode_ != read_only_mode)
			detail::MsiDatabaseCommit_helper(handle_.get());
	}
	std::auto_ptr<view> open_view(char const* table_name)
	{
		handle_t h;
		std::string sql("select * from ");
		sql.append(table_name);
		detail::check_error(detail::MsiDatabaseOpenView_helper(handle_.get(), sql.c_str(), &h));
		return std::auto_ptr<view>(new view(h));
	}
private:
	bool				owner_;
	db_open_mode		mode_;
	database_handle_t	handle_;
};
///////////////////////////////////////////////////////////////////////////////
}//namesapce msi
///////////////////////////////////////////////////////////////////////////////
#endif// !defined(MSI_HPP_INCLUDED)