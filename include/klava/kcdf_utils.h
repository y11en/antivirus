// kcdf_utils.h
//

#ifndef kcdf_utils_h_INCLUDED
#define kcdf_utils_h_INCLUDED

#ifdef __cplusplus

#include <kl_types.h>

#include <klava/klaverr.h>
#include <klava/kdc.h>
#include <klava/kdu.h>
#include <klava/kdu_utils.h>

struct KLAV_Bin_Writer;

// Tags of CDF file

class KCDFTagHandler
{
public:
	virtual KLAV_ERR addTag(KDU_TAG tag, uint32_t value) = 0;
};

KLAV_ERR KCDF_readTags(void const* subhdrData, uint32_t subhdrSize, KCDFTagHandler* handler);

inline KLAV_ERR KCDF_readTags(KDUBlockInfo const& blockInfo, KCDFTagHandler* handler)
{
	return KCDF_readTags(blockInfo.subhdr, blockInfo.subhdr_len, handler);
}

class KCDFTagWriter :
	public KCDFTagHandler
{
public:
	KCDFTagWriter(KLAV_Bin_Writer* writer);

	KLAV_ERR addTag(KDU_TAG tag, uint32_t value);

private:
	KLAV_Bin_Writer* m_writer;
};

// Cluster section of CDF file

class KCDFClusterHandler
{
public:
	virtual KLAV_ERR startSet(uint32_t set_id) = 0;
	virtual KLAV_ERR startCluster() = 0;
	virtual KLAV_ERR addProperty(KCDF_TAG tag, uint32_t value) = 0;
	virtual KLAV_ERR addProperty(KCDF_TAG tag, char const* str, uint32_t len) = 0;
	virtual KLAV_ERR addProperty(KCDF_TAG tag, uint8_t const* bin, uint32_t len) = 0;
	virtual KLAV_ERR endCluster() = 0;
};

KLAV_ERR KCDF_readClusterListSection(void const* sectionData, uint32_t sectionSize, KCDFClusterHandler* handler);

inline KLAV_ERR KCDF_readClusterListSection(KDUSectionInfo const& sectionInfo, KCDFClusterHandler* handler)
{
	return KCDF_readClusterListSection(sectionInfo.data, sectionInfo.size, handler);
}

class KCDFClusterWriter :
	public KCDFClusterHandler
{
public:
	KCDFClusterWriter(KLAV_Bin_Writer* writer);

	KLAV_ERR startSet(uint32_t set_id);
	KLAV_ERR startCluster();
	KLAV_ERR addProperty(KCDF_TAG tag, uint32_t value);
	KLAV_ERR addProperty(KCDF_TAG tag, char const* str, uint32_t len);
	KLAV_ERR addProperty(KCDF_TAG tag, uint8_t const* bin, uint32_t len);
	KLAV_ERR endCluster();

private:
	KLAV_Bin_Writer* m_writer;
	bool m_clusterStart;
};

// Manifest as a whole

class KdcManifestReader
{
public:
	KdcManifestReader();

	// Process manifest - all sections ignored except cluster list
	KLAV_ERR init(const uint8_t* buffer, uint32_t size);

	KLAV_ERR listTags(KCDFTagHandler* handler);
	KLAV_ERR listClusters(KCDFClusterHandler* handler);

	KLAV_ERR get_db_tag(KDU_TAG tag, uint32_t& value);

protected:
	uint8_t const* m_mftSubheaderData;
	uint32_t m_mftSubheaderSize;

	uint8_t const* m_clusterSectionData;
	uint32_t m_clusterSectionSize;

	void clear();
};

struct KLAV_Alloc;

class KdcManifestWriter :
	public KCDFClusterHandler,
	public KCDFTagHandler
{
public:
	KdcManifestWriter(KLAV_Alloc *alloc);
	~KdcManifestWriter();

	void clear ();

	void clearTags();
	KLAV_ERR addTag(KDU_TAG tag, uint32_t value);

	void clearClusters();
	
	KLAV_ERR startSet (uint32_t set_id);
	KLAV_ERR startCluster();
	KLAV_ERR addProperty(KCDF_TAG tag, uint32_t value);
	KLAV_ERR addProperty(KCDF_TAG tag, char const* str, uint32_t len);
	KLAV_ERR addProperty(KCDF_TAG tag, uint8_t const* bin, uint32_t len);
	KLAV_ERR endCluster();

	KLAV_ERR write (KLAV_Bin_Writer *writer);

private:
	class KdcManifestWriterImpl *m_pimpl;
};

// XML-based manifest reader
klav_bool_t KDB_Is_XML_Manifest (
			hKLAV_Alloc alloc,
			const void *mft_data,
			size_t mft_size
		);

KLAV_ERR KDB_Read_XML_Manifest (
			hKLAV_Alloc alloc,
			hKLAV_DB_Manager db_mgr,
			const char *set_name,
			KCDFTagHandler* tag_handler,     // may be 0
			KCDFClusterHandler* kdc_handler  // may be 0
		);

KLAV_ERR KDB_Read_XML_Manifest_Data (
			hKLAV_Alloc alloc,
			hKLAV_DB_Manager db_mgr,
			const void *mft_data,
			size_t mft_size,
			KCDFTagHandler* tag_handler,     // may be 0
			KCDFClusterHandler* kdc_handler  // may be 0
		);

#endif // __cplusplus

#endif // kcdf_utils_h_INCLUDED
