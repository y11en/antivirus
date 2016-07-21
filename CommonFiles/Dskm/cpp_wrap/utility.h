/*
*/

#if !defined(__KL_DSKMWRAP_UTILITY_H)
#define __KL_DSKMWRAP_UTILITY_H

namespace KL  {
namespace DskmLibWrapper  {
namespace Utility  {

//////////////////////////////////////////////////////////////////////////
inline size_t get_string_length(const AVP_char* sz)
{
	return strlen(sz);
}

//////////////////////////////////////////////////////////////////////////
inline size_t get_string_length(const AVP_wchar* sz)
{
	return wcslen(sz);
}

//////////////////////////////////////////////////////////////////////////
inline size_t get_string_buffer_size(const char_t* sz)
{
	return (get_string_length(sz) + 1)*sizeof(char_t);
}

}  // namespace Utility
}  // namespace DSKM
}  // namespace KL

#endif  // !defined(__KL_DSKMWRAP_UTILITY_H)
