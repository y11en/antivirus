// ItemProps.cpp: implementation of the CItemPropsa class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include <ProductCore/ItemProps.h>

//////////////////////////////////////////////////////////////////////

tString	GetNextToken(LPCSTR &strText, bool bBraced)
{
	tString strToken; bool bQuoted = false;
	for(tCHAR pch = *strText; *strText; pch = *strText++)
	{
		if( !bQuoted )
			switch(*strText)
			{
			case ')': if( bBraced ) return strToken; bBraced = true; continue;
			case ',': strText++; return strToken;
			case '(': if( !bBraced ) bBraced = true;
			case ' ': continue;
			case '"': bQuoted = true; continue;
			case '-': if( pch == ')' ) continue;
			default: strToken += *strText; break;
			}
		else
		{
			if(*strText == '"')
			{
				bQuoted = false;
				continue;
			}
			
			strToken += *strText;
		}
	}
	
	return strToken;
}

tDWORD GetAlign( const tString& strProp )
{
	tDWORD nAlign = 0;
	for( size_t i = 0; i < strProp.size(); i++ )
		switch( strProp[ i ] )
		{
		case 'r': nAlign |= ALIGN_RIGHT; break;
		case 'b': nAlign |= ALIGN_BOTTOM; break;
		case 'c': nAlign |= (!i ? ALIGN_HCENTER : ALIGN_VCENTER); break;
		case 'a': nAlign |= (!i ? ALIGN_HAFTER  : ALIGN_VAFTER); break;
		case 'p': nAlign |= (!i ? ALIGN_HBEFORE : ALIGN_VBEFORE); break;
		case 's': nAlign |= (!i ? ALIGN_HSAME   : ALIGN_VSAME); break;
		}
	
	return( nAlign );
}

tString GetPlainText(LPCSTR strProp)
{
	tString strText;
	CItemProps prpCol(strProp, strText);
	return strText;
}

//////////////////////////////////////////////////////////////////////

tQWORD GetIntFromString(LPCSTR strVal, tTYPE_ID eReqType)
{
	if( !strVal || !*strVal )
		return 0;
	
	tQWORD nVal = 0;
	LPCSTR strOr = strchr(strVal, '|');
	
	if( !strOr )
	{
		bool bHex = (strstr(strVal, "0x") == strVal);
		
		sscanf(strVal, bHex ? "0x" PRINTF_QWORD_HEX : PRINTF_LONGLONG, &nVal);
		if( eReqType != tid_QWORD )
			nVal &= 0x00000000FFFFFFFF;
		return nVal;
	}
	
	{
		tString strTmpVal; strTmpVal.assign(strVal, strOr - strVal);
		nVal = GetIntFromString(strTmpVal.c_str(), eReqType);
	}

	return nVal|GetIntFromString(strOr + 1, eReqType);
}

void GetStringFromInt(tQWORD nVal, tString& strVal, tTYPE_ID eReqType)
{
    tCHAR buf[MAX_8B_DIGIT_STRSIZE];
	if( eReqType == tid_QWORD )
	{
		_snprintf(buf, sizeof(buf), PRINTF_LONGLONG, nVal);
	}
	else
	{
		_snprintf(buf, sizeof(buf), "%d", (tDWORD)nVal);
	}
    strVal = buf;
}

//////////////////////////////////////////////////////////////////////

bool _kav_isspace(unsigned char c) { return (c >= 0x09 && c <= 0x0D) || (c == 0x20); }

bool _kav_iscsym(unsigned char c)
{
	if( !c )
		return false;

	if( c == '_' || c == '@' )
		return true;

	if( c >= 'a' && c <= 'z' )
		return true;
	if( c >= 'A' && c <= 'Z' )
		return true;
	if( c >= '0' && c <= '9' )
		return true;
	
	return false;
}

//////////////////////////////////////////////////////////////////////
// CItemPropVals

tString CItemPropVals::m_void;

CItemPropVals::CItemPropVals(LPCSTR strVals) : m_name("")
{
	if( strVals )
		Parse(strVals);
}

tSIZE_T CItemPropVals::Parse(LPCSTR strVals, tDWORD nFlags, bool bValsOnly)
{
	if( !bValsOnly && *strVals++ != '(' )
		return 0;

	int nCounter = bValsOnly ? 0 : 1;
	LPCSTR strEnd = strVals;
	for(;*strEnd; strEnd++)
	{
		if( *strEnd == '(' )
			nCounter++;
		else if( *strEnd == ')' )
		{
			if( bValsOnly )
			{
				if( !nCounter )
					break;
				--nCounter;
			}
			else
				if( !--nCounter )
					break;
		}
	}

	if( nCounter )
		return 0;

	_Parse(strVals, strEnd - strVals, nFlags);
	return strEnd - strVals + (bValsOnly ? 0 : 2);
}

LPCSTR CItemPropVals::DeParse(tString& strProp, LPCSTR strName, bool bValsOnly)
{
	if( !IsValid() )
		return strProp.c_str();
	
	if( !bValsOnly )
	{
		if( strProp.size() )
			strProp += ' ';
		strProp += strName ? strName : m_name;
		strProp += '(';
	}
	
	for(int i = 0, n = size(); i < n; i++)
	{
		if( i ) strProp += ',';
		strProp += at(i);
	}

	if( !bValsOnly )
		strProp += ")";
	return strProp.c_str();
}

bool CItemPropVals::IsValid()
{
	return( this != &CItemProps::m_void );
}

bool CItemPropVals::IsEmpty()
{
	if( !size() )
		return true;

	for(int i = 0, n = size(); i < n; i++)
		if( !at(i).empty() )
			return false;

	return true;
}

bool CItemPropVals::IsExist(LPCSTR strName)
{
	if( !strName )
		return false;
	
	for(int i = 0, n = size(); i < n; i++)
		if( at(i) == strName )
			return true;
	return false;
}

bool CItemPropVals::IsEqual(const CItemPropVals& v)
{
	return cVector<tString, cSimpleMover<tString> >::operator==(v);
}

int CItemPropVals::Count() const
{
	return size();
}

LPCSTR CItemPropVals::GetName()
{
	return m_name.c_str();
}

LPCSTR CItemPropVals::GetStr(tDWORD nIdx)
{
	return Get(nIdx).c_str();
}

bool CItemPropVals::GetBool(tDWORD nIdx)
{
	if( !IsValid() )
		return false;
	
	if( size() <= 1 )
	{
		sswitch( Get(nIdx).c_str() )
		stcase(0)       return false; sbreak;
		stcase(no)      return false; sbreak;
		stcase(false)   return false; sbreak;
		send;

		return( true );
	}

	sswitch( Get(nIdx).c_str() )
	stcase(1)      return true; sbreak;
	stcase(yes)    return true; sbreak;
	stcase(true)   return true; sbreak;
	send;

	return false;
}

tQWORD CItemPropVals::GetLong(tDWORD nIdx)
{
	return GetIntFromString(Get(nIdx).c_str());
}

void CItemPropVals::Del(tDWORD nIdx)
{
	if( nIdx < size() )
		remove(nIdx);
}

tString CItemPropVals::Pop(tDWORD nIdx)
{
	tString strItem = Get(nIdx);
	return Del(nIdx), strItem;
}

tString& CItemPropVals::Get(tDWORD nIdx, bool bCreate)
{
	if( nIdx < size() )
		return at(nIdx);
	if( !bCreate )
		return m_void;

	resize(nIdx + 1);
	return at(nIdx);
}

void CItemPropVals::Assign(CItemPropVals& v, tDWORD nFlags)
{
	if( nFlags & IPV_F_SINGLE )
	{
		tString& str = size() ? at(0) : push_back();
		if( str.size() )
			str += "; ";
		
		str.append(v.Get(0));
	}
	else
	{
		if( !(nFlags & IPV_F_APPEND) )
			clear();

		for(int i = 0, n = v.size(); i < n; i++)
		{
			tString& dst = push_back();
			tString& src = v[i];
			
			if( nFlags & IPV_F_MOVE )
				swap_objmem(dst, src);
			else
				dst.assign(src);
		}
	}

	if( nFlags & IPV_F_MOVE )
		v.clear();
}

void CItemPropVals::Set(tDWORD nIdx, LPCSTR str)
{
	Get(nIdx, true) = str ? str : "";
}

//////////////////////////////////////////////////////////////////////

void CItemPropVals::_Parse(LPCSTR strVals, tSIZE_T nLen, tDWORD nFlags)
{
	if( nFlags & IPV_F_SINGLE )
	{
		tString& str = size() ? at(0) : push_back();
		if( str.size() )
			str += "; ";
		
		str.append(strVals, nLen);
		return;
	}

	if( !(nFlags & IPV_F_APPEND) )
		clear();
	
	if( !nLen )
		return;
	
	int nCounter = 0;
	bool bQuoted = false;
	for(LPCSTR l_sep = strVals, l_prm = strVals; l_sep <= strVals + nLen; l_sep++)
	{
		if( bQuoted )
		{
			if( *l_sep == '"' )
				bQuoted = false;
			
			continue;
		}
		
		if( *l_sep == '"' )
		{
			bQuoted = true;
			continue;
		}

		if( *l_sep != ',' && l_sep < strVals + nLen )
		{
			if( *l_sep == '(' || *l_sep == '<' ) nCounter++;
			if( *l_sep == ')' || *l_sep == '>' ) nCounter--;
			continue;
		}
		
		if( *l_sep == ',' && nCounter )
			continue;

		if( *l_prm == ',' || l_prm == l_sep )
			push_back();
		else
		{
			const tCHAR *l_strTrimBegin = l_prm, *l_strTrimEnd = l_sep - 1; _Trim(l_strTrimBegin, l_strTrimEnd);
			push_back().assign(l_strTrimBegin, l_strTrimEnd - l_strTrimBegin + 1);
		}
		
		l_prm = l_sep + 1;
	}
}

void CItemPropVals::_Trim(LPCSTR& strBegin, LPCSTR& strEnd)
{
	while( _kav_isspace(*strBegin) ) strBegin++;
	if( strBegin > strEnd )
	{
		strEnd = strBegin;
		return;
	}
	
	while( strBegin < strEnd && _kav_isspace(*strEnd) ) strEnd--;
}

//////////////////////////////////////////////////////////////////////
// CItemProps

static tDWORD _CItemProps_GetPropMergeFlags(LPCSTR strProp, tDWORD nFlags = 0)
{
	tDWORD nPrpFlags = 0;
	if( nFlags & IPV_F_SINGLE )
		nPrpFlags |= IPV_F_SINGLE;
	else
	{
		sswitch(strProp)
		scase(STR_PID_ATTRS)		nPrpFlags |= IPV_F_APPEND; sbreak;
		
/*			scase(STR_PID_TIPTEXT)		nPrpFlags |= IPV_F_SINGLE; sbreak;
		scase(STR_PID_TEXT)			nPrpFlags |= IPV_F_SINGLE; sbreak;
		scase(STR_PID_ENABLE)		nPrpFlags |= IPV_F_SINGLE; sbreak;
		scase(STR_PID_VISIBLE)		nPrpFlags |= IPV_F_SINGLE; sbreak;*/
		scase(STR_PID_BIND_PROPS)	nPrpFlags |= IPV_F_SINGLE; sbreak;
		send;
	}

	if( nFlags & IPV_F_MOVE )
		nPrpFlags |= IPV_F_MOVE;

	return nPrpFlags;
}

CItemPropVals CItemProps::m_void;

CItemProps& CItemProps::Parse(LPCSTR strProps, tString& str, LPCSTR * pstrPropsIncl, tDWORD nFlags)
{
	if( !(nFlags & IPV_F_APPEND) )
		clear();
	
	if( !strProps )
		return *this;
	
	tString strOpt; bool bOpt = false;
	int nCounter = 0;
	
	do
	{
		switch( *strProps )
		{
		case '(': case '<': nCounter++; break;
		case ')': case '>': nCounter--; break;
		}

		if( !bOpt )
		{
			if( !nCounter && _kav_iscsym((unsigned char)*strProps) )
			{
				bOpt = true;
				strOpt = *strProps;
				continue;
			}

			if( *strProps )
				str += *strProps;
			continue;
		}

		if( _kav_iscsym((unsigned char)*strProps) )
		{
			strOpt += *strProps;
			continue;
		}

		bOpt = false;

		if( (!(nFlags & IPV_F_VOIDPROP) && (*strProps != '(')) ||
			(pstrPropsIncl && !_IsPropAvlbl(pstrPropsIncl, strOpt.c_str())) )
		{
			str += strOpt;
			if( *strProps )
				str += *strProps;
			continue;
		}
		
		strProps += Get(strOpt.c_str(), true).Parse(strProps, _CItemProps_GetPropMergeFlags(strOpt.c_str(), nFlags)) - 1;
		nCounter--;
	} while( *strProps++ );
	
	return *this;
}

void CItemProps::Destroy()
{
	delete this;
}

CItemPropVals& CItemProps::Get(LPCSTR strName, bool bCreate)
{
	for(int i = 0, n = size(); i < n; i++)
	{
		CItemPropVals& _item = at(i);
		if( _item.m_name == strName )
			return _item;
	}

	if( !bCreate )
		return m_void;

	CItemPropVals& _item = push_back();
	_item.m_name = strName;
	return _item;
}

CItemPropVals& CItemProps::GetByIdx(tDWORD nIdx) const
{
	if( nIdx >= size() )
		return m_void;
	return at(nIdx);
}

tDWORD CItemProps::GetIdx(CItemPropVals& prp) const
{
	tDWORD nIdx = (&prp - data());
	if( nIdx >= size() )
		return -1;
	return nIdx;
}

int CItemProps::Count() const
{
	return size();
}

bool CItemProps::Del(LPCSTR strName)
{
	for(int i = 0, n = size(); i < n; i++)
	{
		CItemPropVals& _item = at(i);
		if( _item.m_name == strName )
			return remove(i), true;
	}
	
	return false;
}

bool CItemProps::DelByIdx(tDWORD nIdx)
{
	if( nIdx >= size() )
		return false;
	return remove(nIdx), true;
}

LPCSTR CItemProps::DeParse(tString& strProps, tDWORD nFlags)
{
	for(int i = 0, n = size(); i < n; i++)
	{
		CItemPropVals& _item = GetByIdx(i);
		if( (nFlags & IPV_F_VOIDPROP) || !_item.IsEmpty() )
			_item.DeParse(strProps);
	}

	return strProps.c_str();
}

void CItemProps::Clear()
{
	clear();
}

CItemProps& CItemProps::Assign(CItemProps& p, tDWORD nFlags)
{
	if( !(nFlags & IPV_F_APPEND) && (nFlags & IPV_F_MOVE) )
	{
		clear();
		memcpy((CItemPropsBase *)this, (CItemPropsBase *)&p, sizeof(CItemPropsBase));
		memset((CItemPropsBase *)&p, 0, sizeof(CItemPropsBase));
		return *this;
	}

	if( !(nFlags & IPV_F_APPEND) )
		clear();

	for(int i = 0, n = p.Count(); i < n; i++)
	{
		CItemPropVals& prp = p.GetByIdx(i);
		LPCSTR strProp = prp.GetName();
		Get(strProp, true).Assign(prp, _CItemProps_GetPropMergeFlags(strProp, nFlags));
	}

	if( nFlags & IPV_F_MOVE )
		p.clear();

	return *this;
}

//////////////////////////////////////////////////////////////////////

bool CItemProps::_IsPropAvlbl(LPCSTR * pstrPropsIncl, LPCSTR strName)
{
	for( ; *pstrPropsIncl; pstrPropsIncl++ )
		if( !strcmp( *pstrPropsIncl, strName ) )
			return( true );
	return(false);
}

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

CItemProps * CRootItem::CreateProps(LPCSTR strProps, tString& str)
{
	CItemProps * pProps = new CItemProps(strProps, str);
	if( !pProps )
		return NULL;
	TrimString(str);
	return pProps;
}

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
