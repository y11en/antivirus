/*!
 * (C) 2002 "Kaspersky Lab"
 *
 * \file klconv.h
 * \author Андрей Казачков
 * \date 2002
 * \brief Набор макросов для конвертирования строк wchar_t*, char, TCHAR*
 *
 */

/*KLCSAK_PUBLIC_HEADER*/

#ifndef KLCONV_H_3D201985_327E_4549_A4D3_5ED76D549617
#define KLCONV_H_3D201985_327E_4549_A4D3_5ED76D549617

#include <algorithm>
#include <stdlib.h>
#include <stddef.h>

#include "std/base/klbase.h"
#include "std/err/klerrors.h"

#ifdef N_PLAT_NLM
# include <nwmalloc.h> // alloca
#endif


/*!
  \brief	Конвертирует мультибайтную строку в юникодную. Если pBuffer и nBuffer равны 0,
			то возвращает требующийся размер буфера (включая завершающий ноль).

  \param	pBuffer [in/out] указатель на буфер, в который будет записан результат
  \param	szaSrc [in] завершённая нулём строка, подлежащая преобразованию
  \param	nBuffer [in] длина буфера pBuffer (в широких символах)
  \return	количество записанных символов
*/
KLCSC_DECL size_t KLSTD_A2WHelper(wchar_t* pBuffer, const char* szaSrc, int nBuffer);


/*! /brief	Конвертирует юникодную строку в мультибайтную.  Если pBuffer и nBuffer равны 0,
			то возвращает требующийся размер буфера (включая завершающий ноль).

  \param	pBuffer [in/out] указатель на буфер, в который будет записан результат
  \param	szaSrc [in] завершённая нулём строка, подлежащая преобразованию
  \param	nBuffer [in] длина буфера pBuffer (в узких символах)
  \return	количество записанных символов
*/
KLCSC_DECL size_t KLSTD_W2AHelper(char* pBuffer, const wchar_t* szwSrc, int nBuffer);

/*! /brief	Макрос KLSTD_USES_CONVERSION определяет переменные, необходимые для конвертирования
			должен присутствовать в начале функции,использующей макросы конвертирования.
*/
#if 1 // def _WIN32
#define KLSTD_USES_CONVERSION		\
	int _convert = 0;		\
	_convert;				\
	const wchar_t* _lpw = NULL;	\
	_lpw;					\
	const char* _lpa = NULL;		\
	_lpa
#else
#define KLSTD_USES_CONVERSION
#endif


/*! /brief	Преобразование юникодной строки в мультибайтную строку
*/
#if 1 // def _WIN32
#define KLSTD_W2A(lpw)														\
		(lpw?	(	_lpw=lpw,											\
					_convert=KLSTD_W2AHelper(NULL, _lpw, 0),					\
					_lpa=(char*)alloca((_convert+1)*sizeof(char)),	\
					_convert=KLSTD_W2AHelper((char*)_lpa, _lpw, _convert),			\
					(char*)_lpa												\
				):NULL													\
		)
#else
#define KLSTD_W2A(lpw) _KLSTD_W2A(lpw)

inline char *_KLSTD_W2A( const wchar_t *lpw )
{
  if ( lpw == 0 ) {
    return 0;
  }
  _conv _c;
  std::string s = _c.narrow( lpw );
  char *ss = new char [s.length() + 1];
  std::copy(s.begin(), s.end(), ss );
  ss[s.length()] = 0;
  return ss;
}
#endif // 0

/*! /brief	Преобразование мультибайтной строки в юникодную строку
*/
#if 1 // def _WIN32
#define KLSTD_A2W(lpa)														\
		(lpa?	(	_lpa=lpa,											\
					_convert=KLSTD_A2WHelper(NULL, _lpa, 0),					\
					_lpw=(wchar_t*)alloca((_convert+1)*sizeof(wchar_t)),	\
					_convert=KLSTD_A2WHelper((wchar_t*)_lpw, _lpa, _convert),			\
					(wchar_t*)_lpw												\
				):NULL													\
		)
#else
#define KLSTD_A2W(lpa) _KLSTD_A2W(lpa)

inline wchar_t *_KLSTD_A2W( const char * lpa)
{
  if ( lpa == 0 ) {
    return 0;
  }
  _conv _c;
  std::wstring s = _c.widen( lpa );
  wchar_t *ss = new wchar_t[s.length() + 1];
  std::copy(s.begin(), s.end(), ss );
  ss[s.length()] = 0;
  return ss;
}
#endif

/*! /brief	Преобразование константной мультибайтной строки в константную юникодную строку
*/
#define KLSTD_A2CW(lpa) ((const wchar_t*)KLSTD_A2W(lpa))

/*! /brief	Преобразование константной юникодной строки в константную мультибайтную строку
*/
#define KLSTD_W2CA(lpw) ((const char*)KLSTD_W2A(lpw))


/*!
	/brief	Далее определены макросы преобразующие TCHAR в wchar_t, char и обратно.
*/

#if defined(_UNICODE) || defined(UNICODE)
	#define KLSTD_T2A		KLSTD_W2A
	#define KLSTD_A2T		KLSTD_A2W
	inline wchar_t*	KLSTD_T2W(wchar_t* lp) { return lp; }
	inline wchar_t*	KLSTD_W2T(wchar_t* lp) { return lp; }
	#define KLSTD_T2CA		KLSTD_W2CA
	#define KLSTD_A2CT		KLSTD_A2CW
	inline const wchar_t* KLSTD_T2CW(const wchar_t* lp) { return lp; }
	inline const wchar_t* KLSTD_W2CT(const wchar_t* lp) { return lp; }
#else
	#define KLSTD_T2W		KLSTD_A2W
	#define KLSTD_W2T		KLSTD_W2A
	inline char* KLSTD_T2A(char* lp) { return lp; }
	inline char* KLSTD_A2T(char* lp) { return lp; }
	#define KLSTD_T2CW	KLSTD_A2CW
	#define KLSTD_W2CT	KLSTD_W2CA
	inline const char* KLSTD_T2CA(const char* lp) { return lp; }
	inline const char* KLSTD_A2CT(const char* lp) { return lp; }
#endif

template< int t_nBufferLength = 128 >
class KLSTD_CW2WEX
{
public:
	KLSTD_CW2WEX( const wchar_t* psz ):
		m_psz( m_szBuffer )
	{
		Init( psz );
	}
	~KLSTD_CW2WEX() throw()
	{
		if( m_psz != m_szBuffer )
		{
			free( m_psz );
		}
	}

	operator wchar_t*() const throw()
	{
		return( m_psz );
	}

private:
	void Init( const wchar_t* psz )
	{
		if (psz == NULL)
		{
			m_psz = NULL;
			return;
		}
		int nLength = wcslen( psz )+1;
		if( nLength > t_nBufferLength )
		{
			m_psz = static_cast< wchar_t* >( malloc( nLength*sizeof( wchar_t ) ) );
            KLSTD_CHKMEM(m_psz);
		}
		memcpy( m_psz, psz, nLength*sizeof( wchar_t ) );
	}

public:
	wchar_t* m_psz;
	wchar_t m_szBuffer[t_nBufferLength];

private:
	KLSTD_CW2WEX( const KLSTD_CW2WEX& ) throw();
	KLSTD_CW2WEX& operator=( const KLSTD_CW2WEX& ) throw();
};
typedef KLSTD_CW2WEX<> KLSTD_CW2W;


template< int t_nBufferLength = 128 >
class KLSTD_CA2AEX
{
public:
	KLSTD_CA2AEX( const char* psz ):
		m_psz( m_szBuffer )
	{
		Init( psz );
	}
	~KLSTD_CA2AEX() throw()
	{
		if( m_psz != m_szBuffer )
		{
			free( m_psz );
		}
	}

	operator char*() const throw()
	{
		return( m_psz );
	}

private:
	void Init( const char* psz )
	{
		if (psz == NULL)
		{
			m_psz = NULL;
			return;
		}
		int nLength = strlen( psz ) + 1;
		if( nLength > t_nBufferLength )
		{
			m_psz = static_cast< char* >( malloc( nLength*sizeof( char ) ) );
            KLSTD_CHKMEM(m_psz);
		}
		memcpy( m_psz, psz, nLength*sizeof( char ) );
	}
public:
	char* m_psz;
	char m_szBuffer[t_nBufferLength];

private:
	KLSTD_CA2AEX( const KLSTD_CA2AEX& ) throw();
	KLSTD_CA2AEX& operator=( const KLSTD_CA2AEX& ) throw();
};
typedef KLSTD_CA2AEX<> KLSTD_CA2A;


template< int t_nBufferLength = 128 >
class KLSTD_CA2CAEX
{
public:
	KLSTD_CA2CAEX( const char* psz ):
		m_psz( psz )
	{
	}
	~KLSTD_CA2CAEX() throw()
	{
	}

	operator const char*() const throw()
	{
		return( m_psz );
	}

public:
	const char* m_psz;

private:
	KLSTD_CA2CAEX( const KLSTD_CA2CAEX& ) throw();
	KLSTD_CA2CAEX& operator=( const KLSTD_CA2CAEX& ) throw();
};
typedef KLSTD_CA2CAEX<> KLSTD_CA2CA;


template< int t_nBufferLength = 128 >
class KLSTD_CW2CWEX
{
public:
	KLSTD_CW2CWEX( const wchar_t* psz ):
		m_psz( psz )
	{
	}
	~KLSTD_CW2CWEX() throw()
	{
	}

	operator const wchar_t*() const throw()
	{
		return( m_psz );
	}

public:
	const wchar_t* m_psz;

private:
	KLSTD_CW2CWEX( const KLSTD_CW2CWEX& ) throw();
	KLSTD_CW2CWEX& operator=( const KLSTD_CW2CWEX& ) throw();
};
typedef KLSTD_CW2CWEX<> KLSTD_CW2CW;


template< int t_nBufferLength = 128 >
class KLSTD_CA2WEX
{
public:
	KLSTD_CA2WEX( const char* psz ):
		m_psz( m_szBuffer )
	{
		Init( psz);
	}

	~KLSTD_CA2WEX() throw()
	{
		if( m_psz != m_szBuffer )
		{
			free( m_psz );
		}
	}

	operator wchar_t*() const throw()
	{
		return( m_psz );
	}

private:
	void Init( const char* psz)
	{
		if (psz == NULL)
		{
			m_psz = NULL;
			return;
		}
		int nLengthA = strlen( psz )+1;
		int nLengthW = nLengthA;

		if( nLengthW > t_nBufferLength )
		{
			m_psz = static_cast< wchar_t* >( malloc( nLengthW*sizeof( wchar_t ) ) );
            KLSTD_CHKMEM(m_psz);
		}
        KLSTD_A2WHelper(m_psz, psz, nLengthW);
	}

public:
	wchar_t* m_psz;
	wchar_t m_szBuffer[t_nBufferLength];

private:
	KLSTD_CA2WEX( const KLSTD_CA2WEX& ) throw();
	KLSTD_CA2WEX& operator=( const KLSTD_CA2WEX& ) throw();
};
typedef KLSTD_CA2WEX<128> KLSTD_CA2W;

template< int t_nBufferLength = 128 >
class KLSTD_CW2AEX
{
public:
	KLSTD_CW2AEX( const wchar_t* psz ):
		m_psz( m_szBuffer )
	{
		Init( psz);
	}

	~KLSTD_CW2AEX() throw()
	{
		if( m_psz != m_szBuffer )
		{
			free( m_psz );
		}
	}

	operator char*() const throw()
	{
		return( m_psz );
	}

private:
	void Init( const wchar_t* psz)
	{
		if (psz == NULL)
		{
			m_psz = NULL;
			return;
		}
		int nLengthW = wcslen( psz )+1;
		int nLengthA = nLengthW*2;

		if( nLengthA > t_nBufferLength )
		{
			m_psz = static_cast< char* >( malloc( nLengthA*sizeof( char ) ) );
            KLSTD_CHKMEM(m_psz);
		}
        KLSTD_W2AHelper(m_psz, psz, nLengthA);
	}

public:
	char* m_psz;
	char m_szBuffer[t_nBufferLength];

private:
	KLSTD_CW2AEX( const KLSTD_CW2AEX& ) throw();
	KLSTD_CW2AEX& operator=( const KLSTD_CW2AEX& ) throw();
};
typedef KLSTD_CW2AEX<> KLSTD_CW2A;


/*
    Макросы конвертирования.
    Примеры использования.
    
    Пример 1.
    void foo(const char* szX);
    ..........................        
    void foo1(const wchar_t* szwX)
    {
        KLSTD_W2CA2 astrX(szwX);
        foo(astrX);
    };
    
    Пример 2.
    void foo(const char* szX);
    ..........................        
    void foo1(const wchar_t* szwX)
    {
        foo(KLSTD_W2CA2(szwX));
    };

    Пример неправильного использования.
    void foo(const char* szX);
    ..........................        
    void foo1(const wchar_t* szwX)
    {
        const char* szX=KLSTD_W2CA2(szwX);
        foo(szX);
    };       
*/
   
    #define KLSTD_W2CA2 KLSTD_CW2A
    #define KLSTD_W2A2 KLSTD_CW2A
    #define KLSTD_A2CW2 KLSTD_CA2W
    #define KLSTD_A2W2 KLSTD_CA2W

#ifdef _UNICODE

	#define KLSTD_W2T2     KLSTD_CW2W
	#define KLSTD_W2CT2    KLSTD_CW2CW
	#define KLSTD_T2W2     KLSTD_CW2W
	#define KLSTD_T2CW2    KLSTD_CW2CW
	#define KLSTD_A2T2     KLSTD_CA2W
	#define KLSTD_A2CT2    KLSTD_CA2W
	#define KLSTD_T2A2     KLSTD_CW2A
	#define KLSTD_T2CA2    KLSTD_CW2A    

#else  // !_UNICODE

	#define KLSTD_W2T2     KLSTD_CW2A
	#define KLSTD_W2CT2    KLSTD_CW2A
	#define KLSTD_T2W2     KLSTD_CA2W
	#define KLSTD_T2CW2    KLSTD_CA2W
	#define KLSTD_A2T2     KLSTD_CA2A
	#define KLSTD_A2CT2    KLSTD_CA2CA
	#define KLSTD_T2A2     KLSTD_CA2A
	#define KLSTD_T2CA2    KLSTD_CA2CA

#endif  // !_UNICODE

#endif // KLCONV_H_3D201985_327E_4549_A4D3_5ED76D549617
