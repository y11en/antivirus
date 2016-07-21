#define IMPEX_CURRENT_HEADER  "e_string.h"
#include <Prague/iface/impexhlp.h>
	
#if !defined(__e_string_h) || defined(IMPEX_TABLE_CONTENT)
#if ( IMPEX_INTERNAL_MODE == IMPEX_INTERNAL_MODE_DECLARE )
#define __e_string_h
	
	// Extrnal includes should be placed here. For ex:
	// #include <windows.h>
	
#endif
	
	IMPEX_BEGIN
		IMPEX_NAME_ID( ECLSID_STRING, 0x8eefa2b8l, tERROR, CalcExportLen,  (tPTR str, tDWORD str_len, tCODEPAGE str_cp, tCODEPAGE export_cp, tDWORD export_flags, tDWORD* export_length) )
		IMPEX_NAME_ID( ECLSID_STRING, 0x10a92e27l, tERROR, CopyTo,         (tPTR dst, tDWORD dst_len, tCODEPAGE dst_cp, tPTR src, tDWORD src_len, tCODEPAGE src_cp, tDWORD flags, tDWORD* outlen) )
		IMPEX_NAME_ID( ECLSID_STRING, 0xab7270acl, tERROR, ConvertLen,     (tDWORD len, tDWORD* out_len, tCODEPAGE src_cp, tCODEPAGE dst_cp) )
    IMPEX_NAME_ID( ECLSID_STRING, 0xed035ef3l, tERROR, ImportFromProp, (hOBJECT from, tPROPID propid, tDWORD* result, tSTRING buff, tDWORD size, tCODEPAGE receive_cp) )
  IMPEX_END
	
#endif
