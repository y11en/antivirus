/** 
 * @file
 * @brief	Sample implementation of ILicensingError interface.
 * @author	Andrey Guzhov
 * @date	12.07.2005
 * @version	1.0
 */

#ifndef LICENSINGERROR_H
#define LICENSINGERROR_H

namespace KasperskyLicensing{

/**
 * Implements ILicensingError interface.
 */
class LicensingError : public ILicensingError
{
public:

	/**
	 * Constructs error object by given error code.
	 * @param[in] e_code error code.
	 */
	explicit LicensingError(ErrorCodeType e_code);
	
	/**
	 * Constructs error object by given error code and operation specific error code.
	 * @param[in] e_code error code.
	 * @param[in] s_code operation specific error code.
	 */
	LicensingError(ErrorCodeType e_code, int s_code);

	/**
	 * Returns raised error code.
	 */
	virtual ErrorCodeType GetErrorCode() const;

	/**
	 * Returns operation specific error code.	
	 */
	virtual int GetSpecificCode() const;

private:

	// raised error code
	ErrorCodeType error_code;

	// operation specific error code
	int specific_code;
};

} // namespace KasperskyLicensing

#endif //LICENSINGERROR_H
