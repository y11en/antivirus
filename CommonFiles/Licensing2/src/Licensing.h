/** 
 * @file
 * @brief	Implementation of ILicensing interface.
 * @author	Andrey Guzhov
 * @date	08.07.2005
 * @version	1.0
 */

#ifndef LICENSING_H
#define LICENSING_H

namespace KasperskyLicensing{

/**
 * Kaspersky %Licensing implementation namespace.
 */
namespace Implementation{

/**
 * Implements ILicensing interface.
 */
class Licensing : public ILicensing
{
public:
	
	Licensing(
		IStorageRW& keyStorage, 
		const ProgramClientInfo& clientInfo, 
		const char_t* dskmPath
	);

	/**
	 * Parces key image and constructs license key object.
	 */
	virtual AutoPointer<ILicenseKey> 
		ParseLicenseKey(const void * image, size_t size) const;

	/**
	 * Creates license utility object by given context.
	 */
	virtual AutoPointer<ILicenseUtility> MakeLicenseUtility(const ILicenseContext& context);

	/**
	 * Creates root license context.
	 */
	virtual AutoPointer<ILicenseContext> MakeLicenseContext() const;

	/**
	 * Returns warnings occured during initialization.
	 */
	virtual WarningEnumerator GetInitWarnings() const;

	/**
	 * Sets specific control mode.
	 */
	void SetControlMode(IStorageR& baseStorage, ControlMode ctrlMode);

private:
	
	// disable copy-construction
	Licensing(const Licensing&);

	// disable assignment
	Licensing& operator=(const Licensing&);

	// license key space
	LicenseKeyspace key_space;

	// license key storage
	IStorageRW& key_storage;

	// initialization warnings array
	std::vector<WarningCode> warnings;
};

} // namespace Implementation
} // namespace KasperskyLicensing

#endif //LICENSING_H
