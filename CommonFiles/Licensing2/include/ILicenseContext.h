/** 
 * @file
 * @brief	Declaration of ILicenseContext interface.
 * @author	Andrey Guzhov
 * @date	12.07.2005
 * @version	1.0
 */

#ifndef ILICENSECONTEXT_H
#define ILICENSECONTEXT_H

namespace KasperskyLicensing {

/** 
 * A license context interface.
 * License context provides a way to associate a client 
 * of Kaspersky %Licensing library with the particular license object.
 * @interface 
 */
class ILicenseContext
{
	friend class AutoPointer<ILicenseContext>;

public:

	/**
	 * Defines item enumerator type.
	 */
	typedef Enumerator<LicenseObjectId> ItemEnumerator;

	/**
	 * Adds item to license context.
	 * @param[in] item an item to add.
	 */
	virtual void AddItem(const LicenseObjectId& item) = 0;

	/**
	 * Returns items enumerator.
	 */
	virtual ItemEnumerator GetItems() const = 0;

	/**
	 * Returns true if license context is empty.
	 */
	virtual bool IsEmpty() const = 0;

	/**
	 * Clears contents of the license context.
	 */
	virtual void Clear() = 0;

protected:

	/**
	 * Destructor is protected in order to prevent direct use of delete operator.
	 */
	virtual ~ILicenseContext() {}
};

} // namespace KasperskyLicensing

#endif // ILICENSECONTEXT_H
