#ifndef __IREGREG_H_
#define __IREGREG_H_

#include "ireg.h"
#include <windows.h>

#pragma warning(disable: 4250)

namespace AVPUpgradeSettings
{

    class CRegRegBase : virtual public IRegBase
    {
        int m_refcnt;
    public:

        CRegRegBase() : m_refcnt(1) {}
        virtual ~CRegRegBase() {}

        virtual void addref() { ++m_refcnt; }
        virtual void release();
    };

    class CRegRegKey;
    class CRegRegValue : public IRegValue, public CRegRegBase
    {
        std::wstring m_name;
        CRegRegKey * m_pkey;

        ERegType     m_type;
        size_t       m_size;
    public:

        CRegRegValue(CRegRegKey * pkey, const wchar_t *name, ERegType type, size_t size);
        virtual ~CRegRegValue ();

        virtual std::wstring getname () { return m_name; }
        virtual size_t       getsize () { return m_size; }
        virtual ERegType     gettype () { return m_type; }

        virtual bool get(void *buffer, size_t& size);
        virtual bool set(void *buffer, size_t  size);

        static ERegType dword2eregtype(DWORD type);
        static DWORD    eregtype2dword(ERegType type);
    };

    class CRegRegValueIterator : public IRegValueIterator, public CRegRegBase
    {
        CRegRegKey * m_pkey;
        int          m_index;
        bool         m_valid;
        std::wstring m_name;

        void getstate();

    public:
        CRegRegValueIterator(CRegRegKey *pkey);
        virtual ~CRegRegValueIterator();

        virtual void next();
        virtual bool isvalid() { return m_valid; }

        virtual std::wstring getname() { return m_name; }
        virtual IRegValue*   get();
    };

    class CRegRegKey : public IRegKey, public CRegRegBase
    {
        HKEY m_hkey;
        std::wstring m_name;

    public:

        CRegRegKey (HKEY hkey, const wchar_t *name);
        virtual ~CRegRegKey ();

        virtual std::wstring getname () { return m_name; }

        virtual IRegKeyIterator* enumkeys();
        virtual IRegValueIterator* enumvalues();

        virtual IRegKey *getkey(const wchar_t *key);
        virtual IRegKey *createkey(const wchar_t *key);

        virtual IRegValue *getvalue(const wchar_t *value);
        virtual IRegValue *setvalue(const wchar_t *value, ERegType type, void *buffer, size_t size);

        HKEY    gethkey() { return m_hkey; }
    };

    class CRegRegKeyIterator : public IRegKeyIterator, public CRegRegBase
    {
        CRegRegKey * m_pkey;
        int          m_index;
        bool         m_valid;
        std::wstring m_name;

        void getstate();

    public:

        CRegRegKeyIterator(CRegRegKey *pkey);
        virtual ~CRegRegKeyIterator ();

        virtual void next();
        virtual bool isvalid() { return m_valid; }

        virtual std::wstring getname() { return m_name; }
        virtual IRegKey* get();
    };

    class CRegRegRoot
    {
    public:

        IRegKey* getkey(HKEY hkey, const wchar_t * subkey);
    };
};

#endif //__IREGREG_H_