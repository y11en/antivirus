/*!
 * (C) 2002 Kaspersky Lab 
 * 
 * \file	NotificationDescrUtils.h
 * \author	Mikhail Karmazine
 * \date	19:16 11.12.2002
 * \brief	Utility functions to convert notification_descr_t to param__params and .
 *				vise versa
 */

#ifndef _NOTIFICATION_DESCR_UTILITY_H__D7AD0E44_D7E9_4bbc_8F93_D339CF49B15E__
#define _NOTIFICATION_DESCR_UTILITY_H__D7AD0E44_D7E9_4bbc_8F93_D339CF49B15E__

const wchar_t pcszNDUNotificationDescrArray[] = L"NotificationDescrArray";

void NotificationDescrsForSoap(
	soap* soap,
	const std::vector<KLEVP::notification_descr_t>& vectNotificationDescr,
	klevpnp_notification_descr_array& notificationDescrArray );

void NotificationDescrsFromSoap(
	klevpnp_notification_descr_array notificationDescrArray,
	std::vector<KLEVP::notification_descr_t>& vectNotificationDescr );

#endif // _NOTIFICATION_DESCR_UTILITY_H__D7AD0E44_D7E9_4bbc_8F93_D339CF49B15E__

// Local Variables:
// mode: C++
// End:

