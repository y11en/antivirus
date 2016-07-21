/** 
 * @file
 * @brief	Declaration of ILicenseKey interface.
 * @author	Andrey Guzhov
 * @date	15.07.2005
 * @version	1.0
 */

#ifndef ILICENSEKEY_H
#define ILICENSEKEY_H

namespace KasperskyLicensing {

/**
 * License key information.
 */
struct LicenseKeyInfo
{
	/**
	 * License type.
	 */
	enum LicenseType
	{
		/**
		 * Commercial license key.
		 */
		LTYPE_COMMERCIAL	= 1,
		/**
		 * Beta license key.
		 */
		LTYPE_BETA			= 2,

		/**
		 * Trial license key.
		 */
		LTYPE_TRIAL			= 3,

		/**
		 * OEM license key.
		 */
		LTYPE_OEM			= 4

	};

	/**
	 * License key serial number.
	 */
	struct SerialNumber
	{
		/**
		 * Customer identifier.
		 */
		unsigned customer_id;

		/**
		 * Application identifier.
		 */
		unsigned application_id;

		/**
		 * License key sequence number.
		 */
		unsigned sequence_number;
	};

	/**
	 * Person contact information.
	 */
	struct PersonContact
	{
		/**
		 * Contact information.
		 */
		struct ContactInfo
		{
			/**
			 * Country of residence.
			 */
			const char_t* phone;

			/**
			 * Country of residence.
			 */
			const char_t* fax;

			/**
			 * Country of residence.
			 */
			const char_t* email;

			/**
			 * Country of residence.
			 */
			const char_t* www;
		};

		/**
		 * Name of responsible person.
		 */
		const char_t* name;

		/**
		 * Country of residence.
		 */
		const char_t* country;

		/**
		 * City of residence.
		 */
		const char_t* city;

		/**
		 * Address of residence.
		 */
		const char_t* address;

		/**
		 * Person contact information.
		 */
		ContactInfo ci_person;

		/**
		 * Support contact information.
		 */
		ContactInfo ci_support;

	};

	/**
	 * License key version.
	 */
	unsigned version;

	/**
	 * License key creation date.
	 */
	time_t creation_date;

	/**
	 * License key serial number.
	 */
	SerialNumber serial_number;

	/**
	 * License type.
	 */
	LicenseType license_type;

	/**
	 * License key life span.
	 */
	unsigned life_span;

	/**
	 * Product information.
	 */
	ProductInfo product_info;

	/**
	 * Distributor information.
	 */
	PersonContact distributor_info;

	/**
	 * Reseller information.
	 */
	PersonContact reseller_info;

	/**
	 * Local office information.
	 */
	PersonContact local_office_info;

	/**
	 * Head quarters information.
	 */
	PersonContact head_quarters_info;

	/**
	 * Support centre information.
	 */
	PersonContact support_centre_info;
};

/**
 * A license key abstraction.
 * Provides methods to access license key information.
 * @interface
 */
class ILicenseKey
{
	friend class AutoPointer<ILicenseKey>;

public:

	/**
	 * Returns license key information.
	 */
	virtual const LicenseKeyInfo& GetKeyInfo() const = 0;

	/**
	 * Returns root license object.
	 */
	virtual const ILicenseObject& GetLicenseObject() const = 0;

	/**
	 * Returns name of license key.
	 */
	virtual const char_t* GetName() const = 0;

protected:

	/**
	 * Destructor is protected in order to prevent direct use of delete operator.
	 */
	virtual ~ILicenseKey() {}
};

} // namespace KasperskyLicensing

#endif // ILICENSEKEY_H
