/** 
 * @file
 * @brief	Declaration of ILicenseObject interface.
 * @author	Andrey Guzhov
 * @date	11.07.2005
 * @version	1.0
 */

#ifndef ILICENSEOBJECT_H
#define ILICENSEOBJECT_H

namespace KasperskyLicensing {

/** 
 * A license object interface.
 * Provides methods to access license restrictions and child license objects.
 * @interface 
 */
class ILicenseObject
{
public:
	
	/** 
	 * Defines restriction enumerator type.
	 */
	typedef Enumerator<const ILicenseRestriction&> RestrictionEnumerator;

	/** 
	 * Defines license object enumerator type.
	 */
	typedef Enumerator<const ILicenseObject&> LicenseObjectEnumerator;

	/** 
	 * Defines supported platform enumerator type.
	 */
	typedef Enumerator<PlatformType> PlatformEnumerator;

	/**
	 * Allows clients to query license restriction by given id.
	 * @param[in] id identifier of restriction to query.
	 * @exception ILicensingError error code specifies the exception reason.
	 * @return reference to queried restriction.
	 */
	virtual const ILicenseRestriction& GetRestriction(unsigned id) const = 0;

	/**
	 * Creates restriction enumerator.
	 * Restriction enumerator allows clients to iterate license restrictions collection.
	 */
	virtual RestrictionEnumerator GetRestrictions() const = 0;

	/**
	 * Creates license object enumerator.
	 * License object enumerator allows clients to iterate child license objects.
	 */
	virtual LicenseObjectEnumerator GetLicenseObjects() const = 0;

	/**
	 * Creates supported platform enumerator.
	 * Supported platform enumerator allows clients to iterate supported platform identifiers.
	 */
	virtual PlatformEnumerator GetSupportedPlatforms() const = 0;

	/**
	 * Returns license object identifier.
	 */
	virtual const LicenseObjectId& GetObjectId() const = 0;

	/**
	 * Returns name of license object.
	 */
	virtual const char_t* GetName() const = 0;

protected:

	/**
	 * Destructor is protected in order to prevent direct use of delete operator. 
	 */
	virtual ~ILicenseObject() {}

private:

	/** @link dependency */
	/*# ILicenseRestriction lnkILicenseRestriction; */
};

} // namespace KasperskyLicensing

#endif // ILICENSEOBJECT_H
