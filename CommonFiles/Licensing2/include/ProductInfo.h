/** 
 * @file
 * @brief	Declaration of product information.
 * @author	Andrey Guzhov
 * @date	12.07.2005
 * @version	1.0
 */

#ifndef PRODUCTINFO_H
#define PRODUCTINFO_H

namespace KasperskyLicensing {

/**
 * Product information.
 */
struct ProductInfo
{
	/**
	 * Product identifier.
	 */
	LicenseObjectId product_id;

	/**
	 * Market sector identifier.
	 */
	unsigned market_sector_id;

	/**
	 * Sales channel identifier.
	 */
	unsigned sales_channel_id;

	/**
	 * Localization identifier.
	 */
	unsigned localization_id;

	/**
	 * Product package.
	 */
	unsigned package;
};

} // namespace KasperskyLicensing

#endif // PRODUCTINFO_H
