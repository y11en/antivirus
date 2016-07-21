#include <LicensingInterface.h>
#include <utility/LicensingError.h>


namespace KasperskyLicensing{

//-----------------------------------------------------------------------------

LicensingError::LicensingError(ErrorCodeType e_code) 
	: error_code(e_code)
	, specific_code(0)
{
}

//-----------------------------------------------------------------------------

LicensingError::LicensingError(ErrorCodeType e_code, int s_code) 
	: error_code(e_code)
	, specific_code(s_code)
{
}

//-----------------------------------------------------------------------------

ILicensingError::ErrorCodeType LicensingError::GetErrorCode() const
{
	return error_code;
}

//-----------------------------------------------------------------------------

int LicensingError::GetSpecificCode() const
{
	return specific_code;
}

//-----------------------------------------------------------------------------

}