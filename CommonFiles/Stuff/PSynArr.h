////////////////////////////////////////////////////////////////////
//
//  Parray.h
//  CPSyncArray template
//  General purpose
//  Victor Matiouchenkov [victor@avp.ru], Kaspersky Labs. 1999
//  Copyright (c) Kaspersky Labs
//
////////////////////////////////////////////////////////////////////

#ifndef __PSYNCARRAY_H
#define __PSYNCARRAY_H

#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include <malloc.h>
#include <windows.h>

#include "stddefs.h"

#define SYS_MAX_UINT  (0xffffffff)

// ---
class CSPACriticalSection {
public:
	CSPACriticalSection();
	~CSPACriticalSection();
	
	class CLock {
	public:
		CLock(const CSPACriticalSection&);
		~CLock();
	private:
		
		const CSPACriticalSection& CritSecObj;
	};
	friend CLock;
	typedef CLock Lock;  // For compatibility with old T-less typename
	
  private:
		
    CRITICAL_SECTION CritSec;
		
    CSPACriticalSection(const CSPACriticalSection&);
    const CSPACriticalSection& operator =(const CSPACriticalSection&);
};

// ---
inline CSPACriticalSection::CSPACriticalSection() {
  ::InitializeCriticalSection(const_cast<CRITICAL_SECTION*>(&CritSec));
}

// ---
inline CSPACriticalSection::~CSPACriticalSection() {
  ::DeleteCriticalSection(const_cast<CRITICAL_SECTION*>(&CritSec));
}

// ---
inline CSPACriticalSection::CLock::CLock(const CSPACriticalSection& sec) :
	CritSecObj(sec) {
  ::EnterCriticalSection(const_cast<CRITICAL_SECTION*>(&CritSecObj.CritSec));
}

// ---
inline CSPACriticalSection::CLock::~CLock() {
  ::LeaveCriticalSection(const_cast<CRITICAL_SECTION*>(&CritSecObj.CritSec));
}


///////////////////////////////////////////////////////
// одномерный массив указателей на обьекты
// классы с указателями
// ---
template <class Type>
class  CPSyncArray {
	
public :
	typedef Type* TPtr;
  CSPACriticalSection m_CritSect;
protected :
  bool          owns;  // элементы можно удалять
	uint          count; // размерность массива
  uint          upper;
  uint16        delta;
	TPtr*         parr;  // массив укзателей на элементы

public :

	CPSyncArray();
	CPSyncArray( uint i_upper, uint16 i_delta=1, bool shouldDelete=true );
	~CPSyncArray();

	bool     OwnsElem() const                              { return owns; }
	void     OwnsElem( bool ownsEl )                       { owns = ownsEl; }

	uint16   Delta() const                                 { return delta; }
	void     Delta( uint16 newDelta )                      { delta = newDelta; }

	void     Add( Type* ent );                             // добавить элемент в конец массива
	void     AddAt( Type *ent, uint index )                { Insert( index, ent ); }
  void     AddAfter(Type* ent, uint index );             // добавить элемент после указанного
	void     Add( CPSyncArray<Type>& o );                       // добавить массив
  void     Add( CPSyncArray<Type>& o, uint index );           // добавить массив в позицию
  void     Insert( uint index, Type *ent );              // вставить элемент перед указанным
  void     EatAway( CPSyncArray<Type>& o );                   // съесть массив
	void     EatAway( CPSyncArray<Type>& o, uint index );       // съесть массив
  CPSyncArray<Type>& operator +=( CPSyncArray<Type>& o );          // добавить массив

	void     Flush( DelType = defDelete );                 // удалить все элементы
	void		 SetSize( uint size, DelType = defDelete);		 // установить размер
  void     Adjust();                                     // удалить лишнюю память
	void     RemoveAll( DelType shdl = defDelete )            { Flush(shdl); } // удалить все элементы обнулить количество элементов
  Type*    RemoveObj( Type *delObject, DelType=defDelete ); // удалить элемент из массива
  Type*    RemoveInd( uint delIndex, DelType del=defDelete ); // удалить элемент из массива
	void     DetachInd( uint delIndex );                      // отсоединить элемент от массива
	void     DetachObj( const Type *delObject );              // отсоединить элемент от массива

	uint     FindIt( const Type *el ) const;               // найти элемент по указателю
	bool     IsExist( const Type *el ) const;              // true если элемент найден
	uint     Count()    const { return count; }            // дать количество элементов массива
  int      MaxIndex() const { return count-1; }          // дать количество элементов массива

	Type*&   operator []( uint loc ) const;

protected:
	bool     CatchMemory();                                // захватить память

private:
	CPSyncArray( const CPSyncArray<Type>& other );                   // запрещено !!!
	CPSyncArray<Type>& operator =( const CPSyncArray<Type>& o );     // запрещено !!!
#if _MSC_VER > 1300
	template<typename U> friend void destroy_array( CPSyncArray<U>& arr );
	template<typename U> friend void destroy_rest_array( CPSyncArray<U>& arr, uint size );
	template<typename U> friend void join_arrays( CPSyncArray<U>& to, CPSyncArray<U>& from );
	template<typename U> friend void insert_array( CPSyncArray<U>& to, CPSyncArray<U>& from, uint index );
	template<typename U> friend uint find_in_array( const CPSyncArray<U>& arr, const U* el );
#else
  friend void destroy_array( CPSyncArray<Type>& arr );
	friend void destroy_rest_array( CPSyncArray<Type>& arr, uint size );
  friend void join_arrays( CPSyncArray<Type>& to, CPSyncArray<Type>& from );
  friend void insert_array( CPSyncArray<Type>& to, CPSyncArray<Type>& from, uint index );
  friend uint find_in_array( const CPSyncArray<Type>& arr, const Type* el );
#endif
};


///////////////////////////////////////////////////////
// конструктор массива
// ---
template <class Type>
inline CPSyncArray<Type>::CPSyncArray()
  : owns( true ),
    count( 0 ),
    upper( 0 ),
    delta( 1 ),
    parr( 0 ) {
}




///////////////////////////////////////////////////////
// конструктор массива
// ---
template <class Type>
inline CPSyncArray<Type>::CPSyncArray( uint i_upper, uint16 i_delta, bool shouldDelete )
  : owns( shouldDelete ),
    count( 0 ),
    upper( i_upper ),
    delta( i_delta ),
    parr( i_upper ? (TPtr *)malloc(i_upper * sizeof(TPtr)) : 0 ) {
}




///////////////////////////////////////////////////////
// деструктор массива
// ---
template <class Type>
inline CPSyncArray<Type>::~CPSyncArray() {
  if ( owns )
    destroy_array( *this );
  free( parr );
	parr = 0;
}


///////////////////////////////////////////////////////
// выдать элемент по индексу
// ---
template <class Type>
inline Type*& CPSyncArray<Type>::operator []( uint loc ) const { 
	CSPACriticalSection::CLock lock( m_CritSect );
	
	return parr[loc];
}



///////////////////////////////////////////////////////
// обнулить количество элементов
// ---
template <class Type>
inline void CPSyncArray<Type>::Flush( DelType del ) {
	CSPACriticalSection::CLock lock( m_CritSect );
	
	if ( del==Delete || (del==defDelete && owns) )
    destroy_array( *this );
  else
    count = 0;
}



///////////////////////////////////////////////////////
// установить количество элементов
// ---
template <class Type>
inline void CPSyncArray<Type>::SetSize( uint size, DelType del ) {
	CSPACriticalSection::CLock lock( m_CritSect );
	
	if ( size <= count ) {
		if ( del==Delete || (del==defDelete && owns) )
			destroy_rest_array( *this, size );
		count = size;
	}
}



///////////////////////////////////////////////////////
// удалить лишнюю память
// ---
template <class Type>
inline void CPSyncArray<Type>::Adjust() {
	CSPACriticalSection::CLock lock( m_CritSect );
	
  if ( count < upper ) {
		parr = ((upper = count) > 0) ? (TPtr*)realloc( parr, upper*sizeof(TPtr) ) : 0;
  }
}



///////////////////////////////////////////////////////
// добавить элемент в конец массива
// ---
template <class Type>
inline void CPSyncArray<Type>::Add( Type* ent ) {
	CSPACriticalSection::CLock lock( m_CritSect );
	
  if ( CatchMemory() )
		parr[count++] = ent;
}




///////////////////////////////////////////////////////
// добавить элемент после заданного
// ---
template <class Type>
inline void CPSyncArray<Type>::AddAfter( Type* ent, uint index ) {
	CSPACriticalSection::CLock lock( m_CritSect );
	
  if ( CatchMemory() ) {         // добавить памяти, если все использовано
		if ( index > count-1 )
			index = count - 1;
		memmove( parr+index+2, parr+index+1, (count-index-1)*sizeof(TPtr) );
													 // передвинем вправо все элементы массива с index+1 до последнего
		parr[ index+1 ] = ent; // записываем новый элемент
		count++;
	}
}



///////////////////////////////////////////////////////
// добавить массив
// в зависимости от владения эл-тами у приемника
// признак владения у источника будет обнуляться
// ---
template <class Type>
inline void CPSyncArray<Type>::Add( CPSyncArray<Type>& o ) {
	CSPACriticalSection::CLock lock( m_CritSect );
	
  join_arrays( *this, o );
}



///////////////////////////////////////////////////////
// добавить массив в позицию index
// (если index>count, то в конец)
// в зависимости от владения эл-тами у приемника
// признак владения у источника будет обнуляться
// ---
template <class Type>
inline void CPSyncArray<Type>::Add( CPSyncArray<Type>& o, uint index ) {
	CSPACriticalSection::CLock lock( m_CritSect );
	
  insert_array( *this, o, index );
}




///////////////////////////////////////////////////////
// вставить элемент перед указанным
// ---
template <class Type>
inline void CPSyncArray<Type>::Insert( uint index, Type *ent ) {
	CSPACriticalSection::CLock lock( m_CritSect );
	
  if ( CatchMemory() ) {         // добавить памяти, если все использовано
		if ( index > count )
			index = count;
		memmove( parr+index+1, parr+index, (count-index) * sizeof(TPtr) );
													 // передвинем вправо все элементы массива с последнего до указанного
		parr[index] = ent;     // записываем новый элемент
		count++;
	}
}




///////////////////////////////////////////////////////
// съесть массив
// ---
template <class Type>
inline void CPSyncArray<Type>::EatAway( CPSyncArray<Type>& o ) {	
	CSPACriticalSection::CLock lock( m_CritSect );

  Add( o );  // снялся флаг 'o.owns'
  o.count = 0;
}





///////////////////////////////////////////////////////
// съесть массив
// ---
template <class Type>
inline void CPSyncArray<Type>::EatAway( CPSyncArray<Type>& o, uint index ) {
	CSPACriticalSection::CLock lock( m_CritSect );
	
  Add( o, index );  // снялся флаг 'o.owns'
  o.count = 0;
}





///////////////////////////////////////////////////////
// скопировать один массив в другой
// в зависимсти от признака владения элементами
// массива приемника, указатели будут перемещаться
// или копироваться
// ---
template <class Type>
inline CPSyncArray<Type>& CPSyncArray<Type>::operator += ( CPSyncArray<Type>& o ) {
	CSPACriticalSection::CLock lock( m_CritSect );

  Add( o );
  return *this;
}




///////////////////////////////////////////////////////
// вернуть индекс элемента в массиве
// ---
template <class Type>
inline uint CPSyncArray<Type>::FindIt( const Type *el ) const {
	CSPACriticalSection::CLock lock( m_CritSect );
	
  return find_in_array( *this, el );
}



///////////////////////////////////////////////////////
// вернуть индекс элемента в массиве
// ---
template <class Type>
inline bool CPSyncArray<Type>::IsExist( const Type *el ) const {
	CSPACriticalSection::CLock lock( m_CritSect );
	
  return find_in_array( *this, el ) != SYS_MAX_UINT;
}



///////////////////////////////////////////////////////
// удалить элемент из массива
// ---
template <class Type>
inline Type* CPSyncArray<Type>::RemoveInd( uint delIndex, DelType del ) {
	CSPACriticalSection::CLock lock( m_CritSect );
	
  TPtr* d = parr + delIndex;
  TPtr  r = (del==Delete || (del==defDelete && owns)) ? (delete *d,(TPtr)0) : *d;
  memcpy( d, d+1, (count-- - delIndex - 1)*sizeof(TPtr) );

  return r;
}




///////////////////////////////////////////////////////
// удалить элемент из массива
// ---
template <class Type>
inline Type* CPSyncArray<Type>::RemoveObj( Type *delObject, DelType del ) {
	CSPACriticalSection::CLock lock( m_CritSect );
	
  uint i = find_in_array( *this, delObject );
  return ( i != SYS_MAX_UINT ) ? RemoveInd(i,del) : 0;
}




///////////////////////////////////////////////////////
// отсоединить элемент от массива
// ---
template <class Type>
inline void CPSyncArray<Type>::DetachInd( uint delIndex ) {
	CSPACriticalSection::CLock lock( m_CritSect );
	
  memcpy( parr+delIndex, parr+delIndex+1, (count-- - delIndex - 1)*sizeof(TPtr) );
}




///////////////////////////////////////////////////////
// отсоединить элемент от массива
// ---
template <class Type>
inline void CPSyncArray<Type>::DetachObj( const Type *delObject ) {
	CSPACriticalSection::CLock lock( m_CritSect );
	
  uint i = find_in_array( *this, delObject );
  if ( i != SYS_MAX_UINT )
    DetachInd( i );
}




///////////////////////////////////////////////////////
// захват большего куска памяти ( если нужно )
// ---
template <class Type>
inline bool CPSyncArray<Type>::CatchMemory() {
	CSPACriticalSection::CLock lock( m_CritSect );
	
	if ( upper == count ) {
    uint size = upper;
		TPtr* p_tmp = (TPtr*)realloc( parr, (upper+delta) * sizeof(TPtr) );
		if ( p_tmp ) {
			upper+=delta;
			parr = p_tmp;
			return true;
		}
		return false;
	}
	return true;
}
/*
///////////////////////////////////////////////////////
// захват большего куска памяти ( если нужно )
// ---
template <class Type>
inline bool CPSyncArray<Type>::CatchMemory() {
	CSPACriticalSection::CLock lock( m_CritSect );
	
	if ( upper == count ) {
    uint size = upper;
		bool standard = false;
		TPtr* p_tmp;
		if ( parr )
			p_tmp = new (parr) TPtr[upper+delta];
		else {
			p_tmp = new TPtr[upper+delta];
			standard = true;
		}
		if ( p_tmp ) {
			if ( !standard && p_tmp == parr && _msize(p_tmp) == (upper*sizeof(TPtr)) ) {
				// мы вызвали realloc, но размер не изменился
				p_tmp = new TPtr[upper+delta];
				if ( p_tmp == 0 )
					return false;
				standard = true;
			}
			upper+=delta;
			if ( parr && standard )	{
				memcpy( p_tmp, parr, size*sizeof(TPtr) );
				delete [] parr;
			}
			parr = p_tmp;
			return true;
		}
		return false;
	}
	return true;
}
*/

// ---
template <class Type>
void destroy_array( CPSyncArray<Type>& arr ) {
  uint i = 0;
  for( CPSyncArray<Type>::TPtr* parr=arr.parr; i<arr.count; i++,parr++ ) {
    delete *parr;
    *parr = 0;
  }  
  arr.count = 0;
}



// ---
template <class Type>
void destroy_rest_array( CPSyncArray<Type>& arr, uint size ) {
  uint i = size;
  for( CPSyncArray<Type>::TPtr* parr=arr.parr + size; i<arr.count; i++,parr++ ) {
    delete *parr;
    *parr = 0;
  }  
  arr.count = size;
}



// ---
template <class Type>
void join_arrays( CPSyncArray<Type>& to, CPSyncArray<Type>& from ) {
  uint newUpper;
  uint newCount = to.count + from.count;

  for( newUpper=to.upper; newUpper<newCount; newUpper+=to.delta );
  if ( newUpper > to.upper ) {
    to.parr = (CPSyncArray<Type>::TPtr*)realloc(to.parr, (to.upper = newUpper)*sizeof(CPSyncArray<Type>::TPtr) );
  }
  memcpy( to.parr+to.count, from.parr, from.count * sizeof(CPSyncArray<Type>::TPtr) );
  to.count = newCount;

  if ( to.owns )
    from.owns = 0;
}



// ---
template <class Type>
void insert_array( CPSyncArray<Type>& to, CPSyncArray<Type>& from, uint index ) {

	if ( index > to.count )
		index = to.count;

  uint newUpper;
  uint newCount = to.count + from.count;
                                 
  for( newUpper=to.upper; newUpper < newCount; newUpper+=to.delta );
  CPSyncArray<Type>::TPtr* p_tmp = new CPSyncArray<Type>::TPtr[ to.upper = newUpper ];
  memcpy( p_tmp,                  to.parr,       index * sizeof(CPSyncArray<Type>::TPtr) );
  memcpy( p_tmp+index,            from.parr,     from.count * sizeof(CPSyncArray<Type>::TPtr) );
  memcpy( p_tmp+index+from.count, to.parr+index, (to.count-index) * sizeof(CPSyncArray<Type>::TPtr) );

  free( to.parr );
  to.parr = p_tmp;
  to.count = newCount;

  if ( to.owns )
		from.owns = 0;
}



// ---
template <class Type>
uint find_in_array( const CPSyncArray<Type>& arr, const Type* el ) {
	uint uiResult = SYS_MAX_UINT;
	
  CPSyncArray<Type>::TPtr* parr = arr.parr;

  for( uint i=0,c=arr.count; i<c; i++,parr++ ) {
    if ( *parr == el ) {
      uiResult = i;
			break;
		}
	}

	return uiResult;
}



#endif  // __PSYNCARRAY_H




