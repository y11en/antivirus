#ifndef __prop_util_h
#define __prop_util_h

#include <Property/Property.h>

#if defined( __cplusplus )
extern "C" {
#endif

AVP_bool GetBoolPropVal( HDATA data, AVP_dword* addr, AVP_dword id );
AVP_bool GetDwordPropVal( HDATA data, AVP_dword* addr, AVP_dword id, AVP_dword* val );
AVP_bool GetStrPropVal( HDATA data, AVP_dword* addr, AVP_dword id, char* buff, AVP_dword len, AVP_dword* olen );
AVP_bool GetPropVal( HDATA data, AVP_dword* addr, AVP_dword id, char* buff, AVP_dword len, AVP_dword* olen );

#if defined ( __cplusplus )
}
#endif

#endif
