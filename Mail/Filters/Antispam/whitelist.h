// whitelist.h: interface for the CWhiteList class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_WHITELIST_H__7DDA834B_DF85_4ED1_A831_EB6B30D23500__INCLUDED_)
#define AFX_WHITELIST_H__7DDA834B_DF85_4ED1_A831_EB6B30D23500__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <Prague/prague.h>
#include <Mail/structs/s_antispam.h>

#ifndef IN
#define IN
#endif
#ifndef OUT
#define OUT
#endif

enum address_type_t 
{
	at_unknown = -1, 
	at_not_found = at_unknown,
	at_white = 0, 
	at_black = 1,
};

class CAddressList
{
public:
	CAddressList();
	virtual ~CAddressList();
	tERROR Init(   IN cAntispamSettings* settings, hOBJECT parent );
	tERROR Find(   IN tSTRING     mail_address, IN tSTRING for_user, OUT address_type_t& address_type, tBOOL bFindWhite, tBOOL bFindBlack, OUT cStringObj* szFound = NULL );
	tERROR Update( IN tSTRING     mail_address, IN tSTRING for_user, IN  address_type_t  address_type );
	tERROR Delete( IN tSTRING     mail_address, IN tSTRING for_user, IN  address_type_t  address_type );
	tERROR Merge(  IN CAddressList* p_another_list );
	tERROR Save();
	tERROR Rollback();

	inline tERROR GetSettings( OUT cAntispamSettings* settings )
	{
		if ( !settings ) 
			return errPARAMETER_INVALID;
		return settings->assign(*m_settings, false);
	}
	inline tBOOL HasBeenChanged()
	{
		return m_bChanged;
	}

protected:
	hREGISTRY          m_persistance_storage;
	cAntispamSettings* m_settings;
	hOBJECT            m_parent;
	tBOOL              m_bInited;
	tBOOL              m_bChanged;

private:

};

#endif // !defined(AFX_WHITELIST_H__7DDA834B_DF85_4ED1_A831_EB6B30D23500__INCLUDED_)
