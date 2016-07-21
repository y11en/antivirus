#ifndef __CREG_H__
#define __CREG_H__

#include "ireg.h"
#include <vector>

namespace AVPUpgradeSettings
{

    class CRegValue;
    class CRegValueIterator;
    class CRegKey;
    class CRegKeyIterator;

    template<class RegBase>
    class CRegBase
    {
    protected:
        RegBase* m_regptr;

        CRegBase<RegBase>(RegBase* ptr) 
        :   m_regptr(ptr) 
        {
        }

    public:

        CRegBase<RegBase>(const CRegBase<RegBase>& that) 
        :   m_regptr(that.m_regptr) 
        {
            m_regptr->addref();
        }

        ~CRegBase<RegBase>() 
        { 
            if (m_regptr) 
                m_regptr->release(); 
        }

        CRegBase<RegBase>& operator=(const CRegBase<RegBase>& that)
        {
            if (m_regptr) 
                m_regptr->release();
            m_regptr = that.m_regptr;
            m_regptr->addref();
            return *this;
        }

        bool isvalid() { return m_regptr != 0; }
    };

    class CRegValue : public CRegBase<IRegValue>
    {
        typedef CRegBase<IRegValue> TBase;
        friend class CRegValueIterator;
        friend class CRegKey;

        CRegValue(IRegValue* ptr) : TBase(ptr) {}

    public:
        CRegValue(const CRegValue& that) : TBase(that) {}
        CRegValue& operator=(const CRegValue& that) { return operator=(that), *this; }

        operator bool() { return isvalid(); }

        std::wstring getname() { return m_regptr->getname(); }
        size_t getsize () { return m_regptr->getsize(); }
        ERegType gettype () { return m_regptr->gettype(); }

        bool get(void *buffer, size_t& size) { return m_regptr->get(buffer, size); }
        bool set(void *buffer, size_t  size) { return m_regptr->set(buffer, size); }

        std::wstring getstring()
        {
            size_t size = getsize();
            if (size)
            {
                std::wstring value;
                value.resize(size/sizeof(wchar_t));
                if (get(&value[0], size))
                {
                    return value;
                }
            }
            return std::wstring(L"");
        }

        unsigned int getdword()
        {
            unsigned int value = 0;
            size_t size = sizeof(value);
            get(&value, size);
            return value;
        }

        unsigned __int64 getqword()
        {
            unsigned __int64 value = 0;
            size_t size = sizeof(value);
            get(&value, size);
            return value;
        }

        bool copyto(CRegKey& tokey);
        bool copyto(CRegKey& tokey, const wchar_t*value);

    };

    class CRegValueIterator : public CRegBase<IRegValueIterator>
    {
        typedef CRegBase<IRegValueIterator> TBase;
        friend class CRegKey;

        CRegValueIterator(IRegValueIterator* ptr) : TBase(ptr) {}

    public:
        CRegValueIterator(const CRegValueIterator& that) : TBase(that) {}
        CRegValueIterator& operator=(const CRegValueIterator& that) { return TBase::operator=(that), *this; }

        operator bool() { return m_regptr->isvalid(); }
        void operator++() { m_regptr->next(); }

        std::wstring getname() { return m_regptr->getname(); }
        CRegValue get() { return CRegValue(m_regptr->get()); }
    };

    class CRegKey : public CRegBase<IRegKey>
    {
        typedef CRegBase<IRegKey> TBase;
        friend class CRegKeyIterator;


    public:
        CRegKey(IRegKey* ptr) : TBase(ptr) {}
        CRegKey(const CRegKey& that) : TBase(that) {}
        CRegKey& operator=(const CRegKey& that) { return TBase::operator=(that), *this; }

        operator bool() { return isvalid(); }

        std::wstring getname() { return m_regptr->getname(); }

        CRegKey getkey(const wchar_t*key) { return CRegKey(m_regptr->getkey(key)); }
        CRegKey createkey(const wchar_t *key) { return CRegKey(m_regptr->createkey(key)); }

        CRegValue getvalue(const wchar_t *value) { return CRegValue(m_regptr->getvalue(value)); }
        CRegValue setvalue(const wchar_t *value, ERegType type, void *buffer, size_t size) { return CRegValue(m_regptr->setvalue(value, type, buffer, size)); }

        CRegKeyIterator enumkeys();
        CRegValueIterator enumvalues();

        bool copyto(CRegKey& tokey);
    };

    class CRegKeyIterator : public CRegBase<IRegKeyIterator>
    {
        typedef CRegBase<IRegKeyIterator> TBase;
        friend class CRegKey;

        CRegKeyIterator(IRegKeyIterator* ptr) : TBase(ptr) {}

    public:
        CRegKeyIterator(const CRegKeyIterator& that) : TBase(that) {}
        CRegKeyIterator& operator=(const CRegKeyIterator& that) { return TBase::operator=(that), *this; }

        operator bool() { return m_regptr->isvalid(); }
        void operator++() { m_regptr->next(); }

        std::wstring getname() { return m_regptr->getname(); }
        CRegKey get() { return CRegKey(m_regptr->get()); }
    };

};

inline AVPUpgradeSettings::CRegKeyIterator AVPUpgradeSettings::CRegKey::enumkeys() 
{ 
    return CRegKeyIterator(m_regptr->enumkeys()); 
}

inline AVPUpgradeSettings::CRegValueIterator AVPUpgradeSettings::CRegKey::enumvalues() 
{ 
    return CRegValueIterator(m_regptr->enumvalues()); 
}

inline bool AVPUpgradeSettings::CRegValue::copyto(CRegKey& tokey)
{
    size_t size = getsize();
    std::vector<unsigned char> buffer(size);
    if (get(&buffer[0], size))
        return tokey.setvalue(getname().c_str(), gettype(), &buffer[0], size);
    else
        return false;
}

inline bool AVPUpgradeSettings::CRegValue::copyto(CRegKey& tokey, const wchar_t*value)
{
    size_t size = getsize();
    std::vector<unsigned char> buffer(size);
    if (get(&buffer[0], size))
        return tokey.setvalue(value, gettype(), &buffer[0], size);
    else
        return false;
}

inline bool AVPUpgradeSettings::CRegKey::copyto(CRegKey& tokey)
{
    for (CRegValueIterator i = enumvalues(); i; ++i)
    {
        if (!i.get().copyto(tokey))
            return false;
    }

    for (CRegKeyIterator i = enumkeys(); i; ++i)
    {
        if (!i.get().copyto(tokey.createkey(i.getname().c_str())))
            return false;
    }
    return true;
}


#endif //__CREG_H__ 