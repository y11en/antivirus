/*!
*		
*		
*		(C) 2002 Kaspersky Lab 
*		
*		\file	BayesTable.cpp
*		\brief	Функционал BayesTableer'а
*		
*		\author Vitaly DVi Denisov
*		\date	12.04.2004 14:35:10
*		
*		
*		
*/		
#pragma warning( disable : 4786 )

#include "bayestable.h"
#include "md5.h"
#ifndef USE_FISHER
	#include "../bayesfilter.h"
#else
	#include "../bfilter.h"
#endif//USE_FISHER

struct CSFDBData
{
	unsigned __int64 hKey;
	CSFDBRecordData data;
	tDWORD   int_data[2];
};

int DeleteRecords(hSECUREFILEDB hDataBase, CSFDBData* dataArray, tDWORD dwArraySize, tBOOL bDeleteUnknownOnly)
{
	tDWORD dwCounter = 0;
	CSFDBRecordData dataSpamHam;
	unsigned __int64  keySpamHam;
	int iDeletedCount = 0;

	if ( CBayesFilter::GetData(">> SPAMHAM >>", dataSpamHam, NULL, hDataBase, &keySpamHam) )
	{
		for ( dwCounter = 0; dwCounter < dwArraySize; dwCounter++ )
		{
			if ( keySpamHam == dataArray[dwCounter].hKey )
				continue;
			else
			{
				long double weight = dataArray[dwCounter].data.GetWeight(dataSpamHam);
				if ( !bDeleteUnknownOnly || abs(weight - 0.5) < 0.1 ) // Удаляем только малозначащие записи, если указано (но не более 10)
				{
					dataSpamHam.ulSpamCount -= dataArray[dwCounter].data.ulSpamCount;
					dataSpamHam.ulHamCount -= dataArray[dwCounter].data.ulHamCount;
					hDataBase->DeleteRecord((tVOID*)&dataArray[dwCounter].hKey);
					hDataBase->UpdateRecord((tVOID*)&keySpamHam, &dataSpamHam);
					if ( iDeletedCount++ > 10 )
						break;
				}
			}
		}
	}
	return iDeletedCount;
}

tERROR pr_call fMsgReceive( hOBJECT _this, tDWORD p_msg_cls_id, tDWORD p_msg_id, hOBJECT p_send_point, hOBJECT p_ctx, hOBJECT p_receive_point, tVOID* p_par_buf, tDWORD* p_par_buf_len )
{
	cERROR err = errOK;
	
	if ( 
		p_msg_cls_id == pmcSFDB && 
		p_msg_id == pmUPDATE_CLUSTER_FULL 
		)
	{
		hSECUREFILEDB hDataBase = (hSECUREFILEDB)p_send_point;
		if ( hDataBase )
		{
			tDWORD dwArraySize = *p_par_buf_len / sizeof(CSFDBData);
			CSFDBData* dataArray = (CSFDBData*)p_par_buf; 
			int iDeletedCount = DeleteRecords( hDataBase, dataArray, dwArraySize, cTRUE);
			if ( !iDeletedCount )
				iDeletedCount = DeleteRecords( hDataBase, dataArray, dwArraySize, cFALSE);
			err = errOK_DECIDED;
		}
	}
	
	return err;
}


CBayesTable::CBayesTable():
	m_hMsgReceiver(NULL)
{
}

CBayesTable::~CBayesTable()
{
	if ( m_hMsgReceiver )
	{
//		if ( m_hDataBase )
//			m_hMsgReceiver->sysUnregisterMsgHandler( pmcSFDB, m_hDataBase );
		m_hMsgReceiver->sysCloseObject();
		m_hMsgReceiver = NULL;
	}
}

tERROR CBayesTable::Init(IN const hOBJECT parent, IN tCHAR* sfdb_path, IN tCHAR* sfdb_save_to)
{
	tERROR error = errOK;

	if ( PR_SUCC(error = CSFDBTableForBayes::Init(parent, sfdb_path, sfdb_save_to)) ) 
	{
		error = g_root->sysCreateObject( (hOBJECT*)&m_hMsgReceiver, IID_MSG_RECEIVER );
		if ( PR_SUCC(error) ) 
			error = m_hMsgReceiver->set_pgRECEIVE_PROCEDURE((tFUNC_PTR)fMsgReceive);
		if ( PR_SUCC(error) ) 
			error = m_hMsgReceiver->sysCreateObjectDone();
		if ( PR_FAIL(error) )
			m_hMsgReceiver = NULL;
		else
			error = m_hMsgReceiver->sysRegisterMsgHandler(
						pmcSFDB, 
						rmhDECIDER, 
						m_hDataBase, 
						IID_ANY, PID_ANY, IID_ANY, PID_ANY 
						);
		if ( PR_FAIL(error) && m_hMsgReceiver )
		{
			m_hMsgReceiver->sysCloseObject();
			m_hMsgReceiver = NULL;
		}
	}
	return error;
}

