/** 
 * @file
 * @brief	Declaration of ILicenseUtility interface.
 * @author	Andrey Guzhov
 * @date	08.07.2005
 * @version	1.0
 */

#ifndef ILICENSEUTILITY_H
#define ILICENSEUTILITY_H

namespace KasperskyLicensing {

/** 
 * A license utility interface.
 * @interface
 */
class ILicenseUtility
{
	friend class AutoPointer<ILicenseUtility>;

public:

	typedef Enumerator<const ILicenseKey&> KeyEnumerator;

	/**
	 * License key status type.
	 */
	enum LicenseKeyStatus
	{
		/**
		 * License key is valid.
		 */
		KEY_STATUS_OK,

		KEY_INCOMPATIBLE_PRODUCT,

		KEY_INCOMPATIBLE_VERSION_TYPE,

		KEY_LICENSE_OBJECT_MISSING

	};
	
	/**
	 * Returns current license context.
	 */
	virtual const ILicenseContext& GetLicenseContext() const = 0;

	/**
	 * Returns license object interface.
	 */
	virtual const ILicenseObject& GetLicenseObject() const = 0;

	/**
	 * Installs license key.
	 */
	virtual const ILicenseKey&
		InstallLicenseKey(const char_t* name, const void* data, size_t size) = 0;

	/**
	 * Uninstalls license key.
	 */
	virtual void UninstallLicenseKey(const LicenseKeyInfo::SerialNumber& sn) = 0;

	/**
	 * Validates license key and returns its status.
	 */
	virtual LicenseKeyStatus GetLicenseKeyStatus(const ILicenseKey& key) const = 0;

	/**
	 * Returns installed keys collection.
	 */
	virtual KeyEnumerator GetInstalledKeys() = 0;

protected:

	/**
	 * Destructor is protected in order to prevent direct use of delete operator.
	 */
	virtual ~ILicenseUtility() {}

private:

	/** @link dependency
	 * @stereotype instantiate*/
	/*# ILicenseKeyManager lnkIKeyRegistry; */

	/** @link dependency
	 * @stereotype use*/
	/*# IStorageR lnkIStorageR; */

	/** @link dependency
	 * @stereotype use*/
	/*# IStorageRW lnkIStorageRW; */

	/** @link dependency 
	 * @stereotype instantiate*/
	/*# ILicenseKeyValidator lnkICheckupDatabase; */

	/** @link dependency */
	/*# ILicenseObject lnkILicenseObject; */
};

} // namespace KasperskyLicensing

#endif // ILICENSEUTILITY_H
