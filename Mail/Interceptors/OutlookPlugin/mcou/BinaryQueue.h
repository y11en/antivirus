// BinaryQueue.h: interface for the CBinaryQueue class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BINARYQUEUE_H__89F52F54_29C9_11D6_8540_006052067816__INCLUDED_)
#define AFX_BINARYQUEUE_H__89F52F54_29C9_11D6_8540_006052067816__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// Class name		: CBinaryElements
// Description		:	Список бинарных объектов.
//						Все открытые функции должны запрашивать указатель на критическую секцию,
//						а все пользователи этого класса обязаны эту секцию предоставлять для 
//						доступа к мемберам из разных потоков.
class CBinaryElement  
{
public:
	CBinaryElement(PVOID pData);
	virtual ~CBinaryElement();
	enum enumOtherMember {First, Prev, Next, Last};

	/*
	// Function name	: GetOtherMember
	// Description	    :	Возвращает элемент pOtherMember относительно текущего 
	//						(или указанного pMember)
	// Return type		: BOOL 
	// Argument         : CBinaryQueue * pMember
	// Argument         : CBinaryQueue * pOtherMember
	// Argument         : enumOtherMember OtherMember
	BOOL GetOtherMember(
		CBinaryElement * pMember, 
		CBinaryElement * pOtherMember, 
		enumOtherMember OtherMember,
		CRITICAL_SECTION * pcsMemberAccess);
	*/

	// Function name	: GetData
	// Description	    :	Возвращает pData текущего элемента
	// Return type		: PVOID 
	PVOID GetData() {return m_pData;};

	// Function name	: InsertMember
	// Description	    :	Вставка нового элемента в конец очереди
	// Return type		: BOOL 
	// Argument         : CBinaryElement * pMember
	BOOL InsertMember( CBinaryElement * pMember );

	// Function name	: DeleteMember
	// Description	    :	Удаление элемента. 
	//						Возвращается указатель на следующий за ним эелемент
	// Return type		: CBinaryElement *
	CBinaryElement * DeleteMember();
	
	// Function name	: GetMemberPrev
	// Description	    :	Возвращается следующий элемент очереди 
	//						(отсчет ведется от текущего или указанного в pMember)
	// Return type		: CBinaryElement * 
	// Argument         : CBinaryElement * pMember
	CBinaryElement * GetMemberPrev( CBinaryElement * pMember = NULL );

	// Function name	: GetMemberNext
	// Description	    :	Возвращается предыдущий элемент очереди
	//						(отсчет ведется от текущего или указанного в pMember)
	// Return type		: CBinaryElement * 
	// Argument         : CBinaryElement * pMember
	CBinaryElement * GetMemberNext( CBinaryElement * pMember = NULL );

	// Function name	: GetMemberFirst
	// Description	    :	Возвращает первый элемент очереди
	// Return type		: CBinaryElement * 
	CBinaryElement * GetMemberFirst();

	// Function name	: GetMemberLast
	// Description	    :	Возвращает последний элемент очереди
	// Return type		: CBinaryElement * 
	CBinaryElement * GetMemberLast();
	
	CBinaryElement * m_pMemberPrev;
	CBinaryElement * m_pMemberNext;

private:
	PVOID m_pData;
};

////////////////////////////////////////////////////////////////////
class CBinaryQueue
{
public:
	CBinaryQueue();
	virtual ~CBinaryQueue();

	// Function name	: GetElementCount
	// Description	    :	
	// Return type		: ULONG 
	ULONG GetElementCount();
	
	// Function name	: GetElement
	// Description	    :	Возвращает pData первого в списке элемента
	// Return type		: PVOID 
	PVOID GetElement();

	// Function name	: InsertElement
	// Description	    :	Вставляет pElement в очередь элементов
	// Return type		: BOOL 
	// Argument         : PVOID pElement
	BOOL InsertElement(PVOID pElement);

	// Function name	: InsertElement
	// Description	    :	Вставляет pElement в очередь элементов на первое место
	// Return type		: BOOL 
	// Argument         : PVOID pElement
	BOOL InsertFirstElement(PVOID pElement);

	// Function name	: DeleteElement
	// Description	    :	Освобождает первый элемент из очереди.
	//						Возвращает его pData
	// Return type		: PVOID 
	PVOID DeleteElement();

	///////////////////////////////////////////////////////////////////////////
	//! \fn				: DeleteElement
	//! \brief			:	Освобождает указанный элемент из очереди
	//!						Возвращает FALSE, если элемент в очереди отсутствует
	//! \return			: PVOID 
	//! \param          : PVOID pData
	BOOL DeleteElement(PVOID pData);

private:
	CBinaryElement * volatile pMembers;
	CRITICAL_SECTION m_csElementAccess;
};

#endif // !defined(AFX_BINARYQUEUE_H__89F52F54_29C9_11D6_8540_006052067816__INCLUDED_)
