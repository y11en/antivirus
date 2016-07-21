#include "precompiled.h"
#include "EnumeratorImp.h"
#include "LicenseContext.h"

namespace KasperskyLicensing {
namespace Implementation {

//-----------------------------------------------------------------------------

LicenseContext::LicenseContext()
{
}

//-----------------------------------------------------------------------------

LicenseContext::LicenseContext(const ILicenseContext& rhs)
{
	// perform deep copying of license context
	for (ItemEnumerator e = rhs.GetItems(); !e.IsDone(); e.Next())
		context.push_back(e.Item());
}

//-----------------------------------------------------------------------------

void LicenseContext::AddItem(const LicenseObjectId& item)
{
	context.push_back(item);
}

//-----------------------------------------------------------------------------

ILicenseContext::ItemEnumerator LicenseContext::GetItems() const
{
	typedef EnumeratorImp<ItemEnumerator, context_t::const_iterator> enumerator_t;
	return new enumerator_t(context.begin(), context.end());
}

//-----------------------------------------------------------------------------

void LicenseContext::Clear()
{
	context.clear();
}

//-----------------------------------------------------------------------------

bool LicenseContext::IsEmpty() const
{
	return context.empty();
}

//-----------------------------------------------------------------------------


} // namespace Implementation
} // namespace KasperskyLicensing
