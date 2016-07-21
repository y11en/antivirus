#if !defined(_VALUE_SPEC_H_)
#define _VALUE_SPEC_H_

template<>
class Value_Base<void> : public iValue {
public:
	virtual tERROR getValue(void* aBuffer, tDWORD aBufferSize, tDWORD* anOutputSize)
	{
		if(anOutputSize)
			*anOutputSize = 0;
		return errOK;
	}

	virtual tERROR setValue(const void* aBuffer, tDWORD aBufferSize)
	{
		return errOK;
	}  
};

template<>
class Value_Base<bool> : public iValue {
public:
	virtual tERROR getValue(void* aBuffer, tDWORD aBufferSize, tDWORD* anOutputSize)
	{
		tDWORD aSize = sizeof(tBOOL);  

		if(anOutputSize)
			*anOutputSize = aSize;

		if(!aBuffer)
			if(!aBufferSize)
				return errBUFFER_TOO_SMALL;
			else
				return errPARAMETER_INVALID;

		if(aBufferSize < aSize)
			return errBUFFER_TOO_SMALL;
    
		memmove(aBuffer, &theValue, sizeof(tBOOL));
		return errOK;
	}


	virtual tERROR setValue(const void* aBuffer, tDWORD aBufferSize)
	{
		if((!aBuffer) || (aBufferSize < sizeof(tBOOL)))
			return errPARAMETER_INVALID;
		memmove( &theValue, aBuffer, sizeof(theValue));
		return errOK;
	}  
protected:
	tBOOL theValue;
};

template<>
class Value_Base<char*> : public iValue {
public:
	Value_Base(cObject* anOwner)
	: theValue(0),
	theOwner(anOwner)
	{}

	~Value_Base()
	{
		theOwner->heapFree(theValue);
	}

	virtual tERROR getValue(void* aBuffer, tDWORD aBufferSize, tDWORD* anOutputSize)
	{
		tDWORD aSize =(theValue) ? strlen(theValue)+ sizeof(tCHAR): 0;

		if(anOutputSize)
			*anOutputSize = aSize;

		if(!aBuffer)
			if(!aBufferSize)
				return errBUFFER_TOO_SMALL;
			else
				return errPARAMETER_INVALID;

		if(aBufferSize < aSize)
			return errBUFFER_TOO_SMALL;
    
		memmove(aBuffer, theValue, aSize);
		return errOK;
	}

	virtual tERROR setValue(const void* aBuffer, tDWORD aBufferSize)
	{
		if(!aBuffer)
			return errPARAMETER_INVALID;

		char* aTmp = 0;

		tERROR anError =theOwner->heapAlloc(reinterpret_cast<void**>(&aTmp), aBufferSize + sizeof(tCHAR));

		if(PR_FAIL(anError))
			return anError;

		if(theValue)
			theOwner->heapFree(theValue);
    
		theValue = aTmp;    
		memmove(theValue, aBuffer, aBufferSize);
		return errOK;
	}  

protected:
	char* theValue;
	cObject* theOwner;
};

template<>
class Value_Base<wchar_t*> : public iValue {
 public:
	Value_Base(cObject* anOwner)
	: theValue(0),
	theOwner(anOwner)
	{}

	~Value_Base()
	{
		theOwner->heapFree(theValue);
	}

	virtual tERROR getValue(void* aBuffer, tDWORD aBufferSize, tDWORD* anOutputSize)
	{
      
		tDWORD aSize =(theValue) ? (wcslen(theValue)+ 1) * sizeof(tWCHAR): 0;

		if(anOutputSize)
			*anOutputSize = aSize;

		if(!aBuffer)
			if(!aBufferSize)
				return errBUFFER_TOO_SMALL;
			else
				return errPARAMETER_INVALID;
    
		if(aBufferSize < aSize)
			return errBUFFER_TOO_SMALL;
    
		memmove(aBuffer, theValue, aSize);
		return errOK;
	}

	virtual tERROR setValue(const void* aBuffer, tDWORD aBufferSize)
	{
		if(!aBuffer)
			return errPARAMETER_INVALID;

		wchar_t* aTmp = 0;

		tERROR anError = theOwner->heapAlloc(reinterpret_cast<tPTR*>(&aTmp), aBufferSize + sizeof(tWCHAR));

		if(PR_FAIL(anError))
			return anError;

		if(theValue)
			theOwner->heapFree(theValue);
  
		theValue = aTmp;
  
		memmove(theValue, aBuffer, aBufferSize);
		return errOK;
	}  

 protected:
	wchar_t* theValue;
	cObject* theOwner;
};

template<>
class Value_Base<void*> : public iValue {
public:
	Value_Base(cObject* anOwner)
	: theValue(0),
	theSize(0),
	theOwner(anOwner)
	{}

	~Value_Base()
	{
		theOwner->heapFree(theValue);
	}

	virtual tERROR getValue(void* aBuffer, tDWORD aBufferSize, tDWORD* anOutputSize)
	{
      
		if(anOutputSize)
			*anOutputSize = theSize;

		if(!aBuffer)
			if(!aBufferSize)
				return errBUFFER_TOO_SMALL;
			else
				return errPARAMETER_INVALID;
    
		if(aBufferSize < theSize)
			return errBUFFER_TOO_SMALL;
    
		memmove(aBuffer, theValue, theSize);
		return errOK;
	}

	virtual tERROR setValue(const void* aBuffer, tDWORD aBufferSize)
	{
		if(!aBuffer)
			return errPARAMETER_INVALID;

		unsigned char* aTmp = 0;

		tERROR anError = theOwner->heapAlloc(reinterpret_cast<tPTR*>(&aTmp), aBufferSize);
		if(PR_FAIL(anError))
			return anError;

		if(theValue)
			theOwner->heapFree(theValue);

		theValue = aTmp;

		memmove(theValue, aBuffer, aBufferSize);
		theSize = aBufferSize;
		return errOK;
	}  

protected:
	unsigned char* theValue;
	unsigned int theSize;
	cObject* theOwner;
};
#endif //_VALUE_SPEC_H_
