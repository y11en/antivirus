// SpamFilter.cpp: implementation of the CSpamFilter class.
//
//////////////////////////////////////////////////////////////////////

#include "common.h"
#include "DataBase.h"
#include "Message_Info.h"

#include "SpamFilter.h"
#include "smtp-filter/common/merge.h"

#include "_include/wintypes.h"
#include "_include/wintypes.h"

#include "client-filter/librules/interpreter.h"

#include <Prague/prague.h>
#include <Extract/iface/i_mailmsg.h>
#undef swap
#include <Extract/plugin/p_mailmsg.h>
#include <Extract/plugin/p_msoe.h>
#include <string>

#include <Prague/pr_oid.h>
#include "BKFHlp.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSpamFilter::CSpamFilter(CDataBase *pDataBase): m_pDataBase(pDataBase)
{
}

CSpamFilter::~CSpamFilter()
{

}

class CmpFunctor
{
public:
	CmpFunctor(envelope* env) : envelope_for_sort(env) {};
	CmpFunctor(const CmpFunctor &src) : envelope_for_sort(src.envelope_for_sort) {};
	CmpFunctor& operator=(const CmpFunctor &rhs)
	{
		if(this != &rhs)
		{
			envelope_for_sort = rhs.envelope_for_sort;
		}
		return *this;
	}
	
	CmpFunctor& operator*(){ return *this; };

	int operator()(const void *a, const void *b)
	{
		const msg_header *mh_a = (const msg_header *)a;
		const msg_header *mh_b = (const msg_header *)b;

		return strcasecmp(ENVELOPE_HEAP_STRING(envelope_for_sort, mh_a->header),
		      ENVELOPE_HEAP_STRING(envelope_for_sort, mh_b->header));
	}
private:
	envelope *envelope_for_sort;
};

int envelope_prepare_headers_KASP(envelope *e);

SPAM_TYPE CSpamFilter::CheckHeaders(MESSAGE_INFO *pMsg, OUT STATISTICS *pStatistic)
{
	if ( !m_pDataBase || !m_pDataBase->GetPDB() )
		return ST_NOT_SPAM;

	SPAM_TYPE stRet = ST_NOT_SPAM;
	envelope e; envelope_create(&e);
    bool bHasHeaders = true; 
    LPCTSTR pHeaders = pMsg->GetPropStr(pgMESSAGE_INTERNET_HEADERS);

	e.msg_size = 1; //pMsg->GetPropLong(pgMESSAGE_PART_CONTENT_LENGTH); // msgh->m_MsgSize;
    if( !pHeaders || 0 == lstrlen(pHeaders) )
        bHasHeaders = false;
    else
    {
        envelope_set_data(&e, pHeaders, lstrlen(pHeaders));
        envelope_prepare_headers_KASP(&e);
        if(e.headers.used == 0)
            bHasHeaders = false;
	}
	
	if ( bHasHeaders )
	{
		action_storage as;		
		interpreter_config ic;	
		//Init interpreter_config some structures
		memset(&ic, 0, sizeof(ic));
		memset(&as, 0, sizeof(as));
		ic.e = &e;
		ic.as = &as;
		ic.ldb = 0;
		ic.pdb = m_pDataBase->GetPDB();
		ic.msg = 0; //Memorize pointer to the message structure
		
		if ( action_storage_init(&as, &e) )
		{
			if ( 0 == local_mergesort(e.headers.array, e.headers.used, sizeof(msg_header), CmpFunctor(&e)))
			if ( 0 == action_header_modify(as.common_action, HEADER_MODIFY_ADD,"X-SpamTest-Internal-Flag-KIS2006",""))
			if ( 0 == profile_interpretate(&ic, false))
			if ( 0 == action_storage_normalize(&as, ACTIONS_STORAGE_NORMALIZE_LIBSPAMTEST))
				stRet = AnalyzeActionStorage(as, pStatistic);
			action_storage_destroy(&as);
		}
	}
	envelope_destroy(&e);
    return stRet;
}

SPAM_TYPE CSpamFilter::CheckGSG(IN MESSAGE_INFO *pMsg, OUT STATISTICS *pStatistic)
{
	if ( !m_pDataBase || !m_pDataBase->GetMD5Array() )
		return ST_NOT_SPAM;
	
    bool matched = false;
    if(m_pDataBase->GetBinaryProcessor())
    {
        MESSAGEPART_INFO* pAtt = NULL;
        for(int i = 0; ; ++i)
        {
            binary_clearall(m_pDataBase->GetBinaryProcessor());
            pAtt = pMsg->GetMessagePart(i);
			if(!pAtt)
				break;
            //Get signatures for all attachments with using binaryprocessor  
            byte buff[256]; memset(buff,0,256);
            int nAttSize = pAtt->GetPropLong(pgMESSAGE_PART_CONTENT_LENGTH);
            IStream* pData = pAtt->GetData();
            for (int buff_counter = 0; buff_counter < nAttSize; )
            {
                ULONG buff_size = 0;
                if ( SUCCEEDED(pData->Read(buff, 256, &buff_size)) )
                {
                    binary_put(m_pDataBase->GetBinaryProcessor(), buff, buff_size);
                    buff_counter += buff_size;
                }
                else
				{
					pMsg->ReleaseMessagePart(pAtt);
                    break;
				}
            }
            binary_endpart(m_pDataBase->GetBinaryProcessor());

            // Собственно, проверка по GSG
            for ( 
                unsigned int sig_num=0; 
                !matched && sig_num<m_pDataBase->GetBinaryProcessor()->md5list->used; 
                sig_num++ 
                )
            {
				int n_md5sigfind = md5sigfind(m_pDataBase->GetMD5Array(),
                                              m_pDataBase->GetBinaryProcessor()->md5list->array[sig_num].md5);
                if (
                    ( n_md5sigfind >= 0) &&
                    (m_pDataBase->GetBinaryProcessor()->md5list->array[sig_num].type > NONE) &&
                    (m_pDataBase->GetBinaryProcessor()->md5list->array[sig_num].type <= SIG_GSG7)
                    )
                    matched = true;
            }
			pMsg->ReleaseMessagePart(pAtt);
        }
    }
    if(matched)
	{
		if ( pStatistic ) pStatistic->dwFacility = SF_GSG;
        return ST_SPAM;
	}
    return ST_NOT_SPAM;
}

SPAM_TYPE CSpamFilter::CheckRecentTerms(IN MESSAGE_INFO *pMsg, OUT STATISTICS *pStatistic)
{
	if(!m_pDataBase)
		return ST_NOT_SPAM;

	PR_TRACE((0, prtIMPORTANT, "spamtst\tChecking message for recent terms..."));

	unsigned int spam_relevant_index = -1;

	// обрабатываем в зависимости от типа
	long pgId = pMsg->GetPropLong(pgPLUGIN_ID);
	if(pgId == PID_MAILMSG)
		spam_relevant_index = BKF_CheckMAILMSG(pMsg, m_pDataBase->GetBKFBase());
	else if(pgId == PID_MSOE)
		spam_relevant_index = BKF_CheckMSOE(pMsg, m_pDataBase->GetBKFBase());
	else
		PR_ASSERT(0);

	if(spam_relevant_index != -1)
	{
		if(pStatistic)
			pStatistic->dwFacility = SF_RECENTTERMS;
		return ST_SPAM;
	}
	return ST_NOT_SPAM;
}

HRESULT CSpamFilter::Process(IN BOOL bUsePDB, IN BOOL bUseGSG, IN BOOL bUseRecentTerms,
								IN MESSAGE_INFO* pMsg, OUT SPAM_TYPE* pSpamType, OUT STATISTICS* pStatistic)
{
	if(!pSpamType)
		return E_FAIL;
	SPAM_TYPE stRecentTerms = bUseRecentTerms ? CheckRecentTerms(pMsg, pStatistic) : ST_NOT_SPAM;
	SPAM_TYPE stPDB = bUsePDB && stRecentTerms == ST_NOT_SPAM ? CheckHeaders(pMsg, pStatistic) : ST_NOT_SPAM;
	SPAM_TYPE stGSG = bUseGSG && stRecentTerms == ST_NOT_SPAM && stPDB == ST_NOT_SPAM ? CheckGSG(pMsg, pStatistic) : ST_NOT_SPAM;
	if(stPDB == ST_SPAM || stGSG == ST_SPAM || stRecentTerms == ST_SPAM)
		*pSpamType = ST_SPAM;
	else if(stPDB == ST_PROBABLE_SPAM || stGSG == ST_PROBABLE_SPAM || stRecentTerms == ST_PROBABLE_SPAM)
		*pSpamType = ST_PROBABLE_SPAM;
	else
		*pSpamType = ST_NOT_SPAM;
	return S_OK;
}

SPAM_TYPE CSpamFilter::AnalyzeActionStorage(IN action_storage& as, OUT STATISTICS *pStatistic)
{
    //Analyze statuses 
	SPAM_TYPE status = ST_NOT_SPAM; //Not detected 
    for(size_t j = 0; j < as.common_action->headers.used; j++)
	{
		if((as.common_action->headers.array[j].type == HEADER_MODIFY_REMOVE) && (strcmp(as.common_action->headers.array[j].header, "X-SpamTest-Status") == 0)) 
        {
            status = ST_NOT_SPAM; //Not detected
            continue;
        }

        if((as.common_action->headers.array[j].type == HEADER_MODIFY_REMOVE) || (as.common_action->headers.array[j].type == HEADER_MODIFY_NONE))
			continue;

		//Get message status
		if(strcmp(as.common_action->headers.array[j].header, "X-SpamTest-Status") == 0)
		{
			if(strcmp(as.common_action->headers.array[j].value, "SPAM") == 0)
				status = ST_SPAM; //SPAM
			else if(_stricmp(as.common_action->headers.array[j].value, "Probable spam") == 0)
				status = ST_PROBABLE_SPAM; //Probable spam
			else if(_stricmp(as.common_action->headers.array[j].value, "OBSCENE") == 0)
				status = ST_SPAM; //Obscene
			else if(_stricmp(as.common_action->headers.array[j].value, "FORMAL") == 0)
				status = ST_SPAM; //Formal
		}
	}
	if ( pStatistic && (status!=ST_NOT_SPAM) )
		pStatistic->dwFacility = SF_PDB;
    return status;
}
