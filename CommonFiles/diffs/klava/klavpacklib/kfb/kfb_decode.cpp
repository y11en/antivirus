// kfb_decode.cpp
//

#include "kfb_pack.h"
#include <lzmalib/lzma_encode.h>
#include <lzmalib/lzma_decode.h>

////////////////////////////////////////////////////////////////

bool KlavPack_KFB_IsPacked(
			const std::vector<unsigned char> &SourceVector
		)
{
	return Diff_KFB_IsPacked (&SourceVector [0], SourceVector.size ());
}

bool Diff_KFB_IsPacked (const unsigned char *data, size_t size)
{
	KFB2_FILE_HEADER hdr;

	if (size < sizeof (hdr))
		return false;

	memcpy(&hdr, data, sizeof (hdr));

	if (hdr.kfb_magic[0] == KFB2_MAGIC_0 &&
		hdr.kfb_magic[1] == KFB2_MAGIC_1 &&
		hdr.kfb_magic[2] == KFB2_MAGIC_2 &&
		hdr.kfb_magic[3] == KFB2_MAGIC_3)
	{
		// Yes, it is a new-format KFB
		if (hdr.kfb_flags & KFB_F_USES_COMPRESSION)
			return true;
	}

	return false;
}

////////////////////////////////////////////////////////////////

bool Do_KFB_Unpack (
		const unsigned char * oldData,
		size_t oldSize,
		std::vector<unsigned char> &resultBuffer
	)
{
	KFB2_FILE_HEADER oldHdr;
	KFB2_FILE_HEADER newHdr;

	if (oldSize < sizeof (oldHdr))
		return false;

	memcpy(&oldHdr, oldData, sizeof (oldHdr));
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

	if (!(oldHdr.kfb_flags & KFB_F_USES_COMPRESSION))
		// not a compressed KFB
		return false;

	if (((oldHdr.kfb_flags & KFB_F_BIG_ENDIAN) != 0) != KFB_NATIVE_ENDIAN)
		// non-native byte order not supported here
		return false;

//	uint32_t pageSize = uint32_t(1) << oldHdr.kfb_pageshift;
//	if (sizeof oldHdr > pageSize)
//		// some corruption
//		return false;

	newHdr = oldHdr;
	newHdr.kfb_flags &= ~KFB_F_USES_COMPRESSION;

	const KFB2_SECTION_TABLE_RECORD * oldSectionTable = (const KFB2_SECTION_TABLE_RECORD *)(oldData + oldHdr.kfb_sectbl_off);
	uint32_t sectionCnt = oldHdr.kfb_sectbl_size / sizeof (KFB2_SECTION_TABLE_RECORD);

	if (sectionCnt * sizeof (KFB2_SECTION_TABLE_RECORD) != oldHdr.kfb_sectbl_size)
		// some corruption
		return false;

	SectionTable newSectionTable;
	uint32_t totalSize = sizeof newHdr;

	// Lay the new file out

	for (uint32_t i = 0; i < sectionCnt; ++i)
	{
		KFB2_SECTION_TABLE_RECORD const& oldSection = oldSectionTable[i];
		uint32_t off;
		uint32_t idx;

		uint32_t usize = 0;
		allocSectionSpace(oldSection.kfb_section_usize, oldSection.kfb_section_alignment, newSectionTable, &off, &idx, &totalSize);
		
		KFB2_SECTION_TABLE_RECORD& newSection = newSectionTable[idx];
		newSection.kfb_section_flags = oldSection.kfb_section_flags & ~KFB_SECT_F_COMPRESSED;
		newSection.kfb_section_type = oldSection.kfb_section_type;
		newSection.kfb_fragment_id = oldSection.kfb_fragment_id;
		newSection.kfb_section_alignment = oldSection.kfb_section_alignment;

		resultBuffer.resize(totalSize);

		bool old_sect_compressed = (oldSection.kfb_section_flags & KFB_SECT_F_COMPRESSED) != 0;
		if (old_sect_compressed && oldSection.kfb_section_cmethod == KFB_COMPRESSION_METHOD_NONE)
			old_sect_compressed = false;

		if ((oldSection.kfb_section_flags & KFB_SECT_F_CRC32) != 0)
		{
			uint32_t crc = KlavCRC32 (oldData + oldSection.kfb_section_off, oldSection.kfb_section_csize, 0);
			if (crc != oldSection.kfb_section_crc32)
				return false; // CRC mismatch
		}

		if (old_sect_compressed)
		{
			if (oldSection.kfb_section_cmethod != KFB_COMPRESSION_METHOD_LZMA)
				return false; // unsupported compression method

			// Unpack section data
			int r = LzmaDecodeMem2Mem(oldData + oldSection.kfb_section_off, oldSection.kfb_section_csize, &resultBuffer.front() + off, oldSection.kfb_section_usize);

			if (r != LZMA_DECODE_ERR_OK)
				return false;

			newSection.kfb_section_flags |= KFB_SECT_F_CRC32;
			newSection.kfb_section_crc32 = KlavCRC32 (&resultBuffer.front() + off, oldSection.kfb_section_usize, 0);
		}
		else
		{
			if (oldSection.kfb_section_usize != oldSection.kfb_section_csize)
				// some corruption
				return false;

			// Simply copy section data
			memcpy(&resultBuffer.front() + off, oldData + oldSection.kfb_section_off, oldSection.kfb_section_usize);

			newSection.kfb_section_flags |= KFB_SECT_F_CRC32;
			newSection.kfb_section_crc32 = KlavCRC32 (oldData + oldSection.kfb_section_off, oldSection.kfb_section_usize, 0);
		}
	}

	SectionTable tmp = newSectionTable;
	uint32_t dummy2 = 0;

	newHdr.kfb_sectbl_size = (uint32_t) newSectionTable.size() * sizeof (KFB2_SECTION_TABLE_RECORD);
	allocSectionSpace(newHdr.kfb_sectbl_size, KFB_ALIGNMENT_16, tmp, &newHdr.kfb_sectbl_off, &dummy2, &totalSize);

	resultBuffer.resize(totalSize);
	memcpy(&resultBuffer.front() + newHdr.kfb_sectbl_off, &newSectionTable.front(), newHdr.kfb_sectbl_size);

	newHdr.kfb_file_size = totalSize;
	memcpy(&resultBuffer.front(), &newHdr, sizeof newHdr);

	return true;
}

////////////////////////////////////////////////////////////////

bool KlavPack_KFB_DecodeVector(
		const std::vector<unsigned char> &SourceVector,
		std::vector<unsigned char> &DestinationVector
	)
{
	return Do_KFB_Unpack (& SourceVector [0], SourceVector.size (), DestinationVector);
}

bool Diff_KFB_Unpack (
		const unsigned char *pk_data,
		size_t pk_size,
		Diff_Buffer *unp_buf
	)
{
	memset (unp_buf, 0, sizeof (Diff_Buffer));

	std::vector <unsigned char> * outbuf = new std::vector <unsigned char> ();
	if (outbuf == 0)
		return false;

	if (! Do_KFB_Unpack (pk_data, pk_size, *outbuf))
	{
		delete outbuf;
		return false;
	}

	unp_buf->m_data = & ((*outbuf) [0]);
	unp_buf->m_size = outbuf->size ();
	unp_buf->m_pimpl = outbuf;

	return true;
}

