/*!
 * (C) 2003 Kaspersky Lab 
 * 
 * \file	jrnl/common.h
 * \author	Mikhail Karmazine
 * \date	21.04.2003 17:46:48
 * \brief	
 * 
 */


#ifndef __JRNL_COMMON_H__
#define __JRNL_COMMON_H__

# include "osdep.h"
#include <sstream>

#ifdef WIN32
# include <share.h>
# include <io.h>
# include <fcntl.h>
# include <stdio.h>

# include <byteorder.h>
#endif

#if defined(__unix) || defined(N_PLAT_NLM)

#ifdef _LITTLE_ENDIAN
#  define MachineToLittleEndian(v) (v)
#  define LittleEndianToMachine(v) (v)
#else
# include <byteorder.h>
#endif

#endif

#include <std/base/klbase.h>
#include <std/io/klio.h>

// TEXT_REPR_TRUE and TEXT_REPR_FALSE shall have the same length
#define TEXT_REPR_TRUE    "t"
#define TEXT_REPR_FALSE   "f"

#define TEXT_REPR_VERSION                   "version:"
#define TEXT_REPR_PAGE_LIMIT_TYPE           "page limit type:"
#define TEXT_REPR_MAX_REC_IN_PAGE           "max record count in page:"
#define TEXT_REPR_MAX_PAGE_SIZE             "max page size:"
#define TEXT_REPR_LIMIT_PAGES               "limit pages:"
#define TEXT_REPR_USE_REC_NUM_INDEX         "use recnum idx:"
#define TEXT_REPR_CHANGING                  "changing:"
#define TEXT_REPR_FILE_MARKED_AS_DELETED    "marked as deleted:"
#define TEXT_REPR_FILE_PHYS_DELETED         "file physically deleted:"
#define TEXT_REPR_TOTAL_REC_COUNT           "total record count:"
#define TEXT_REPR_DELETED_REC_COUNT         "deleted record count:"
#define TEXT_REPR_IDENTITY                  "identity:"
#define TEXT_REPR_ID                        "id:"
#define TEXT_REPR_DELETED                   "deleted:"
#define TEXT_REPR_START_POSITION            "start position:"
#define TEXT_REPR_START_ID                  "start id:"
#define TEXT_REPR_REC_POS                   "rec pos:"

#define JRNL_DELIM   "\r\n"  // shall not be changed, because we depends on fgets
			                 // function, and fgets looks for either "\n" or "\r\n",
			                 // depending on platform.

#define JRNL_DELIM_UNICODE   L"\r\n"  // shall not be changed, because we depends on fgets
			                 // function, and fgets looks for either "\n" or "\r\n",
			                 // depending on platform.

#define IDX_FILENAME_ADDON		L"_idx"
#define CTRL_FILE_ID_EXT		L".id"


const int STATIC_BUF_LEN = 100;
const int JRNL_VERSION = 1;

char* fgets_bin_win32( char *string, int n, FILE *stream, bool* pbDelimFound = NULL );
std::wstring GenerateTempJournalName( const std::wstring & wstrSourceName );

#endif //__JRNL_COMMON_H__
