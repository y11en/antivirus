////////////////////////////////////////////////////////////////////
//
//  Pathstr.h
//  Smart path pointer
//  General purpose
//  Victor Matiouchenkov [victor@avp.ru], Kaspersky Labs. 1999
//  Copyright (c) Kaspersky Labs
//
////////////////////////////////////////////////////////////////////

#ifndef __PATHSTRA_H
#define __PATHSTRA_H

#include <mbstring.h>
#include <tchar.h>
#include "cpointer.h"


// Класс для выполнения операций со строками, несущими файловые пути
// Выполняет операции объединения путей
// "c:\first" ^ "second\file.ext" -> "c:\first\second\file.ext"
// "c:\first" + "second\file.ext" -> "c:\firstsecond\file.ext"
 
static bool CPSA_Os2K = false;// Dont use thread locale oversetting LOBYTE(HIWORD(::GetVersion())) >= 5;

// ---
class CPathStrA : public CAPointer<char> {
protected :
  int selfLen;
public :
/*
  CPathStrA() : CAPointer<char>( new char[1 << 1] ), selfLen( 1 ) {
		if ( (char *)*this )
			*((char *)*this) = '\0';
  }
*/
  CPathStrA() : CAPointer<char>(0), selfLen(0) {
  }

  CPathStrA( int len ) : CAPointer<char>( new char[(selfLen=len?len:1) << 1] ) {
		if ( (char *)*this )
	    *((char *)*this) = '\0';
  }
  CPathStrA( const char *other ) : 
	  CAPointer<char>(new char[(selfLen=(int)(other
#if defined(_MBCS)
	//?_mbslen((_CPUC)other)
	?strlen(other)
#else
	?strlen(other)
#endif
		:0) + 1) << 1]) {
		if ( (char *)*this )
	    *((char *)*this) = '\0';
    *this = other;
  }
  
	CPathStrA( const wchar_t *other ) : CAPointer<char>(new char[selfLen=(int)(other?wcslen(other):0) + 1]) {
		if ( other ) {
			DWORD dwSize = wc2mb( other, -1, NULL, 0 );//::WideCharToMultiByte( CP_ACP, 0/*MB_COMPOSITE*/, other, -1, NULL, 0, NULL, NULL );
			Realloc( dwSize, false );
			if ( (char *)*this )
				wc2mb( other, -1, ((char *)*this), dwSize );//::WideCharToMultiByte( CP_ACP, 0/*MB_COMPOSITE*/, other, -1, ((char *)*this), dwSize, NULL, NULL );
		}
		else {
			Realloc( 1, false );
			if ( (char *)*this )
				*((char *)*this) = '\0';
		}
	}
	
	CPathStrA( const CPathStrA &other ) : 
	  CAPointer<char>(new char[(selfLen=(int)((char *)const_cast<CPathStrA &>(other) ? (
#if defined(_MBCS)
	//_mbslen((_CPUC)(char *)const_cast<CPathStrA &>(other)) + 1) : 1)) << 1]) 
	strlen((char *)const_cast<CPathStrA &>(other)) + 1) : 1)) << 1]) 
#else
		strlen((char *)const_cast<CPathStrA &>(other)) + 1) : 1)) << 1]) 
#endif
	{
		if ( (char *)*this )
			*((char *)*this) = '\0';
    *this = (char *)const_cast<CPathStrA &>(other);
  }

  CPathStrA( char other ) : CAPointer<char>(new char[(selfLen=3) << 1]) {
		int cW = other & 0xffff;
    *this = (char *)&cW;
  }
  
  char *operator =( const wchar_t *other ) {
		if ( other ) {
			DWORD dwSize = wc2mb( other, -1, NULL, 0 );//::WideCharToMultiByte( CP_ACP, 0/*MB_COMPOSITE*/, other, -1, NULL, 0, NULL, NULL );
			Realloc( dwSize, false );
			if ( (char *)*this )
				wc2mb( other, -1, ((char *)*this), dwSize );//::WideCharToMultiByte( CP_ACP, 0/*MB_COMPOSITE*/, other, -1, ((char *)*this), dwSize, NULL, NULL );
		}
		else {
			Realloc( 1, false );
			if ( (char *)*this )
				*((char *)*this) = '\0';
		}
    return (char *)*this;
	}
	
  char *operator =( const CPathStrA &other ) {
    *this = (char *)const_cast<CPathStrA &>(other);
    return (char *)*this;
  }
  
  char *operator =( const char *other ) {
		if ( other ) {
#if defined(_MBCS)
			Realloc( (int)/*_mbslen*/strlen(other) + 1, false );
			if ( (char *)*this )
				_mbscpy( (_PUC)(char *)*this, (_CPUC)other );
#else
			Realloc( (int)strlen(other) + 1, false );
			if ( (char *)*this )
				strcpy( (char *)*this, other );
#endif
		}
		else {
			Realloc( 1, false );
			if ( (char *)*this )
				*((char *)*this) = '\0';
		}
		return (char *)*this;
  }
  
  char *operator ^ ( const char *other ) {
		if ( other ) {
#if defined(_MBCS)
			int  len = (char *)*this ? (int)/*_mbslen*/strlen( (char *)*this ) : 0;
			Realloc( len + (int)/*_mbslen*/strlen(other) + 2 );
			if ( (char *)*this ) {
				bool trailingBack = false;
				if ( len ) {
					if ( _mbsnextc(_mbsninc((_CPUC)((char *)*this), len - 1)) == '\\' ||
						_mbsnextc(_mbsninc((_CPUC)((char *)*this), len - 1)) == '/' )
						trailingBack = true;
				}
				if ( _mbsnextc((_CPUC)(char *)*this) != L'\0' && !trailingBack && 
					_mbsnextc((_CPUC)other) != '\\' && _mbsnextc((_CPUC)other) != '/' ) {
					_mbscat( (_PUC)(char *)*this, (_CPUC)"\\" );
					trailingBack = true;
				}
      
				if ( trailingBack && (_mbsnextc((_CPUC)other) == '\\' || _mbsnextc((_CPUC)other) == '/') )
					other = (_CPC)_mbsinc( (_CPUC)other );
				
				return (_PC)_mbscat( (_PUC)(char *)*this, (_CPUC)other );
			}
#else
			int  len = (int)strlen( (char *)*this );
			Realloc( len + (int)strlen(other) + 2 );
			if ( (char *)*this ) {
				bool trailingBack = false;
				if ( len ) {
					if ( *((char *)*this + (len - 1)) == '\\' ||
						*((char *)*this + (len - 1)) == '/' )
						trailingBack = true;
				}
				if ( *((char *)*this) != 0 && !trailingBack && 
					*(other) != '\\' && *(other) != '/' ) {
					strcat( (char *)*this, "\\" );
					trailingBack = true;
				}
      
				if ( trailingBack && (*(other) == '\\' || *(other) == '/') )
					other = other + 1;
				
				return strcat( (char *)*this, other );
			}
#endif
		}
		return (char *)*this;
  }

  char *operator ^= ( const char *other ) {
    return operator ^ ( other );
  }
  
  char *operator + ( const char *other ) {
		if ( other ) {
#if defined(_MBCS)
			Realloc( selfLen + (int)/*_mbslen*/strlen(other) + !!!selfLen );
			return (_PC)_mbscat( (_PUC)(char *)*this, (_CPUC)other );
#else
			Realloc( selfLen + (int)strlen(other) + 1 );
			return strcat( (char *)*this, other );
#endif
		}
		return (char *)*this;
  }
  
  char *operator += ( const char *other ) {
    return operator + ( other );
  }
  
  void Realloc( int newLen, bool bSafe = true ) {
    if ( newLen > selfLen ) {
			if ( bSafe ) {
				char *p = Relinquish();
				CAPointer<char>::operator = ( new char[newLen] );
				//*((char *)*this) = L'\0';
				if ( p ) {
					if ( (char *)*this )
		#if defined(_MBCS)
						_mbscpy( (_PUC)(char *)*this, (_CPUC)p );
		#else
						strcpy( (char *)*this, p );
		#endif
					delete [] p;
				}
				else {
					if ( (char *)*this )
						*((char *)*this) = '\0';
				}
			}
			else {
				CAPointer<char>::operator = ( new char[newLen] );
				if ( (char *)*this )
					*((char *)*this) = '\0';
			}
      selfLen = newLen;
    }  
  }
protected :
	int wc2mb( const wchar_t *lpWideCharStr, int cchWideChar, char *lpMultiByteStr, int cbMultiByte ) {
		int nRetVal;
  
		if ( CPSA_Os2K ) {
			LCID locale = ::GetThreadLocale();
			LCID system = ::GetSystemDefaultLCID();
			if ( locale != system )
				::SetThreadLocale( system );
    
			nRetVal = ::WideCharToMultiByte( CP_THREAD_ACP, WC_COMPOSITECHECK, lpWideCharStr, cchWideChar, lpMultiByteStr, cbMultiByte, 0, 0 ); 
    
			if ( locale != system )
				::SetThreadLocale( locale );
		}
		else
			nRetVal = ::WideCharToMultiByte( CP_ACP, WC_COMPOSITECHECK, lpWideCharStr, cchWideChar, lpMultiByteStr, cbMultiByte, 0, 0 );
  
		return nRetVal;
	}

};


// ---
class CMultiStrA : public CPathStrA {
protected :
  int selfSize;
	int count;
public :
  CMultiStrA() : CPathStrA( 2 ) {
		selfSize = 2;
		count = 0;
#if defined(_MBCS)
		//*_mbsninc((_CPUC)((char *)*this), selfSize - 1) = '\0';
		((char *)*this)[selfSize - 1] = '\0';
#else
		((char *)*this)[selfSize - 1] = '\0';
#endif
  }

  CMultiStrA( int len ) : CPathStrA( len + 2 ) {
		selfSize = 2;
		count = 0;
#if defined(_MBCS)
		//*_mbsninc((_CPUC)((char *)*this), selfSize - 1) = '\0';
		((char *)*this)[selfSize - 1] = '\0';
#else
		((char *)*this)[selfSize - 1] = '\0';
#endif
  }

#if defined(_MBCS)
  CMultiStrA( const char *other ) : CPathStrA(selfSize=(int)/*_mbslen*/strlen(other) + 2) {
		*this = other;
		//*_mbsninc((_CPUC)((char *)*this), selfSize - 1) = '\0';
		((char *)*this)[selfSize - 1] = '\0';
		ComputeCount();
  }
#else
  CMultiStrA( const char *other ) : CPathStrA(selfSize=(int)strlen(other) + 2) {
		*this = other;
		((char *)*this)[selfSize - 1] = '\0';
		ComputeCount();
  }
#endif
  
  CMultiStrA( const char *other, int len ) : CPathStrA(2) {
		selfSize = 2;
#if defined(_MBCS)
		//*_mbsninc((_CPUC)((char *)*this), selfSize - 1) = '\0';
		((char *)*this)[selfSize - 1] = '\0';
#else
		((char *)*this)[selfSize - 1] = '\0';
#endif
		Realloc( len );
		memcpy(	((char *)*this), other, len );
		ComputeCount();
  }
  
  CMultiStrA( const CMultiStrA &other ) : CPathStrA(other.Size()) {
		memcpy(	((char *)*this), ((char *)const_cast<CMultiStrA&>(other)), other.Size() );
		selfSize = other.Size();
		ComputeCount();
  }

  char *operator =( const CMultiStrA &other ) {
		Realloc( other.Size() );
		selfSize = other.Size();
		memcpy(	((char *)*this), ((char *)const_cast<CMultiStrA&>(other)), other.Size() );
		ComputeCount();
		return (char *)*this;
  }
  
  char *operator =( const char *other ) {
		if ( other ) {
#if defined(_MBCS)
			Realloc( (int)/*_mbslen*/strlen(other) + 2 );
			selfSize = (int)/*_mbslen*/strlen(other) + 2;
			_mbscpy( (_PUC)((char *)*this), (_CPUC)other );
			//*_mbsninc((_CPUC)((char *)*this), selfSize - 1) = '\0';
			((char *)*this)[selfSize - 1] = '\0';
#else
			Realloc( (int)strlen(other) + 2 );
			selfSize = (int)strlen(other) + 2;
			strcpy( ((char *)*this), other );
			((char *)*this)[selfSize - 1] = '\0';
#endif
		}
		ComputeCount();
		return (char *)*this;
  }
  
  char *operator + ( const char *other ) {
		if ( other ) {
			int oldSize = selfSize;
#if defined(_MBCS)
			Realloc( selfSize + (int)/*_mbslen*/strlen(other) + 1 );
			selfSize = (oldSize > 2 ? oldSize : 1) + (int)/*_mbslen*/strlen(other) + 1;
			//_mbscpy( _mbsninc((_CPUC)((char *)*this), (oldSize > 2 ? oldSize - 1 : 0)), (_CPUC)other );
			_mbscpy( (_PUC)((_CPUC)((char *)*this) + (oldSize > 2 ? oldSize - 1 : 0)), (_CPUC)other );
			//*_mbsninc((_CPUC)((char *)*this), selfSize - 1) = '\0';
			((char *)*this)[selfSize - 1] = '\0';
#else
			Realloc( selfSize + (int)strlen(other) + 1 );
			selfSize = (oldSize > 2 ? oldSize : 1) + (int)strlen(other) + 1;
			strcpy( (char *)*this + (oldSize > 2 ? oldSize - 1 : 0), other );
			((char *)*this)[selfSize - 1] = '\0';
#endif
		}
		ComputeCount();
		return (char *)*this;
  }
  
  char *operator += ( const char *other ) {
    return operator + ( other );
  }
  
  void Realloc( int newLen ) {
    if ( newLen > selfLen ) {
      char *p = Relinquish();
      CAPointer<char>::operator = ( new char[newLen << 1] );
      memcpy( (char *)*this, p, selfLen );
      delete [] p;
      selfLen = newLen;
    }  
  }

	char *operator []( int loc ) const { 
		int iC = 0;
		char *pC = (char *)const_cast<CMultiStrA&>(*this);
#if defined(_MBCS)
		while ( pC && _mbsnextc((_CPUC)pC) != '\0' && iC < loc && iC != count ) {
			iC++;
			pC = (_PC)_mbsninc( (_CPUC)pC, _mbslen((_CPUC)pC) ) + 1;
		}
#else
		while ( pC && *pC != '\0' && iC < loc && iC != count ) {
			iC++;
			pC++;
		}
#endif
		return pC;
	}

	int Size() const { return selfSize; }
	int Count() const { return count; }

private :
	void ComputeCount() {
		count = 0;
		char *pC = (char *)*this;
#if defined(_MBCS)
		while ( pC && _mbsnextc((_CPUC)pC) != '\0' ) {
			count++;
			pC = (_PC)_mbsninc( (_CPUC)pC, _mbslen((_CPUC)pC) )  + 1;
		}
#else
		while ( pC && *pC != '\0' ) {
			count++;
			pC++;
		}
#endif
	}

  char *operator ^ ( const char *other ) {
		return 0;
  }
  char *operator ^= ( const char *other ) {
		return 0;
  }
  
};

// ---
class CMSIteratorA {
  const CMultiStrA &array;
				char			*curr;
public:
  CMSIteratorA( const CMultiStrA& arr ) 
    : array( arr ),
      curr( const_cast<CMultiStrA&>(arr) ) {
  }
#if defined(_MBCS)
  operator bool()					const { return _mbsnextc((_CPUC)curr) != '\0'; }
  operator char*()				const { return curr; }
  char* operator ++()           { curr = (_PC)_mbsninc((_CPUC)curr, _mbslen((_CPUC)curr)) + 1; return curr; }
  char* operator ++(int)        { char *tmp = curr; curr = (_PC)_mbsninc((_CPUC)curr, _mbslen((_CPUC)curr)) + 1; return tmp; }
#else
  operator bool()					const { return *curr != '\0'; }
  operator char*()				const { return curr; }
  char* operator ++()						{ curr += strlen(curr) + 1; return curr; }
  char* operator ++(int)				{ char *tmp = curr; curr += strlen(curr) + 1; return tmp; }
#endif
};




#endif
