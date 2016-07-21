// combool.h: VARIANT_BOOL wrapper
/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2000 Chris Sells
// All rights reserved.
//
// NO WARRANTIES ARE EXTENDED. USE AT YOUR OWN RISK.
//
// To contact the author with suggestions or comments, use csells@sellsbrothers.com.
/////////////////////////////////////////////////////////////////////////////
// CComBool is a class to prevent the misuse of the VARIANT_BOOL type.
// VARIANT_BOOL is a problem because its legal values are -1 and 0 instead of
// 1 and 0, making converting back and forth between bool, BOOL and
// VARIANT_BOOL problematic.
/////////////////////////////////////////////////////////////////////////////
// History:
//  1/17/00 -- Filled out and applied suggestions from Michael Entin [entin@swusa.com].
//  1/16/00 -- First release.
/////////////////////////////////////////////////////////////////////////////
// Usage:
/*
extern HRESULT IEvenChecker::IsEven([in] long n, [out, retval] VARIANT_BOOL* pvb);

HRESULT IsEven(IEvenChecker* pec, VARIANT_BOOL* pvb)
{
    CComBool    b;  // Supports ctors for all bool types
    _ASSERTE(!b);   // Starts as false, supports operator! and operator bool

    pec->IsEven(2, &b); // Supports operator&
    _ASSERTE(b == VARIANT_TRUE);    // Supports operator== and operator !=

    b = false;  // Support operator =
    _ASERERT(b);

    b = VARIANT_BOOL(2);    // Asserts on illegial VARIANT_BOOL values

    return b.CopyTo(pvb);   // Checks for NULL pointer
}
*/

#pragma once
#ifndef INC_COMBOOL
#define INC_COMBOOL

#include "crtdbg.h" // _ASSERTE

class CComBool
{
public:
    CComBool(bool b = false) : m_vb(VariantBool(b)) {}
    CComBool(VARIANT_BOOL vb) : m_vb(vb) { Assert(vb); }
    CComBool(BOOL b) : m_vb(VariantBool(b)) {}
    CComBool(const CComBool& rhs) : m_vb(rhs.m_vb) {}

    operator bool() const   { return m_vb != VARIANT_FALSE; }
    bool operator !() const { return m_vb == VARIANT_FALSE; }

    operator VARIANT_BOOL() const { return m_vb; }
    VARIANT_BOOL* operator &()    { return &m_vb; }

    operator BOOL() const { return m_vb != VARIANT_FALSE; }

    CComBool& operator =(bool b)              { m_vb = VariantBool(b); return *this; }
    CComBool& operator =(VARIANT_BOOL vb)     { Assert(vb); m_vb = vb; return *this; }
    CComBool& operator =(BOOL b)              { m_vb = VariantBool(b); return *this; }
    CComBool& operator =(const CComBool& rhs) { m_vb = rhs.m_vb; return *this; }

    bool operator==(bool b) const              { return VariantBool(b) == m_vb; }
    bool operator==(VARIANT_BOOL vb) const     { Assert(vb); return vb == m_vb; }
    bool operator==(BOOL b) const              { return VariantBool(b) == m_vb; }
    bool operator==(const CComBool& rhs) const { return rhs.m_vb == m_vb; }

    bool operator!=(bool b) const              { return VariantBool(b) != m_vb; }
    bool operator!=(VARIANT_BOOL vb) const     { Assert(vb); return vb != m_vb; }
    bool operator!=(BOOL b) const              { return VariantBool(b) != m_vb; }
    bool operator!=(const CComBool& rhs) const { return rhs.m_vb != m_vb; }

    HRESULT CopyTo(VARIANT_BOOL* pvb) const
    {
        if( !pvb ) return E_POINTER;
        *pvb = m_vb;
        return S_OK;
    }

    static
    VARIANT_BOOL VariantBool(bool b) { return b ? VARIANT_TRUE : VARIANT_FALSE; }

    static
    VARIANT_BOOL VariantBool(BOOL b) { return b ? VARIANT_TRUE : VARIANT_FALSE; }

    static
    void Assert(VARIANT_BOOL vb) { _ASSERTE((vb == VARIANT_TRUE) || (vb == VARIANT_FALSE)); }

private:
    VARIANT_BOOL    m_vb;
};

#endif  // INC_COMBOOL
