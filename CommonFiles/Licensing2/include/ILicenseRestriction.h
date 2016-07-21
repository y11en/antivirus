/** 
 * @file
 * @brief	Declaration of ILicenseRestriction interface.
 * @author	Andrey Guzhov
 * @date	11.07.2005
 * @version	1.0
 */

#ifndef ILICENSERESTRICTION_H
#define ILICENSERESTRICTION_H

namespace KasperskyLicensing {

/** 
 * A license restriction interface.
 * Allows clients to examine restriction and obtain its nominal value.
 * @interface
 */
class ILicenseRestriction
{
public:

	/**
	 * Returns license restriction identifier.
	 */
	virtual unsigned GetId() const = 0;

	/**
	 * Returns name of license restriction.
	 */
	virtual const char_t* GetName() const = 0;

	/**
	 * Examines license restriction.
	 * @param[in] value current value of restriction.
	 * @exception ILicensingError error code specifies the exception reason.
	 * @return automatic pointer to verdict interface.
	 */
	virtual AutoPointer<IVerdict> Examine(unsigned value) const = 0;

protected:

	/**
	 * Destructor is protected in order to prevent direct use of delete operator 
	 */
	virtual ~ILicenseRestriction() {}

private:

	/** @link dependency
	 * @stereotype instantiate*/
	/*# IVerdict lnkIVerdict; */
};

} // namespace KasperskyLicensing

#endif // ILICENSERESTRICTION_H
