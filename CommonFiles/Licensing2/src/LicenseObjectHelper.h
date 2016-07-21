#ifndef LICENSEOBJECTHELPER_H
#define LICENSEOBJECTHELPER_H

namespace KasperskyLicensing {
namespace Implementation {

class LicenseObjectHelper
{
public:
	LicenseObjectHelper(const ILicenseObject& obj);

	bool IsPlatformSupported(unsigned platform_id) const;

	ILicenseObject::LicenseObjectEnumerator 
		FindLicenseObject(const ILicenseContext& context, unsigned platform_id) const;

	bool ContainsLicenseObject(const ILicenseContext& context, unsigned platform_id) const;

private:

	// recursively finds license object
	ILicenseObject::LicenseObjectEnumerator 
		FindLicenseObject(ILicenseContext::ItemEnumerator& e, unsigned platform_id) const;

	const ILicenseObject& lic_object;
};

bool operator == (const LicenseObjectId& lhs, const LicenseObjectId& rhs);

} // namespace Implementation
} // namespace KasperskyLicensing

#endif // LICENSEOBJECTHELPER_H