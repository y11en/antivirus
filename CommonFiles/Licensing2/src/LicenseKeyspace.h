/** 
 * @file
 * @brief	Implementation of license key space.
 * @author	Andrey Guzhov
 * @date	18.07.2005
 * @version	1.0
 */

#ifndef LICENSEKEYSPACE_H
#define LICENSEKEYSPACE_H

#include "LicenseKey.h"
#include "LicenseObjectWrapper.h"
#include "SignatureVerifier.h"


namespace KasperskyLicensing {
namespace Implementation {

/**
 * A license key space abstraction.
 * The license key space manages combined license object and installed key list.
 * Combined license object is an object which combines all license objects from 
 * all installed license keys. License key space provides methods to access and 
 * manage combined license object and installed key list.
 */
class LicenseKeyspace
{
public:

	/**
	 * Constructs object.
	 */
	LicenseKeyspace(const ProgramClientInfo clientInfo, const char_t* dskmPath);

	/**
	 * Returns license key by given serial number and context.
	 */
	const ILicenseKey& 
		GetLicenseKey(const LicenseKeyInfo::SerialNumber& sn, const ILicenseContext& context) const;

	/**
	 * Returns license key list.
	 */
	ILicenseUtility::KeyEnumerator GetLicenseKeys(const ILicenseContext& context) const;

	/**
	 * Returns license object for given license context.
	 */
	const ILicenseObject& GetLicenseObject(const ILicenseContext& context) const;

	/**
	 * Adds specified license key.
	 */
	const ILicenseKey& AddLicenseKey(const IObjectImage& image, const ILicenseContext& context);

	/**
	 * Removes specified license key.
	 */
	void RemoveLicenseKey(const LicenseKeyInfo::SerialNumber& sn);

	/**
	 * Parses license key.
	 */
	AutoPointer<LicenseKey> ParseLicenseKey(const const IObjectImage& image) const;

	/**
	 * Validates license key and returns its status.
	 */
	ILicenseUtility::LicenseKeyStatus 
		GetLicenseKeyStatus(const ILicenseKey& key, const ILicenseContext& context) const;

	/**
	 * Returns control database object.
	 */
	ControlDatabase& GetControlDatabase();

private:
	typedef pointainer<std::list<LicenseKey*> > key_list_t;

	// disable copy-construction
	LicenseKeyspace(const LicenseKeyspace&);

	// disable assignment
	LicenseKeyspace& operator=(const LicenseKeyspace&);

	// finds license key by given serial number
	key_list_t::const_iterator FindLicenseKey(const LicenseKeyInfo::SerialNumber& sn) const;

	// list of license keys
	key_list_t key_list;

	// root license object
	AutoPointer<LicenseObjectWrapper> lic_object;

	// program client information
	ProgramClientInfo client_info;

	// control database
	ControlDatabase	control_db;

	// signature verifier
	SignatureVerifier verifier;
};

} // namespace Implementation
} // namespace KasperskyLicensing

#endif //LICENSEKEYSPACE_H
