// kdb_info.cpp
//
//

#include "avp3info.h"
#include <AV/structs/s_avp3info.h>
#include <Prague/pr_time.h>

#include <klava/klav_prague.h>
#include <klava/kcdf_utils.h>

tERROR GetBaseFileInfo_KDB (hOBJECT hParent, const void *mft_data, tUINT mft_size, cAVP3BasesInfo* pInfo)
{
	tERROR error = errOK;

	class Handler : public KCDFTagHandler
	{
	public:
		Handler () :
			m_db_timestamp (0), m_db_record_count (0)
		{
		}

		uint32_t m_db_timestamp;
		uint32_t m_db_record_count;

		KLAV_ERR addTag(KDU_TAG tag, uint32_t value)
		{
			switch (tag)
			{
			case KDU_CDFILE_TAG_TIMESTAMP:
				m_db_timestamp = value;
				break;
			case KLAV_KERNEL_PARM_VERDICT_COUNT:
				m_db_record_count = value;
				break;
			default:
				break;
			}

			return KLAV_OK;
		}

	} handler;

	KLAV_Pr_Alloc alloc (hParent);

	if (KDB_Is_XML_Manifest (& alloc, mft_data, mft_size))
	{
		KLAV_ERR err = KDB_Read_XML_Manifest_Data (&alloc, 0, mft_data, mft_size, & handler, 0);
		if (KLAV_FAIL (err))
			return errOBJECT_DATA_CORRUPTED;
	}
	else
	{
		KdcManifestReader mft_reader;

		KLAV_ERR err = mft_reader.init ((const uint8_t *) mft_data, mft_size);
		if (KLAV_FAIL (err))
			return errOBJECT_DATA_CORRUPTED;

		err = mft_reader.listTags (& handler);
		if (KLAV_FAIL (err))
			return errOBJECT_DATA_CORRUPTED;
	}

	time_t db_time = (time_t) handler.m_db_timestamp + KDC_TIMESTAMP_BASE;

	cDateTime db_dt (db_time);
	db_dt.CopyTo (pInfo->m_dtBasesDate);
	pInfo->m_dwViruseCount = handler.m_db_record_count;

	return errOK;
}
