#ifndef __KLCSPWD_CONV_H__
#define __KLCSPWD_CONV_H__

#include "std/klcspwd/crypto.h"

#ifdef _WIN32

#include <atlbase.h>

#define KLCSPWD_COUNTOF(_x) (sizeof(_x)/sizeof((_x)[0]))

namespace KLCSPWD
{
    template< int t_nBufferLength = 128 >
    class KLCSPWD_CA2WEX
    {
    public:
	    KLCSPWD_CA2WEX( const char* psz )
            :   m_psz( m_szBuffer )
            ,   m_cbsz(0)
	    {
		    Init( psz);
	    }

	    ~KLCSPWD_CA2WEX() throw()
	    {
		    if( m_psz != m_szBuffer )
		    {
			    Free(m_psz , m_cbsz);
		    };
            Clean(m_szBuffer, t_nBufferLength*sizeof(m_szBuffer[0]));
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
                m_cbsz = nLengthW*sizeof( wchar_t );
			    m_psz = static_cast< wchar_t* >( malloc( m_cbsz ) );
		    }
            if(m_psz)
                AtlA2WHelper(m_psz, psz, nLengthW);
	    }

    public:
	    wchar_t*    m_psz;
        size_t      m_cbsz;
	    wchar_t m_szBuffer[t_nBufferLength];

    private:
	    KLCSPWD_CA2WEX( const KLCSPWD_CA2WEX& ) throw();
	    KLCSPWD_CA2WEX& operator=( const KLCSPWD_CA2WEX& ) throw();
    };
    typedef KLCSPWD_CA2WEX<128> KLCSPWD_CA2W;

    template< int t_nBufferLength = 128 >
    class KLCSPWD_CW2AEX
    {
    public:
	    KLCSPWD_CW2AEX( const wchar_t* psz )
            :   m_psz( m_szBuffer )
            ,   m_cbsz(0)
	    {
		    Init( psz);
	    }

	    ~KLCSPWD_CW2AEX() throw()
	    {
		    if( m_psz != m_szBuffer )
		    {
			    Free( m_psz , m_cbsz);
		    }
            Clean(m_szBuffer, t_nBufferLength*sizeof(m_szBuffer[0]));
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
                m_cbsz = nLengthA*sizeof( char );
			    m_psz = static_cast< char* >( malloc( m_cbsz ) );
		    }
            if(m_psz)
                AtlW2AHelper(m_psz, psz, nLengthA);
	    }

    public:
	    char*       m_psz;
        size_t      m_cbsz;
	    char        m_szBuffer[t_nBufferLength];

    private:
	    KLCSPWD_CW2AEX( const KLCSPWD_CW2AEX& ) throw();
	    KLCSPWD_CW2AEX& operator=( const KLCSPWD_CW2AEX& ) throw();
    };
    typedef KLCSPWD_CW2AEX<> KLCSPWD_CW2A;

    template<class T> 
    class CArrayPointer
    {
    public:	    
	    CArrayPointer()
            :   P(0)
            ,   N(0)
        {;}
	    virtual ~CArrayPointer()
        {
            Free(P, N);
        };
        T* Allocate(size_t nItems)
        {
            Free(P, N);
            N = nItems * sizeof(T);
            P = (T*)malloc(N);
            return P;
        };
	    T&   operator  *() {return *P;}
		     operator T*() {return P;}
		     operator const T*() const {return P;}
	    int  operator  !() const {return P == 0;}
	    T*   Relinquish() {T* p = P; P = 0; return p;}
    protected:
	    T*      P;
        size_t  N;

    private:
	    void* operator new(size_t) { return 0;}  // prohibit use of new
    };

};

#endif

#endif //__KLCSPWD_CONV_H__
