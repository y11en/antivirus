/** 
 * @file
 * @brief	Implementation of ILicenseKey interface.
 * @author	Andrey Guzhov
 * @date	15.07.2005
 * @version	1.0
 */

#ifndef LICENSEKEY_H
#define LICENSEKEY_H

#include "LicenseObject.h"
#include "ControlDatabase.h"

namespace KasperskyLicensing {
namespace Implementation {

/**
 * Implements ILicenseKey interface
 */
class LicenseKey : public ILicenseKey, private LicenseEnvironment
{
public:

	/**
	 * Constructs license key.
	 */
	LicenseKey(const IObjectImage& obj, const ControlDatabase& cdb);

	/**
	 * Returns license key information.
	 */
	virtual const LicenseKeyInfo& GetKeyInfo() const;

	/**
	 * Returns root license object of license key.
	 */
	virtual const ILicenseObject& GetLicenseObject() const;

	/**
	 * Returns name of license key.
	 */
	virtual const char_t* GetName() const;

	/**
	 * Returns root license object.
	 */
	virtual const LicenseObject& GetLicenseRoot() const;

private:

	// string array type
	typedef std::vector<std::basic_string<char_t> > string_array_t;

	// set computation environment
	virtual void FillContext(
		calculation_context& context, 
		time_t time, 
		unsigned nominal, 
		unsigned value
	) const;

	// returns sequence number of license key
	virtual unsigned GetSequenceNo() const;
	
	// fills contact information
	void FillContactInfo(
		const data_tree::node& data, 
		LicenseKeyInfo::PersonContact& contact, 
		string_array_t& array
	);

	// disable copying
	LicenseKey(const LicenseKey&);
	
	// disable assignment
	LicenseKey& operator=(const LicenseKey&);

	// root license object
	AutoPointer<LicenseObject>	lic_object;

	// generic license key information
	LicenseKeyInfo	key_info;

	// license key name
	std::basic_string<char_t> key_name;

	// string arrays stores contact data
	string_array_t	distributor,
					reseller,
					local_office,
					head_quarters,
					support_centre;

	const ControlDatabase& control_db;
};

} // namespace Implementation
} // namespace KasperskyLicensing

#endif //LICENSEKEY_H
