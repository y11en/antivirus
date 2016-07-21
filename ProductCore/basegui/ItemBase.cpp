// ItemBase.cpp: implementation of the CItemBased class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include <ProductCore/ItemBase.h>
#include <ProductCore/ItemProps.h>
#include "../../Prague/IniLib/IniLib.h"

#include <Prague/iface/e_loader.h>

//////////////////////////////////////////////////////////////////////

tBOOL osd_InflateRect(RECT * rc, int x, int y)
{
	rc->left   -= x;
	rc->right  += x;
	rc->top    -= y;
	rc->bottom += y;
	return cTRUE;
}

tBOOL osd_OffsetRect(RECT * rc, int x, int y)
{
	rc->left   += x;
	rc->right  += x;
	rc->top    += y;
	rc->bottom += y;
	return cTRUE;
}

tBOOL osd_PtInRect(RECT * rc, POINT& pt)
{
	if( pt.x < rc->left || pt.x > rc->right )
		return cFALSE;
	if( pt.y < rc->top || pt.y > rc->bottom )
		return cFALSE;
	return cTRUE;
}

int cStrData::cmp(LPCSTR str, tDWORD len) const
{
	if( n == -1 && len == -1 )
		return strcmp(p, str);
	
	if( len == -1 )
		len = strlen(str);
	tDWORD n1 = (n == -1) ? strlen(p) : n;

	if( n1 != len )
		return n1 < len ? -1 : 1;
	return memcmp(p, str, n1);
}

void _Str_TruncateSpaces(tString& str);

//////////////////////////////////////////////////////////////////////
// CItemBase

static CItemBase * _ItemBase_GetNextEnumItem(CItemBase * pOwner, CItemBase * pEnumItem, bool bBackward = false)
{
	CItemBase * p = pEnumItem;
	
	tDWORD nEnumFlags = bBackward ? ENUMITEMS_BACKWARD : 0;
	for(; p = pOwner->EnumNextItem(p, nEnumFlags);)
	{
		nEnumFlags &= ~ENUMITEMS_SKIPCHILDREN;

		if( p->m_nFlags & STYLE_ENUM )
		{
			nEnumFlags |= ENUMITEMS_SKIPCHILDREN;
			continue;
		}

		if( !(p->m_nFlags & STYLE_ENUM_VALUE) )
			continue;

		break;
	}

	return p;
}

//////////////////////////////////////////////////////////////////////

CItemBase::CItemBase()
{
	m_pRoot = NULL;
	m_pOwner = NULL;
	m_pParent = NULL;
	m_pSink = NULL;
	m_pFont = NULL;
	m_pBorder = NULL;
	m_nFlags = 0;
	m_nState = 0;
	m_nIconAlign = 0;
	m_nTextFlags = 0;
	m_pUserData = NULL;
	m_nPos = 0;
	m_nOrder = -1;
	m_nTabGroup = -1;
	m_pBgCtx = NULL;
	m_pOverrides = NULL;
	m_pEditData = NULL;
	m_pBinding = NULL;
	m_pProps = NULL;

	memset(&m_ptOffset, 0, sizeof(m_ptOffset));
	memset(&m_rcShift, 0, sizeof(m_rcShift));
	memset(&m_rcMargin, 0, sizeof(m_rcMargin));
	memset(&m_szSize, 0, sizeof(m_szSize));
	memset(&m_szSizeMin, 0, sizeof(m_szSizeMin));
	memset(&m_szSizeMax, 0, sizeof(m_szSizeMax));
	memset(&m_ptResizeScale, 0, sizeof(m_ptResizeScale));
	memset(&m_szSizeWoScale, 0, sizeof(m_szSizeWoScale));

	m_ref = 1;
	m_lockUpdate = 0;

	m_bHideExpanded = 0;
	m_bHideCollapsed = 0;
}

void CItemBase::Cleanup()
{
	if( m_pParent )
		m_pParent->RemoveItem(this);
	
	if( m_pBinding )
		delete m_pBinding, m_pBinding = NULL;

	Clear();
	AttachSink(NULL, false);

	if( m_pRoot->m_pSelected == this )
		m_pRoot->m_pSelected = NULL;

	if( m_pRoot->m_pTrackItem == this )
		m_pRoot->m_pTrackItem = NULL;
	
	if( !m_strTipText.empty() )
		m_pRoot->EnableToolTip(this, false);
	
	m_pRoot->AnimateItem(this, false);

	if( m_pBgCtx )
		delete m_pBgCtx, m_pBgCtx = NULL;
	
	if( m_pIcon && (m_nFlags & STYLE_DESTROY_ICON) )
		m_pIcon->Destroy(), m_pIcon = NULL;

	m_pIcon.Clean();
	m_pBackground.Clean();

	if( m_pOverrides )
		delete m_pOverrides, m_pOverrides = NULL;
	if( m_pEditData )
		m_pEditData->Destroy(), m_pEditData = NULL;

	if( m_pProps )
		delete m_pProps, m_pProps = NULL;
}

void CItemBase::AddRef()  { PrInterlockedIncrement((tLONG *)&m_ref); }
void CItemBase::Release() { if( !PrInterlockedDecrement((tLONG *)&m_ref) ) delete this; }
void CItemBase::Destroy() { if( m_pRoot ) Cleanup(), m_pRoot = NULL; Release(); }

void CItemBase::Clear()
{
	Items aItems(m_aItems);
	m_aItems.clear();

	for(size_t i = 0; i < aItems.size(); i++)
	{
		aItems[i]->m_pParent = NULL;
		aItems[i]->Destroy();
	}
}

void CItemBase::SendEvent(tDWORD nEventId, cSerializable* pData, bool bBroadcast)
{
	if( m_pSink )
		m_pSink->OnEvent(nEventId, pData);

	if( bBroadcast )
	{
		for(size_t i = 0; i < m_aItems.size(); i++)
			m_aItems[i]->SendEvent(nEventId, pData, bBroadcast);
	}
	else
	{
		if(m_pParent)
			m_pParent->SendEvent(nEventId, pData, bBroadcast);
	}
}

void CItemBase::SaveSettings(cAutoRegKey& pKey)
{
	for(size_t i = 0; i < m_aItems.size(); i++)
		m_aItems[i]->SaveSettings(pKey);

	if( !(m_nFlags & STYLE_REMEMBER_DATA) || !OnCanRememberData(this, true) )
		return;

	tString strKey; GetOwnerRelativePath(strKey, GetOwner(true), true, "Data");
	switch( GetType() )
	{
	case tid_STRING_OBJ:
	case tid_UNK:
	{
		cVariant data; GetValue(data);
		pKey.set_strobj(strKey.c_str(), data.ToStringObj());
	} break;

	case tid_BOOL:
	case tid_DWORD:
	{
		cVariant data; GetValue(data);
		pKey.set_dword(strKey.c_str(), data.ToDWORD());
	} break;
	
	case tid_QWORD:
	{
		cVariant data; GetValue(data);
		pKey.set_qword(strKey.c_str(), data.ToQWORD());
	} break;
	}
}

void CItemBase::LoadSettings(cAutoRegKey& pKey)
{
	for(size_t i = 0; i < m_aItems.size(); i++)
		m_aItems[i]->LoadSettings(pKey);

	if( !(m_nFlags & STYLE_REMEMBER_DATA) || !OnCanRememberData(this, false) )
		return;

	tString strKey; GetOwnerRelativePath(strKey, GetOwner(true), true, "Data");
	switch( GetType() )
	{
	case tid_STRING_OBJ:
	case tid_UNK:
	{
		cStrObj strData;
		if( PR_SUCC(pKey.get_strobj(strKey.c_str(), strData)) )
			SetValue(cVariant(strData.data()));
		else
			return;
	} break;

	case tid_BOOL:
	case tid_DWORD:
	{
		tDWORD nData = 0;
		if( PR_SUCC(pKey.get_dword(strKey.c_str(), nData)) )
			SetInt(nData);
	} break;
	
	case tid_QWORD:
	{
		tQWORD nData = 0;
		if( PR_SUCC(pKey.get_qword(strKey.c_str(), nData)) )
			SetInt(nData);
	} break;
	}

	SetChangedData();
}

void CItemBase::_InitOverrides(CItemPropVals& prp)
{
	if( !prp.IsValid() )
		return;

	if( !m_pOverrides )
		m_pOverrides = new Overrides();
	else
		m_pOverrides->clear();
	
	__InitOverrides(*m_pOverrides, prp);
}

void CItemBase::__InitOverrides(Overrides& _ovrds, CItemPropVals& prp, tString& strOverId)
{
	tDWORD nPrevIdSize = strOverId.size();
	
	CItemProps _Params(prp.GetStr(), TOR_tString, NULL, IPV_F_SINGLE);
	for(int p = 0, n = _Params.Count(); p < n; p++)
	{
		Override& _o = _ovrds.push_back();

		CItemPropVals& v = _Params.GetByIdx(p);

		LPCSTR strSubstPrp[] = {STR_PID_SUBST, STR_PID_TEXTSUBST, STR_PID_TIPTEXTSUBST, NULL};
		CItemProps _Props(v.GetStr(), _o.m_strProps, strSubstPrp);
		_Str_TruncateSpaces(_o.m_strProps);

		_o.m_strId = v.GetName();
		if( nPrevIdSize )
			strOverId += GUI_STR_AUTOSECT_SEP_STR;
		strOverId += _o.m_strId;
		
		__InitOverrides(_o.m_aItems, _Props.Get(STR_PID_SUBST), strOverId);

		// Получение текста
		{
			CItemPropVals& prpTextSubst = _Props.Get(STR_PID_TEXTSUBST);
			if( prpTextSubst.IsValid() )
				_o.m_strText = prpTextSubst.Get();
			else if( m_pParent )
			{
				tString strLocId;
				strLocId += m_strItemId;
				strLocId += GUI_STR_AUTOSECT_SEP_STR;
				strLocId += strOverId;
				m_pRoot->GetString(_o.m_strText, PROFILE_LOCALIZE, m_pParent->m_strSection.c_str(), strLocId.c_str(), "", (m_pRoot->m_nGuiFlags & GUIFLAG_EDITOR) ? GETSTRING_RAW : m_pParent);
			}
		}

		// Получение текста тултипа
		{
			CItemPropVals& prpTipTextSubst = _Props.Get(STR_PID_TIPTEXTSUBST);
			if( prpTipTextSubst.IsValid() )
				_o.m_strTipText = prpTipTextSubst.Get();
			else if( m_pParent )
			{
				tString strLocId;
				strLocId += m_strItemId;
				strLocId += GUI_STR_AUTOSECT_SEP_STR;
				strLocId += strOverId;
				strLocId += ".tt";
				m_pRoot->GetString(_o.m_strTipText, PROFILE_LOCALIZE, m_pParent->m_strSection.c_str(), strLocId.c_str(), "", (m_pRoot->m_nGuiFlags & GUIFLAG_EDITOR) ? GETSTRING_RAW : m_pParent);
			}
		}

		strOverId.erase(nPrevIdSize);
	}
}

void CItemBase::_GetOverridesPropString(Overrides& _ovrds, tString& str, bool bText)
{
	CItemProps _Params;
	for(int i = 0, n = _ovrds.size(); i < n; i++)
	{
		Override& _o = _ovrds[i];

		tString& strProps = _Params.Get(_o.m_strId.c_str(), true).Get(0, true);
		strProps += _o.m_strProps;
		
		if( bText && _o.m_strText.size() )
		{
			CItemPropVals prpSubstText;
			prpSubstText.Get(0, true) = _o.m_strText;
			prpSubstText.DeParse(strProps, STR_PID_TEXTSUBST);
		}
		
		_GetOverridesPropString(_o.m_aItems, strProps);
	}

	CItemProps _OverrideProp; _Params.DeParse(_OverrideProp.Get(STR_PID_SUBST, true).Get(0, true), IPV_F_VOIDPROP);
	_OverrideProp.DeParse(str);
}

void CItemBase::_GetOverride(CItemBase * pExtItem, tString& strId, CItemProps& _Props, tString& strSection)
{
	if( !pExtItem )
		return;

	Override * _over = pExtItem->_GetOverrideItem(strId);

	CItemBase * pOverridesOwner = pExtItem->m_pParent ? pExtItem->m_pParent->GetByFlagsAndState(STYLE_EXT_ITEM, ISTATE_ALL, NULL, gbfasUpToParent) : NULL;
	if( !pOverridesOwner )
		return;

	if( _over )
	{
		// Добавление переопределенных свойств
		if( _over->m_strProps.size() )
		{
			tString strProps = _over->m_strProps;
			if( m_pRoot->m_nGuiFlags & GUIFLAG_EDITOR )
				m_pRoot->_GetString(strProps, PROFILE_LAYOUT, pOverridesOwner->m_strSection.c_str(), "", pOverridesOwner);
			
			tString strSectionOver;
			CItemProps _PropsOver; _PropsOver.Parse(strProps.c_str(), strSectionOver);

			if( _ExtractSection(strSectionOver, strSection) )
				_Props.Parse(m_pRoot->GetString(TOR_tString, PROFILE_LAYOUT, strSection.c_str(), STR_SELF, "", pOverridesOwner), TOR(tString,()), NULL, IPV_F_APPEND);

			_Props.Assign(_PropsOver, IPV_F_APPEND|IPV_F_MOVE);
		}

		// Попытка получить переопределенный текст
		if( _over->m_strText.size() )
		{
			tString& strText = _Props.Get(STR_PID_TEXT, true).Get(0, true);
			strText = _over->m_strText;
			if( m_pRoot->m_nGuiFlags & GUIFLAG_EDITOR )
				m_pRoot->_GetString(strText, PROFILE_LAYOUT, pOverridesOwner->m_strSection.c_str(), "", pOverridesOwner);
		}

		// Попытка получить переопределенный тултип текст
		if( _over->m_strTipText.size() )
		{
			tString& strTipText = _Props.Get(STR_PID_TIPTEXT, true).Get(0, true);
			strTipText = _over->m_strTipText;
			if( m_pRoot->m_nGuiFlags & GUIFLAG_EDITOR )
				m_pRoot->_GetString(strTipText, PROFILE_LAYOUT, pOverridesOwner->m_strSection.c_str(), "", pOverridesOwner);
		}
	}

	if( !(pExtItem->m_nFlags & STYLE_EXTPRM) )
		return;

	if( pExtItem->m_strItemId != STR_SELF )
	{
		strId.insert(0, GUI_STR_AUTOSECT_SEP_STR);
		strId.insert(0, pExtItem->m_strItemId.c_str());
	}
	
	_GetOverride(pOverridesOwner, strId, _Props, strSection);
}

CItemBase::Override * CItemBase::__GetOverrideItem(Overrides& _ovrds, LPSTR strOverId, bool bCreateIfNotextst)
{
	tDWORD nSep = -1;
	{
		for(tDWORD i = 0; strOverId[i]; i++)
		{
			if( strOverId[i] == GUI_STR_AUTOSECT_SEP )
			{
				nSep = i;
				break;
			}
		}
	}

	if( nSep != -1 )
		strOverId[nSep] = 0;

	Override * pOvrd = NULL;
	for(tDWORD i = 0, n = _ovrds.size(); i < n; i++)
	{
		Override& _o = _ovrds[i];
		if( !strcmp(_o.m_strId.c_str(), strOverId) )
		{
			pOvrd = &_o;
			break;
		}
	}

	if( !pOvrd && bCreateIfNotextst )
	{
		pOvrd = &_ovrds.push_back();
		pOvrd->m_strId = strOverId;
	}
	
	if( nSep == -1 )
		return pOvrd;
	
	strOverId[nSep] = GUI_STR_AUTOSECT_SEP;
	return pOvrd ? __GetOverrideItem(pOvrd->m_aItems, strOverId + nSep + 1, bCreateIfNotextst) : NULL;
}

CItemBase::Override * CItemBase::_GetOverrideItem(tString& strOverId, bool bCreateIfNotextst)
{
	if( strOverId.empty() )
		return NULL;
	
	if( !m_pOverrides )
	{
		if( !bCreateIfNotextst )
			return NULL;

		m_pOverrides = new Overrides();
	}

	return __GetOverrideItem(*m_pOverrides, &strOverId[0], bCreateIfNotextst);
}

LPCSTR CItemBase::LoadLocString(tString& str, LPCSTR strId, bool bRaw, bool * bFromGlobal)
{
	CItemBase * pLocOwner = GetByFlagsAndState(STYLE_EXT_ITEM, ISTATE_ALL, NULL, gbfasUpToParent);
	if( pLocOwner )
	{
		bool bSimple = true;
		if( pLocOwner != m_pParent )
			for(tDWORD i = 0; i < pLocOwner->m_aItems.size(); i++)
				if( pLocOwner->m_aItems[i]->m_strItemId == strId )
				{
					bSimple = false;
					break;
				}
		
		if( bSimple )
			m_pRoot->GetString(str, PROFILE_LOCALIZE, pLocOwner->m_strSection.c_str(), strId, "", bRaw ? GETSTRING_RAW : NULL);
		else if( m_pParent )
		{
			tString strLocId = m_pParent->m_strSection;
			strLocId += GUI_STR_AUTOSECT_SEP_STR;
			strLocId += strId;
			m_pRoot->GetString(str, PROFILE_LOCALIZE, pLocOwner->m_strSection.c_str(), strLocId.c_str(), "", bRaw ? GETSTRING_RAW : NULL);
		}
	}

	if( str.size() )
	{
		if( bFromGlobal )
			*bFromGlobal = false;
		return str.c_str();
	}

	if( bFromGlobal )
		*bFromGlobal = true;
	return m_pRoot->GetString(str, PROFILE_LOCALIZE, NULL, strId, "", bRaw ? GETSTRING_RAW : NULL);
}

bool CItemBase::Resolve(tDWORD nFmtId, LPCSTR strMacro, tString& strResult)
{
	if( CItemBase * pLocOwner = GetByFlagsAndState(STYLE_EXT_ITEM, ISTATE_ALL, NULL, gbfasUpToParent) )
		m_pRoot->GetString(strResult, nFmtId, pLocOwner->m_strSection.c_str(), strMacro, "", pLocOwner);

	return strResult.empty() ? m_pRoot->Resolve(nFmtId, strMacro, strResult) : true;
}

tTYPE_ID CItemBase::GetType()
{
	if( m_pSink && (m_pSink->OnBindingFlags() & ITEMBIND_F_VALUE) )
		return m_pSink->OnGetType();
	return (m_nFlags & (STYLE_ENUM|STYLE_ENUM_VALUE|STYLE_CHECKBOX_LIKE)) ? tid_UNK : tid_STRING_OBJ;
}

bool CItemBase::SetType(tTYPE_ID nType)
{
	if( m_pSink )
		return m_pSink->OnSetType(nType);
	return CValueBase::SetType(nType);
}

bool CItemBase::IsVector()
{
	if( m_pSink )
		return m_pSink->OnIsVector();
	return CValueBase::IsVector();
}

bool CItemBase::IsOut()
{
	if( m_pSink && (m_pSink->OnBindingFlags() & ITEMBIND_F_VALUE) )
		return m_pSink->OnIsOut();
	return !!(m_nFlags & (STYLE_ENUM|STYLE_CHECKBOX_LIKE));
}

bool CItemBase::SetValue(const cVariant &pValue)
{
	if( m_pSink && (m_pSink->OnBindingFlags() & ITEMBIND_F_VALUE) )
		return m_pSink->OnSetValue(pValue);

	if( GetType() == tid_STRING_OBJ )
		return SetText(variant_strref(pValue).c_str());

	if( m_nFlags & STYLE_CHECKBOX_LIKE )
	{
		Select(pValue);
		return true;
	}

	if( !(m_nFlags & STYLE_ENUM) )
		return false;

	if( m_nState & ISTATE_DATACHANGING )
		return false;
	
	CItemBase * pSel = NULL;
	for(CItemBase * p = NULL; p = _ItemBase_GetNextEnumItem(this, p);)
	{
		cVariant v; p->GetValue(v);
		if( pValue == v )
		{
			pSel = p;
			break;
		}
	}

	if( !pSel )
		return false;

	for(CItemBase * p = NULL; p = _ItemBase_GetNextEnumItem(this, p);)
		p->Select(pSel == p);

	return true;
}

bool CItemBase::GetValue(cVariant &pValue)
{
	if( m_pSink && (m_pSink->OnBindingFlags() & ITEMBIND_F_VALUE) )
		return m_pSink->OnGetValue(pValue);

	if( GetType() == tid_STRING_OBJ )
		return pValue = GetText(), true;

	if( m_nFlags & STYLE_ENUM_VALUE )
	{
		if( m_pBinding && m_pBinding->GetHandler(CItemBinding::hValue) )
			m_pBinding->GetHandler(CItemBinding::hValue)->Exec(pValue, TOR(cNodeExecCtx, (m_pRoot, this)));
		
		if( pValue.Type() == cVariant::vtVoid )
			if( CItemBase * pEnumOwner = m_pParent->GetByFlagsAndState(STYLE_ENUM, ISTATE_ALL, NULL, gbfasUpToParent) )
				GetOwnerRelativePath(pValue.MakeString(), pEnumOwner);
		
		return true;
	}

	if( m_nFlags & STYLE_CHECKBOX_LIKE )
	{
		pValue = !!(m_nState & ISTATE_SELECTED);
		return true;
	}

	if( !(m_nFlags & STYLE_ENUM) )
		return false;

	for(CItemBase * p = NULL; p = _ItemBase_GetNextEnumItem(this, p);)
		if( p->m_nState & ISTATE_SELECTED )
			return p->GetValue(pValue), true;

	return false;
}

void CItemBase::OnInited()
{
	if( !(m_nFlags & STYLE_ENUM) )
		return;

	CItemBase * pSelItem = NULL;
	CItemBase * pFirst = NULL;
	CItemBase * pLast = NULL;
	for(CItemBase * p = NULL; p = _ItemBase_GetNextEnumItem(this, p);)
	{
		if( !pFirst )
			pFirst = p;
		pLast = p;
		
		if( p->m_nState & ISTATE_SELECTED )
			return;

		if( p->m_nState & ISTATE_DEFAULT )
			pSelItem = p;
	}
	
	if( !pSelItem && pFirst )
		pSelItem = (pFirst->m_nFlags & ALIGN_REVERSE) ? pLast : pFirst;
	
	if( pSelItem )
		pSelItem->Select();
}

void CItemBase::DeInit()
{
	if( (m_nFlags & STYLE_TOP_ITEM) && !(m_pRoot->m_nGuiFlags & GUIFLAG_EDITOR) )
	{
		cAutoRegKey pProfileKey;
		tString strKey = "Windows\\"; strKey += m_strSection;
		pProfileKey.open(m_pRoot->m_cfg, cRegRoot, strKey.c_str(), cTRUE);

		SaveSettings(pProfileKey);
	}

	OnDeInit();
	
	for(size_t i = 0; i < m_aItems.size(); i++)
		m_aItems[i]->DeInit();
}

void CItemBase::InitItem(LPCSTR strType)
{
	if( !m_pOwner )
		m_pOwner = m_pParent->GetOwner();
}

void CItemBase::ApplyStyle(LPCSTR strStyle)
{
	sswitch( strStyle )
	scase(STR_PID_HIDDEN)             m_nState |= ISTATE_HIDDENEX;       sbreak;
	scase(STR_PID_DISABLED)           m_nState |= ISTATE_DISABLEDEX;     sbreak;
	scase(STR_PID_LOCK)               m_nState |= ISTATE_FROZEN;         sbreak;
	scase(STR_PID_INDEX)              m_nState |= ISTATE_DEFAULT;        sbreak;
	scase(STR_PID_DEFAULT)            m_nState |= ISTATE_DEFAULT;        sbreak;
	scase(STR_PID_CHECKED)            m_nState |= ISTATE_SELECTED;       sbreak;
	scase(STR_PID_CLICKABLE)          m_nFlags |= STYLE_CLICKABLE;       sbreak;
	scase(STR_PID_SINGLELINE)         m_nFlags |= TEXT_SINGLELINE;       sbreak;
	scase(STR_PID_NOBINDHIDE)         m_nFlags |= STYLE_NOBINDHIDE;      sbreak;
	scase(STR_PID_LOCKBUTTON)         m_nFlags |= STYLE_POLICY_LB;       sbreak;
	scase(STR_PID_FIXSTYLE)           m_nFlags |= STYLE_FIX;             sbreak;
	scase(STR_PID_BODY)               m_nFlags |= STYLE_BODY;            sbreak;
	scase(STR_PID_NOPARENTDISABLE)    m_nFlags |= STYLE_NOPARENTDISABLE; sbreak;
	scase(STR_PID_IFLASTNOEXTEND)     m_nFlags |= STYLE_IFLASTNOEXTEND;  sbreak;
	scase(STR_PID_REMEMBERDATA)       m_nFlags |= STYLE_REMEMBER_DATA;   sbreak;
	scase(STR_PID_HIDEEXPANDED)       m_bHideExpanded = 1;               sbreak;
	scase(STR_PID_HIDECOLLAPSED)      m_bHideCollapsed = 1;              sbreak;
	scase(STR_PID_UNDERLINEOFF)       m_nFlags |= STYLE_UNDERLINEOFF;    sbreak;
	scase(STR_PID_BUTTONLIKE)         m_nFlags |= STYLE_BUTTON_LIKE|STYLE_CLICKABLE; m_nFlags &= ~(STYLE_CHECKBOX_LIKE|STYLE_ENUM_VALUE); sbreak;
	scase(STR_PID_CHECKBOXLIKE)       m_nFlags |= STYLE_CHECKBOX_LIKE|STYLE_CLICKABLE; m_nFlags &= ~(STYLE_BUTTON_LIKE|STYLE_ENUM_VALUE); sbreak;
	scase(STR_PID_RADIOLIKE)          m_nFlags |= STYLE_ENUM_VALUE|STYLE_CLICKABLE; m_nFlags &= ~(STYLE_BUTTON_LIKE|STYLE_CHECKBOX_LIKE); sbreak;
	scase(STR_PID_ENUM)               m_nFlags |= STYLE_ENUM;            sbreak;
	scase(STR_PID_NOACTIVATE)         m_nFlags |= STYLE_NOACTIVATE;      sbreak;
	scase(STR_PID_DYNSHOW)            m_nFlags |= STYLE_DYNAMIC_SHOW;    sbreak;
	scase(STR_PID_SHEETPAGES)         m_nFlags |= STYLE_SHEET_PAGES;     sbreak;
	scase(STR_PID_DEBUG)              m_nFlags |= STYLE_DEBUG;           sbreak;
	send;
}

void CItemBase::GetAddSize(SIZE& szAdd)
{
	if( m_nFlags & TEXT_SINGLELINE )
		szAdd.cy += SINGLELINE_MARGSIZE*2;

	if( m_pIcon )
	{
		SIZE szIcon = m_pIcon->Size();
		
		if( !( m_nIconAlign & ALIGN_HCENTER ) )
			szAdd.cx += szIcon.cx + GetIconMargSize();
		else if( szAdd.cx < szIcon.cx )
			szAdd.cx = szIcon.cx;
		
		if( !( m_nIconAlign & ALIGN_VCENTER ) )
		{
			if( m_nIconAlign & ALIGN_HCENTER )
				szAdd.cy += szIcon.cy + GetIconMargSize();
		}
		else if( szAdd.cy < szIcon.cy )
			szAdd.cy = szIcon.cy;
	}

	if( m_pBackground )
	{
		SIZE szBg = m_pBackground->Size();

		if( szAdd.cx < szBg.cx )
			szAdd.cx = szBg.cx;

		if( szAdd.cy < szBg.cy )
			szAdd.cy = szBg.cy;
	}
}

void CItemBase::_ApplyAttrs(CItemProps& strProps)
{
	CItemPropVals& l_prpAttr = strProps.Get(STR_PID_ATTRS);
	for(int p = 0, n = l_prpAttr.Count(); p < n; p++)
	{
		const tString& strStyle = l_prpAttr.Get(p);
		if( !strStyle.empty() )
			ApplyStyle(strStyle.c_str());
	}
}

void CItemBase::InitProps(CItemProps& props, CSinkCreateCtx * pCtx)
{
	if( m_pSink )
		m_pSink->OnInitProps(props);

	// Бордюр
	m_pBorder = m_pRoot->GetBorder(props.Get(STR_PID_BORDER).GetStr());

	// Дефолтный шрифт
	SetFont(NULL);
	
	// Выравнивание внешнее
	{
		CItemPropVals& prp = props.Get(STR_PID_ALIGN);
		if( prp.IsValid() )
		{
			m_nFlags &= ~(ALIGN_MASK);
			m_nFlags |= GetAlign(prp.GetStr());
		}
	}

	// Выравнивание текста
	{
		CItemPropVals& l_prpTextAlign = props.Get(STR_PID_TEXTALIGN);
		if( l_prpTextAlign.IsValid() )
			m_nTextFlags = GetAlign(l_prpTextAlign.GetStr());
		else
			m_nTextFlags = (tDWORD)(m_nFlags & ALIGN_TEXT_MASK);
	}

	// Выравнивание иконы
	{
		CItemPropVals& l_prpIconAlign = props.Get(STR_PID_ICONALIGN);
		if( l_prpIconAlign.IsValid() )
			m_nIconAlign = GetAlign(l_prpIconAlign.GetStr());
		else
			m_nIconAlign = ALIGN_LEFT|ALIGN_VCENTER;
	}

	// Смещения
	{
		CItemPropVals& l_prpPos = props.Get(STR_PID_OFFSET);
		if( l_prpPos.IsValid() )
		{
			m_rcShift.left   = (LONG)l_prpPos.GetLong(m_nFlags & ALIGN_REVERSE_X ? 2 : 0);
			m_rcShift.top    = (LONG)l_prpPos.GetLong(m_nFlags & ALIGN_REVERSE_Y ? 3 : 1);
			m_rcShift.right  = (LONG)l_prpPos.GetLong(m_nFlags & ALIGN_REVERSE_X ? 0 : 2);
			m_rcShift.bottom = (LONG)l_prpPos.GetLong(m_nFlags & ALIGN_REVERSE_Y ? 1 : 3);
		}
	}

	// Поля
	if( !GUI_ISSTYLE_SIMPLE(m_pRoot) || !(m_nFlags & STYLE_BODY) )
	{
		CItemPropVals& l_prpMargins = props.Get(STR_PID_MARGINS);
		if( l_prpMargins.IsValid() )
		{
			m_rcMargin.left   = (LONG)l_prpMargins.GetLong(0);
			m_rcMargin.top    = (LONG)l_prpMargins.GetLong(1);
			m_rcMargin.right  = l_prpMargins.Count() >= 3 ? (LONG)l_prpMargins.GetLong(2) : m_rcMargin.left;
			m_rcMargin.bottom = l_prpMargins.Count() >= 4 ? (LONG)l_prpMargins.GetLong(3) : m_rcMargin.top;
		}
	}
	else
	{
		m_rcMargin.left   = 0;
		m_rcMargin.top    = 0;
		m_rcMargin.right  = 0;
		m_rcMargin.bottom = 0;
	}

	// Размер
	{
		CItemPropVals& l_prpSize = props.Get(STR_PID_SIZE);
		if( l_prpSize.IsValid() )
		{
			m_nFlags &= ~(AUTOSIZE|AUTOSIZE_PARENT);
			
			m_szSizeMin.cx = (LONG)l_prpSize.GetLong(0);
			m_szSizeMin.cy = (LONG)l_prpSize.GetLong(1);
			m_szSizeMax.cx = (LONG)l_prpSize.GetLong(4);
			m_szSizeMax.cy = (LONG)l_prpSize.GetLong(5);

			sswitch(l_prpSize.GetStr(0))
			scase("p") m_nFlags |= AUTOSIZE_PARENT_X; sbreak;
			scase("a") m_nFlags |= AUTOSIZE_X; sbreak;
			send;

			sswitch(l_prpSize.GetStr(1))
			scase("p") m_nFlags |= AUTOSIZE_PARENT_Y; sbreak;
			scase("a") m_nFlags |= AUTOSIZE_Y; sbreak;
			send;
		}

		if( l_prpSize.Count() <= 0  )
			m_nFlags |= AUTOSIZE_X;

		if( l_prpSize.Count() <= 1  )
			m_nFlags |= AUTOSIZE_Y;
	}

	m_pRoot->AdjustMetrics(this);

	// Коэффициенты ресайзинга
	{
		CItemPropVals& l_prpResizeScale = props.Get(STR_PID_RESIZE);
		m_ptResizeScale.x = l_prpResizeScale.Get(0).empty() ? (m_strSection.empty() ? 0 : 100) : (LONG)l_prpResizeScale.GetLong(0);
		m_ptResizeScale.y = l_prpResizeScale.Get(1).empty() ? (m_strSection.empty() ? 0 : 100) : (LONG)l_prpResizeScale.GetLong(1);
	}

	// Порядок табуляции
	{
		if( m_nOrder == (tSIZE_T)-1 )
		{
			tSIZE_T nOrder;
			
			CItemPropVals& l_prpOrder = props.Get(STR_PID_ORDER);
			if( l_prpOrder.IsValid() )
			{
				LPCSTR strAfter = l_prpOrder.GetStr();
				if( strAfter && isalpha(strAfter[0]) )
				{
					if( CItemBase * pAfter = m_pParent->GetItem(strAfter) )
						nOrder = pAfter->m_nOrder + 1;
				}
				else
					nOrder = (tSIZE_T)l_prpOrder.GetLong();
			}
			else
			{
				// Rotate order of reversive defined elements
				nOrder = m_nPos;
				for(size_t i = m_pParent->m_aItems.size(); i > 0; i--)
				{
					if( !(m_pParent->m_aItems[i - 1]->m_nFlags & (ALIGN_HBEFORE|ALIGN_VBEFORE)) )
						break;
					
					if( nOrder )
						nOrder--;
				}
			}
		
			if( m_pParent->m_aItems.size() == m_nPos + 1 )
			{
				for(size_t i = 0; i < m_pParent->m_aItems.size() - 1; i++)
				{
					CItemBase * pItem = m_pParent->m_aItems[i];
					if( pItem->m_nOrder >= nOrder )
						pItem->m_nOrder++;
				}
			}
			
			m_nOrder = nOrder;
		}
		
		if( m_nTabGroup == (tWORD)-1 )
			m_nTabGroup = (tWORD)props.Get(STR_PID_TABGROUP).GetLong();
	}

	// Идентификатор справки
	m_nHelpId = (tWORD)props.Get(STR_PID_HELP).GetLong();

	// Обработка биндинга
	{
		if( m_pBinding )
			m_pBinding->Init(props);
		else
		{
			CItemBinding pBinding(this, pCtx->m_item);
			if( pBinding.Init(props) )
				if( m_pBinding )
					m_pBinding->Assign(pBinding);
				else
					m_pBinding = new CItemBinding(pBinding);
		}
	}
}

void CItemBase::PreBind(CItemBase* pItem, CSinkCreateCtx * pCtx)
{
	OnInitBinding(pItem, pCtx);
	if( m_pSink )
		m_pSink->OnInitBinding(pItem, pCtx);
	
	if( m_pParent)
		if( m_nFlags & STYLE_TOP_ITEM && (pItem != this) )
			m_pRoot->PreBind(pItem, pCtx);
		else
			m_pParent->PreBind(pItem, pCtx);
}

CItemBase * CItemBase::GetNextOrderChild(CItemBase * pPrev, bool bBackward)
{
	size_t n = m_aItems.size();
	if( !n )
		return NULL;
	
	tSIZE_T nOrder;
	if( pPrev )
	{
		if( bBackward )
		{
			if( !pPrev->m_nOrder )
				return NULL;
			nOrder = pPrev->m_nOrder - 1;
		}
		else
		{
			if( pPrev->m_nOrder + 1 == n )
				return NULL;
			nOrder = pPrev->m_nOrder + 1;
		}
	}
	else
		nOrder = bBackward ? (n - 1) : 0;
	
	for(Items::iterator i = m_aItems.begin(); i != m_aItems.end(); i++)
		if( (*i)->m_nOrder == nOrder )
			return *i;

	return NULL;
}

CItemBase * CItemBase::GetNextChildTabstop(CItemBase * pPrev, tDWORD nFlags)
{
	nFlags |= ENUMITEMS_BYORDER;
	if( pPrev && pPrev->IsOwner() )
		nFlags |= ENUMITEMS_SKIPCHILDREN;
	
	CItemBase * pTab = pPrev;
	while( pTab = EnumNextItem(pTab, nFlags) )
	{
		nFlags &= ~ENUMITEMS_SKIPCHILDREN;

		if( !pTab->IsTabstop() )
			continue;

		if( nFlags & ENUMITEMS_TAB_ACTIVE &&
			!(pTab->IsVisible() && pTab->IsEnabled()) )
			continue;
		
/*		if( pPrev )
		{
			bool bSameGroup = pTab->IsSameTabGroup(pPrev);
			if( nFlags & ENUMITEMS_TAB_GROUP )
			{
				if( bSameGroup )
					continue;
			}
			else
			{
				if( !bSameGroup )
					continue;
			}
		}*/

		return pTab;
	}
	
	return NULL;
}

bool CItemBase::SetNextPage(bool bBackward)
{
	if( !(m_nFlags & STYLE_ENUM) )
		return false;

	CItemBase * pNext = NULL;
	while( pNext = _ItemBase_GetNextEnumItem(this, pNext) )
		if( pNext->m_nState & ISTATE_SELECTED )
			break;

	if( pNext )
	{
		if( pNext->m_nFlags & ALIGN_REVERSE )
			pNext = bBackward ? _ItemBase_GetNextEnumItem(this, pNext, false) : _ItemBase_GetNextEnumItem(this, pNext, true);
		else
			pNext = bBackward ? _ItemBase_GetNextEnumItem(this, pNext, true) : _ItemBase_GetNextEnumItem(this, pNext, false);
	}
	
	if( pNext )
		pNext->Select();

	return true;
}

CItemBase * CItemBase::GetCurPage()
{
	return NULL;
}

bool CItemBase::SetCurPage(CItemBase * pPage)
{
	if( (m_nFlags & (STYLE_SHEET_ITEM|STYLE_ENUM)) != (STYLE_SHEET_ITEM|STYLE_ENUM) )
		return false;

	if( !pPage || !(pPage->m_nFlags & STYLE_ENUM_VALUE) )
		return false;
	
	pPage->Select();
	return true;
}

bool CItemBase::IsSameTabGroup(CItemBase * pItem)
{
	CItemBase * p1 = this;
	CItemBase * p2 = pItem;
	int n1 = 0, n2 = 0;
	
	do
	{
		if( p1->m_pParent )
		{
			p1 = p1->m_pParent;
			n1++;
		}
		
		if( p2->m_pParent )
		{
			p2 = p2->m_pParent;
			n2++;
		}
	} while( p1->m_pParent || p2->m_pParent );

	p1 = this;
	p2 = pItem;

	for( ; n2 > n1; n2-- )
		p2 = p2->m_pParent;
	for( ; n1 > n2; n1-- )
		p1 = p1->m_pParent;

	while( n1-- )
	{
		if( p1->m_nTabGroup != p2->m_nTabGroup )
			return false;
		p1 = p1->m_pParent;
		p2 = p2->m_pParent;
	}

	return true;
}

void CItemBase::CorrectTabOrder()
{
	CItemBase * pOwner = GetOwner();
	if( !pOwner )
		return;

	CItemBase * pTab = NULL;
	CItemBase * pTabPrev = NULL;
	while( pTab = pOwner->GetNextChildTabstop(pTab) )
	{
		pTab->SetTabOrder(pTabPrev, true);
		if( pTab->IsOwner() )
			pTab->CorrectTabOrder();
		pTabPrev = pTab;
	}
}

LPCSTR CItemBase::GetOwnerRelativePath(tString& strPath, CItemBase * pOwner, bool bFullPath, LPCSTR strSuffix, CItemBase * p)
{
	if( !p )
		p = this;
	
	// Дошли до овнера
	if( pOwner == this )
	{
		if( pOwner != p )
			return strPath.c_str();

		strPath += STR_SELF;
		return strPath.c_str();
	}

	// Если есть родитель, сначала получим начало пути
	if( bFullPath && m_pParent )
		m_pParent->GetOwnerRelativePath(strPath, pOwner, bFullPath, NULL, p);

	// Пристыковываем текущий идентификатор
	if( strPath.size() )
		strPath += GUI_STR_AUTOSECT_SEP_STR;
	strPath += m_strItemId;

	// Если есть, пристыковываем суффикс
	if( strSuffix )
	{
		strPath += GUI_STR_ITEMIDSUFF_SEP_STR;
		strPath += strSuffix;
	}

	return strPath.c_str();
}

void CItemBase::PreInit(CItemBase * pTop, CSinkCreateCtx * pCtx)
{
	PreInit1(pCtx);
	PreInit2();
	CleanProps();
}

void CItemBase::PreInit1(CSinkCreateCtx * pCtx)
{
	if( m_pProps )
		InitProps(*m_pProps, pCtx);

	CSinkCreateCtx ctx(this);

	size_t i, n = m_aItems.size();
	for(i = 0; i < n; i++)
		m_aItems[i]->PreInit1(&ctx);
}

void CItemBase::PreInit2()
{
	size_t i, n = m_aItems.size();
	for(i = 0; i < n; i++)
		m_aItems[i]->PreInit2();

	OnInit();
	if( m_pSink )
		m_pSink->OnInit();
}

void CItemBase::CleanProps()
{
	if( m_pProps )
		delete m_pProps, m_pProps = NULL;

	for(size_t i = 0; i < m_aItems.size(); i++)
		m_aItems[i]->CleanProps();
}

void CItemBase::PostInit(CItemBase * pTop)
{
	InitSize();
	
	OnUpdateProps(UPDATE_FLAG_TEXT|UPDATE_FLAG_FONT|UPDATE_FLAG_ICON|UPDATE_FLAG_BACKGROUND);
	
	// If child is not from m_aItems don't recalc it
	if( !m_pParent || m_pParent->m_aItems.size() )
		RecalcLayout(true);

	if( !(m_nFlags & STYLE_CUSTOMCHILDREN) )
		for(tSIZE_T i = 0; i < GetChildrenCount(); i++)
			GetByPos(i)->PostInit(pTop);

	m_nState |= ISTATE_INITED;

	OnUpdateProps(UPDATE_FLAG_POSITION|UPDATE_FLAG_SIZE, ISTATE_DISABLED|ISTATE_DEFAULT|ISTATE_SELECTED);

	if( m_pSink )
		m_pSink->OnInited();

	// Загрузка сохраненных данных
	if( !(m_pRoot->m_nGuiFlags & GUIFLAG_EDITOR) && pTop == this )
	{
		CItemBase * pOwner = GetOwner(true);
		if( !pOwner )
			pOwner = this;
		
		cAutoRegKey pProfileKey;
		tString strKey = "Windows\\"; strKey += pOwner->m_strSection;
		if( PR_SUCC(pProfileKey.open(m_pRoot->m_cfg, cRegRoot, strKey.c_str())) )
			LoadSettings(pProfileKey);
	}

	OnInited();
}

void CItemBase::PostShow()
{
	OnUpdateProps(0, ISTATE_HIDE);
	if( !(m_nFlags & STYLE_CUSTOMCHILDREN) )
		for(size_t i = 0; i < m_aItems.size(); i++)
			m_aItems[i]->PostShow();
}

void CItemBase::ReLoad()
{
	DeInit();
	
	m_nState |= ISTATE_HIDE;
	OnUpdateProps(0, ISTATE_HIDE);
	m_nState &= ~ISTATE_HIDE;

	Clear();

	CItemSink * pSink = m_pSink;
	if( m_pSink )
		m_pSink->OnDestroy(), m_pSink = NULL;

	m_nState &= ~ISTATE_INITED;

	CItemBase* pItem = this;

	tDWORD nFlags = 0;
	if( m_nFlags & STYLE_DESTROYSINK )
		nFlags |= LOAD_FLAG_DESTROY_SINK;

	m_pParent->LoadItem(pSink, pItem, m_strSection.c_str(), m_strItemId.c_str(), nFlags);
	OnUpdateProps(0, ISTATE_HIDE);
}

void CItemBase::LoadChilds(tDWORD nFlags, CItemBase * pTop, CSinkCreateCtx * pCtx)
{
	sIniSection * pSec = m_pRoot->GetIniSection(PROFILE_LAYOUT, m_strSection.c_str());
	if( !pSec )
		return;

	tString strProps;
	for(sIniLine * pLine = pSec->line_head; pLine; pLine = pLine->next)
	{
		const tCHAR * strLine = pLine->name;
		if( !strLine || *strLine == '$' || *strLine == STR_SELF_SYMB )
			continue;

		if( m_pRoot->m_nGuiFlags & GUIFLAG_EDITOR )
		{
			strProps.erase();

			// Оставляем переопределения с макросами
			LPCSTR strOvrd[] = {STR_PID_SUBST, NULL}; CItemProps _SubstProps(pLine->value, strProps, strOvrd);
			m_pRoot->_GetString(strProps, PROFILE_LAYOUT, m_strSection.c_str(), "", this);
			_SubstProps.DeParse(strProps);
		}
		else
		{
			strProps = pLine->value;
			m_pRoot->_GetString(strProps, PROFILE_LAYOUT, m_strSection.c_str(), "", this);
		}
		
		CItemBase * pItem = NULL; LoadInternal(NULL, strLine, strProps.c_str(), pItem, nFlags, pTop, pCtx);
	}
}

bool CItemBase::_ExtractSection(tString& strProps, tString& strSection, bool bCheckOnly)
{
	if( LPCSTR strSectBegin = strchr(strProps.c_str(), '[') )
		if( LPCSTR strSectEnd = strchr(strSectBegin, ']') )
		{
			if( !bCheckOnly )
			{
				if( &strProps == &strSection )
				{
					strSection.erase(strSectEnd - strSection.c_str(), strSection.size() - (strSectEnd - strSection.c_str()));
					strSection.erase(0, strSectBegin - strSection.c_str() + 1);
				}
				else
				{
					strSection.assign(strSectBegin + 1, strSectEnd - strSectBegin - 1);
					strProps.erase(strSectBegin - strProps.c_str(), strSectEnd - strSectBegin + 1);
				}
			}
			
			return true;
		}

	return false;
}

bool CItemBase::LoadInternal(CItemSink* pSink, LPCSTR p_strItemId, LPCSTR p_strItemInfo, CItemBase*& p_pItem, tDWORD nFlags, CItemBase * pTop, CSinkCreateCtx * pCtx)
{
	tString strId = p_strItemId ? p_strItemId : "";
	tString strItemSection;
	
	CItemProps _PropsInfo;
	{
		tString strInfo;
		if( p_strItemInfo )
			strInfo = p_strItemInfo;

		if( !p_strItemInfo || !*p_strItemInfo )
		{
			if( p_pItem )
			{
				strItemSection = p_pItem->m_strSection;
				if( strItemSection.empty() && !p_pItem->m_pOwner )
					strItemSection = p_pItem->m_strItemId;
			}
		}
		else
		{
			_PropsInfo.Parse(p_strItemInfo, strItemSection);
			if( !_ExtractSection(strItemSection, strItemSection) )
				strItemSection.erase();
		}
	}

	if( strId.empty() && p_pItem )
		strId = p_pItem->m_strItemId;

	if( strId.empty() )
		strId = strItemSection;

	if( strId.empty() )
		return false;

	CItemProps _Props;
	tString strItemOrigSection;
	bool bExtPrm;
	{
		if( strItemSection.size() )
		{
			tString strSelfProps; m_pRoot->GetString(strSelfProps, PROFILE_LAYOUT, strItemSection.c_str(), STR_SELF, "", this);
			tString strSelfOverride;
			_Props.Parse(strSelfProps.c_str(), strSelfOverride, NULL, IPV_F_APPEND);

			if( _ExtractSection(strSelfOverride, strSelfOverride, true) )
			{
				CItemBase pTemp;
				pTemp.m_nFlags |= STYLE_EXT_ITEM;
				pTemp.m_strSection = strItemSection;
				pTemp.m_strItemId = strId;
				AddItem(&pTemp);

				pTemp._InitOverrides(_PropsInfo.Get(STR_PID_SUBST));
				_PropsInfo.Del(STR_PID_SUBST);

				if( pTemp.m_strItemId == STR_SELF )
					pTemp.m_nFlags |= STYLE_EXTPRM;

				_PropsInfo.DeParse(strSelfProps);
				
				if( p_pItem && p_pItem->m_pParent )
				{
					p_pItem->m_pParent->RemoveItem(p_pItem);
					pTemp.AddItem(p_pItem);
				}
				
				pTemp.LoadInternal(pSink, STR_SELF, strSelfProps.c_str(), p_pItem, nFlags, pTop, pCtx);
				if( p_pItem )
				{
					pTemp.RemoveItem(p_pItem);
					AddItem(p_pItem);
					p_pItem->m_strSection = strItemSection;
					p_pItem->m_strItemId = strId;
				}
				
				pTemp.Cleanup();
				return !!p_pItem;
			}
		}

		if( _Props.Count() )
		{
			if( _PropsInfo.Count() )
				_Props.Parse(_PropsInfo.DeParse(TOR_tString, IPV_F_VOIDPROP), TOR_tString, NULL, IPV_F_APPEND);
		}
		else
			swap_objmem(_Props, _PropsInfo);

		if( m_pRoot->m_nGuiFlags & GUIFLAG_EDITOR )
			strItemOrigSection = strItemSection;

		if( bExtPrm = _Props.Get(STR_PID_EXTPRM).GetBool() )
			_GetOverride(GetByFlagsAndState(STYLE_EXT_ITEM, ISTATE_ALL, NULL, gbfasUpToParent), TOR(tString,(strId)), _Props, strItemSection);
	}

	if( !(m_pRoot->m_nGuiFlags & GUIFLAG_EDITOR) )
	{
		bool bLoad = true;
		
		if( !p_pItem && this != m_pRoot && m_aItems.size() && (strId == m_aItems[m_aItems.size() - 1]->m_strItemId) )
			bLoad = false;
		else
		{
			if( m_pRoot->m_pSink )
			{
				CItemPropVals& l_prpUsefor = _Props.Get(STR_PID_USEFOR);
				if( l_prpUsefor.IsValid() )
					bLoad = ((CRootSink *)m_pRoot->m_pSink)->CheckCondition(this, l_prpUsefor.GetStr(0));
			}
		}

		if( !bLoad )
			return false;
	}

	CItemPropVals& l_prpItemType = _Props.Get(STR_PID_TYPE);

	CItemBase* pReplaceItem = ((nFlags & LOAD_FLAG_REPLACE) && p_pItem) ? p_pItem : NULL;
	if( pReplaceItem )
		p_pItem = NULL;

	if( !p_pItem )
		if( p_pItem = CreateChild(strId.c_str(), l_prpItemType.GetStr()) )
			p_pItem->m_strItemType = l_prpItemType.GetStr();

	if( !p_pItem )
		if( p_pItem = m_pRoot->CreateItem(strId.c_str(), l_prpItemType.GetStr()) )
			p_pItem->m_strItemType = l_prpItemType.GetStr();

	if( !p_pItem )
		p_pItem = new CItemBase();

 	if( p_pItem->m_strItemType == GUI_ITEMT_DIALOG )
 		((CDialogItem *)p_pItem)->SetCtxItem(pCtx->m_item);

	if( !pTop )
		pTop = p_pItem;
	
	if( pReplaceItem )
	{
		p_pItem->m_pRoot     = m_pRoot;
		p_pItem->m_pParent   = pReplaceItem->m_pParent;
		p_pItem->m_nPos      = pReplaceItem->m_nPos;
		p_pItem->m_nOrder    = pReplaceItem->m_nOrder;
		p_pItem->m_nTabGroup = pReplaceItem->m_nTabGroup;
		m_aItems[p_pItem->m_nPos] = p_pItem;

		pReplaceItem->m_pParent = NULL;
		pReplaceItem->Destroy();
	}
	
	p_pItem->m_strSection = strItemSection;
	p_pItem->m_strItemId = strId.empty() ? strItemSection : strId;

	if( !(p_pItem->m_nState & ISTATE_PREINITED) )
	{
		if( !pReplaceItem )
			AddItem(p_pItem);

		p_pItem->_ApplyAttrs(_Props);
	}

	if( _Props.Get(STR_PID_EXT).GetBool() )
		p_pItem->m_nFlags |= STYLE_EXT;
	if( bExtPrm || strId == STR_SELF )
		p_pItem->m_nFlags |= STYLE_EXTPRM;

	if( pSink )
		p_pItem->AttachSink(pSink, false);

	if( (m_pRoot->m_nGuiFlags & GUIFLAG_EDITOR) && (nFlags & LOAD_FLAG_EDIT) )
		CItemEditData::Init(p_pItem, _Props, strItemOrigSection);
	
	if( !(p_pItem->m_nState & ISTATE_PREINITED) )
	{
		p_pItem->InitItem(l_prpItemType.GetStr());
		p_pItem->m_nState |= ISTATE_PREINITED;
	}

	if( m_nFlags & STYLE_DEBUG )
		debugBreak();

	// Имя синка
	{
		CItemPropVals& prp = _Props.Get(STR_PID_ALIAS);
		if( prp.IsValid() )
			p_pItem->m_strItemAlias = prp.GetStr();
	}

	// Переопределения
	p_pItem->_InitOverrides(_Props.Get(STR_PID_SUBST));

	p_pItem->PreBind(p_pItem, pCtx);

	if( strItemSection.size() )
		p_pItem->LoadChilds(nFlags & (LOAD_FLAG_EDIT), pTop, &CSinkCreateCtx(p_pItem));

	if( (m_pRoot->m_nGuiFlags & GUIFLAG_ADMIN) && (m_pRoot->m_nGuiFlags & GUIFLAG_LOCKBUTTONS) )
	{
		CItemPropVals& l_prpLockBtn = _Props.Get(STR_PID_LOCKBUTTON);
		if( l_prpLockBtn.IsValid() )
		{
			tString strId(p_pItem->m_strItemId); strId += "__lb";
			CItemBase * pLockBtn = NULL;
			
			if( p_pItem->m_strItemType == GUI_ITEMT_GROUP )
			{
				p_pItem->LoadInternal(NULL, strId.c_str(), "[" STR_SID_POLICYLBGROUP "]", pLockBtn, 0, NULL, pCtx);
			}
			else
			if( p_pItem->m_strItemType.empty() && p_pItem->m_strItemId == "DelimiterLine" )
			{
				CItemBase * pParent = p_pItem->m_pParent;
				if( pParent )
					pParent->LoadInternal(NULL, strId.c_str(), "[" STR_SID_POLICYLBLINE "]", pLockBtn, 0, NULL, pCtx);
			}
			else
			{
				CItemBase * pParent = p_pItem->m_pParent;
				if( pParent && pParent->LoadInternal(NULL, strId.c_str(), "[" STR_SID_POLICYLB "]", pLockBtn, 0, NULL, pCtx) )
					pParent->MoveItem(p_pItem, pLockBtn);
			}
			
			if( pLockBtn )
			{
				pLockBtn->m_nFlags |= STYLE_POLICY_LB;
				pLockBtn->m_strText = l_prpLockBtn.GetStr();
			}
		}
	}

	if( !(m_pRoot->m_nGuiFlags & GUIFLAG_EDITOR) && p_pItem->m_pOverrides )
		delete p_pItem->m_pOverrides, p_pItem->m_pOverrides = NULL;

	p_pItem->m_pProps = new CItemProps();
	p_pItem->m_pProps->Assign(_Props, IPV_F_MOVE);
	return true;
}

void CItemBase::LoadMenuInternal(CPopupMenuItem * pMenu, CItemBase * pItem, LPCSTR strLoadInfo, CSinkCreateCtx *pCtx)
{
	tString strBuff;
	if( !strLoadInfo )
	{
		if( m_strSection.empty() && m_pParent )
		{
			strBuff = m_pParent->m_strSection;
			strBuff += GUI_STR_AUTOSECT_SEP;
			strBuff += m_strItemId;
		}
		else
			strBuff = m_strSection;
		
		strBuff += GUI_STR_AUTOSECT_SEP;
		strBuff += "menu";
		strLoadInfo = strBuff.c_str();
	}

	pMenu->AddFromSection(strLoadInfo, pCtx);
	
	OnMenu(pMenu, pItem);
	
	if( !(m_nFlags & STYLE_TOP_ITEM) && m_pParent )
		m_pParent->LoadMenuInternal(pMenu, pItem, NULL, pCtx);
}

bool CItemBase::AttachSink(CItemSink* pSink, bool bDestroy)
{
	if( m_pSink )
	{
		m_pSink->OnDestroy();
		m_pSink->m_pItem = NULL;
		if( m_nFlags & STYLE_DESTROYSINK )
		{
			m_pSink->Destroy();
			m_nFlags &= ~STYLE_DESTROYSINK;
		}
	}
	
	m_pSink = pSink;
	
	if( m_pSink )
	{
		m_pSink->m_pItem = this;
		if( bDestroy )
			m_nFlags |= STYLE_DESTROYSINK;
		m_pSink->OnCreate();
	}
	
	return true;
}

bool CItemBase::LoadItem(CItemSink* pSink, CItemBase*& pItem, LPCSTR strSection, LPCSTR strItemId, tDWORD nFlags, CSinkCreateCtx *pCtx)
{
	CSinkCreateCtx ctx(this);
	if( !pCtx )
		pCtx = &ctx;

	tString strItemInfo;
	if( strSection )
	{
		if( !(nFlags & LOAD_FLAG_INFO) )
			strItemInfo = "[";
		strItemInfo += strSection;
		if( !(nFlags & LOAD_FLAG_INFO) )
			strItemInfo += "]";
	}

	if( nFlags & LOAD_FLAG_INFO && strItemInfo.empty() )
		strItemInfo = " ";

	if( !LoadInternal(pSink, strItemId, strItemInfo.c_str(), pItem, nFlags & (LOAD_FLAG_REPLACE|LOAD_FLAG_EDIT), NULL, pCtx) )
		return false;
	
	if( nFlags & LOAD_FLAG_DESTROY_SINK )
		pItem->m_nFlags |= STYLE_DESTROYSINK;

	pItem->PreInit(pItem, pCtx);

	pItem->_CheckEnabled();
	pItem->_CheckShow();

	pItem->UpdateData(true);

	// Если загрузка в контейнер для редактирования
	if( pItem->m_pEditData && !m_pEditData )
		memset(&pItem->m_rcShift, 0, sizeof(pItem->m_rcShift));
	
	if( !(m_nState & ISTATE_INITED) )
		return true;

	if( nFlags & LOAD_FLAG_NOSHOW )
	{
		pItem->m_nState |= ISTATE_HIDDENEX;
		_CheckShow();
	}

	if( nFlags & LOAD_FLAG_PRELOAD )
		return true;

	pItem->PostInit(pItem);

	if( m_nState & ISTATE_INITED )
		pItem->CorrectTabOrder();

	pItem->PostShow();
	
	if( m_nFlags & STYLE_SHEET_ITEM )
		OnUpdateProps(UPDATE_FLAG_TEXT);
	return true;
}

bool CItemBase::MoveItem(CItemBase* pRelativeItem, CItemBase* pItem, bool bAfter)
{
	if( !pItem )
		return false;
	
	if( (pRelativeItem && pRelativeItem->m_nPos >= GetChildrenCount()) || pItem->m_nPos >= GetChildrenCount() )
		return false;

	RemoveItem(pItem);

	pItem->m_nPos = bAfter ?
		(pRelativeItem ? (pRelativeItem->m_nPos + 1) : 0) :
		(pRelativeItem ? pRelativeItem->m_nPos : m_aItems.size());
	pItem->m_pParent = this;
	m_aItems.insert(m_aItems.begin() + pItem->m_nPos, pItem);
	for(size_t i = pItem->m_nPos + 1; i < m_aItems.size(); i++)
		m_aItems[i]->m_nPos++;

	// copy attributes: offset, aligment,
	CItemBase * pNextItem = bAfter ? NULL : pRelativeItem;
	if( pNextItem )
	{
		RECT rcItemShift = pItem->m_rcShift;
		pItem->m_nFlags &= ~ALIGN_MASK;
		pItem->m_nFlags |= pNextItem->m_nFlags & ALIGN_MASK;
		pItem->m_rcShift = pNextItem->m_rcShift;
		pItem->m_rcMargin = pNextItem->m_rcMargin;
		
		if( pItem->m_nFlags & ALIGN_REVERSE_X )
			pItem->m_rcShift.left = rcItemShift.left;
		else
			pItem->m_rcShift.right = rcItemShift.right;
		
		if( pItem->m_nFlags & ALIGN_REVERSE_Y )
			pItem->m_rcShift.top = rcItemShift.top;
		else
			pItem->m_rcShift.bottom = rcItemShift.bottom;
		
		pNextItem->m_nFlags &= ~ALIGN_MASK;
		pNextItem->m_nFlags |= ALIGN_VSAME|ALIGN_HAFTER;
		pNextItem->m_rcShift.left = 0;
		pNextItem->m_rcShift.top = 0;
		pNextItem->m_rcShift.right = 0;
		pNextItem->m_rcShift.bottom = 0;
	}

	CorrectTabOrder();

	if( m_nFlags & STYLE_SHEET_ITEM )
		OnUpdateProps(UPDATE_FLAG_TEXT);
	return true;
}

bool CItemBase::AddItem(CItemBase* pItem)
{
	pItem->m_pRoot = m_pRoot;
	pItem->m_pParent = this;
	pItem->m_nPos = m_aItems.size();
	pItem->m_lockUpdate = m_lockUpdate;
	m_aItems.push_back(pItem);
	return true;
}

CItemBase* CItemBase::CloneItem(CItemBase* pClone, LPCSTR strItemId, bool bUseIni)
{
	if( bUseIni )
	{
		CItemBase* pItem = NULL;
		LoadItem(NULL, pItem, m_pRoot->GetString(TOR(tString,()), PROFILE_LAYOUT, m_strSection.c_str(), pClone->m_strItemId.c_str(), "", this), strItemId, LOAD_FLAG_NOSHOW|LOAD_FLAG_INFO);
		return pItem;
	}
	
	CItemBase* pItem = pClone ? new CItemBase(*pClone) : new CItemBase();
	pItem->m_strItemId = strItemId;
	AddItem(pItem);
	pItem->RecalcLayout(true);
	return pItem;
}

void CItemBase::RemoveItem(CItemBase* pItem)
{
	m_aItems.erase(m_aItems.begin() + pItem->m_nPos);
	for(size_t i = 0; i < m_aItems.size(); i++)
	{
		CItemBase * pChild = m_aItems[i];
		if( i >= pItem->m_nPos )
			pChild->m_nPos--;
		if( pChild->m_nOrder > pItem->m_nOrder )
			pChild->m_nOrder--;
	}
	
	pItem->m_pParent = NULL;
}

void CItemBase::LockUpdate(bool bLock, bool bUpdate)
{
	if( bLock )
	{
		if( !m_lockUpdate++ )
			m_nState &= ~ISTATE_NEED_UPDATE;
	}
	else if( m_lockUpdate )
		m_lockUpdate--;

	bool bUpdateSelf = bUpdate && !m_lockUpdate && (m_nState & ISTATE_NEED_UPDATE);
	if( bUpdateSelf && !(m_nFlags & STYLE_NON_OWNER) )
		bUpdate = false;

	if( !(m_nFlags & STYLE_CUSTOMCHILDREN) )
	{
		size_t i, n = m_aItems.size();
		for(i = 0; i < n; i++)
			m_aItems[i]->LockUpdate(bLock, bUpdate);
	}

	if( bUpdateSelf )
		Update();
}

void CItemBase::SetInt(tQWORD val, bool bNotify)
{
	CValueBase::SetInt(val);
	if( bNotify )
		SetChangedData();
}

void CItemBase::RecalcItemsLayout(bool bSize)
{
	if( !m_pParent )
		return;

	RecalcPlacement(bSize);
	if( !m_aItems.size() )
		return;
	
	if( !(m_nFlags & STYLE_CUSTOMCHILDREN) )
		for(size_t i = 0; i < m_aItems.size(); i++)
			m_aItems[i]->RecalcItemsLayout(bSize);

	if( m_nFlags & AUTOSIZE )
		RecalcParentSize();
}

CItemBase* CItemBase::GetItemWithBackground()
{
	if( m_pBackground || (m_nFlags & STYLE_TOP_ITEM) || 
		(m_nFlags & STYLE_WINDOW) && (m_nFlags & STYLE_DRAW_BACKGROUND) /*&& !(m_nFlags & STYLE_TRANSPARENT)*/ )
		return this;
	return m_pParent ? m_pParent->GetItemWithBackground() : NULL;
}

bool CItemBase::IsDrawInParent()
{
	return !(m_nFlags & STYLE_WINDOW) || (m_nFlags & STYLE_NON_OWNER) && !(m_nFlags & STYLE_TRANSPARENT);
}

void CItemBase::OnAdjustUpdateRect(RECT& rcUpdate)
{
	if( m_pIcon )
	{
		RECT rcClient; GetClientRectEx(rcClient, false, true);

		SIZE szIcon = m_pIcon->Size();
		szIcon.cx += rcClient.left;
		szIcon.cy += rcClient.top;
		
		if( RECT_CX(rcUpdate) < szIcon.cx )
			rcUpdate.right += szIcon.cx - RECT_CX(rcUpdate);
		if( RECT_CY(rcUpdate) < szIcon.cy )
			rcUpdate.bottom += szIcon.cy - RECT_CY(rcUpdate);
	}
}

bool CItemBase::GetUpdateRectEx(RECT& rcFrame, RECT* rcUpdate, CItemBase* pOwner)
{
	if( !(m_nState & ISTATE_INITED) )
		return false;

	if( pOwner || IsDrawInParent() )
	{
		GetParentRect(rcFrame, pOwner);
	}
	else
	{
		rcFrame.left = 0;
		rcFrame.top = 0;
		rcFrame.right = m_szSize.cx;
		rcFrame.bottom = m_szSize.cy;
	}

	OnAdjustUpdateRect(rcFrame);

	if( !rcUpdate )
		return true;

/*	if( rcUpdate->left == rcUpdate->right || rcUpdate->top == rcUpdate->bottom )
	{
		*rcUpdate = rcFrame;
		return true;
	}
*/
	if( rcUpdate->left > rcFrame.right || rcUpdate->right < rcFrame.left ||
		rcUpdate->top > rcFrame.bottom || rcUpdate->bottom < rcFrame.top )
		return false;

	return true;
}

bool CItemBase::Clip(HDC dc, RECT* rcUpdate, CItemBase* pOwner)
{
	if( (m_nFlags & STYLE_NO_CLIPING) || m_pRoot->m_pDrawExcl == this )
		return false;

	RECT rcFrame;
	if( !GetUpdateRectEx(rcFrame, rcUpdate, pOwner ? pOwner : this) )
		return false;

	if( pOwner && !IsDrawInParent() )
	{
		if( m_nFlags & STYLE_TOP_ITEM )
			return false;

		m_pRoot->ClipRect(dc, &rcFrame);
		return true;
	}

	bool bRet = false;
	if( !(m_nFlags & STYLE_CUSTOMCHILDREN) )
		for(size_t i = 0; i < m_aItems.size(); i++)
		{
			CItemBase* pItem = m_aItems[i];
			if( !(pItem->m_nState & ISTATE_HIDE) )
				bRet |= pItem->Clip(dc, rcUpdate, pOwner ? pOwner : this);
		}
	
	return bRet;
}

bool CItemBase::Draw(HDC dc, RECT* rcUpdate)
{
	if( m_pRoot->m_pDrawExcl == this )
		return true;
	
	RECT rcFrame;
	if( !GetUpdateRectEx(rcFrame, rcUpdate) )
		return false;

	HDC hDrawDc = dc;
	if( m_nFlags & STYLE_TRANSPARENT )
	{
		HDC hTempDc = m_pRoot->GetTempDC(rcFrame, rcUpdate);
		if( hTempDc )
			hDrawDc = hTempDc;
		
		Clip(hDrawDc, rcUpdate);
	}

	DrawItem(hDrawDc, rcFrame);
	DrawChildren(hDrawDc, rcUpdate);
	
	if( m_nFlags & STYLE_TRANSPARENT )
		m_pRoot->CopyTempDC(hDrawDc, dc, rcFrame);
	return true;
}

void CItemBase::DrawItem(HDC dc, RECT& rcFrame)
{
	RECT rcClient; GetClientRectEx(rcClient, false, true);
	rcClient.left += rcFrame.left, rcClient.right += rcFrame.left;
	rcClient.top += rcFrame.top, rcClient.bottom += rcFrame.top;

	if( (m_nFlags & STYLE_TRANSPARENT) && !(m_nFlags & STYLE_TOP_ITEM) )
	{
		RECT rcBg; GetParentRect(rcBg, this);
		DrawParentBackground(dc, &rcBg);
	}

	if( !(m_nFlags & STYLE_WINDOW) || (m_nFlags & STYLE_DRAW_BACKGROUND) )
	{
		DrawBackground(dc, rcClient);
		DrawBorder(dc, rcClient);
	}

	if( !(m_nFlags & STYLE_WINDOW) || (m_nFlags & STYLE_DRAW_CONTENT) )
		DrawContent(dc, rcClient);
}

void CItemBase::DrawChildren(HDC dc, RECT* rcUpdate)
{
	if( !(m_nFlags & STYLE_CUSTOMCHILDREN) )
		for(size_t i = 0; i < m_aItems.size(); i++)
		{
			CItemBase* pItem = m_aItems[i];
			if( !(pItem->m_nState & ISTATE_HIDE) && pItem->IsDrawInParent() )
			{
				pItem->Draw(dc, rcUpdate);
			}
		}
}

bool CItemBase::DrawParentBackground(HDC dc, RECT * rcDraw)
{
	CItemBase* pBgParent = m_pParent->GetItemWithBackground();
	if( !pBgParent )
		return false;

	RECT rcItem; GetParentRect(rcItem, pBgParent);
	rcItem.left = -rcItem.left;
	rcItem.top = -rcItem.top;
	pBgParent->DrawBackground(dc, rcItem);
	return true;
}

void CItemBase::DrawBorder(HDC dc, RECT& rcClient)
{
	if( m_pBorder )
		m_pBorder->Draw(dc, rcClient);
}

void CItemBase::DrawBackground(HDC dc, RECT& rcClient, RECT * prcClip)
{
	if( m_pBackground )
		m_pBackground->Draw(dc, rcClient, m_pBgCtx, m_pBackground.GetImage(GetStateImage()));
}

void CItemBase::DrawContent(HDC dc, RECT& rcClient)
{
	rcClient.left   += m_rcMargin.left;
	rcClient.top    += m_rcMargin.top;
	rcClient.right  -= m_rcMargin.right;
	rcClient.bottom -= m_rcMargin.bottom;

	DrawIconText(dc, rcClient);
}

void CItemBase::DrawIconText(HDC dc, RECT& rc)
{
	// Calc picture and elem aval rect
	RECT  l_rcImage, l_rcText = rc;
	tDWORD nTextFlags = GetFontFlags();
	
	if( m_pIcon )
	{
		SIZE szIcon = m_pIcon->Size();

		if( !( m_nIconAlign & ALIGN_HCENTER ) )
		{
			if( m_nIconAlign & ALIGN_RIGHT )
			{
				l_rcImage.right = rc.right;
				l_rcImage.left  = rc.right - (szIcon.cx + GetIconMargSize());

				l_rcText.left   = rc.left;
				l_rcText.right  = l_rcImage.left;
			}
			else
			{
				l_rcImage.left  = rc.left;
				l_rcImage.right = rc.left + (szIcon.cx + GetIconMargSize());

				l_rcText.left   = l_rcImage.right;
				l_rcText.right  = rc.right;
			}
		}
		else
		{
			l_rcImage.left  = (int)(rc.left + rc.right - szIcon.cx)/2;
			l_rcImage.right = l_rcImage.left + szIcon.cx;

			nTextFlags |= ALIGN_HCENTER;
		}

		if( !( m_nIconAlign & ALIGN_VCENTER ) )
		{
			if( m_nIconAlign & ALIGN_BOTTOM )
			{
				l_rcImage.bottom = rc.bottom;
				l_rcImage.top    = rc.bottom - (szIcon.cy + GetIconMargSize());

				if( m_nIconAlign & ALIGN_HCENTER )
				{
					l_rcText.top     = rc.top;
					l_rcText.bottom  = l_rcImage.top;
				}
			}
			else
			{
				l_rcImage.top    = rc.top;
				l_rcImage.bottom = rc.top + (szIcon.cy + GetIconMargSize());

				if( m_nIconAlign & ALIGN_HCENTER )
				{
					l_rcText.top     = l_rcImage.bottom;
					l_rcText.bottom  = rc.bottom;
				}
			}
		}
		else
		{
			l_rcImage.top    = m_szSize.cy >= szIcon.cy ? ((int)(rc.top + rc.bottom - szIcon.cy)/2) : (rc.top - m_rcMargin.top);
			l_rcImage.bottom = l_rcImage.top + szIcon.cy;

			nTextFlags |= ALIGN_VCENTER;
		}

		if( m_pIcon )
			m_pIcon->Draw(dc, l_rcImage, m_pBgCtx, m_pIcon.GetImage(GetStateImage()));
	}
	
	if( m_nFlags & TEXT_SINGLELINE )
		nTextFlags |= ALIGN_VCENTER;
	
	if( m_pFont )
		m_pFont->Draw(dc, OnGetDisplayText(), l_rcText, nTextFlags);
}

CItemBase * CItemBase::HitTest(POINT& pt, CItemBase * pInParent, bool bClickable)
{
	if( m_nState & ISTATE_HIDE )
		return NULL;

	if( !pInParent )
		pInParent = this;

	for(size_t i = 0; i < m_aItems.size(); i++)
	{
		CItemBase* pHitItem = m_aItems[i]->HitTest(pt, pInParent, bClickable);
		if( pHitItem )
			return pHitItem;
	}

	if( bClickable )
	{
		if((m_nFlags & STYLE_CLICKABLE) && m_pParent && (m_pParent->m_nFlags & STYLE_CLICKABLE))
		{

		}
		else
		{
			if( m_nState & ISTATE_DISABLED )
				return NULL;

			if( !(m_nFlags & STYLE_CLICKABLE) )
				return NULL;

			if( !(m_nFlags & STYLE_NON_OWNER) && (m_nFlags & STYLE_WINDOW) && pInParent != this )
				return NULL;
		}
	}

	RECT rcFrame; GetParentRect(rcFrame, pInParent);
	if( rcFrame.left <= pt.x && rcFrame.right >= pt.x &&
		rcFrame.top <= pt.y && rcFrame.bottom >= pt.y )
		return this;

	return NULL;
}

void CItemBase::Click(bool bForce, bool rClick)
{
	if( !bForce && (m_nState & ISTATE_DISABLED) )
		return;

	CItemBase*& pClicked = m_pRoot->m_pClicked;
	pClicked = this;
	bool res = OnClicked(this);
	pClicked = NULL;

	if( !res )
	{
		if( m_pBinding )
			m_pBinding->ProcessReflection(rClick ?
				CItemBinding::hOnRClick : CItemBinding::hOnClick);
	}
}

void CItemBase::DblClick()
{
	if( m_nState & ISTATE_DISABLED )
		return;

	LoadItemMenu(NULL, LOAD_MENU_FLAG_DEFITEM|LOAD_MENU_FLAG_DESTROY);
	OnDblClicked(this);
}

void CItemBase::Select()
{
	if( !(m_nFlags & STYLE_ENUM_VALUE) )
	{
		OnSelect(this);
		return;
	}

	if( m_nState & ISTATE_SELECTED )
		return;

	CItemBase * pEnumOwner = GetByFlagsAndState(STYLE_ENUM, ISTATE_ALL, NULL, CItemBase::gbfasUpToParent);
	if( !pEnumOwner )
		return;

	if( m_pOwner ) m_pOwner->LockUpdate(true, false);
	
	pEnumOwner->m_nState |= ISTATE_DATACHANGING;
	
	bool bChanged = false;
	for(CItemBase * p = NULL; p = _ItemBase_GetNextEnumItem(pEnumOwner, p);)
	{
		if( this == p )
		{
			if( !(p->m_nState & ISTATE_SELECTED) )
				bChanged = true;
			
			p->Select(true);
		}
		else
			p->Select(false);
	}
	
	pEnumOwner->m_nState &= ~ISTATE_DATACHANGING;

	if( bChanged )
	{
		pEnumOwner->SetChangedData();
		pEnumOwner->m_nState |= ISTATE_NEED_UPDATE;
	}

	if( m_pOwner ) m_pOwner->LockUpdate(false, true);
}

void CItemBase::Show(bool bShow, tDWORD nFlags)
{
	if( m_bHideExpanded || m_bHideCollapsed )
	{
		// поддержка управления видимостью в раздвигаемых диалогах
		CDialogItem* pDlg = (CDialogItem*)GetOwner(true);
		if( pDlg && pDlg->IsExpandable() )
		{
			// чтобы биндинг не перебил невидимость элемента, 
			// при необходиости делаем его невидимым явно
			if( m_bHideExpanded && pDlg->IsExpanded() )
				bShow = false;
			if( m_bHideCollapsed && !pDlg->IsExpanded() )
				bShow = false;
		}
	}

	if( !(nFlags & SHOWITEM_DIRECT) )
	{
		if( m_pEditData )
			return;

		if( m_pParent && (m_pParent->m_nFlags & STYLE_SHEET_PAGES) )
		{
			CItemBase * pSheet = m_pParent->GetByFlagsAndState(STYLE_SHEET_ITEM, ISTATE_ALL, NULL, gbfasUpToParent);
			if( pSheet )
				pSheet->ShowPageItem(this, bShow);
			return;
		}
	}
	
	if( bShow == !(m_nState & ISTATE_HIDDENEX) )
		return;

	if( bShow )
		m_nState &= ~ISTATE_HIDDENEX;
	else
		m_nState |= ISTATE_HIDDENEX;

	LockUpdate(true);
	_CheckShow(!(nFlags & SHOWITEM_NORECALC));
	
	if( m_nState & ISTATE_INITED )
		OnChangedState(this, ISTATE_HIDE);

	LockUpdate(false);
}

void CItemBase::Show(LPCSTR strItem, bool bShow)
{
	CItemBase * pItem = strItem ? GetItem(strItem) : this;
	if( pItem )
		pItem->Show(bShow);
}

void CItemBase::_CheckShow(bool bRecacl)
{
	bool bOn = !(m_nState & (ISTATE_HIDDENEX));
	if( m_pParent && (m_pParent->m_nState & ISTATE_HIDE) )
		bOn = false;

	bool bChanged = (bOn != !(m_nState & ISTATE_HIDE));
	if( bChanged )
	{
		if( bOn )
			m_nState &= ~ISTATE_HIDE;
		else
			m_nState |= ISTATE_HIDE;
		
		OnSyncProps();
	}

	if( bRecacl && (m_nState & ISTATE_INITED) )
		RecalcLayout(false);

	if( bChanged )
	{
		if( m_nState & ISTATE_INITED )
		{
			if( !m_strTipText.empty() )
				m_pRoot->EnableToolTip(this, bOn);
		}
	}

	for(tSIZE_T i = 0; i < GetChildrenCount(); i++)
		GetByPos(i)->_CheckShow();

	if( bChanged && (m_nState & ISTATE_INITED) )
		OnUpdateProps(UPDATE_FLAG_POSITION|UPDATE_FLAG_SIZE, ISTATE_HIDE);
}

void CItemBase::Enable(LPCSTR strItem, bool bOn, bool bDisableOnly)
{
	CItemBase * pItem = strItem ? GetItem(strItem) : this;
	if( pItem )
	{
		if( !bOn || !bDisableOnly )
			pItem->Enable(bOn);
	}
}

void CItemBase::Enable(bool bEnable)
{
	if( bEnable == !(m_nState & ISTATE_DISABLEDEX) )
		return;

	if( bEnable )
		m_nState &= ~ISTATE_DISABLEDEX;
	else
		m_nState |= ISTATE_DISABLEDEX;

	_CheckEnabled();
}

void CItemBase::Freeze(bool bFreeze)
{
	if( m_nFlags & STYLE_ENUM )
	{
		for(size_t i = 0; i < m_aItems.size(); i++)
			m_aItems[i]->Freeze(bFreeze);
	
		return;
	}

	if( bFreeze == !!(m_nState & ISTATE_FROZEN) )
		return;

	if( bFreeze )
		m_nState |= ISTATE_FROZEN;
	else
		m_nState &= ~ISTATE_FROZEN;

	_CheckEnabled();
}

void CItemBase::EnableDep(bool bEnable)
{
	if( bEnable == !(m_nState & ISTATE_DISABLEDDEP) )
		return;

	if( bEnable )
		m_nState &= ~ISTATE_DISABLEDDEP;
	else
		m_nState |= ISTATE_DISABLEDDEP;

	_CheckEnabled();
}

void CItemBase::_CheckEnabled()
{
	bool bOn = !(m_nState & (ISTATE_FROZEN|ISTATE_DISABLEDEX|ISTATE_DISABLEDDEP));

	CItemBase * pParentCheck = m_pParent;
	if( m_pParent && (m_nFlags & STYLE_NOPARENTDISABLE) )
		pParentCheck = m_pParent->m_pParent;
	
	if( pParentCheck && !(pParentCheck->m_nFlags & STYLE_TOP_ITEM) &&
		(pParentCheck->m_nState & ISTATE_DISABLED) )
		bOn = false;

	if( bOn != !(m_nState & ISTATE_DISABLED) )
	{
		if( bOn )
			m_nState &= ~ISTATE_DISABLED;
		else
			m_nState |= ISTATE_DISABLED;

		if( m_pBgCtx )
			m_pBgCtx->m_bGrayed = !bOn;
		
		OnSyncProps();
		
		if( m_nState & ISTATE_INITED )
			OnUpdateProps(0, ISTATE_DISABLED);
		
		if( m_pSink )
			m_pSink->OnEnable(bOn);
	}

	for(tSIZE_T i = 0; i < GetChildrenCount(); i++)
		GetByPos(i)->_CheckEnabled();
}

void CItemBase::OnUpdateProps(int flags, tDWORD nStateMask, RECT * rc)
{
	if( flags & UPDATE_FLAG_NO_UPDATE )
		return;

	if( !m_strTipText.empty() && (flags & (UPDATE_FLAG_SIZE|UPDATE_FLAG_POSITION)) )
		m_pRoot->EnableToolTip(this, true);

	if( (nStateMask & (ISTATE_DISABLED|ISTATE_HIDE|ISTATE_HOTLIGHT|ISTATE_SELECTED)) || flags )
		Update();
}

LPCSTR CItemBase::OnGetDisplayText()
{
	return m_strText.c_str();
}

void CItemBase::Focus(bool bFocus)
{
	OnFocus(bFocus);
	Update();
}

void CItemBase::OnFocus(bool bFocusted)
{
	if( bFocusted )
		m_nState |= ISTATE_FOCUSED;
	else
		m_nState &= ~ISTATE_FOCUSED;
}

void CItemBase::SetDefault(bool bOn)
{
	if( bOn )
		m_nState |= ISTATE_DEFAULT;
	else
		m_nState &= ~ISTATE_DEFAULT;

	if( m_nState & ISTATE_INITED )
		OnUpdateProps(0, ISTATE_DEFAULT);
}

void CItemBase::Update(RECT *rc)
{
	if( !(m_nState & ISTATE_INITED) )
		return;

	if( (m_nFlags & STYLE_WINDOW) && (!(m_nFlags & STYLE_NON_OWNER) || (m_nFlags & STYLE_TRANSPARENT)) )
		return;
	
//	if( m_nState & ISTATE_RESIZING )
//		return;
	
	if( !m_pOwner || (m_pParent && (m_pParent->m_nState & ISTATE_HIDE)) )
		return;

	RECT rcOwner; GetParentRect(rcOwner);
	RECT rcUpdate;
	if( rc )
	{
		rcUpdate = *rc;
		rcUpdate.left += rcOwner.left, rcUpdate.right += rcOwner.left;
		rcUpdate.top += rcOwner.top, rcUpdate.bottom += rcOwner.top;
	}
	else
	{
		rcUpdate = rcOwner;
		OnAdjustUpdateRect(rcUpdate);
	}

	if( m_lockUpdate )
	{
		m_pOwner->InvalidateEx(&rcUpdate);
		m_nState |= ISTATE_NEED_UPDATE;
	}
	else
		m_pOwner->Update(&rcUpdate);
}

void CItemBase::UpdateProps(int nFlags, tPTR pData)
{
	for(size_t i = 0; i < m_aItems.size(); i++)
		m_aItems[i]->UpdateProps(nFlags, pData);

	OnUpdateProps(nFlags|UPDATE_FLAG_NO_UPDATE, ISTATE_ALL);
}

void CItemBase::Resize(tDWORD cx, tDWORD cy, bool bDirect)
{
	RECT rcRepaint = {0, 0, m_szSize.cx, m_szSize.cy};

	if( m_lockUpdate )
		Update();

	bool bChangedX = false, bChangedY = false;
	if( cx && (bDirect || IsResizeble(true)) )
	{
		m_szSizeWoScale.cx += cx;
		if( m_ptResizeScale.x && m_ptResizeScale.x < 100 )
			m_szSize.cx = m_szSizeMin.cx + (m_szSizeWoScale.cx - m_szSizeMin.cx)*m_ptResizeScale.x/100;
		else
			m_szSize.cx = m_szSizeWoScale.cx;
		bChangedX = true;
	}

	if( cy && (bDirect || IsResizeble(false)) )
	{
		m_szSizeWoScale.cy += cy;
		if( m_ptResizeScale.y && m_ptResizeScale.y < 100 )
			m_szSize.cy = m_szSizeMin.cy + (m_szSizeWoScale.cy - m_szSizeMin.cy)*m_ptResizeScale.y/100;
		else
			m_szSize.cy = m_szSizeWoScale.cy;
		bChangedY = true;
	}

	RecalcLayout(!bDirect);

	if( bChangedX || bChangedY )
	{
		if( m_nState & ISTATE_INITED )
			OnUpdateProps(UPDATE_FLAG_SIZE);

		if( (m_nFlags & AUTOSIZE) != AUTOSIZE )
		{
			if( m_nFlags & AUTOSIZE_X )
				bChangedX = false;
			if( m_nFlags & AUTOSIZE_Y )
				bChangedY = false;
			
			if( !(m_nFlags & STYLE_CUSTOMCHILDREN) )
				for(size_t i = 0, n = m_aItems.size(); i < n; i++)
				{
					CItemBase * pItem = m_aItems[i];
					if( !(pItem->m_nFlags & (STYLE_TOP_ITEM|STYLE_OWN_PLACEMENT)) )
						pItem->Resize(bChangedX ? cx : 0, bChangedY ? cy : 0, false);
				}
		}
	}
}

void CItemBase::SetRect(RECT rc)
{
	m_rcShift.left = rc.left;
	m_rcShift.top = rc.top;
	//Reoffset();
	
	Resize(RECT_CX(rc) - m_szSize.cx, RECT_CY(rc) - m_szSize.cy, true);
}

void CItemBase::Reoffset()
{
	if( m_nState & ISTATE_HIDDENEX )
		return;

	if( !(m_nFlags & STYLE_CUSTOMCHILDREN) )
		for(size_t i = 0; i < m_aItems.size(); i++)
			m_aItems[i]->Reoffset();

	if( m_nState & ISTATE_INITED )
		OnUpdateProps(UPDATE_FLAG_POSITION|UPDATE_FLAG_NO_UPDATE);
}

void CItemBase::TimerRefresh(tDWORD nTimerSpin)
{
	if( m_pSink )
		m_pSink->OnTimerRefresh(nTimerSpin);

	for( size_t i = 0; i < m_aItems.size(); i++ )
		if( !( m_aItems[i]->m_nState & ISTATE_HIDE ) )
			m_aItems[i]->TimerRefresh(nTimerSpin);
}

void CItemBase::GetParentRect(RECT& rc, CItemBase* pOwner)
{
	if( pOwner == this )
	{
		rc.left = 0;
		rc.top = 0;
		rc.right = m_szSize.cx;
		rc.bottom = m_szSize.cy;
		return;
	}

	rc.left = m_ptOffset.x;
	rc.top = m_ptOffset.y;

	if( !pOwner )
		pOwner = m_pOwner;

	CItemBase* pItem = m_pParent;
	while( pItem )
	{
		RECT rcParent;
		pItem->GetClientRectEx(rcParent, true);

		rc.left += rcParent.left;
		rc.top += rcParent.top;

		if( pItem == pOwner )
			break;

		if( pItem->m_nFlags & STYLE_TOP_ITEM )
		{
			RECT rcTop; pItem->GetParentRect(rcTop, m_pRoot);
			rc.left += rcTop.left;
			rc.top += rcTop.top;
			break;
		}
		else
		{
			rc.left += pItem->m_ptOffset.x;
			rc.top += pItem->m_ptOffset.y;
		}

		pItem = pItem->m_pParent;
	}

	rc.right = rc.left + m_szSize.cx;
	rc.bottom = rc.top + m_szSize.cy;
}

CItemBase* CItemBase::GetItem(const cStrData &strItemId, LPCSTR strItemType, bool bRecurse)
{
	CItemBase* pFind = GetNextItem(NULL, strItemId, strItemType, false);
	if( pFind )
		return pFind;

	if( !bRecurse )
		return NULL;

	for(CItemBase* pItem = GetChildFirst(false); pItem; pItem = pItem->GetNext(false))
		if( pFind = pItem->GetItem(strItemId, strItemType) )
			return pFind;
	
	return NULL;
}

CItemSink* CItemBase::GetSink(LPCSTR strItemId, LPCSTR strItemType, bool bRecurse)
{
	CItemBase* pItem = GetItem(strItemId, strItemType, bRecurse);
	if( !pItem || !pItem->m_pSink )
		return NULL;
	return pItem->m_pSink;
}

CItemBase* CItemBase::GetNextItem(CItemBase * pPrev, const cStrData &strItemId, LPCSTR strItemType, bool bRecurse, bool bBackward)
{
	if( !pPrev )
		pPrev = bRecurse ? this : (bBackward ? GetChildLast(false) : GetChildFirst(false));
	else
		pPrev = bRecurse ? EnumNextItem(pPrev, bBackward ? ENUMITEMS_BACKWARD : 0) : (bBackward ? pPrev->GetPrev(false) : pPrev->GetNext(false));

	for(; pPrev; pPrev = bRecurse ? EnumNextItem(pPrev, bBackward ? ENUMITEMS_BACKWARD : 0) : (bBackward ? pPrev->GetPrev(false) : pPrev->GetNext(false)))
	{
//		if( pPrev->CheckAttrs(strItemId, strItemType) )
//			break;
		if( strItemType &&
			(pPrev->m_strItemType != strItemType) && (pPrev->m_strItemAlias != strItemType) )
			continue;
		if( strItemId.p && strItemId.cmp(pPrev->m_strItemId) )
			continue;
		
		break;
	}

	return pPrev;
}

CItemBase* CItemBase::EnumNextItem(CItemBase * pPrev, tDWORD nFlags)
{
	CItemBase* pItem = EnumNextItemInternal(pPrev, nFlags);
	if( pItem )
		return pItem;
	
	if( !(nFlags & ENUMITEMS_LOOPED) )
		return NULL;

	return EnumNextItemInternal(NULL, nFlags);
}

CItemBase* CItemBase::EnumNextItemInternal(CItemBase * pPrev, tDWORD nFlags)
{
	CItemBase * pNextItem = NULL;

	if( nFlags & ENUMITEMS_BACKWARD )
	{
		if( pPrev )
		{
			if( pPrev == this )
				return NULL;

			if( pPrev->m_pParent )
			{
				if( nFlags & ENUMITEMS_BYORDER )
					pNextItem = pPrev->m_pParent->GetNextOrderChild(pPrev, true);
				else
					pNextItem = pPrev->GetPrev(false);
			}
			
			if( !pNextItem )
				return pPrev->m_pParent;
		}
		
		if( !pPrev )
			pNextItem = this;

		do
		{
			pPrev = pNextItem;
			if( nFlags & ENUMITEMS_BYORDER )
				pNextItem = pPrev->GetNextOrderChild(NULL, true);
			else
				pNextItem = pPrev->GetChildLast(false);
		} while( pNextItem );

		return pPrev;
	}
	else
	{
		if( !pPrev )
			pPrev = this;

		if( !(nFlags & ENUMITEMS_SKIPCHILDREN) )
		{
			if( nFlags & ENUMITEMS_BYORDER )
				pNextItem = pPrev->GetNextOrderChild(NULL, false);
			else
				pNextItem = pPrev->GetChildFirst(false);
		}
		
		if( pNextItem )
			return pNextItem;

		if( pPrev == this )
			return NULL;
		
		do
		{
			if( pPrev->m_pParent )
			{
				if( nFlags & ENUMITEMS_BYORDER )
					pNextItem = pPrev->m_pParent->GetNextOrderChild(pPrev, false);
				else
					pNextItem = pPrev->GetNext(false);
			}
			
			if( pNextItem )
				return pNextItem;
			
			pNextItem = pPrev->m_pParent;
			if( pNextItem == this )
				pNextItem = NULL;
			
		} while( pPrev = pNextItem );
	}

	return NULL;
}

CItemBase* CItemBase::GetDefItem()
{
	for(CItemBase * pItem = GetChildFirst(); pItem; pItem = pItem->GetNext())
	{
		if( pItem->m_nState & ISTATE_DEFAULT )
			return( pItem );
		
		if( CItemBase * pDef = pItem->GetDefItem() )
			return( pDef );
	}

	return( NULL );
}

tDWORD CItemBase::GetItemId()
{
	return ((tDWORD)(this) >> 16) ^ ((tDWORD)(this) & 0x0000ffff);
}

CItemBase* CItemBase::GetById(tDWORD nId)
{
	for(CItemBase * pItem = NULL; pItem = EnumNextItem(pItem); )
		if( pItem->GetItemId() == nId )
			return pItem;
	
	return NULL;
}

CItemBase* CItemBase::GetByFlagsAndState(tQWORD nFlags, tDWORD nState, CItemBase * pPrev, tDWORD nMode)
{
#define _GBFS_CHECK(_n, _m, _all)	((_m == _all) ? true : !!(_n & _m))
	
	if( (nMode & 0xFFFF) == gbfasUpToParent )
	{
		if( _GBFS_CHECK(m_nFlags, nFlags, STYLE_ALL) && _GBFS_CHECK(m_nState, nState, ISTATE_ALL) )
			return this;
		return m_pParent ? m_pParent->GetByFlagsAndState(nFlags, nState, pPrev, nMode) : NULL;
	}
	
	for(CItemBase * pItem = pPrev; pItem = GetNextItem(pItem, NULL, NULL, (nMode & 0xFFFF) == gbfasRecurse, !!(nMode & gbfasBackward)); )
		if( _GBFS_CHECK(pItem->m_nFlags, nFlags, STYLE_ALL) && _GBFS_CHECK(pItem->m_nState, nState, ISTATE_ALL) )
			return pItem;
	
	return NULL;

#undef _GBFS_CHECK
}

CItemBase* CItemBase::GetItemByPath(LPCSTR strItemPath, bool bGoUp, bool bExact)
{
	if( !strItemPath || !*strItemPath )
		return this;
	
	cStrData strItemId;
	if( LPCSTR strSep = strchr(strItemPath, '.') )
	{
		strItemId.p = strItemPath;
		strItemId.n = strSep - strItemPath;
		strItemPath = strSep + 1;
	}
	else
	{
		strItemId.p = strItemPath;
		strItemId.n = strlen(strItemPath);
		strItemPath = "";
	}

	if( !strItemId.cmp("parent", sizeof("parent") - 1) )
		return m_pParent->GetItemByPath(strItemPath, false);

	if( bExact )
	{
		CItemBase * pItem = GetItem(strItemId, NULL, false);
		if( !pItem )
			return NULL;
		return pItem->GetItemByPath(strItemPath, false, true);
	}
	
	for(CItemBase * pStart = this; pStart; pStart = pStart->m_pParent)
	{
		for( CItemBase * pItem = pStart; pItem = pStart->GetNextItem(pItem, strItemId); )
			if( CItemBase * pRes = pItem->GetItemByPath(strItemPath, false) )
				return pRes;

		if( !strItemId.cmp(pStart->m_strItemId) )
			if( CItemBase * pRes = pStart->GetItemByPath(strItemPath, false) )
				return pRes;

		if( !bGoUp || pStart->m_nFlags & STYLE_TOP_ITEM )
			break;
	}
	
	return NULL;
}

CItemBase* CItemBase::GetItem(HWND hWnd, CItemBase * pPrev, bool bBackward)
{
	if( !hWnd )
		return NULL;
	
	tSIZE_T nTotal = pPrev ? GetTotalChildrenCount() : 0;
	
	tDWORD nFlags = (pPrev ? (ENUMITEMS_BYORDER|ENUMITEMS_LOOPED) : 0)|(bBackward ? ENUMITEMS_BACKWARD : 0);
	CItemBase * pItem = pPrev;
	while( pItem = EnumNextItem(pItem, nFlags) )
	{
		if( pItem == pPrev )
			break;
		
		if( pItem->GetWindow() == hWnd )
			return pItem;

		if( pPrev && !(--nTotal) )	// For break potentional infinite result of ENUMITEMS_LOOPED flag
			break;
	}

	return NULL;
}

CItemBase* CItemBase::GetByPos(tSIZE_T nIdx)
{
	return (nIdx < m_aItems.size()) ? m_aItems[nIdx] : NULL;
}

tSIZE_T CItemBase::GetChildrenCount()
{
	return m_aItems.size();
}

tSIZE_T CItemBase::GetTotalChildrenCount()
{
	tSIZE_T nTotal = 0;
	
	tSIZE_T i = 0, n = m_aItems.size();
	for(; i < n; i++)
		nTotal += m_aItems[i]->GetTotalChildrenCount();

	return nTotal + n;
}

bool CItemBase::IsParent(CItemBase* pParent)
{
	CItemBase* pItem = m_pParent;
	while(pItem)
	{
		if( pItem == pParent )
			return true;
		pItem = pItem->m_pParent;
	}
	return false;
}

long CItemBase::IsChecked(LPCSTR strItem)
{
	CItemBase * pItem = strItem ? GetItem(strItem) : this;
	if( !pItem )
		return 0;
	
	switch(pItem->GetType())
	{
	case tid_BOOL:
	case tid_DWORD:
	{
		cVariant l_val; pItem->GetValue(l_val);
		return l_val ? 1 : 0;
	}
	}

	return 0;
}

bool CItemBase::IsEnabled(LPCSTR strItem)
{
	CItemBase * pItem = strItem ? GetItem(strItem) : this;
	return pItem && !(pItem->m_nState & ISTATE_DISABLED);
}

bool CItemBase::IsVisible(LPCSTR strItem)
{
	CItemBase * pItem = strItem ? GetItem(strItem) : this;
	if( !pItem )
		return false;
	
	return !(pItem->m_nState & ISTATE_HIDE);
}

CItemBase* CItemBase::GetOwner(bool bTop, LPCSTR strItemId)
{
	for(CItemBase* pOwner = this; pOwner; pOwner = pOwner->m_pParent )
		if( pOwner->IsOwner() )
			if( !bTop || (pOwner->m_nFlags & STYLE_TOP_ITEM) )
			{
				if( strItemId && pOwner->m_strItemId != strItemId )
					return NULL;
				return pOwner;
			}

	return m_pRoot;
}

CItemBase* CItemBase::GetBottomItem()
{
	CItemBase* pChild = NULL;
	if( m_aItems.size() )
	{
		pChild = m_aItems[0];
		CItemBase* pNewChild = pChild->GetBottomItem();
		if( pNewChild )
			pChild = pNewChild;
	}
	return pChild;
}

CItemBase* CItemBase::GetNext(bool bVisible)
{
	size_t nCount = m_pParent->m_aItems.size();
	for(size_t i = m_nPos+1; i < nCount; i++)
	{
		CItemBase* pItem = m_pParent->m_aItems[i];
		if( !bVisible || !(pItem->m_nState & ISTATE_HIDDENEX) )
			return pItem;
	}
	return NULL;
}

CItemBase* CItemBase::GetPrev(bool bVisible)
{
	for(tNATIVE_INT i = m_nPos - 1; i >= 0; i--)
	{
		CItemBase* pItem = m_pParent->m_aItems[i];
		if( !bVisible || !(pItem->m_nState & ISTATE_HIDDENEX) )
			return pItem;
	}
	return NULL;
}

CItemBase* CItemBase::GetChildFirst(bool bVisible)
{
	if( !m_aItems.size() )
		return NULL;
	
	CItemBase* pItem = m_aItems[0];
	if( !bVisible || !(pItem->m_nState & ISTATE_HIDDENEX) )
		return pItem;
	return pItem->GetNext();
}

CItemBase* CItemBase::GetChildLast(bool bVisible)
{
	if( !m_aItems.size() )
		return NULL;
	
	CItemBase* pItem = m_aItems[m_aItems.size() - 1];
	if( !bVisible || !(pItem->m_nState & ISTATE_HIDDENEX) )
		return pItem;
	return pItem->GetPrev();
}

void CItemBase::GetMinSize(SIZE& szSizeMin)
{
	if( m_nState & ISTATE_HIDDENEX )
	{
		szSizeMin.cx = szSizeMin.cy = 0;
		return;
	}
	
	szSizeMin.cx = m_szSizeMin.cx;
	szSizeMin.cy = m_szSizeMin.cy;
	
	if( !(m_nFlags & AUTOSIZE) )
		return;

	RECT rcClient; GetClientRectEx(rcClient, true);
	
	SIZE szMargins;
	szMargins.cx = rcClient.left + (m_szSize.cx - rcClient.right);
	szMargins.cy = rcClient.top + (m_szSize.cy - rcClient.bottom);

	if( m_nFlags & AUTOSIZE_X )
		szSizeMin.cx = 0;
	
	if( m_nFlags & AUTOSIZE_Y )
		szSizeMin.cy = 0;

	for(CItemBase * pItem = GetChildFirst(); pItem; pItem = pItem->GetNext())
	{
		if( (m_nFlags & AUTOSIZE_X) && !(pItem->m_nFlags & ALIGN_RIGHT) )
		{
			int nPos = szMargins.cx + pItem->m_ptOffset.x + pItem->m_szSizeMin.cx;
			if( szSizeMin.cx < nPos )
				szSizeMin.cx = nPos;
		}

		if( (m_nFlags & AUTOSIZE_Y) && !(pItem->m_nFlags & ALIGN_BOTTOM) )
		{
			int nPos = szMargins.cy + pItem->m_ptOffset.y + pItem->m_szSizeMin.cy;
			if( szSizeMin.cy < nPos )
				szSizeMin.cy = nPos;
		}
	}
}

CPopupMenuItem * CItemBase::LoadItemMenu(POINT * ptMenu, tDWORD nFlags, CItemBase * pPlaceItem, CSinkCreateCtx *pCtx)
{
	CSinkCreateCtx dummyCtx(this);
	if( !pCtx )
		pCtx = &dummyCtx;

	LPCSTR strMenuInfo = NULL;
	cVariant vMenuInfo;
	if( pCtx->m_args && pCtx->m_argc )
	{
		pCtx->m_args[0]->Exec(vMenuInfo, *pCtx->m_ctx);
		if( vMenuInfo.Type() == cVariant::vtString )
			strMenuInfo = vMenuInfo.c_str();
		if( !strMenuInfo )
			return NULL;
	}

	CPopupMenuItem * pMenu = (CPopupMenuItem *)m_pRoot->CreateItem(NULL, GUI_ITEMT_MENU);
	if( !pMenu )
		return NULL;
	
	pMenu->m_strItemId = "PopupMenu";
	pMenu->m_strItemType = GUI_ITEMT_MENU;
	pMenu->m_nFlags |= STYLE_CUSTOMCHILDREN;
	AddItem(pMenu);
	
	if( !pPlaceItem )
		pPlaceItem = this;

	pMenu->PreBind(pMenu, pCtx);
	LoadMenuInternal(pMenu, this, strMenuInfo, pCtx);
	pMenu->PreInit(pMenu, pCtx);
	pMenu->UpdateData(true);
	
	pMenu->_CheckEnabled();
	pMenu->_CheckShow();

	if( !pMenu->GetChildrenCount() )
	{
		pMenu->Destroy();
		return NULL;
	}
	
	pMenu->SetFlags(nFlags);
	
	if( pPlaceItem->m_nFlags & STYLE_OWN_PLACEMENT )
	{
	}
	else if( nFlags & LOAD_MENU_FLAG_BOTTOMOFITEM )
	{
		RECT rc; pPlaceItem->GetParentRect(rc, pPlaceItem->m_pRoot);
		POINT ptItem = {rc.left, rc.bottom};
		pMenu->SetPt(ptItem);
	}
	else if( ptMenu )
		pMenu->SetPt(*ptMenu);

	bool bShow = true;
	if( pMenu->m_pSink && !pMenu->m_pSink->OnMenu(pMenu, this) )
		bShow = false;

	for(CItemBase * pParent = this; pParent && bShow; pParent = pParent->m_pParent)
	{
		if( pParent->m_pSink && !pParent->m_pSink->OnMenu(pMenu, this) )
			bShow = false;

		if( pParent->m_nFlags & STYLE_TOP_ITEM )
			break;
	}

	if( bShow )
	{
		bool dataChanged = false;
		pMenu->Track(NULL, &dataChanged);
		if( pCtx->m_ctx )
			pCtx->m_ctx->m_datachanged = dataChanged;
	}
	
	if( nFlags & LOAD_MENU_FLAG_DESTROY )
		pMenu->Destroy(), pMenu = NULL;
	return pMenu;
}

bool CItemBase::IsResizeble(bool bHorz)
{
	if( bHorz )
	{
		if( m_nFlags & AUTOSIZE_PARENT_X )
			return false;
		if( (m_nFlags & AUTOSIZE_X) && !(m_nFlags & STYLE_TOP_ITEM) )
			return false;
		if( !m_ptResizeScale.x )
			return false;
	}
	else
	{
		if( m_nFlags & (AUTOSIZE_PARENT_Y|TEXT_SINGLELINE) )
			return false;
		if( (m_nFlags & AUTOSIZE_Y) && !(m_nFlags & STYLE_TOP_ITEM) )
			return false;
		if( !m_ptResizeScale.y )
			return false;
	}
	
	return true;
}

void CItemBase::InitSize()
{
	SIZE szNew = m_szSizeMin;

// x
	if( !szNew.cx && m_pBackground )
		szNew.cx = m_pBackground->Size().cx;

	if( !szNew.cx && (m_strItemType.empty() && !m_strText.empty()) && !(m_nFlags & (ALIGN_HCENTER|ALIGN_VCENTER|AUTOSIZE_Y|AUTOSIZE_PARENT_X)) )
		m_nFlags |= TEXT_SINGLELINE;

	if( !(m_nFlags & (AUTOSIZE_X|AUTOSIZE_PARENT_X)) )
		m_szSize.cx = szNew.cx;

// y
	if( !szNew.cy && m_pBackground )
		szNew.cy = m_pBackground->Size().cy;

	if( !(m_nFlags & (AUTOSIZE_Y|AUTOSIZE_PARENT_Y)) )
		m_szSize.cy = szNew.cy;

	m_szSizeWoScale = m_szSize;
}

void CItemBase::GetClientRectEx(RECT& rcClient, bool bMargined, bool bCorrect)
{
	rcClient.left =	rcClient.top = 0;
	rcClient.right = m_szSize.cx;
	rcClient.bottom = m_szSize.cy;

	if( bMargined )
	{
		rcClient.left   += m_rcMargin.left;
		rcClient.top    += m_rcMargin.top;
		rcClient.right  -= m_rcMargin.right;
		rcClient.bottom -= m_rcMargin.bottom;
	}
	
	OnAdjustClientRect(rcClient);

	if( bCorrect )
	{
		if( rcClient.left > rcClient.right )
			rcClient.right = rcClient.left;
		if( rcClient.top > rcClient.bottom )
			rcClient.bottom = rcClient.top;
	}
}

void CItemBase::RecalcPlacement(bool bSize)
{
	SIZE  szClientAvail, szClient;
	{
		RECT rcParent = {0, 0, 0, 0};
		if( m_pParent )
			m_pParent->GetClientRectEx(rcParent, true);
		szClient.cx = RECT_CX(rcParent);
		szClient.cy = RECT_CY(rcParent);

		szClientAvail = szClient;
	}

	CItemBase *pPrev = (m_nFlags & ALIGN_LINKED) ? GetPrev() : NULL;

	// Recalc size
	SIZE szNew = m_szSize;
	
	if( bSize )
	{
		// Не надо проверять автосайз у родителя, т.к. размер родителя зависит только от элементов без размера по родителю
		if( m_pParent/* && !(m_pParent->m_nFlags & AUTOSIZE_X)*/ )
		{
			if( (m_nFlags & ALIGN_RIGHT) || ((m_nFlags & ALIGN_HBEFORE) && !pPrev) )
				szClientAvail.cx -= m_rcShift.right;
			else if( m_nFlags & ALIGN_HCENTER )
				szClientAvail.cx -= m_rcShift.left;
			else if( (m_nFlags & ALIGN_HAFTER) && pPrev )
				szClientAvail.cx -= pPrev->m_ptOffset.x + pPrev->m_szSize.cx + pPrev->m_rcShift.right + m_rcShift.left + m_rcShift.right;
			else if( (m_nFlags & ALIGN_HBEFORE) && pPrev )
				szClientAvail.cx -= m_rcShift.right + (szClientAvail.cx - pPrev->m_ptOffset.x) + ((pPrev->m_nFlags & ALIGN_REVERSE_X) ? pPrev->m_rcShift.left : 0) + m_rcShift.left;
			else if( (m_nFlags & ALIGN_HSAME) && pPrev )
				szClientAvail.cx -= pPrev->m_ptOffset.x + m_rcShift.left;
			else
				szClientAvail.cx -= m_rcShift.left + m_rcShift.right;
		}

		if( m_pParent/* && !(m_pParent->m_nFlags & AUTOSIZE_Y)*/ )
		{
			if( (m_nFlags & ALIGN_BOTTOM) || ((m_nFlags & ALIGN_VBEFORE) && !pPrev) )
				szClientAvail.cy -= m_rcShift.bottom;
			else if( m_nFlags & ALIGN_VCENTER )
				szClientAvail.cy -= m_rcShift.top;
			else if( (m_nFlags & ALIGN_VAFTER) && pPrev )
				szClientAvail.cy -= pPrev->m_ptOffset.y + pPrev->m_szSize.cy + pPrev->m_rcShift.bottom + m_rcShift.top + m_rcShift.bottom;
			else if( (m_nFlags & ALIGN_VBEFORE) && pPrev )
				szClientAvail.cy -= m_rcShift.bottom + (szClientAvail.cy - pPrev->m_ptOffset.y) + ((pPrev->m_nFlags & ALIGN_REVERSE_Y) ? pPrev->m_rcShift.top : 0) + m_rcShift.top;
			else if( (m_nFlags & ALIGN_VSAME) && pPrev )
				szClientAvail.cy -= pPrev->m_ptOffset.y + m_rcShift.top;
			else
				szClientAvail.cy -= m_rcShift.top + m_rcShift.bottom;
		}

		if( szClientAvail.cx < 0 )
			szClientAvail.cx = 0;
		if( szClientAvail.cy < 0 )
			szClientAvail.cy = 0;

		if( (m_nFlags & AUTOSIZE) && m_pFont && (m_strSection.empty() || (m_nFlags & STYLE_CUSTOMCHILDREN)) )
		{
			SIZE l_szTxt = {MAX_CTL_SIZE, MAX_CTL_SIZE}, l_szAdjust;
			RECT rcCli;
			GetClientRectEx(rcCli, true);
			
			{
				RECT rcAdjust = {0, 0, MAX_CTL_SIZE, MAX_CTL_SIZE};
				OnAdjustClientRect(rcAdjust);
				rcAdjust.left   += m_rcMargin.left;
				rcAdjust.top    += m_rcMargin.top;
				rcAdjust.right  -= m_rcMargin.right;
				rcAdjust.bottom -= m_rcMargin.bottom;

				l_szAdjust.cx = MAX_CTL_SIZE - RECT_CX(rcAdjust);
				l_szAdjust.cy = MAX_CTL_SIZE - RECT_CY(rcAdjust);
			}

			if( !(m_nFlags & (AUTOSIZE_X|AUTOSIZE_PARENT_X)) )
				l_szTxt.cx = RECT_CX(rcCli);
			else if( m_pParent && !(m_pParent->m_nFlags & AUTOSIZE_X) )
				l_szTxt.cx = szClientAvail.cx - l_szAdjust.cx;
			if( m_szSizeMax.cx && l_szTxt.cx > m_szSizeMax.cx )
				l_szTxt.cx = m_szSizeMax.cx;
			
			if( !(m_nFlags & (AUTOSIZE_Y|AUTOSIZE_PARENT_Y)) || (m_nFlags & TEXT_SINGLELINE) )
				l_szTxt.cy = RECT_CY(rcCli);
			if( m_szSizeMax.cy && l_szTxt.cy > m_szSizeMax.cy )
				l_szTxt.cy = m_szSizeMax.cy;

			OnCalcTextSize(m_pRoot->GetDesktopDC(), l_szTxt,
				GetFontFlags() & ~(ALIGN_VCENTER|ALIGN_RIGHT|ALIGN_BOTTOM));
			
			GetAddSize(l_szTxt);
			l_szTxt.cx += m_rcMargin.left + m_rcMargin.right;
			l_szTxt.cy += m_rcMargin.top + m_rcMargin.bottom;

			if( m_nFlags & AUTOSIZE_X )
			{
				szNew.cx = l_szTxt.cx;
				if( m_pParent && !(m_pParent->m_nFlags & AUTOSIZE_X) && (szNew.cx > szClientAvail.cx) )
					szNew.cx -= szNew.cx - szClientAvail.cx;
			}

			if( m_nFlags & AUTOSIZE_Y )
				szNew.cy = l_szTxt.cy;
		
			if( m_szSizeMax.cy && szNew.cy > m_szSizeMax.cy )
				szNew.cy = m_szSizeMax.cy;

			if( (szNew.cx && szNew.cx < l_szTxt.cx) || (szNew.cy && szNew.cy < l_szTxt.cy) )
			{
				if( m_strTipText.empty() )
				{
					m_nState |= ISTATE_WRAPTIP;
					SetTipText(OnGetDisplayText());
				}
			}
			else if( m_nState & ISTATE_WRAPTIP )
			{
				m_nState &= ~ISTATE_WRAPTIP;
				SetTipText(NULL);
			}
		}

		if( (m_nFlags & AUTOSIZE_PARENT) && m_pParent )
		{
			if( (m_nFlags & AUTOSIZE_PARENT_X) && !(m_nFlags & AUTOSIZE_X) )
				szNew.cx = szClientAvail.cx;
			
			if( (m_nFlags & AUTOSIZE_PARENT_Y) && !(m_nFlags & AUTOSIZE_Y) )
				szNew.cy = szClientAvail.cy;
		}
	}

	// Recalc offset
	POINT ptNew = m_ptOffset;

	if( (m_nFlags & ALIGN_RIGHT) || ((m_nFlags & ALIGN_HBEFORE) && !pPrev) )
		ptNew.x = szClient.cx - (m_rcShift.right + szNew.cx);
	else if( m_nFlags & ALIGN_HCENTER )
		ptNew.x = (szClient.cx - szNew.cx)/2 - m_rcShift.left;
	else if( (m_nFlags & ALIGN_HAFTER) && pPrev )
		ptNew.x = pPrev->m_ptOffset.x + pPrev->m_szSize.cx + pPrev->m_rcShift.right + m_rcShift.left;
	else if( (m_nFlags & ALIGN_HBEFORE) && pPrev )
		ptNew.x = pPrev->m_ptOffset.x - (szNew.cx + m_rcShift.right) - ((pPrev->m_nFlags & ALIGN_REVERSE_X) ? pPrev->m_rcShift.left : 0);
	else if( (m_nFlags & ALIGN_HSAME) && pPrev )
		ptNew.x = pPrev->m_ptOffset.x + m_rcShift.left;
	else
		ptNew.x = m_rcShift.left;

	if( (m_nFlags & ALIGN_BOTTOM) || ((m_nFlags & ALIGN_VBEFORE) && !pPrev) )
		ptNew.y = szClient.cy - (m_rcShift.bottom + szNew.cy);
	else if( m_nFlags & ALIGN_VCENTER )
	{
		ptNew.y = (szClient.cy - szNew.cy)/2 - m_rcShift.top;
		if( ptNew.y < 0 )
			ptNew.y = 0;
	}
	else if( (m_nFlags & ALIGN_VAFTER) && pPrev )
		ptNew.y = pPrev->m_ptOffset.y + pPrev->m_szSize.cy + pPrev->m_rcShift.bottom + m_rcShift.top;
	else if( (m_nFlags & ALIGN_VBEFORE) && pPrev )
		ptNew.y = pPrev->m_ptOffset.y - (szNew.cy + m_rcShift.bottom) - ((pPrev->m_nFlags & ALIGN_REVERSE_Y) ? pPrev->m_rcShift.top : 0);
	else if( (m_nFlags & ALIGN_VSAME) && pPrev )
		ptNew.y = pPrev->m_ptOffset.y + m_rcShift.top;
	else
		ptNew.y = m_rcShift.top;
	
	// Update size
	bool bNeedUpdate = bSize && (m_szSize.cx != szNew.cx || m_szSize.cy != szNew.cy)
		|| ptNew.x != m_ptOffset.x || ptNew.y != m_ptOffset.y;

	RECT rcUpdate; bool bUpdateParent = false;
	if( bNeedUpdate )
	{
		bUpdateParent = m_pOwner && (m_nState & ISTATE_INITED) && !m_lockUpdate &&
			(!(m_nFlags & STYLE_WINDOW) || (m_nFlags & STYLE_NON_OWNER));

		if( bUpdateParent )
			GetParentRect(rcUpdate);
	}

	if( bSize && (m_szSize.cx != szNew.cx || m_szSize.cy != szNew.cy) )
		Resize(szNew.cx - m_szSize.cx, szNew.cy - m_szSize.cy);

	// Update offset
	if( (ptNew.x != m_ptOffset.x || ptNew.y != m_ptOffset.y) && !(m_nState & ISTATE_HIDDENEX) )
	{
		m_ptOffset = ptNew;
		Reoffset();
	}

	if( bUpdateParent )
		m_pOwner->Update(&rcUpdate);

	if( bNeedUpdate && !(m_nFlags & STYLE_WINDOW) )
		Update(NULL);
}

void CItemBase::OnCalcTextSize(HDC dc, SIZE& sz, tDWORD nFlags)
{
	LPCSTR strText = OnGetDisplayText();
	bool bTextEmpty = !strText || !(*strText);

	RECT rcBound = {0, 0, sz.cx, sz.cy};
	m_pFont->Draw(dc, bTextEmpty ? "W" : strText, rcBound, nFlags|TEXT_CALCSIZE);

	sz.cx = bTextEmpty ? 0 : RECT_CX(rcBound);
	sz.cy = RECT_CY(rcBound);
}

bool CItemBase::IsFit(CItemBase *pChild)
{
	RECT rect; GetClientRectEx(rect, true);
	RECT childRect;	pChild->GetParentRect(childRect, this);

	return childRect.left >= rect.left && childRect.right <= rect.right && 
		childRect.top >= rect.top && childRect.bottom <= rect.bottom;
}

// Пересчет расположения элемента
void CItemBase::RecalcLayout(bool bSize)
{
	if( m_nFlags & STYLE_DEBUG )
		debugBreak();

	if( !m_pParent )
		return;

	bool bUseDynamicShow = !!(m_nFlags & STYLE_DYNAMIC_SHOW);// && !(m_nState & ISTATE_HIDDENEX);// && !(m_pParent->m_nFlags & AUTOSIZE_PARENT);
	if( bUseDynamicShow )
		m_nState &= ~(ISTATE_HIDDENEX|ISTATE_HIDE);

	RecalcPlacement(bSize);
	
	if( bUseDynamicShow && !m_pParent->IsFit(this) )
	{
		m_nState |= (ISTATE_HIDDENEX|ISTATE_HIDE);
	}

	CItemBase *pNext = GetNext();
	while( pNext && (pNext->m_nFlags & ALIGN_LINKED) )
	{
		pNext->RecalcPlacement(!!(pNext->m_nFlags & AUTOSIZE_PARENT));
		pNext = pNext->GetNext();
	}

	tQWORD &nParentFlags = m_pParent->m_nFlags;
	if( (nParentFlags & AUTOSIZE) && !((m_nFlags & STYLE_TOP_ITEM) && (nParentFlags & STYLE_TOP_ITEM)) )
		m_pParent->RecalcParentSize();
}

void CItemBase::RecalcParentSize()
{
	RECT rcParentClient; GetClientRectEx(rcParentClient, true);
	
	SIZE szMargins;
	szMargins.cx = rcParentClient.left + (m_szSize.cx - rcParentClient.right);
	szMargins.cy = rcParentClient.top + (m_szSize.cy - rcParentClient.bottom);
	
	SIZE szNew = m_szSize;
	if( m_nFlags & AUTOSIZE_X )
		szNew.cx = 0;
	if( m_nFlags & AUTOSIZE_Y )
		szNew.cy = 0;
	
	CItemBase * pNext = NULL;
	for(CItemBase * pItem = GetChildFirst(); pItem; pItem = pNext)
	{
		pNext = pItem->GetNext();
		if( pItem->m_nFlags & STYLE_TOP_ITEM )
			continue;
		
		if( (m_nFlags & AUTOSIZE_X) && !(pItem->m_nFlags & AUTOSIZE_PARENT_X) )
		{
			int nPos = szMargins.cx + pItem->m_szSize.cx;
			if( pItem->m_nFlags & (ALIGN_RIGHT|ALIGN_HCENTER) )
				nPos += pItem->m_rcShift.right + pItem->m_rcShift.left;
			else
				nPos += pItem->m_ptOffset.x + pItem->m_rcShift.right;
			
			if( szNew.cx < nPos )
				szNew.cx = nPos;
		}

		// Не учитываем некорректно заданных относительных размеров
		if( (m_nFlags & AUTOSIZE_Y) && !(pItem->m_nFlags & AUTOSIZE_PARENT_Y) )
		{
			int nPos = szMargins.cy + pItem->m_szSize.cy;
			if( pItem->m_nFlags & (ALIGN_BOTTOM|ALIGN_VCENTER) )
				nPos += pItem->m_rcShift.bottom + pItem->m_rcShift.top;
			else
			{
				nPos += pItem->m_ptOffset.y;
				if( pNext || !(pItem->m_nFlags & STYLE_IFLASTNOEXTEND) )
					nPos += pItem->m_rcShift.bottom;
			}
			
			if( szNew.cy < nPos )
				szNew.cy = nPos;
		}
	}
	
	RECT rcUpdate;
	bool bUpdateParent = m_pOwner && (m_nState & ISTATE_INITED) && !m_lockUpdate &&
		(!(m_nFlags & STYLE_WINDOW) || (m_nFlags & STYLE_NON_OWNER));

	if( bUpdateParent )
		GetParentRect(rcUpdate);

	if( (szNew.cx != m_szSize.cx) || (szNew.cy != m_szSize.cy) )
		Resize(szNew.cx - m_szSize.cx, szNew.cy - m_szSize.cy);

	if( bUpdateParent )
		m_pOwner->Update(&rcUpdate);
}

bool CItemBase::SetText(LPCSTR strText)
{
	if( !strText )
		strText = "";
	if( m_strText == strText )
		return false;
	m_strText = strText;
	m_nState |= ISTATE_CHANGED;
	
	if( m_nFlags & STYLE_DEBUG )
		debugBreak();

	if( !(m_nState & ISTATE_INITED) )
		return false;

	RecalcLayout(true);
	OnUpdateProps(UPDATE_FLAG_TEXT);
	if( m_pParent && (m_pParent->m_nFlags & STYLE_SHEET_PAGES) )
	{
		CItemBase * pSheet = m_pParent->GetByFlagsAndState(STYLE_SHEET_ITEM, ISTATE_ALL, NULL, gbfasUpToParent);
		if( pSheet )
			pSheet->OnUpdateProps(UPDATE_FLAG_TEXT);
	}
	
	return true;
}

void CItemBase::SetFont(CFontStyle* pFont)
{
	if( !pFont )
		pFont = m_pRoot->GetFont(RES_DEF_FONT);

	if( m_pFont == pFont )
		return;
	
	m_pFont = pFont;
	
	if( !(m_nState & ISTATE_INITED) )
		return;

	RecalcLayout(true);
	OnUpdateProps(UPDATE_FLAG_FONT);
}

void CItemBase::SetIcon(CIcon* pIcon)
{
	if( m_pIcon == pIcon )
		return;
	
	m_pIcon.Clean();
	if( m_pIcon && (m_nFlags & STYLE_DESTROY_ICON) )
		m_pIcon->Destroy();
	m_pIcon = pIcon;
	if( m_pIcon && (m_nFlags & STYLE_DESTROY_ICON) )
		m_pIcon->AddRef();

	if( m_pIcon && !m_pBgCtx )
	{
		m_pBgCtx = new CBgContext();
		m_pBgCtx->m_bGrayed = !!(m_nState & ISTATE_DISABLED);
	}

	if( m_nState & ISTATE_INITED )
	{
		OnUpdateProps(UPDATE_FLAG_ICON);
		RecalcLayout(true);
	}
}

void CItemBase::SetIcon(LPCSTR strIcon)
{
	if( !strIcon )
	{
		SetIcon((CIcon *)NULL);
		return;
	}

	CItemPropVals prpIcons;
	prpIcons.Parse(strIcon, 0, true);
	
	if( int nCount = prpIcons.Count() )
	{
		SetIcon(m_pRoot->GetIcon(prpIcons.GetStr(0)));

		tDWORD bIdxChanged = 0;
		for(int i = 1; i < nCount; i++)
			if( !prpIcons.Get(i).empty() )
				bIdxChanged |= (tDWORD)m_pIcon.SetImage(i-1, (tDWORD)prpIcons.GetLong(i));

		if( bIdxChanged )
			OnUpdateProps(UPDATE_FLAG_ICON);
	}
	else
		SetIcon((CIcon *)NULL);
}


void CItemBase::SetBackground(CBackground* pBackground)
{
//	if( GUI_ISSTYLE_SIMPLE(m_pRoot) && !(m_nFlags & STYLE_FIX) )
//		return;
	
	if( m_pBackground == pBackground )
		return;
	
	m_pBackground.Clean();
	m_pBackground = pBackground;
	if( m_pBackground && !m_pBgCtx )
	{
		m_pBgCtx = new CBgContext();
		m_pBgCtx->m_bGrayed = !!(m_nState & ISTATE_DISABLED);
	}

	if( m_nState & ISTATE_INITED )
	{
		OnUpdateProps(UPDATE_FLAG_BACKGROUND);
		RecalcLayout(true);
	}
}

void CItemBase::SetBackground(LPCSTR strBackground)
{
	if( !strBackground )
	{
		SetBackground((CBackground *)NULL);
		return;
	}	

	CItemPropVals prpBackgrounds;
	prpBackgrounds.Parse(strBackground, 0, true);
	
	if( int nCount = prpBackgrounds.Count() )
	{
		SetBackground(m_pRoot->GetBackground(prpBackgrounds.GetStr(0)));
		
		tDWORD bIdxChanged = 0;
		for(int i = 1; i < nCount; i++)
			if( !prpBackgrounds.Get(i).empty() )
				bIdxChanged |= (tDWORD)m_pBackground.SetImage(i-1, (tDWORD)prpBackgrounds.GetLong(i));

		if( bIdxChanged )
			OnUpdateProps(UPDATE_FLAG_BACKGROUND);
	}
	else
		SetBackground((CBackground *)NULL);
}


bool CItemBase::SetTipText(LPCSTR strText)
{
	if( strText )
	{
		if( strText == m_strTipText )
			return true;
		m_strTipText = strText;
	}
	else
	{
		if( m_strTipText.empty() )
			return true;
		m_strTipText.erase();
	}
	m_pRoot->EnableToolTip(this, !m_strTipText.empty());
	return true;
}

void CItemBase::Hotlight(bool bOn)
{
	if( bOn == !!(m_nState & ISTATE_HOTLIGHT) )
		return;

	if( bOn )
		m_nState |= ISTATE_HOTLIGHT;
	else
		m_nState &= ~ISTATE_HOTLIGHT;

	OnChangedState(this, ISTATE_HOTLIGHT);

	if( m_nState & ISTATE_INITED )
		OnUpdateProps(0, ISTATE_HOTLIGHT);
}

void CItemBase::Select(bool bSelect)
{
	if( bSelect == !!(m_nState & ISTATE_SELECTED) )
		return;
	
	if( bSelect )
		m_nState |= ISTATE_SELECTED;
	else
		m_nState &= ~ISTATE_SELECTED;

	OnChangedState(this, ISTATE_SELECTED);

	if( m_nState & ISTATE_INITED )
		OnUpdateProps(0, ISTATE_SELECTED);
}

tDWORD CItemBase::GetFontFlags()
{
	tDWORD nFlags = (m_nTextFlags & ALIGN_TEXT_MASK) | ((tDWORD)m_nFlags & TEXT_SINGLELINE);
	if( m_nState & ISTATE_DISABLED )
		nFlags |= TEXT_GRAYED;
	if( m_nState & ISTATE_HOTLIGHT && !(m_nFlags & STYLE_UNDERLINEOFF) )
		nFlags |= TEXT_HOTLIGHT;
	if( (m_nState & ISTATE_FOCUSED) && !(m_nFlags & STYLE_WINDOW) && !(m_nFlags & STYLE_UNDERLINEOFF))
		nFlags |= TEXT_HOTLIGHT;
	return nFlags;
}

int CItemBase::GetStateImage()
{
	if( m_nState & ISTATE_DISABLED ) return 3;
	if( m_nState & ISTATE_SELECTED ) return 1;
	if( m_nState & ISTATE_HOTLIGHT ) return 5;
	if( m_nState & ISTATE_FOCUSED )  return 2;
	if( m_nState & ISTATE_DEFAULT )  return 4;
	
	return 0;
}

int CItemBase::GetIconMargSize()
{
	if( !m_pFont )
		return 2;
	
	RECT rcText = {0, 0, MAX_CTL_SIZE, MAX_CTL_SIZE};
	m_pFont->Draw(m_pRoot->GetDesktopDC(), " ", rcText, TEXT_CALCSIZE);
	return RECT_CX(rcText);
}

cNode * CItemBase::CreateOperand(const char *name, cNode *container, cAlloc *al)
{
	return m_pSink ? m_pSink->CreateOperand(name, container, al) : NULL;
}

cNode * CItemBase::CreateOperator(const char *name, cNode **args, cAlloc *al)
{
	return m_pSink ? m_pSink->CreateOperator(name, args, al) : NULL;
}

//////////////////////////////////////////////////////////////////////

void CItemBase::OnActivate(CItemBase* pItem, bool bActivate)
{
	if( m_pSink )
		m_pSink->OnActivate(pItem, bActivate);

	if( m_pParent )
		m_pParent->OnActivate(pItem, bActivate);
}

bool CItemBase::OnCanRememberData(CItemBase * pItem, bool bSave)
{
	if( m_pSink )
		if( !m_pSink->OnCanRememberData(pItem, bSave) )
			return false;
	
	if( !(m_nFlags & STYLE_TOP_ITEM)  && m_pParent )
		return m_pParent->OnCanRememberData(pItem, bSave);
	return true;
}

void CItemBase::OnSelect(CItemBase* pItem)
{
	if( m_pSink )
		if( m_pSink->OnSelect(pItem) )
			return;
	
	if( !(m_nFlags & STYLE_TOP_ITEM) && m_pParent )
		m_pParent->OnSelect(pItem);
}

bool CItemBase::OnClicked(CItemBase* pItem)
{
	if( m_pSink )
		if( m_pSink->OnClicked(pItem) )
			return true;

	if( !(m_nFlags & STYLE_TOP_ITEM) && m_pParent )
		if( m_pParent->OnClicked(pItem) )
			return true;

	return false;
}

void CItemBase::OnDblClicked(CItemBase* pItem)
{
	if( m_pSink )
		if( m_pSink->OnDblClicked(pItem) )
			return;

	if( !(m_nFlags & STYLE_TOP_ITEM)  && m_pParent )
		m_pParent->OnDblClicked(pItem);
}

bool CItemBase::OnChangingData(CItemBase * pItem)
{
	if( m_pBinding )
	{
		cVar vRes; m_pBinding->ProcessReflection(CItemBinding::hOnChanging, vRes);
		if( vRes.Type() != cVariant::vtVoid && !vRes )
			return false;
	}

//	if( m_pSink && !m_pSink->OnChangingData(pItem) )
//		return false;

	if( m_pParent && !(m_nFlags & STYLE_TOP_ITEM) && !m_pParent->OnChangingData(pItem) )
		return false;

	return true;
}

void CItemBase::OnChangedData(CItemBase* pItem)
{
	if( pItem == this && IsOut() )
	{
		if( m_pBinding && m_pBinding->GetHandler(CItemBinding::hValue) && !OnChangingData(this) )
		{
			m_pBinding->UpdateData(true);
			return;
		}

		OnChangedProp(CBindProps::prpValue);
		if( m_pBinding )
			m_pBinding->ProcessReflection(CItemBinding::hOnChange);
	}

	if( m_pSink )
		m_pSink->OnChangedData(pItem);

	if( m_pParent && !(m_nFlags & STYLE_TOP_ITEM) )
		m_pParent->OnChangedData(pItem);
}

void CItemBase::OnChangedState(CItemBase* pItem, tDWORD nStateMask)
{
	if( pItem == this )
		OnChangedProp(CBindProps::StateMask2PropID(nStateMask));

	if( m_pSink )
		m_pSink->OnChangedState(pItem, nStateMask);
	
	if( m_pParent && !(m_nFlags & STYLE_TOP_ITEM) )
		m_pParent->OnChangedState(pItem, nStateMask);
}

void CItemBase::OnMouseHover(CItemBase* pItem)
{
	if( !(m_nFlags & STYLE_TOP_ITEM) && m_pParent )
		m_pParent->OnMouseHover(pItem);
	if( m_pSink )
		m_pSink->OnMouseHover(pItem);
}

void CItemBase::OnMouseLeave(CItemBase* pItem)
{
	if( !(m_nFlags & STYLE_TOP_ITEM) && m_pParent )
		m_pParent->OnMouseLeave(pItem);
	if( m_pSink )
		m_pSink->OnMouseLeave(pItem);
}

void CItemBase::OnNavigate(CItemBase * pItem, const tString &strURL, int Flags, tCPTR pPostData, tSIZE_T nPostDataLen, const tString &strHeaders, bool &bCancel)
{
	if( m_pSink )
		if( m_pSink->OnNavigate(pItem, strURL, Flags, pPostData, nPostDataLen, strHeaders, bCancel) )
			return;

	if( !(m_nFlags & STYLE_TOP_ITEM) && m_pParent )
		m_pParent->OnNavigate(pItem, strURL, Flags, pPostData, nPostDataLen, strHeaders, bCancel);
}

//////////////////////////////////////////////////////////////////////
