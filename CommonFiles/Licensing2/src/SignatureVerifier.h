/** 
 * @file
 * @brief	Implementaion of signature verifier.
 * @author	Andrey Guzhov
 * @date	30.07.2005
 * @version	1.0
 */

#ifndef SIGNATUREVERIFIER_H
#define SIGNATUREVERIFIER_H

namespace KasperskyLicensing {
namespace Implementation {

/**
 * Signature verifier.
 */
class SignatureVerifier
{
public:

	/**
	 * Constructs verifier object.
	 */
	explicit SignatureVerifier(const char_t* dskmPath);

	/**
	 * Verifies license key signature.
	 */
	bool VerifyKeySignature(const IObjectImage& image) const;

	/**
	 * Verifies AV database file signature.
	 */
	bool VerifyBaseSignature(const IObjectImage& image) const;

private:
	class DskmLibrary;

	// object type for signature verification
	enum ObjectType { OBJ_TYPE_LICKEY, OBJ_TYPE_AVDB };

	// verifies signature of given object
	bool VerifySignature(const IObjectImage& image, ObjectType otype) const;

	// path to DSKM registries and public keys
	std::basic_string<char_t> dskm_path;
	
	// DSKM library initializer
	static DskmLibrary lib_dskm;
};

/**
 * DSKM library initializer.
 */
class SignatureVerifier::DskmLibrary
{
public:
	
	/**
	 * Performs library initialization.
	 */
	DskmLibrary();

	/**
	 * Performs library deinitialization.
	 */
	~DskmLibrary();

	/**
	 * Returns DSKM library handle.
	 */
	HDSKM GetHandle() const;

private:
	
	HDSKM hDskm;
};


} // namespace Implementation
} // namespace KasperskyLicensing

#endif // SIGNATUREVERIFIER_H