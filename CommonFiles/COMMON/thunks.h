//==============================================================================
// PRODUCT: GUIDLGS
//==============================================================================
#ifndef __GUIDLGS_THUNKS_H
#define __GUIDLGS_THUNKS_H

#ifndef _M_IX86
#error CCThunk/CCStdThunk is implemented for X86 only!
#endif

#pragma pack(push, 1)

//******************************************************************************
// 
//******************************************************************************
template < class Base, class CallbackFunc = FARPROC >
class CCThunk
{
private:
    BYTE m_mov;              // mov ecx, %pThis
    INT_PTR m_this;          //
    BYTE m_jmp;              // jmp func
    INT_PTR m_relproc;       // relative jmp
    
public:
    typedef void ( Base:: * TMFP ) ();
    
    CCThunk ( TMFP method, const Base * pThis )
    {
        m_mov = 0xB9;
        m_this = ( INT_PTR ) pThis;
        m_jmp = 0xE9;
        m_relproc = * ( INT_PTR * ) ( &method ) - ( INT_PTR ) ( this + 1 );
        
		DWORD dwOldProtection = 0;
		::VirtualProtect (this, sizeof (*this), PAGE_EXECUTE_READWRITE, &dwOldProtection);
        ::FlushInstructionCache ( ::GetCurrentProcess (), this, sizeof ( *this ) );
    }
    
    CallbackFunc GetThunk () const
    {
        return reinterpret_cast < CallbackFunc > ( this );
    }
};

//******************************************************************************
// 
//******************************************************************************
template < class Base, class CallbackFunc = FARPROC >
class CCStdThunk
{
private:
    BYTE m_mov;                   // mov eax, %pThis
    INT_PTR m_this;               //
    DWORD m_xchg_push;            // xchg eax, [esp] : push eax
    BYTE m_jmp;                   // jmp func
    INT_PTR m_relproc;            // relative jmp
    
public:
    typedef void ( __stdcall Base:: * TMFP ) ();
    
    CCStdThunk ( TMFP method, const Base * pThis )
    {
        m_mov = 0xB8;
        m_this = ( INT_PTR ) pThis;
        m_xchg_push = 0x50240487;
        m_jmp = 0xE9;
        m_relproc = * ( INT_PTR * ) ( &method ) - ( INT_PTR ) ( this + 1 );
        
		DWORD dwOldProtection = 0;
		::VirtualProtect (this, sizeof (*this), PAGE_EXECUTE_READWRITE, &dwOldProtection);
        ::FlushInstructionCache ( ::GetCurrentProcess (), this, sizeof ( *this ) );
    }
    
    CallbackFunc GetThunk () const
    {
        return reinterpret_cast < CallbackFunc > ( this );
    }
};

#pragma pack(pop)

#endif // __GUIDLGS_THUNKS_H

//==============================================================================
