#include "precompiled.h"
#include "EnumeratorImp.h"
#include "Verdict.h"

namespace KasperskyLicensing {
namespace Implementation {

//-----------------------------------------------------------------------------

Verdict::Verdict(bool value) : verdict_value(value)
{
}

//-----------------------------------------------------------------------------

Verdict::Verdict(bool value, unsigned notif_id) : verdict_value(value)
{
	AddNotification(notif_id);
}

//-----------------------------------------------------------------------------

bool Verdict::IsPositive() const
{
	return verdict_value;
}

//-----------------------------------------------------------------------------

IVerdict::NotificationEnumerator Verdict::GetNotifications() const
{
	return new EnumeratorImp<
		NotificationEnumerator, 
		std::vector<unsigned>::const_iterator
	>(notif_list.begin(), notif_list.end());
}

//-----------------------------------------------------------------------------

void Verdict::AddNotification(unsigned id)
{
	// ignore zero identifiers
	if (id)
	{
		std::vector<unsigned>::const_iterator it = 
			std::find(notif_list.begin(), notif_list.end(), id);
		if (it == notif_list.end())
			notif_list.push_back(id);
	}
}

//-----------------------------------------------------------------------------

} // namespace Implementation
} // namespace KasperskyLicensing
