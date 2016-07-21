// SerializableView.cpp: implementation of the CSerializableView class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include <Prague/iface/e_ktrace.h>
#include <ProductCore/structs/s_settings.h>
#include <ProductCore/binding.h>
#include "Formats.h"
#include <algorithm>

propid_t CBindProps::ConvertPropID(const char *propName)
{
	sswitch(propName)
	scase("value")    return prpValue;		sbreak;
	scase("icon")     return prpIcon;		sbreak;
	scase("font")     return prpFont;		sbreak;
	scase("bg")       return prpBackground;	sbreak;
	scase("visible")  return prpVisible;	sbreak;
	scase("enabled")  return prpEnabled;	sbreak;
	scase("animate")  return prpAnimated;	sbreak;
	scase("selected") return prpSelected;	sbreak;
	scase("hotlight") return prpHotlight;	sbreak;
	scase("text")     return prpText;		sbreak;
	send;
	
	return prpNone;
}

propid_t CBindProps::StateMask2PropID(tDWORD nStateMask)
{
	tDWORD prpMask = 0;
	if( nStateMask & ISTATE_HIDE )     prpMask |= CBindProps::prpVisible;
	if( nStateMask & ISTATE_HOTLIGHT ) prpMask |= CBindProps::prpHotlight;
	if( nStateMask & ISTATE_SELECTED ) prpMask |= CBindProps::prpSelected;
	if( nStateMask & ISTATE_DISABLED ) prpMask |= CBindProps::prpEnabled;
	if( nStateMask & ISTATE_CUSTOM )   prpMask |= CBindProps::prpCustom;
	return prpMask;
}

//////////////////////////////////////////////////////////////////////

cNode* CRootItem::CreateNode(LPCSTR strFormat, CFieldsBinder *pBinder, CItemBase *owner)
{
	CNodeFactory factory(owner, pBinder);
	return cNode::Parse(strFormat, &factory, factory.m_pAlloc);
}

cNode* CRootItem::CreateNode(tTYPE_ID type, tDWORD flags, tPTR data)
{
	return new cMemberAcc(type, flags, data);
}

LPCSTR CRootItem::GetFormatedText(tString &strResult, cNode *pNode, CStructData *pData)
{
	strResult.clear();
	if( pNode )
	{
		cVariant res;
		pNode->Exec(res, TOR(cNodeExecCtx, (this, NULL, pData)));
		res.ToString(strResult);
	}
	return strResult.c_str();
}

LPCWSTR CRootItem::GetFormatedText(cStrObj &strResult, cNode *pNode, CStructData *pData)
{
	strResult.clear();
	if( pNode )
	{
		cVariant res;
		pNode->Exec(res, TOR(cNodeExecCtx, (this, NULL, pData)));
		res.ToStringObj(strResult);
	}
	return strResult.data();
}

LPCSTR CRootItem::GetFormatedText(tString &strResult, LPCSTR strFormat, CFieldsBinder *pBinder, CStructData *pData)
{
	CGuiPtr<cNode> pNode = CreateNode(strFormat, pBinder);
	return GetFormatedText(strResult, pNode, pData);
}

LPCWSTR CRootItem::GetFormatedText(cStrObj &strResult, LPCSTR strFormat, CFieldsBinder *pBinder, CStructData *pData)
{
	CGuiPtr<cNode> pNode = CreateNode(strFormat, pBinder);
	return GetFormatedText(strResult, pNode, pData);
}

LPCSTR CRootItem::GetFormatedText(tString &strResult, LPCSTR strFormat, cSerializable *pStruct)
{
	CGuiPtr<CFieldsBinder> pBinder = CreateFieldsBinder();
	pBinder->AddDataSource(pStruct);
	return GetFormatedText(strResult, strFormat, pBinder, NULL);
}

LPCWSTR CRootItem::GetFormatedText(cStrObj &strResult, LPCSTR strFormat, cSerializable *pStruct)
{
	CGuiPtr<CFieldsBinder> pBinder = CreateFieldsBinder();
	pBinder->AddDataSource(pStruct);
	return GetFormatedText(strResult, strFormat, pBinder, NULL);
}

CFieldsBinder* CRootItem::CreateFieldsBinder(const cSerDescriptor* pDesc)
{
	return new CFieldsBinder(this, NULL, NULL, pDesc);
}

//////////////////////////////////////////////////////////////////////
// CValData

tQWORD CValData::GetInteger()
{
	switch(m_type)
	{
	case tid_STRING:     return GetIntFromString(*(tSTRING *)m_pData);
	case tid_STRING_OBJ: return GetIntFromString(((cStringObj *)m_pData)->c_str(cCP_ANSI));
	case tid_DATETIME:   return (tDWORD)GetDateTime();
	}
	
	return GetIntegerNative();
}

bool CValData::SetInteger(tQWORD v)
{
	switch(m_type)
	{
	case tid_BOOL:     m_size == sizeof(bool) ? *(bool*)m_pData = !!v : *(tBOOL*)m_pData = (tBOOL)v; return true;
	case tid_BYTE:     *(tBYTE*)m_pData = (tBYTE)v; return true;
	case tid_SHORT:
	case tid_WORD:     *(tWORD*)m_pData = (tWORD)v; return true;
	case tid_INT:      
	case tid_UINT:     
	case tid_LONG:
	case tid_ERROR:
	case tid_DWORD:    *(tDWORD*)m_pData = (tDWORD)v; return true;
	case tid_QWORD:
	case tid_LONGLONG: *(tQWORD*)m_pData = v; return true;
	case tid_PTR:      *(tPTR*)m_pData = (tPTR)v; return true;
	}
	
	return false;
}

//////////////////////////////////////////////////////////////////////
// CFieldsBinder

void CFieldsBinder::Destroy() { delete this; }

void CFieldsBinder::ClearSources(bool bClearSect)
{
	for(Sections::iterator it = m_pSections.begin(); it != m_pSections.end(); it++)
	{
		Structs& l_structs = it->second;
		for(size_t i = 0; i < l_structs.size(); i++)
			l_structs[i]->release();
		l_structs.clear();
	}

	if( bClearSect )
		m_pSections.clear();
}

void CFieldsBinder::ClearSource(cSerializable* pData)
{
	for(Sections::iterator it = m_pSections.begin(), end = m_pSections.end(); it != end; it++)
	{
		Structs& structs = it->second;
		Structs::iterator i = std::find(structs.begin(), structs.end(), pData);
		if( i != structs.end() )
		{
			structs.erase(i);
			return;
		}
	}
}

void CFieldsBinder::AddDataSource(cSerializable* pData, LPCSTR strSect, bool bAddFirst)
{
	if( !pData )
		return;
	
	if( !strSect )
		strSect = "";
	
	Sections::iterator l_sect = m_pSections.find(strSect);
	if( l_sect == m_pSections.end() )
		l_sect = m_pSections.insert(Sections::value_type(strSect, Structs())).first;

	if( bAddFirst )
		l_sect->second.insert(l_sect->second.begin(), pData);
	else
		l_sect->second.push_back(pData);
	pData->addRef();
}

bool CFieldsBinder::GetDataSource(CStructId& nStructId, cSerializable** pStruct)
{
	Sections::iterator l_sect = m_pSections.find(nStructId.m_sect ? nStructId.m_sect : "");
	if( l_sect != m_pSections.end() )
	{
		Structs& l_structs = l_sect->second;
		for(size_t i = 0; i < l_structs.size(); i++)
		{
			cSerializable * pSer = l_structs[i];
			if( pSer->isBasedOn(nStructId.m_serId) )
			{
				if( pStruct )
					*pStruct = l_structs[i];
				return true;
			}
		}
	}
	
	return m_pParent ? m_pParent->GetDataSource(nStructId, pStruct) : false;
}

bool CFieldsBinder::CheckDataSource(CStructId& nStructId, cSerializable* pStruct, bool bRecurse)
{
	Sections::iterator l_sect = m_pSections.find(nStructId.m_sect ? nStructId.m_sect : "");
	if( l_sect != m_pSections.end() )
	{
		Structs& l_structs = l_sect->second;
		for(size_t i = 0; i < l_structs.size(); i++)
		{
			cSerializable * pSer = l_structs[i];
			if( pStruct )
			{
				if( pStruct == pSer )
					return true;
			}
			else if( pSer->isBasedOn(nStructId.m_serId) )
				return true;
		}
	}
	
	return bRecurse && m_pParent ? m_pParent->CheckDataSource(nStructId, pStruct, bRecurse) : false;
}

CFieldsBinder::Structs *CFieldsBinder::GetNamespace(const char **ns)
{
	const char *noNamespace = "";
	if( !ns )
		ns = &noNamespace;

	Sections::iterator i = m_pSections.find(*ns);
	if( i == m_pSections.end() )
	{
		if( m_pSink && m_pSink->OnAddDataSection(*ns) )
			i = m_pSections.find(*ns);
	}
	if( i != m_pSections.end() )
	{
		*ns = i->first.c_str();
		return &i->second;
	}
	return NULL;
}

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
