#include <Prague/prague.h>

#include "miniarchiver.h"

#include <string.h>

#define DIRSIZE_STEP 16

tDWORD CountCRC32Io(hIO hIO, tBYTE* Buffer, tDWORD dwBufferSize)
{
tDWORD CRC32 = 0;
tDWORD read;
tQWORD qwOffset = 0;

    CALL_IO_SeekRead(hIO, &read, qwOffset, Buffer, dwBufferSize);
    while (read != 0)
    {
        CRC32 = iCountCRC32(read, Buffer, CRC32);
        qwOffset += read;
        CALL_IO_SeekRead(hIO, &read, qwOffset, Buffer, dwBufferSize);
    }
    return CRC32;
}

tDWORD CountCRC32SeqIo(hSEQ_IO hFileSeqIo,tBYTE* Buffer, tDWORD dwBufferSize)
{
tDWORD CRC32 = 0;
tDWORD read;

	CALL_SeqIO_Read(hFileSeqIo, &read, Buffer, dwBufferSize);
	while(read != 0 )
	{
		CRC32 = iCountCRC32(read, Buffer, CRC32);
		CALL_SeqIO_Read(hFileSeqIo,&read, Buffer,dwBufferSize);
	}
	CALL_SeqIO_SeekSet(hFileSeqIo,NULL, 0);
	return CRC32;
}

tERROR CopyFile(hSEQ_IO hSrc, hSEQ_IO Dest, tBYTE* Buffer, tDWORD dwBufferSize)
{
tDWORD read;
tERROR error;

	error = errOBJECT_READ;
	error = CALL_SeqIO_Read(hSrc, &read, Buffer, dwBufferSize);
	while(read != 0 )
	{
		if(CALL_SeqIO_Write(Dest,NULL, Buffer, read) != errOK)
			break;
		CALL_IO_Flush(Dest);
		error = CALL_SeqIO_Read(hSrc, &read, Buffer, dwBufferSize);
	}
	if(error == errEOF || error == errOUT_OF_OBJECT)
		error = errOK;
	return error;
}

tERROR iFindEndHeader(hHEAP hLocalHeap, hSEQ_IO io, tQWORD qwStartOffset, tDWORD * pdwDirOffset, tDWORD * pdwDirSize)
{
  EndRecord end;
  tQWORD qwOffset;
  tQWORD qwFileLen;
  tDWORD i;
  tBOOL  bFound;
  tDWORD dwTmp;
  tQWORD qwDirOffset;

	bFound = cFALSE;
	CALL_SeqIO_GetSize(io, &qwOffset, IO_SIZE_TYPE_EXPLICIT); qwFileLen = qwOffset;
	CALL_SeqIO_SeekRead(io, &dwTmp, qwOffset-sizeof(EndRecord), &end, sizeof(EndRecord));
	if ( sizeof(EndRecord) != dwTmp )
		return(errNOT_FOUND);

	if( end.end_of_dir_signature != END_RECORD_SIG )
	{
    tDWORD dwBlkLen;
    tERROR Error;
    tBYTE* Buf;

		/* find it (may be comments) */
    dwBlkLen = (tDWORD)((qwFileLen < 0x1000) ? (qwFileLen) : (0x1000));
    if ( dwBlkLen < sizeof(tDWORD) )
      return(errNOT_FOUND);
    qwOffset = qwFileLen - dwBlkLen;
    if ( PR_FAIL(Error = CALL_Heap_Alloc(hLocalHeap, &Buf, dwBlkLen + 16)) )
      return(Error);

    Error = CALL_SeqIO_SeekRead(io, &dwTmp, qwOffset, Buf, dwBlkLen);
    if ( PR_SUCC(Error) )
    {
       for ( i = (dwBlkLen - 4); i > 0; i-- )
       {
         if ( END_RECORD_SIG == *(tDWORD*)(&Buf[i]) )
         {
           qwOffset += i;
           bFound = cTRUE;
           break;
         }
       }
    }

    CALL_Heap_Free(hLocalHeap, Buf);
		if ( !bFound )
      return(errNOT_FOUND);

    CALL_SeqIO_Seek(io, NULL, qwOffset, cSEEK_SET);
    CALL_SeqIO_Read(io, &dwTmp, &end, sizeof(EndRecord));
    if( sizeof(EndRecord) != dwTmp )
       return(errNOT_FOUND);
    if( end.end_of_dir_signature != END_RECORD_SIG )
       return(errNOT_FOUND);
	}

  qwDirOffset = (tQWORD)end.offset_to_starting_directory - qwStartOffset;
  if ( end.size_of_the_central_directory != 0 ) /* non empty archive */
  {
	  CALL_SeqIO_SeekRead(io, &dwTmp, qwDirOffset, &i, sizeof(tDWORD));
	  if( dwTmp != sizeof(tDWORD) || i != HEADER_FILE_SIG )
		    return(errNOT_FOUND);
  }
  
  if (pdwDirSize)
  {
    *pdwDirSize = end.size_of_the_central_directory;
  }
  if (pdwDirOffset)
  {
    *pdwDirOffset = (tDWORD)qwDirOffset;
  }

	return(errOK);
}

tDWORD iSkipNumberOfFile(hSEQ_IO io, tDWORD dwOffset, tDWORD dwSkip)
{
FileHeader DirHeader;
tDWORD dwTmp;
	
	while(dwSkip--)
	{
		CALL_SeqIO_SeekRead(io,&dwTmp, dwOffset,&DirHeader,sizeof(FileHeader));
		if(sizeof(FileHeader)==dwTmp)
		{
			if(DirHeader.entral_file_header_signature == HEADER_FILE_SIG)
			{
				dwOffset+=DirHeader.filename_length+DirHeader.file_comment_length+
					DirHeader.extra_field_length+sizeof(FileHeader);
			}
			else
				return 0;
			
		}
		else
			return 0;
	}
	return dwOffset;
}


tERROR PackFileToZIP(hIO hFileIO, hSEQ_IO hZipSeqIo, LocalFileHeader* pLocalHeader, tBYTE* Buffer, tDWORD dwBufferSize, tCHAR* szName, tDWORD dwSizeOfName, tQWORD* lpdwDataPos)
{
tERROR error;
hIO hPackedIO;
hSEQ_IO hPackedSeqIo;
hSEQ_IO hFileSeqIo;
hTRANSFORMER hPacker;
tQWORD qwSize;
tQWORD qwSize1;
tINT iTmp;

	error = errOBJECT_NOT_CREATED;
	*lpdwDataPos = 0;
	//CALL_IO_SeekRead(hFileIO, NULL, 0, Buffer, dwBufferSize);

	pLocalHeader->general_purpose_bit_flag &= ~1; //clear password flag

	if(pLocalHeader->compression_method == DEFLATED)
	{
		pLocalHeader->compression_method = DEFLATED;
		error = CALL_SYS_ObjectCreate((hOBJECT)hFileIO, &hPacker, IID_TRANSFORMER, PID_DEFLATE,0);
		if(PR_SUCC(error))
		{
			CALL_SYS_PropertySetDWord(hPacker, plDEFLATE_WINDOW_SIZE,32*1024);
			iTmp= -15;
			CALL_SYS_PropertySet(hPacker, NULL, plDEFLATE_WINDOW_MULTIPIER, &iTmp, sizeof(iTmp));
		}
		else
			return error;
	}
	else
	{
		pLocalHeader->compression_method = STORED;
		error = CALL_SYS_ObjectCreate((hOBJECT)hFileIO, &hPacker, IID_TRANSFORMER, PID_STORED,0);
	}
// Create Temp File to output
	hPackedSeqIo = NULL;
	error = CALL_SYS_ObjectCreate((hOBJECT)hFileIO,(hOBJECT*)&hPackedIO, IID_IO, PID_TMPFILE,0);
	if(PR_SUCC(error))
	{
		CALL_SYS_PropertySetDWord( (hOBJECT)hPackedIO, pgOBJECT_ACCESS_MODE, fACCESS_RW);
		CALL_SYS_PropertySetDWord( (hOBJECT)hPackedIO, pgOBJECT_OPEN_MODE,   fOMODE_CREATE_ALWAYS);// + fOMODE_SHARE_DENY_READ);
		error = CALL_SYS_ObjectCreateDone(hPackedIO);
		if(PR_SUCC(error))
		{
			error = CALL_SYS_ObjectCreateQuick(hPackedIO, &hPackedSeqIo, IID_SEQIO, PID_ANY,0);
		}
	}
	
	if(PR_FAIL(error))
	{
		return error;
	}

	CALL_SYS_PropertySetObj(hPacker, pgOUTPUT_SEQ_IO, (hOBJECT)hPackedSeqIo);

	hFileSeqIo = CreateSeqIO(hFileIO);

	if(!hFileSeqIo )
		return errOBJECT_NOT_CREATED;
	CALL_SYS_PropertySetObj(hPacker, pgINPUT_SEQ_IO, (hOBJECT)hFileSeqIo);
	
	error = CALL_SYS_ObjectCreateDone( hPacker);
	if(PR_FAIL(error))
	{
		CALL_SYS_ObjectClose(hPacker);
		return error;
	}
	
	error = CALL_Transformer_Proccess(hPacker, &qwSize);
	if(PR_FAIL(error ))
	{
		return error;
	}


	pLocalHeader->compressed_size = (tDWORD)qwSize;
	CALL_SeqIO_SeekEnd(hZipSeqIo, lpdwDataPos, 0);

//	qwSize = 0;
//	CALL_SeqIO_GetSize(hPackedIO, &qwSize, IO_SIZE_TYPE_EXPLICIT);

	qwSize1 = 0;
	CALL_SeqIO_GetSize(hZipSeqIo, &qwSize1, IO_SIZE_TYPE_EXPLICIT);
	qwSize += qwSize1 + sizeof(LocalFileHeader) + dwSizeOfName;

	error = CALL_SeqIO_SetSize(hZipSeqIo, qwSize);

	if(error == errOK)
	{
//		CALL_SeqIO_GetSize(hPackedIO, &qwSize, IO_SIZE_TYPE_EXPLICIT);
//		pLocalHeader->compressed_size = (tDWORD)qwSize;
		pLocalHeader->filename_length = (tBYTE)dwSizeOfName;
		pLocalHeader->extra_field_length = 0;
		if(pLocalHeader->compression_method == STORED)
		{
			CALL_SeqIO_Seek(hPackedSeqIo, NULL, 0, cSEEK_SET);
			pLocalHeader->crc32 = CountCRC32SeqIo(hPackedSeqIo, Buffer, dwBufferSize);
		}
		else
			CALL_SYS_PropertyGet(hPacker, NULL, plDEFLATE_INPUT_CRC, &pLocalHeader->crc32, sizeof(tDWORD));
		
		CALL_SeqIO_Write(hZipSeqIo, NULL, pLocalHeader, sizeof(LocalFileHeader));
		error = CALL_SeqIO_Write(hZipSeqIo, NULL, szName, dwSizeOfName);

		CALL_SeqIO_Seek(hPackedSeqIo, NULL, 0, cSEEK_SET);
		if(error == errOK)
			error = CopyFile(hPackedSeqIo, hZipSeqIo, Buffer, dwBufferSize);
	}
	if(hPackedIO)
		CALL_SYS_ObjectClose(hPackedIO);
	if(hFileSeqIo)
		CALL_SYS_ObjectClose(hFileSeqIo);
	return error;
}

tERROR WriteIntoMainDir(hSEQ_IO hMainDirSeqIO, LocalFileHeader* pLocalHeader, tCHAR* name, FileHeader* pHeader, tQWORD qwOffsetOfFile)
{
tERROR error;
tQWORD qwSize;

	CALL_SeqIO_SeekEnd(hMainDirSeqIO, NULL, 0);

	qwSize = 0;
	CALL_SeqIO_GetSize(hMainDirSeqIO,&qwSize, IO_SIZE_TYPE_EXPLICIT);
	qwSize += sizeof(FileHeader)+pLocalHeader->filename_length;

	error = CALL_SeqIO_SetSize(hMainDirSeqIO, qwSize);
	if(error == errOK)
	{
		pHeader->entral_file_header_signature = HEADER_FILE_SIG;
		pHeader->version_made_by = 0;
		pHeader->version_needed_to_extract = pLocalHeader->version_needed_to_extract;
		pHeader->general_purpose_bit_flag = pLocalHeader->general_purpose_bit_flag;
		pHeader->compression_method = pLocalHeader->compression_method;
		pHeader->last_mod_file_time = pLocalHeader->last_mod_file_time;
		pHeader->last_mod_file_date = pLocalHeader->last_mod_file_date;
		pHeader->crc32 = pLocalHeader->crc32;
		pHeader->compressed_size = pLocalHeader->compressed_size;
		pHeader->uncompressed_size = pLocalHeader->uncompressed_size;
		pHeader->filename_length = pLocalHeader->filename_length;
		pHeader->extra_field_length = 0;//pLocalHeader->extra_field_length;
		pHeader->file_comment_length = 0;
		pHeader->disk_number_start = 0;
		pHeader->internal_file_attributes = 0;
		pHeader->external_file_attributes = 0;
		pHeader->relative_offset_of_local_header = (tDWORD)qwOffsetOfFile;

		CALL_SeqIO_Write(hMainDirSeqIO, NULL, pHeader, sizeof(FileHeader));
		error = CALL_SeqIO_Write(hMainDirSeqIO, NULL, name, pLocalHeader->filename_length);
	}
	return error;
}

tERROR WriteMainDir(hSEQ_IO hNewZipSeqIO,hSEQ_IO hMainDirSeqIO, tBYTE* Buffer, tDWORD dwBufferSize, tDWORD dwTotalRecords)
{
tERROR error;
tDWORD read;
EndRecord er;
tDWORD dwSizeOfDir;
tQWORD qwSize, qwSize1;

	CALL_SeqIO_SeekEnd(hNewZipSeqIO, &qwSize, 0);
	er.offset_to_starting_directory = (tDWORD) qwSize;
	CALL_SeqIO_SeekEnd(hNewZipSeqIO, NULL, 0);

	qwSize = 0;
	CALL_SeqIO_GetSize(hNewZipSeqIO, &qwSize, IO_SIZE_TYPE_EXPLICIT);

	qwSize1 = 0;
	CALL_SeqIO_GetSize(hMainDirSeqIO,&qwSize1, IO_SIZE_TYPE_EXPLICIT);
	qwSize += qwSize1;


	error = CALL_SeqIO_SetSize(hNewZipSeqIO, qwSize+sizeof(EndRecord));
	if(error == errOK)
	{
		CALL_SeqIO_SeekSet(hMainDirSeqIO, NULL, 0);
		CALL_SeqIO_Read(hMainDirSeqIO, &read, Buffer,dwBufferSize);
		dwSizeOfDir=read;
		while(read != 0 )
		{
			if(CALL_SeqIO_Write(hNewZipSeqIO,NULL, Buffer, read)!=errOK)
				break;
			CALL_SeqIO_Read(hMainDirSeqIO,&read, Buffer,dwBufferSize);
			dwSizeOfDir+=read;
		}
		er.end_of_dir_signature = END_RECORD_SIG;
		er.number_of_this_disk = 0;
		er.disk_with_start_of_the_central_directory = 0;
		er.total_entries = (tWORD)dwTotalRecords;
		er.total_entries_on_this_disk = er.total_entries;
		er.size_of_the_central_directory = dwSizeOfDir;
		er.comment_length = 0;
		error = CALL_SeqIO_Write(hNewZipSeqIO, NULL, &er, sizeof(EndRecord));
	}
	if(error == errEOF)
		error = errOK;
	return error;
}

tVOID CreateLocalHeader(hIO io, LocalFileHeader* pLocalHeader)
{
tQWORD qwTmp;
tDWORD year  ;
tDWORD month ;
tDWORD day   ;
tDWORD hour  ;
tDWORD minute;
tDWORD second;
tDT dt	;

	year = 1990;
	month = 1;
	day = 1;
	hour = 0;
	minute = 0;
	second = 0;
	
	Now(&dt);
	DTGet((const tDT*)&dt, &year, &month, &day, &hour, &minute, &second, NULL);
	

	pLocalHeader->sig=LOCAL_HDR_SIG;
	pLocalHeader->version_needed_to_extract = 0x14;
	pLocalHeader->compression_method = DEFLATED;
	pLocalHeader->general_purpose_bit_flag = 0;
	pLocalHeader->last_mod_file_time = (((tWORD)hour)<<11) + (((tWORD)minute)<<5) + ((tWORD)(second/2));
	pLocalHeader->last_mod_file_date = (((tWORD)year-1980)<<9) + (((tWORD)month)<<5) + (tWORD)(day);
	CALL_IO_GetSize(io,&qwTmp, IO_SIZE_TYPE_EXPLICIT);
	pLocalHeader->uncompressed_size = (tDWORD) qwTmp;
}

tVOID UpdateLocalHeader(hIO io, LocalFileHeader* pLocalHeader)
{
tQWORD qwTmp;
tDWORD year  ;
tDWORD month ;
tDWORD day   ;
tDWORD hour  ;
tDWORD minute;
tDWORD second;
tDT dt	;

	year = 1990;
	month = 1;
	day = 1;
	hour = 0;
	minute = 0;
	second = 0;
	
	Now(&dt);
	DTGet((const tDT*)&dt, &year, &month, &day, &hour, &minute, &second, NULL);

	pLocalHeader->last_mod_file_time = (((tWORD)hour)<<11) + (((tWORD)minute)<<5) + ((tWORD)(second/2));
	pLocalHeader->last_mod_file_date = (((tWORD)year-1980)<<9) + (((tWORD)month)<<5) + (tWORD)(day);
	CALL_IO_GetSize(io,&qwTmp, IO_SIZE_TYPE_EXPLICIT);
	pLocalHeader->uncompressed_size = (tDWORD) qwTmp;
}


hSEQ_IO CreateSeqIO(hIO hIO)
{
hSEQ_IO hSeqIo;


	CALL_SYS_ObjectCreateQuick(hIO, &hSeqIo, IID_SEQIO, PID_ANY, 0);
/*	CALL_SYS_ObjectCreate((hOBJECT)hIO,&hSeqIo,IID_SEQIO, PID_ANY,0);
	if(hSeqIo)
	{
//		CALL_SYS_PropertySetDWord(hSeqIo, pgBUFFER_SIZE, 0x100);
		if(errOK!=CALL_SYS_ObjectCreateDone( hSeqIo))
		{
			CALL_SYS_ObjectClose(hSeqIo);
			hSeqIo = NULL;
		}
	}*/
	return hSeqIo;
}

tERROR CopyFileWithCRC(hSEQ_IO hSrcSeqIo, hSEQ_IO hDestSeqIo, tDWORD* lpdwCRC32, tBYTE* Buffer, tDWORD dwBufferSize)
{
tERROR error;
tQWORD qwSize;
tQWORD qwSize1;

	error=errOBJECT_NOT_CREATED;

	qwSize = 0;
	CALL_SeqIO_SeekEnd(hDestSeqIo, &qwSize, 0);

	qwSize1 = 0;
	CALL_SeqIO_GetSize(hSrcSeqIo,&qwSize1, IO_SIZE_TYPE_EXPLICIT);
	qwSize += qwSize1;

	error = CALL_SeqIO_SetSize(hDestSeqIo, qwSize);
	if(error==errOK)
	{
		CALL_SeqIO_SeekSet(hSrcSeqIo, NULL, 0);
		error = CopyFile(hSrcSeqIo, hDestSeqIo, Buffer, dwBufferSize);
	}

	return error;
}
