#ifndef _LOGFILE_H
#define _LOGFILE_H

/*!
 * (C) 2003-2005 Ashmanov and Partners JSC
 *
 * \file 
 * \author A&P
 * \date 2003
 * \brief Log file functions
 *
 */

#define LOG_STAT -2
#define LOG_INFO -1
#define LOG_MSG   0
#define LOG_DEBUG 1 

// type=-2 - Stat
// type=-1 - Info
// type=0  - Msg
// type=1  - Debug


//! Write a record of 'type' to the log file
void WriteToLog(int type, const char *str1, const char *str2=NULL, const char *str3=NULL, const char *str4=NULL, const char *str5=NULL, const char *str6=NULL);

void PrintfToLog(int type, const char *fmt, ...);

/*!
  \brief Get/Set statistics
  \param lpszCategory   [in] Category name
  \param nMessages      [int] number of messages to add to given cathegory.
  \param pdwTotalCount  [out] returns new total count
  \param pdwMonthCount  [out] returns new mounth count 
  \param pdwDayCount    [out] returns new day count
*/
//! Write a record of MSG type to the log file


#endif // _LOGFILE_H
