/*!
*		
*		
*		(C) 2002 Kaspersky Lab 
*		
*		\file	s_as_trainsupport.h
*		\brief	Структуры настроек as_trainsupport
*		
*		\author Vitaly DVi Denisov
*		
*		
*		
*/		



#ifndef _S_AS_TRAINSUPPORT_H_
#define _S_AS_TRAINSUPPORT_H_

#if defined (_WIN32)
#include <mapidefs.h>
#elif defined (__unix__)
typedef struct _SBinary
{
	unsigned long		cb;
	unsigned char* 		lpb;
} SBinary;
#endif

#include <Prague/pr_serializable.h>
#include <Prague/iface/e_ktime.h>
#include <Prague/iface/i_objptr.h>

#include <Mail/structs/s_antispam.h>

#ifndef PID_AS_TRAINSUPPORT
#define PID_AS_TRAINSUPPORT  ((tPID)(40022))
#endif

enum as_trainsupportId
{
	asts_EntryID = asstruct_LAST,
	asts_ObjectInfo,
	asts_FolderInfo,
	asts_TrainFolderInfo,
	asts_TrainStatus,
	asts_TrainSettings,
	asts_LAST
};


typedef cBuff<tBYTE, 256> cBYTEBuff;
// Уникальный идентификатор письма
struct EntryID_t : public cSerializable 
{
	EntryID_t():
	bAllocated(cFALSE),
	cSerializable()
	{
		cb = 0;
	};
	EntryID_t(const EntryID_t& _Entry)
	{
		EntryID_t();
		*this = _Entry;
	}
	virtual ~EntryID_t()
	{
		if ( bAllocated )
		{
			buff.clean(); 
		}
	}
	
	#define SET_OPERATOR_RAVNO(_type_, _lpb) \
	EntryID_t& operator=(const _type_& _Entry)	\
	{	\
		if ( bAllocated )	\
		{	\
			buff.clean();	\
			cb = 0;	\
			bAllocated = false;	\
		}	\
		if ( !bAllocated )	\
		{	\
			tPTR lpb = buff.get(_Entry.cb, false);	\
			if ( lpb )	\
			{	\
				cb = _Entry.cb;	\
				memcpy(lpb, (const void*)(((_type_&)(_Entry))._lpb), cb);		\
				bAllocated = true;	\
			}	\
		}	\
		return *this;	\
	};

	DECLARE_IID(EntryID_t, cSerializable, PID_AS_TRAINSUPPORT, asts_EntryID);

	SET_OPERATOR_RAVNO(EntryID_t, get_lpb());
	SET_OPERATOR_RAVNO(SBinary, lpb);
	tPTR get_lpb(){ return buff.get(cb, false); };

	tULONG cb;
	cBYTEBuff buff;

private:
	tBOOL bAllocated;
};
IMPLEMENT_SERIALIZABLE_BEGIN( EntryID_t, 0 )
	SER_VALUE( cb,   tid_LONG,   "cb" )
	SER_VALUE( buff, tid_BUFFER, "lpb" )
IMPLEMENT_SERIALIZABLE_END()

struct ObjectInfo_t : public cSerializable
{
	ObjectInfo_t():
		szName(""),
		m_objPtr(NULL),
		cSerializable()
		{;};
	DECLARE_IID(ObjectInfo_t, cSerializable, PID_AS_TRAINSUPPORT, asts_ObjectInfo);
	cStringObj szName;	//!< Имя объекта
	EntryID_t EntryID;	//!< Идентификатор объекта
	hObjPtr m_objPtr;	//!< Указатель на объект hObjPtr. !!! Не сериализуется !!!

		inline tERROR SetObjPtr(hObjPtr objPtr)
		{
			if ( m_objPtr )
				m_objPtr->sysCloseObject();
			m_objPtr = NULL;
			if ( objPtr )
			{
				tERROR err = objPtr->Clone(&m_objPtr);
				PR_ASSERT_MSG(PR_SUCC(err), "Failed to clone ObjPtr");
				return err;	
			}
			else
				return errNOT_OK;
		};
		virtual ~ObjectInfo_t()
		{
			if ( m_objPtr )
				m_objPtr->sysCloseObject();
		};
};
IMPLEMENT_SERIALIZABLE_BEGIN( ObjectInfo_t, 0 )
	SER_VALUE( szName,  tid_STRING_OBJ,  "Name" )
	SER_VALUE( EntryID, EntryID_t::eIID, "EntryID" )
IMPLEMENT_SERIALIZABLE_END()

// Информация о папке
struct FolderInfo_t: public ObjectInfo_t
{
	enum FolderType
	{
		ftUndefined = 0,
		ftInBox = 1,
		ftOutBox = 2,
		ftSentItems = 3,
		ftDeletedItems = 4,
		ftDrafts = 5
	};

	FolderInfo_t( ObjectInfo_t* _MDB = NULL, ObjectInfo_t* _Entry = NULL, tBOOL _bHasChildren = cTRUE ):
	bHasChildren(_bHasChildren),
	bCanCreateSubfolders(cFALSE),
	bCanHaveMessages(cFALSE),
	dwMaxTrainMessages(50),
	folderType(ftUndefined),
	ObjectInfo_t()
	{
		if ( _Entry ) 
		{
			szName = _Entry->szName;
			EntryID = _Entry->EntryID;
			SetObjPtr(_Entry->m_objPtr);
		}
		if ( _MDB ) 
		{
			MDB.szName = _MDB->szName;
			MDB.EntryID = _MDB->EntryID;
			MDB.SetObjPtr(_MDB->m_objPtr);
		}
	}
	FolderInfo_t(const FolderInfo_t& _FolderInfo):
	ObjectInfo_t()
	{
		operator=(_FolderInfo);
	}
	FolderInfo_t& operator=(const FolderInfo_t& _FolderInfo)
	{
		szName = _FolderInfo.szName;
		EntryID = _FolderInfo.EntryID;
		SetObjPtr(_FolderInfo.m_objPtr);
		MDB.szName = _FolderInfo.MDB.szName;
		MDB.EntryID = _FolderInfo.MDB.EntryID;
		MDB.SetObjPtr(_FolderInfo.MDB.m_objPtr);
		bHasChildren = _FolderInfo.bHasChildren;
		bCanCreateSubfolders = _FolderInfo.bCanCreateSubfolders;
		bCanHaveMessages = _FolderInfo.bCanHaveMessages;
		dwMaxTrainMessages = _FolderInfo.dwMaxTrainMessages;
		folderType = _FolderInfo.folderType;
		return *this;
	}

	DECLARE_IID(FolderInfo_t, ObjectInfo_t, PID_AS_TRAINSUPPORT, asts_FolderInfo);
	ObjectInfo_t MDB;			//!< Указатель на MDB, содержащей папку
	tBOOL bHasChildren;			//!< Имеет ли данная папка детей?
	tBOOL bCanCreateSubfolders;	//!< Можно ли в данной папке создать подпапку?
	tBOOL bCanHaveMessages;		//!< Могут ли в данной папке находиться почтовые сообщения?
	tDWORD dwMaxTrainMessages;  //!< Максимальное количество сообщений в данной папке для обучения
	FolderType folderType;		//!< Тип каталога

};
IMPLEMENT_SERIALIZABLE_BEGIN( FolderInfo_t, 0 )
	SER_BASE( ObjectInfo_t,  "ObjectInfo" )
	SER_VALUE( MDB,          ObjectInfo_t::eIID, "MDB" )
	SER_VALUE( bHasChildren,        tid_BOOL,    "HasChildren" )
	SER_VALUE( bCanCreateSubfolders,tid_BOOL,    "CanCreateSubfolders" )
	SER_VALUE( bCanHaveMessages,    tid_BOOL,    "CanHaveMessages" )
	SER_VALUE( dwMaxTrainMessages,  tid_DWORD,   "MaxTrainMessages" )
IMPLEMENT_SERIALIZABLE_END()


typedef cVector<FolderInfo_t> FolderInfoList_t;

struct TrainFolderInfo_t : public FolderInfo_t
{
	enum FolderState
	{
		FolderUndefined = 0,
		FolderHAM = 1,
		FolderSPAM = 2
	};
	
	TrainFolderInfo_t() : 
		FolderInfo_t(),
		m_FolderState(FolderUndefined)
	{};

	TrainFolderInfo_t(const FolderInfo_t &FldInfo) :
		FolderInfo_t(),
		m_FolderState(FolderUndefined)
	{
		FolderInfo_t::operator=(FldInfo);
	};

	DECLARE_IID(TrainFolderInfo_t, FolderInfo_t, PID_AS_TRAINSUPPORT, asts_TrainFolderInfo);
	FolderState m_FolderState;
};
IMPLEMENT_SERIALIZABLE_BEGIN( TrainFolderInfo_t, 0 )
	SER_BASE( FolderInfo_t,  "FolderInfo" )
	SER_VALUE( m_FolderState, tid_DWORD,  "FolderState" )
IMPLEMENT_SERIALIZABLE_END()

//! Настройки для первичного обучения
typedef cVector<TrainFolderInfo_t> TrainFolderInfoList_t;


struct TrainSettings_t : public cSerializable
{
	TrainSettings_t() :
		cSerializable()
		{}
	DECLARE_IID(TrainSettings_t, cSerializable, PID_AS_TRAINSUPPORT, asts_TrainSettings);
	TrainFolderInfoList_t m_Folders;
};
IMPLEMENT_SERIALIZABLE_BEGIN( TrainSettings_t, 0 )
	SER_VECTOR( m_Folders,       TrainFolderInfo_t::eIID, "Folders" )
IMPLEMENT_SERIALIZABLE_END()



struct TrainStatus_t : public cSerializable 
{
	TrainStatus_t():
		cSerializable(),
		ulMessagesCount(0),			
		ulMessagesProcessed(0),		
		szCurrentFolder(""),		
		szCurrentMessage(""),
		ulTimeToFinish(0),
		ulPercentCompleted(0),
		bTrainComplete(cFALSE)
		{;};
		
	DECLARE_IID(TrainStatus_t, cSerializable, PID_AS_TRAINSUPPORT, asts_TrainStatus);
	tULONG ulMessagesCount;			//!< Количество сообщений, предназначенных для тренировки
	tULONG ulMessagesProcessed;		//!< Количество обработанных сообщений
	cStringObj szCurrentFolder;		//!< Имя обрабатываемой папки
	cStringObj szCurrentMessage;	//!< Обрабатываемое письмо
	tDWORD ulTimeToFinish;
	tDWORD ulPercentCompleted;
	tBOOL bTrainComplete;
};
IMPLEMENT_SERIALIZABLE_BEGIN( TrainStatus_t, 0 )
	SER_VALUE( ulMessagesCount,     tid_DWORD,       "MessagesCount" )
	SER_VALUE( ulMessagesProcessed, tid_DWORD,       "MessagesProcessed" )
	SER_VALUE( szCurrentFolder,     tid_STRING_OBJ,  "CurrentFolder" )
	SER_VALUE( szCurrentMessage,    tid_STRING_OBJ,  "CurrentMessage" )
	SER_VALUE( ulTimeToFinish,      tid_DWORD,       "TimeToFinish" )
	SER_VALUE( ulPercentCompleted,  tid_DWORD,       "PercentCompleted" )
	SER_VALUE( bTrainComplete,      tid_BOOL,        "TrainComplete" )
IMPLEMENT_SERIALIZABLE_END()

#endif//_S_AS_TRAINSUPPORT_H_
