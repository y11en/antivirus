// value.h
#if !defined(_VALUE_H_)
#define _VALUE_H_

#include <string.h>
#include <wchar.h>
#include <prague.h>

class iValue{
public:
	virtual tTYPE_ID getType() = 0;
	virtual tERROR getValue(void* aBuffer, tDWORD aBufferSize, tDWORD* anOutputSize) = 0;
	virtual tERROR setValue(const void* aBuffer, tDWORD aBufferSize) = 0;
	virtual tERROR getValueAsString(char* aBuffer, tDWORD aBufferSize, tDWORD* anOutputSize) = 0;
	virtual tERROR setValueAsString(const char* aBuffer) = 0;

	virtual ~iValue(){}
};

#include "value_base.h"

template <typename T, tTYPE_ID TypeID>
class Value : public StringValue_Base <T>{
public:
	tTYPE_ID getType()
	{
	    return TypeID;
	}
};

#include "value_spec.h"
#include "stringvalue_spec.h"

template <>
class Value<tSTRING,tid_STRING> : public StringValue_Base <tSTRING>{
public:
	Value(cObject* anObject)
	: StringValue_Base <tSTRING>(anObject)
	{}

	virtual tTYPE_ID getType()
	{
		return tid_STRING;
	}
};


template <>
class Value<tWSTRING,tid_WSTRING> : public StringValue_Base <tWSTRING>{
public:
	Value(cObject* anObject)
		: StringValue_Base <tWSTRING>(anObject)
	{}

	virtual tTYPE_ID getType()
	{
		return tid_WSTRING;
	}
};

template <>
class Value<tPTR,tid_PTR> : public StringValue_Base <tPTR>{
public:
	Value(cObject* anObject)
	: StringValue_Base <tPTR>(anObject)
	{}

	virtual tTYPE_ID getType()
	{
		return tid_PTR;
	}
};

#endif // _VALUE_H_
