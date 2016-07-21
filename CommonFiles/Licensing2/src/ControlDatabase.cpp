#include "precompiled.h"
#include <LAX.h>
#include "strconv.hpp"
#include "ControlDatabase.h"

namespace KasperskyLicensing{
namespace Implementation{

//-----------------------------------------------------------------------------

ControlDatabase::ControlDatabase(const char_t* dskmPath) : verifier(dskmPath)
{
	// reset to initial state
	Reset();
}

//-----------------------------------------------------------------------------

void ControlDatabase::LoadDatabase(const IStorageR& storage, ControlMode mode)
{
	// reset to initial state
	Reset();
	// try to load and parse index
	try
	{
		// load image of database index
		AutoPointer<IObjectImage> image = storage.LoadObject(TXT("kavset.xml"));
		// verify image signature
		if (verifier.VerifyBaseSignature(*image))
		{
			// parse database index
			Index index(*image);
			// set update date
			update_date = index.GetUpdateDate();
			// analyze control mode and perform proper action
			switch (mode)
			{
			case CTRL_BLIST_AND_BASES:
				CheckBasesConsistency(index, storage);
			case CTRL_BLIST_ONLY:
				LoadBlacklist(index, storage);
				break;
			}
		}
		else
		{
			index_status = INDEX_INCORRECT_SIGN;
		}
	}
	catch(ILicensingError& e)
	{
		// analyze error code
		switch(e.GetErrorCode())
		{
		case ILicensingError::LICERR_STORAGE_READ:
			index_status = INDEX_READ_ERROR;
			break;
		case ILicensingError::LICERR_PARSING_ERROR:
			index_status = INDEX_PARSING_ERROR;
			break;
		default:
			throw;
		}
	}

	if (index_status != INDEX_OK)
	{
		// set error status
		switch (mode)
		{
		case CTRL_BLIST_AND_BASES:
			avdb_status = AVDB_BAD_INDEX;
		case CTRL_BLIST_ONLY:
			blist_status = BLST_BAD_INDEX;
			break;
		};
	}
}

//-----------------------------------------------------------------------------

void ControlDatabase::Reset()
{
	update_date		= 0;
	index_status	= INDEX_OK;
	avdb_status		= AVDB_OK;
	blist_status	= BLST_OK;
	black_list.swap(black_list_t());
}

//-----------------------------------------------------------------------------

bool ControlDatabase::IsKeyBanned(const LicenseKeyInfo::SerialNumber& sn) const
{
	black_list_t::const_iterator it = black_list.begin(), end = black_list.end();

	if (std::binary_search(it, end, sn.sequence_number))
		return true;

	return false;
}

//-----------------------------------------------------------------------------

ControlDatabase::AvdbStatus ControlDatabase::GetAvdbStatus() const
{
	return avdb_status;
}

//-----------------------------------------------------------------------------

ControlDatabase::BlistStatus ControlDatabase::GetBlackListStatus() const
{
	return blist_status;
}

//-----------------------------------------------------------------------------

ControlDatabase::IndexStatus ControlDatabase::GetIndexStatus() const
{
	return index_status;
}

//-----------------------------------------------------------------------------

time_t ControlDatabase::GetUpdateDate() const
{
	return update_date;
}

//-----------------------------------------------------------------------------

void ControlDatabase::LoadBlacklist(const Index& index, const IStorageR& storage)
{
	// size of black list label
	const size_t LABEL_SIZE = 4;
	// size of numeric fields
	const size_t NUMERIC_SIZE = 4;
	// size of header
	const size_t HEADER_SIZE = 140;
	// version of black list file
	const size_t CURRENT_VERSION = 1;

	AutoPointer<IObjectImage> image;

	try
	{
		// load image of database index
		image = storage.LoadObject(index.GetBlistName());
	}
	catch(ILicensingError&)
	{
		blist_status = BLST_READ_ERROR;
		return;
	}

	// verify image signature
	if (!verifier.VerifyBaseSignature(*image))
	{
		blist_status = BLST_INCORRECT_SIGN;
		return;
	}

	const char* it	= static_cast<const char*>(image->GetObjectImage());
	const char* end	= it + image->GetObjectSize();

	// check black list label
	if (it + LABEL_SIZE > end || strncmp(it, "KLBL", LABEL_SIZE))
	{
		blist_status = BLST_PARSING_ERROR;
		return;
	}
	it += LABEL_SIZE;

	// check header size
	if (it + NUMERIC_SIZE > end || *reinterpret_cast<const unsigned*>(it) != HEADER_SIZE)
	{
		blist_status = BLST_PARSING_ERROR;
		return;
	}
	it += NUMERIC_SIZE;

	// check black list file version
	if (it + NUMERIC_SIZE > end || *reinterpret_cast<const unsigned*>(it) != CURRENT_VERSION)
	{
		blist_status = BLST_PARSING_ERROR;
		return;
	}
	it += HEADER_SIZE;

	// extract number of records
	if (it + NUMERIC_SIZE > end)
	{
		blist_status = BLST_PARSING_ERROR;
		return;
	}
	const unsigned rec_number = *reinterpret_cast<const unsigned*>(it);
	it += NUMERIC_SIZE;
	// check for proper number of records
	if (it + rec_number * NUMERIC_SIZE > end)
	{
		blist_status = BLST_PARSING_ERROR;
		return;
	}
	// store sequence numbers of banned keys
	for (unsigned i = 0; i < rec_number; i++)
		black_list.push_back(reinterpret_cast<const unsigned*>(it)[i]);

	// sort array of elements
	std::sort(black_list.begin(), black_list.end(), std::less<unsigned>());

	blist_status = BLST_OK;
}

//-----------------------------------------------------------------------------

void ControlDatabase::CheckBasesConsistency(const Index& index, const IStorageR& storage)
{
	Index::avbase_names_t::const_iterator 
		it(index.GetAvdbNames().begin()), end(index.GetAvdbNames().end());

	for (; it != end; it++)
	{
		try
		{
			// load AV database file
			AutoPointer<IObjectImage> image = storage.LoadObject(it->c_str());
			// check object signature
			if (!verifier.VerifyBaseSignature(*image))
			{
				avdb_status = AVDB_INCONSISTENT;
				return;
			}
		}
		catch(ILicensingError&)
		{
			avdb_status = AVDB_INCONSISTENT;
			return;
		}
	}

	avdb_status = AVDB_OK;
}


//-----------------------------------------------------------------------------

const SignatureVerifier& ControlDatabase::GetSignatureVerifier() const
{
	return verifier;
}

//-----------------------------------------------------------------------------

ControlDatabase::Index::Index(const IObjectImage& image)
{
	typedef LAX::UTF8_UTF8<LAX::STD_STRING_ADAPTER<char, std::string> >		transform_t;
	typedef LAX::STD_ATTR_MAP_ADAPTER<char, LAX::XmlAttrMap<transform_t> >	adapter_t;
	typedef LAX::XmlReader<transform_t, adapter_t, LAX::XmlNoExceptions>	parser_t;

	parser_t parser(static_cast<const char*>(image.GetObjectImage()), image.GetObjectSize());

	// top level element of index
	parser_t::Element elem;
	// attributes of element
	parser_t::AttrMap attrs;

	// load top level element
	if (parser.readElement(elem, LAX::XML_F_ELT_REQUIRED | LAX::XML_F_CHECK_NAME, attrs, "UpdateFiles") != LAX::XML_OK)
		throw LicensingError(ILicensingError::LICERR_PARSING_ERROR);

	// load update date
	const parser_t::char_t* val = 0;
	// find update date attribute
	if (strlen(val = attrs.find("UpdateDate")) < 8)
		throw LicensingError(ILicensingError::LICERR_PARSING_ERROR);

	tm updtm = { 0 };
	// read update date
	if (!sscanf(val, "%2d%2d%4d", &updtm.tm_mday, &updtm.tm_mon, &updtm.tm_year))
		throw LicensingError(ILicensingError::LICERR_PARSING_ERROR);

	// normalize update date
	updtm.tm_mon--; updtm.tm_year -= 1900; updtm.tm_isdst = -1;
	// set update date
	if(!(upd_date = mktime(&updtm)))
		throw LicensingError(ILicensingError::LICERR_PARSING_ERROR);

	int err = LAX::XML_OK;

	while ((err = parser.readElement(elem, 0, attrs)) == LAX::XML_OK)
	{
		// load only file descriptions
		if (elem.compareXmlTag("FileDescription"))
		{
			// load file description
			const parser_t::char_t *eltype = 0, *fname = 0;
			// get element type
			if (!(eltype = attrs.find("UpdateType")))
				throw LicensingError(ILicensingError::LICERR_PARSING_ERROR);
			// get file name
			if (!(fname = attrs.find("Filename")))
				throw LicensingError(ILicensingError::LICERR_PARSING_ERROR);

			if(!strcmp(eltype, "base"))
				avbase_names.push_back(conv::string_conv<char_t>(fname));
			else if(!strcmp(eltype, "blst"))
				blist_name = conv::string_conv<char_t>(fname);
		}
	}

	if (!(err == LAX::XML_OK || err == LAX::XML_END))
		throw LicensingError(ILicensingError::LICERR_PARSING_ERROR);
}

//-----------------------------------------------------------------------------

time_t ControlDatabase::Index::GetUpdateDate() const
{
	return upd_date;
}

//-----------------------------------------------------------------------------

const char_t* ControlDatabase::Index::GetBlistName() const
{
	return blist_name.c_str();
}

//-----------------------------------------------------------------------------

const ControlDatabase::Index::avbase_names_t& 
	ControlDatabase::Index::GetAvdbNames() const
{
	return avbase_names;
}

//-----------------------------------------------------------------------------

} // namespace Implementation
} // namespace KasperskyLicensing
