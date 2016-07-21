// mft_reader.cpp
//

#include <klava/kdb_utils.h>

#ifdef _MSC_VER
#endif

////////////////////////////////////////////////////////////////
// Manifest reader

KdbManifestReader::KdbManifestReader()
{
	clear ();
}

KLAV_ERR KdbManifestReader::init (const uint8_t* buffer, uint32_t size)
{
	clear ();

	KDUBlockReader blockReader(buffer, size);

	KDUBlockInfo blk;
	KDUErr kduErr = blockReader.getNextBlock(blk);
	
	if (kduErr != KDU_E_OK) return KLAV_EFORMAT;
	if (blk.type != KDU_BLOCK_TYPE_DBFILE) return KLAV_EFORMAT;

	if (blk.subhdr_len == KDU_SUBHDR_DBFILE_SIZE)
	{
		const KDU_SUB_HEADER_DBFILE *subhdr = (const KDU_SUB_HEADER_DBFILE *)blk.subhdr;
		m_dbVersion = KDU_SUBHDR_DBFILE_SEQ(subhdr);
	}
	else if (blk.subhdr_len == KDU_SUBHDR_DBFILE_V2_SIZE)
	{
		const KDU_SUB_HEADER_DBFILE_V2 *subhdr = (const KDU_SUB_HEADER_DBFILE_V2 *)blk.subhdr;
		m_dbVersion = KDU_SUBHDR_DBFILE_V2_SEQ(subhdr);
		m_dbIncarnation = KDU_SUBHDR_DBFILE_V2_INC(subhdr);
	}
	else
	{
		return KLAV_EFORMAT;
	}
	
	KDUSectionReader sectionReader(blk.data, blk.size);
	KDUSectionInfo sect;

	kduErr = sectionReader.getNextSection(sect);
	
	if (kduErr != KDU_E_OK) return KLAV_EFORMAT;
	if (sect.sectionType != KDBID_KERNEL_PARMS || sect.fragmentID != 0 || sect.raw) return KLAV_EFORMAT;

	m_parmSectionData = sect.data;
	m_parmSectionSize = sect.size;

	kduErr = sectionReader.getNextSection(sect);
	
	if (kduErr != KDU_E_OK) return KLAV_EFORMAT;
	if (sect.sectionType != KDBID_DBFLIST || sect.fragmentID != 0 || sect.raw != 0) return KLAV_EFORMAT;

	m_dbListSectionData = sect.data;
	m_dbListSectionSize = sect.size;

	return KLAV_OK;
}

void KdbManifestReader::clear ()
{
	m_dbVersion = 0;
	m_dbIncarnation = 0;
	m_parmSectionData = 0;
	m_parmSectionSize = 0;
	m_dbListSectionData = 0;
	m_dbListSectionSize = 0;
}

uint32_t KdbManifestReader::getKernelParm (uint32_t id)
{
	uint32_t val = 0;
	findKernelParm (id, &val);
	return val;
}

bool KdbManifestReader::findKernelParm (uint32_t id, uint32_t *pval)
{
	KernelParmEnum en = enumKernelParms ();
	KernelParm parm;

	for (;;) {
		if (en.getNext (parm) != KLAV_OK) break;
		if (parm.id == id) {
			*pval = parm.value;
			return true;
		}
	}
	*pval = 0;
	return false;
}

////////////////////////////////////////////////////////////////
// KernelParmEnum

void KdbManifestReader::KernelParmEnum::init (const uint8_t* data, uint32_t size)
{
	m_reader.init (data, size);
}

KLAV_ERR KdbManifestReader::KernelParmEnum::getNext (KernelParm& parm)
{
	KDUErr err = KDU_E_OK;

	parm.clear ();

	KDUAttr attr;

	err = m_reader.getNextRecord(attr);
	if (err == KDU_E_EOF) return KLAV_EOF;
	if (err != KDU_E_OK || attr.type() != KDU_ATTR_TYPE_ID) return KLAV_EFORMAT;

	parm.id = attr.getValue ();

	err = m_reader.getRecordAttr(attr);
	if (err != KDU_E_OK || attr.type() != KDU_ATTR_TYPE_UINT) return KLAV_EFORMAT;

	parm.value = attr.getValue ();

	return KLAV_OK;
}

////////////////////////////////////////////////////////////////
// SectionEnum

void KdbManifestReader::SectionEnum::init (const uint8_t* dbListSection, uint32_t dbListSectionSize)
{
	m_reader.init (dbListSection, dbListSectionSize);
}

KLAV_ERR KdbManifestReader::SectionEnum::getNextSection (SectionInfo& sectInfo)
{
	KDUErr err = KDU_E_OK;

	sectInfo.clear ();

	KDUAttr attr;

	err = m_reader.getNextRecord(attr);
	if (err == KDU_E_EOF) return KLAV_EOF;

	if (err != KDU_E_OK || attr.type() != KDU_ATTR_TYPE_ID) return KLAV_EFORMAT;

	sectInfo.sectionType = attr.getValue ();
	sectInfo.mergerTypeFlags = attr.extraByte ();

	err = m_reader.getRecordAttr(attr);
	if (err == KDU_E_EOF) return KLAV_OK;
	if (err != KDU_E_OK) return KLAV_EFORMAT;

	if (attr.type () == KDU_ATTR_TYPE_ID) { // Fragment ID
		sectInfo.fragmentID = attr.getValue ();

		err = m_reader.getRecordAttr(attr);
		if (err == KDU_E_EOF) return KLAV_OK;
		if (err != KDU_E_OK) return KLAV_EFORMAT;
	}

	// size
	if (attr.type() != KDU_ATTR_TYPE_UINT) return KLAV_EFORMAT;
	sectInfo.size = attr.getValue ();

	err = m_reader.getRecordAttr (attr);
	// skip over signature (ignored for now)
//	if (err != KDU_E_OK || attr.type() != KDU_ATTR_TYPE_BINARY) return KLAV_EFORMAT;

//	sectInfo.signature = attr.getVector(&sectInfo.signatureLen);

	return KLAV_OK;
}

////////////////////////////////////////////////////////////////
// SectionInfo

void KdbManifestReader::SectionInfo::clear ()
{
	sectionType = 0;
	fragmentID = 0;
	mergerTypeFlags = 0;
	size = 0;
	signature = 0;
	signatureLen = 0;
}


