// AVP Prague stamp begin( Interface header,  )
// -------- ANSI C++ Code Generator 1.0 --------
// -------- Thursday,  18 October 2007,  15:26 --------
// -------------------------------------------
// Copyright © Kaspersky Lab 1996-2006.
// -------------------------------------------
// Project     -- Not defined
// Sub project -- Not defined
// Purpose     -- Not defined
// Author      -- Denisov
// File Name   -- addressbookextractor.cpp
// -------------------------------------------
// AVP Prague stamp end



// AVP Prague stamp begin( Interface version,  )
#define AddressBookExtractor_VERSION ((tVERSION)1)
// AVP Prague stamp end



// AVP Prague stamp begin( Header includes,  )
#include <Prague/prague.h>
#include <Prague/iface/i_root.h>
#include <Prague/pr_vtbl.h>
#include <Mail/plugin/p_as_trainsupport.h>
// AVP Prague stamp end



#include <Mail/iface/i_addressbookextractor.h>
#include <vector>
using std::vector;

#include <memory>
using std::auto_ptr;


#include "OEAddrBook.h"
using AddrBook::COEAddrBook;
using AddrBook::TAddrFolderPtr;
using AddrBook::CAddrFolderBase;
using AddrBook::Strings;

#include "OLAddrBook.h"
using AddrBook::COLAddrBook;

#define MICROSOFT_OUTLOOK			"Microsoft Office Outlook"
#define MICROSOFT_OUTLOOK_EXPRESS	"Microsoft Outlook Express"
#define MICROSOFT_WINDOWS_MAIL		"Microsoft Windows Mail"



// AVP Prague stamp begin( C++ class declaration,  )
struct pr_novtable CAddressBookExtractor : public cAddressBookExtractor {
private:
// Internal function declarations
	tERROR pr_call ObjectInitDone();
	tERROR pr_call ObjectPreClose();

// Property function declarations

public:
// External function declarations
	tERROR pr_call IsEnabled( tBOOL* p_pbEnabled );
	tERROR pr_call GetFolderName( tSIZE_T p_hFolder, hSTRING p_name );
	tERROR pr_call GetChildFolders( tSIZE_T p_hFolder, cSerializable* p_pFolders );
	tERROR pr_call GetFolderContent( tSIZE_T p_hFolder, cSerializable* p_pAddresses );

// Data declaration
// AVP Prague stamp end



private:
	
	struct AddrFolderTreeItem 
	{
		AddrFolderTreeItem(const TAddrFolderPtr& fld):folder(fld), bExpanded(false){}
		TAddrFolderPtr folder;
		bool bExpanded;
		cVector<tSIZE_T> childIndexes;
	};

	typedef vector<AddrFolderTreeItem> FolderTree;

	void AddChildFolders(tSIZE_T index);
	void AddRootItems();

private:

	FolderTree					m_FolderTree;


// AVP Prague stamp begin( C++ class declaration end,  )
public:
	mDECLARE_INITIALIZATION(CAddressBookExtractor)
};
// AVP Prague stamp end



// AVP Prague stamp begin( Global property variable declaration,  )
// Interface "AddressBookExtractor". Static(shared) property table variables
// AVP Prague stamp end



// AVP Prague stamp begin( Internal (kernel called) interface method implementation, ObjectInitDone )
// Extended method comment
//    Notification that all necessary properties have been set and object must go to operation state
// Result comment
//    Returns value differ from errOK if object cannot function properly
// Parameters are:
tERROR CAddressBookExtractor::ObjectInitDone() {
	tERROR error = errNOT_IMPLEMENTED;
	PR_TRACE_FUNC_FRAME( "AddressBookExtractor::ObjectInitDone method" );

	// Place your code here

	AddRootItems();
	error = errOK;

	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Internal (kernel called) interface method implementation, ObjectPreClose )
// Extended method comment
//    Kernel warns object it is going to close all children
// Behaviour comment
//    Object takes all necessary "before preclosing" actions
// Parameters are:
tERROR CAddressBookExtractor::ObjectPreClose() {
	tERROR error = errNOT_IMPLEMENTED;
	PR_TRACE_FUNC_FRAME( "AddressBookExtractor::ObjectPreClose method" );

	m_FolderTree.clear();
	error = errOK;

	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, IsEnabled )
// Parameters are:
tERROR CAddressBookExtractor::IsEnabled( tBOOL* p_pbEnabled ) {
	tERROR error = errNOT_IMPLEMENTED;
	PR_TRACE_FUNC_FRAME( "AddressBookExtractor::IsEnabled method" );

	// Place your code here
	if (!p_pbEnabled)
		return errPARAMETER_INVALID;

	*p_pbEnabled = cTRUE;

	error = errOK;

	return error;
}
// AVP Prague stamp end


// AVP Prague stamp begin( External (user called) interface method implementation, GetFolderName )
// Parameters are:
tERROR CAddressBookExtractor::GetFolderName( tSIZE_T p_hFolder, hSTRING p_name ) {
	tERROR error = errNOT_IMPLEMENTED;
	PR_TRACE_FUNC_FRAME( "AddressBookExtractor::GetFolderName method" );

	// Place your code here
	if (!p_name)
		return errPARAMETER_INVALID;

	if (p_hFolder >= m_FolderTree.size())
		return errBAD_INDEX;
	
	string fldName = m_FolderTree[p_hFolder].folder->GetName();
	p_name->ImportFromBuff(NULL, (tPTR)fldName.c_str(), fldName.length(), cCP_ANSI, cSTRING_Z);
	error = errOK;
	
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, GetChildFolders )
// Parameters are:
tERROR CAddressBookExtractor::GetChildFolders( tSIZE_T p_hFolder, cSerializable* p_pFolders ) {
	tERROR error = errNOT_IMPLEMENTED;
	PR_TRACE_FUNC_FRAME( "AddressBookExtractor::GetChildFolders method" );

	// Place your code here
	if (!p_pFolders)
		return errPARAMETER_INVALID;
	
	if (p_hFolder >= m_FolderTree.size())
		return errBAD_INDEX;

	if (!m_FolderTree[p_hFolder].bExpanded)
	{
		AddChildFolders(p_hFolder);		
		m_FolderTree[p_hFolder].bExpanded= true;
	}

	FolderList_t childFolders;
	childFolders.folders = m_FolderTree[p_hFolder].childIndexes;
	p_pFolders->assign(childFolders, false);

	error = errOK;

	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, GetFolderContent )
// Parameters are:
tERROR CAddressBookExtractor::GetFolderContent( tSIZE_T p_hFolder, cSerializable* p_pAddresses ) {
	tERROR error = errNOT_IMPLEMENTED;
	PR_TRACE_FUNC_FRAME( "AddressBookExtractor::GetFolderContent method" );

	// Place your code here
	if (!p_pAddresses)
		return errPARAMETER_INVALID;

	if (p_hFolder >= m_FolderTree.size())
		return errBAD_INDEX;

	Strings addresses;
	m_FolderTree[p_hFolder].folder->GetContents(addresses);

	AddressList_t addressList;

	Strings::const_iterator it		= addresses.begin();
	Strings::const_iterator it_end	= addresses.end();
	for (; it != it_end; ++it)
	{
		addressList.addresses.push_back(it->c_str());
	}

	p_pAddresses->assign(addressList, false);

	error = errOK;

	return error;
}
// AVP Prague stamp end



void CAddressBookExtractor::AddRootItems()
{
	m_FolderTree.clear();

	TAddrFolderPtr rootFld;
	m_FolderTree.push_back(AddrFolderTreeItem(rootFld));

	
	TAddrFolderPtr folderRefOE(new COEAddrBook((tBYTE)GetVersion() < 6 ? MICROSOFT_OUTLOOK_EXPRESS : MICROSOFT_WINDOWS_MAIL));
	if (folderRefOE->Init())
	{
		m_FolderTree.push_back(AddrFolderTreeItem(folderRefOE));
		m_FolderTree[0].childIndexes.push_back(1); // link with root
	}

	TAddrFolderPtr folderRefOL(new COLAddrBook);
	if (folderRefOL->Init())
	{
		m_FolderTree.push_back(AddrFolderTreeItem(folderRefOL));
		m_FolderTree[0].childIndexes.push_back(2); // link with root
	}

	m_FolderTree[0].bExpanded = true;
}

void CAddressBookExtractor::AddChildFolders(tSIZE_T index)
{
	if (index >= m_FolderTree.size())
		return;
	
	tSIZE_T nextChildIndex = m_FolderTree.size() - 1;
	cVector<tSIZE_T> childIndexes;

	CAddrFolderBase::AddrFolders childFolders;
	if (m_FolderTree[index].folder->GetChildFolders(childFolders))
	{
		CAddrFolderBase::AddrFolders::const_iterator it		= childFolders.begin();
		CAddrFolderBase::AddrFolders::const_iterator it_end = childFolders.end();

		for (; it != it_end; ++it)
		{
			m_FolderTree.push_back(AddrFolderTreeItem(*it));
			childIndexes.push_back(++nextChildIndex);
		}
	}

	m_FolderTree[index].childIndexes = childIndexes;
}


// AVP Prague stamp begin( C++ class regitration,  )
// Interface "AddressBookExtractor". Register function
tERROR CAddressBookExtractor::Register( hROOT root ) {
	tERROR error;
// AVP Prague stamp end



// AVP Prague stamp begin( Internal method table,  )
mINTERNAL_TABLE_BEGIN(AddressBookExtractor)
	mINTERNAL_METHOD(ObjectInitDone)
	mINTERNAL_METHOD(ObjectPreClose)
mINTERNAL_TABLE_END(AddressBookExtractor)
// AVP Prague stamp end



// AVP Prague stamp begin( External method table,  )
mEXTERNAL_TABLE_BEGIN(AddressBookExtractor)
	mEXTERNAL_METHOD(AddressBookExtractor, IsEnabled)
	mEXTERNAL_METHOD(AddressBookExtractor, GetFolderName)
	mEXTERNAL_METHOD(AddressBookExtractor, GetChildFolders)
	mEXTERNAL_METHOD(AddressBookExtractor, GetFolderContent)
mEXTERNAL_TABLE_END(AddressBookExtractor)
// AVP Prague stamp end



// AVP Prague stamp begin( Registration call,  )

	PR_TRACE_FUNC_FRAME_( *root, "AddressBookExtractor::Register method", &error );

	error = CALL_Root_RegisterIFace(
		root,                                   // root object
		IID_ADDRESSBOOKEXTRACTOR,               // interface identifier
		PID_AS_TRAINSUPPORT,                    // plugin identifier
		0x00000000,                             // subtype identifier
		AddressBookExtractor_VERSION,           // interface version
		VID_DENISOV,                            // interface developer
		&i_AddressBookExtractor_vtbl,           // internal(kernel called) function table
		(sizeof(i_AddressBookExtractor_vtbl)/sizeof(tPTR)),// internal function table size
		&e_AddressBookExtractor_vtbl,           // external function table
		(sizeof(e_AddressBookExtractor_vtbl)/sizeof(tPTR)),// external function table size
		NULL,                                   // property table
		0,                                      // property table size
		sizeof(CAddressBookExtractor)-sizeof(cObjImpl),// memory size
		0                                       // interface flags
	);

	#ifdef _PRAGUE_TRACE_
		if ( PR_FAIL(error) )
			PR_TRACE( (root,prtDANGER,"AddressBookExtractor(IID_ADDRESSBOOKEXTRACTOR) registered [%terr]",error) );
	#endif // _PRAGUE_TRACE_
// AVP Prague stamp end



// AVP Prague stamp begin( C++ class regitration end,  )
	return error;
}

tERROR pr_call AddressBookExtractor_Register( hROOT root ) { return CAddressBookExtractor::Register(root); }
// AVP Prague stamp end



// AVP Prague stamp begin( Caution !!!,  )
// You have to implement propetry: pgINTERFACE_VERSION
// You have to implement propetry: pgINTERFACE_COMMENT
// AVP Prague stamp end



