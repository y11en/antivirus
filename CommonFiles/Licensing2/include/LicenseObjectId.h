/** 
 * @file
 * @brief	Declaration of license object identifier.
 * @author	Andrey Guzhov
 * @date	08.07.2005
 * @version	1.0
 */

#ifndef LICENSEOBJECTID_H
#define LICENSEOBJECTID_H

namespace KasperskyLicensing{

/**
 * License object identifier.
 */
struct LicenseObjectId
{
	/** 
	 * License object type.
	 */
	enum Type
	{
		/**
		 * Specifies product license object type. 
		 */
		OBJ_TYPE_PRODUCT,

		/**
		 * Specifies application license object type. 
		 */
		OBJ_TYPE_APPLICATION,

		/**
		 * Specifies component license object type. 
		 */
		OBJ_TYPE_COMPONENT
	};

	/**
	 * License object identifier. 
	 */
    unsigned short id;

	/**
	 * License object type. 
	 */
	Type type;

	/**
	 * Major version of license object.
	 */
	unsigned short major_version;

	/**
	 * Minor version of license object. 
	 */
	unsigned short minor_version;

};

}
#endif // LICENSEOBJECTID_H
