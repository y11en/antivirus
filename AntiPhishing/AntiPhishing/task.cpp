// AVP Prague stamp begin( Interface header,  )
// -------- ANSI C++ Code Generator 1.0 --------
// -------- Wednesday,  04 April 2007,  19:43 --------
// -------------------------------------------
// Copyright © Kaspersky Lab 1996-2006.
// -------------------------------------------
// Project     -- Kaspersky Anti-Virus
// Sub project -- Not defined
// Purpose     -- Not defined
// Author      -- Pavlushchik
// File Name   -- task.cpp
// -------------------------------------------
// AVP Prague stamp end



// AVP Prague stamp begin( Interface version,  )
#define Task_VERSION ((tVERSION)1)
// AVP Prague stamp end



// AVP Prague stamp begin( Header includes,  )
#include <Prague/prague.h>
#include <Prague/pr_vtbl.h>
#include <Prague/pr_remote.h>
#include <Prague/plugin/p_hash_md5.h>
#include <Prague/pr_time.h>
#include <Prague/iface/i_csect.h>
#include <Prague/iface/i_io.h>
#include <Prague/iface/i_root.h>
#include <Prague/iface/i_reg.h>
#include <Prague/iface/e_loader.h>
#include <Prague/iface/i_mutex.h>
#include <Version/ver_product.h>

#include <AntiPhishing/plugin/p_antiphishing.h>

#include <AV/structs/s_avs.h>

#include <ProductCore/environment.h>
#include <Updater/updater.h>

#include <stdlib.h> 
#include <minmax.h>

#include "dbheader.h"
#include "pattern.h"
#include "lstcache.h"
// AVP Prague stamp end

typedef struct tag_HASH_INDEX {
	tQWORD* pData;
	tDWORD  nCount;
} tHASH_INDEX;

#define LCASE(c) ( c >= 'A' ? ( c <= 'Z' ? c | 0x20 : c ) : c )

// AVP Prague stamp begin( C++ class declaration,  )
struct pr_novtable AntiPhishing : public cTask 
{
private:
// Internal function declarations
	tERROR pr_call ObjectInit();
	tERROR pr_call ObjectInitDone();
	tERROR pr_call ObjectPreClose();
	tERROR pr_call MsgReceive( tDWORD p_msg_cls_id, tDWORD p_msg_id, hOBJECT p_send_point, hOBJECT p_ctx, hOBJECT p_receive_point, tVOID* p_par_buf, tDWORD* p_par_buf_len );

// Property function declarations

public:
// External function declarations
	tERROR pr_call SetSettings( const cSerializable* p_settings );
	tERROR pr_call GetSettings( cSerializable* p_settings );
	tERROR pr_call AskAction( tActionId p_actionId, cSerializable* p_actionInfo );
	tERROR pr_call SetState( tTaskRequestState p_state );
	tERROR pr_call GetStatistics( cSerializable* p_statistics );

// Data declaration
	hTASKMANAGER m_hTM;          // --
	tTaskState   m_nTaskState;   // --
	tDWORD       m_nTaskSessionId; // --
	tDWORD       m_nTaskId;      // --
	tDWORD       m_nTaskParentId; // --
// AVP Prague stamp end



	hCRITICAL_SECTION m_hCS;

	tBOOL       m_bClosed;

	tDWORD      m_nFilterMsgClass;
	tDWORD		m_nUrlFilteringMode;
	cStringObj	m_sDatabaseUpdCat;
	tBOOL       m_bAskAction;

	cAntiPhishingStatistics m_Statistics;
	tANTIPHISHING_DB m_Header;
	tBYTE*  m_pDBData;
	tWCHAR** m_pWildCards;
	tHASH_INDEX m_Index[256];
	cStringObj m_sDatabase;
	enActions  m_nApply2AllAction;
	cListCache m_WhiteListCache;
	cListCache m_BlackListCache;
	cListCache m_UserBlackListCache;
	hHASH      m_hHashMD5;
	hOBJECT    m_hHTTP;
	cVector<hMUTEX> m_vMutexes;

	struct tag_LastAnswer {
		time_t  time;
		cStrObj	sUrl;
		tERROR  nErrResult;
	} m_LastAnswer;
	
    // lockLoadBasesMutex [in] - indicates whether bases mutex shold be locked,
     // Note: it should not be locked if pm_COMPONENT_PREINSTALLED message received, because updater locks the mutex itself
	tERROR Reload(const bool lockLoadBasesMutex);
	tERROR ReloadHashesDatabase();
	tERROR FreeDatabase();
	tERROR UrlCanonicalizeInplace(tWCHAR* pszUrl);
	tERROR SyncWithTextFile(cStrObj& sName, cListCache& cache, cVector<cEnabledStrItem>& list);
	tERROR CreateMutex(hMUTEX* phMutex, const tWCHAR* pName, tBOOL bGlobal);

// AVP Prague stamp begin( C++ class declaration end,  )
public:
	mDECLARE_INITIALIZATION(AntiPhishing)
};
// AVP Prague stamp end



int __cdecl compare_qword( const void *arg1, const void *arg2 )
{
	tQWORD qw1, qw2;
	qw1 = *(tQWORD*)arg1;
	qw2 = *(tQWORD*)arg2;
	if (qw1 < qw2)
		return -1;
	if (qw1 > qw2)
		return 1;
	return 0;
}

void CalcUnique(iObj* _this, tQWORD* pUnique, const cStrObj& name)
{
	hHASH hHash;
	*pUnique = 0;
	if (PR_SUCC(_this->sysCreateObjectQuick((hOBJECT*)&hHash, IID_HASH, PID_HASH_MD5)))
	{
		hHash->Update((tBYTE*) (const wchar_t*)name.c_str(cCP_UNICODE), name.length() * 2);

		DWORD dwHash[4];
		if (PR_SUCC(hHash->GetHash((tBYTE*)&dwHash, sizeof(dwHash))))
		{
			dwHash[0] ^= dwHash[2];
			dwHash[1] ^= dwHash[3];
			memcpy(pUnique, &dwHash, sizeof(tQWORD));
		}

		hHash->sysCloseObject();
	}
}


// AVP Prague stamp begin( Global property variable declaration,  )
// Interface "Task". Static(shared) property table variables
// AVP Prague stamp end



// AVP Prague stamp begin( Internal (kernel called) interface method implementation, ObjectInit )
// Extended method comment
//    Kernel notifies an object about successful creating of object
// Behaviour comment
//    Initializes internal object data
// Result comment
//    Returns value differ from errOK if object cannot be initialized
// Parameters are:
tERROR AntiPhishing::ObjectInit()
{
	tERROR error = errOK;
	PR_TRACE_FUNC_FRAME( "Task::ObjectInit method" );

	// Place your code here

	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Internal (kernel called) interface method implementation, ObjectInitDone )
// Extended method comment
//    Notification that all necessary properties have been set and object must go to operation state
// Result comment
//    Returns value differ from errOK if object cannot function properly
// Parameters are:
tERROR AntiPhishing::ObjectInitDone()
{
	tERROR error = errOK;
	PR_TRACE_FUNC_FRAME( "Task::ObjectInitDone method" );

	error = sysCreateObjectQuick((hOBJECT*)&m_hCS, IID_CRITICAL_SECTION);

	m_WhiteListCache.Init(*this);
	m_BlackListCache.Init(*this);
	m_UserBlackListCache.Init(*this);


	return error;
}
// AVP Prague stamp end



tERROR AntiPhishing::FreeDatabase()
{
	cAutoCS _lock(m_hCS, cTRUE);
	if (m_pDBData)
		heapFree(m_pDBData);
	if (m_pWildCards)
	{
		unsigned long i;
		for (i=0; i<m_Header.wildcards_count; i++)
			heapFree(m_pWildCards[i]);
		heapFree(m_pWildCards);
	}
	m_pDBData = NULL;
	m_pWildCards = NULL;
	return errOK;
}

// AVP Prague stamp begin( Internal (kernel called) interface method implementation, ObjectPreClose )
// Extended method comment
//    Kernel warns object it is going to close all children
// Behaviour comment
//    Object takes all necessary "before preclosing" actions
// Parameters are:
tERROR AntiPhishing::ObjectPreClose()
{
	tERROR error = errOK;
	PR_TRACE_FUNC_FRAME( "Task::ObjectPreClose method" );
	m_bClosed = cTRUE;
	cAutoCS _lock(m_hCS, cTRUE);
	if (m_hHTTP)
		m_hTM->ReleaseService(TASKID_TM_ITSELF, m_hHTTP);
	FreeDatabase();
	m_WhiteListCache.Free();
	m_BlackListCache.Free();
	m_UserBlackListCache.Free();
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Internal (kernel called) interface method implementation, MsgReceive )
// Extended method comment
//    Receives message sent to the object or to the one of the object parents as broadcast
// Parameters are:
//   "msg_cls_id"    : Message class identifier
//   "msg_id"        : Message identifier
//   "send_point"    : Object where the message was send initially
//   "ctx"           : Context of the object processing
//   "receive_point" : Point of distributing message (tree top for RegisterMsgHandler call
//   "par_buf"       : Buffer of the parameters
//   "par_buf_len"   : Lenght of the buffer of the parameters
tERROR AntiPhishing::MsgReceive( tDWORD p_msg_cls_id, tDWORD p_msg_id, hOBJECT p_send_point, hOBJECT p_ctx, hOBJECT p_receive_point, tVOID* p_par_buf, tDWORD* p_par_buf_len )
{
	tERROR error = errOK;
	tDWORD nFilterMsgClass = 0;
	tDWORD nUrlFilteringMode = 0;
	PR_TRACE_FUNC_FRAME( "Task::MsgReceive method" );

	if (m_bClosed)
		return errOK;

	{
		cAutoCS _lock(m_hCS, cTRUE); // protect local data, they can be changed by SetSettings
		if (m_bClosed)
			return errOK;
		
		nFilterMsgClass = m_nFilterMsgClass;
		nUrlFilteringMode = m_nUrlFilteringMode;

		if (
			p_msg_cls_id == pmc_TASK_STATE_CHANGED &&
			m_nTaskState == TASK_STATE_RUNNING &&
			m_hHTTP &&
			p_send_point->propGetPID() == m_hHTTP->propGetPID()
			)
		{
			m_nTaskState = (tTaskState)p_msg_id;
			sysSendMsg(pmc_TASK_STATE_CHANGED, m_nTaskState, NULL, NULL, NULL);
		}
	}

	if (p_msg_cls_id == nFilterMsgClass && (p_msg_id == msg_URL_FILTER_CHECK_URL || p_msg_id == msg_URL_FILTER_BLOCK_IF_NOT_WHITELISTED))
	{
		if (m_nTaskState != TASK_STATE_RUNNING)
			return errOK;

		if (p_par_buf_len != SER_SENDMSG_PSIZE)
			return errPARAMETER_INVALID;
		error = AskAction(p_msg_id, (cSerializable*)p_par_buf);
		if (error == errOK)
			error = errOK_DECIDED;
		PR_TRACE((this, prtNOT_IMPORTANT, "urlf\tMsgReceive(msg_URL_FILTERING_CHECK_URL): AskAction result %terr", error ));
		return error;
	}

	if(p_msg_cls_id == pmc_UPDATER && p_msg_id == pm_COMPONENT_PREINSTALLED)
	{
		cSerializable *pSer = p_par_buf_len == SER_SENDMSG_PSIZE ? (cSerializable *)p_par_buf : NULL;
		cUpdaterUpdatedFileList *updatedFiles = pSer && pSer->getIID() == cUpdaterUpdatedFileList::eIID ?
			reinterpret_cast<cUpdaterUpdatedFileList *>(pSer) : NULL;
		if(!updatedFiles)
			return errPARAMETER_INVALID;
		
		cStrObj _sDatabaseUpdCat;
		{
			cAutoCS _lock(m_hCS, false);
			_sDatabaseUpdCat = m_sDatabaseUpdCat;
		}
		if(updatedFiles->m_selectedComponentIdentidier == _sDatabaseUpdCat)
		{
			cAutoCS _lock(m_hCS, true); // protect local data, they can be changed by SetSettings

			PR_TRACE((this, prtIMPORTANT, "urlf\tReloading databases..."));
			// возврат ошибки означает откат обновления!
			error = Reload(false);
			PR_TRACE((this, prtIMPORTANT, "urlf\tDatabase reloaded (%terr)", error));
		}
	}

	return error;
}
// AVP Prague stamp end



tERROR AntiPhishing::Reload(const bool lockLoadBasesMutex)
{
	tERROR error = errOK;

	cAntiPhishingSettings* pSettings = NULL;
	if (PR_FAIL(m_hTM->LockTaskSettings(*this, (cSerializable**) &pSettings, cFALSE)))
	{
		PR_TRACE((this, prtERROR, "urlf\tGet settings from bl failed"));
		return errUNEXPECTED;
	}
	
	tBOOL bSettingsChanged = cFALSE;

	do 
	{
		cAutoCS _lock(m_hCS, cTRUE);
		
		cBasesLoadLocker LoadLocker((cObject *)this, m_sDatabaseUpdCat.c_str(cCP_ANSI),
            lockLoadBasesMutex ? cBasesLoadLocker::withLock : cBasesLoadLocker::noLock);

		if (m_nUrlFilteringMode & UFM_USE_WHITELIST)
		{
			error = m_WhiteListCache.Build(pSettings->m_vWhiteList);
			if (PR_FAIL(error))
			{
				PR_TRACE((this, prtERROR, "urlf\tSetSettings: whitelist cache build error (%terr)", error ));
				break;
			}
		}
		
		if (m_nUrlFilteringMode & UFM_USE_DATABASE)
			ReloadHashesDatabase();
		
		if (m_nUrlFilteringMode & UFM_USE_BLACKLIST)
		{
			cStrObj sTextBlDB = pSettings->m_sTextBlDB;
			sysSendMsg(pmc_PRODUCT_ENVIRONMENT, pm_EXPAND_ENVIRONMENT_STRING, (hOBJECT)cAutoString (sTextBlDB), 0, 0);
			PR_TRACE((this, prtNOTIFY, "urlf\tSetSettings: loading %S...", sTextBlDB.data() ));
			
			m_BlackListCache.Build(pSettings->m_vBlackList);
			
			error = SyncWithTextFile(sTextBlDB, m_BlackListCache, pSettings->m_vBlackList);
			PR_TRACE((this, PR_FAIL(error) ? prtERROR : prtNOTIFY, "urlf\tSetSettings: loading %S completed with %terr", sTextBlDB.data(), error ));
			if (PR_FAIL(error))
			{
				cAskObjectAction action;
				action.m_nNonCuredReason = NCREASON_REPONLY;
				action.m_nDetectDanger = DETDANGER_HIGH;
				action.m_nDetectType = DETYPE_UNDETECT;
				action.m_nDescription = error;
				action.m_strObjectName = sTextBlDB;
				action.m_nObjectStatus = OBJSTATUS_CORRUPTED;
				action.m_tmTimeStamp = cDateTime();
				sysSendMsg(pmc_EVENTS_CRITICAL, 0, NULL, &action, SER_SENDMSG_PSIZE);
			}
			
			// warnALREADY - nothing changed
			// errOK - synchronized successfuly
			
			if (error == errOK)
				bSettingsChanged = cTRUE;
			
			if (PR_SUCC(error))
				error = m_UserBlackListCache.Build(pSettings->m_vUserBlackList);
			if (PR_FAIL(error))
			{
				PR_TRACE((this, prtERROR, "urlf\tSetSettings: user blacklist cache build error (%terr)", error ));
				break;
			}
			
	#ifdef _PRAGUE_TRACE_
			PR_TRACE((this, prtNOT_IMPORTANT, "urlf\tBlackList cache:"));
			m_BlackListCache.Dump();
			PR_TRACE((this, prtNOTIFY, "urlf\tUserBlackList cache:"));
			m_UserBlackListCache.Dump();
			PR_TRACE((this, prtNOTIFY, "urlf\tWhiteList cache:"));
			m_WhiteListCache.Dump();
	#endif
		} 
	} while (false); // for "break"s
	
	m_hTM->UnlockTaskSettings(*this, pSettings, bSettingsChanged);
	
	return error;
}


tERROR AntiPhishing::ReloadHashesDatabase() 
{
	tERROR error = errOK;
	hIO io = NULL;
	cAutoCS _lock(m_hCS, cTRUE);
//	if (!SignCheck)
//		error = errOBJECT_NOT_INITIALIZED;
	if (PR_SUCC(error))
		error = sysCreateObject((hOBJECT*)&io, IID_IO, PID_NATIVE_FIO);
	if (PR_SUCC(error))
	{
		cAutoObj<cIO> _close_io(io);
		error = io->set_pgOBJECT_OPEN_MODE(fOMODE_OPEN_IF_EXIST);
		if (PR_SUCC(error))
			error = m_sDatabase.copy(io, pgOBJECT_FULL_NAME);
		if (PR_SUCC(error))
			error = io->sysCreateObjectDone();
//		if (PR_SUCC(error))
//			error = SignCheck((hOBJECT)io, cTRUE, NULL, 0, cTRUE, NULL);
		if (PR_SUCC(error))
		{
			tDWORD dwBytes, dwRead;
			unsigned long* pheader = (unsigned long*)&m_Header;
			unsigned long header_key = 0x56D3932A;
			tDWORD i;

			FreeDatabase();

			dwBytes = sizeof(m_Header);
			error = io->SeekRead(&dwRead, 0, &m_Header, dwBytes);
			if (dwBytes != dwRead)
				error = errOBJECT_DATA_CORRUPTED;
			
			// decrypt header
			for (i=1; i<sizeof(m_Header)/sizeof(unsigned long);i++)
			{
				pheader[i] ^= header_key;
				header_key += header_key;
			}
			
			if (PR_SUCC(error) && (0 != memcmp(m_Header.signature, ANTIPHISHING_DB_SIGNATURE, 4)))
			{
				PR_TRACE((this, prtERROR, "urlf\tReloadDatabase: ANTIPHISHING_DB_SIGNATURE not matched"));
				error = errOBJECT_DATA_CORRUPTED;
			}

			if (PR_SUCC(error))
			{
				m_Statistics.m_nURLsInDatabase = m_Header.hashes_count;
				error = heapAlloc((tPTR*)&m_pDBData, m_Header.total_db_size);
				if (PR_SUCC(error))
				{
					error = io->SeekRead(&dwRead, 0, m_pDBData, m_Header.total_db_size);
					if (m_Header.total_db_size != dwRead)
						error = errOBJECT_DATA_CORRUPTED;
					if (PR_SUCC(error))
					{
						tDWORD dwLastPos = 0;
						tDWORD dwPos;
						tQWORD* pHashes = (tQWORD*)(m_pDBData + m_Header.hashes_offset);
						qsort(pHashes, m_Header.hashes_count, sizeof(tQWORD), compare_qword);
						for (i=0; i<256; i++)
						{
							dwPos = dwLastPos;
							while (dwPos < m_Header.hashes_count && ((tBYTE*)&pHashes[dwPos])[7] == i)
								dwPos++;
							m_Index[i].pData = &pHashes[dwLastPos];
							m_Index[i].nCount = dwPos-dwLastPos;
							dwLastPos = dwPos;
						}
					}
				}
				if (PR_SUCC(error) && m_Header.wildcards_count)
				{
					hSTRING hStr;
					error = sysCreateObjectQuick((hOBJECT*)&hStr, IID_STRING);
					if (PR_SUCC(error))
					{
						error = heapAlloc((tPTR*)&m_pWildCards, m_Header.wildcards_count * sizeof(tCHAR*));
						if (PR_SUCC(error))
						{
							tCHAR* pwildcards = (tCHAR*)m_pDBData + m_Header.wildcards_offset;
							tCHAR c = 0;
							tDWORD nWildCards = 0;
							for (i=0; i<m_Header.wildcards_size; i++)
								pwildcards[i] ^= 0x56;
							for (i=0; i<m_Header.wildcards_size; i++)
							{
								if (0 == c)
								{
									tERROR error;
									tDWORD size;
									tWCHAR* pwildcard_w = NULL;
									if (nWildCards == m_Header.wildcards_count)
										break;
									PR_TRACE((this, prtNOT_IMPORTANT, "urlf\tReloadDatabase: found ANSI wildcard[%d] %s", nWildCards, pwildcards+i));
									error = hStr->ImportFromBuff(NULL, pwildcards+i, 0, cCP_ANSI, 0);
									if (PR_SUCC(error))
									{
										error = hStr->LengthEx(&size, cSTRING_WHOLE, cCP_UNICODE, cSTRING_Z);
										PR_TRACE((this, prtNOT_IMPORTANT, "urlf\tReloadDatabase: LengthEx result %terr, size=%d", error, size));
									}
									if (PR_SUCC(error))
									{
										error = heapAlloc((tPTR*)&pwildcard_w, size);
										PR_TRACE((this, prtNOT_IMPORTANT, "urlf\tReloadDatabase: allocated %d bytes for UNI wildcard, %terr", size, error));
									}
									if (PR_SUCC(error))
									{
										error = hStr->ExportToBuff(NULL, cSTRING_WHOLE, pwildcard_w, size, cCP_UNICODE, cSTRING_Z);
										PR_TRACE((this, prtNOT_IMPORTANT, "urlf\tReloadDatabase: ExportToBuff result %terr", error));
									}
									if (PR_SUCC(error))
									{
										PR_TRACE((this, prtNOT_IMPORTANT, "urlf\tReloadDatabase: loaded wildcard %S", pwildcard_w));
										m_pWildCards[nWildCards++] = pwildcard_w;
									}
									else
									{
										PR_TRACE((this, prtERROR, "urlf\tReloadDatabase: ANSI->UNI conversion failed with %terr, wildcard[%d] %s", error, nWildCards, pwildcards+i));
									}
								}
								c = pwildcards[i];
							}
						}
						hStr->sysCloseObject();
					}
				}
			}
		}
	}
//	if (PR_FAIL(error))
//	{
//		tDWORD size = sizeof(error);
//		sysSendMsg(pmc_ANTI_PHISHING, msg_ANTI_PHISHING_BAD_DATABASE, NULL, &error, &size);
//	}
	/*if (PR_FAIL(error))
	{
		cAskObjectAction action;
		action.m_nNonCuredReason = NCREASON_REPONLY;
		action.m_nDetectDanger = DETDANGER_HIGH;
		action.m_nDetectType = DETYPE_UNDETECT;
		action.m_nDescription = error;
		action.m_strObjectName = m_sDatabase;
		action.m_nObjectStatus = OBJSTATUS_CORRUPTED;
		action.m_tmTimeStamp = cDateTime();
		PR_TRACE((this, prtERROR, "urlf\tReloadDatabase(%S) failed with %terr",m_sDatabase.data(), error));
		sysSendMsg(pmc_EVENTS_CRITICAL, 0, NULL, &action, SER_SENDMSG_PSIZE);
	}*/
	
	if (PR_SUCC(error))
	{
		PR_TRACE((this, prtIMPORTANT, "urlf\tReloadDatabase: succeeded, hashes=%d, wildcard=%d", m_Header.hashes_count, m_Header.wildcards_count));
	}
	else
	{
		PR_TRACE((this, prtERROR, "urlf\tReloadDatabase: failed with %terr", error));
	}
	return error;
}

#define LIST_RT_FLAG_SYNCRONIZED         0x1000000

// warnALREADY - nothing changed
// errOK - synchronized successfuly
tERROR AntiPhishing::SyncWithTextFile(cStrObj& sName, cListCache& cache, cVector<cEnabledStrItem>& list)
{
	tERROR error;
	hIO io;
	tQWORD qsize;
	tDWORD size;
	tCHAR* buff = NULL;
	tCHAR* name = NULL;
	tDWORD i;
	tDWORD hint = 0;
	error = sysCreateObject((hOBJECT*)&io, IID_IO, PID_NATIVE_FIO);
	if (PR_FAIL(error))
		return error;
	cAutoObj<cIO> _close(io);
	error = sName.copy(io, pgOBJECT_FULL_NAME);
	if (PR_SUCC(error))
		error = io->set_pgOBJECT_OPEN_MODE(fOMODE_OPEN_IF_EXIST);
	if (PR_SUCC(error))
		error = io->set_pgOBJECT_ACCESS_MODE(fACCESS_READ);
	if (PR_SUCC(error))
		error = io->sysCreateObjectDone();
	if (PR_FAIL(error))
	{
		PR_TRACE((this, prtIMPORTANT, "urlf\tSyncWithTextFile: cannot open file %S", sName.data()));
		return error;
	}

//	if (SignCheck)
//	{
//		if (PR_FAIL(error = SignCheck((hOBJECT)io, cTRUE, NULL, 0, cTRUE, NULL)))
//			return error;
//	}
	
	if (PR_FAIL(error = io->GetSize(&qsize, IO_SIZE_TYPE_EXPLICIT)))
		return error;
	size = (tDWORD)qsize + 10;
	if (PR_FAIL(error = heapAlloc((tPTR*)&buff, size)))
		return error;
	io->SeekRead(&size, 0, buff, size);
	name = buff;

	tDWORD c = cache.m_vItems.count();
	error = warnALREADY;
	for (i=0; i<c; i++)
		cache.m_vItems[i].dwFlags &= ~LIST_RT_FLAG_SYNCRONIZED;

	tDWORD insert_count = 0;
	// find existing - check sync
	for (i=0; i<size; i++)
	{
		if (buff[i] == '\r' || buff[i] == '\n')
		{
			bool enabled = true;
			buff[i] = 0;
			if (*name == '-')
			{
//				enabled = false;
				name++;
			}
			if (*name && *name != ';')
			{
				bool found = false;
				tDWORD found_index = 0;
				tDWORD len = buff+i-name;
				if (hint<c && len==cache.m_vItems[hint].nLen && 
					(0 == list[hint].m_sName.compare(name, fSTRING_COMPARE_CASE_INSENSITIVE)))
				{
					PR_TRACE((this, prtNOTIFY, "urlf\tSyncWithTextFile: found %d, %s", hint, name));
					found = true;
					found_index = hint;
					hint++;
				}
				else
				{
					for (tDWORD i=hint+1; i<c; i++)
					{
						if (len != cache.m_vItems[i].nLen)
							continue;
						if (0 == list[i].m_sName.compare(name, fSTRING_COMPARE_CASE_INSENSITIVE))
						{
							PR_TRACE((this, prtNOTIFY, "urlf\tSyncWithTextFile: found %d, %s", i, name));
							found = true;
							found_index = i;
							hint = i+1; // next
							break;
						}
					}
				}
				if (found)
				{
					cache.m_vItems[found_index].dwFlags |= LIST_RT_FLAG_SYNCRONIZED;
//					if (((bool)!!list[found_index].m_bEnabled) != enabled)
//					{
//						PR_TRACE((this, prtNOTIFY, "urlf\tSyncWithTextFile: %s %s", enabled ? "enabled" : "disabled", name));
//						list[found_index].m_bEnabled = enabled;
//						error = errOK; // settings changed
//					}
				}
				else
				{
					insert_count++;
				}
			}
			name = buff+i+1;
		}
	}
	
	// delete old
	i=c;
	while (i>0)
	{
		i--;
		if (0 == (cache.m_vItems[i].dwFlags & LIST_RT_FLAG_SYNCRONIZED))
		{
			PR_TRACE((this, prtNOTIFY, "urlf\tSyncWithTextFile: deleted %d, %S", i, cache.m_vItems[i].pName));
			list.remove(i);
			error = errOK; // settings changed
		}
		if (i==0)
			break;
	}

	// insert new
	hint = 0;
	name = buff;
	for (i=0; i<size; i++)
	{
		if (buff[i] == 0)
		{
			bool enabled = true;
			if (*name == '-')
			{
				//enabled = false;
				name++;
			}
			if (*name && *name != ';')
			{
				if (hint>=list.count() || 0 != list[hint].m_sName.compare(name, fSTRING_COMPARE_CASE_INSENSITIVE))
				{
					cEnabledStrItem& item = list.insert(hint);
					item.m_bEnabled = enabled;
					item.m_sName = name;
					error = errOK; // settings changed
					PR_TRACE((this, prtNOTIFY, "urlf\tSyncWithTextFile: inserted %d, %s", hint, name));
					insert_count--;
					if (0 == insert_count)
						break;
				}
				hint++;
			}
			name = buff+i+1;
		}
	}

	heapFree(buff);
	if (error != warnALREADY)
		error = cache.Build(list);
	return error;
}

tERROR AntiPhishing::CreateMutex(hMUTEX* phMutex, const tWCHAR* pName, tBOOL bGlobal)
{
	tERROR error;
	hMUTEX hMutex = NULL;
	
	if (!phMutex)
		return errPARAMETER_INVALID;
	*phMutex = NULL;
	error = sysCreateObject((hOBJECT*)&hMutex, IID_MUTEX);
	if (PR_SUCC(error))
	{
		error = hMutex->set_pgGLOBAL(bGlobal);
		if (PR_SUCC(error))
			error = hMutex->set_pgOBJECT_NAME((tPTR)pName, 0, cCP_UNICODE);
		if (PR_SUCC(error))
			error = hMutex->sysCreateObjectDone();
		if (PR_FAIL(error))
			hMutex->sysCloseObject();
	}
	PR_TRACE((this, PR_SUCC(error) ? prtNOTIFY : prtERROR, "urlf\tCreateMutex: %S, %terr", pName,  error));
	*phMutex = hMutex;
	return error;
}

// AVP Prague stamp begin( External (user called) interface method implementation, SetSettings )
// Parameters are:
tERROR AntiPhishing::SetSettings( const cSerializable* p_settings )
{
	tERROR error = errOK;
	PR_TRACE_FUNC_FRAME( "Task::SetSettings method" );

	if (!p_settings)
		return errPARAMETER_INVALID;

	if (!p_settings->isBasedOn(cAntiPhishingSettings::eIID))
	{
		PR_TRACE((this, prtERROR, "urlf\tSetSettings incompatible (%terr)", error ));
		return errPARAMETER_INVALID;
	}

	cAntiPhishingSettings* pSettings = (cAntiPhishingSettings*) p_settings;

	if (pSettings->m_bLoadHTTP && !m_hHTTP)
	{
		error = m_hTM->GetService(TASKID_TM_ITSELF, *this, AVP_SERVICE_TRAFFICMONITOR_HTTP, &m_hHTTP, cREQUEST_SYNCHRONOUS);
		if ( PR_SUCC(error) && m_hHTTP )
			sysRegisterMsgHandler(pmc_TASK_STATE_CHANGED, rmhLISTENER, m_hTM, IID_ANY, m_hHTTP->propGetPID(), IID_ANY, PID_ANY);
	}

	cAutoCS _lock(m_hCS, cTRUE);
	
	m_LastAnswer.sUrl.clear();

	m_nFilterMsgClass = pSettings->m_nFilterMsgClass;
	m_nUrlFilteringMode = pSettings->m_nUrlFilteringMode;
	m_sDatabaseUpdCat = pSettings->m_sDatabaseUpdCat;
	m_bAskAction = pSettings->m_bAskAction;
	
	if (PR_SUCC(error) && !pSettings->m_sGlobalTaskName.empty())
		error = PRRegisterObject((tCHAR*)pSettings->m_sGlobalTaskName.c_str(cCP_ANSI), *this);
	
	if (pSettings->m_nFilterMsgClass)
		sysRegisterMsgHandler(pSettings->m_nFilterMsgClass, rmhDECIDER, m_hTM, IID_ANY, PID_ANY, IID_ANY, PID_ANY );
	if (pSettings->m_sDatabaseUpdCat.length() != 0)
		sysRegisterMsgHandler(pmc_UPDATER, rmhDECIDER, m_hTM, IID_ANY, PID_ANY, IID_ANY, PID_ANY );

	if (pSettings->m_nUrlFilteringMode & UFM_USE_DATABASE)
	{
		m_sDatabase = pSettings->m_sDataBase;
		sysSendMsg(pmc_PRODUCT_ENVIRONMENT, pm_EXPAND_ENVIRONMENT_STRING, (hOBJECT)cAutoString (m_sDatabase), 0, 0);
	}

	if (PR_SUCC(error))
		error = Reload(true);

	if (PR_SUCC(error))
	{
		hMUTEX hMutex = NULL;
		tDWORD c;
		// close open mutexes if any
		PR_TRACE((this, prtNOTIFY, "urlf\tMutexes to be created %d, embscr=%d", pSettings->m_vMutexes.count(), pSettings->m_bBlockEmbeddedingScripts ));
		for (c=0; c<m_vMutexes.count(); c++)
			m_vMutexes[c]->sysCloseObject();
		m_vMutexes.clear();
		// open mutexes
		if (pSettings->m_bBlockEmbeddedingScripts)
		{
			if (PR_SUCC(CreateMutex(&hMutex, L"KLBANNERCHECK_PR_DEF1689312", cTRUE)))
				m_vMutexes.push_back(hMutex);
		}
		for (c=0; c<pSettings->m_vMutexes.count(); c++)
		{
			PR_TRACE((this, prtNOTIFY, "urlf\tMutexes %S enabled=%d", pSettings->m_vMutexes[c].m_sName.data(),  pSettings->m_vMutexes[c].m_bEnabled));
			if (!pSettings->m_vMutexes[c].m_bEnabled)
				continue;
			error = CreateMutex(&hMutex, pSettings->m_vMutexes[c].m_sName.data(), cTRUE);
			if (PR_FAIL(error))
			{
				PR_TRACE((this, prtERROR, "urlf\tMutex create failed %S %terr", pSettings->m_vMutexes[c].m_sName.data(), error ));
				break;
			}
			m_vMutexes.push_back(hMutex);
		}
	}
	
	if (PR_FAIL(error))
	{
		PR_TRACE((this, prtERROR, "urlf\tSetSettings failed with %terr", error ));
		return error;
	}

	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, GetSettings )
// Parameters are:
tERROR AntiPhishing::GetSettings( cSerializable* p_settings )
{
	tERROR error = errNOT_SUPPORTED;	// get settings from BL
	PR_TRACE_FUNC_FRAME( "Task::GetSettings method" );

	return error;
}
// AVP Prague stamp end



//#include <shlwapi.h>
#define URL_UNESCAPE                    0x10000000
#define URL_ESCAPE_UNSAFE               0x20000000
#define URL_PLUGGABLE_PROTOCOL          0x40000000
#define URL_WININET_COMPATIBILITY       0x80000000
#define URL_DONT_ESCAPE_EXTRA_INFO      0x02000000
#define URL_DONT_UNESCAPE_EXTRA_INFO    URL_DONT_ESCAPE_EXTRA_INFO
#define URL_BROWSER_MODE                URL_DONT_ESCAPE_EXTRA_INFO
#define URL_ESCAPE_SPACES_ONLY          0x04000000
#define URL_DONT_SIMPLIFY               0x08000000
#define URL_NO_META                     URL_DONT_SIMPLIFY
#define URL_UNESCAPE_INPLACE            0x00100000
#define URL_CONVERT_IF_DOSPATH          0x00200000
#define URL_UNESCAPE_HIGH_ANSI_ONLY     0x00400000
#define URL_INTERNAL_PATH               0x00800000  // Will escape #'s in paths
#define URL_FILE_USE_PATHURL            0x00010000
#define URL_ESCAPE_PERCENT              0x00001000
#define URL_ESCAPE_SEGMENT_ONLY         0x00002000  // Treat the entire URL param as one URL segment.

int __inline isalnumW(tWCHAR wc)
{
	if (wc >= 'a' && wc <= 'z')
		return 1;
	if (wc >= 'A' && wc <= 'Z')
		return 1;
	if (wc >= '0' && wc <= '9')
		return 1;
	return 0;
}

bool unescape_charW(const tWCHAR* hexstr, tINT chars, tWCHAR* pwc)
{
	tWCHAR res = 0;
	tWCHAR wc;
	tINT i;
	for (i=0; i<chars; i++)
	{
		res <<= 4;
		wc = *hexstr++;
		if (wc >= '0' && wc <= '9')
			res |= (wc - '0');
		else if (wc >= 'a' && wc <= 'f')
			res |= (wc - 'a' + 10);
		else if (wc >= 'A' && wc <= 'F')
			res |= (wc - 'a' + 10);
		else
			return false;
	}
	*pwc = res;
	return true;
}

bool UrlUnescapeInplaceW(tWCHAR* pszUrl)
{
    const tWCHAR* src;
    tWCHAR *dst;
	tWCHAR wc;
	
    if(!pszUrl)
		return false;
	
    src = dst = pszUrl;
	
    do
	{
		wc = *src;
		if (wc == L'%') 
		{
			if (src[1] == 'U')
			{
				if (unescape_charW(src+2, 4, &wc)) 
					src += 4; /* Advance to end of escape */
			}
			else 
			{
				if (unescape_charW(src+1, 2, &wc)) 
					src += 2; /* Advance to end of escape */
			}
		}
		*dst++ = wc;
		src++;
    } while (wc);
	
    return true;
}

bool UrlSimplifyInplaceW(tWCHAR* pszUrl)
{
	tWCHAR *wk1, *wk2, *mp, *root;
    tINT state;
    tDWORD nLen, nWkLen;

	/*
	* state =
	*         0   initial  1,3
	*         1   have 2[+] alnum  2,3
	*         2   have scheme (found :)  4,6,3
	*         3   failed (no location)
	*         4   have //  5,3
	*         5   have 1[+] alnum  6,3
	*         6   have location (found /) save root location
	*/
	
	wk1 = wk2 = (tWCHAR*)pszUrl;
	state = 0;
	while (*wk1) 
	{
		switch (state) 
		{
		case 0:
			if (!isalnumW(*wk1)) {state = 3; break;}
			*wk2++ = *wk1++;
			if (!isalnumW(*wk1)) {state = 3; break;}
			*wk2++ = *wk1++;
			state = 1;
			break;
		case 1:
			*wk2++ = *wk1;
			if (*wk1++ == L':') state = 2;
			break;
		case 2:
			if (*wk1 != L'/') {state = 3; break;}
			*wk2++ = *wk1++;
			if (*wk1 != L'/') {state = 6; break;}
			*wk2++ = *wk1++;
			state = 4;
			break;
		case 3:
			if (wk1 == wk2)
				return true;
			nWkLen = wcslen(wk1);
			memcpy(wk2, wk1, (nWkLen + 1) * sizeof(tWCHAR));
			wk1 += nWkLen;
			wk2 += nWkLen;
			break;
		case 4:
			if (!isalnumW(*wk1) && (*wk1 != L'-') && (*wk1 != L'.')) {state = 3; break;}
			while(isalnumW(*wk1) || (*wk1 == L'-') || (*wk1 == L'.')) *wk2++ = *wk1++;
			state = 5;
			break;
		case 5:
			if (*wk1 != L'/') {state = 3; break;}
			*wk2++ = *wk1++;
			state = 6;
			break;
		case 6:
			/* Now at root location, cannot back up any more. */
			/* "root" will point at the '/' */
			root = wk2-1;
			while (*wk1) {
//					PR_TRACE((this, prtNOTIFY, "wk1=%c\n", (tCHAR)*wk1));
				mp = wcschr(wk1, L'/');
				if (!mp) {
					nWkLen = wcslen(wk1);
					memcpy(wk2, wk1, (nWkLen + 1) * sizeof(tWCHAR));
					wk1 += nWkLen;
					wk2 += nWkLen;
					continue;
				}
				nLen = mp - wk1 + 1;
				memcpy(wk2, wk1, nLen * sizeof(tWCHAR));
				wk2 += nLen;
				wk1 += nLen;
				if (*wk1 == L'.') {
//						TRACE("found '/.'\n");
					if (*(wk1+1) == L'/') {
						/* case of /./ -> skip the ./ */
						wk1 += 2;
					}
					else if (*(wk1+1) == L'.') {
						/* found /..  look for next / */
//							TRACE("found '/..'\n");
						if (*(wk1+2) == L'/' || *(wk1+2) == L'?' || *(wk1+2) == L'#' || *(wk1+2) == 0) {
							/* case /../ -> need to backup wk2 */
//								TRACE("found '/../'\n");
							*(wk2-1) = L'\0';  /* set end of string */
							mp = wcsrchr(root, L'/');
							if (mp && (mp >= root)) {
								/* found valid backup point */
								wk2 = mp + 1;
								if(*(wk1+2) != L'/')
									wk1 += 2;
								else
									wk1 += 3;
							}
							else {
								/* did not find point, restore '/' */
								*(wk2-1) = L'/';
							}
						}
					}
				}
			}
			*wk2 = L'\0';
			break;
		default:
			//FIXME("how did we get here - state=%d\n", state);
			return false;
		}
	}
	*wk2 = L'\0';
//		PR_TRACE((this, prtNOTIFY, "UrlCanonicalize: Simplified, orig <%S>, simple <%S>\n", pszUrl, lpszUrlCpy));
	return true;
}

bool UrlFixupInplaceW(tWCHAR* pszUrl, tUINT* pnUrlLen, cStrBuff* pDomain, tUINT* pnDomainLen, bool* pbDomainOnly)
{
	tWCHAR *src, *dst, *domain = NULL;
	tWCHAR wc;
	bool bDomainAssigned = false;


	src = dst = pszUrl;

	if (pDomain)
		*pDomain = L"";
	if (pnDomainLen)
		*pnDomainLen = 0;
	if (pbDomainOnly)
		*pbDomainOnly = false;

	while (1)
	{
		wc = *src;
		if (!wc)
			break;
		if (wc == ':' && src[1] == '/')
		{
			if (src[2] == '/')
			{
				if (!domain)
					domain = dst + 3;
			}
			else
			{
				src++; // replace ":/" with "/"
				wc = '/';
			}
		}
		else if (wc == '/' && !bDomainAssigned && domain && dst > domain)
		{
			if (pDomain)
			{
				int len = dst-domain;
				PR_TRACE((g_root, prtNOTIFY, "urlf\tUrlFixupInplaceW: domain_start=%S, len=%d", domain, len));
				pDomain->reserve((len+1) * sizeof(tWCHAR), false);
				memcpy((void*)pDomain->ptr(), domain, len * sizeof(tWCHAR));
				((tWCHAR*)pDomain->ptr())[len] = 0;
				PR_TRACE((g_root, prtNOTIFY, "urlf\tUrlFixupInplaceW: domain=%S", pDomain->ptr()));
				if (pnDomainLen)
					*pnDomainLen = len;
			}
			bDomainAssigned = true;
			if (pbDomainOnly && !src[1] )
			{
				PR_TRACE((g_root, prtNOTIFY, "urlf\tUrlFixupInplaceW: domainonly=true"));
				*pbDomainOnly = true;
			}
		}
		if (src != dst)
			*dst = wc;
		src++;
		dst++;
	};
	while (dst > pszUrl && (dst[-1] == '\r' || dst[-1] == '\n' || dst[-1] == '/'))
		dst--;
	*dst = 0;
	if (pnUrlLen)
		*pnUrlLen = dst - pszUrl;
	return true;
}


tERROR AddStringToHash(hHASH hHash, tWCHAR* pwcString, tBOOL bLowerCase)
{
	tERROR error;
	tWCHAR wc;
	tUINT size;
	if (!pwcString)
		return errOK;
	while (wc = *pwcString)
	{
		if (bLowerCase)
		{
			if (wc >= 'A' && wc <= 'Z')
				wc |= 0x20; // lower-case
		}
		size = wc <= 0xFF ? 1 : sizeof(wc);
		if (PR_FAIL(error = hHash->Update((tBYTE*) &wc, size)))
			return error;
		pwcString++;
	}
	return errOK;
}

bool ExtractDomain(WCHAR* pUrl, DWORD dwLen, WCHAR** ppDomain, DWORD* pdwDomainLen)
{
	WCHAR* pDomain = NULL;
	WCHAR* pDomainEnd = pUrl;

	while (dwLen)
	{
		WCHAR c = *pDomainEnd;
		if (0==c)
			return false;
		if ('/'==c || '?'==c)
			break;
		if (':'==c && '/'==pDomainEnd[1] && '/'==pDomainEnd[2])
		{
			pDomainEnd+=3;
			pDomain = pDomainEnd;
		}
		pDomainEnd++;
		dwLen--;
	}
	if (pDomain && pDomainEnd!=pDomain)
	{
		*pdwDomainLen = pDomainEnd - pDomain;
		*ppDomain = pDomain;
		return true;
	}

	return false;
}

int CompareDomains(WCHAR* pUrl1, DWORD dwUrl1Len, WCHAR* pUrl2, DWORD dwUrl2Len)
{
	int level = 0;
	WCHAR *pDomain1, *pDomain2;
	DWORD dwLen1, dwLen2;
//	DWORD dwLen11, dwLen12;
	if (!ExtractDomain(pUrl1, dwUrl1Len, &pDomain1, &dwLen1))
		return 0;
	if (!ExtractDomain(pUrl2, dwUrl2Len, &pDomain2, &dwLen2))
		return -1;
//	dwLen11 = dwLen1; dwLen12 = dwLen2;
//	do {
//		WCHAR c1, c2;
//		dwLen1--; dwLen2--;
//		c1 = pDomain1[dwLen1]; c1 = LCASE(c1);
//		c2 = pDomain2[dwLen2]; c2 = LCASE(c2);
//		if (c1 != c2)
//			break;
//		if ('.' == c1)
//			level++;
//	} while(dwLen1 && dwLen2);
//	if (!dwLen1 && !dwLen2)
//		level++;
//	if (0 == level)
//	{
//		dwLen1 = dwLen11; dwLen2 = dwLen12;
		while (dwLen1 && pDomain1[dwLen1] != '.') 
			dwLen1--;
		while (dwLen2 && pDomain2[dwLen2] != '.') 
			dwLen2--;
		if (dwLen1 && dwLen2)
		{
			do {
				WCHAR c1, c2;
				dwLen1--; dwLen2--;
				c1 = pDomain1[dwLen1]; c1 = LCASE(c1);
				c2 = pDomain2[dwLen2]; c2 = LCASE(c2);
				if (c1 != c2)
					break;
				if ('.' == c1)
					level++;
			} while(dwLen1 && dwLen2);
			if (!dwLen1 && !dwLen2)
				level++;
		}
		if (level)
			level = -(level+1);
//	}
//	ODS(("*** CompareDomains: %d", level));
	return level;
}


// AVP Prague stamp begin( External (user called) interface method implementation, AskAction )
// Parameters are:
tERROR AntiPhishing::AskAction( tActionId p_actionId, cSerializable* p_actionInfo )
{
	tERROR error = errOK;
	PR_TRACE_FUNC_FRAME( "Task::AskAction method" );

	
	if (p_actionId == msg_URL_FILTER_CHECK_URL || p_actionId == msg_URL_FILTER_BLOCK_IF_NOT_WHITELISTED)
	{
		cStrBuff sURL;
		tWCHAR* cptr;
		tWCHAR c;
		tUINT nUrlLen, nDomainLen;
		time_t now = cDateTime();
		cStrBuff sDomain;
		tWCHAR* pRule = NULL;
		bool bDomainOnly = false;

		if (m_nTaskState != TASK_STATE_RUNNING)
			return errOK;

		if (!p_actionInfo)
		{
			PR_TRACE((this, prtERROR, "urlf\tAskAction: msg_URL_FILTER_CHECK_URL, p_actionInfo = NULL"));
			return errPARAMETER_INVALID;
		}
		if (!p_actionInfo->isBasedOn(cAntiPhishingCheckRequest::eIID))
		{
			PR_TRACE((this, prtERROR, "urlf\tAskAction: msg_URL_FILTER_CHECK_URL, not cAntiPhishingCheckRequest::eIID (%08X)", p_actionInfo->getIID()));
			return errOBJECT_INCOMPATIBLE;
		}

		cAntiPhishingCheckRequest* pRequest = (cAntiPhishingCheckRequest*)p_actionInfo;
		if (pRequest->m_sURL.length() == 0)
		{
			PR_TRACE((this, prtIMPORTANT, "urlf\tAskAction: msg_URL_FILTER_CHECK_URL, empty URL"));
			return errOK;
		}
		sURL = pRequest->m_sURL.c_str(cCP_UNICODE);
		cptr = sURL;
		while (c = *cptr)
		{
			if (c >= 'A' && c <= 'Z')
				*cptr |= 0x20;
			else if (c == '?')
			{
				*cptr = 0;
				break;
			}
			cptr++;
		}
		PR_TRACE((this, prtIMPORTANT, "urlf\tAskAction: msg_URL_FILTER_CHECK_URL, URL=%S", sURL.ptr()));

		// canonicalize and fixup
		UrlSimplifyInplaceW(sURL);
		UrlUnescapeInplaceW(sURL);
		UrlFixupInplaceW(sURL, &nUrlLen, &sDomain, &nDomainLen, &bDomainOnly);
		
		PR_TRACE((this, prtIMPORTANT, "urlf\tAskAction: msg_URL_FILTER_CHECK_URL, canonical URL=%S, Domain=%S", sURL.ptr(), sDomain.ptr()));

		{
			// check url under lock
			cAutoCS _lock(m_hCS, cFALSE);

			if (pRequest->m_bInteractive && m_LastAnswer.time && !m_LastAnswer.sUrl.empty())
			{
				PR_TRACE((this, prtIMPORTANT, "urlf\tAskAction: URL=%S, last url was %S with answer %terr", (tWCHAR*)sURL, m_LastAnswer.sUrl.data(), m_LastAnswer.nErrResult));
				if ((unsigned)(now - m_LastAnswer.time) < 15) // 15 seconds
				{
					if (m_LastAnswer.sUrl.compare((tWCHAR*)sURL) == 0)
					{
						PR_TRACE((this, prtIMPORTANT, "urlf\tAskAction: URL=%S, using last answer %terr", (tWCHAR*)sURL, m_LastAnswer.nErrResult));
						m_LastAnswer.time = now;
						return m_LastAnswer.nErrResult;
					}
				}
			}

			// check white lists
			if (PR_SUCC(error) && (m_nUrlFilteringMode & UFM_USE_WHITELIST))
			{
				if (m_WhiteListCache.Find(sURL, nUrlLen, sDomain, nDomainLen, &pRule))
				{
					PR_TRACE((this, prtIMPORTANT, "urlf\tAskAction: msg_URL_FILTER_CHECK_URL: '%S' found in whitelist", sURL.ptr()));
					if (pRule)
						pRequest->m_sTemplate = pRule;
					return errOK;
				}
			}
			
			// check black lists
			if (PR_SUCC(error) && ((m_nUrlFilteringMode & UFM_DENY_UNKNOWN) || (p_actionId == msg_URL_FILTER_BLOCK_IF_NOT_WHITELISTED)))
			{
				PR_TRACE((this, prtIMPORTANT, "urlf\tAskAction: msg_URL_FILTER_CHECK_URL: '%S' not found in whitelist and UFM_DENY_UNKNOWN set", sURL.ptr()));
				error = errACCESS_DENIED;
			}

			if (PR_SUCC(error) && (m_nUrlFilteringMode & UFM_USE_BLACKLIST))
			{
				if (m_UserBlackListCache.Find(sURL, nUrlLen, sDomain, nDomainLen, &pRule))
				{
					PR_TRACE((this, prtIMPORTANT, "urlf\tAskAction: msg_URL_FILTER_CHECK_URL: '%S' found in user blacklist", sURL.ptr()));
					error = errACCESS_DENIED;
				}
			}
			
			if (PR_SUCC(error) && bDomainOnly && (m_nUrlFilteringMode & UFM_DONT_DENY_DOMAIN))
			{
				PR_TRACE((this, prtIMPORTANT, "urlf\tAskAction: msg_URL_FILTER_CHECK_URL: '%S' domain, allow", sURL.ptr()));
				pRequest->m_sTemplate = L"\\";
				return errOK;
			}
					
			if (PR_SUCC(error) && (m_nUrlFilteringMode & UFM_USE_BLACKLIST))
			{
				if (m_BlackListCache.Find(sURL, nUrlLen, sDomain, nDomainLen, &pRule))
				{
					PR_TRACE((this, prtIMPORTANT, "urlf\tAskAction: msg_URL_FILTER_CHECK_URL: '%S' found in blacklist", sURL.ptr()));
					error = errACCESS_DENIED;
				}
			}
			
			if (PR_SUCC(error) && (m_nUrlFilteringMode & UFM_USE_DATABASE))
			{
				if (!m_pDBData)
				{
					PR_TRACE((this, prtERROR, "urlf\tAskAction: msg_URL_FILTER_CHECK_URL: m_pDBData=NULL"));
					return errOBJECT_NOT_INITIALIZED;
				}
				
				bool matched = false;
				if (m_pWildCards)
				{
					tDWORD i;
					for (i=0; i<m_Header.wildcards_count; i++)
					{
						if (m_pWildCards[i]) // double-check
						{
							if (matched = MatchWithPatternW((unsigned short*)m_pWildCards[i], (unsigned short*)sURL.ptr(), false, false))
							{
								pRule = m_pWildCards[i];
								break;
							}
							PR_TRACE((this, prtNOT_IMPORTANT, "urlf\tAskAction: msg_URL_FILTER_CHECK_URL: wildcard '%S', not matched", m_pWildCards[i]));
						}
					}
				}
				
				if (matched)
				{
					PR_TRACE((this, prtIMPORTANT, "urlf\tAskAction: msg_URL_FILTER_CHECK_URL: URL=%S matched wildcard, returning errACCESS_DENIED", sURL.ptr()));
					error = errACCESS_DENIED;
				}
				else
				{
					tDWORD  dwHash[4];
					tBYTE*  pbHash = (tBYTE*)&dwHash;
					tQWORD* pqwHash = (tQWORD*)&dwHash;
					if (NULL == m_hHashMD5)
						error = sysCreateObjectQuick((hOBJECT*)&m_hHashMD5, IID_HASH, PID_HASH_MD5);
					if (PR_SUCC(error))
					{
						error = m_hHashMD5->Reset();
						if (PR_SUCC(error))
							error = AddStringToHash(m_hHashMD5, sURL, cTRUE);
						if (PR_SUCC(error))
							error = m_hHashMD5->GetHash(pbHash, sizeof(dwHash));
					}
					if (PR_FAIL(error))
					{
						PR_TRACE((this, prtERROR, "urlf\tAskAction: msg_URL_FILTER_CHECK_URL: calc hash failed %terr", error));
						return error;
					}
					
					dwHash[0] ^= dwHash[2];
					dwHash[1] ^= dwHash[3];
					
					tUINT nIndex = ((tBYTE*)&dwHash)[7];
					PR_TRACE((this, prtNOT_IMPORTANT, "urlf\tAskAction: msg_URL_FILTER_CHECK_URL: searching hash %08X%08X from %d hashes in bunch", dwHash[1], dwHash[0], m_Index[nIndex].nCount));
					{
						tDWORD i;
						tQWORD* pData = m_Index[nIndex].pData;
						for (i=0; i<m_Index[nIndex].nCount; i++)
						{
							PR_TRACE((this, prtNOT_IMPORTANT, "urlf\tAskAction: msg_URL_FILTER_CHECK_URL: bunch hash (%d) %08X%08X", i, ((tDWORD*)&pData[i])[1], ((tDWORD*)&pData[i])[0]));
						}
						
					}
					if (bsearch(pqwHash, m_Index[nIndex].pData, m_Index[nIndex].nCount, sizeof(tQWORD), compare_qword))
					{
						PR_TRACE((this, prtIMPORTANT, "urlf\tAskAction: msg_URL_FILTER_CHECK_URL: URL=%S found in hashes, returning errACCESS_DENIED", sURL.ptr()));
						pRequest->m_sTemplate = (tWCHAR*)sURL;
						error = errACCESS_DENIED;
					}
					else
					{
						PR_TRACE((this, prtIMPORTANT, "urlf\tAskAction: msg_URL_FILTER_CHECK_URL: hash %08X%08X not found", dwHash[1], dwHash[0]));
					}
				}
			}
		} // leave m_hCS lock
		

		if (error == errACCESS_DENIED && (m_nUrlFilteringMode & UFM_DONT_DENY_DOMAIN))
		{
			if (!pRequest->m_sReferrer.empty())
			{
				cStrBuff sReferrer = pRequest->m_sReferrer.data();
				UINT nRefferrerLen = 0;
				// canonicalize and fixup
				UrlSimplifyInplaceW(sReferrer);
				UrlUnescapeInplaceW(sReferrer);
				UrlFixupInplaceW(sReferrer, &nRefferrerLen, NULL, NULL, NULL);
				int nCmpRes = CompareDomains(sURL, nUrlLen, sReferrer, nRefferrerLen);
				if (nCmpRes >= 2)
				{
					PR_TRACE((this, prtIMPORTANT, "urlf\tAskAction: msg_URL_FILTER_CHECK_URL: '%S' domain, allow", sURL.ptr()));
					pRequest->m_sTemplate = L"host";
					return errOK;
				}
			}
		}

		if (error == errACCESS_DENIED)
		{
			bool bAsk = false;
			cAskObjectAction action;

			{   // aquire lock
				cAutoCS _lock(m_hCS, cFALSE);
				m_Statistics.m_nURLsDetected++;
				if (pRule)
					pRequest->m_sTemplate = pRule;

				{
					// load request/report settings
					cAntiPhishingSettings* pSettings = NULL;
					if (PR_SUCC(error = m_hTM->LockTaskSettings(*this, (cSerializable**) &pSettings, cTRUE)))
					{
						error = action.assign(pSettings->m_AskActionDef, false);
						m_hTM->UnlockTaskSettings(*this, pSettings, cFALSE);
					}
				}

#if VER_PRODUCTVERSION_HIGH <= 7
				action.m_strObjectName = pRequest->m_sProcessName;
				action.m_strDetectObject = pRequest->m_sURL;
				action.m_strDetectName = pRequest->m_sTemplate;
				action.m_nDescription  = pRequest->m_nPID;
#else // VER_PRODUCTVERSION_HIGH
				action.m_strObjectName = pRequest->m_sURL;
				action.m_nPID          = pRequest->m_nPID;
#endif // VER_PRODUCTVERSION_HIGH
				action.m_nResultAction = action.m_nDefaultAction;
				action.m_tmTimeStamp = now;

				if (m_nApply2AllAction != ACTION_UNKNOWN)
				{
					action.m_nResultAction = m_nApply2AllAction;
					PR_TRACE((this, prtNOTIFY, "urlf\tUsing Apply2All action (%d)", m_nApply2AllAction));
				}
				else if (action.m_nResultAction != action.m_nActionsAll)
				{
					if (!m_bAskAction)
					{
						PR_TRACE((this, prtNOTIFY, "urlf\tAskAction: !m_Settings.m_bAskAction, DENIED URL=%S", (tWCHAR*)sURL));
						action.m_nResultAction = ACTION_DENY;
					}
					else if (!pRequest->m_bInteractive)
					{
						PR_TRACE((this, prtNOTIFY, "urlf\tAskAction: !pRequest->m_bInteractive, DENIED URL=%S", (tWCHAR*)sURL));
						action.m_nResultAction = ACTION_DENY;
					}
					else
					{
						bAsk = true;
					}
				}
			} // release lock

			if (bAsk)
			{
				PR_TRACE((this, prtNOTIFY, "urlf\tAskAction: URL=%S, user request...", (tWCHAR*)sURL));
				error = sysSendMsg(pmcASK_ACTION, cAskObjectAction::DISINFECT, NULL, &action, SER_SENDMSG_PSIZE);
				PR_TRACE((this, prtNOTIFY, "urlf\tAskAction: URL=%S, user request deny=%d", (tWCHAR*)sURL, action.m_nResultAction == ACTION_DENY));
				if (action.m_nApplyToAll == APPLYTOALL_TRUE)
					m_nApply2AllAction = action.m_nResultAction;
			}

			//report
			action.m_nObjectStatus =  (action.m_nResultAction == ACTION_DENY ? OBJSTATUS_DENIED : OBJSTATUS_ALLOWED);
			sysSendMsg(action.m_nResultAction == ACTION_DENY ? pmc_EVENTS_CRITICAL : pmc_EVENTS_NOTIFY, 0, NULL, &action, SER_SENDMSG_PSIZE);
			
			if (m_sDatabaseUpdCat == cUPDATE_CATEGORY_ANTIPHISHING)
			{
				cInfectedObjectInfo inf(action);
				inf.m_nDescription = 0;
				CalcUnique(this, &inf.m_qwUniqueId, pRequest->m_sURL);
				sysSendMsg(pmc_UPDATE_THREATS_LIST, 0, this, &inf, SER_SENDMSG_PSIZE);
			}

			{   // aquire lock
				cAutoCS _lock(m_hCS, cFALSE);
				m_LastAnswer.time = cDateTime();
				m_LastAnswer.sUrl = (tWCHAR*)sURL;
				m_LastAnswer.nErrResult = (action.m_nResultAction == ACTION_ALLOW ? errOK : errACCESS_DENIED);
			}   // release lock

			if (action.m_nResultAction == ACTION_ALLOW)
			{
				PR_TRACE((this, prtIMPORTANT, "urlf\tAskAction: URL=%S ALLOW", (tWCHAR*)sURL));
				return errOK;
			}

			if(action.m_nResultAction == ACTION_REPORTONLY)
			{
				PR_TRACE((this, prtIMPORTANT, "urlf\tAskAction: URL=%S SKIP", (tWCHAR*)sURL));
				return errOK;
			}

			if (action.m_nResultAction == ACTION_DENY)
			{
				PR_TRACE((this, prtIMPORTANT, "urlf\tAskAction: URL=%S DENY", (tWCHAR*)sURL));
				return errACCESS_DENIED;
			}
			
			PR_TRACE((this, prtERROR, "urlf\tAskAction: AskAction returned unknown result: %08X, %terr, result ALLOW by default", action.m_nResultAction, error));
			// default - allow
			return errOK;
		}
		return error;
	}

	if (p_actionId == msg_ANTI_PHISHING_CHECK_DATABASE)
	{
		if (!m_pDBData)
		{
			PR_TRACE((this, prtERROR, "urlf\tAskAction: msg_ANTI_PHISHING_CHECK_DATABASE: m_pDBData = NULL, errOBJECT_NOT_INITIALIZED"));
			return errOBJECT_NOT_INITIALIZED;
		}
		PR_TRACE((this, prtIMPORTANT, "urlf\tAskAction: msg_ANTI_PHISHING_CHECK_DATABASE: errOK"));
		return errOK;
	}

	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, SetState )
// Parameters are:
tERROR AntiPhishing::SetState( tTaskRequestState p_state )
{
	tERROR error = errOK;
	PR_TRACE_FUNC_FRAME( "Task::SetState method" );

	switch (p_state)
	{
	case TASK_REQUEST_RUN:
		m_nTaskState = TASK_STATE_RUNNING;
		break;
	case TASK_REQUEST_PAUSE:
		m_nTaskState = TASK_STATE_PAUSED;
		break;
	case TASK_REQUEST_STOP:
		m_nTaskState = TASK_STATE_STOPPED;
		break;
	default:
		return error = errPARAMETER_INVALID;
	}

	return error = warnTASK_STATE_CHANGED;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, GetStatistics )
// Parameters are:
tERROR AntiPhishing::GetStatistics( cSerializable* p_statistics )
{
	tERROR error;
	PR_TRACE_FUNC_FRAME( "Task::GetStatistics method" );

	if (!p_statistics)
		return errPARAMETER_INVALID;

	error = p_statistics->assign(m_Statistics, false);

	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( C++ class regitration,  )
// Interface "Task". Register function
tERROR AntiPhishing::Register( hROOT root ) 
{
	tERROR error;
// AVP Prague stamp end



// AVP Prague stamp begin( Property table,  )
mpPROPERTY_TABLE(Task_PropTable)
	mSHARED_PROPERTY( pgINTERFACE_VERSION, Task_VERSION )
	mSHARED_PROPERTY_PTR( pgINTERFACE_COMMENT, "", 1 )
	mpLOCAL_PROPERTY_BUF( pgTASK_TM, AntiPhishing, m_hTM, cPROP_BUFFER_READ_WRITE | cPROP_WRITABLE_ON_INIT )
	mpLOCAL_PROPERTY_BUF( pgTASK_STATE, AntiPhishing, m_nTaskState, cPROP_BUFFER_READ )
	mpLOCAL_PROPERTY_BUF( pgTASK_SESSION_ID, AntiPhishing, m_nTaskSessionId, cPROP_BUFFER_READ_WRITE | cPROP_WRITABLE_ON_INIT )
	mpLOCAL_PROPERTY_BUF( pgTASK_ID, AntiPhishing, m_nTaskId, cPROP_BUFFER_READ_WRITE | cPROP_WRITABLE_ON_INIT )
	mpLOCAL_PROPERTY_BUF( pgTASK_PARENT_ID, AntiPhishing, m_nTaskParentId, cPROP_BUFFER_READ_WRITE | cPROP_WRITABLE_ON_INIT )
mpPROPERTY_TABLE_END(Task_PropTable)
// AVP Prague stamp end



// AVP Prague stamp begin( Internal method table,  )
mINTERNAL_TABLE_BEGIN(Task)
	mINTERNAL_METHOD(ObjectInit)
	mINTERNAL_METHOD(ObjectInitDone)
	mINTERNAL_METHOD(ObjectPreClose)
	mINTERNAL_METHOD(MsgReceive)
mINTERNAL_TABLE_END(Task)
// AVP Prague stamp end



// AVP Prague stamp begin( External method table,  )
mEXTERNAL_TABLE_BEGIN(Task)
	mEXTERNAL_METHOD(Task, SetSettings)
	mEXTERNAL_METHOD(Task, GetSettings)
	mEXTERNAL_METHOD(Task, AskAction)
	mEXTERNAL_METHOD(Task, SetState)
	mEXTERNAL_METHOD(Task, GetStatistics)
mEXTERNAL_TABLE_END(Task)
// AVP Prague stamp end



// AVP Prague stamp begin( Registration call,  )

	PR_TRACE_FUNC_FRAME_( *root, "Task::Register method", &error );

	error = CALL_Root_RegisterIFace(
		root,                                   // root object
		IID_TASK,                               // interface identifier
		PID_ANTIPHISHING,                       // plugin identifier
		0x00000000,                             // subtype identifier
		Task_VERSION,                           // interface version
		VID_MIKE,                               // interface developer
		&i_Task_vtbl,                           // internal(kernel called) function table
		(sizeof(i_Task_vtbl)/sizeof(tPTR)),     // internal function table size
		&e_Task_vtbl,                           // external function table
		(sizeof(e_Task_vtbl)/sizeof(tPTR)),     // external function table size
		Task_PropTable,                         // property table
		mPROPERTY_TABLE_SIZE(Task_PropTable),   // property table size
		sizeof(AntiPhishing)-sizeof(cObjImpl),  // memory size
		0                                       // interface flags
	);

	#ifdef _PRAGUE_TRACE_
		if ( PR_FAIL(error) )
			PR_TRACE( (root,prtDANGER,"Task(IID_TASK) registered [%terr]",error) );
	#endif // _PRAGUE_TRACE_
// AVP Prague stamp end



// AVP Prague stamp begin( C++ class regitration end,  )
	return error;
}

tERROR pr_call Task_Register( hROOT root ) { return AntiPhishing::Register(root); }
// AVP Prague stamp end



// AVP Prague stamp begin( Caution !!!,  )
// You have to implement propetry: pgTASK_PERSISTENT_STORAGE
// AVP Prague stamp end



