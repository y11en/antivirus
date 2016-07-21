#ifndef LICENSERESTRICTIONWRAPPER_H
#define LICENSERESTRICTIONWRAPPER_H

#include "LicenseRestriction.h"

namespace KasperskyLicensing {
namespace Implementation {

class LicenseRestrictionWrapper : public ILicenseRestriction
{
public:

	/**
	 * Constructs wrapper object by given real object.
	 */
	LicenseRestrictionWrapper(const LicenseRestriction& r);

	/**
	 * Returns license restriction identifier.
	 */
	virtual unsigned GetId() const;

	/**
	 * Returns name of license restriction.
	 */
	virtual const char_t* GetName() const;

	/**
	 * Examines license restriction.
	 */
	virtual AutoPointer<IVerdict> Examine(unsigned value) const;

	/**
	 * Returns true if wrapper has no restrictions.
	 */
	bool IsEmpty() const;
	
	/**
	 * Merges license restriction.
	 */
	void Merge(const LicenseRestriction& r);

	/**
	 * Subtracts license restriction.
	 */
	void Subtract(const LicenseRestriction& r);

private:

	typedef std::map<unsigned, const LicenseRestriction*> lic_restr_map_t;

	// restriction identifier
	unsigned id;

	// name of restriction
	std::basic_string<char_t> name;

	// license restrictions map
	lic_restr_map_t restr_map;

};

} // namespace Implementation 
} // namespace KasperskyLicensing

#endif // LICENSERESTRICTIONWRAPPER_H