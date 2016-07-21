#pragma once
#include "AddrBook.h"
//#include <mapix.h>

namespace AddrBook
{
	class COLAddrBookInitializer
	{
	public:
		COLAddrBookInitializer();
		~COLAddrBookInitializer();

		bool Inited() const { return m_bInited; }
		void SetAddrBook(LPADRBOOK pAddrBook); 

	private:
		bool		m_bInited;
		LPADRBOOK	m_pAddrBook;
	};

	typedef shared_ptr<COLAddrBookInitializer> TOLInitializer;

	class COLFolderBase : public CAddrFolderBase
	{
	public:
		COLFolderBase(const char * name): CAddrFolderBase(name), m_pABCont(NULL) {}
		COLFolderBase(const char * name, const TOLInitializer& initRef, LPABCONT pABCont);

		virtual ~COLFolderBase();

		virtual bool GetContents(Strings& strings) { strings.clear(); return true; }
		virtual bool GetChildFolders(AddrFolders& folders);

	protected:

		virtual bool ChildRequest() { return true; }
		virtual void ExtractChildFolders();

	protected:
		TOLInitializer	m_initRef;
		LPABCONT		m_pABCont;
		CAddrFolderBase::AddrFolders m_childs;
	};

	// обычный адресный каталог
	class COLAddrFolder : public COLFolderBase
	{
	public:
		COLAddrFolder(const char * name, const TOLInitializer& initRef, LPABCONT pABCont);
		
		virtual bool GetContents(Strings& strings);

	protected:		
		virtual bool ChildRequest();
		void ExtractContents();
	private:
		bool	m_bIsChildExtracted;
		bool	m_bIsContentExtracted;
		Strings m_contents;
	};


	// корневой каталог для OE
	class COLAddrBook : public COLFolderBase
	{
		typedef std::pair<std::string, bool> ProfileString;
		typedef vector<ProfileString> Profiles;

	public:

		COLAddrBook() : COLFolderBase("Microsoft OutLook"), m_bIsChildExtracted(false) {}

		virtual bool Init();

	protected:
		virtual bool ChildRequest();
		virtual void ExtractChildFolders();
		void GetProfiles(Profiles& profiles);
		bool SessionLogOn(ULONG hWnd, LPCTSTR profile, LPCTSTR pswd, ULONG flags);

	protected:
		bool	m_bIsChildExtracted;
	};
}