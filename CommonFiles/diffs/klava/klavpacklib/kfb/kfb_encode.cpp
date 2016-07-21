// kfb_encode.cpp
//

#include "kfb_pack.h"
#include <lzmalib/lzma_encode.h>

////////////////////////////////////////////////////////////////

bool Do_KFB_Pack (
			const unsigned char *oldData,
			size_t oldSize,
			std::vector<unsigned char> &resultBuffer
		)
{
	KFB2_FILE_HEADER oldHdr;
	KFB2_FILE_HEADER newHdr;

	if (oldSize < sizeof (oldHdr))
		return false;

	memcpy (&oldHdr, oldData, sizeof (oldHdr));
	if (oldHdr.kfb_magic[0] != KFB2_MAGIC_0 ||
		oldHdr.kfb_magic[1] != KFB2_MAGIC_1 ||
		oldHdr.kfb_magic[2] != KFB2_MAGIC_2 ||
		oldHdr.kfb_magic[3] != KFB2_MAGIC_3)
		// not a KFB
		return false;

	uint32_t sectRecSize = oldHdr.kfb_secrec_size * sizeof (uint32_t);

	if (oldHdr.kfb_hdrsize * sizeof (uint32_t) != sizeof oldHdr ||
		sectRecSize != sizeof (KFB2_SECTION_TABLE_RECORD) ||
		oldHdr.kfb_file_size > oldSize ||
		oldHdr.kfb_sectbl_off >= oldHdr.kfb_file_size ||
		oldHdr.kfb_sectbl_size >= oldHdr.kfb_file_size ||
		oldHdr.kfb_sectbl_off + oldHdr.kfb_sectbl_size > oldHdr.kfb_file_size)
		// some corruption
		return false;

	if ((oldHdr.kfb_flags & KFB_F_USES_COMPRESSION) != 0)
		// not an uncompressed KFB
		return false;

	if (((oldHdr.kfb_flags & KFB_F_BIG_ENDIAN) != 0) != KFB_NATIVE_ENDIAN)
		// non-native byte order not supported here
		return false;

//	uint32_t pageSize = uint32_t(1) << oldHdr.kfb_pageshift;
//	if (sizeof oldHdr > pageSize)
//		// some corruption
//		return false;

	newHdr = oldHdr;
	newHdr.kfb_flags |= KFB_F_USES_COMPRESSION;

	const KFB2_SECTION_TABLE_RECORD * oldSectionTable = (const KFB2_SECTION_TABLE_RECORD *)(oldData + oldHdr.kfb_sectbl_off);
	uint32_t sectionCnt = oldHdr.kfb_sectbl_size / sizeof (KFB2_SECTION_TABLE_RECORD);

	if (sectionCnt * sizeof (KFB2_SECTION_TABLE_RECORD) != oldHdr.kfb_sectbl_size)
		// some corruption
		return false;

	SectionTable newSectionTable;
	uint32_t totalSize = sizeof (KFB2_FILE_HEADER);

	for (uint8_t mode = KFB_SECT_F_RESIDENT; mode >= KFB_SECT_F_NONLOADABLE; --mode)
	{
		for (uint32_t i = 0; i < sectionCnt; ++i)
		{
			const KFB2_SECTION_TABLE_RECORD& oldSection = oldSectionTable[i];
			if ((oldSection.kfb_section_flags & KFB_SECT_F_LOADABILITY_MASK) != mode)
				continue;

			// First of all, decide if compressing the section is effective
			std::vector<uint8_t> compressionBuf;

			// The compressed data size must not be greater than that of uncompressed data,
			// but we need some more of the buffer to prevent the algorithm from cycling
			unsigned compressedDataSize = oldSection.kfb_section_usize + 0x100;

			if (compressedDataSize < oldSection.kfb_section_usize ||
				compressedDataSize != oldSection.kfb_section_usize + 0x100)
				// paranoia
				return false;

			if ((oldSection.kfb_section_flags & KFB_SECT_F_CRC32) != 0)
			{
				uint32_t crc = KlavCRC32 (oldData + oldSection.kfb_section_off, oldSection.kfb_section_usize, 0);
				if (crc != oldSection.kfb_section_crc32)
					return false; // CRC mismatch
			}

			uint32_t off = 0;
			uint32_t idx = 0;

			compressionBuf.resize(compressedDataSize);
			int r = LzmaEncodeMem2Mem(oldData + oldSection.kfb_section_off, oldSection.kfb_section_usize, &compressionBuf.front(), &compressedDataSize);

			if (r != LZMA_ENCODE_ERR_OK || compressedDataSize >= oldSection.kfb_section_usize)
			{
				// compression failed or gave no effect
				// store the section uncompressed
				allocSectionSpace(oldSection.kfb_section_usize, oldSection.kfb_section_alignment, newSectionTable, &off, &idx, &totalSize);
				KFB2_SECTION_TABLE_RECORD& newSection = newSectionTable[idx];
				newSection.kfb_section_flags = oldSection.kfb_section_flags | KFB_SECT_F_CRC32;
				newSection.kfb_section_type = oldSection.kfb_section_type;
				newSection.kfb_fragment_id = oldSection.kfb_fragment_id;
				newSection.kfb_section_usize = oldSection.kfb_section_usize;
				newSection.kfb_section_alignment = oldSection.kfb_section_alignment;
				newSection.kfb_section_crc32 = KlavCRC32 (oldData + oldSection.kfb_section_off, oldSection.kfb_section_usize, 0);

				resultBuffer.resize(totalSize);

				memcpy(&resultBuffer.front() + off, oldData + oldSection.kfb_section_off, oldSection.kfb_section_usize);
			}
			else
			{
				// compression was effective
				uint32_t size = compressedDataSize;

				if (size != compressedDataSize)
					// paranoia
					return false;

				allocSectionSpace(size, KFB_ALIGNMENT_1, newSectionTable, &off, &idx, &totalSize);
				KFB2_SECTION_TABLE_RECORD& newSection = newSectionTable[idx];
				newSection.kfb_section_flags = oldSection.kfb_section_flags | KFB_SECT_F_COMPRESSED | KFB_SECT_F_CRC32;
				newSection.kfb_section_type = oldSection.kfb_section_type;
				newSection.kfb_fragment_id = oldSection.kfb_fragment_id;
				newSection.kfb_section_usize = oldSection.kfb_section_usize;
				newSection.kfb_section_cmethod = KFB_COMPRESSION_METHOD_LZMA;
				newSection.kfb_section_alignment = oldSection.kfb_section_alignment;
				newSection.kfb_section_crc32 = KlavCRC32 (& compressionBuf.front (), compressedDataSize, 0);

				resultBuffer.resize(totalSize);

				memcpy(&resultBuffer.front() + off, &compressionBuf.front(), compressedDataSize);
			}
		}
	}

	SectionTable tmp = newSectionTable;
	uint32_t dummy2;

	newHdr.kfb_sectbl_size = (uint32_t) newSectionTable.size() * sizeof (KFB2_SECTION_TABLE_RECORD);
	allocSectionSpace(newHdr.kfb_sectbl_size, KFB_ALIGNMENT_16, tmp, &newHdr.kfb_sectbl_off, &dummy2, &totalSize);

	resultBuffer.resize(totalSize);
	memcpy(&resultBuffer.front() + newHdr.kfb_sectbl_off, &newSectionTable.front(), newHdr.kfb_sectbl_size);

	newHdr.kfb_file_size = totalSize;
	memcpy(&resultBuffer.front(), &newHdr, sizeof newHdr);

	return true;
}

////////////////////////////////////////////////////////////////

bool KlavPack_KFB_EncodeVector (
			const std::vector<unsigned char> &originalBuffer,
			std::vector<unsigned char> &resultBuffer
		)
{
	return Do_KFB_Pack (& originalBuffer [0], originalBuffer.size (), resultBuffer);
}

bool Diff_KFB_Pack (
		const unsigned char *unp_data,
		size_t unp_size,
		Diff_Buffer *pk_buf
	)
{
	memset (pk_buf, 0, sizeof (Diff_Buffer));

	std::vector <unsigned char> * outbuf = new std::vector <unsigned char> ();
	if (outbuf == 0)
		return false;

	if (! Do_KFB_Pack (unp_data, unp_size, *outbuf))
	{
		delete outbuf;
		return false;
	}

	pk_buf->m_data = & ((*outbuf) [0]);
	pk_buf->m_size = outbuf->size ();
	pk_buf->m_pimpl = outbuf;

	return true;
}

////////////////////////////////////////////////////////////////
