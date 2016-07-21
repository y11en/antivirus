/** 
 * @file
 * @brief	Activation error definitions.
 * @author	Andrey Guzhov
 * @date	18.04.2007
 */

#ifndef _AG_ACTIVATIONERRORS_H_
#define _AG_ACTIVATIONERRORS_H_

namespace OnlineActivation
{
/**
 * Basic exception class.
 * @stereotype exception 
 */
class ActivationError
{
public:

	/**
	 * Error type enumeration.
	 */
	enum ErrorType
	{

		/**
		 * System error.
		 * Error code contains Win32 API error value.
		 */
		SYSTEM_ERROR = 1,

		/**
		 * HTTP protocol error.
		 * Error code contains HTTP status code.
		 */
		HTTP_ERROR,

		/**
		 * Activation server error.
		 * Error code contains activation server error value.
		 */
		SERVER_ERROR,

		/**
		 * Application logic error.
		 * Error code contains error value.
		 */
		LOGICAL_ERROR

	};

	/**
	 * Returns error type.
	 */
	virtual ErrorType GetErrorType() const = 0;

	/**
	 * Returns error code.
	 */
	virtual unsigned long GetErrorCode() const = 0;
};

/**
 * Operating system error.
 * @stereotype exception 
 */
class SystemError : public ActivationError
{
public:
	SystemError(unsigned long nError)
		: m_nError(nError) {}
	/**
	 * Returns error type.
	 */
	virtual ErrorType GetErrorType() const
		{ return SYSTEM_ERROR; }
	/**
	 * Returns Win32 API error code.
	 */
	virtual unsigned long GetErrorCode() const
		{ return m_nError; }
private:
	/**
	 * Stores Win32 API error code.
	 */
	unsigned long m_nError;
};

/**
 * HTTP protocol error.
 * Indicates HTTP error occurred during activation.
 * @stereotype exception 
 */
class HttpError : public ActivationError
{
public:
	HttpError(unsigned long nStatus)
		: m_nStatus(nStatus) {}
	/**
	 * Returns error type.
	 */
	virtual ErrorType GetErrorType() const
		{ return HTTP_ERROR; }
	/**
	 * Returns HTTP_STATUS_* value defined in wininet.h.
	 */
	virtual unsigned long GetErrorCode() const
		{ return m_nStatus; }
private:
	/**
	 * Stores HTTP error status code.
	 */
	unsigned long m_nStatus;
};

/**
 * Activation server error.
 * @stereotype exception 
 */
class ServerError : public ActivationError
{
public:

	/**
	 * Server error code enumeration.
	 */
	enum ErrorCode
	{
		/**
		 * Unknown server error occurred.
		 */
		UNKNOWN_SERVER_ERROR = 1,

		/**
		 * Internal server error occurred.
		 */
		INTERNAL_SERVER_ERROR,

		/**
		 * There is no key for supplied activation code.
		 */
		KEY_NOT_FOUND,

		/**
		 * Key or activation code is blocked.
		 */
		KEY_OR_ACTIVATION_CODE_BLOCKED,

		/**
		 * Mandatory request parameters are missing.
		 */
		MANDATORY_PARAMETERS_MISSING,
		
		/**
		 * Wrong customer id or password.
		 */
		WRONG_CUSTOMER_ID_OR_PASSWORD,
		
		/**
		 * Wrong activation code.
		 */
		WRONG_ACTIVATION_CODE,

		/**
		 * Supplied application id is not listed on the server.
		 */
		WRONG_APPLICATION_ID,
		
		/**
		 * Client must supply activation code.
		 */
		ACTIVATION_CODE_REQUIRED,
		
		/**
		 * Client must use mobile activation protocol for supplied activation code.
		 */
		USE_MOBILE_ACTIVATION,
		
		/**
		 * Activation period expired.
		 */
		ACTIVATION_PERIOD_EXPIRED,
		
		/**
		 * Quantity of activation attempts exhausted.
		 */
		ACTIVATION_ATTEMPTS_EXHAUSTED,
	};

	ServerError(ErrorCode nError)
		: m_nError(nError) {}
	/**
	 * Returns error type.
	 */
	virtual ErrorType GetErrorType() const
		{ return SERVER_ERROR; }
	/**
	 * Returns activation server error.
	 */
	virtual unsigned long GetErrorCode() const
		{ return m_nError; }
private:
	/**
	 * Stores activation server error.
	 */
	ErrorCode m_nError;
};

/**
 * Logical error.
 * This error informs that library's application level logic occurred.
 * @stereotype exception 
 */
class LogicalError : public ActivationError
{
public:
	enum ErrorCode
	{
		/**
		 * Unexpected (unknown) error.
		 */
		UNEXPECTED_ERROR = 1,

		/**
		 * Invalid parameter supplied, e.g. empty activation URL list.
		 */
		INVALID_PARAMETER,

		/**
		 * Activation code is invalid (wrong checksum).
		 */
		INVALID_ACTIVATION_CODE,

		/**
		 * Customer identifier is invalid.
		 */
		INVALID_CUSTOMER_ID,

		/**
		 * Customer password is invalid.
		 */
		INVALID_CUSTOMER_PASSWORD,

		/**
		 * Activation server returned unknown response.
		 */
		UNKNOWN_SERVER_RESPONSE,
		
		/**
		 * Activation request execution aborted.
		 */
		OPERATION_ABORTED
	};

	LogicalError(ErrorCode nError)
		: m_nError(nError) {}

	/**
	 * Returns error type.
	 */
	virtual ErrorType GetErrorType() const
		{ return LOGICAL_ERROR; }
	/**
	 * Returns error value.
	 */
	virtual unsigned long GetErrorCode() const
		{ return m_nError; }
private:
	/**
	 * Stores error value.
	 */
	ErrorCode m_nError;
};

} // namespace OnlineActivation

#endif //_AG_ACTIVATIONERRORS_H_
