////////////////////////////////////////////////////////////////////
//
//  Pathstr.h
//  Smart path pointer
//  General purpose
//  Victor Matiouchenkov [victor@avp.ru], Kaspersky Labs. 1999
//  Copyright (c) Kaspersky Labs
//
////////////////////////////////////////////////////////////////////

#ifndef __PATHSTRW_H
#define __PATHSTRW_H

#include "cpointer.h"

#if !defined(_UNICODE)
__inline unsigned int __cdecl _wcsnextc(const wchar_t * _cpc) { return (unsigned int)*_cpc; }
__inline wchar_t * __cdecl _wcsninc(const wchar_t * _pc, size_t _sz) { return (wchar_t *)(_pc+_sz); }
__inline wchar_t * __cdecl _wcsdec(const wchar_t * _cpc1, const wchar_t * _cpc2) { return (wchar_t *)((_cpc1)>=(_cpc2) ? NULL : ((_cpc2)-1)); }
__inline wchar_t * __cdecl _wcsinc(const wchar_t * _pc) { return (wchar_t *)(_pc+1); }
_CRTIMP wchar_t*  __cdecl wcscat(wchar_t *, const wchar_t *);
//#else
//#define _CPUC   const unsigned char *
//_CRTIMP size_t __cdecl _mbslen(const unsigned char *);
#endif

// Класс для выполнения операций со строками, несущими файловые пути
// Выполняет операции объединения путей
// "c:\first" ^ "second\file.ext" -> "c:\first\second\file.ext"
// "c:\first" + "second\file.ext" -> "c:\firstsecond\file.ext"
 
static bool CPSW_Os2K = false;// Dont use thread locale oversetting LOBYTE(HIWORD(::GetVersion())) >= 5;

// ---
class CPathStrW : public CAPointer<wchar_t> {
protected :
  int selfLen;
public :
/*
  CPathStrW() : CAPointer<wchar_t>( new wchar_t[1] ), selfLen( 1 ) {
		if ( (wchar_t *)*this )
			*((wchar_t *)*this) = L'\0';
  }
*/
  CPathStrW() : CAPointer<wchar_t>(0), selfLen( 0 ) {
  }

  CPathStrW( int len ) : CAPointer<wchar_t>( new wchar_t[selfLen=len?len:1] ) {
		if ( (wchar_t *)*this )
	    *((wchar_t *)*this) = L'\0';
  }

  CPathStrW( const wchar_t *other ) : CAPointer<wchar_t>(new wchar_t[selfLen=(int)(other?wcslen(other):0) + 1]) {
		if ( (wchar_t *)*this )
	    *((wchar_t *)*this) = L'\0';
    *this = other;
  }
  
  CPathStrW( const char *other ) : CAPointer<wchar_t>(new wchar_t[selfLen=(int)(other?strlen(other):0) + 1]) {
		if ( other ) {
			DWORD dwSize = mb2wc(other, -1, NULL, 0) + 1;//::MultiByteToWideChar( CP_ACP, 0/*WC_SEPCHARS*/, other, -1, NULL, 0 ) + 1;
			Realloc( dwSize, false );
			if ( (wchar_t *)*this )
				mb2wc(other, -1, ((wchar_t *)*this), dwSize);//::MultiByteToWideChar( CP_ACP, 0/*WC_SEPCHARS*/, other, -1, ((wchar_t *)*this), dwSize );
		}
		else {
			Realloc( 1, false );
			if ( (wchar_t *)*this )
				*((wchar_t *)*this) = L'\0';
		}
  }
  
  CPathStrW( const CPathStrW &other ) : CAPointer<wchar_t>(new wchar_t[selfLen=(int)((wchar_t *)const_cast<CPathStrW &>(other) ? (wcslen((wchar_t *)const_cast<CPathStrW &>(other)) + 1) : 1)]) {
		if ( (wchar_t *)*this )
	    *((wchar_t *)*this) = L'\0';
    *this = (wchar_t *)const_cast<CPathStrW &>(other);
  }
	
  CPathStrW( wchar_t other ) : CAPointer<wchar_t>(new wchar_t[selfLen=3]) {
		/*
		int cW = other & 0xffff;
    *this = (wchar_t *)&cW;
		*/
    *((wchar_t *)*this) = other;
    *_wcsninc((wchar_t *)*this, 1) = L'\0';
  }
  
  wchar_t *operator =( const char *other ) {
		if ( other ) {
			DWORD dwSize = mb2wc(other, -1, NULL, 0);//::MultiByteToWideChar( CP_ACP, 0/*WC_SEPCHARS*/, other, -1, NULL, 0 ) + 1;
			Realloc( dwSize, false );
			if ( (wchar_t *)*this )
				mb2wc(other, -1, ((wchar_t *)*this), dwSize);//::MultiByteToWideChar( CP_ACP, 0/*WC_SEPCHARS*/, other, -1, ((wchar_t *)*this), dwSize );
		}
		else {
			Realloc( 1, false );
			if ( (wchar_t *)*this )
				*((wchar_t *)*this) = L'\0';
		}
    return (wchar_t *)*this;
  }
  
  wchar_t *operator =( const CPathStrW &other ) {
		/*
    *((wchar_t *)*this) = L'\0';
    *this += (wchar_t *)const_cast<CPathStrW &>(other);
    return *this;
		*/
    *this = (wchar_t *)const_cast<CPathStrW &>(other);
    return (wchar_t *)*this;
  }
  
  wchar_t *operator =( const wchar_t *other ) {
		/*
    *((wchar_t *)*this) = L'\0';
    *this += other;
    return *this;
		*/
		if ( other ) {
			Realloc( (int)wcslen(other) + 1, false );
			if ( (wchar_t *)*this )
				wcscpy( (wchar_t *)*this, other );
		}
		else {
			Realloc( 1, false );
			if ( (wchar_t *)*this )
				*((wchar_t *)*this) = L'\0';
		}
		return (wchar_t *)*this;
  }
  
  wchar_t *operator ^ ( const wchar_t *other ) {
		if ( other ) {
			int  len = (wchar_t *)*this ? (int)wcslen( (wchar_t *)*this ) : 0;
			Realloc( len + (int)wcslen(other) + 2 );
			if ( (wchar_t *)*this ) {
				bool trailingBack = false;
				if ( len ) {
					if ( _wcsnextc(_wcsninc(((wchar_t *)*this), len - 1)) == L'\\' ||
							 _wcsnextc(_wcsninc(((wchar_t *)*this), len - 1)) == L'/' )
						trailingBack = true;
				}
				if ( _wcsnextc((wchar_t *)*this) != L'\0' && !trailingBack && 
						 _wcsnextc(other) != L'\\' && _wcsnextc(other) != L'/' ) {
					wcscat( (wchar_t *)*this, L"\\" );
					trailingBack = true;
				}
      
				if ( trailingBack && (_wcsnextc(other) == L'\\' || _wcsnextc(other) == L'/') )
					other = _wcsinc( other );

				return wcscat( (wchar_t *)*this, other );
			}
		}
		return (wchar_t *)*this;
  }

  wchar_t *operator ^= ( const wchar_t *other ) {
    return operator ^ ( other );
  }
  
  wchar_t *operator + ( const wchar_t *other ) {
		if ( other ) {
			Realloc( selfLen + (int)wcslen(other) + !!!selfLen );
			return wcscat( (wchar_t *)*this, other );
		}
		return (wchar_t *)*this;
  }
  
  wchar_t *operator += ( const wchar_t *other ) {
    return operator + ( other );
  }
  
  void Realloc( int newLen, bool bSafe = true ) {
    if ( newLen > selfLen ) {
			if ( bSafe ) {
				wchar_t *p = Relinquish();
				CAPointer<wchar_t>::operator = ( new wchar_t[newLen] );
				//*((wchar_t *)*this) = L'\0';
				if ( p ) {
					if ( (wchar_t *)*this )
						wcscpy( (wchar_t *)*this, p );
					delete [] p;
				}
				else {
					if ( (wchar_t *)*this )
						*((wchar_t *)*this) = L'\0';
				}
			}
			else {
				CAPointer<wchar_t>::operator = ( new wchar_t[newLen] );
				if ( (wchar_t *)*this )
					*((wchar_t *)*this) = L'\0';
			}
      selfLen = newLen;
    }  
  }

protected : 
	int mb2wc( const char *lpMultiByteStr, int cbMultiByte, wchar_t *lpWideCharStr, int cchWideChar ) {
		int nRetVal;
		if ( CPSW_Os2K ) {
			LCID locale = ::GetThreadLocale();
			LCID system = ::GetSystemDefaultLCID();
			if ( locale != system )
				::SetThreadLocale( system );
    
			nRetVal = ::MultiByteToWideChar( CP_THREAD_ACP, MB_PRECOMPOSED, lpMultiByteStr, cbMultiByte, lpWideCharStr, cchWideChar );
    
			if ( locale != system )
				::SetThreadLocale( locale );
		}
		else
			nRetVal = MultiByteToWideChar( CP_ACP, MB_PRECOMPOSED, lpMultiByteStr, cbMultiByte, lpWideCharStr, cchWideChar );
  
		return nRetVal;
	}

};


// ---
class CMultiStrW : public CPathStrW {
protected :
  int selfSize;
	int count;
public :
  CMultiStrW() : CPathStrW( 2 ) {
		selfSize = 2;
		count = 0;
		*_wcsninc(((wchar_t *)*this), selfSize - 1) = L'\0';
  }

  CMultiStrW( int len ) : CPathStrW( len + 2 ) {
		selfSize = 2;
		count = 0;
		*_wcsninc(((wchar_t *)*this), selfSize - 1) = L'\0';
  }

  CMultiStrW( const wchar_t *other ) : CPathStrW(selfSize=(int)wcslen(other) + 2) {
		*this = other;
		*_wcsninc(((wchar_t *)*this), selfSize - 1) = L'\0';
		ComputeCount();
  }
  
  CMultiStrW( const wchar_t *other, int len ) : CPathStrW(2) {
		if ( *other ) {
			Realloc( len );
			memcpy(	((wchar_t *)*this), other, len * sizeof(wchar_t) );
			selfSize = len;
		}
		else
			selfSize = 2;
		*_wcsninc(((wchar_t *)*this), selfSize - 1) = L'\0';
		ComputeCount();
  }
  
  CMultiStrW( const CMultiStrW &other ) : CPathStrW(other.Size()) {
		memcpy(	((wchar_t *)*this), ((wchar_t *)const_cast<CMultiStrW&>(other)), other.Size() * sizeof(wchar_t) );
		selfSize = other.Size();
		ComputeCount();
  }

  wchar_t *operator =( const CMultiStrW &other ) {
		Realloc( other.Size() );
		selfSize = other.Size();
		memcpy(	((wchar_t *)*this), ((wchar_t *)const_cast<CMultiStrW&>(other)), other.Size() * sizeof(wchar_t) );
		ComputeCount();
		return (wchar_t *)*this;
  }
  
  wchar_t *operator =( const wchar_t *other ) {
		if ( other ) {
			Realloc( (int)wcslen(other) + 2 );
			selfSize = (int)wcslen(other) + 2;
			wcscpy( ((wchar_t *)*this), other );
			*_wcsninc(((wchar_t *)*this), selfSize - 1) = L'\0';
		}
		ComputeCount();
		return (wchar_t *)*this;
  }
  
  wchar_t *operator + ( const wchar_t *other ) {
		if ( other ) {
			int oldSize = selfSize;
			Realloc( selfSize + (int)wcslen(other) + 1 );
			selfSize = (oldSize > 2 ? oldSize : 1) + (int)wcslen(other) + 1;
			wcscpy( _wcsninc(((wchar_t *)*this), (oldSize > 2 ? oldSize - 1 : 0)), other );
			*_wcsninc(((wchar_t *)*this), selfSize - 1) = L'\0';
		}
		ComputeCount();
		return (wchar_t *)*this;
  }
  
  wchar_t *operator += ( const wchar_t *other ) {
    return operator + ( other );
  }
  
  void Realloc( int newLen ) {
    if ( newLen > selfLen ) {
      wchar_t *p = Relinquish();
      CAPointer<wchar_t>::operator = ( new wchar_t[newLen] );
      memcpy( (wchar_t *)*this, p, selfLen * sizeof(wchar_t) );
      delete [] p;
      selfLen = newLen;
    }  
  }

	wchar_t *operator []( int loc ) const { 
		int iC = 0;
		wchar_t *pC = (wchar_t *)const_cast<CMultiStrW&>(*this);
		while ( pC && _wcsnextc(pC) != L'\0' && iC < loc && iC != count ) {
			iC++;
			pC = _wcsninc( pC, wcslen(pC) ) + 1;
		}
		return pC;
	}

	int Size() const { return selfSize; }
	int Count() const { return count; }
	void Clean() {
		selfSize = 2;
		count = 0;
		CPathStrW::operator = ( L"" );
		*_wcsninc(((wchar_t *)*this), selfSize - 1) = L'\0';
	}

private :
	void ComputeCount() {
		count = 0;
		wchar_t *pC = (wchar_t *)*this;
		while ( pC && _wcsnextc(pC) != L'\0' ) {
			count++;
			pC = _wcsninc( pC, wcslen(pC) )  + 1;
		}
	}

  wchar_t *operator ^ ( const wchar_t *other ) {
		return 0;
  }
  wchar_t *operator ^= ( const wchar_t *other ) {
		return 0;
  }
  
};

// ---
class CMSIteratorW {
  const CMultiStrW &array;
				wchar_t			*curr;
public:
  CMSIteratorW( const CMultiStrW& arr ) 
    : array( arr ),
      curr( const_cast<CMultiStrW&>(arr) ) {
  }
  operator bool()        const { return _wcsnextc(curr) != L'\0'; }
  operator wchar_t*()      const { return curr; }
  wchar_t* operator ++()          { curr = _wcsninc(curr, wcslen(curr)) + 1; return curr; }
  wchar_t* operator ++(int)       { wchar_t *tmp = curr; curr = _wcsninc(curr, wcslen(curr)) + 1; return tmp; }
};




#endif
