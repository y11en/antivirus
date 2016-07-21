#include "precompiled.h"
#include "strconv.hpp"
#include "LicenseKeyPid.h"
#include "LicenseCondition.h"

namespace KasperskyLicensing {
namespace Implementation {

//-----------------------------------------------------------------------------

LicenseCondition::LicenseCondition(const data_tree::node& data)
{
	std::basic_string<char_t> s;
	data.get_child(AVP_PID_KEY_CONDITION_EXPRESSION).get_value().as_string(s);
	lic_expression.parse(conv::string_conv<char>(s));
	notif_id = data.get_child(AVP_PID_KEY_CONDITION_NOTIFY).get_value().as_uint();
}

//-----------------------------------------------------------------------------
	
bool LicenseCondition::Evaluate(const calculation_context& context) const
{
	return lic_expression.evaluate(context) != 0;
}

//-----------------------------------------------------------------------------

unsigned LicenseCondition::GetNotificationId() const
{
	return notif_id;
}

//-----------------------------------------------------------------------------

} // namespace Implementation
} // namespace KasperskyLicensing
