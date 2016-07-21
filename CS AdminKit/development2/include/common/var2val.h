/*!
 * (C) 2003 Kaspersky Lab 
 * 
 * \file	var2val.h
 * \author	Andrew Kazachkov
 * \date	06.01.2004 11:06:17
 * \brief	
 * 
 */

#ifndef __KLPAR_VAR2VAL_H__
#define __KLPAR_VAR2VAL_H__

#include <std/base/klstd.h>
#include <std/par/params.h>
#include <wtypes.h>

namespace KLPAR
{
    class CDecimal : public DECIMAL
    {
    public:
	    CDecimal()
	    {
		    clear();
	    }	
	    CDecimal(const CDecimal& x)
	    {
		    static_cast<DECIMAL&>(*this)=static_cast<const DECIMAL&>(x);
	    }	
	    CDecimal(const DECIMAL& x)
	    {
		    static_cast<DECIMAL&>(*this)=x;
	    }
	    CDecimal(unsigned long x)
	    {
		    clear();
		    Lo32 = x;
	    }
	    CDecimal(long x)
	    {
		    clear();
		    if(x >= 0)
		    {
			    Lo32 = x;
		    }
		    else
		    {
			    Lo32 = -x;
			    sign = DECIMAL_NEG;
		    };
	    }
	    CDecimal(unsigned __int64 x)
	    {
		    clear();
		    Lo64 = x;
	    }
	    CDecimal(__int64 x)
	    {
		    clear();
		    if(x >= 0)
		    {
			    Lo64 = x;
		    }
		    else
		    {
			    Lo64 = -x;
			    sign = DECIMAL_NEG;
		    };
	    }
	    void __fastcall clear();
	    __fastcall operator __int64() const;
	    __fastcall operator double() const;
    protected:
        __int64 __fastcall Pow10(unsigned n) const;
    };

    /*!
      \brief	Variant2Value

      \param	var		[in]
      \param	ppValue	[out]
    */
    void Variant2Value(VARIANT& var, KLPAR::Value** ppValue, bool bAllowNull = false);

    /*!
      \brief	Value2Variant

      \param	pValue	[in]
      \param	var		[out]
    */
    void Value2Variant(KLPAR::Value* pValue, VARIANT& var);
    
    void CreateVarBinary(const void* pData, size_t nData, VARIANT& var);
    
    bool GetBinaryFromVar(VARIANT& varVal, KLSTD::MemoryChunk** ppChunk);
}
#endif //__KLPAR_VAR2VAL_H__