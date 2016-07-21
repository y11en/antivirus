#ifndef __REGIFACE_H__
#define __REGIFACE_H__

#include <string>

namespace AVPUpgradeSettings
{

    class IRegBase
    {
    public:
        virtual void addref() = 0;
        virtual void release() = 0;
    };

    enum ERegType
    {
        ERegTypeNone,
        ERegTypeBinary,
        ERegTypeDword,
        ERegTypeQword,
        ERegTypeExpandString,
        ERegTypeString
    };

    class IRegValue : virtual public IRegBase
    {
    public:


        virtual std::wstring getname() = 0;
        virtual size_t getsize () = 0;
        virtual ERegType gettype () = 0;

        virtual bool get(void *buffer, size_t& size) = 0;
        virtual bool set(void *buffer, size_t  size) = 0;
    };

    class IRegValueIterator : virtual public IRegBase
    {
    public:
        virtual void next() = 0;
        virtual bool isvalid() = 0;

        virtual std::wstring getname() = 0;
        virtual IRegValue* get() = 0;
    };

    class IRegKeyIterator;
    class IRegKey : virtual public IRegBase
    {
    public:
        virtual std::wstring getname() = 0;

        virtual IRegKeyIterator* enumkeys() = 0;
        virtual IRegValueIterator* enumvalues() = 0;

        virtual IRegKey *getkey(const wchar_t *key) = 0;
        virtual IRegKey *createkey(const wchar_t *key) = 0;

        virtual IRegValue *getvalue(const wchar_t *value) = 0;
        virtual IRegValue *setvalue(const wchar_t *value, ERegType type, void *buffer, size_t size) = 0;
    };

    class IRegKeyIterator : virtual public IRegBase
    {
    public:
        virtual void next() = 0;
        virtual bool isvalid() = 0;

        virtual std::wstring getname() = 0;
        virtual IRegKey* get() = 0;
    };

};

#endif //__REGIFACE_H__