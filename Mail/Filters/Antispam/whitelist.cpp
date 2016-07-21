// whitelist.cpp: implementation of the CAddressList class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "whitelist.h"
#include "BWFind.h"

CAddressList::CAddressList():
	m_bInited(cFALSE),
	m_bChanged(cFALSE)
{
}

CAddressList::~CAddressList()
{
}

tERROR CAddressList::Init( cAntispamSettings* settings, hOBJECT parent )
{
	cERROR err = errOK;
	if ( m_bInited )
		return errOBJECT_ALREADY_EXISTS;

	m_settings = settings;
	m_parent = parent;
	m_bInited = cTRUE;

	return err;
}

tERROR CAddressList::Find( tSTRING mail_address, tSTRING for_user, OUT address_type_t& address_type, tBOOL bFindWhite, tBOOL bFindBlack, OUT cStringObj* szFound )
{
	cERROR err = errOK;

	address_type = at_not_found;
	if( cAsBwList * pUserSett = (cAsBwList *)m_settings->settings_byuser(cStringObj(for_user), true) )
	{
		tINT iFoundPos = -1;
		std::set<tSIZE_T> found_indices;
		if(bFindWhite &&
			BWFind::AsBwListFindPattern(pUserSett->m_aWhiteAddrs, mail_address, 0, found_indices) &&
			!found_indices.empty())
		{
			PR_ASSERT(found_indices.size() == 1);
			address_type = at_white;
			if(szFound)
				*szFound = pUserSett->m_aWhiteAddrs[*found_indices.begin()].m_sRule;
			return err;
		}
		if ( bFindBlack && 
			BWFind::AsBwListFindPattern(pUserSett->m_aBlackAddrs, mail_address, 0, found_indices) &&
			!found_indices.empty())
		{
			PR_ASSERT(found_indices.size() == 1);
			address_type = at_black;
			if(szFound)
				*szFound = pUserSett->m_aBlackAddrs[*found_indices.begin()].m_sRule;
			return err;
		}
	}

	return err;
}

tUINT RemoveFromWhiteAddrs(cAsBwListRule& rule, cAsBwListRules& hWhiteRules)
{
	// В белом списке мы снимаем галку m_bEnabled со всех элементов, совпадающих по маске с rule
	//	return hWhiteRules.remove_by_value(rule);
	tUINT ind_ret = cAsBwListRules::npos;
	for (tUINT i = 0; i < hWhiteRules.count(); i++)
	{
		if ( cSTRING_COMP_EQ == rule.m_sRule.compare( hWhiteRules.at(i).m_sRule, fSTRING_COMPARE_CASE_INSENSITIVE ))
		{
			hWhiteRules.at(i).m_bEnabled = cFALSE;
			ind_ret = i;
		}
	}
	return ind_ret;
}

tUINT RemoveFromBlackAddrs(cAsBwListRule& rule, cAsBwListRules& hBlackRules)
{
	// В черном списке мы удаляем все элементы, полностью совпадающие rule.m_sRule
	tUINT ind_ret = cAsBwListRules::npos;
	for (tUINT i = 0; i < hBlackRules.count(); i++)
	{
		if ( cSTRING_COMP_EQ == rule.m_sRule.compare(hBlackRules.at(i).m_sRule, fSTRING_COMPARE_CASE_INSENSITIVE ))
		{
			hBlackRules.remove(i);
			ind_ret = i;
			i--;
		}
	}
	return ind_ret;
}

tUINT AddToAddr(cAsBwListRule& rule, cAsBwListRules& hRules)
{
	// Если в списке есть элементы, полностью совпадающие с rule.m_sRule, просто выставим им галку m_bEnabled
	// Если такого элемента нет, добавим его
	//	if ( -1 == hRules.find(rule) )
	//		hRules.push_back(rule);
	//	return errOK;
	tUINT ind_ret = cAsBwListRules::npos;
	for (tUINT i = 0; i < hRules.count(); i++)
	{
		if ( cSTRING_COMP_EQ == rule.m_sRule.compare( hRules.at(i).m_sRule, fSTRING_COMPARE_CASE_INSENSITIVE ))
		{
			hRules.at(i).m_bEnabled = cTRUE;
			ind_ret = i;
		}
	}
	if ( ind_ret == cAsBwListRules::npos )
		hRules.push_back(rule);
	return ind_ret;
}


tERROR CAddressList::Update( tSTRING mail_address, tSTRING for_user, IN address_type_t address_type )
{
	cAsBwList * pUserSett = (cAsBwList *)m_settings->settings_byuser(cStringObj(for_user), true);
	if ( !pUserSett )
		return errNOT_OK;

	cAsBwListRule rule(mail_address, cTRUE, cTRUE);	
	if ( rule.m_sRule.empty() )
		return errPARAMETER_INVALID;
	switch (address_type) 
	{
	case at_white:
		AddToAddr(rule, pUserSett->m_aWhiteAddrs);
		RemoveFromBlackAddrs(rule, pUserSett->m_aBlackAddrs);
		break;
	case at_black:
		AddToAddr(rule, pUserSett->m_aBlackAddrs);
		RemoveFromWhiteAddrs(rule, pUserSett->m_aWhiteAddrs);
		break;
	}

	return Save();
}

tERROR CAddressList::Delete( tSTRING mail_address, tSTRING for_user, IN address_type_t address_type )
{
	if ( cAsBwList * pUserSett = (cAsBwList *)m_settings->settings_byuser(cStringObj(for_user), true) )
	{
		cAsBwListRule rule(mail_address, cTRUE, cTRUE);
		if ( rule.m_sRule.empty() )
			return errPARAMETER_INVALID;
		switch (address_type) 
		{
		case at_unknown: // To delete all
			RemoveFromWhiteAddrs(rule, pUserSett->m_aWhiteAddrs);
			RemoveFromBlackAddrs(rule, pUserSett->m_aBlackAddrs);
			break;
		case at_white:
			RemoveFromWhiteAddrs(rule, pUserSett->m_aWhiteAddrs);
			break;
		case at_black:
			RemoveFromBlackAddrs(rule, pUserSett->m_aBlackAddrs);
			break;
		}
	}

	return Save();
}

tERROR CAddressList::Merge( CAddressList* p_another_list )
{
	cERROR err = errOK;
	cAntispamSettings settings;

	err = p_another_list->GetSettings(&settings);
	if ( PR_FAIL(err) )
		return err;

	for ( tULONG ulAnotherUser = 0; ulAnotherUser < settings.m_users.size(); ulAnotherUser++ )
	{
		cAsBwList * pAnotherUserSett = (cAsBwList *)settings.m_users.at(ulAnotherUser).m_settings.ptr_ref();
		if( !pAnotherUserSett )
			continue;

		cAsBwList * pUserSett = (cAsBwList *)m_settings->settings_byuser(settings.m_users.at(ulAnotherUser).m_sUserId, true, pAnotherUserSett);
		if( !pUserSett )
			continue;

		tULONG ulAnotherAddress = 0;
		for ( ulAnotherAddress = 0; ulAnotherAddress < pAnotherUserSett->m_aWhiteAddrs.count(); ulAnotherAddress++ )
			AddToAddr(pAnotherUserSett->m_aWhiteAddrs.at(ulAnotherAddress), pUserSett->m_aWhiteAddrs);
		for ( ulAnotherAddress = 0; ulAnotherAddress < pAnotherUserSett->m_aBlackAddrs.count(); ulAnotherAddress++ )
			AddToAddr(pAnotherUserSett->m_aBlackAddrs.at(ulAnotherAddress), pUserSett->m_aBlackAddrs);
	}

	return Save();
}

tERROR CAddressList::Save()
{
	m_bChanged = cTRUE;
	return errOK;
}

tERROR CAddressList::Rollback()
{
	cERROR err = errOK;

	for ( tULONG ulUser = 0; ulUser < m_settings->m_users.size(); ulUser++ )
	{
		cAsBwList * pUserSett = (cAsBwList *)m_settings->m_users.at(ulUser).m_settings.ptr_ref();
		if ( !pUserSett ) continue;
		tULONG ulAddress = 0;
		for ( ulAddress = 0; ulAddress < pUserSett->m_aWhiteAddrs.count(); ulAddress++ )
			if ( pUserSett->m_aWhiteAddrs.at(ulAddress).m_bNew )
				pUserSett->m_aWhiteAddrs.remove(ulAddress--);
		for ( ulAddress = 0; ulAddress < pUserSett->m_aBlackAddrs.count(); ulAddress++ )
			if ( pUserSett->m_aBlackAddrs.at(ulAddress).m_bNew )
				pUserSett->m_aBlackAddrs.remove(ulAddress--);
	}

	return err;
}