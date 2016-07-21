/** 
 * @file
 * @brief	Implementation of ILicenseContext interface.
 * @author	Andrey Guzhov
 * @date	08.07.2005
 * @version	1.0
 */

#ifndef LICENSECONTEXT_H
#define LICENSECONTEXT_H

#include <vector>

namespace KasperskyLicensing {
namespace Implementation {

/**
 * Implements ILicenseContext interface.
 */
class LicenseContext : public ILicenseContext
{
public:

	/**
	 * Constructs empty license context.
	 */
	LicenseContext();

	/**
	 * Constructs copy of specified license context.
	 */
	LicenseContext(const ILicenseContext& rhs);

	/**
	 * Adds item to license context.
	 */
	virtual void AddItem(const LicenseObjectId& item);

	/**
	 * Returns items enumerator.
	 */
	virtual ItemEnumerator GetItems() const;

	/**
	 * Returns true if license context is empty.
	 */
	virtual bool IsEmpty() const;

	/**
	 * Clears contents of the license context.
	 */
	virtual void Clear();

private:

	// disable assignment
	LicenseContext& operator=(const LicenseContext&);

	// license context type
	typedef std::vector<LicenseObjectId> context_t;

	// stores license context
	context_t	context;
};

} // namespace Implementation
} // namespace KasperskyLicensing

#endif //LICENSECONTEXT_H
