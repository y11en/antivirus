////////////////////////////////////////////////////////////////////
//
//  PSynQue.h
//  Synchronized pointer queue
//  AVP genegic purposes stuff
//  Victor Matiouchenkov [victor@avp.ru], Kaspersky Labs. 1999
//  Copyright (c) Kaspersky Labs
//
////////////////////////////////////////////////////////////////////

#ifndef __PSYNQUE_H__
#define __PSYNQUE_H__

#include <windows.h>
//#include "PArray.h"
#include "SList.h"


// ---
//template <class T> class CPSynQueue : private CPArray<T> { PArray
template <class T> class CPSynQueue : private CPList<T> {
	CRITICAL_SECTION m_CritSect;
	HANDLE					 m_hSynEvent;
public :
  //CPSynQueue( UINT nUpper, WORD nDelta = 1, bool bShouldDelete = true, LPCTSTR pSyncObjectName = NULL ); PArray
  CPSynQueue( bool bShouldDelete = true, LPCTSTR pSyncObjectName = NULL );
  virtual ~CPSynQueue();
	
	T				*Push( T *pEntry );    
	T			  *Pop();          
	int      GetSize() const;
	void     Empty();
	void     SetOwns( bool bSet );
	
	typedef bool (*CheckFunc)(T* pQueueObject, T* pCheckObject);
	bool     Check(T* pCheckObject, CheckFunc);
	void     Clean(T* pCheckObject, CheckFunc);

	operator HANDLE() const { return m_hSynEvent; }
};

// ---
template <class T>
//inline CPSynQueue<T>::CPSynQueue( UINT nUpper, WORD nDelta, bool bShouldDelete, LPCTSTR pSyncObjectName ) : PArray
inline CPSynQueue<T>::CPSynQueue( bool bShouldDelete, LPCTSTR pSyncObjectName ) :
	CPList<T>( bShouldDelete ) {

  m_hSynEvent = ::CreateEvent( NULL, TRUE, FALSE, pSyncObjectName );
  //m_hSynEvent = ::CreateSemaphore( NULL, 0, 0x7fffffff, pSyncObjectName );

  ::InitializeCriticalSection( const_cast<CRITICAL_SECTION*>(&m_CritSect) );
}

// ---
template <class T>
inline CPSynQueue<T>::~CPSynQueue() {
  ::CloseHandle( m_hSynEvent );
  ::DeleteCriticalSection(const_cast<CRITICAL_SECTION*>(&m_CritSect));
}


// ---
template <class T>
inline T *CPSynQueue<T>::Push( T *pEntry ) {
  ::EnterCriticalSection(const_cast<CRITICAL_SECTION*>(&m_CritSect));

	Add( pEntry );

  ::SetEvent( m_hSynEvent );
  //::ReleaseSemaphore( m_hSynEvent, 1, NULL );
	
  ::LeaveCriticalSection(const_cast<CRITICAL_SECTION*>(&m_CritSect));

	return pEntry;
}

// ---
template <class T>
inline T *CPSynQueue<T>::Pop() {
  ::EnterCriticalSection(const_cast<CRITICAL_SECTION*>(&m_CritSect));

	T *pEntry = NULL;

	if ( Count() ) {
		pEntry = GetFirstData();
		Detach( pEntry );
	}
/* PArray
	if ( Count() ) {
		pEntry = (*this)[0];

		bool bOwns = OwnsElem();
		OwnsElem( false );
		RemoveInd( 0 );
		OwnsElem( bOwns );
	}
*/

	if ( !Count() )
		::ResetEvent( m_hSynEvent );

  ::LeaveCriticalSection(const_cast<CRITICAL_SECTION*>(&m_CritSect));

	return pEntry;
}

// ---
template <class T>
inline bool CPSynQueue<T>::Check( T* pCheckObject, CheckFunc pCheck ) {
  ::EnterCriticalSection(const_cast<CRITICAL_SECTION*>(&m_CritSect));

	bool bResult = false;

	if ( Count() ) {
		CPLIterator<T> iter( *this );
		while ( iter ) {
			if ( pCheck(iter.GetData(), pCheckObject) ) {
				bResult = true;
				break;
			}
			iter++;
		}
	}
/* PArray
	if ( Count() ) {
		for ( int i=0,c=Count(); i<c && !bResult; i++ ) {
			if ( pCheck((*this)[i], pCheckObject) ) {
				bResult = true;
				break;
			}
		}
	}
*/
  ::LeaveCriticalSection(const_cast<CRITICAL_SECTION*>(&m_CritSect));

	return bResult;
}

// ---
template <class T>
inline void CPSynQueue<T>::Clean( T* pCheckObject, CheckFunc pCheck ) {
  ::EnterCriticalSection(const_cast<CRITICAL_SECTION*>(&m_CritSect));

	if ( Count() ) {
		CPLIterator<T> iter( *this );
		while ( iter ) {
			if ( pCheck(iter.GetData(), pCheckObject) ) {
				iter.Remove( Delete );
			}
			else
				iter++;
		}

/*	PArray
		for ( int i=MaxIndex(); i>=0; i-- ) {
			if ( pCheck((*this)[i], pCheckObject) ) {
				bool bOwns = OwnsElem();
				OwnsElem( true );
				RemoveInd( i );
				OwnsElem( bOwns );
			}
		}
*/
	}

	if ( !Count() )
		::ResetEvent( m_hSynEvent );

  ::LeaveCriticalSection(const_cast<CRITICAL_SECTION*>(&m_CritSect));
}

// ---
template <class T>
inline int CPSynQueue<T>::GetSize() const {
  ::EnterCriticalSection(const_cast<CRITICAL_SECTION*>(&m_CritSect));

	int nCount = Count();

  ::LeaveCriticalSection(const_cast<CRITICAL_SECTION*>(&m_CritSect));

	return nCount;
}


// ---
template <class T>
inline void CPSynQueue<T>::Empty() {
  ::EnterCriticalSection(const_cast<CRITICAL_SECTION*>(&m_CritSect));

	bool bOwns = OwnsElem();
	OwnsElem( true );
	Flush();
	OwnsElem( bOwns );

  ::LeaveCriticalSection(const_cast<CRITICAL_SECTION*>(&m_CritSect));
}


// ---
template <class T>
inline void CPSynQueue<T>::SetOwns( bool bSet ) {
  ::EnterCriticalSection(const_cast<CRITICAL_SECTION*>(&m_CritSect));

	OwnsElem( bSet );

  ::LeaveCriticalSection(const_cast<CRITICAL_SECTION*>(&m_CritSect));
}



#endif //__PSYNQUE_H__
