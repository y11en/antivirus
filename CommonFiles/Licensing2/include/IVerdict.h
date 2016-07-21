/** 
 * @file
 * @brief	Declaration of IVerdict interface.
 * @author	Andrey Guzhov
 * @date	11.07.2005
 * @version	1.0
 */

#ifndef IVERDICT_H
#define IVERDICT_H

namespace KasperskyLicensing {

/** 
 * A verdict abstraction.
 * Verdict is the result of license restriction examination. Using verdict clients 
 * are able to determine whether corresponding license restriction is satisfied or not.
 * @interface
 */
class IVerdict
{
	friend class AutoPointer<IVerdict>;

public:

	/**
	 * Defines notification enumerator type.
	 */
	typedef Enumerator<unsigned> NotificationEnumerator;

	/**
	 * Returns true if corresponding license restriction is satisfied.
	 */
	virtual bool IsPositive() const = 0;

	/**
	 * Returns licensing notifications.
	 * Enumerator provides clients with the information about 
	 * notifications raised while examination of license restriction.
	 */
	virtual NotificationEnumerator GetNotifications() const = 0;

protected:

	/**
	 * Destructor is protected in order to prevent direct use of delete operator. 
	 */
	~IVerdict() {}
	
};

} // namespace KasperskyLicensing

#endif // IVERDICT_H
