#include "cregreg.h"
#include <windows.h>

using namespace AVPUpgradeSettings;

// --- CRegRegBase

void CRegRegBase::release()
{
    if (--m_refcnt == 0)
    {
        delete this;
    }
}

// --- CRegRegValue

CRegRegValue::CRegRegValue(CRegRegKey * pkey, const wchar_t *name, ERegType type, size_t size)
:   m_pkey(pkey),
    m_name(name),
    m_type(type),
    m_size(size)
{
    m_pkey->addref();
}

CRegRegValue::~CRegRegValue ()
{
    m_pkey->release();
}

bool CRegRegValue::get(void *buffer, size_t& size)
{
    DWORD cbdata = DWORD(size);
    DWORD err = RegQueryValueExW(m_pkey->gethkey(), m_name.c_str(), 0, 0, (LPBYTE)buffer, &cbdata);
    if (err == ERROR_SUCCESS)
    {
        size = cbdata;
        return true;
    }
    else
    {
        size = cbdata;
        return false;
    }
}

bool CRegRegValue::set(void *buffer, size_t size)
{
    DWORD cbdata = DWORD(size);
    DWORD err = RegSetValueExW(m_pkey->gethkey(), m_name.c_str(), 0, m_type, (LPBYTE)buffer, cbdata);
    if (err == ERROR_SUCCESS)
    {
        m_size = size;
        return true;
    }
    else
    {
        return false;
    }
}

ERegType CRegRegValue::dword2eregtype(DWORD type)
{
    switch(type)
    {
    case REG_BINARY:    return ERegTypeBinary;
    case REG_DWORD:     return ERegTypeDword;
    case REG_QWORD:     return ERegTypeQword;
    case REG_EXPAND_SZ: return ERegTypeExpandString;
    case REG_SZ:        return ERegTypeString;
    default:
                        return ERegTypeNone;
    }
}

DWORD CRegRegValue::eregtype2dword(ERegType type)
{
    switch(type)
    {
    case ERegTypeNone:          return REG_NONE;
    case ERegTypeBinary:        return REG_BINARY;
    case ERegTypeDword:         return REG_DWORD;
    case ERegTypeQword:         return REG_QWORD;
    case ERegTypeExpandString:  return REG_EXPAND_SZ;
    case ERegTypeString:        return REG_SZ;
    default:                    return  REG_NONE;
    }
}


// --- CRegRegValueIterator

CRegRegValueIterator::CRegRegValueIterator(CRegRegKey *pkey)
:   m_pkey(pkey),
    m_index(0),
    m_valid(true)
{
    m_pkey->addref();

    getstate ();
}

CRegRegValueIterator::~CRegRegValueIterator()
{
    m_pkey->release();
}

void CRegRegValueIterator::getstate()
{
    wchar_t name[8192];
    DWORD namesize = sizeof(name);
    DWORD err = RegEnumValueW(m_pkey->gethkey(), m_index, name, &namesize, 0, 0, 0, 0);
    if (err == ERROR_SUCCESS)
    {
        m_valid = true;
        m_name = name;
    }
    else
    {
        m_valid = false;
    }
}

void CRegRegValueIterator::next()
{
    ++m_index;
    getstate();
}

IRegValue* CRegRegValueIterator::get()
{
    return m_pkey->getvalue(m_name.c_str());
}


// --- CRegRegKey

CRegRegKey::CRegRegKey (HKEY hkey, const wchar_t *name)
:   m_hkey(hkey),
    m_name(name)
{
}

CRegRegKey::~CRegRegKey ()
{
    RegCloseKey(m_hkey);
}

IRegKeyIterator* CRegRegKey::enumkeys()
{
    return new CRegRegKeyIterator(this);
}

IRegValueIterator* CRegRegKey::enumvalues()
{
    return new CRegRegValueIterator(this);
}

IRegKey *CRegRegKey::getkey(const wchar_t *key)
{
    HKEY hreskey;
    DWORD err = RegOpenKeyExW(m_hkey, key, 0, KEY_ALL_ACCESS, &hreskey);
    if (err == ERROR_SUCCESS)
        return new CRegRegKey(hreskey, key);
    else
        return 0;
}

IRegKey *CRegRegKey::createkey(const wchar_t *key)
{
    HKEY hreskey;
    DWORD err = RegCreateKeyExW(m_hkey, key, 0, 0, 0, KEY_ALL_ACCESS, 0, &hreskey, 0);
    if (err == ERROR_SUCCESS)
        return new CRegRegKey(hreskey, key);
    else
        return 0;
}

IRegValue *CRegRegKey::getvalue(const wchar_t *value)
{
    DWORD type;
    DWORD cbdata;
    DWORD err = RegQueryValueExW(m_hkey, value, 0, &type, 0, &cbdata);
    if (err == ERROR_SUCCESS)
        return new CRegRegValue(this, value, CRegRegValue::dword2eregtype(type), cbdata);
    else
        return 0;
}

IRegValue *CRegRegKey::setvalue(const wchar_t *value, ERegType type, void *buffer, size_t size)
{
    DWORD cbdata = DWORD(size);
    DWORD err = RegSetValueExW(m_hkey, value, 0, CRegRegValue::eregtype2dword(type), (LPBYTE)buffer, cbdata);
    if (err == ERROR_SUCCESS)
        return new CRegRegValue(this, value, type, size);
    else
        return 0;
}

// --- CRegRegKeyIterator

CRegRegKeyIterator::CRegRegKeyIterator(CRegRegKey *pkey)
:   m_pkey(pkey),
    m_index(0),
    m_valid(true)
{
    m_pkey->addref();

    getstate();
}

CRegRegKeyIterator::~CRegRegKeyIterator ()
{
    m_pkey->release();
}

void CRegRegKeyIterator::getstate()
{
    wchar_t name[256];
    DWORD namesize = sizeof(name);
    DWORD err = RegEnumKeyExW(m_pkey->gethkey(), m_index, name, &namesize, 0, 0, 0, 0);
    if (err == ERROR_SUCCESS)
    {
        m_valid = true;
        m_name = name;
    }
    else
    {
        m_valid = false;
    }
}

void CRegRegKeyIterator::next()
{
    ++m_index;
    getstate();
}

IRegKey* CRegRegKeyIterator::get()
{
    return m_pkey->getkey(m_name.c_str());
}

// --- CRegRegRoot

IRegKey* CRegRegRoot::getkey(HKEY hkey, const wchar_t * subkey)
{
    HKEY hreskey;
    DWORD err = RegOpenKeyExW(hkey, subkey, 0, KEY_ALL_ACCESS, &hreskey);
    if (err == ERROR_SUCCESS)
        return new CRegRegKey(hreskey, subkey);
    else
        return 0;
}
