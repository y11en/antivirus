#ifndef SHAREDSTATE_H
#define SHAREDSTATE_H

namespace KasperskyLicensing {
namespace Implementation {

#include "ControlDatabase.h"
#include "LicenseKeyspace.h"

class SharedState
{
public:

	/**
	 * Returns control database.
	 */
	virtual const ControlDatabase& GetControlDatabase() const = 0;

	/**
	 * Returns information about program client.
	 */
	virtual const ProgramClientInfo& GetProgramClientInfo() const = 0;

	/**
	 * Returns license key storage.
	 */
	virtual IStorageRW& GetKeyStorage() = 0;

	/**
	 * Returns license key space.
	 */
	virtual LicenseKeyspace& GetLicenseKeyspace() = 0;

	/**
	 * Destroys object.
	 */
	virtual ~SharedState() {}

};

} // namespace Implementation
} // namespace KasperskyLicensing

#endif // SHAREDSTATE_H