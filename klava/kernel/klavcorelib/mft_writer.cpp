// mft_writer.cpp
//

#include <klava/kdb_utils.h>
#include <klava/klavstl/vector.h>

#include <klava/kdu_utils.h>

#include <string.h>

#ifdef _MSC_VER
# pragma intrinsic (memcpy)
#endif

////////////////////////////////////////////////////////////////
// KdbManifestWriterImpl

class KdbManifestWriterImpl
{
public:
		KdbManifestWriterImpl (KLAV_Alloc *alloc)
			: m_allocator (alloc), m_dbVersion (0),
			  m_kernelParmTable (alloc), m_sectionTable (alloc) {}

		~KdbManifestWriterImpl () {}

	void destroy ()
		{ KLAV_DELETE (this, m_allocator); }

	void clear ();

	KLAV_ERR copy (KdbManifestReader& reader);

	void set_db_version (uint32_t db_release, uint32_t db_version)
		{ m_dbVersion = db_version; m_dbIncarnation = db_release; }

	uint32_t getKernelParm (uint32_t id);
	KLAV_ERR setKernelParm (uint32_t id, uint32_t value);
	void clearKernelParm (uint32_t id);
	void clearKernelParms ();

	void clearSections ();
	KLAV_ERR addSection (const KdbManifestSectionInfo& sectInfo);
	KLAV_ERR removeSection (uint32_t sectionType, uint32_t fragmentID);

	KLAV_ERR write (KLAV_Bin_Writer *writer, bool writeSignatures);

private:
	KLAV_Alloc *     m_allocator;
	uint32_t        m_dbVersion;
	uint32_t		m_dbIncarnation;

	typedef KdbManifestKernelParm KernelParm;

	struct SectionInfo : public KdbManifestSectionInfo
	{
		KLAV_Alloc * m_allocator;

		SectionInfo (KLAV_Alloc *alloc = 0) : m_allocator (alloc)
			{ signature = 0; signatureLen = 0; }

		~SectionInfo ()
			{ clear (); }

		KLAV_ERR copy (const KdbManifestSectionInfo& sect);
		void clear ();
	};

	typedef klavstl::trivial_vector <KernelParm, klav_allocator> KernelParmTable;
	KernelParmTable m_kernelParmTable;

	typedef klavstl::trivial_vector <SectionInfo, klav_allocator> SectionTable;
	SectionTable    m_sectionTable;
};

KLAV_ERR KdbManifestWriterImpl::SectionInfo::copy (const KdbManifestSectionInfo& sect)
{
	clear ();
	
	sectionType = sect.sectionType;
	fragmentID = sect.fragmentID;
	mergerTypeFlags = sect.mergerTypeFlags;
	size = sect.size;

	if (sect.signature != 0)
	{
		signature = (const uint8_t *)m_allocator->alloc (sect.signatureLen);
		if (signature == 0) return KLAV_ENOMEM;

		memcpy ((void *)signature, sect.signature, sect.signatureLen);
		signatureLen = sect.signatureLen;
	}

	return KLAV_OK;
}

void KdbManifestWriterImpl::SectionInfo::clear ()
{
	if (signature != 0)
	{
		m_allocator->free ((void *)signature);
		signature = 0;
	}
	signatureLen = 0;
}

KLAV_ERR KdbManifestWriterImpl::copy (KdbManifestReader& reader)
{
	clear ();

	m_dbVersion = reader.get_db_version ();

	KdbManifestReader::KernelParmEnum parmEnum = reader.enumKernelParms ();
	KdbManifestKernelParm parm;

	for (;;)
	{
		KLAV_ERR err = parmEnum.getNext (parm);
		if (err == KLAV_EOF) break;
		if (err != KLAV_OK) return err;

		err = setKernelParm (parm.id, parm.value);;
		if (err != KLAV_OK) return err;
	}

	KdbManifestReader::SectionEnum sectEnum = reader.enumSections ();
	KdbManifestSectionInfo sectInfo;

	for (;;) {
		KLAV_ERR err = sectEnum.getNextSection (sectInfo);
		if (err == KLAV_EOF) break;
		if (err != KLAV_OK) return err;

		err = addSection (sectInfo);
		if (err != KLAV_OK) return err;
	}

	return KLAV_OK;
}

void KdbManifestWriterImpl::clear ()
{
	m_dbVersion = 0;
	m_dbIncarnation = 0;
	clearKernelParms ();
	clearSections ();
}

uint32_t KdbManifestWriterImpl::getKernelParm (uint32_t id)
{
	KernelParmTable::const_iterator iter = m_kernelParmTable.begin ();
	KernelParmTable::const_iterator iend = m_kernelParmTable.end ();

	for (; iter != iend; ++iter)
	{
		const KernelParm& parm = *iter;
		if (parm.id == id) return parm.value;
	}

	return 0;
}

KLAV_ERR KdbManifestWriterImpl::setKernelParm (uint32_t id, uint32_t value)
{
	KernelParmTable::iterator iter = m_kernelParmTable.begin ();
	KernelParmTable::iterator iend = m_kernelParmTable.end ();

	for (; iter != iend; ++iter)
	{
		KernelParm& parm = *iter;
		if (parm.id == id) {
			parm.value = value;
			return KLAV_OK;
		}
	}

	KernelParm parm;
	parm.id = id;
	parm.value = value;

	if (! m_kernelParmTable.push_back (parm)) return KLAV_ENOMEM;
	return KLAV_OK;
}

void KdbManifestWriterImpl::clearKernelParm (uint32_t id)
{
	KernelParmTable::iterator iter = m_kernelParmTable.begin ();
	KernelParmTable::iterator iend = m_kernelParmTable.end ();

	for (; iter != iend; ++iter)
	{
		const KernelParm& parm = *iter;
		if (parm.id == id) {
			m_kernelParmTable.erase (iter);
			return;
		}
	}
}

void KdbManifestWriterImpl::clearKernelParms ()
{
	m_kernelParmTable.clear ();
}

void KdbManifestWriterImpl::clearSections ()
{
	m_sectionTable.clear ();
}

KLAV_ERR KdbManifestWriterImpl::addSection (const KdbManifestSectionInfo& sect)
{
	SectionTable::const_iterator iter = m_sectionTable.begin ();
	SectionTable::const_iterator iend = m_sectionTable.end ();

	for (; iter != iend; ++iter)
	{
		const SectionInfo& sectInfo = *iter;
		if (sectInfo.sectionType == sect.sectionType && sectInfo.fragmentID == sect.fragmentID)
		{
			return KLAV_EDUPLICATE;
		}
	}

	size_t idx = m_sectionTable.size ();
	if (! m_sectionTable.resize (idx + 1)) return KLAV_ENOMEM;

	KLAV_ERR err = m_sectionTable[idx].copy (sect);
	if (err != KLAV_OK) {
		m_sectionTable.resize (idx);
		return err;
	}

	return KLAV_OK;
}

KLAV_ERR KdbManifestWriterImpl::removeSection (uint32_t sectionType, uint32_t fragmentID)
{
	SectionTable::iterator iter = m_sectionTable.begin ();
	SectionTable::iterator iend = m_sectionTable.end ();

	for (; iter != iend; ++iter)
	{
		SectionInfo& sectInfo = *iter;
		if (sectInfo.sectionType == sectionType && sectInfo.fragmentID == fragmentID)
		{
			m_sectionTable.erase (iter);
			return KLAV_OK;
		}
	}

	return KLAV_ENOTFOUND;
}

class KduBufWriter : public KLAV_Buffer_Writer <klavstl::buffer <klav_allocator> >
{
public:
	typedef KLAV_Buffer_Writer <klavstl::buffer <klav_allocator> > Base;

	KduBufWriter (KLAV_Alloc *alloc) : Base (m_buffer), m_buffer (alloc) {}
	virtual ~KduBufWriter () {}

	void clear ()
		{ m_buffer.clear (); }

	const uint8_t *data () const
		{ return &m_buffer.front (); }

	uint32_t size () const
		{ return (uint32_t)m_buffer.size (); }

private:
	klavstl::buffer <klav_allocator> m_buffer;
};

KLAV_ERR KdbManifestWriterImpl::write (KLAV_Bin_Writer *writer, bool writeSignatures)
{
	KduBufWriter buf (m_allocator);
	KduBufWriter blkBuf (m_allocator);

	{
		KernelParmTable::const_iterator iter = m_kernelParmTable.begin ();
		KernelParmTable::const_iterator iend = m_kernelParmTable.end ();

		for (; iter != iend; ++iter)
		{
			const KernelParm& parm = *iter;
			if (KDUWriteAttr (&buf, KDU_ATTR_TYPE_ID, parm.id, KDU_ATTR_F_SOR) != KDU_E_OK) return KLAV_ENOMEM;
			if (KDUWriteAttr (&buf, KDU_ATTR_TYPE_UINT, parm.value) != KDU_E_OK) return KLAV_ENOMEM;
		}
	}

	KDUSectionInfo sect;
	sect.data = buf.data ();
	sect.size = buf.size ();
	sect.sectionType = KDBID_KERNEL_PARMS;
	sect.fragmentID = 0;
	sect.raw = 0;

	if (KDUWriteSection (&blkBuf, sect) != KDU_E_OK) return KLAV_ENOMEM;

	buf.clear ();

	{
		SectionTable::const_iterator iter = m_sectionTable.begin ();
		SectionTable::const_iterator iend = m_sectionTable.end ();

		for (; iter != iend; ++iter)
		{
			const SectionInfo& sectInfo = *iter;

			if (KDUWriteAttr (&buf, KDU_ATTR_TYPE_ID, sectInfo.sectionType, KDU_ATTR_F_SOR, (uint8_t)sectInfo.mergerTypeFlags) != KDU_E_OK) return KLAV_ENOMEM;

			if (sectInfo.fragmentID != 0)
			{
				if (KDUWriteAttr (&buf, KDU_ATTR_TYPE_ID, sectInfo.fragmentID) != KDU_E_OK) return KLAV_ENOMEM;
			}

			if (writeSignatures)
			{
				if (KDUWriteAttr (&buf, KDU_ATTR_TYPE_UINT, sectInfo.size) != KDU_E_OK) return KLAV_ENOMEM;
				if (KDUWriteVectorAttr (&buf, KDU_ATTR_TYPE_STRING, sectInfo.signature, sectInfo.signatureLen) != KDU_E_OK) return KLAV_ENOMEM;
			}
		}
	}
	
	sect.data = buf.data ();
	sect.size = buf.size ();
	sect.sectionType = KDBID_DBFLIST;
	sect.fragmentID = 0;
	sect.raw = 0;

	if (KDUWriteSection (&blkBuf, sect) != KDU_E_OK) return KLAV_ENOMEM;

	KDU_SUB_HEADER_DBFILE_V2 subhdr;
	KDU_SUBHDR_DBFILE_V2_FILL (&subhdr, m_dbVersion, m_dbIncarnation);

	KDUBlockInfo blk;
	blk.data = blkBuf.data ();
	blk.size = blkBuf.size ();
	blk.subhdr = (const uint8_t *)&subhdr;
	blk.subhdr_len = KDU_SUBHDR_DBFILE_V2_SIZE;
	blk.type = KDU_BLOCK_TYPE_DBFILE;
	if (KDUWriteBlock (writer, blk) != KDU_E_OK) return KLAV_ENOMEM;

	return KLAV_OK;
}

////////////////////////////////////////////////////////////////
// KdbManifestWriter

KdbManifestWriter::KdbManifestWriter (KLAV_Alloc *alloc) : m_pimpl (0)
{
	m_pimpl = KLAV_NEW (alloc) KdbManifestWriterImpl (alloc);
}

KdbManifestWriter::~KdbManifestWriter ()
{
	if (m_pimpl != 0)
	{
		m_pimpl->destroy ();
		m_pimpl = 0;
	}
}

KLAV_ERR KdbManifestWriter::copy (KdbManifestReader& reader)
{
	if (m_pimpl == 0) return KLAV_ENOMEM;
	return m_pimpl->copy (reader);
}

void KdbManifestWriter::clear ()
{
	if (m_pimpl != 0) m_pimpl->clear ();
}

uint32_t KdbManifestWriter::getKernelParm (uint32_t id)
{
	if (m_pimpl == 0) return 0;
	return m_pimpl->getKernelParm (id);
}

KLAV_ERR KdbManifestWriter::setKernelParm (uint32_t id, uint32_t value)
{
	if (m_pimpl == 0) return KLAV_ENOMEM;
	return m_pimpl->setKernelParm (id, value);
}

void KdbManifestWriter::clearKernelParm (uint32_t id)
{
	m_pimpl->clearKernelParm (id);
}

void KdbManifestWriter::clearKernelParms ()
{
	m_pimpl->clearKernelParms ();
}

void KdbManifestWriter::set_db_version (uint32_t db_release, uint32_t db_version)
{
	if (m_pimpl != 0)
		m_pimpl->set_db_version (db_release, db_version);
}

void KdbManifestWriter::clearSections ()
{
	if (m_pimpl != 0)
		m_pimpl->clearSections ();
}

KLAV_ERR KdbManifestWriter::addSection (const SectionInfo& info)
{
	if (m_pimpl == 0)
		return KLAV_ENOMEM;
	return m_pimpl->addSection (info);
}

KLAV_ERR KdbManifestWriter::removeSection (uint32_t sectionType, uint32_t fragmentID)
{
	if (m_pimpl == 0)
		return KLAV_ENOMEM;
	return m_pimpl->removeSection (sectionType, fragmentID);
}

KLAV_ERR KdbManifestWriter::write (KLAV_Bin_Writer *writer, bool writeSignatures)
{
	if (m_pimpl == 0)
		return KLAV_ENOMEM;
	return m_pimpl->write (writer, writeSignatures);
}





