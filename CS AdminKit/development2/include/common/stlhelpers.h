#ifndef __KLSTLHELPERS_H__
#define __KLSTLHELPERS_H__

namespace KLSTD
{
    class CStringWrapper
    {
    public:
        CStringWrapper(const wchar_t* x)
            :   m_szwX(x?x:L"")
        {;};
        CStringWrapper(const CStringWrapper& x)
            :   m_szwX(x.m_szwX)
        {;};
        bool operator < (const CStringWrapper& x) const 
        {
            return wcscmp(m_szwX , x.m_szwX) < 0;
        };
        bool operator < (const wchar_t* x) const 
        {
            return wcscmp(m_szwX , x) < 0;
        };
        const wchar_t* c_str() const
        {
            return m_szwX;
        };
    protected:
        const wchar_t* m_szwX;
    };
};

#endif //__KLSTLHELPERS_H__