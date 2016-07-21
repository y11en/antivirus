#pragma once


#include <vector>
#include <string>

#include <wab.h>
#include "AddrBook.h"

namespace AddrBook
{

	class COEWabHandler
	{
	public:
		COEWabHandler();
		virtual ~COEWabHandler();

		bool Inited() const { return m_pAddrBook != NULL; }
		bool OpenAddrBook(LPADRBOOK * ppAddrBook, LPWABOBJECT * ppWABObj);

	private:

		LPWABOPEN	m_fWabOpen;
		HMODULE		m_hWABLib;
		LPADRBOOK	m_pAddrBook;	// первичная пустая книга
		LPWABOBJECT	m_pWABObj;
	};

	typedef shared_ptr<COEWabHandler> TOEWabHandler;

	class COEAddrBookInitializer
	{
	public:
		COEAddrBookInitializer(const TOEWabHandler& handler);

		~COEAddrBookInitializer();

		bool Inited() const { return (m_parentRef != NULL); }
		bool GetABContByName(const char * name, LPABCONT * ppABCont);
		void Free(void * ptr);

	private:

		// ссылка на первую адресную книгу,
		// должна освобождаться самой последней!
		TOEWabHandler m_parentRef;
		LPADRBOOK	m_pAddrBook;
		LPWABOBJECT	m_pWABObj;
	};

	typedef shared_ptr<COEAddrBookInitializer> TOEWabInitializer;

	class COEFolderBase : public CAddrFolderBase
	{
	public:
		COEFolderBase(const char * name): CAddrFolderBase(name), m_pABCont(NULL) {}
		COEFolderBase(const char * name, const TOEWabInitializer& initRef):
		CAddrFolderBase(name), m_initRef(initRef), m_pABCont(NULL)
		{}


		virtual ~COEFolderBase();

		virtual bool GetContents(Strings& strings) { strings.clear(); return true; }
		virtual bool GetChildFolders(AddrFolders& folders);

	private:
		COEFolderBase(const COEFolderBase& rhs){}
		const COEFolderBase& operator = (const COEFolderBase& rhs) { return *this; }
	protected:

		virtual bool ChildRequest() { return true; }
		void ExtractChildFolders();

	protected:
		TOEWabInitializer m_initRef;
		CAddrFolderBase::AddrFolders m_childs;
		LPABCONT m_pABCont;
	};

	// обычный адресный каталог
	class COEAddrFolder : public COEFolderBase
	{
	public:
		COEAddrFolder(const char * name, LPABCONT pABCont, const TOEWabInitializer& initRef);
		virtual ~COEAddrFolder();

		virtual bool GetContents(Strings& strings);

	protected:		
		virtual bool ChildRequest();
		void ExtractContents();
	private:
		bool	m_bIsChildExtracted;
		bool	m_bIsContentExtracted;
		Strings m_contents;
	};

	// каталог identity
	class COEIdentityFolder : public COEFolderBase
	{
	public:
		COEIdentityFolder(const char * name, const char * identityId, const TOEWabInitializer& initRef);

	protected:
		virtual bool ChildRequest();

	private:
		string		m_id;
	};

	// корневой каталог для OE
	class COEAddrBook : public COEFolderBase
	{
	public:

		COEAddrBook() : COEFolderBase("Microsoft OutLook Express"), m_bInited(false) {}
		COEAddrBook(const char * name) : COEFolderBase(name), m_bInited(false) {}

		virtual bool Init();

	protected:
		virtual bool CreateIdentitiesList();
	protected:
		TOEWabHandler	m_WABhandler;
		bool			m_bInited;
	};


}



