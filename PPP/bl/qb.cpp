// -------------------------------------------

#include "bl.h"

#include <plugin/p_win32_nfio.h>
#include <structs/s_avs.h>
#include <structs/s_qb.h>

// ------------------------------------------------

tERROR BlImpl::init_qb()
{
	if( m_qb )
		return errOK;

	tERROR err = sysCreateObject((cObj**)&m_qb, IID_QB, PID_QB, SUBTYPE_ANY);

	if( PR_SUCC(err) )
	{
		cStrObj path; ExpandEnvironmentString(cENVIRONMENT_QUARANTINE, path);
		err = path.copy(m_qb, plQB_STORAGE_PATH );
	}

	if( PR_SUCC(err) )
		err = m_qb->propSetDWord(plQB_STORAGE_ID, cQB_OBJECT_ID_BACKUP);

	if( PR_SUCC(err) )
		err = m_qb->sysCreateObjectDone();

	return err;
}

tERROR BlImpl::store_qb_object(cInfectedObjectInfo* info)
{
	if( info->m_strObjectName.empty() )
	{
		PR_TRACE((this, prtERROR, "bl\tObject name empty in QB store operation"));
		return errOBJECT_INVALID;
	}

	tERROR err = errOK;
	if( PR_FAIL(err = init_qb()) )
		return err;

	hIO hSource = NULL; bool bOwnSource = true;
	if( info->isBasedOn(cAskQBAction::eIID) )
		hSource = ((cAskQBAction*)info)->m_hSource;

	if( !hSource )
	{
		cIOObj ioFile(*this, cAutoString (info->m_strObjectName), fACCESS_READ | fACCESS_FORCE_READ | fACCESS_FORCE_OPEN, fOMODE_OPEN_IF_EXIST | fOMODE_OPEN_IF_EXECUTABLE);
		if( PR_FAIL(err = ioFile.last_error()) )
			PR_TRACE((this, prtERROR, "tm\tCannot create IO for QB store operation, object: %S, %terr", info->m_strObjectName.data(), err));
		else
			hSource = ioFile.relinquish(), bOwnSource = false;
	}
	
	if( !hSource )
		return err;

	cQBObject qbObj;
	qbObj.assign(*info, false);
	qbObj.m_AdditionalData = info;
	
	cAutoObj<cToken> hToken;
	err = sysCreateObjectQuick(hToken, IID_TOKEN);
	if( PR_SUCC(err) )
		err = qbObj.m_strUserName.assign(hToken, pgOBJECT_NAME);
	if( PR_FAIL(err) )
		PR_TRACE((this, prtERROR, "tm\tCannot get user name for QB store operation, object: %S, %terr", info->m_strObjectName.data(), err));

	err = hSource->GetSize(&qbObj.m_qwSize, IO_SIZE_TYPE_EXPLICIT);
	if( PR_FAIL(err) )
		PR_TRACE((this, prtERROR, "tm\tCannot get IO size for QB store operation, object: %S, %terr", info->m_strObjectName.data(), err));

	if( PR_SUCC(err) )
	{
		err = m_qb->StoreObject((hOBJECT)hSource, PID_ANY, &qbObj, NULL, &qbObj.m_qwObjectId);

		if( info->isBasedOn(cQBObject::eIID) )
		{
			if( err == errOBJECT_ALREADY_EXISTS )
			{
				err = m_qb->DeleteObject(qbObj.m_qwObjectId);
				err = m_qb->StoreObject((hOBJECT)hSource, PID_ANY, &qbObj, NULL, &qbObj.m_qwObjectId);
			}

			if( PR_SUCC(err) )
				hSource->set_pgOBJECT_DELETE_ON_CLOSE(cTRUE);
		}
	}

	if( PR_FAIL(err) )
		PR_TRACE((this, prtERROR, "tm\tQB failed to store object: %S, %terr", info->m_strObjectName.data(), (tERROR)err));

	if( !bOwnSource )
		err = hSource->sysCloseObject();

	if( info->isBasedOn(cQBObject::eIID) )
	{
		info->assign(qbObj, false);
	}
	else
	{
		if( err == errOBJECT_ALREADY_EXISTS )
			err = errOK;

		if( info->isBasedOn(cAskObjectAction::eIID) )
		{
			cAskObjectAction* pInfo = (cAskObjectAction*)info;
			pInfo->m_nResultAction = PR_SUCC(err) ? ACTION_OK : ACTION_CANCEL;
		}
	}
	info->m_qwQBObjectId = qbObj.m_qwObjectId;
	return err;
}

tERROR BlImpl::clean_QB(const tDATETIME* p_time_stamp) 
{
	tERROR err = errOK;
	if( PR_FAIL(err = init_qb()) )
		return err;

	tINT i, count = 0;
	if( PR_SUCC(err) )
		err = m_qb->GetCount((tDWORD*)&count, cCOUNT_TOTAL|cCOUNT_REFRESH);

	cDateTime limit(p_time_stamp);
	for(i = 0; i < count; i++)
	{
		cQBObject qbObj;
		tOBJECT_ID objectID = 0; 
		if( PR_FAIL(m_qb->GetObjectByIndex(NULL,&objectID, i, cFALSE, &qbObj)) ||
				!p_time_stamp || qbObj.m_tmTimeStamp == -1 || !qbObj.m_tmTimeStamp ||
				cDateTime(qbObj.m_tmTimeStamp) <= limit )
			if( PR_SUCC(m_qb->DeleteObject(objectID)) )
				i--, count--;
	}
	return err;
}
