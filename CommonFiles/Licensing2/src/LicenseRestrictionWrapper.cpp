#include "precompiled.h"
#include "Verdict.h"
#include "LicenseRestrictionWrapper.h"

namespace KasperskyLicensing {
namespace Implementation {

//-----------------------------------------------------------------------------

LicenseRestrictionWrapper::LicenseRestrictionWrapper(const LicenseRestriction& r)
	: id(r.GetId()), name(r.GetName())
{
	Merge(r);
}

//-----------------------------------------------------------------------------

void LicenseRestrictionWrapper::Merge(const LicenseRestriction& r)
{
	restr_map.insert(std::make_pair(r.GetSequenceNo(), &r));
}

//-----------------------------------------------------------------------------

void LicenseRestrictionWrapper::Subtract(const LicenseRestriction& r)
{
	lic_restr_map_t::iterator it = restr_map.find(r.GetSequenceNo());
	// restriction is not found
	if (it == restr_map.end())
		throw LicensingError(LicensingError::LICERR_NOT_FOUND);
	// erase element
	restr_map.erase(it);
}

//-----------------------------------------------------------------------------

bool LicenseRestrictionWrapper::IsEmpty() const
{
	return restr_map.empty();
}

//-----------------------------------------------------------------------------

unsigned LicenseRestrictionWrapper::GetId() const
{
	return id;
}

//-----------------------------------------------------------------------------

const char_t* LicenseRestrictionWrapper::GetName() const
{
	return name.c_str();
}

//-----------------------------------------------------------------------------

AutoPointer<IVerdict> LicenseRestrictionWrapper::Examine(unsigned value) const
{
	// operation time
	time_t cur_time = time(0);
	// additive value
	unsigned additive_val = 0;
	// calculate additive value
	lic_restr_map_t::const_iterator it(restr_map.begin()), end(restr_map.end());
	// accumulate value
	for(; it != end; ++it)
	{
		if (it->second->IsAdditive(cur_time, value))
			additive_val += it->second->GetNominalValue();
	}
	// total verdict
	bool total_verdict = false;
	// notifications array
	std::vector<unsigned> notif;
	// calculate total verdict
	for (it = restr_map.begin(); it != end; ++it)
	{	
		AutoPointer<IVerdict> verdict;

		// if restriction is additive
		if (it->second->IsAdditive(cur_time, value))
			// use additive value to calculate verdict
			verdict = it->second->Examine(cur_time, additive_val, value);
		else
			// use nominal value of license restriction
			verdict = it->second->Examine(cur_time, value);

		IVerdict::NotificationEnumerator ne = verdict->GetNotifications();

		// current verdict is positive
		if (verdict->IsPositive())
		{
			// if total verdict is positive
			if (total_verdict)
				// add notification to array
				ne.IsDone() ? void() : notif.push_back(ne.Item());
			else
			{
				// set positive total verdict
				total_verdict = true;
				// clear all previous notifications
				notif.clear();
				// add current notification
				ne.IsDone() ? void() : notif.push_back(ne.Item());
			}
		}
		// current verdict is negative
		else
		{
			// only for negative total verdict
			if (!total_verdict)
				// accumulate notifications
				ne.IsDone() ? void() : notif.push_back(ne.Item());
		}
	}

	// form result
	AutoPointer<Verdict> result(new Verdict(total_verdict));
	// copy notifications
	for (std::vector<unsigned>::const_iterator jt = notif.begin(); jt != notif.end(); ++jt)
		result->AddNotification(*jt);

	return result.Release();
}

//-----------------------------------------------------------------------------

} // namespace Implementation 
} // namespace KasperskyLicensing
