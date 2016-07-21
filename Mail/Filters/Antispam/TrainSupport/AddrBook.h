#pragma once

#include <boost/shared_ptr.hpp>
using boost::shared_ptr;

#include <string>
using std::string;

#include <vector>
using std::vector;


namespace AddrBook
{
	// базовый класс для каталога адресов
	
	typedef vector<string> Strings;

	class CAddrFolderBase;
	typedef shared_ptr<CAddrFolderBase> TAddrFolderPtr;

	class CAddrFolderBase
	{
	public:
		
		CAddrFolderBase(const char * name): m_name (name ? name : ""){}
		CAddrFolderBase(){}

		typedef vector<TAddrFolderPtr> AddrFolders;

		virtual ~CAddrFolderBase(){}

		virtual bool GetContents(Strings& strings) = 0;
		virtual bool GetChildFolders(AddrFolders& folders) = 0;
		virtual string GetName() const { return m_name; }
		virtual bool Init() { return true; }

	protected:
		void SetName(const string& name) { m_name = name; }
	private:
		string m_name;
	};



}