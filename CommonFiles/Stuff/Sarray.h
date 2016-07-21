////////////////////////////////////////////////////////////////////
//
//  Sarray.h
//  CSArray template
//  General purpose
//  Andrey Doukhvalov [petrovitch@avp.ru], Kaspersky Labs. 1999
//  Victor Matiouchenkov [victor@avp.ru], Kaspersky Labs. 1999
//  Copyright (c) Kaspersky Labs
//
////////////////////////////////////////////////////////////////////

#ifndef __SARRAY_H
#define __SARRAY_H

#ifndef __STDLIB_H
#include <stdlib.h>
#endif

#ifndef _INC_MEMORY
#include <memory.h>
#endif

#include "stddefs.h"

#define SYS_MAX_UINT  (0xffffffff)

///////////////////////////////////////////////////////
// одномерный массив обьектов не содержащих указателей !!!!!!!
// В массиве нельзя хранить об'екты содержащие указатели или
// классы с указателями, а также абстрактные классы с наследниками
// ---
template <class Type>
class CSArray {
protected :
  uint   count; // размерность массива
	uint   upper;
	uint16 delta;
	Type*  parr;

public :
	CSArray( uint i_max=0, uint16 i_delta=1 );
	CSArray( const CSArray<Type>& other );
	~CSArray() { delete [] (char *)(parr); parr = 0; count = 0;}

	uint16 Delta() const                                 { return delta; }
	void   Delta( uint16 newDelta )                      { delta = newDelta; }

  void   SetSize( uint newSize, bool clear=true );     // указать новый размер массива
  void   Reserve( uint additionalSpace );              // зарезервировать место под элементы
  void   Flush() { count = 0; }                        // обнулить количество элементов
  void   HardFlush() { Flush(); Adjust(); }            // освободить всю память
  void   Adjust();                                     // удалить лишнюю память
  Type*  Add();                                        // добавить элемент в конец массива
  Type*  AddItems( uint n );                           // добавить n элементов в конец массива
  Type*  Add(const Type& ent);                         // добавить элемент в конец массива
  Type*  AddAt( const Type& ent, uint index )          { return InsertInd( index, ent ); }
  Type*  AddAfter( const Type& ent, uint index );      // добавить элемент после указанного
  Type*  InsertObj( const Type& index, const Type& ent ); // вставить элемент перед указанным
  Type*  InsertInd( uint index, const Type& ent );        // вставить элемент перед указанным
  void   RemoveInd( uint delIndex );                   // удалить элемент из массива
  void   RemoveObj( const Type& delObject );           // удалить элемент из массива
  void   Fill( uint fillCount, const Type& fillData ); // заполнить массив значениями
  uint   FindIt( const Type& object ) const;           // вернуть индекс элемента в массиве
  bool   IsExist( const Type& object ) const;          // true если элемент найден
  uint   Count()    const { return count; }            // дать количество элементов массива
  int    MaxIndex() const { return count-1; }          // дать количество элементов массива

  CSArray<Type>& operator = ( const CSArray<Type>& o );
  CSArray<Type>& operator += ( const CSArray<Type>& o );
  Type& operator []( uint loc ) const { return parr[loc]; }

  typedef int (*CompFunc)( const Type *, const Type * );
	void   Sort( CompFunc comp );                        // сортировать массив

	Type	*GetAddr() { return parr; }	                   // выдать адрес начала массива

protected :
	bool   CatchMemory();                                // захватить память

  #if _MSC_VER >= 1300
    template<class Type>
  #endif
  friend void  set_array_size( CSArray<Type>& to, uint newSize, bool clear );
  
  #if _MSC_VER >= 1300
    template<class Type>
  #endif
  friend Type* add_n_to_array( CSArray<Type>& to, uint n );

  #if _MSC_VER >= 1300
    template<class Type>
  #endif
  friend uint  find_in_array( const CSArray<Type>& arr, const Type& object );

  #if _MSC_VER >= 1300
    template<class Type>
  #endif
  friend void  fill_array( CSArray<Type>& arr, uint fillCount, const Type& fillData );

  #if _MSC_VER >= 1300
    template<class Type>
  #endif
  friend void  reserve_space( CSArray<Type>& arr, uint addSpace );
};


// ---
template <class Type>
class SBIterator {
  CSArray<Type>& array;
  uint          curr;
public:
  SBIterator( const CSArray<Type>& arr ) 
    : array( arr ),
      curr( arr.count-1 ) {
  }
  operator bool()        const { return curr != SYS_MAX_UINT; }
  operator Type&()       const { return array[curr]; }
  Type& operator --(int)       { return array[ ((bool)*this) ? curr-- : 0 ]; }
};



///////////////////////////////////////////////////////
// массив с итераторными ф-иями
// ---
template <class Type>
class  SIArray : public CSArray<Type> {
public :
	SIArray( uint i_max=0, uint16 i_delta=1 )
		: CSArray<Type>( i_max, i_delta ) {}
	SIArray( const CSArray<Type>& other )
		: CSArray<Type>( other ) {}

	typedef void ( *IteratorFunc ) ( Type& obj );
	void   ForEach( IteratorFunc func ) const;

	typedef void ( *ParIteratorFunc ) ( Type& obj, void *pars );
	void   ForEach( ParIteratorFunc func, void *pars ) const;

	typedef bool ( *CompareFunc ) ( Type& obj, void *pars );
	uint   FirstThat( CompareFunc func, void *pars, uint from=0 ) const;
  
  friend void  for_each_in_array( const SIArray<Type>& arr, IteratorFunc func );
  friend void  for_each_in_array( const SIArray<Type>& arr, ParIteratorFunc func, void* pars );
  friend uint  first_that_in_array( const SIArray<Type>& arr, CompareFunc func, void* pars, uint from );
};



///////////////////////////////////////////////////////
// Конструктор массива
// ---
template <class Type>
inline CSArray<Type>::CSArray( uint i_max, uint16 i_delta ) {
  count = 0;
	upper = 0;
	parr  = 0;
	delta = i_delta;
	SetSize( i_max );
}



///////////////////////////////////////////////////////
// Конструктор копирования массива
// ---
template <class Type>
inline CSArray<Type>::CSArray( const CSArray<Type>& o ) {
  count = o.count;
  upper = o.upper;
  delta = o.delta;
  parr  = !upper? 0 : (Type*)new char[ upper*sizeof(Type) ];
  if ( count ) {
    if ( count > upper )
      count = upper;
    memcpy( parr, o.parr, count*sizeof(Type) );
  }
}



///////////////////////////////////////////////////////
// указать новый размер массива
// если clear != 0 массив очистится
// ---
template <class Type>
inline void CSArray<Type>::SetSize( uint newSize, bool clear ) {
  set_array_size( *this, newSize, clear );
}




///////////////////////////////////////////////////////
// зарезервировать место под n элементов
// ---
template <class Type>
inline void CSArray<Type>::Reserve( uint additionalSpace ) {
  reserve_space( *this, additionalSpace );
}


///////////////////////////////////////////////////////
// отсечь неиспользуемую часть
// ---
template <class Type>
inline void CSArray<Type>::Adjust() {
  if ( count < upper )
    SetSize( count, 0 );
}



///////////////////////////////////////////////////////
// добавить элемент в конец массива
// и вернуть указатель на него
// --- !!!!!!!!!!!
template <class Type>
inline Type* CSArray<Type>::Add() {
  if ( CatchMemory() )
		return &parr[ count++ ];
	return (Type*)0;
}




///////////////////////////////////////////////////////
// добавить n элементов в конец массива
// и вернуть указатель на первый добавленный
// --- 
template <class Type>
inline Type* CSArray<Type>::AddItems( uint n ) {
  return n ? add_n_to_array(*this,n) : 0;
}




///////////////////////////////////////////////////////
// добавить элемент в конец массива
// ---
template <class Type>
inline Type* CSArray<Type>::Add( const Type& ent ) {
  if ( CatchMemory() )
		return (Type*)memcpy( parr+count++, &ent, sizeof(Type) );
	return (Type*)0;
}




///////////////////////////////////////////////////////
// добавить элемент после заданного
// ---
template <class Type>
inline Type* CSArray<Type>::AddAfter( const Type& ent, uint index ) {
  if ( CatchMemory() ) {
		memmove( parr+index+2, parr+index+1, sizeof(Type)*(count++-index-1) );
		return (Type*)memcpy( parr+index+1, &ent, sizeof(Type) );
	}
	return (Type*)0;
}




///////////////////////////////////////////////////////
// вставить элемент перед указанным
// ---
template <class Type>
inline Type* CSArray<Type>::InsertInd( uint index, const Type& ent ) {
  if ( CatchMemory() ) {                             // добавить памяти, если все использовано
		memmove( parr+index+1, parr+index, (count++-index) * sizeof(Type) );
																							 // передвинем вправо все элементы массива с последнего до указанного
		return (Type*)memcpy( parr+index, &ent, sizeof(Type) );  // записываем новый элемент
	}
	return (Type*)0;
}



///////////////////////////////////////////////////////
// вставить элемент перед указанным
// ---
template <class Type>
inline Type* CSArray<Type>::InsertObj( const Type& ind, const Type& ent ) {
  uint index = FindIt(ind);
  if ( index != SYS_MAX_UINT )
    return InsertInd( index, ent );
  else
    return 0;
}



///////////////////////////////////////////////////////
// вернуть индекс элемента в массиве
// --- 
template <class Type>
inline uint CSArray<Type>::FindIt( const Type& object ) const {
  return find_in_array( *this, object );
}




///////////////////////////////////////////////////////
// true если элемент найден
// --- 
template <class Type>
inline bool CSArray<Type>::IsExist( const Type& object ) const {
  return find_in_array( *this, object ) != SYS_MAX_UINT;
}



///////////////////////////////////////////////////////
// удалить элемент из массива
// ---
template <class Type>
inline void CSArray<Type>::RemoveObj( const Type& delObject ) {
  uint ind = FindIt( delObject );
  if ( ind != SYS_MAX_UINT )
    RemoveInd( ind );
}




///////////////////////////////////////////////////////
// удалить элемент из массива
// ---
template <class Type>
inline void CSArray<Type>::RemoveInd( uint delIndex ) { 
  memcpy( parr+delIndex, parr+delIndex+1, (count-- - delIndex-1)*sizeof(Type) );
} 


///////////////////////////////////////////////////////
// заполнить массив значениями
// ---
template <class Type>
inline void CSArray<Type>::Fill( uint fillCount, const Type& fillData ) {
  fill_array( *this, fillCount, fillData );
}




///////////////////////////////////////////////////////
// присвоение массива массиву
// ---
template <class Type>
inline CSArray<Type>& CSArray<Type>::operator = ( const CSArray<Type>& o ) {
  if ( upper < o.count ) {
    delete [] (char *)( parr );
    upper=o.count;
    parr = o.count ? (Type*)new char[ o.count * sizeof(Type) ] : 0;
  }
  count = o.count;
  memcpy( parr, o.parr, count * sizeof(Type) );
  return *this;
}




///////////////////////////////////////////////////////
// присвоение массива массиву
// ---
template <class Type>
inline CSArray<Type>& CSArray<Type>::operator += ( const CSArray<Type>& o ) {
  if ( upper < count + o.count ) {
    upper = uint16(count + o.count);
		Type* p_tmp = upper ? (Type*)new char[ upper * sizeof(Type) ] : 0;
		memcpy( p_tmp, parr, count * sizeof(Type) );
		delete [] (char *)( parr );
		parr = p_tmp;
  }
  memcpy( parr+count, o.parr, o.count * sizeof(Type) );
	count += o.count;
  return *this;
}




///////////////////////////////////////////////////////
// захват большего куска памяти ( если нужно )
// ---
template <class Type>
inline bool CSArray<Type>::CatchMemory() {
	if (upper == count) {
    uint   cpy = upper;
		Type *p_tmp = (Type*)new char[ (upper+delta) * sizeof(Type) ];
		if ( p_tmp ) {
			upper+=delta;
			if ( parr ) {
				memcpy( p_tmp, parr, cpy * sizeof(Type) );
				delete [] (char *)( parr );
			}
			parr = p_tmp;
			return true;
		}
		return false;
  }
	return true;
}




///////////////////////////////////////////////////////
// выполнить функцию для каждого элемента
// ---
template <class Type>
inline void SIArray<Type>::ForEach( IteratorFunc func ) const {
  for_each_in_array( *this, func );
}




///////////////////////////////////////////////////////
// выполнить функцию с параметрами для каждого элемента
// ---
template <class Type>
inline void SIArray<Type>::ForEach( ParIteratorFunc func, void* pars ) const {
  for_each_in_array( *this, func, pars );
}




///////////////////////////////////////////////////////
// найти элемент по условию
// ---
template <class Type>
inline uint SIArray<Type>::FirstThat( CompareFunc func, void* pars, uint from ) const {
  return first_that_in_array( *this, func, pars, from );
}




///////////////////////////////////////////////////////
// сортировать массив
// --- !!!!!!!!!!
template <class Type>
inline void CSArray<Type>::Sort( CompFunc fcmp ) {
  typedef int (*QCompFunc)( const void*, const void* );
	qsort( (void *)parr, count, sizeof(Type), (QCompFunc)fcmp );
}


// ---
template <class Type>
void set_array_size( CSArray<Type>& arr, uint newSize, bool clear ) {
  if ( newSize != arr.upper ) {
    arr.upper = newSize;
    Type *p_tmp = newSize ? (Type*)new char[ newSize * sizeof(Type) ] : 0;
    if ( clear )
      arr.count = 0;
    else {
      arr.count = arr.count < newSize ? arr.count : newSize;
      if ( arr.count && arr.parr )
        memcpy( p_tmp, arr.parr, arr.count * sizeof(Type) );
    }
    delete [] (char *)( arr.parr );
    arr.parr = p_tmp;
  }
  else if ( clear )
    arr.count = 0;
}


// ---
template <class Type>
Type* add_n_to_array( CSArray<Type>& to, uint n ) {
  uint oc = to.count;
  to.count += n;
  if (to.upper < to.count ) {
    while( to.upper < to.count )
      to.upper += to.delta;
    Type *p_tmp = (Type*)new char[ to.upper * sizeof(Type) ];
    memcpy( p_tmp, to.parr, oc * sizeof(Type) );
    delete [] (char *)( to.parr );
    to.parr = p_tmp;
  }
  return &to.parr[oc];
}


// ---
template <class Type>
uint find_in_array( const CSArray<Type>& arr, const Type& object ) {
  Type* parr = (Type*)arr.parr;
  for( uint i=0; i<arr.count; i++,parr++ )
    if ( !memcmp( parr, &object, sizeof(Type) ) )
      return i;
  return SYS_MAX_UINT;
}


// ---
template <class Type>
void fill_array( CSArray<Type>& arr, uint fillCount, const Type& fillData ) {
  if ( arr.upper < fillCount ) {
    delete [] (char *)( arr.parr );
    arr.parr = (Type*)new char[ (arr.upper=fillCount) * sizeof(Type) ];
  }
  arr.count = fillCount;
  Type* parr = (Type*)arr.parr;
  for ( uint i=0; i<arr.count; i++,parr++ )
    memcpy( parr, &fillData, sizeof(Type) );
}


// ---
template <class Type>
#if _MSC_VER < 1300
void for_each_in_array( const SIArray<Type>& arr, SIArray<Type>::IteratorFunc func ) {
#else
void for_each_in_array( const SIArray<Type>& arr, typename SIArray<Type>::IteratorFunc func ) {
#endif
  Type* parr = (Type*)arr.parr;
	for( uint i=0; i<arr.count; i++,parr++ )
		func( *parr );
}


// ---
template <class Type>
#if _MSC_VER < 1300
void for_each_in_array( const SIArray<Type>& arr, SIArray<Type>::ParIteratorFunc func, void* pars ) {
#else
void for_each_in_array( const SIArray<Type>& arr, typename SIArray<Type>::ParIteratorFunc func, void* pars ) {
#endif
  Type* parr = (Type*)arr.parr;
	for( uint i=0; i<arr.count; i++,parr++ )
		func( *parr, pars );
}


// ---
template <class Type>
#if _MSC_VER < 1300
uint first_that_in_array( const SIArray<Type>& arr, SIArray<Type>::CompareFunc func, void* pars, uint from ) {
#else
uint first_that_in_array( const SIArray<Type>& arr, typename SIArray<Type>::CompareFunc func, void* pars, uint from ) {
#endif
  Type* parr = ((Type*)arr.parr) + from;
	for( uint i=from; i<arr.count; i++,parr++ ) {
		if ( func(*parr,pars) )
			return i;
	}
	return SYS_MAX_UINT;
}


// ---
template <class Type>
void reserve_space( CSArray<Type>& arr, uint additionalSpace ) {
  uint space = arr.upper - arr.count;
  if ( additionalSpace > space ) {
    additionalSpace -= space;
    arr.upper += additionalSpace;
    Type *p_tmp = (Type*)new char[ arr.upper * sizeof(Type) ];
    if ( arr.count && arr.parr )
      memcpy( p_tmp, arr.parr, arr.count * sizeof(Type) );
    delete [] (char *)( arr.parr );
    arr.parr = p_tmp;
  }
}



#endif  // __SARRAY_H

