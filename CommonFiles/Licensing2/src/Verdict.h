/** 
 * @file
 * @brief	Implementaion of IVerdict interface.
 * @author	Andrey Guzhov
 * @date	12.07.2005
 * @version	1.0
 */

#ifndef VERDICT_H
#define VERDICT_H

#include <vector>

namespace KasperskyLicensing {
namespace Implementation {

/**
 * Implements IVerdict interface.
 */
class Verdict : public IVerdict
{
public:

	/**
	 * Constructs verdict object by given verdict value.
	 * @param[in] value verdict value.
	 */
	explicit Verdict(bool value);

	/**
	 * Constructs verdict object by given verdict value and notification id.
	 * @param[in] value verdict value.
	 * @param[in] notif_id notification identifier.
	 */
	Verdict(bool value, unsigned notif_id);

	/**
	 * Returns true if corresponding license restriction is satisfied.
	 */
	virtual bool IsPositive() const;

	/**
	 * Returns notification enumerator.
	 */
	virtual NotificationEnumerator GetNotifications() const;

	/**
	 * Adds notification identifier.
	 */
	void AddNotification(unsigned id);

private:
	
	// verdict value
	bool verdict_value;

	// notifications list
	std::vector<unsigned> notif_list;
};

} // namespace Implementation
} // namespace KasperskyLicensing

#endif // VERDICT_H
