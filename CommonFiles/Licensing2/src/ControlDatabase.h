/** 
 * @file
 * @brief	Implementation of control database.
 * @author	Andrey Guzhov
 * @date	08.07.2005
 * @version	1.0
 */

#ifndef CONTROLDATABASE_H
#define CONTROLDATABASE_H

#include <vector>
#include <string>
#include "SignatureVerifier.h"

namespace KasperskyLicensing{
namespace Implementation{

/**
 * A control database abstraction.
 */
class ControlDatabase
{
public:

	enum IndexStatus
	{
		INDEX_OK,
		INDEX_READ_ERROR,
		INDEX_INCORRECT_SIGN,
		INDEX_PARSING_ERROR
	};

	enum AvdbStatus
	{
		AVDB_OK,
		AVDB_BAD_INDEX,
		AVDB_INCONSISTENT
	};

	enum BlistStatus
	{
		BLST_OK,
		BLST_BAD_INDEX,
		BLST_READ_ERROR,
		BLST_INCORRECT_SIGN,
		BLST_PARSING_ERROR
	};

	class Index;
	
	/**
	 * Constructs control database.
	 */
	ControlDatabase(const char_t* dskmPath);

	/**
	 * Loads database according to specified control mode.
	 */
	void LoadDatabase(const IStorageR& storage, ControlMode mode);

	/**
	 * Returns true if license key is in the black list.
	 */
	bool IsKeyBanned(const LicenseKeyInfo::SerialNumber& sn) const;

	/**
	 * Returns status of database index.
	 */
	IndexStatus GetIndexStatus() const;

	/**
	 * Returns anti-virus database status.
	 */
	AvdbStatus GetAvdbStatus() const;

	/**
	 * Returns black list status.
	 */
	BlistStatus GetBlackListStatus() const;

	/**
	 * Returns database update date.
	 */
	time_t GetUpdateDate() const;

	/**
	 * Returns signature verifier object.
	 */
	const SignatureVerifier& GetSignatureVerifier() const;

private:

	// reset to initial state
	void Reset();

	// disable copy-construction
	ControlDatabase(const ControlDatabase&);

	// disable assignment
	ControlDatabase& operator=(const ControlDatabase&);

	// checks AV-database consistency
	void CheckBasesConsistency(const Index& index, const IStorageR& storage);

	// loads black list
	void LoadBlacklist(const Index& index, const IStorageR& storage);

	// black list type
	typedef std::vector<unsigned> black_list_t;

	// signature verifier
	SignatureVerifier verifier;

	// index status
	IndexStatus		index_status;

	// anti-virus database status
	AvdbStatus		avdb_status;

	// black list status
	BlistStatus		blist_status;

	// black list
	black_list_t	black_list;

	// AV-database update date
	time_t			update_date;
};


class ControlDatabase::Index
{
public:

	typedef std::vector<std::basic_string<char_t> > avbase_names_t;

	/**
	 * Constructs index object.
	 */
	Index(const IObjectImage& image);

	/**
	 * Returns index update date.
	 */
	time_t GetUpdateDate() const;

	/**
	 * Returns black list file name.
	 */
	const char_t* GetBlistName() const;

	/**
	 * Returns names of AV database files.
	 */
	const avbase_names_t& GetAvdbNames() const;

private:

	// index update date
	time_t upd_date;

	// names of anti-virus database files
	std::vector<std::basic_string<char_t> > avbase_names;

	// name of black list
	std::basic_string<char_t> blist_name;
};

} // namespace Implementation
} // namespace KasperskyLicensing

#endif // CONTROLDATABASE_H
