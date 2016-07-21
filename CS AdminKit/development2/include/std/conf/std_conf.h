/*!
 * (C) 2003 Kaspersky Lab 
 * 
 * \file	std_read_conf.h
 * \author	Andrew Kazachkov
 * \date	20.10.2006 18:30:45
 * \brief	
 * 
 */

#include <std/base/klbase.h>
#include <std/par/params.h>

#ifndef __KLSTD_READ_CONF_H__
#define __KLSTD_READ_CONF_H__

namespace KLSTD
{
    class KLSTD_NOVTABLE ReadConfigFilter
        :   public KLBaseQI
    {
    public:
        virtual bool Filter(
                        const wchar_t*  szwSection,
                        const wchar_t*  szwVariable,
                        const wchar_t*  szwValue,
                        KLPAR::Value**  ppValue) = 0;
    };
};

/*!
  \brief	Reads specified conf file

  \param	IN conf file name
  \param	IN zero-terminated array of section to read. if NULL, read all sections
  \return	containms sections as first-level subcontainers

For instance following conf 

[section one]
    var1_1 = "val1_1"
    var1_2 = "val1_2"
    var1_3 = "val1_3"
[section two]
    var2_1 = "val2_1"
    var2_2 = "val2_2"
    var2_3 = "val2_3"

will result in following Params container

    ---+--section one
       |  +--var1_1 = "val1_1"  
       |  +--var1_2 = "val1_2"
       |  +--var1_3 = "val1_3"
       |
       +--section two
          +--var2_1 = "val2_1"
          +--var2_2 = "val2_2"
          +--var2_3 = "val2_3"
*/

KLCSC_DECL void KLSTD_ReadConfig(
            const wchar_t*      szwFileName, 
            const wchar_t**     ppSections,
            KLSTD::KLBaseQI*    pFilter,
            KLPAR::Params**     ppResult);

namespace KLSTD
{
    class KLSTD_NOVTABLE TextFileLineReader
        :   public KLBaseQI
    {
    public:
        virtual void ReadLine(std::wstring& wstrLine) = 0;
        virtual bool IsEOF() = 0;
    };

    typedef KLSTD::CAutoPtr<TextFileLineReader> TextFileLineReaderPtr;
};


/*!
  \brief	Reading text file (possibly Unicode text file) line-by-line. 
            Supported formats (under Windows):
                1. Current codepage
                2. UTF8
                3. UCS16
                4. UCS16 Big Endian
               

  \param	szwFileName IN file name
  \param	ppResult OUT pointer to pointer to TextFileLineReader interface
*/
KLCSC_DECL void KLSTD_CreateTextFileLineReader(
                    const wchar_t*              szwFileName, 
                    KLSTD::TextFileLineReader** ppResult);

#endif //__KLSTD_READ_CONF_H__
