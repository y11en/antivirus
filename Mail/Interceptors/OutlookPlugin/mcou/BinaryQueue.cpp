// BinaryQueue.cpp: implementation of the CBinaryElement class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "BinaryQueue.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBinaryElement::CBinaryElement(PVOID pData)
{
	m_pData = pData;
	m_pMemberPrev = NULL;
	m_pMemberNext = NULL;
}

CBinaryElement::~CBinaryElement()
{
	if (m_pMemberPrev) m_pMemberPrev->m_pMemberNext = m_pMemberNext;
	if (m_pMemberNext) m_pMemberNext->m_pMemberPrev = m_pMemberPrev;
}

CBinaryElement * CBinaryElement::GetMemberPrev( CBinaryElement * pMember )
{
	if (!pMember)
		return m_pMemberPrev;
	else
		return pMember->m_pMemberPrev;
}

CBinaryElement * CBinaryElement::GetMemberNext( CBinaryElement * pMember )
{
	if (!pMember)
		return m_pMemberNext;
	else
		return pMember->m_pMemberNext;
}

CBinaryElement * CBinaryElement::GetMemberFirst()
{
	if (m_pMemberPrev != NULL)
		return m_pMemberPrev->GetMemberFirst();
	else
		return this;
}

CBinaryElement * CBinaryElement::GetMemberLast()
{
	if (m_pMemberNext != NULL)
		return m_pMemberNext->GetMemberLast();
	else
		return this;
}

BOOL CBinaryElement::InsertMember( CBinaryElement * pMember )
{
	BOOL bResult = FALSE;

	if (pMember)
	{
		CBinaryElement * pMemberTemp = this->GetMemberLast();
		pMemberTemp->m_pMemberNext = pMember;
		pMember->m_pMemberPrev = pMemberTemp;
		bResult = TRUE;
	}
	
	return bResult;
}

CBinaryElement * CBinaryElement::DeleteMember()
{
	CBinaryElement * pMemberTemp = this->m_pMemberNext;
	delete this;
	return pMemberTemp;
}




///////////////////////////////////////////////////////////////////////

CBinaryQueue::CBinaryQueue()
:	pMembers			(0)
{
	InitializeCriticalSection(&m_csElementAccess);
}

CBinaryQueue::~CBinaryQueue()
{
	DeleteCriticalSection(&m_csElementAccess);
}

ULONG CBinaryQueue::GetElementCount()
{
	ULONG ulResult = 0;

	EnterCriticalSection(&m_csElementAccess);
	if (pMembers)
	{
		CBinaryElement * pTempMember;
		
		pTempMember = pMembers->GetMemberFirst();

		while (pTempMember)
		{
			ulResult++;
			pTempMember = pMembers->GetMemberNext();
		}
	}
	LeaveCriticalSection(&m_csElementAccess);

	return ulResult;
}

PVOID CBinaryQueue::GetElement()
{
	PVOID pData = NULL;

	EnterCriticalSection(&m_csElementAccess);
	if (pMembers)
		pData = pMembers->GetMemberFirst()->GetData();
	LeaveCriticalSection(&m_csElementAccess);
	
	return pData;
}

BOOL CBinaryQueue::InsertFirstElement(PVOID pElement)
{
	CBinaryElement * pNew = new CBinaryElement(pElement);

	EnterCriticalSection(&m_csElementAccess);
	if ( pNew )
		pNew->InsertMember(pMembers);
	pMembers = pNew;
	LeaveCriticalSection(&m_csElementAccess);

	if ( pNew )
		return TRUE;
	else
		return FALSE;
}

BOOL CBinaryQueue::InsertElement(PVOID pElement)
{
	CBinaryElement * pNew = new CBinaryElement(pElement);

	EnterCriticalSection(&m_csElementAccess);
	if ( pNew )
	{
		if (pMembers)
			pMembers->InsertMember(pNew);
		else
			pMembers = pNew;
	}
	LeaveCriticalSection(&m_csElementAccess);

	if ( pNew )
		return TRUE;
	else
		return FALSE;
}

PVOID CBinaryQueue::DeleteElement()
{
	PVOID pResult = 0;

	EnterCriticalSection(&m_csElementAccess);
	if (pMembers)
	{
		pResult = pMembers->GetMemberFirst()->GetData();
		pMembers = pMembers->DeleteMember();
	}
	LeaveCriticalSection(&m_csElementAccess);

	return pResult;
}

BOOL CBinaryQueue::DeleteElement(PVOID pData)
{
	PVOID pResult = 0;
	BOOL bResult = TRUE;

	EnterCriticalSection(&m_csElementAccess);
	if (pMembers)
	{
		CBinaryElement* pMember = pMembers->GetMemberFirst();
		do 
		{
			pResult = pMember->GetData();
		} 
		while( 
			(pResult != pData) &&
			(pMember = pMember->GetMemberNext())  
			);

		if ( pMember == pMembers ) 
			pMembers = pMember->DeleteMember();
		else if ( pMember ) 
			pMember->DeleteMember();
		else
			bResult = FALSE;
	}
	LeaveCriticalSection(&m_csElementAccess);

	return bResult;
}
