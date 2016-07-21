#pragma once

#include <ObjBase.h>
#include <comdef.h>

#include <exception>
using std::exception;

#include <MAPIX.h>

#include <string>
using std::string;

#include <vector>
using std::vector;

#include <boost/shared_ptr.hpp>
using boost::shared_ptr;


namespace MAPIAccess
{
	_COM_SMARTPTR_TYPEDEF(IAddrBook, IID_IAddrBook);
	_COM_SMARTPTR_TYPEDEF(IABContainer, IID_IABContainer);

	class COutLookAddrBookImpl;
	class CAddrFolderImpl;
	typedef shared_ptr<COutLookAddrBookImpl> TAddrBookRef;

	typedef vector<string> Strings;

	class CMAPIException : public exception
	{
	public:
		CMAPIException(const char * msg) : exception(msg)
		{}
	};

	class CMAPIInitializer
	{
	public:
		CMAPIInitializer();
		~CMAPIInitializer();
	};

	typedef shared_ptr<CMAPIInitializer> TMAPILibRef;
	class COutLookAddrBook;

	class CAddrFolder
	{
		friend COutLookAddrBook;
		typedef shared_ptr<CAddrFolderImpl> TFolderImplPtr;

		CAddrFolder(IABContainerPtr& pContainer, const string& name, TMAPILibRef mapiRef, const IAddrBookPtr& pAddrBook):
		m_pContainer(pContainer), m_name(name), m_mapiRef(mapiRef), m_pAddrBook(pAddrBook)
		{}

	public:

		CAddrFolder(){}

		typedef vector<CAddrFolder> AddrFolders;

		bool GetContents(Strings& strings) const;
		bool GetChildFolders(AddrFolders& folders) const;
		string GetName() const { return m_name; }

	private:
		string			m_name;

		TMAPILibRef		m_mapiRef;	// MAPI reference have to be first in declaration
		IAddrBookPtr	m_pAddrBook; // reference to address book
		IABContainerPtr	m_pContainer;		
	};

	class COutLookAddrBook
	{
	public:

		typedef std::pair<std::string, bool> ProfileString;
		typedef vector<ProfileString> Profiles;

		COutLookAddrBook();
		virtual ~COutLookAddrBook();

		bool GetProfilesCount(unsigned int& profileCount);
		bool GetProfiles(Profiles& profiles);

		bool SessionLogOn(
			ULONG hWnd = 0,
			LPCTSTR profile = "",
			LPCTSTR pswd = NULL,
			FLAGS flags = MAPI_NO_MAIL | MAPI_EXTENDED | MAPI_NEW_SESSION | MAPI_USE_DEFAULT
		);

		bool GetRootFolder(CAddrFolder& folderHandler);

	public:

		static bool TryMAPIInitialize();

	private:
		TMAPILibRef		m_mapiRef; // MAPI reference have to be first in declaration
		IAddrBookPtr	m_pAddrBook;
	};


}

