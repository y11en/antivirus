#include <time.h>
#include <malloc.h>
#include <tchar.h>
#include <io.h>
#include <StuffIO\Utils.h>

#if defined(_UNICODE) || defined(UNICODE)
#define TCSCLEN(p) (p ? _tcslen(p) : 0)
#else
#define TCSCLEN(p) (p ? strlen(p) : 0)
#endif


#if !defined(_countof)
	#define _countof(array) (sizeof(array)/sizeof(array[0]))
#endif

// ---
bool IsDlgClassName( HWND win ) {
	if ( win ) {
		TCHAR buf[256];
		::GetClassName( win, buf, _countof(buf) );
		return !_tcsicmp( buf, _T("#32770") );
	}
	return false;
}


// ---
int SizeValueInDecChars( DWORD nValue ) {
	int nSizeOfDWord = 10;
	int nDigBase     = 1000000000;
	while( !(nValue / nDigBase) ) {
		nDigBase /= 10;
		nSizeOfDWord--;
	}
	return nSizeOfDWord;
}


// ---
UINT GetMinUnusedValue( CSArray<UINT> &rcIdArray ) {
	UINT nResId = 1;
	if ( rcIdArray.Count() ) {
		for ( UINT c=UINT(rcIdArray.Count()); nResId<=c; nResId++ ) {
			bool bFound = false;
			for ( UINT i=0; i<c; i++ ) {
				if ( rcIdArray[i] == nResId ) {
					bFound = true;
					break;
				}
			}
			if ( !bFound )
				break;
		}
	}
	return nResId;
}

// ---
CSArray<UINT> uniqueIdArray;

// ---
UINT CreateUniqueId() {
	if ( uniqueIdArray.Count() ) {
		for ( UINT i=0,c=UINT(uniqueIdArray.Count()); i<c; i++ ) {
			if ( uniqueIdArray[i] != (i + 1) ) {
				uniqueIdArray.InsertInd( i, (i + 1) );
				return i + 1;
			}
		}
	}
	
	UINT res = uniqueIdArray.Count() + 1;
	uniqueIdArray.Add( res );

	return res;
}


// ---
void DestroyUniqueId( UINT id ) {
	for ( UINT i=0; i<UINT(uniqueIdArray.Count()); i++ ) {
		if ( uniqueIdArray[i] == id ) {
			uniqueIdArray.RemoveInd( i );
			break;
		}
	}
}



// ---
int GetStandartComboHeight( HFONT hFont, HINSTANCE hInstance ) {
  HWND hWnd = ::CreateWindow(//Ex( 0,
                               _T("COMBOBOX"),
                               _T(""),
                               WS_CHILD | CBS_DROPDOWN, //0x40230142l,
                               0, 0, 100, 100,
                               ::GetDesktopWindow(),
                               0,
                               hInstance,
                               0);
  ::SendMessage( hWnd, WM_SETFONT, WPARAM(hFont), 0 );
  RECT rcWndRect;
  ::GetWindowRect( hWnd, &rcWndRect );
  int nHeight = rcWndRect.bottom - rcWndRect.top;
  ::DestroyWindow( hWnd );

  return nHeight;
}

// ---
static bool ProcessName( TCHAR *pBegin, TCHAR *pEnd ) {
  TCHAR *pCurr = pEnd;
  while ( pCurr && _tcsnextc(pCurr) == L'z' && pCurr >= pBegin )
    pCurr = _tcsdec(pBegin, pCurr);
	if ( !pCurr )
		pCurr = pBegin;
  if ( pCurr >= pBegin ) {
    if ( _tcsnextc(pCurr) == L'.' )
			pCurr = _tcsdec(pBegin, pCurr);
		*pCurr = _tcsnextc(pCurr) + 1;
		pCurr = _tcsinc(pCurr);
    while( pCurr <= pEnd ) {
      if ( _tcsnextc(pCurr) != L'.' )
        *pCurr = L'a';
			pCurr = _tcsinc(pCurr);
		}
    return true;
  }
  else
    return false;
}


// формат шаблона - [path]\[]XXXXXX.ext
//            или - [path]\[]XXXXXX
// ---
TCHAR *GetUniqueFileName( const TCHAR *pszPath, const TCHAR *pszTemplate ) {

	static TCHAR pBuff[_MAX_PATH << 1];

  _tcsncpy( pBuff, pszPath, _countof(pBuff) );
  if ( _tcsnextc(_tcsninc(pBuff, _tcsclen(pBuff)-1)) != L'\\' )
    _tcscat( pBuff, _T("\\") );
  _tcscat( pBuff, pszTemplate );

  TCHAR *pSource = pBuff;
  int  nDotPos  = 0;

// выходим на конец строки
  pSource = _tcsninc( pSource, _tcsclen( pSource ) - 1 );

// ищем начало имени файла в строке
  while ( pSource && pSource != pBuff && _tcsnextc(_tcsdec(pBuff,pSource)) != L'\\' ) {
    if ( _tcsnextc(pSource) == L'.' )
      nDotPos++;
    pSource = _tcsdec( pBuff, pSource );
  }

	if ( !pSource )
		pSource = pBuff;

// ищем начало шаблона в имени файла
  while ( _tcsnextc(pSource) != L'\0' && !(_tcsnextc(pSource) == L'X' || _tcsnextc(pSource) == L'x') )
    pSource = _tcsinc(pSource);

  if ( *pSource ) {
// шаблон найден
    TCHAR *pBegin = pSource;
    int  i    = 0;

// инициируем систему случайных чисел младшими разр€дами времени
    srand( (unsigned int)time(0) );

// замен€ем весь шаблон на байты сформированные из случайных чисел
		while ( _tcsnextc(pSource) != L'\0' && (_tcsnextc(pSource) == L'X' || _tcsnextc(pSource) == L'x') ) {
      *pSource = (TCHAR)((rand() % 26) + L'a');
			pSource = _tcsinc(pSource);
			i++;
    }

    if (i < 6 )
// шаилон меньше 6 символов
      return NULL;

    pSource--;
    if ( !nDotPos )
// шаблон не содержал '.'
      *_tcsninc(pSource, - 3) = L'.';

// пока есть файл с таким именем на диске
    while ( !_taccess(pBuff, 0) ) {
      if ( !::ProcessName(pBegin, pSource) )
// шаблон исчерпан
        return NULL;
    }

    pSource = new TCHAR[TCSCLEN(pBuff) + 1];
		_tcscpy( pSource, pBuff);

    return pSource;
  }
  return NULL;
}

// "ќткачать" очередь сообщений
void PumpWaitingMessages( HWND hWndOwner, UINT wMsgFilterMin, UINT wMsgFilterMax ) {
  MSG msg;
	while ( ::PeekMessage(&msg, hWndOwner, wMsgFilterMin, wMsgFilterMax, PM_REMOVE) ) {
		::TranslateMessage( &msg );
		::DispatchMessage( &msg );
	}
}

