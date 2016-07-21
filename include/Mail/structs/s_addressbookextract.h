/*!
*		
*		
*		(C) 2007 Kaspersky Lab 
*		
*		\file	s_addressbookextract.h
*		\brief	Структуры передачи данных AddressBookExtract
*		
*		\author Nickolay Lozhkarev
*		
*		
*		
*/		



#ifndef _S_AS_ADDRESS_BOOK_EXTRACT_H_
#define _S_AS_ADDRESS_BOOK_EXTRACT_H_

#include <Prague/pr_serializable.h>


#include <Mail/structs/s_antispam.h>

#ifndef PID_AS_TRAINSUPPORT
#define PID_AS_TRAINSUPPORT  ((tPID)(40022))
#endif

enum as_importFromAddrBookId
{
	asiab_AddressList,
	asiab_FolderList,
	asiab_ProfileInfo,
	asiab_ProfileList,
	asiab_LAST
};


struct AddressList_t : public cSerializable 
{
	DECLARE_IID(AddressList_t, cSerializable, PID_AS_TRAINSUPPORT, asiab_AddressList);
	cVector<cStringObj> addresses;	//!< список SMTP адресов
};

IMPLEMENT_SERIALIZABLE_BEGIN(AddressList_t, 0 )
SER_VALUE( addresses,     tid_VECTOR,       "AddressList" )
IMPLEMENT_SERIALIZABLE_END()

struct FolderList_t : public cSerializable 
{
	DECLARE_IID(FolderList_t, cSerializable, PID_AS_TRAINSUPPORT, asiab_FolderList);
	cVector<tSIZE_T> folders;	//!< список идентификаторов адресных каталогов
};

IMPLEMENT_SERIALIZABLE_BEGIN(FolderList_t, 0 )
SER_VALUE( folders,     tid_VECTOR,       "FolderList" )
IMPLEMENT_SERIALIZABLE_END()

struct ProfileInfo_t : public cSerializable 
{
	DECLARE_IID(ProfileInfo_t, cSerializable, PID_AS_TRAINSUPPORT, asiab_ProfileInfo);
	cStringObj	profileName;	//!< имя профиля
	tBOOL		bDefault;		//!< признак профиля по умолчанию
};

IMPLEMENT_SERIALIZABLE_BEGIN(ProfileInfo_t, 0 )
SER_VALUE( profileName,     tid_STRING_OBJ, "ProfileName" )
SER_VALUE( bDefault,		tid_BOOL,       "DefaultValue" )
IMPLEMENT_SERIALIZABLE_END()

struct ProfileList_t : public cSerializable 
{
	DECLARE_IID(ProfileList_t, cSerializable, PID_AS_TRAINSUPPORT, asiab_ProfileList);
	cVector<ProfileInfo_t> profiles;	//!< список профилей
};

IMPLEMENT_SERIALIZABLE_BEGIN(ProfileList_t, 0 )
SER_VALUE( profiles,     tid_VECTOR,       "ProfileList" )
IMPLEMENT_SERIALIZABLE_END()



#endif//_S_AS_ADDRESS_BOOK_EXTRACT_H_
