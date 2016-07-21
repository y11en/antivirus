/*!
 * (C) 2003 Kaspersky Lab 
 * 
 * \file	parbinser.h
 * \author	Andrew Kazachkov
 * \date	05.12.2003 12:36:44
 * \brief	
 * 
 */

#ifndef __KL_PARBINSER_H__
#define __KL_PARBINSER_H__

#include "std/par/parstreams.h"
#include "std/par/valenum.h"
#include "std/err/klerrors.h"
#include "std/par/parserialize.h"

namespace KLPAR
{
    static const AVP_dword c_dwNullParamsSize=0xFFFFFFFF;
    
    typedef enum
    {
        SBVT_NULL,
        SBVT_EMPTY,
        SBVT_STRING,
        SBVT_BOOL,
        SBVT_INT,
        SBVT_LONG,
        SBVT_DATE_TIME,
        SBVT_DATE,
        SBVT_BINARY,
        SBVT_FLOAT,
        SBVT_DOUBLE,
        SBVT_PARAMS,
        SBVT_ARRAY
    }serbinval_t;

    class CParamsBinWriter : public EnumValuesCallback
    {
    public:
        CParamsBinWriter(
                CBinWriter&             writer,                
                const SerParamsFormat*  pFormat);
       
        void Start(KLPAR::Params*  pParams);

    protected:
        void WriteParams(KLPAR::Params* pParams);

        void WriteValue(KLPAR::Value* pValue);

        inline CBinWriter& WriteValueType(serbinval_t type)
        {
            m_Writer << AVP_byte(type);
            return (m_Writer);
        }
    protected://EnumValuesCallback implementation
        KLSTD_INTERAFCE_MAP_BEGIN(KLPAR::EnumValuesCallback)
        KLSTD_INTERAFCE_MAP_END()
        unsigned long AddRef(){return 0;};
	    unsigned long Release(){return 0;};
        void OnEnumeratedValue(const wchar_t* szwName, KLPAR::Value*);
    protected:
        CBinWriter&             m_Writer;
        const SerParamsFormat*  m_pFormat;
    };

    class CParamsBinReader
    {
    public:
        CParamsBinReader(
                        CBinReader&             reader,
                        const SerParamsFormat*  pFormat);
       
        void Start(KLPAR::Params**  ppParams);

    protected:
        void KLSTD_FASTCALL ReadParams(KLPAR::Params** ppParams);

        void KLSTD_FASTCALL ReadValue(KLPAR::Value** ppValue);

        KLSTD_INLINEONLY CBinReader& KLSTD_FASTCALL ReadValueType(serbinval_t& type)
        {
            AVP_byte type2;
            m_Reader >> type2;
            type=(serbinval_t)type2;
            return (m_Reader);
        }
    protected:
        CBinReader&                     m_Reader;
        const SerParamsFormat*          m_pFormat;
        std::wstring                    m_wstrBuffer;
        KLSTD::CAutoPtr<ValuesFactory>  m_pFactory;                

    };

};
#endif //__KL_PARBINSER_H__
