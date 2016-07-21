#include "precompiled.h"
#include "datatree.h"
#include "LicenseKeyPid.h"
#include "LicenseKey.h"

namespace KasperskyLicensing{
namespace Implementation{

//-----------------------------------------------------------------------------

LicenseKey::LicenseKey(const IObjectImage& obj, const ControlDatabase& cdb) 
	: key_name(obj.GetObjectName())
	, control_db(cdb)
{
	try
	{
		// parse binary data tree
		data_tree dtree(obj.GetObjectImage(), obj.GetObjectSize());
		// extract license key information node
		const data_tree::node& keyInfo = dtree.get_root().get_child(AVP_PID_KEYINFO);
		// check license key version
		if (keyInfo.get_child(AVP_PID_KEYVERSION).get_value().as_uint() != 6)
			throw LicensingError(ILicensingError::LICERR_KEY_INCOMPATIBLE);

		// extract license key information
		key_info.version = 
			keyInfo.get_child(AVP_PID_KEYVERSION).get_value().as_uint();
		key_info.creation_date = 
			keyInfo.get_child(AVP_PID_KEY_PROD_DATE).get_value().as_date();

		// serial number is stored as 3-dword-array
		AVP_dword snbuf[3] = { 0 };
		keyInfo.get_child(AVP_PID_KEYSERIALNUMBER).get_value().as_binary(&snbuf, sizeof(snbuf));
		key_info.serial_number.customer_id = snbuf[0];
		key_info.serial_number.application_id = snbuf[1];
		key_info.serial_number.sequence_number = snbuf[2];

		key_info.license_type = static_cast<LicenseKeyInfo::LicenseType> 
			(keyInfo.get_child(AVP_PID_KEYLICENCETYPE).get_value().as_uint());
		key_info.life_span = 
			keyInfo.get_child(AVP_PID_KEYLIFESPAN).get_value().as_uint();
		
		// find root license object
		const data_tree::node::child_enumerator& children = 
			keyInfo.get_child(AVP_PID_KEY_LO_ROOT).get_children();
		if (children.IsDone())
			throw LicensingError(ILicensingError::LICERR_PARSING_ERROR);
		// at root level only one license object is present
		const data_tree::node& licobj = children.Item();
		// construct root license object
		lic_object = AutoPointer<LicenseObject>(new LicenseObject(licobj, *this));
		
		// extract product information
		const data_tree::node& prodInfo = keyInfo.get_child(AVP_PID_KEY_PRODUCT_INFO);
		key_info.product_info.market_sector_id = 
			prodInfo.get_child(AVP_PID_KEY_PRODUCT_MARKETSECTOR).get_value().as_uint();
		key_info.product_info.sales_channel_id = 
			prodInfo.get_child(AVP_PID_KEY_PRODUCT_SALESCHANNEL).get_value().as_uint();
		key_info.product_info.localization_id = 
			prodInfo.get_child(AVP_PID_KEY_PRODUCT_LOCALIZATION).get_value().as_uint();
		key_info.product_info.package = 
			prodInfo.get_child(AVP_PID_KEY_PACKAGE).get_value().as_uint();
		// set product identifier from root license object
		key_info.product_info.product_id = lic_object->GetObjectId();

		// distributor information
		FillContactInfo(
			keyInfo.get_child(AVP_PID_KEY_DISTRIBUTOR_INFO), 
			key_info.distributor_info, 
			distributor
		);
		// reseller information
		FillContactInfo(
			keyInfo.get_child(AVP_PID_KEY_RESELLER_INFO), 
			key_info.reseller_info, 
			reseller
		);
		// local office information
		FillContactInfo(
			keyInfo.get_child(AVP_PID_KEY_LOCALOFFICE_INFO), 
			key_info.local_office_info, 
			local_office
		);
		// head quarters information
		FillContactInfo(
			keyInfo.get_child(AVP_PID_KEY_HQ_INFO), 
			key_info.head_quarters_info, 
			head_quarters
		);
		// support centre information
		FillContactInfo(
			keyInfo.get_child(AVP_PID_KEY_SUPPORTCENTER_INFO), 
			key_info.support_centre_info, 
			support_centre
		);
	}
	catch (std::runtime_error&)
	{
		throw LicensingError(ILicensingError::LICERR_PARSING_ERROR);
	}
}
	
//-----------------------------------------------------------------------------

const LicenseKeyInfo& LicenseKey::GetKeyInfo() const
{
	return key_info;
}

//-----------------------------------------------------------------------------

unsigned LicenseKey::GetSequenceNo() const
{
	return key_info.serial_number.sequence_number;
}

//-----------------------------------------------------------------------------

const LicenseObject& LicenseKey::GetLicenseRoot() const
{
	return  *lic_object;
}

//-----------------------------------------------------------------------------

const ILicenseObject& LicenseKey::GetLicenseObject() const
{
	return GetLicenseRoot();
}

//-----------------------------------------------------------------------------

void LicenseKey::FillContactInfo(
	const data_tree::node& data, 
	LicenseKeyInfo::PersonContact& contact, 
	string_array_t& array
)
{
	// set array size according to number of elements in PersonContact structure
	array.resize(sizeof(LicenseKeyInfo::PersonContact) / sizeof(char_t*));

	// extract contact information
	const data_tree::node& ci = data.get_child(AVP_PID_KEY_CONTACT_INFO);
	// fill name
	ci.get_child(AVP_PID_KEY_CONTACT_NAME).get_value().as_string(array[0]);
	contact.name = array[0].c_str();
	// fill country name
	ci.get_child(AVP_PID_KEY_CONTACT_COUNTRY).get_value().as_string(array[1]);
	contact.country = array[1].c_str();
	// fill city
	ci.get_child(AVP_PID_KEY_CONTACT_CITY).get_value().as_string(array[2]);
	contact.city = array[2].c_str();
	// fill address
	ci.get_child(AVP_PID_KEY_CONTACT_ADDRESS).get_value().as_string(array[3]);
	contact.address = array[3].c_str();

	// fill phone number
	ci.get_child(AVP_PID_KEY_CONTACT_PHONE).get_value().as_string(array[4]);
	contact.ci_person.phone = array[4].c_str();
	// fill fax number
	ci.get_child(AVP_PID_KEY_CONTACT_FAX).get_value().as_string(array[5]);
	contact.ci_person.fax = array[5].c_str();
	// fill email address
	ci.get_child(AVP_PID_KEY_CONTACT_EMAIL).get_value().as_string(array[6]);
	contact.ci_person.email = array[6].c_str();
	// fill web url
	ci.get_child(AVP_PID_KEY_CONTACT_WWW).get_value().as_string(array[7]);
	contact.ci_person.www = array[7].c_str();

	// extract support information
	const data_tree::node& si = data.get_child(AVP_PID_KEY_SUPPORT_INFO);
	// fill phone number
	si.get_child(AVP_PID_KEY_CONTACT_PHONE).get_value().as_string(array[8]);
	contact.ci_support.phone = array[8].c_str();
	// fill fax number
	si.get_child(AVP_PID_KEY_CONTACT_FAX).get_value().as_string(array[9]);
	contact.ci_support.fax = array[9].c_str();
	// fill email address
	si.get_child(AVP_PID_KEY_CONTACT_EMAIL).get_value().as_string(array[10]);
	contact.ci_support.email = array[10].c_str();
	// fill web url
	si.get_child(AVP_PID_KEY_CONTACT_WWW).get_value().as_string(array[11]);
	contact.ci_support.www = array[11].c_str();

}

//-----------------------------------------------------------------------------

const char_t* LicenseKey::GetName() const
{
	return key_name.c_str();
}

//-----------------------------------------------------------------------------

void LicenseKey::FillContext(
	calculation_context& context, 
	time_t time, 
	unsigned nominal, 
	unsigned value
) const
{
	context.cur_date		= time;
	context.nom_value		= nominal;
	context.cur_value		= value;
	context.exp_date		= key_info.creation_date + key_info.life_span * 86400;
	context.is_invalid_avdb	= control_db.GetAvdbStatus() != ControlDatabase::AVDB_OK;
	context.is_invalid_blst	= control_db.GetBlackListStatus() != ControlDatabase::BLST_OK;
	context.is_sn_blisted	= control_db.IsKeyBanned(key_info.serial_number);
	context.update_date		= control_db.GetUpdateDate();
}

//-----------------------------------------------------------------------------

} // namespace Implementation
} // namespace KasperskyLicensing