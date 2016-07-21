#ifndef UPDATER_STAFF_H_D7E08C34_B144_46e6_BDDB_E03C486F4664
#define UPDATER_STAFF_H_D7E08C34_B144_46e6_BDDB_E03C486F4664

#if _MSC_VER > 1000
    #pragma once
#endif

#include "stdinc.h"

#include "../include/update_source.h"

namespace KLUPD {

enum Style
{
    posixStyle,
    windowsStyle,
};

// outputs last error (errno) in human-readable way
// posixStyle [in] - use POSIX style for 'errno'. GetLastError() for non-POSIX
extern KAVUPDCORE_API NoCaseString errnoToString(const int lastError, const Style &);

// translates file operation related last error (errno) into updater understandable code
extern CoreError lastErrorToUpdaterFileErrorCode(const int lastError);

// checks if URL is started with "ftp://" or "http://" prefix
extern bool isFTPorHTTP(const EUpdateSourceType &type, const Path &url);

// converts updater XML date representation to comparable sequence number
extern unsigned long updateDateStringToSequenceNumber(const NoCaseString &dateString);
extern unsigned long addDays(const unsigned long date, const unsigned int daysToAdd);

// appendMode [in] - indicates if file should be appended. Otherwise file is truncated
extern CoreError saveDataToFile(const Path &fullFileName,
                                const unsigned char *data, const size_t size,
                                const bool append, KLUPD::AutoStream &, KLUPD::Log *);

}   // namespace KLUPD

#endif  // #ifndef UPDATER_STAFF_H_D7E08C34_B144_46e6_BDDB_E03C486F4664
