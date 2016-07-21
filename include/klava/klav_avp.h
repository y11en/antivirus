// klav_avp.h
//
// Klava-AVP3 interworking structures
//

#ifndef klav_avp_h_INCLUDED
#define klav_avp_h_INCLUDED

#include <kl_types.h>

#ifndef VOID
# define VOID void
 typedef char CHAR;
 typedef int  BOOL;
 typedef long LONG; // for Typedef.h

# ifndef far
#  define far
# endif // far

# include "../../AVP32/Bases/Format/Typedef.h"

#endif

#include "../../AVP32/Bases/Format/Records.h"
#include "../../AVP32/Bases/Format/Base.h"

#include "../../AVP32/Bases/SIZES.h"
#include "../../AVP32/Bases/Types.h"
#include "../../AVP32/Bases/Method.h"

////////////////////////////////////////////////////////////////
// Region type/IDs (KEEP IN SYNC WITH KATRAN SOURCE!!!)

#define KLAV_REGION_TYPE_AVP3 0x0010
// region ID is equal to page offset

#define KLAV_REGION_NAME_HEADER  "Header"
#define KLAV_REGION_NAME_PAGE_A  "Page_A"
#define KLAV_REGION_NAME_PAGE_B  "Page_B"
#define KLAV_REGION_NAME_PAGE_E  "Page_E"
#define KLAV_REGION_NAME_PAGE_C  "Page_C"

////////////////////////////////////////////////////////////////
// Link extra header format (little-endian):
//
// {
//   uint32_t total_size;                 // including header
//   uint32_t extra_size;                 // typically: 0x01
//   uint8_t  extra_data [extra_size];    // typically: { 0x00 }
// }

#endif // klav_avp_h_INCLUDED


