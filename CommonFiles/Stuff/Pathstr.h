////////////////////////////////////////////////////////////////////
//
//  Pathstr.h
//  Smart path pointer
//  General purpose
//  Victor Matiouchenkov [victor@avp.ru], Kaspersky Labs. 1999
//  Copyright (c) Kaspersky Labs
//
////////////////////////////////////////////////////////////////////

#ifndef __PATHSTR_H
#define __PATHSTR_H

#ifndef linux
 #include <tchar.h>
#else 
 //#include <wchar.h>
 //typedef wchar_t     TCHAR;
 #ifndef _TCHAR_DEFINED
  //#if !__STDC__
   typedef char  TCHAR;
  //#endif
  #define _TCHAR_DEFINED
 #endif
#endif 

#include "cpointer.h"

#if defined(_UNICODE) || defined(UNICODE)
#define CPS_TCSCLEN(p) (p ? _tcsclen(p) : 0)
#else
#define CPS_TCSCLEN(p) (p ? strlen(p) : 0)
#endif
	 
// Класс для выполнения операций со строками, несущими файловые пути
// Выполняет операции объединения путей
// "c:\first" ^ "second\file.ext" -> "c:\first\second\file.ext"
// "c:\first" + "second\file.ext" -> "c:\firstsecond\file.ext"
 
// ---
class CPathStr : public CAPointer<TCHAR> {
protected :
  int selfLen;
public :
/*
  CPathStr() : CAPointer<TCHAR>( new TCHAR[1] ), selfLen( 1 ) {
		if ( (TCHAR *)*this )
	    *((TCHAR *)*this) = L'\0';
  }
*/
  CPathStr() : CAPointer<TCHAR>(0), selfLen(0) {
  }
  CPathStr( int len ) : CAPointer<TCHAR>( new TCHAR[(selfLen=len?len:1)] ) {
		if ( (TCHAR *)*this )
	    *((TCHAR *)*this) = L'\0';
  }
/*  
  CPathStr( const TCHAR *other ) : CAPointer<TCHAR>(new TCHAR[1]), selfLen( 1 ) {
    *((TCHAR *)*this) = L'\0';
    *this += other;
  }
  
  CPathStr( const CPathStr &other ) : CAPointer<TCHAR>(new TCHAR[1]), selfLen( 1 ) {
    *((TCHAR *)*this) = L'\0';
    *this += (TCHAR *)const_cast<CPathStr &>(other);
  }
*/  
  CPathStr( const TCHAR *other ) : CAPointer<TCHAR>(new TCHAR[(selfLen=(int)(CPS_TCSCLEN(other)) + 1)]) {
		if ( (TCHAR *)*this )
	    *((TCHAR *)*this) = L'\0';
    *this = other;
  }
  
  CPathStr( const CPathStr &other ) : CAPointer<TCHAR>(new TCHAR[(selfLen=(int)(CPS_TCSCLEN((TCHAR *)const_cast<CPathStr &>(other)) + 1))]) {
		if ( (TCHAR *)*this )
	    *((TCHAR *)*this) = L'\0';
    *this = (TCHAR *)const_cast<CPathStr &>(other);
  }

  CPathStr( TCHAR other ) : CAPointer<TCHAR>(new TCHAR[(selfLen=3)]) {
		int cW = other & 0xffff;
    *this = (TCHAR *)&cW;
		/*
    *((TCHAR *)*this) = other;
    *_tcsninc((TCHAR *)*this, 1) = L'\0';
		*/
  }
  
  TCHAR *operator =( const CPathStr &other ) {
		/*
    *((TCHAR *)*this) = L'\0';
    *this += (TCHAR *)const_cast<CPathStr &>(other);
    return *this;
		*/
    *this = (TCHAR *)const_cast<CPathStr &>(other);
    return (TCHAR *)*this;
  }
  
  TCHAR *operator =( const TCHAR *other ) {
		/*
    *((TCHAR *)*this) = L'\0';
    *this += other;
    return *this;
		*/
		if ( other ) {
			Realloc( (int)(CPS_TCSCLEN(other) + 1), false );
			if ( (TCHAR *)*this )
				_tcscpy( (TCHAR *)*this, other );
		}
		else {
			Realloc( 1, false );
			if ( (TCHAR *)*this )
				*((TCHAR *)*this) = L'\0';
		}
		return (TCHAR *)*this;
  }
  
  TCHAR *operator ^ ( const TCHAR *other ) {
		if ( other ) {
			int  len = (TCHAR *)*this ? (int)CPS_TCSCLEN( (TCHAR *)*this ) : 0;
			Realloc( len + (int)(CPS_TCSCLEN(other) + 2) );
			if ( (TCHAR *)*this ) {
				bool trailingBack = false;
				if ( len ) {
					if ( _tcsnextc(_tcsninc(((TCHAR *)*this), len - 1)) == L'\\' ||
							 _tcsnextc(_tcsninc(((TCHAR *)*this), len - 1)) == L'/' )
						trailingBack = true;
				}
				if ( _tcsnextc((TCHAR *)*this) != L'\0' && !trailingBack && 
						 _tcsnextc(other) != L'\\' && _tcsnextc(other) != L'/' ) {
					_tcscat( (TCHAR *)*this, _T("\\") );
					trailingBack = true;
				}
      
				if ( trailingBack && (_tcsnextc(other) == L'\\' || _tcsnextc(other) == L'/') )
					other = _tcsinc( other );

				return _tcscat( (TCHAR *)*this, other );
			}
		}
		return (TCHAR *)*this;
  }

  TCHAR *operator ^= ( const TCHAR *other ) {
    return operator ^ ( other );
  }
  
  TCHAR *operator + ( const TCHAR *other ) {
		if ( other ) {
			Realloc( selfLen + CPS_TCSCLEN(other) + !!!selfLen );
			if ( (TCHAR *)*this )
				return _tcscat( (TCHAR *)*this, other );
		}
		return (TCHAR *)*this;
  }
  
  TCHAR *operator += ( const TCHAR *other ) {
    return operator + ( other );
  }
  
  void Realloc( int newLen, bool bSafe = true ) {
    if ( newLen > selfLen ) {
			if ( bSafe ) {
				TCHAR *p = Relinquish();
				CAPointer<TCHAR>::operator = ( new TCHAR[newLen] );
				//*((TCHAR *)*this) = L'\0';
				if ( p ) {
					if ( (TCHAR *)*this )
						_tcscpy( (TCHAR *)*this, p );
					delete [] p;
				}
				else {
					if ( (TCHAR *)*this )
						*((TCHAR *)*this) = L'\0';
				}
			}
			else {
				CAPointer<TCHAR>::operator = ( new TCHAR[newLen] );
				if ( (TCHAR *)*this )
					*((TCHAR *)*this) = L'\0';
			}
      selfLen = newLen;
    }  
  }
};


// ---
class CMultiStr : public CPathStr {
protected :
  int selfSize;
	int count;
public :
  CMultiStr() : CPathStr( 2 ) {
		selfSize = 2;
		count = 0;
		//*_tcsninc(((TCHAR *)*this), selfSize - 1) = L'\0';
		*(((TCHAR *)*this) + (selfSize - 1)) = L'\0';
  }

  CMultiStr( int len ) : CPathStr( len + 2 ) {
		selfSize = 2;
		count = 0;
		//*_tcsninc(((TCHAR *)*this), selfSize - 1) = L'\0';
		*(((TCHAR *)*this) + (selfSize - 1)) = L'\0';
  }

  CMultiStr( const TCHAR *other ) : CPathStr(selfSize=(int)CPS_TCSCLEN(other) + 2) {
		*this = other;
		//*_tcsninc(((TCHAR *)*this), selfSize - 1) = L'\0';
		*(((TCHAR *)*this) + (selfSize - 1)) = L'\0';
		ComputeCount();
  }
  
  CMultiStr( const TCHAR *other, int len ) : CPathStr(2) {
		selfSize = 2;
		//*_tcsninc(((TCHAR *)*this), selfSize - 1) = L'\0';
		*(((TCHAR *)*this) + (selfSize - 1)) = L'\0';
		Realloc( len );
		memcpy(	((TCHAR *)*this), other, len );
		ComputeCount();
  }
  
  CMultiStr( const CMultiStr &other ) : CPathStr(other.Size()) {
		memcpy(	((TCHAR *)*this), ((TCHAR *)const_cast<CMultiStr&>(other)), other.Size() );
		selfSize = other.Size();
		ComputeCount();
  }

  TCHAR *operator =( const CMultiStr &other ) {
		Realloc( other.Size() );
		selfSize = other.Size();
		memcpy(	((TCHAR *)*this), ((TCHAR *)const_cast<CMultiStr&>(other)), other.Size() );
		ComputeCount();
		return (TCHAR *)*this;
  }
  
  TCHAR *operator =( const TCHAR *other ) {
		if ( other ) {
			Realloc( (int)(CPS_TCSCLEN(other) + 2) );
			selfSize = (int)(CPS_TCSCLEN(other) + 2);
			_tcscpy( ((TCHAR *)*this), other );
			//*_tcsninc(((TCHAR *)*this), selfSize - 1) = L'\0';
			*(((TCHAR *)*this) + (selfSize - 1)) = L'\0';
		}
		ComputeCount();
		return (TCHAR *)*this;
  }
  
  TCHAR *operator + ( const TCHAR *other ) {
		if ( other ) {
			int oldSize = selfSize;
			Realloc( selfSize + (int)(CPS_TCSCLEN(other) + 1) );
			selfSize = (oldSize > 2 ? oldSize : 1) + (int)(CPS_TCSCLEN(other)) + 1;
			//_tcscpy( _tcsninc(((TCHAR *)*this), (oldSize > 2 ? oldSize - 1 : 0)), other );
			_tcscpy( (((TCHAR *)*this) + (oldSize > 2 ? oldSize - 1 : 0)), other );
			//*_tcsninc(((TCHAR *)*this), selfSize - 1) = L'\0';
			*(((TCHAR *)*this) + (selfSize - 1)) = L'\0';
		}
		ComputeCount();
		return (TCHAR *)*this;
  }
  
  TCHAR *operator += ( const TCHAR *other ) {
    return operator + ( other );
  }
  
  void Realloc( int newLen ) {
    if ( newLen > selfLen ) {
      TCHAR *p = Relinquish();
      CAPointer<TCHAR>::operator = ( new TCHAR[newLen] );
      memcpy( (TCHAR *)*this, p, selfLen );
      delete [] p;
      selfLen = newLen;
    }  
  }

	TCHAR *operator []( int loc ) const { 
		int iC = 0;
		TCHAR *pC = (TCHAR *)const_cast<CMultiStr&>(*this);
		while ( pC && _tcsnextc(pC) != L'\0' && iC < loc && iC != count ) {
			iC++;
			pC = _tcsninc( pC, _tcsclen(pC) ) + 1;
		}
		return pC;
	}

	int Size() const { return selfSize; }
	int Count() const { return count; }

private :
	void ComputeCount() {
		count = 0;
		TCHAR *pC = (TCHAR *)*this;
		while ( pC && _tcsnextc(pC) != L'\0' ) {
			count++;
			pC = _tcsninc( pC, _tcsclen(pC) )  + 1;
		}
	}

  TCHAR *operator ^ ( const TCHAR *other ) {
		return 0;
  }
  TCHAR *operator ^= ( const TCHAR *other ) {
		return 0;
  }
  
};

// ---
class CMSIterator {
  const CMultiStr &array;
				TCHAR			*curr;
public:
  CMSIterator( const CMultiStr& arr ) 
    : array( arr ),
      curr( const_cast<CMultiStr&>(arr) ) {
  }
  operator bool()        const { return _tcsnextc(curr) != L'\0'; }
  operator TCHAR*()      const { return curr; }
  TCHAR* operator ++()          { curr = _tcsninc(curr, _tcsclen(curr)) + 1; return curr; }
  TCHAR* operator ++(int)       { TCHAR *tmp = curr; curr = _tcsninc(curr, _tcsclen(curr)) + 1; return tmp; }
/*
  TCHAR* operator --()          { while(curr != array && !*(--curr)); while(curr != array && *(--curr)); return curr; }
  TCHAR* operator --(int)       { TCHAR *tmp = curr; while(curr != array && !*(--curr)); while(curr != array && *(--curr)); return curr; }
*/
};




#endif
