#if !defined (_VALUE_BASE_H_)
#define _VALUE_BASE_H_


template <typename T>
class Value_Base : public iValue {
public:
	Value_Base () 
	{
		memset ( &theValue, 0, sizeof ( theValue ) );
	}

	virtual tERROR getValue ( void* aBuffer, tDWORD aBufferSize, tDWORD* anOutputSize )
	{
		tDWORD aSize = sizeof ( T );  

		if ( anOutputSize )
			*anOutputSize = aSize;

		if ( !aBuffer ) 
			if ( !aBufferSize )
				return errBUFFER_TOO_SMALL;
			else
				return errPARAMETER_INVALID;
    
		if ( aBufferSize < aSize ) 
			return errBUFFER_TOO_SMALL;
    
		memmove ( aBuffer, &theValue, sizeof ( T ) );
		return errOK;
	}


	virtual tERROR setValue ( const void* aBuffer, tDWORD aBufferSize ) 
	{
		if ( ( !aBuffer ) || ( aBufferSize < sizeof ( T ) ) )
			return errPARAMETER_INVALID;
		memmove (  &theValue, aBuffer, sizeof ( theValue ) );
		return errOK;
	}  

protected:
	T theValue;
};

template <typename T>
class StringValue_Base : public Value_Base<T>
{};

#endif // _VALUE_BASE_H_
