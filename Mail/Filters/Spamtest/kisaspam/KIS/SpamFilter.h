// SpamFilter.h: interface for the CSpamFilter class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SPAMFILTER_H__DFBF9B2C_F1DD_42AC_99C4_43E0AEC2C502__INCLUDED_)
#define AFX_SPAMFILTER_H__DFBF9B2C_F1DD_42AC_99C4_43E0AEC2C502__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "kis_face.h"
#include "client-filter/librules/interpreter.h"
#include "message_info.h"

class   CDataBase;
class   MESSAGE_INFO;

enum SPAM_TYPE      {ST_NOT_SPAM = 0, ST_SPAM = 1, ST_PROBABLE_SPAM = 2};
enum SPAM_FACILITY  {SF_PDB = 1, SF_GSG = 2, SF_RECENTTERMS = 3};
// enum corresopnded to:
// Return Values:	= 0 - Not spam
//					> 0 - Category of spam:
//					= 1 - Spam
//					= 2 - Probable spam
//					= 3 - Obscene or Suspicious 
//					= 4 - Formal

class CSpamFilter  
{
    CDataBase   *m_pDataBase;
public:
	CSpamFilter(CDataBase *pDataBase);
	virtual ~CSpamFilter();
    // Process - return verdict about message
	HRESULT Process (
		IN      BOOL                bUsePDB,
		IN      BOOL                bUseGSG,
		IN      BOOL                bUseRecentTerms,
        IN      MESSAGE_INFO*       pMsg, 
        OUT     SPAM_TYPE*          pSpamType, 
        OUT     STATISTICS*         pStatistic=NULL
        );
protected:
    SPAM_TYPE CheckHeaders        (IN MESSAGE_INFO *pMsg, OUT STATISTICS *pStatistic);
    SPAM_TYPE CheckGSG            (IN MESSAGE_INFO *pMsg, OUT STATISTICS *pStatistic);
	SPAM_TYPE CheckRecentTerms    (IN MESSAGE_INFO *pMsg, OUT STATISTICS *pStatistic);
    SPAM_TYPE AnalyzeActionStorage(IN action_storage& as, OUT STATISTICS *pStatistic);
};


#endif // !defined(AFX_SPAMFILTER_H__DFBF9B2C_F1DD_42AC_99C4_43E0AEC2C502__INCLUDED_)
