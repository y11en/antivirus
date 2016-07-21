/*!
 * (C) 2003 Kaspersky Lab 
 * 
 * \file	avt/acedecl.h
 * \author	Mikhail Karmazine
 * \date	28.02.2003 14:16:33
 * \brief	—труктура, используема€ дл€ определени€ одного ACE.
 *          ƒл€ определени€ статических ACL используютс€ константеые массивы.
 * 
 */

#ifndef __KLAVT_ACEDECL_H__
#define __KLAVT_ACEDECL_H__

#include <avp/avp_data.h>

enum { // «начени€ не должны мен€тьс€, поэтому используетс€ €вное задание значений (страховка от добавлени€ в середину).
	KLAVT_ACT_GNRL		= 0,
	KLAVT_ACT_KLGSYN	= 1,
	KLAVT_ACT_EV		= 2,
	KLAVT_ACT_EVPROP	= 3,
	KLAVT_ACT_EVPROC	= 4,
	KLAVT_ACT_EVPNP		= 5,
	KLAVT_ACT_HST		= 6,
	KLAVT_ACT_GRP		= 7,
	KLAVT_ACT_UPDT		= 8,
	KLAVT_ACT_PKG		= 9,
	KLAVT_ACT_POL		= 10,
	KLAVT_ACT_PRSSP		= 11,
	KLAVT_ACT_RPT		= 12,
	KLAVT_ACT_SSSRV		= 13,
	KLAVT_ACT_TR		= 14,
	KLAVT_ACT_TSK		= 15,
	KLAVT_ACT_LICSRV	= 16,
	KLAVT_ACT_PRES		= 17,
	KLAVT_ACT_PRTS		= 18,
	KLAVT_ACT_SYNC		= 19,
	KLAVT_ACT_NAG		= 20,
    KLAVT_ACT_AGT       = 21,
    KLAVT_ACT_ADHST     = 22,
	KLAVT_ACT_SRVH		= 23,
	KLAVT_ACT_SRVHM		= 24,
	KLAVT_ACT_SRVHS		= 25,
	KLAVT_ACT_SRVS		= 26,
    KLAVT_ACT_DPNS		= 27,
	KLAVT_ACT_NLST		= 28,
    KLAVT_ACT_AKLWNGT	= 29,
	KLAVT_ACT_SPL		= 30
};

#define KLAVT_READ		0x00000008
#define KLAVT_WRITE		0x00000004
#define KLAVT_DELETE	0x00000001
#define KLAVT_EXECUTE	0x00000002
#define KLAVT_CONTROL	0x00000010
#define KLAVT_CONNECT	0x00000020
#define KLAVT_ALL		0x0000FFFF

#define KLAVT_ACT_HST__SS_READ	0x00000100
#define KLAVT_ACT_HST__SS_WRITE	0x00000200

#define KLAVT_ACT_PKG__SS_READ	0x00000100
#define KLAVT_ACT_PKG__SS_WRITE	0x00000200

#define KLAVT_ACT_POL__SS_READ	0x00000100
#define KLAVT_ACT_POL__SS_WRITE	0x00000200

#define KLAVT_ACT_TR__CONNECT_GATEWAY 0x00000100
#define KLAVT_ACT_RT__TEST		0x00000200

//#define KLAVT_ALL      (KLAVT_READ|KLAVT_WRITE|KLAVT_EXECUTE)
//#define KLAUTH_ALL		0x0000FFFF
/*!
	Custom flags prefixes
	KLAUTH_COMPONENT_	for ComponentInstance
	KLAUTH_TASK_		for component's tasks
	KLAUTH_ACT_...__...		for action group, where ... - action group abbreviation and action abbreviation
*/

//////////////////////////////////////////////////////////////////////////
// Windows groups:
#define KLAVT_EVERYONE_GROUP          1
#define KLAVT_DOMAIN_ADMINS_GROUP     2
#define KLAVT_LOCAL_SYSTEM            3
#define KLAVT_KL_ADMINS_GROUP         4
#define KLAVT_KL_OPERATORS_GROUP      5
#define KLAVT_ADMINISTRATORS_GROUP    6

#ifdef _DEBUG
#define KLAVT_KL_SOURCE_SAFE_GROUP    7 // used in regression tests only
#endif // _DEBUG

// basic authentication groups:
#define KLAVT_BA                    100L

// ssl-based authentication groups:
#define KLAVT_SSLA                    200L

//////////////////////////////////////////////////////////////////////////
// basic authentication groups:
// (define here groups like NAGENT)
#define KLAVT_BA_NAGENT         (KLAVT_BA)
#define KLAVT_BA_SERVER         (KLAVT_BA + 1L)
#define KLAVT_BA_EVENTS_ONLY    (KLAVT_BA + 2L)
#define KLAVT_SSLA_SLAVE_SERVER   (KLAVT_SSLA + 1L)
#define KLAVT_SSLA_MASTER_SERVER  (KLAVT_SSLA + 2L)
#define KLAVT_MOBILE_DEVICE       (KLAVT_SSLA + 3L)

namespace KLAVT
{
    struct ACE_DECLARATION
    {
        int nAccountId;
        AVP_dword dwAccess;
        bool bAllow;
    };
}

#endif //__KLAVT_ACEDECL_H__

// Local Variables:
// mode: C++
// End:
