#if !defined(_STRINGVALUE_SPEC_H_)
#define _STRINGVALUE_SPEC_H_

#include <climits>
#include <ctype.h>
#include <stdlib.h>
#include <utf8.h>

#if defined(_MSC_VER)
inline tLONGLONG strtoll(const char* nptr, char** endpnt, int base)
{ 
	*endpnt = const_cast <char*>(nptr);
	while(**endpnt && isspace(**endpnt))
		*endpnt++;
	int aSign = 1;
	if((**endpnt == '+') || (**endpnt == '-'))
		aSign =(*(*endpnt)++ == '-')? -1 : 1;
	if((base == 16) && 
	    (**endpnt == '0') && 
	    ((*(*endpnt + 1)== 'x') && 
	      (*(*endpnt + 1)== 'X')))
		*endpnt += 2;
	char* aBegin = *endpnt;
	if(base == 16)
		while(**endpnt && isxdigit(**endpnt))
			(*endpnt)++;
	if(base == 10)
		while(**endpnt && isdigit(**endpnt))
			(*endpnt)++;
	if(**endpnt)
		return 0;    
	char* anEnd = *endpnt - 1;
	tLONGLONG aResult = 0;
	if(base == 16){
		tLONGLONG anIndex = 1;
		do {
			int aChar = tolower(*anEnd);
			if((aChar <= 'f') && (aChar >= 'a'))
				aResult +=(aChar - 'a')* anIndex;
			else
				aResult +=(aChar - '0')* anIndex;
			anIndex *= base;
		}
		while(anEnd-- != aBegin);
	}
	if(base == 10){ 
		tLONGLONG anIndex = 1;
		do {
			aResult +=(*anEnd - '0')* anIndex;
			anIndex *= base;
		}
		while(anEnd-- != aBegin);
	}
	aResult *= aSign;
	return aResult;
}

inline tQWORD strtoull(const char* nptr, char** endpnt, int base)
{
	return strtoll(nptr, endpnt, base);
}
#endif

template<typename T>
tERROR getString(T aValue, char* aBuffer, tDWORD aBufferSize, tDWORD* anOutputSize, const char* aPattern)
{
	char aTmp [0x40] = {0};
	sprintf(aTmp, aPattern, aValue);

	tDWORD aSize = strlen(aTmp);  

	if(anOutputSize)
		*anOutputSize = aSize;
    
	if(aBufferSize < aSize)
		return errBUFFER_TOO_SMALL;

	if(!aBuffer)
		return errPARAMETER_INVALID;
    
	memmove(aBuffer, aTmp, aSize);
	return errOK;
}


template<typename T, typename TT, TT(*Func)(const char*, char**, int), T MaxValue>
tERROR setString(T& aValue, const char* aBuffer)
{
	if(!aBuffer)
		return errPARAMETER_INVALID;
	char* aEndPtr = 0;
	TT aTmp = Func(aBuffer, &aEndPtr, 10);
	if(*aEndPtr){
		aTmp = strtoul(aBuffer, &aEndPtr, 16);
		if(*aEndPtr)
			return errPARAMETER_INVALID;
	}
	if(MaxValue && (aTmp > MaxValue))
		return errPARAMETER_INVALID;
	aValue = static_cast<T>(aTmp);
	return errOK;
}

template <> 
class StringValue_Base<bool>: public Value_Base<bool> { 
public:
	tERROR getValueAsString(char* aBuffer, tDWORD aBufferSize, tDWORD* anOutputSize)
	{
		tDWORD aSize = (theValue)? sizeof("1"): sizeof("0");
		
		if(anOutputSize)
			*anOutputSize = aSize;
		
		if(aBufferSize < aSize)
			return errBUFFER_TOO_SMALL;
		
		if(!aBuffer)
			return errPARAMETER_INVALID;
		
		strcpy(aBuffer,(theValue)? "1" : "0");
		return errOK;
	}

	tERROR setValueAsString(const char* aBuffer)
	{
		const char *aBegin = aBuffer;
		while(isspace(*aBegin))
			++aBegin;
		const char *anEnd = aBegin;
		while(*anEnd && !isspace(*anEnd))
			++anEnd;
		if(anEnd - aBegin == 4 && 
		   !strncmp(aBegin, "true", 4)){
			theValue = true;
			return errOK;
		}
		if(anEnd - aBegin == 5 && 
		   !strncmp(aBegin, "false", 5)){
			theValue = false;
			return errOK;
		}
		
		if(!isdigit(*aBuffer))
			return errPARAMETER_INVALID;
		theValue = strtoul(aBuffer, 0, 0);
		return errOK;
	}
};

template <> 
class StringValue_Base<char>: public Value_Base<char> { 
public:
	tERROR getValueAsString(char* aBuffer, tDWORD aBufferSize, tDWORD* anOutputSize)
	{
		tDWORD aSize = sizeof(theValue);  
		
		if(anOutputSize)
			*anOutputSize = aSize;
		
		if(aBufferSize < aSize)
			return errBUFFER_TOO_SMALL;
		
		if(!aBuffer)
			return errPARAMETER_INVALID;
		
		*aBuffer = theValue;
		return errOK;
	}

	tERROR setValueAsString(const char* aBuffer)
	{
		if(!aBuffer)
			return errPARAMETER_INVALID;
		theValue = *aBuffer;    
		return errOK;
	}
};

template <> 
class StringValue_Base<wchar_t>: public Value_Base<wchar_t> { 
public:
	tERROR getValueAsString(char* aBuffer, tDWORD aBufferSize, tDWORD* anOutputSize)
	{
		return errNOT_OK;    
	}

	tERROR setValueAsString(const char* aBuffer)
	{
		return errNOT_OK;    
	}
};

template <> 
class StringValue_Base<void*>: public Value_Base<void*> { 
public:
	StringValue_Base(cObject* anOwner)
		: Value_Base<void*>(anOwner)
		{}

	tERROR getValueAsString(char* aBuffer, tDWORD aBufferSize, tDWORD* anOutputSize)
	{
		if(anOutputSize)
			*anOutputSize = theSize * 2 + sizeof(tCHAR);
        
		if(aBufferSize <= theSize * 2)
			return errBUFFER_TOO_SMALL;

		if(!aBuffer)
			return errPARAMETER_INVALID;
    
		for(unsigned int i = 0; i < theSize; ++i){
			tERROR error = getString(theValue[i], aBuffer + i * 2, aBufferSize - i * 2, 0, "%02X");
			if(PR_FAIL(error))
				return error;
		}
    
		return errOK;
	}

	tERROR setValueAsString(const char* aBuffer)
	{
		tDWORD aSize = strlen(aBuffer);  	  
		if(aSize%2)
			return errPARAMETER_INVALID;
		unsigned char* aValue = 0;
		aSize /= 2;
		tERROR anError = theOwner->heapAlloc(reinterpret_cast<void**>(&aValue), aSize);

		if(PR_FAIL(anError))
			return anError;

		char aTmp [5] = {0};
		aTmp[0] = '0';
		aTmp[1] = 'x';

		for(unsigned int i = 0; i < aSize; ++i){
			aTmp[2] = aBuffer[i * 2];
			aTmp[3] = aBuffer[i * 2 + 1];
			anError = setString<unsigned char, unsigned long,strtoul,UCHAR_MAX>(aValue [ i ], aTmp);
			if(PR_FAIL(anError)){
				theOwner->heapFree(aValue);
				return anError;
			}
		}
		if(theValue)
			theOwner->heapFree(theValue);
		theValue = aValue;
		theSize = aSize;

		return anError;
	}
};

template <>
class Value<void*,tid_BINARY> : public StringValue_Base <void*> {
public:
	Value(cObject* anObject)
		: StringValue_Base <void*>(anObject)
		{}

	virtual tTYPE_ID getType()
	{
		return tid_BINARY;
	}
};


template <> 
class StringValue_Base<char*>: public Value_Base<char*> { 
public:
	StringValue_Base(cObject* anOwner)
		: Value_Base<char*>(anOwner)
		{}

	tERROR getValueAsString(char* aBuffer, tDWORD aBufferSize, tDWORD* anOutputSize)
	{
		return getValue(aBuffer, aBufferSize, anOutputSize);
	}


	tERROR setValueAsString(const char* aBuffer)
	{
		return setValue(aBuffer,(aBuffer)? strlen(aBuffer): 0);
	}
	
};

template <> 
class StringValue_Base<wchar_t*>: public Value_Base<wchar_t*> { 
public:
	StringValue_Base(cObject* anOwner)
		: Value_Base<wchar_t*>(anOwner)
		{}

	tERROR getValueAsString(char* aBuffer, tDWORD aBufferSize, tDWORD* anOutputSize)
	{
		tDWORD aSize = 0;  

		wchar_t* pos = theValue;	  
		while(*pos)
			aSize += utf8_char_size(*pos++);
   
		if(aSize)
			aSize += sizeof(char);

		if(anOutputSize)
			*anOutputSize = aSize;

		if(aBufferSize < aSize)
			return errBUFFER_TOO_SMALL;
    
		if(!aBuffer)
			return errPARAMETER_INVALID;
    
		utf8_encode_string(theValue, wcslen(theValue), aBuffer);

		return errOK;	  
	}


	tERROR setValueAsString(const char* aBuffer)
	{
		Value<void*,tid_BINARY> aTmp(theOwner);
		aTmp.setValueAsString(aBuffer);
		wchar_t* aValue = 0;
		unsigned int aSize = 0;
		aTmp.getValue(0, 0, &aSize);
		tERROR anError = theOwner->heapAlloc(reinterpret_cast<void**>(&aValue), aSize + sizeof(tWCHAR)); 
		if(PR_FAIL(anError))
			return anError;
		anError = aTmp.getValue(aValue, aSize, 0);
		if(PR_FAIL(anError)){
			theOwner->heapFree(aValue);	    
			return anError;
		}
		if(theValue)
			theOwner->heapFree(theValue);
		theValue = aValue; 
		return anError;
	}

	tERROR setValueAsString2(const char* aBuffer)
	{
		tDWORD aSize = 0;  

		const char* pos = aBuffer;	  
		while(*pos){
			pos += utf8_char_size(static_cast<unsigned char>(*pos));
			++aSize;
		}

		wchar_t* aValue = 0;

		tERROR anError = theOwner->heapAlloc(reinterpret_cast<void**>(&aValue), (aSize + sizeof(tCHAR)) * sizeof(wchar_t)); 
		if(PR_FAIL(anError))
			return anError;

		utf8_decode_string(aBuffer, strlen(aBuffer), aValue);

		if(theValue)
			theOwner->heapFree(theValue);
		theValue = aValue; 

		return errOK;	  
	}

};

template <> 
class StringValue_Base<signed char>: public Value_Base<signed char> { 
public:
	tERROR getValueAsString(char* aBuffer, tDWORD aBufferSize, tDWORD* anOutputSize)
	{
		return getString(theValue, aBuffer, aBufferSize, anOutputSize, "%i"); 
	}

	tERROR setValueAsString(const char* aBuffer)
	{
		return setString<signed char, long,strtol,CHAR_MAX>(theValue, aBuffer);
	}
};

template <> 
class StringValue_Base<unsigned char>: public Value_Base<unsigned char> { 
public:
	tERROR getValueAsString(char* aBuffer, tDWORD aBufferSize, tDWORD* anOutputSize)
	{
		return getString(theValue, aBuffer, aBufferSize, anOutputSize, "%u"); 
	}

	tERROR setValueAsString(const char* aBuffer)
	{
		return setString<unsigned char, unsigned long,strtoul,UCHAR_MAX>(theValue, aBuffer);
	}
};

template <> 
class StringValue_Base<signed short>: public Value_Base<signed short> { 
public:
	tERROR getValueAsString(char* aBuffer, tDWORD aBufferSize, tDWORD* anOutputSize)
	{
		return getString(theValue, aBuffer, aBufferSize, anOutputSize, "%i"); 
	}

	tERROR setValueAsString(const char* aBuffer)
	{
		return setString<short, long, strtol, SHRT_MAX>(theValue, aBuffer);
	}
};

#if !defined(_MSC_VER)||(_MSC_VER>=1400)
template <> 
class StringValue_Base<unsigned short>: public Value_Base<unsigned short> { 
public:
	tERROR getValueAsString(char* aBuffer, tDWORD aBufferSize, tDWORD* anOutputSize)
	{
		return getString(theValue, aBuffer, aBufferSize, anOutputSize, "%u"); 
	}

	tERROR setValueAsString(const char* aBuffer)
	{
		return setString<unsigned short, unsigned long, strtoul, USHRT_MAX>(theValue, aBuffer);
	}
};
#endif

template <> 
class StringValue_Base<int>: public Value_Base<int> { 
public:
	tERROR getValueAsString(char* aBuffer, tDWORD aBufferSize, tDWORD* anOutputSize)
	{
		return getString(theValue, aBuffer, aBufferSize, anOutputSize, "%i"); 
	}

	tERROR setValueAsString(const char* aBuffer)
	{
		return setString<int, long, strtol, INT_MAX>(theValue, aBuffer);
	}
};

template <> 
class StringValue_Base<unsigned int>: public Value_Base<unsigned int> { 
public:
	tERROR getValueAsString(char* aBuffer, tDWORD aBufferSize, tDWORD* anOutputSize)
	{
		return getString(theValue, aBuffer, aBufferSize, anOutputSize, "%u");     
	}

	tERROR setValueAsString(const char* aBuffer)
	{
		return setString<unsigned int, unsigned long, strtoul, UINT_MAX>(theValue, aBuffer);
	}
};

template <> 
class StringValue_Base<long>: public Value_Base<long> { 
public:
	tERROR getValueAsString(char* aBuffer, tDWORD aBufferSize, tDWORD* anOutputSize)
	{
		return getString(theValue, aBuffer, aBufferSize, anOutputSize, "%i"); 
	}

	tERROR setValueAsString(const char* aBuffer)
	{
		return setString<long, long, strtol, 0>(theValue, aBuffer);
	}
};

template <> 
class StringValue_Base<unsigned long>: public Value_Base<unsigned long> { 
public:
	tERROR getValueAsString(char* aBuffer, tDWORD aBufferSize, tDWORD* anOutputSize)
	{
		return getString(theValue, aBuffer, aBufferSize, anOutputSize, "%u"); 
	}

	tERROR setValueAsString(const char* aBuffer)
	{
		return setString<unsigned long, unsigned long, strtoul, 0>(theValue, aBuffer);
	}
};


#if !defined(_MSC_VER)||(_MSC_VER >= 1400)
template <> 
class StringValue_Base<long long>: public Value_Base<long long> { 
public:
	tERROR getValueAsString(char* aBuffer, tDWORD aBufferSize, tDWORD* anOutputSize)
	{
		return getString(theValue, aBuffer, aBufferSize, anOutputSize, PRINTF_LONGLONG); 
	}

	tERROR setValueAsString(const char* aBuffer)
	{
		return setString<long long, long long, strtoll, 0>(theValue, aBuffer);
	}
};

template <> 
class StringValue_Base<unsigned long long>: public Value_Base<unsigned long long> { 
public:
	tERROR getValueAsString(char* aBuffer, tDWORD aBufferSize, tDWORD* anOutputSize)
	{
		return getString(theValue, aBuffer, aBufferSize, anOutputSize, PRINTF_QWORD); 
	}

	tERROR setValueAsString(const char* aBuffer)
	{
		return setString<unsigned long long, unsigned long long, strtoull, 0>(theValue, aBuffer);
	}
};
#elif defined(_MSC_VER)
template <> 
class StringValue_Base<__int64>: public Value_Base<__int64> { 
public:
	tERROR getValueAsString(char* aBuffer, tDWORD aBufferSize, tDWORD* anOutputSize)
	{
		return getString(theValue, aBuffer, aBufferSize, anOutputSize, PRINTF_LONGLONG); 
	}

	tERROR setValueAsString(const char* aBuffer)
	{
		return setString<__int64, __int64, strtoll, 0>(theValue, aBuffer);
	}
};

template <> 
class StringValue_Base<unsigned __int64>: public Value_Base<unsigned __int64> { 
public:
	tERROR getValueAsString(char* aBuffer, tDWORD aBufferSize, tDWORD* anOutputSize)
	{
		return getString(theValue, aBuffer, aBufferSize, anOutputSize, PRINTF_QWORD); 
	}

	tERROR setValueAsString(const char* aBuffer)
	{
		return setString<unsigned __int64, unsigned __int64, strtoull, 0>(theValue, aBuffer);
	}
};
#endif //!defined(_MSC_VER)||(_MSC_VER >= 1400)

template <> 
class StringValue_Base<double>: public Value_Base<double> { 
public:
	tERROR getValueAsString(char* aBuffer, tDWORD aBufferSize, tDWORD* anOutputSize)
	{
		return getString(theValue, aBuffer, aBufferSize, anOutputSize, "%f"); 
	}

	tERROR setValueAsString(const char* aBuffer)
	{
		if(!aBuffer)
			return errPARAMETER_INVALID;
		char* aEndPtr = 0;
		theValue = strtod(aBuffer, &aEndPtr);
		if(*aEndPtr)
			return errPARAMETER_INVALID;
		return errOK;
	}
};

// dummy templates

template <> 
class StringValue_Base<void>: public Value_Base<void> { 
public:
	tERROR getValueAsString(char* aBuffer, tDWORD aBufferSize, tDWORD* anOutputSize)
	{
		if(anOutputSize)
			*anOutputSize = 0;
		return errOK;
	}

	tERROR setValueAsString(const char* aBuffer)
	{
		return errOK;
	}
};

template <> 
class StringValue_Base<tFUNC_PTR>: public Value_Base<tFUNC_PTR> { 
public:
	tERROR getValueAsString(char* aBuffer, tDWORD aBufferSize, tDWORD* anOutputSize)
	{
		if(anOutputSize)
			*anOutputSize = 0;
		return errOK;
	}

	tERROR setValueAsString(const char* aBuffer)
	{
		return errOK;
	}
};

template <> 
class StringValue_Base<tDATETIME>: public Value_Base<tDATETIME> { 
public:
	tERROR getValueAsString(char* aBuffer, tDWORD aBufferSize, tDWORD* anOutputSize)
	{
		if(anOutputSize)
			*anOutputSize = 0;
		return errOK;
	}

	tERROR setValueAsString(const char* aBuffer)
	{
		return errOK;
	}
};

#endif // _STRINGVALUE_SPEC_H_
