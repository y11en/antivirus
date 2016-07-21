/** 
 * @file
 * @brief	Implementation of ILicenseObject interface.
 * @author	Andrey Guzhov
 * @date	11.07.2005
 * @version	1.0
 */

#ifndef LICENSEOBJECT_H
#define LICENSEOBJECT_H

#include "LicenseRestriction.h"

namespace KasperskyLicensing {
namespace Implementation {

/**
 * Implements ILicenseObject interface.
 */
class LicenseObject : public ILicenseObject
{
public:

	// license restriction list type
	typedef pointainer<std::vector<LicenseRestriction*> > RestrictionList;

	// license object list type
	typedef pointainer<std::vector<LicenseObject*> > LicenseObjectList;

	/**
	 * Constructs object using data tree.
	 */
	explicit LicenseObject(const data_tree::node& data, const LicenseEnvironment& env);

	/**
	 * Allows clients to query license restriction by given id.
	 */
	virtual const ILicenseRestriction& GetRestriction(unsigned id) const;

	/**
	 * Creates restriction iterator.
	 */
	virtual RestrictionEnumerator GetRestrictions() const;

	/**
	 * Creates license object iterator.
	 */
	virtual LicenseObjectEnumerator GetLicenseObjects() const;

	/**
	 * Creates supported platform iterator.
	 */
	virtual PlatformEnumerator GetSupportedPlatforms() const;

	/**
	 * Returns license object information.
	 */
	virtual const LicenseObjectId& GetObjectId() const;

	/**
	 * Returns name of license object.
	 */
	virtual const char_t* GetName() const;

	const RestrictionList& GetRestrictionList() const;

	const LicenseObjectList& GetLicenseObjectList() const;

private:

	// disable copying
	LicenseObject(const LicenseObject&);
	
	// disable assignment
	LicenseObject& operator=(const LicenseObject&);

	// supported platform list type
	typedef std::vector<PlatformType> platform_list_t;

	// license object identifier
	LicenseObjectId lic_object_id;

	// license object name
	std::basic_string<char_t> lic_object_name;

	// license restriction list
	RestrictionList restr_list;

	// license object list
	LicenseObjectList lic_object_list;

	// supported platform list
	platform_list_t platform_list;
};

} // namespace Implementation
} // namespace KasperskyLicensing

#endif //LICENSEOBJECT_H
