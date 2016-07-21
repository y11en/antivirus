// tns_blk.cpp
//
//

#include "tns_mgr.h"

uint32_t TNS_BLK_UNIT_SIZE [] =
{
	sizeof (TNS_NODE), // == 16
	8,
	16,
	32,
	64,
	128,
	256,
	512,
	1024
};

// assuming block size = 4K:
//
// starting offset of block data
uint32_t TNS_BLK_DATA_START [] =
{
	0x10 + 0x20,         // 0x30 size=16: 256-bit bitmap
	0x10 + 0x40,         // 0x48 size=8:  512-bit bitmap
	0x10 + 0x20,         // 0x30 size=16: 256-bit bitmap
	0x10 + 0x10,         // 0x20 size=32: 128-bit bitmap
	0x10 + 0x10,         // 0x20 size=64: 64-bit bitmap + aligmnent
	0x10 + 0x10,         // 0x10 size=128: 32-bit bitmap + alignment
	0x10 + 0x10,         // 0x10 size=256: 16-bit bitmap + alignment
	0x10 + 0x10,         // 0x10 size=512: 8-bit bitmap + alignment
	0x10 + 0x10,         // 0x10 size=1024: 4-bit bitmap + alignment
};

#define BLK_UNIT_COUNT(UNIT_SIZE,HDR_SIZE) (((TNS_BLK_SIZE)-(HDR_SIZE)) / (UNIT_SIZE))

// number of units in block
uint32_t TNS_BLK_UNIT_COUNT [] =
{
	BLK_UNIT_COUNT(0x10,  0x30),
	BLK_UNIT_COUNT(0x08,  0x50),
	BLK_UNIT_COUNT(0x10,  0x30),
	BLK_UNIT_COUNT(0x20,  0x20),
	BLK_UNIT_COUNT(0x40,  0x20),
	BLK_UNIT_COUNT(0x80,  0x20),
	BLK_UNIT_COUNT(0x100, 0x20),
	BLK_UNIT_COUNT(0x200, 0x20),
	BLK_UNIT_COUNT(0x400, 0x20),
};

int tns_blk_find_free_item (const TNS_BLK_HDR *hdr)
{
	const uint8_t *bmp = ((const uint8_t *) hdr) + TNS_BLK_BITMAP_START;

	const uint32_t unit_idx = hdr->tdb_unit_size - '0';
	const uint32_t max_items = TNS_BLK_UNIT_COUNT [unit_idx];
	const uint32_t bmp_bytes = ((max_items + 7) & ~0x7) >> 3;

	for (uint32_t i = 0; i < bmp_bytes; ++i)
	{
		if (bmp [i] != 0xFF)
		{
			uint32_t b = bmp [i];
			for (uint32_t j = 0; j < 8; ++j)
			{
				if ((b & 1) == 0)
				{
					uint32_t idx = (i * 8) + j;
					if (idx >= max_items)
						return -1;

					return (int) idx;
				}
				b >>= 1;
			}
		}
	}

	return -1;
}

uint32_t tns_blk_capa (uint32_t unit_size_idx, uint32_t used)
{
	uint32_t blk_capa = TNS_BLK_UNIT_COUNT [unit_size_idx];
	// 'used' percent
	uint32_t used_ppc = used * 4 / blk_capa;
	switch (used_ppc)
	{
	case 0: // 0-25% used, 75%-100% free
		return TNS_BLK_CAPA_3;
	case 1: // 25-50% used, 50-75% free
	case 2: // 50-75% used, 25-50% free
		return TNS_BLK_CAPA_2;
	case 3: // 75-99% used, 1-25% free
		return TNS_BLK_CAPA_1;
	default:
		break;
	}
	return TNS_BLK_CAPA_0; // 100% used, 0% free
}
