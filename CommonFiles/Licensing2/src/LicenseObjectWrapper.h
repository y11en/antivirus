#ifndef LICENSEOBJECTWRAPPER_H
#define LICENSEOBJECTWRAPPER_H

#include "LicenseObject.h"
#include "LicenseRestrictionWrapper.h"

namespace KasperskyLicensing {
namespace Implementation {
	
class LicenseObjectWrapper : public ILicenseObject
{
public:
	/**
	 * Constructs wrapper object.
	 */
	LicenseObjectWrapper(const LicenseObject& obj, unsigned platform);

	/**
	 * Allows clients to query license restriction by given id.
	 */
	virtual const ILicenseRestriction& GetRestriction(unsigned id) const;

	/**
	 * Creates restriction enumerator.
	 */
	virtual RestrictionEnumerator GetRestrictions() const;

	/**
	 * Creates license object enumerator.
	 */
	virtual LicenseObjectEnumerator GetLicenseObjects() const;

	/**
	 * Creates supported platform enumerator.
	 */
	virtual PlatformEnumerator GetSupportedPlatforms() const;

	/**
	 * Returns license object identifier.
	 */
	virtual const LicenseObjectId& GetObjectId() const;

	/**
	 * Returns name of license object.
	 */
	virtual const char_t* GetName() const;

	/**
	 * Merges license object and assigns key identifier.
	 */
	void Merge(const LicenseObject& obj);

	/**
	 * Subtracts license object.
	 */
	void Subtract(const LicenseObject& obj);

private:

	// merges license restriction
	void MergeRestriction(const LicenseRestriction& r);

	// merges license object
	void MergeLicenseObject(const LicenseObject& o);

	// subtracts license restriction
	void SubtractRestriction(const LicenseRestriction& r);

	// subtracts license object
	void SubtractLicenseObject(const LicenseObject& o);

	// returns reference count
	unsigned GetReferenceCount() const;

	// disable copying
	LicenseObjectWrapper(const LicenseObjectWrapper&);
	
	// disable assignment
	LicenseObjectWrapper& operator=(const LicenseObjectWrapper&);

	// license restriction list type
	typedef pointainer<std::list<LicenseRestrictionWrapper*> > lic_restr_list_t;

	// license object list type
	typedef pointainer<std::list<LicenseObjectWrapper*> > lic_object_list_t;

	// supported platform list type
	typedef std::list<PlatformType> platform_list_t;

	// license object identifier
	LicenseObjectId lic_object_id;

	// license object name
	std::basic_string<char_t> lic_object_name;

	// license restriction list
	lic_restr_list_t restr_list;

	// supported platform list
	platform_list_t platform_list;

	// license object list
	lic_object_list_t lic_object_list;

	// target platform identifier
	unsigned target_platform_id;

	// license object reference counter
	unsigned ref_count;
};

} // namespace Implementation
} // namespace KasperskyLicensing

#endif // LICENSEOBJECTWRAPPER_H