// file_blkdump.cpp
//
//

#include "tns_db.h"

////////////////////////////////////////////////////////////////

class File_Dumper
{
public:
	File_Dumper (TNS_Block_Device *dev, TNS_Printer *printer, klav_bool_t dump_data)
		: m_dev (dev), m_printer (printer), m_dump_data (dump_data) {}

	void dump_file ();

private:
	TNS_Block_Device * m_dev;
	TNS_Printer * m_printer;
	klav_bool_t   m_dump_data;

	TNS_Block_Device * file () const
		{ return m_dev; }

	bool dump_header (const TNS_DBF_HDR *hdr);
	void dump_bam (uint32_t blk_cnt, int n);
	void dump_blk (uint32_t blk);
	void release_bams (uint32_t blk_cnt);

	void print (const char *fmt, ...);
};

////////////////////////////////////////////////////////////////

static const char * blk_kind_name (int kind)
{
	switch (kind)
	{
	case TNS_NODEKIND_NORM:
		return "NORM";
	case TNS_NODEKIND_PRIV:
		return "PRIV";
	case TNS_NODEKIND_TEMP:
		return "TEMP";
	default:
		break;
	}
	return "???";
}

static const char * blk_capa_name (int capa)
{
	switch (capa)
	{
	case TNS_BLK_CAPA_0:
		return "100%";
	case TNS_BLK_CAPA_1:
		return ">75%";
	case TNS_BLK_CAPA_2:
		return "25-50%";
	case TNS_BLK_CAPA_3:
		return "<25%";
	default:
		break;
	}
	return "???";
}

void File_Dumper::print (const char *fmt, ...)
{
	va_list ap;
	va_start (ap, fmt);
	m_printer->print (fmt, ap);
	va_end (ap);
}

void File_Dumper::dump_file ()
{
	KLAV_ERR err = KLAV_OK;

	uint32_t file_blk_cnt = 0;
	err = file ()->get_size (& file_blk_cnt);
	if (KLAV_FAILED (err))
	{
		print ("ERROR 0x%X getting file size\n", err);
		return;
	}

	print ("File physical block count: %u (0x%X)\n", file_blk_cnt, file_blk_cnt);
	if (file_blk_cnt == 0)
		return;

	const TNS_DBF_HDR * header = (const TNS_DBF_HDR *) file ()->read_block (0);
	if (header == 0)
	{
		print ("ERROR reading file header block\n");
		return;
	}

	print ("File header dump:\n");

	uint32_t blk_cnt = header->tndb_blk_cnt;
	uint32_t seqno   = header->tndb_seqno;

	bool f = dump_header (header);

//	file ()->release_block (0);
	if (! f)
		return;

	// dump BAMs
	print ("\n");
	print ("Active Block Allocation Map (%u):\n", seqno & 1);
	dump_bam (blk_cnt, seqno & 1);

	print ("\n");
	print ("Backup Block Allocation Map (%u):\n", 1 - (seqno & 1));
	dump_bam (blk_cnt, 1 - (seqno & 1));

	print ("\n");
	for (uint32_t blk = 0; blk < blk_cnt; ++blk)
	{
		uint32_t bam_sect = blk / TNS_BLKS_PER_BAM;
		uint32_t bam_idx  = blk % TNS_BLKS_PER_BAM;
		if (bam_idx < TNS_BAM_BLKS) // BAM block ?
			continue;

		dump_blk (blk);
	}

//	release_bams (blk_cnt);
}

bool File_Dumper::dump_header (const TNS_DBF_HDR *header)
{
	print (" Version: %u.%u (0x%X:0x%X)\n", header->tndb_ver_major,
		header->tndb_ver_minor, header->tndb_ver_major, header->tndb_ver_minor);

	const char * endian_name = "UNKNOWN";

	if (header->tndb_endian == TNS_DBF_ENDIAN_LITTLE)
		endian_name = "LITTLE";
	else if (header->tndb_endian == TNS_DBF_ENDIAN_BIG)
		endian_name = "BIG";

	print (" Endianness: %u (%s)\n", header->tndb_endian, endian_name);
	print (" Flags: 0x%X\n", header->tndb_flags);

	if (header->tndb_endian != TNS_DBF_ENDIAN_NATIVE)
	{
		print ("File has non-native endianness, cannot dump\n");
		return false;
	}

	print (" Data block count: %u (0x%X)\n", header->tndb_blk_cnt, header->tndb_blk_cnt);
	print (" Release: %u (0x%X)\n", header->tndb_release, header->tndb_release);
	print (" Seqno:   %u (0x%X)\n", header->tndb_seqno, header->tndb_seqno);

	int active_idx = header->tndb_seqno & 1;

	uint32_t active_root = header->tndb_parms [active_idx].tndb_root_node;
	uint32_t backup_root = header->tndb_parms [1 - active_idx].tndb_root_node;

	print (" Active tree parms (%u): Root node: $%X.%X, Hash level: %u\n", active_idx,
		(active_root >> 16), (active_root & 0xFFFF), header->tndb_parms [active_idx].tndb_hash_level);

	print (" Backup tree parms (%u): Root node: $%X.%X, Hash level: %u\n", 1-active_idx,
		(backup_root >> 16), (backup_root & 0xFFFF), header->tndb_parms [1-active_idx].tndb_hash_level);

	return true;
}

void File_Dumper::dump_bam (uint32_t blk_cnt, int bam)
{
	uint32_t bam_sects = (blk_cnt + TNS_BLKS_PER_BAM - 1) / TNS_BLKS_PER_BAM;
	for (uint32_t i = 0; i < bam_sects; ++i)
	{
		uint32_t bam_idx = i * TNS_BLKS_PER_BAM + bam + TNS_HDR_BLKS;

		const uint32_t * bam_data = (const uint32_t *) (file ()->read_block (bam_idx));
		if (bam_data == 0)
		{
			print ("ERROR reading BAM block %u (0x%X)\n", bam_idx, bam_idx);
			continue;
		}

		for (uint32_t b = TNS_BAM_BLKS; b < TNS_BLKS_PER_BAM; ++b)
		{
			uint32_t blk_no = i * TNS_BLKS_PER_BAM + b;
//			if (blk_no >= blk_cnt)
//				break;

			uint32_t blk_desc = bam_data [b];
			uint32_t blk_id = TNS_BLKDESC_GET_ID (blk_desc);
			uint32_t blk_kind = TNS_BLKDESC_GET_KIND (blk_desc);
			uint32_t blk_unit = TNS_BLKDESC_GET_UNIT (blk_desc);
			uint32_t blk_capa = TNS_BLKDESC_GET_CAPA (blk_desc);

			if (blk_desc != 0)
			{
				print ("BLK %u (0x%X) ID=%u (0x%X) KIND: %s UNIT: %d (%d) USED: %s\n",
					blk_no, blk_no, blk_id, blk_id, blk_kind_name (blk_kind),
					blk_unit, TNS_BLK_UNIT_SIZE[blk_unit], blk_capa_name (blk_capa));
			}
		}
	}
}

void File_Dumper::dump_blk (uint32_t blk)
{
}

void File_Dumper::release_bams (uint32_t blk_cnt)
{
	uint32_t bam_sects = (blk_cnt + TNS_BLKS_PER_BAM - 1) / TNS_BLKS_PER_BAM;
	for (uint32_t i = 0; i < bam_sects; ++i)
	{
		uint32_t bam_idx = i * TNS_BLKS_PER_BAM + TNS_HDR_BLKS;
		file ()->release_block (bam_idx);
		file ()->release_block (bam_idx + 1);
	}
}

////////////////////////////////////////////////////////////////

void KLAV_CALL KLAV_Tree_File_Dump (
		TNS_Block_Device * dev,
		TNS_Printer * printer,
		klav_bool_t dump_data
	)
{
	File_Dumper dumper (dev, printer, dump_data);
	dumper.dump_file ();
}

