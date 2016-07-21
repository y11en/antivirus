/*
*/

#if !defined(__KL_DSKMWRAP_EXCEPTIONS_H)
#define __KL_DSKMWRAP_EXCEPTIONS_H

#include <string>
#include <dskm/dskm.h>

namespace KL  {
namespace DskmLibWrapper  {

//////////////////////////////////////////////////////////////////////////
class DskmLibWrapperException
{
public:
	enum Reason
	{
		DskmLibInitFailed = 1,
		OutOfMemory,
		NotImplemented,
		InvalidArgument,
		InvalidValueCast
	};
	DskmLibWrapperException(Reason reason) : m_reason(reason)  {}
	Reason GetReason() const  { return m_reason; }
	std::string GetReasonDesc() const  { return GetReasonDesc(m_reason); }
	static std::string GetReasonDesc(Reason reason);
private:
	Reason m_reason;
};

//////////////////////////////////////////////////////////////////////////
inline std::string DskmLibWrapperException::GetReasonDesc(Reason reason)
{
	switch (reason)
	{
		case DskmLibInitFailed	: return "DSKM library initialization failed";
		case OutOfMemory		: return "Memory allocation failed";
		case NotImplemented		: return "Method is not implemented";
		case InvalidArgument	: return "Invalid argument";
		case InvalidValueCast	: return "Invalid value cast";
		default					: return "Unknown error";
	}
}

//////////////////////////////////////////////////////////////////////////
class DskmLibException
{
public:
	DskmLibException(AVP_dword code) : m_code(code)  {}
	AVP_dword GetErrorCode() const  { return m_code; }
	std::string GetErrorCodeDesc() const  { return GetErrorCodeDesc(m_code); }
	static std::string GetErrorCodeDesc(AVP_dword code);
private:
	AVP_dword m_code;
};

//////////////////////////////////////////////////////////////////////////
inline std::string DskmLibException::GetErrorCodeDesc(AVP_dword code)
{
	switch (code)
	{
		case DSKM_ERR_OK						: return "No error";
		case DSKM_ERR_NOT_INITIALIZED			: return "Library is not initialized";
		case DSKM_ERR_IO_NOT_LOADED				: return "IO is not loaded";
		case DSKM_ERR_CANNOT_CREATE_REG_FILE	: return "Can't create registry file";
		case DSKM_ERR_CANNOT_WRITE_REG_FILE		: return "Can't write to registry file";
		case DSKM_ERR_CANNOT_WRITE_REG_TOMEM	: return "Can't write registry to memory";
		case DSKM_ERR_CANNOT_OPEN_REG_FILE		: return "Can't open registry file";
		case DSKM_ERR_CANNOT_READ_REG_FILE		: return "Can't read from registry file";
		case DSKM_ERR_INVALID_REG_FILE			: return "Invalid registry file";
		case DSKM_ERR_INVALID_REG_FILE_VERSION	: return "Invalid registry file version";
		case DSKM_ERR_INVALID_REG_FILE_OBJTYPE	: return "Invalid registry file object type";
		case DSKM_ERR_KEY_NOT_FOUND				: return "Key not found";
		case DSKM_ERR_REG_NOT_FOUND				: return "Registry not found";
		case DSKM_ERR_CANNOT_CALC_OBJ_HASH		: return "Can't calculate object hash";
		case DSKM_ERR_NOT_ALL_OBJECTS_FOUND		: return "Not all objects found";
		case DSKM_ERR_INVALID_SIGN				: return "Invalid sign";
		case DSKM_ERR_INVALID_BUFFER			: return "Invalid buffer";
		case DSKM_ERR_INVALID_PARAMETER			: return "Invalid parameter";
		case DSKM_ERR_INVALID_OBJTYPE			: return "Invalid object type";
		case DSKM_ERR_DATA_PROCESSING			: return "Data processing error";
		case DSKM_ERR_HASH_NOT_FOUND			: return "Hash not found";
		case DSKM_ERR_ANOTHER_BASE_REG_FOUND	: return "Another base registry was found";
		case DSKM_ERR_LIB_CODE_HACKED			: return "Library code was hacked";
		case DSKM_ERR_CANNOT_CREATE_SIGN		: return "Can't create sign";
		case DSKM_ERR_CRIPTO_LIB				: return "Cripto library error";
		case DSKM_UNKNOWN_ERROR					: return "Unknown error";
		default									: return "Error is not supported by DskmLibWrapper";
	}
}

}  // namespace DSKM
}  // namespace KL

#endif  // !defined(__KL_DSKMWRAP_EXCEPTIONS_H)
