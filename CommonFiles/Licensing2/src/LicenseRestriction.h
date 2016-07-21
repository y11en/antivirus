/** 
 * @file
 * @brief	Implementation of ILicenseRestriction interface.
 * @author	Andrey Guzhov
 * @date	11.07.2005
 * @version	1.0
 */

#ifndef LICENSERESTRICTION_H
#define LICENSERESTRICTION_H

#include "datatree.h"
#include "LicenseCondition.h"
#include "LicenseEnvironment.h"

namespace KasperskyLicensing {
namespace Implementation {
	
/**
 * Implements ILicenseRestriction interface.
 */
class LicenseRestriction : public ILicenseRestriction
{
public:

	/**
	 * Constructs license restriction object.
	 */
	LicenseRestriction(const data_tree::node& data, const LicenseEnvironment& env);

	/**
	 * Returns license restriction identifier.
	 */
	virtual unsigned GetId() const;

	/**
	 * Returns license restriction name.
	 */
	virtual const char_t* GetName() const;

	/**
	 * Examines license restriction.
	 */
	virtual AutoPointer<IVerdict> Examine(unsigned value) const;

	/**
	 * Examines license restriction.
	 */
	AutoPointer<IVerdict> Examine(time_t time, unsigned value) const;

	/**
	 * Examines license restriction.
	 */
	AutoPointer<IVerdict> Examine(time_t time, unsigned nominal, unsigned value) const;

	/**
	 * Returns true if restriction is additive.
	 */
	bool IsAdditive(time_t time, unsigned value) const;

	/**
	 * Returns nominal value of license restriction.
	 */
	unsigned GetNominalValue() const;

	/**
	 * Returns sequence number of the owning license key.
	 */
	unsigned GetSequenceNo() const;

private:

	// disable copying
	LicenseRestriction(const LicenseRestriction&);
	
	// disable assignment
	LicenseRestriction& operator=(const LicenseRestriction&);

	// license conditions list type
	typedef pointainer<std::vector<LicenseCondition*> >lic_cond_list_t;

	// restriction identifier
	unsigned id;

	// restriction name
	std::basic_string<char_t> name;

	// additive expression
	expression additive_expr;

	// nominal value of restriction
	unsigned nom_value;

	// default notification id
	unsigned default_notif_id;

	// license conditions list
	lic_cond_list_t cond_list;
	
	// calculation context
	const LicenseEnvironment& lic_env;
};

} // namespace Implementation
} // namespace KasperskyLicensing

#endif //LICENSERESTRICTION_H
