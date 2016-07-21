#include <malloc.h>
#include <memory.h>
#include <stdio.h>

#include "../shared/differr.h"
#include "../shared/types.h"
#include "../shared/crc32.h"
#include "../lzma/lzmapack.h"

#include "klz.h"
#include "trace.h"

static const char AVPBaseSign[] = "AVP Antiviral Database";

extern char* GetDiffErrorStr(int error);

int GenKLZ(char* src_path, char* src_fn, char* res_path, char* res_fn)
{
	int Res;
	FILE *fsrc = NULL;
	FILE *fdst = NULL;
	tBYTE *PData, *Data;
	size_t DSize = 0, PSize = 0;
	KLZ_HDR hdr;
	char buff[0x100];

	fsrc = fopen(src_path, "rb");
	if (NULL == fsrc)
	{
		_Trace( 0, "Error: Packing to KLZ failed, cannot open source %s\n", src_path);
		return DIFF_ERR_FILE;
	}
	fread(buff, 1, sizeof(buff), fsrc);
	if (0 == memcmp(AVPBaseSign, buff, sizeof(AVPBaseSign)-1))
	{
		_Trace( 0, "Info: Source is %s, packing to KLZ skipped\n", AVPBaseSign);
		return DIFF_ERR_AVCPACK;
	}

	fseek(fsrc, 0, SEEK_END);
	DSize = ftell(fsrc);
	fseek(fsrc, 0, SEEK_SET);
	if (DSize < 100)
	{
		_Trace( 0, "Error: Packing to KLZ failed, source too small (%d bytes)\n", DSize);
		return DIFF_ERR_FILE;
	}
	/* alloc pack buffer */
	PSize = (DSize - 100); // / 10 * 9;
	if ( NULL == (Data = (tBYTE*)malloc(DSize + 16)) )
	{
		_Trace( 0, "Error: Packing to KLZ failed, no memory (%d)\n", DSize);
		fclose(fsrc);
		return(DIFF_ERR_NOMEM);
	}
	if ( NULL == (PData = (tBYTE*)malloc(PSize + 16)) )
	{
		_Trace( 0, "Error: Packing to KLZ failed, no memory (%d)\n", PSize);
		free(Data);
		fclose(fsrc);
		return(DIFF_ERR_NOMEM);
	}
	fread(Data, 1, DSize, fsrc);
	fclose(fsrc);
	
	//	  {
	//		  tUINT _lzma, _zlib;
	//		  _lzma = PSize;
	//		  _zlib = PSize;
	//		  Res = LzmaPackMem2Mem(DTbl, DSize, PDif, &_lzma);
	//		  Res = (Z_OK == compress2(PDif, &_zlib, DTbl, DSize, 9) ? DIFF_ERR_OK : DIFF_ERR_PACK);
	//		  _Trace( 0, "Compressed Diff-Tables size: %u bytes LZMA, %u bytes ZLIB\n", _lzma, _zlib);
	//	  }
	
	/* compress data */
	Res = LzmaPackMem2Mem(Data, DSize, PData, &PSize);
	free(Data);
	
	if (Res != DIFF_ERR_OK)
	{
		if (DIFF_ERR_NOMEM == Res)
			_Trace( 0, "Info: Packing to KLZ skipped, result size large then source\n");
		else
			_Trace( 0, "Error: Packing to KLZ failed with %s (%d)\n", GetDiffErrorStr(Res), Res);
		free(PData);
		return Res;
	}

	fdst = fopen(res_path, "wb");
	if (NULL == fdst)
	{
		_Trace( 0, "Error: Packing to KLZ failed, cannot open destination %s\n", res_path);
		free(PData);
		return DIFF_ERR_FILE;
	}

	memcpy(&hdr.bSign, bKLZSign, 4);
	hdr.nHdrSize = sizeof(hdr);
	hdr.DSize = DSize;
	hdr.PSize = PSize;
	hdr.dwCrc = CRC32(PData, PSize, 0);

	if (sizeof(hdr) != fwrite(&hdr, 1, sizeof(hdr), fdst))
	{
		_Trace( 0, "Error: Packing to KLZ failed, cannot write destination %s\n", res_path);
		free(PData);
		fclose(fdst);
		return DIFF_ERR_FILE;
	}

	if (PSize != fwrite(PData, 1, PSize, fdst))
	{
		_Trace( 0, "Error: Packing to KLZ failed, cannot write destination %s\n", res_path);
		free(PData);
		fclose(fdst);
		return DIFF_ERR_FILE;
	}
	fclose(fdst);
	
	free(PData);
	return DIFF_ERR_OK;
}
