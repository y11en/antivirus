////////////////////////////////////////////////////////////////////
//
//  ObjectSet.h
//  ObjectSet utility class definition
//  AVP general purposes stuff
//  Victor Matiouchenkov [victor@avp.ru], Kaspersky Labs. 2000-2003
//  Copyright (c) Kaspersky Labs
//
////////////////////////////////////////////////////////////////////
#ifndef __OBJECTSET_H__
#define __OBJECTSET_H__

#include <stdarg.h>
#include "SArray.h"


// ---
template<class Type> 
class CObjectSet : public CSArray<Type> {
public :
	CObjectSet();
	explicit CObjectSet( const CObjectSet &other );
	CObjectSet( int nCount, ... );

	bool operator | ( const Type& object ) const;
	bool operator == ( const CObjectSet& object ) const;
};

// ---
template <class Type>
inline CObjectSet<Type>::CObjectSet() :
	CSArray<Type>() {
}

// ---
template <class Type>
inline CObjectSet<Type>::CObjectSet( const CObjectSet &other ) :
	CSArray<Type>() {

	for ( int i=0,c=other.Count(); i<c; i++ ) {
		Add( other[i] );
	}
}


// ---
template <class Type>
inline CObjectSet<Type>::CObjectSet( int nCount, ... ) :
	CSArray<Type>( nCount ) {
	if ( nCount ) {
		va_list ArgList;
		va_start( ArgList, nCount );
		Type Val;
		do {
			memcpy( &Val, &va_arg(ArgList, Type), sizeof(Type) );
			Add( Val );
		} while( --nCount );
		va_end( ArgList );
	}
}


// ---
template <class Type>
inline bool CObjectSet<Type>::operator | ( const Type& object ) const {
	return IsExist( object );
}

// ---
template <class Type>
inline bool CObjectSet<Type>::operator == ( const CObjectSet& object ) const {
	if ( Count() == object.Count() ) {
		for ( int i=0,c=Count(); i<c; i++ ) {
			if ( (*this)[i] != object[i] )
				return false;
		}
	}
	return true;
}

#endif // __NUMSET_H__

