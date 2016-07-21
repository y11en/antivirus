/** 
 * @file
 * @brief	Implementation of license condition.
 * @author	Andrey Guzhov
 * @date	11.07.2005
 * @version	1.0
 */

#ifndef LICENSECONDITION_H
#define LICENSECONDITION_H

#include "datatree.h"
#include "expression.h"

namespace KasperskyLicensing {
namespace Implementation {

/**
 * Implements license condition abstraction.
 */
class LicenseCondition
{
public:

	LicenseCondition(const data_tree::node& data);

	/**
	 * Evaluates license condition.
	 */
	bool Evaluate(const calculation_context& context) const;

	/**
	 * Returns notification identifier.
	 */
	unsigned GetNotificationId() const;

private:

	// notification identifier
	unsigned notif_id;

	// expression of license condition 
	expression lic_expression;
};

} // namespace Implementation
} // namespace KasperskyLicensing

#endif //LICENSECONDITION_H
