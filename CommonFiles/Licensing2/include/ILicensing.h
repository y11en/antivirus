/** 
 * @file
 * @brief	Declaration of ILicensing interface.
 * @author	Andrey Guzhov
 * @date	08.07.2005
 * @version	1.0
 */

#ifndef ILICENSING_H
#define ILICENSING_H

namespace KasperskyLicensing {

/**
 * Information about program client.
 */
struct ProgramClientInfo
{
	/**
	 * Client version type. 
	 */
	enum VersionType
	{
		/**
		 * Beta. 
		 */
		VER_TYPE_BETA,

		/**
		 * Release. 
		 */
		VER_TYPE_RELEASE
	};

	/**
	 * Client product information.
	 */
	ProductInfo product_info;

	/**
	 * Program version type.
	 */
	VersionType	version_type;

	/**
	 * Platform identifier.
	 */
	PlatformType platform_id;
};

/** 
 * The basic interface of the Kaspersky Licensing library.
 * Provides factory methods for ILicenseToolkit and ILicenseContext 
 * interfaces creation and license key parsing.
 * @interface
 */
class ILicensing
{
	friend class AutoPointer<ILicensing>;
	
public:

	/**
	 * Initialization warning codes.
	 */
	enum WarningCode
	{
		/**
		 * Incorrect index file of anti-virus database.
		 */
		WARN_INCORRECT_DBINDEX	= 1,

		/**
		 * Black list is incorrect.
		 */
		WARN_INCORRECT_BLACKLIST,

		/**
		 * Anti-virus database is inconsistent.
		 */
		WARN_INCONSISTENT_AVDATABASE,

		/**
		 * Incorrect license keys were skipped.
		 */
		WARN_INCORRECT_KEYS_SKIPPED
	};

	/**
	 * Initialization warning codes.
	 */
	typedef Enumerator<WarningCode> WarningEnumerator;

	/** 
	 * Parces key image and constructs license key object.
	 * @param[in]	data	binary data of key image.
	 * @param[in]	size	size of key image.
	 * @exception ILicensingError error code specifies the exception reason.
	 * @return automatic pointer to instantiated license key interface.
	 */
	virtual AutoPointer<ILicenseKey> ParseLicenseKey(const void* data, size_t size) const = 0;

	/**
	 * Creates license utility object by given context.
	 * @param[in]	context	context for which license utility will be created.
	 * @exception ILicensingError error code specifies the exception reason.
	 * @return automatic pointer to instantiated license utility interface.
	 */
	virtual AutoPointer<ILicenseUtility> MakeLicenseUtility(const ILicenseContext& context) = 0;

	/**
	 * Creates root license context.
	 * @return automatic pointer to instantiated license context interface.
	 */
	virtual AutoPointer<ILicenseContext> MakeLicenseContext() const = 0;

	/**
	 * Returns warnings occured during initialization.
	 */
	virtual WarningEnumerator GetInitWarnings() const = 0;

protected:

	/**
	 * Destructor is protected in order to prevent direct use of delete operator. 
	 */
	virtual ~ILicensing() {}
	
private:

	/** @link dependency
	 * @stereotype instantiate*/
	/*# ILicenseKey lnkILicenseKey; */

	/** @link dependency 
	 * @stereotype use*/
	/*# ILicenseContext lnkILicenseContext; */

	/** @link dependency 
	 * @stereotype instantiate*/
	/*# ILicenseUtility lnkILicenseUtility; */
};

/**
 * Factory function for licensing interface creation.
 * Created interface provides no additional license key validation.
 * @param[in]	keyStorage		license key storage interface.
 * @param[in]	clientInfo		information about program client.
 * @exception ILicensingError error code specifies the exception reason.
 * @return automatic pointer to instantiated licensing library interface.
 */
AutoPointer<ILicensing> MakeLicensing(
	IStorageRW& keyStorage, 
	const ProgramClientInfo& clientInfo,
	const char_t* dskmPath
);

/**
 * Extended factory function for licensing interface creation.
 * License keys will be additionally checked up in accordance with specified mode.
 * @param[in]	keyStorage		license key storage interface.
 * @param[in]	baseStorage		anti-virus database storage interface.
 * @param[in]	ctrlMode		license keys control mode.
 * @param[in]	appVersionType	specifies type of application.
 * @exception ILicensingError error code specifies the exception reason.
 * @return automatic pointer to instantiated licensing library interface.
 */
AutoPointer<ILicensing> MakeLicensingEx(
	IStorageRW& keyStorage, 
	IStorageR& baseStorage, 
	ControlMode ctrlMode, 
	const ProgramClientInfo& clientInfo,
	const char_t* dskmPath
);

} // namespace KasperskyLicensing

#endif //ILICENSING_H
