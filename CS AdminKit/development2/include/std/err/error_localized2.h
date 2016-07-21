#ifndef __ERROR_LOCALIZED2_H__
#define __ERROR_LOCALIZED2_H__

#include <std/err/error_localized.h>

namespace KLERR
{
    //! provides error localization info
    class KLSTD_NOVTABLE ErrorLocalization2
        :   public ErrorLocalization
    {
    public:
        /*!
          \brief    Writes localization info into specified error. 

          \param	szwValue IN locinfo 
          \param	szwDefFormat IN default format string
        */
        KLSTD_NOTHROW virtual void LocSetInfo2(
                            const ErrLocAdapt&  locinfo,
                            const wchar_t*      szwDefFormat) throw() = 0;
    };
};

#endif //__ERROR_LOCALIZED2_H__
