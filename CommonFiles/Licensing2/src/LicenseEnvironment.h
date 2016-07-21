#ifndef LICENSEENVIRONMENT_H
#define LICENSEENVIRONMENT_H

namespace KasperskyLicensing {
namespace Implementation {

class LicenseEnvironment
{
public:

	virtual void FillContext(
		calculation_context& context, 
		time_t time, 
		unsigned nominal, 
		unsigned value
	) const = 0;

	virtual unsigned GetSequenceNo() const = 0;
};

} // namespace Implementation
} // namespace KasperskyLicensing

#endif // LICENSEENVIRONMENT_H