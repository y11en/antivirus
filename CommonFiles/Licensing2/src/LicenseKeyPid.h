#ifndef LICENSEKEYPID_H
#define LICENSEKEYPID_H

#include <Property/Property.h>
#include <LoadKeys/AVPKeyID.h>

namespace KasperskyLicensing{
namespace Implementation{

// licensing application identifier
const unsigned char AVP_APID_LICENSING				= 0x0A;

// product information
const AVP_dword AVP_PID_KEY_PRODUCT_INFO_ID			= 40;
const AVP_dword AVP_PID_KEY_PRODUCT_INFO			= MAKE_AVP_PID(AVP_PID_KEY_PRODUCT_INFO_ID, AVP_APID_LICENSING, avpt_nothing, 0);
// market sector
const AVP_dword AVP_PID_KEY_PRODUCT_MARKETSECTOR_ID	= 41;
const AVP_dword AVP_PID_KEY_PRODUCT_MARKETSECTOR	= MAKE_AVP_PID(AVP_PID_KEY_PRODUCT_MARKETSECTOR_ID, AVP_APID_LICENSING, avpt_dword, 0);
// sales channel
const AVP_dword AVP_PID_KEY_PRODUCT_SALESCHANNEL_ID	= 42;
const AVP_dword AVP_PID_KEY_PRODUCT_SALESCHANNEL	= MAKE_AVP_PID(AVP_PID_KEY_PRODUCT_SALESCHANNEL_ID, AVP_APID_LICENSING, avpt_dword, 0);
// localization id
const AVP_dword AVP_PID_KEY_PRODUCT_LOCALIZATION_ID	= 43;
const AVP_dword AVP_PID_KEY_PRODUCT_LOCALIZATION	= MAKE_AVP_PID(AVP_PID_KEY_PRODUCT_LOCALIZATION_ID, AVP_APID_LICENSING, avpt_dword, 0);
// package id
const AVP_dword AVP_PID_KEY_PACKAGE_ID				= 44;
const AVP_dword AVP_PID_KEY_PACKAGE					= MAKE_AVP_PID(AVP_PID_KEY_PACKAGE_ID, AVP_APID_LICENSING, avpt_dword, 0);

// list of license objects
const AVP_dword AVP_PID_KEY_LO_LIST_ID				= 50;
const AVP_dword AVP_PID_KEY_LO_LIST					= MAKE_AVP_PID(AVP_PID_KEY_LO_LIST_ID, AVP_APID_LICENSING, avpt_nothing, 0);
// root license object
const AVP_dword AVP_PID_KEY_LO_ROOT_ID				= 51;
const AVP_dword AVP_PID_KEY_LO_ROOT					= MAKE_AVP_PID(AVP_PID_KEY_LO_ROOT_ID, AVP_APID_LICENSING, avpt_nothing, 0);
// license object name
const AVP_dword AVP_PID_KEY_LO_NAME_ID				= 52;
const AVP_dword AVP_PID_KEY_LO_NAME					= MAKE_AVP_PID(AVP_PID_KEY_LO_NAME_ID, AVP_APID_LICENSING, avpt_str, 0);
// license object version
const AVP_dword AVP_PID_KEY_LO_VERSION_ID			= 53;
const AVP_dword AVP_PID_KEY_LO_VERSION				= MAKE_AVP_PID(AVP_PID_KEY_LO_VERSION_ID, AVP_APID_LICENSING, avpt_dword, 0);
// supported platform list
const AVP_dword AVP_PID_KEY_PRODUCT_PLATFORM_LIST_ID	= 53;
const AVP_dword AVP_PID_KEY_PRODUCT_PLATFORM_LIST		= MAKE_AVP_PID(AVP_PID_KEY_PRODUCT_PLATFORM_LIST_ID, AVP_APID_LICENSING, avpt_nothing, 0);


// license restriction list
const AVP_dword AVP_PID_KEY_RESTRICTION_LIST_ID		= 60;
const AVP_dword AVP_PID_KEY_RESTRICTION_LIST		= MAKE_AVP_PID(AVP_PID_KEY_RESTRICTION_LIST_ID, AVP_APID_LICENSING, avpt_nothing, 0);
// restriction name
const AVP_dword AVP_PID_KEY_RESTRICTION_NAME_ID		= 61;
const AVP_dword AVP_PID_KEY_RESTRICTION_NAME		= MAKE_AVP_PID(AVP_PID_KEY_RESTRICTION_NAME_ID, AVP_APID_LICENSING, avpt_str, 0);
// nominal value of licensing restriction
const AVP_dword AVP_PID_KEY_RESTRICTION_VALUE_ID	= 62;
const AVP_dword AVP_PID_KEY_RESTRICTION_VALUE		= MAKE_AVP_PID(AVP_PID_KEY_RESTRICTION_VALUE_ID, AVP_APID_LICENSING, avpt_dword, 0);
// additive condition
const AVP_dword AVP_PID_KEY_ADDITIVE_CONDITION_ID	= 63;
const AVP_dword AVP_PID_KEY_ADDITIVE_CONDITION		= MAKE_AVP_PID(AVP_PID_KEY_ADDITIVE_CONDITION_ID, AVP_APID_LICENSING, avpt_str, 0);
// notification id when restriction is violated
const AVP_dword AVP_PID_KEY_RESTRICTION_DEFAULT_NOTIFY_ID	= 64;
const AVP_dword AVP_PID_KEY_RESTRICTION_DEFAULT_NOTIFY		= MAKE_AVP_PID(AVP_PID_KEY_RESTRICTION_DEFAULT_NOTIFY_ID, AVP_APID_LICENSING, avpt_dword, 0);

// license condition list
const AVP_dword AVP_PID_KEY_CONDITION_LIST_ID		= 70;
const AVP_dword AVP_PID_KEY_CONDITION_LIST			= MAKE_AVP_PID(AVP_PID_KEY_CONDITION_LIST_ID, AVP_APID_LICENSING, avpt_nothing, 0);
// expression of license condition
const AVP_dword AVP_PID_KEY_CONDITION_EXPRESSION_ID	= 71;
const AVP_dword AVP_PID_KEY_CONDITION_EXPRESSION	= MAKE_AVP_PID(AVP_PID_KEY_CONDITION_EXPRESSION_ID, AVP_APID_LICENSING, avpt_str, 0);
// notification identifier
const AVP_dword AVP_PID_KEY_CONDITION_NOTIFY_ID		= 72;
const AVP_dword AVP_PID_KEY_CONDITION_NOTIFY		= MAKE_AVP_PID(AVP_PID_KEY_CONDITION_NOTIFY_ID, AVP_APID_LICENSING, avpt_dword, 0);

// contact information
const AVP_dword AVP_PID_KEY_CONTACT_INFO_ID			= 80;
const AVP_dword AVP_PID_KEY_CONTACT_INFO			= MAKE_AVP_PID(AVP_PID_KEY_CONTACT_INFO_ID, AVP_APID_LICENSING, avpt_nothing, 0);
// support info
const AVP_dword AVP_PID_KEY_SUPPORT_INFO_ID			= 81;
const AVP_dword AVP_PID_KEY_SUPPORT_INFO			= MAKE_AVP_PID(AVP_PID_KEY_SUPPORT_INFO_ID, AVP_APID_LICENSING, avpt_nothing, 0);
// name of the contact
const AVP_dword AVP_PID_KEY_CONTACT_NAME_ID			= 82;
const AVP_dword AVP_PID_KEY_CONTACT_NAME			= MAKE_AVP_PID(AVP_PID_KEY_CONTACT_NAME_ID, AVP_APID_LICENSING, avpt_str, 0);
// country 
const AVP_dword AVP_PID_KEY_CONTACT_COUNTRY_ID		= 83;
const AVP_dword AVP_PID_KEY_CONTACT_COUNTRY			= MAKE_AVP_PID(AVP_PID_KEY_CONTACT_COUNTRY_ID, AVP_APID_LICENSING, avpt_str, 0);
// city
const AVP_dword AVP_PID_KEY_CONTACT_CITY_ID			= 84;
const AVP_dword AVP_PID_KEY_CONTACT_CITY			= MAKE_AVP_PID(AVP_PID_KEY_CONTACT_CITY_ID, AVP_APID_LICENSING, avpt_str, 0);
// address
const AVP_dword AVP_PID_KEY_CONTACT_ADDRESS_ID		= 85;
const AVP_dword AVP_PID_KEY_CONTACT_ADDRESS			= MAKE_AVP_PID(AVP_PID_KEY_CONTACT_ADDRESS_ID, AVP_APID_LICENSING, avpt_str, 0);
// phone
const AVP_dword AVP_PID_KEY_CONTACT_PHONE_ID		= 86;
const AVP_dword AVP_PID_KEY_CONTACT_PHONE			= MAKE_AVP_PID(AVP_PID_KEY_CONTACT_PHONE_ID, AVP_APID_LICENSING, avpt_str, 0);
// fax
const AVP_dword AVP_PID_KEY_CONTACT_FAX_ID			= 87;
const AVP_dword AVP_PID_KEY_CONTACT_FAX				= MAKE_AVP_PID(AVP_PID_KEY_CONTACT_FAX_ID, AVP_APID_LICENSING, avpt_str, 0);
// email
const AVP_dword AVP_PID_KEY_CONTACT_EMAIL_ID		= 88;
const AVP_dword AVP_PID_KEY_CONTACT_EMAIL			= MAKE_AVP_PID(AVP_PID_KEY_CONTACT_EMAIL_ID, AVP_APID_LICENSING, avpt_str, 0);
// web
const AVP_dword AVP_PID_KEY_CONTACT_WWW_ID			= 89;
const AVP_dword AVP_PID_KEY_CONTACT_WWW				= MAKE_AVP_PID(AVP_PID_KEY_CONTACT_WWW_ID, AVP_APID_LICENSING, avpt_str, 0);

// distributor information
const AVP_dword AVP_PID_KEY_DISTRIBUTOR_INFO_ID		= 90;
const AVP_dword AVP_PID_KEY_DISTRIBUTOR_INFO		= MAKE_AVP_PID(AVP_PID_KEY_DISTRIBUTOR_INFO_ID, AVP_APID_LICENSING, avpt_nothing, 0);
// reseller information
const AVP_dword AVP_PID_KEY_RESELLER_INFO_ID		= 91;
const AVP_dword AVP_PID_KEY_RESELLER_INFO			= MAKE_AVP_PID(AVP_PID_KEY_RESELLER_INFO_ID, AVP_APID_LICENSING, avpt_nothing, 0);
// local office information
const AVP_dword AVP_PID_KEY_LOCALOFFICE_INFO_ID		= 92;
const AVP_dword AVP_PID_KEY_LOCALOFFICE_INFO		= MAKE_AVP_PID(AVP_PID_KEY_LOCALOFFICE_INFO_ID, AVP_APID_LICENSING, avpt_nothing, 0);
// head quarters information
const AVP_dword AVP_PID_KEY_HQ_INFO_ID				= 93;
const AVP_dword AVP_PID_KEY_HQ_INFO					= MAKE_AVP_PID(AVP_PID_KEY_HQ_INFO_ID, AVP_APID_LICENSING, avpt_nothing, 0);
// support center information
const AVP_dword AVP_PID_KEY_SUPPORTCENTER_INFO_ID	= 94;
const AVP_dword AVP_PID_KEY_SUPPORTCENTER_INFO		= MAKE_AVP_PID(AVP_PID_KEY_SUPPORTCENTER_INFO_ID, AVP_APID_LICENSING, avpt_nothing, 0);

		
} // namespace Implementation
} // namespace KasperskyLicensing

#endif // LICENSEKEYPID_H