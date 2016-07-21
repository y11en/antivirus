// kcdf_utils.cpp
//

#include <klava/kcdf_utils.h>
#include <klava/kdb_ids.h>
#include <klava/klavdef.h>
#include <klava/klavstl/vector.h>

KLAV_ERR KCDF_readTags(void const* subhdrData, uint32_t subhdrSize, KCDFTagHandler* handler)
{
	KDUAttrReader attrReader(subhdrData, subhdrSize);

	while (!attrReader.atEnd())
	{
		if (!attrReader.atStartOfRecord())
			return KLAV_EFORMAT;

		KLAV_ERR handlerErr;
		KDUAttr attr;
		KDUErr err;

		err = attrReader.getNextAttr(attr);
		if (err || attr.type() != KDU_ATTR_TYPE_UINT || attrReader.atEndOfRecord())
			return KLAV_EFORMAT;
		KCDF_TAG tag = attr.getValue();

		err = attrReader.getNextAttr(attr);
		if (err || attr.type() != KDU_ATTR_TYPE_UINT || !(attrReader.atEndOfRecord() || attrReader.atEnd()))
			return KLAV_EFORMAT;
		uint32_t value = attr.getValue();

		handlerErr = handler->addTag(tag, value);
		if (!KLAV_SUCCEEDED(handlerErr))
			return handlerErr;
	}

	return KLAV_OK;
}

KCDFTagWriter::KCDFTagWriter(KLAV_Bin_Writer* writer) :
	m_writer(writer)
{
}

KLAV_ERR KCDFTagWriter::addTag(KDU_TAG tag, uint32_t value)
{
	if (KDUWriteAttr(m_writer, KDU_ATTR_TYPE_UINT, tag, KDU_ATTR_F_SOR))
		return KLAV_EUNKNOWN;
	if (KDUWriteAttr(m_writer, KDU_ATTR_TYPE_UINT, value))
		return KLAV_EUNKNOWN;
	return KLAV_OK;
}

KLAV_ERR KCDF_readClusterListSection(void const* sectionData, uint32_t sectionSize, KCDFClusterHandler* handler)
{
	KDUAttrReader attrReader(sectionData, sectionSize);

	KLAV_ERR handlerErr;

	handlerErr = handler->startSet(0);
	if (!KLAV_SUCCEEDED(handlerErr))
		return handlerErr;

	while (!attrReader.atEnd())
	{
		if (!attrReader.atStartOfRecord())
			return KLAV_EFORMAT;

		KDUAttr attr;
		KDUErr err;

		handlerErr = handler->startCluster();
		if (!KLAV_SUCCEEDED(handlerErr))
			return handlerErr;

		do 
		{
			err = attrReader.getNextAttr(attr);
			if (err || attr.type() != KDU_ATTR_TYPE_UINT || attrReader.atEndOfRecord())
				return KLAV_EFORMAT;
			KCDF_TAG tag = attr.getValue();
			err = attrReader.getNextAttr(attr);
			if (err)
				return KLAV_EFORMAT;
			switch (attr.type())
			{
			case KDU_ATTR_TYPE_UINT:
				handlerErr = handler->addProperty(tag, attr.getValue());
				break;
			case KDU_ATTR_TYPE_STRING:
				{
					uint32_t s;
					uint8_t const* p = attr.getVector(&s);
					handlerErr = handler->addProperty(tag, reinterpret_cast<char const*>(p), s);
				} break;
			case KDU_ATTR_TYPE_BINARY:
				{
					uint32_t s;
					uint8_t const* p = attr.getVector(&s);
					handlerErr = handler->addProperty(tag, p, s);
				} break;
			default:
				return KLAV_EFORMAT;
			}

			if (!KLAV_SUCCEEDED(handlerErr))
				return handlerErr;
		} while (!attrReader.atEnd() && !attrReader.atEndOfRecord());

		handlerErr = handler->endCluster();
		if (!KLAV_SUCCEEDED(handlerErr))
			return handlerErr;
	}

	return KLAV_OK;
}

KCDFClusterWriter::KCDFClusterWriter(KLAV_Bin_Writer* writer) :
	m_writer(writer),
	m_clusterStart(true)
{}

KLAV_ERR KCDFClusterWriter::startSet (uint32_t set_id)
{
	return KLAV_OK;
}

KLAV_ERR KCDFClusterWriter::startCluster()
{
	m_clusterStart = true;
	return KLAV_OK;
}

KLAV_ERR KCDFClusterWriter::addProperty(KCDF_TAG tag, uint32_t value)
{
	uint8_t flags = 0;
	if (m_clusterStart)
	{
		flags = KDU_ATTR_F_SOR;
		m_clusterStart = false;
	}
	if (KDUWriteAttr(m_writer, KDU_ATTR_TYPE_UINT, tag, flags))
		return KLAV_EUNKNOWN;
	if (KDUWriteAttr(m_writer, KDU_ATTR_TYPE_UINT, value))
		return KLAV_EUNKNOWN;
	return KLAV_OK;
}

KLAV_ERR KCDFClusterWriter::addProperty(KCDF_TAG tag, char const* str, uint32_t len)
{
	uint8_t flags = 0;
	if (m_clusterStart)
	{
		flags = KDU_ATTR_F_SOR;
		m_clusterStart = false;
	}
	if (KDUWriteAttr(m_writer, KDU_ATTR_TYPE_UINT, tag, flags))
		return KLAV_EUNKNOWN;
	if (KDUWriteVectorAttr(m_writer, KDU_ATTR_TYPE_STRING, str, len))
		return KLAV_EUNKNOWN;
	return KLAV_OK;
}

KLAV_ERR KCDFClusterWriter::addProperty(KCDF_TAG tag, uint8_t const* bin, uint32_t len)
{
	uint8_t flags = 0;
	if (m_clusterStart)
	{
		flags = KDU_ATTR_F_SOR;
		m_clusterStart = false;
	}
	if (KDUWriteAttr(m_writer, KDU_ATTR_TYPE_UINT, tag, flags))
		return KLAV_EUNKNOWN;
	if (KDUWriteVectorAttr(m_writer, KDU_ATTR_TYPE_BINARY, bin, len))
		return KLAV_EUNKNOWN;
	return KLAV_OK;
}

KLAV_ERR KCDFClusterWriter::endCluster()
{
	m_clusterStart = true;
	return KLAV_OK;
}

KdcManifestReader::KdcManifestReader()
{
	clear();
}

KLAV_ERR KdcManifestReader::init(const uint8_t* buffer, uint32_t size)
{
	clear ();

	KDUBlockReader blockReader(buffer, size);

	KDUBlockInfo blk;
	KDUErr kduErr = blockReader.getNextBlock(blk);

	if (kduErr != KDU_E_OK) return KLAV_EFORMAT;
	if (blk.type != KDU_BLOCK_TYPE_CDFILE) return KLAV_EFORMAT;

// 	if (blk.subhdr_len != sizeof (KDU_SUB_HEADER_CDFILE))
// 		return KLAV_EFORMAT;

// 	const KDU_SUB_HEADER_CDFILE *subhdr = (const KDU_SUB_HEADER_CDFILE *)blk.subhdr;
// 	m_dbVersion = KDU_SUBHDR_CDFILE_SEQ(subhdr);
// 	m_dbRelease = KDU_SUBHDR_CDFILE_RELEASE(subhdr);

	m_mftSubheaderData = blk.subhdr;
	m_mftSubheaderSize = blk.subhdr_len;

	KDUSectionReader sectionReader(blk.data, blk.size);
	KDUSectionInfo sect;

	while (!sectionReader.atEnd())
	{
		kduErr = sectionReader.getNextSection(sect);
		if (kduErr != KDU_E_OK) return KLAV_EFORMAT;

		switch (sect.sectionType)
		{
		case KDBID_CLUSTERLIST:
			if (sect.fragmentID != 0 || sect.raw || m_clusterSectionData)
				return KLAV_EFORMAT;
			m_clusterSectionData = sect.data;
			m_clusterSectionSize = sect.size;
			break;
		}
	}

	if (!m_clusterSectionData)
		return KLAV_EFORMAT;

	return KLAV_OK;
}

KLAV_ERR KdcManifestReader::get_db_tag(KDU_TAG tag, uint32_t& value)
{
	KDUAttrReader attrReader(m_mftSubheaderData, m_mftSubheaderSize);

	while (!attrReader.atEnd())
	{
		KDUAttr attr;
		KDUErr err;

		if (!attrReader.atStartOfRecord())
			return KLAV_ECORRUPT;

		err = attrReader.getNextAttr(attr);
		if (err || attr.type() != KDU_ATTR_TYPE_UINT)
			return KLAV_ECORRUPT;

		uint32_t currentTag = attr.getValue();

		if (attrReader.atEndOfRecord())
			return KLAV_ECORRUPT;

		err = attrReader.getNextAttr(attr);
		if (err)
			return KLAV_ECORRUPT;

		if (currentTag != tag)
			continue;

		if (attr.type() != KDU_ATTR_TYPE_UINT)
			continue;

		value = attr.getValue();
		return KLAV_OK;
	}

	return KLAV_ENOTFOUND;
}

KLAV_ERR KdcManifestReader::listTags(KCDFTagHandler* handler)
{
	if (m_mftSubheaderData)
		return KCDF_readTags(m_mftSubheaderData, m_mftSubheaderSize, handler);
	else
		return KLAV_ENOINIT;
}

KLAV_ERR KdcManifestReader::listClusters(KCDFClusterHandler* handler)
{
	if (m_clusterSectionData)
		return KCDF_readClusterListSection(m_clusterSectionData, m_clusterSectionSize, handler);
	else
		return KLAV_ENOINIT;
}

void KdcManifestReader::clear()
{
	m_mftSubheaderData = 0;
	m_mftSubheaderSize = 0;
	m_clusterSectionData = 0;
	m_clusterSectionSize = 0;
}

class KdcManifestWriterImpl
{
public:
	KdcManifestWriterImpl (KLAV_Alloc *alloc) :
		m_allocator (alloc),
		m_clusterListBuf(alloc),
		m_clusterTags(alloc),
		m_bufferWriter(m_clusterListBuf),
		m_writer(&m_bufferWriter)
		{}

	void destroy ()
		{ KLAV_DELETE (this, m_allocator); }

	void clear ()
		{
			clearTags();
			clearClusters();
		};

	void clearTags()
		{ m_clusterTags.clear(); }
	KLAV_ERR addTag(KDU_TAG tag, uint32_t value);

	void clearClusters()
		{ m_clusterListBuf.clear(); }

	KLAV_ERR startSet (uint32_t set_id);
	KLAV_ERR startCluster();
	KLAV_ERR addProperty(KCDF_TAG tag, uint32_t value);
	KLAV_ERR addProperty(KCDF_TAG tag, char const* str, uint32_t len);
	KLAV_ERR addProperty(KCDF_TAG tag, uint8_t const* bin, uint32_t len);
	KLAV_ERR endCluster();

	KLAV_ERR write (KLAV_Bin_Writer *writer);

private:
	KLAV_Alloc *    m_allocator;

	typedef klavstl::trivial_vector<uint8_t, klav_allocator> ClusterListBuf;
	ClusterListBuf m_clusterListBuf;
	KLAV_Buffer_Writer<ClusterListBuf> m_bufferWriter;

	struct TagValue
	{
		KDU_TAG tag;
		uint32_t value;
	};

	typedef klavstl::trivial_vector<TagValue, klav_allocator> ClusterTagBuf;
	ClusterTagBuf m_clusterTags;

	KCDFClusterWriter m_writer;
};

KLAV_ERR KdcManifestWriterImpl::addTag(KDU_TAG tag, uint32_t value)
{
	size_t s = m_clusterTags.size();
	for (size_t i = 0; i < s; ++i)
	{
		if (m_clusterTags[i].tag == tag)
		{
			m_clusterTags[i].value = value;
			return KLAV_OK;
		}
	}

	TagValue tv = { tag, value };
	if (!m_clusterTags.push_back(tv))
		return KLAV_EUNKNOWN;
	return KLAV_OK;
}

KLAV_ERR KdcManifestWriterImpl::startCluster()
{
	return m_writer.startCluster();
}

KLAV_ERR KdcManifestWriterImpl::addProperty(KCDF_TAG tag, uint32_t value)
{
	return m_writer.addProperty(tag, value);
}

KLAV_ERR KdcManifestWriterImpl::addProperty(KCDF_TAG tag, char const* str, uint32_t len)
{
	return m_writer.addProperty(tag, str, len);
}

KLAV_ERR KdcManifestWriterImpl::addProperty(KCDF_TAG tag, uint8_t const* bin, uint32_t len)
{
	return m_writer.addProperty(tag, bin, len);
}

KLAV_ERR KdcManifestWriterImpl::endCluster()
{
	return m_writer.endCluster();
}

KLAV_ERR KdcManifestWriterImpl::write (KLAV_Bin_Writer *writer)
{
	typedef klavstl::trivial_vector<uint8_t, klav_allocator> SectionBuf;
	SectionBuf secBuf(m_allocator);
	KLAV_Buffer_Writer<SectionBuf> secWriter(secBuf);

	KDUSectionInfo sectionInfo;
	sectionInfo.data = reinterpret_cast<uint8_t const*>(&m_clusterListBuf.front());
	sectionInfo.size =  (uint32_t) m_clusterListBuf.size ();
	sectionInfo.raw = false;
	sectionInfo.sectionType = KDBID_CLUSTERLIST;
	sectionInfo.fragmentID = 0;

	if (KDUWriteSection(&secWriter, sectionInfo))
		return KLAV_EUNKNOWN;

	typedef klavstl::trivial_vector<uint8_t, klav_allocator> TagBuf;
	TagBuf tagBuf(m_allocator);
	KLAV_Buffer_Writer<TagBuf> tagWriter(tagBuf);
	size_t s = m_clusterTags.size();
	for (size_t i = 0; i < s; ++i)
	{
		if (KDUWriteAttr(&tagWriter, KDU_ATTR_TYPE_UINT, m_clusterTags[i].tag, KDU_ATTR_F_SOR) ||
			KDUWriteAttr(&tagWriter, KDU_ATTR_TYPE_UINT, m_clusterTags[i].value))
			return KLAV_EUNKNOWN;
	}

	KDUBlockInfo blockInfo;
	blockInfo.data = reinterpret_cast<uint8_t const*>(&secBuf.front());
	blockInfo.size = (uint32_t) secBuf.size();
	blockInfo.type = KDU_BLOCK_TYPE_CDFILE;
	blockInfo.subhdr = &tagBuf.front();
	blockInfo.subhdr_len = uint32_t(tagBuf.size());

	if (KDUWriteBlock(writer, blockInfo))
		return KLAV_EUNKNOWN;

	return KLAV_OK;
}

KdcManifestWriter::KdcManifestWriter(KLAV_Alloc *alloc) : m_pimpl (0)
{
	m_pimpl = KLAV_NEW (alloc) KdcManifestWriterImpl (alloc);
}

KdcManifestWriter::~KdcManifestWriter()
{
	if (m_pimpl != 0)
	{
		m_pimpl->destroy ();
		m_pimpl = 0;
	}
}

void KdcManifestWriter::clear ()
{
	if (m_pimpl)
		m_pimpl->clear();
}

void KdcManifestWriter::clearTags()
{
	if (m_pimpl)
		m_pimpl->clearTags();
}

KLAV_ERR KdcManifestWriter::addTag(KDU_TAG tag, uint32_t value)
{
	if (m_pimpl)
		return m_pimpl->addTag(tag, value);
	else
		return KLAV_ENOINIT;
}

void KdcManifestWriter::clearClusters()
{
	if (m_pimpl)
		m_pimpl->clearClusters();
}

KLAV_ERR KdcManifestWriter::startSet (uint32_t set_id)
{
	return KLAV_OK;
}

KLAV_ERR KdcManifestWriter::startCluster()
{
	if (m_pimpl)
		return m_pimpl->startCluster();
	else
		return KLAV_ENOINIT;
}

KLAV_ERR KdcManifestWriter::addProperty(KCDF_TAG tag, uint32_t value)
{
	if (m_pimpl)
		return m_pimpl->addProperty(tag, value);
	else
		return KLAV_ENOINIT;
}

KLAV_ERR KdcManifestWriter::addProperty(KCDF_TAG tag, char const* str, uint32_t len)
{
	if (m_pimpl)
		return m_pimpl->addProperty(tag, str, len);
	else
		return KLAV_ENOINIT;
}

KLAV_ERR KdcManifestWriter::addProperty(KCDF_TAG tag, uint8_t const* bin, uint32_t len)
{
	if (m_pimpl)
		return m_pimpl->addProperty(tag, bin, len);
	else
		return KLAV_ENOINIT;
}

KLAV_ERR KdcManifestWriter::endCluster()
{
	if (m_pimpl)
		return m_pimpl->endCluster();
	else
		return KLAV_ENOINIT;
}

KLAV_ERR KdcManifestWriter::write (KLAV_Bin_Writer *writer)
{
	if (m_pimpl)
		return m_pimpl->write(writer);
	else
		return KLAV_ENOINIT;
}
