/** 
 * @file
 * @brief	Implementation of ILicenseUtility interface.
 * @author	Andrey Guzhov
 * @date	08.07.2005
 * @version	1.0
 */

#ifndef LICENSEUTILITY_H
#define LICENSEUTILITY_H

//#include "LicenseContext.h"
#include "SharedState.h"

namespace KasperskyLicensing {
namespace Implementation {

/**
 * Implements ILicenseUtility interface.
 */
class LicenseUtility : public ILicenseUtility
{
public:

	/**
	 * Constructs license utility object.
	 */
	LicenseUtility(
		IStorageRW& keyStorage, 
		LicenseKeyspace& keySpace, 
		const ILicenseContext& context
	);

	/**
	 * Returns license object interface.
	 */
	virtual const ILicenseContext& GetLicenseContext() const;

	/**
	 * Returns license object interface.
	 */
	virtual const ILicenseObject& GetLicenseObject() const;

	/**
	 * Installs license key.
	 */
	virtual const ILicenseKey& 
		InstallLicenseKey(const char_t* name, const void* data, size_t size);

	/**
	 * Uninstalls license key.
	 */
	virtual void UninstallLicenseKey(const LicenseKeyInfo::SerialNumber& sn);

	/**
	 * Validates license key and returns its status.
	 */
	virtual LicenseKeyStatus GetLicenseKeyStatus(const ILicenseKey& key) const;

	/**
	 * Returns installed keys collection.
	 */
	virtual KeyEnumerator GetInstalledKeys();

private:

	// disable copy-construction 
	LicenseUtility(const LicenseUtility&);

	// disable assignment 
	LicenseUtility& operator=(const LicenseUtility&);

	// reference license keyspace
	LicenseKeyspace& key_space;

	// reference keys storage
	IStorageRW& key_storage;

	// current license context
	LicenseContext lic_context;
};

} // namespace Implementation
} // namespace KasperskyLicensing

#endif // LICENSEUTILITY_H
