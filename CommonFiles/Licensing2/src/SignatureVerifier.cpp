#include "precompiled.h"
#include "strconv.hpp"
#include "SignatureVerifier.h"


namespace KasperskyLicensing {
namespace Implementation {

//-----------------------------------------------------------------------------

SignatureVerifier::SignatureVerifier(const char_t* dskmPath) : dskm_path(dskmPath)
{
}

//-----------------------------------------------------------------------------

bool SignatureVerifier::VerifyKeySignature(const IObjectImage& image) const
{
	return VerifySignature(image, OBJ_TYPE_LICKEY);
}

//-----------------------------------------------------------------------------

bool SignatureVerifier::VerifyBaseSignature(const IObjectImage& image) const
{
	return VerifySignature(image, OBJ_TYPE_AVDB);
}

//-----------------------------------------------------------------------------

bool SignatureVerifier::VerifySignature(const IObjectImage& image, ObjectType otype) const
{
	HDSKMLIST hList = 0;
	// create parameter list
	AVP_dword err = DSKM_ParList_Create(&hList);
	// add object image
	if (DSKM_OK(err))
	{
		DSKM_ParList_AddBufferedObject(
			hList, 
			0, 
			const_cast<void*>(image.GetObjectImage()), 
			image.GetObjectSize(), 
			0, 
			0
		);
	}
	if (DSKM_NOT_OK(err))
	{
		if (hList)
			DSKM_ParList_Delete(hList);

		throw LicensingError(ILicensingError::LICERR_SIGNLIB_ERROR, err);
	}

	// perform signature verification
	switch(otype)
	{
	case OBJ_TYPE_LICKEY:
		err = DSKM_CheckObjectsUsingInsideSignAndKeysFolder(
			lib_dskm.GetHandle(),
			hList,
			DSKM_OTYPE_LICENCE_KEY, 
			conv::string_conv<char>(dskm_path).c_str()
		);
		break;
	case OBJ_TYPE_AVDB:
		err = DSKM_CheckObjectsUsingHashRegsFolder(
			lib_dskm.GetHandle(),
			hList,
			32759, // TODO: define constant for AV bases
			conv::string_conv<char>(dskm_path).c_str()
		);
		break;
	default:
		assert(false);
		break;
	}

	DSKM_ParList_Delete(hList);

	return DSKM_OK(err);
}		

//-----------------------------------------------------------------------------

SignatureVerifier::DskmLibrary SignatureVerifier::lib_dskm;

//-----------------------------------------------------------------------------

SignatureVerifier::DskmLibrary::DskmLibrary()
{
	hDskm = DSKM_InitLibrary(malloc, free, true);
}

//-----------------------------------------------------------------------------

HDSKM SignatureVerifier::DskmLibrary::GetHandle() const
{
	return hDskm;
}

//-----------------------------------------------------------------------------

SignatureVerifier::DskmLibrary::~DskmLibrary()
{
	if (hDskm)
		DSKM_DeInitLibrary(hDskm, false);
}

//-----------------------------------------------------------------------------

} // namespace Implementation
} // namespace KasperskyLicensing
