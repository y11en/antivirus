#include "precompiled.h"
#include "strconv.hpp"
#include "LicenseKeyPid.h"
#include "Verdict.h"
#include "datatree.h"
#include "LicenseRestriction.h"

namespace KasperskyLicensing {
namespace Implementation {

//-----------------------------------------------------------------------------

LicenseRestriction::LicenseRestriction(
	const data_tree::node& data, 
	const LicenseEnvironment& env
) : lic_env(env)
{
	// fill common attributes
	id = GET_AVP_PID_ID(data.id());
	data.get_child(AVP_PID_KEY_RESTRICTION_NAME).get_value().as_string(name);
	nom_value = data.get_child(AVP_PID_KEY_RESTRICTION_VALUE).get_value().as_uint();
	std::basic_string<char_t> s;
	data.get_child(AVP_PID_KEY_ADDITIVE_CONDITION).get_value().as_string(s);
	additive_expr.parse(conv::string_conv<char>(s));
	default_notif_id = data.get_child(AVP_PID_KEY_RESTRICTION_DEFAULT_NOTIFY).get_value().as_uint();

	// fill license condition list
	data_tree::node::child_enumerator conditions = 
		data.get_child(AVP_PID_KEY_CONDITION_LIST).get_children();
	for(; !conditions.IsDone(); conditions.Next())
	{
		AutoPointer<LicenseCondition> cond(new LicenseCondition(conditions.Item()));
		cond_list.push_back(cond.GetPointer());
		cond.Release();
	}
}

//-----------------------------------------------------------------------------

unsigned LicenseRestriction::GetId() const
{
	return id;
}

//-----------------------------------------------------------------------------

unsigned LicenseRestriction::GetSequenceNo() const
{
	return lic_env.GetSequenceNo();
}

//-----------------------------------------------------------------------------

const char_t* LicenseRestriction::GetName() const
{
	return name.c_str();
}

//-----------------------------------------------------------------------------

AutoPointer<IVerdict> LicenseRestriction::Examine(unsigned value) const
{
	return Examine(time(0), nom_value, value);
}
//-----------------------------------------------------------------------------

AutoPointer<IVerdict> LicenseRestriction::Examine(time_t time, unsigned value) const
{
	return Examine(time, nom_value, value);
}

//-----------------------------------------------------------------------------

AutoPointer<IVerdict> LicenseRestriction::Examine(time_t time, unsigned nominal, unsigned value) const
{
	lic_cond_list_t::const_iterator
		it	= cond_list.begin(),
		end	= cond_list.end();

	calculation_context context = { 0 };
	// set environment for calculation
	lic_env.FillContext(context, time, nominal, value);

	try
	{
		// successively evaluate conditions until it evaluates to true
		for (; it != end && !(*it)->Evaluate(context); ++it);
	}
	// division by zero
	catch(std::overflow_error)
	{
		throw LicensingError(ILicensingError::LICERR_OWERFLOW);
	}

	// no conditions evaluated to true
	if (it == end)
		// return negative verdict with default notification
		return new Verdict(false, default_notif_id);

	// otherwise return positive verdict with notification id obtained from condition
	return new Verdict(true, (*it)->GetNotificationId());
}

//-----------------------------------------------------------------------------

unsigned LicenseRestriction::GetNominalValue() const
{
	return nom_value;
}

//-----------------------------------------------------------------------------

bool LicenseRestriction::IsAdditive(time_t time, unsigned value) const
{
	calculation_context context = { 0 };
	lic_env.FillContext(context, time, nom_value, value);
	return additive_expr.evaluate(context) != 0;
}

//-----------------------------------------------------------------------------

} // namespace Implementation
} // namespace KasperskyLicensing
