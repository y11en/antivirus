#ifndef __KLINITMAIN_H__
#define __KLINITMAIN_H__

#ifdef _DEBUG
    #include <crtdbg.h>
#endif

#include <stdlib.h>

namespace KLINITMAIN
{

    class CInitMainBase
    {
    protected:
        CInitMainBase(const wchar_t* szwProduct, const wchar_t* szwVersion, bool bIsSvc);
        ~CInitMainBase();
    protected:
        const bool  g_bIsSvc;
    };

    class CInitMainSvc
        :   public CInitMainBase
    {
    public:
        CInitMainSvc(const wchar_t* szwProduct = L"", const wchar_t* szwVersion = L"");
        ~CInitMainSvc();
    };

    class CInitMainUt
        :   public CInitMainBase
    {
    public:
        CInitMainUt(const wchar_t* szwProduct = L"", const wchar_t* szwVersion = L"");
        ~CInitMainUt();
    };
};

namespace KLSTD
{
    void SetupServiceDirectory();
};


#endif //__KLINITMAIN_H__
