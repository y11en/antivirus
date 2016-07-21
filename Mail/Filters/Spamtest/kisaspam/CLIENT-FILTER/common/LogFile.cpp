#include "commhdr.h"
#pragma hdrstop

/*!
 * (C) 2003-2005 Ashmanov and Partners JSC
 *
 * \file 
 * \author A&P
 * \date 2003
 * \brief License management functions. 
 *
 */ 

#ifndef STRICT
#define STRICT 1// for Windows.H
#endif

#include <windows.h>
#include <stdio.h>
#include <stdarg.h>

#include "logfile.h"

#include "apversion.h"
static const TCHAR szGlobalOptionsKey[]=TEXT("Software\\")REG_KEY;
static HKEY GetGlobalOptionsKeyLocalMachine() // OL also call DuplicateCurrentUserToLocalMashine
{
    HKEY hkOptions=0;
    RegCreateKey(HKEY_LOCAL_MACHINE, szGlobalOptionsKey, &hkOptions);
    return hkOptions;
}
static int GetLogLevel()
{
    int ret=0;
    HKEY hk=GetGlobalOptionsKeyLocalMachine();
    if(hk) {
        DWORD dwSize=sizeof(int);
        RegQueryValueEx(hk, TEXT("TraceLevel"), NULL, NULL, (BYTE *)&ret, &dwSize);
        RegCloseKey(hk);
    }
    return ret;
}
static int GetLogSize()
{
    int ret=0;
    HKEY hk=GetGlobalOptionsKeyLocalMachine();
    if(hk) {
        DWORD dwSize=sizeof(int);
        RegQueryValueEx(hk, TEXT("LogSize"), NULL, NULL, (BYTE *)&ret, &dwSize);
        RegCloseKey(hk);
    }
    return ret;
}
void GetLogPath(LPTSTR filename)
{
    *filename='\0';
    HKEY hk=GetGlobalOptionsKeyLocalMachine();
    if(hk) {
        DWORD dwSize=_MAX_PATH;
        RegQueryValueEx(hk, TEXT("LogPath"), NULL, NULL, (BYTE *)filename, &dwSize);
        RegCloseKey(hk);
    }
}
int GetLogSMTP()
{
    int ret=0;
    HKEY hk=GetGlobalOptionsKeyLocalMachine();
    if(hk) {
        DWORD dwSize=sizeof(int);
        RegQueryValueEx(hk, TEXT("LogSMTP"), NULL, NULL, (BYTE *)&ret, &dwSize);
        RegCloseKey(hk);
    }
    return ret;
}

static int AddString(char *buf, int ccbuf, const char *str)
{
    if ( !str || !*str || ccbuf <= 2 ) 
        return 0;
    // needs storage for leading space and trailing zero
    int len = strlen(str);
    if ( len > ccbuf-2 )
        len = ccbuf-2;
    *buf++ = ' ';
    memcpy(buf,str,len);
    buf[len] = '\0';
    return len+1;
}

void WriteToLog(int type, const char *str1, const char *str2, const char *str3, const char *str4, const char *str5, const char *str6 )
{
    int lmax=GetLogLevel();
    if(type<=lmax) {
        TCHAR filename[_MAX_PATH];
        GetLogPath(filename);
        HFILE log=_lopen(filename,OF_WRITE);
        if(log==HFILE_ERROR) log=_lcreat(filename,0);
        else {
            LONG size=_llseek(log,0,FILE_END);
            int maxsize=GetLogSize()*1024L*1024L;
            if(maxsize>0) {
                if(size>=maxsize) {
                    _lclose(log); // Close
                    log=_lcreat(filename,0); // And create new
                }
            }
        }
        //prepare message
        //2003-08-04 23:51:55 Stat Daily ProbableSpam 2
        //2003-08-04 23:48:55 Info "OL-Plugin loaded Anti-Spam database"
        //2003-08-04 23:49:15 Msg Obscene "wowik@mail.ru" "alexiv@ashmanov.com" "" "Re: Iienaiea"

        char buff[1024*8]; 
        int ccdone = 0, ccbuff = sizeof(buff)-3; // needs space for trailing "\r\n" and zero
        buff[ccbuff] = '\0';

        SYSTEMTIME st;
        GetLocalTime(&st);
        if(lmax>=LOG_DEBUG) {   // with threads
          ccdone = _snprintf(buff,ccbuff,
              TEXT("0x%04x %04d-%02d-%02d %02d:%02d:%02d %s %s"), GetCurrentThreadId(), st.wYear,st.wMonth,st.wDay,st.wHour,st.wMinute,st.wSecond,
              type==LOG_STAT?  TEXT("Stat"): 
              type==LOG_INFO?  TEXT("Info"): 
              type==LOG_MSG?   TEXT("Msg"): 
                  /*LOG_DEBUG*/TEXT("Debug"),
              str1
          );
        } else {
          ccdone = _snprintf(buff,ccbuff,
              TEXT("%04d-%02d-%02d %02d:%02d:%02d %s %s"),st.wYear,st.wMonth,st.wDay,st.wHour,st.wMinute,st.wSecond,
              type==LOG_STAT?  TEXT("Stat"): 
              type==LOG_INFO?  TEXT("Info"): 
              type==LOG_MSG?   TEXT("Msg"): 
                  /*LOG_DEBUG*/TEXT("Debug"),
              str1
          );
        }
        if ( 0 > ccdone || ccdone > ccbuff ) 
            ccdone = ccbuff;

        ccdone += AddString(buff+ccdone, ccbuff-ccdone, str2);
        ccdone += AddString(buff+ccdone, ccbuff-ccdone, str3);
        ccdone += AddString(buff+ccdone, ccbuff-ccdone, str4);
        ccdone += AddString(buff+ccdone, ccbuff-ccdone, str5);
        ccdone += AddString(buff+ccdone, ccbuff-ccdone, str6);

        for(char *cp=buff; *cp; cp++) if(*cp=='\n' || *cp=='\r') *cp=' ';
        strcpy(buff+ccdone,"\r\n");

        _lwrite(log,buff,strlen(buff)); // Do write in single operation for multithread/multitask environment
        #ifdef _DEBUG
        OutputDebugString(buff);
        #endif// _DEBUG
        _lclose(log);
    }
}

static HKEY GetStatisticsCategoryKey(LPCTSTR lpszCategory)
{
    HKEY hkCategory=0;
    HKEY hk=GetGlobalOptionsKeyLocalMachine();
    if(hk) {
        TCHAR szKey[120];
        wsprintf(szKey,TEXT("Statistics\\%s"),lpszCategory);
        RegCreateKey(hk,szKey,&hkCategory);
        RegCloseKey(hk);
    }
    return hkCategory;
}
void Statistics(LPCTSTR lpszCategory, DWORD nMessages, DWORD *pdwTotalCount, DWORD *pdwMonthCount, DWORD *pdwDayCount)
{
    if(pdwTotalCount) *pdwTotalCount=0;
    if(pdwMonthCount) *pdwMonthCount=0;
    if(pdwDayCount) *pdwDayCount=0;
    HKEY hkCategory=GetStatisticsCategoryKey(lpszCategory);
    if(hkCategory) {
        BOOL bNewMonth=FALSE;
        BOOL bNewDay=FALSE;
        {
            BOOL bNeedSave=FALSE;
            SYSTEMTIME st;

            DWORD dwSize=sizeof(st);
            LONG ret=RegQueryValueEx(hkCategory,TEXT("Time"),NULL,NULL,(LPBYTE)&st,&dwSize); // REG_BINARY
            if(ret || dwSize!=sizeof(st)) {
                GetLocalTime(&st);
                bNeedSave=TRUE; // first time?
            }

            SYSTEMTIME stCurrent;
            GetLocalTime(&stCurrent);

            bNewMonth==stCurrent.wYear!=st.wYear || stCurrent.wMonth!=st.wMonth;
            bNewDay=bNewMonth || stCurrent.wDay!=st.wDay;

            if(bNewDay/* && nMessages*/) bNeedSave=TRUE; // do not save if nothing interesting changed and messages added
            if(bNeedSave) {
                GetLocalTime(&st);
                RegSetValueEx(hkCategory,TEXT("Time"),0,REG_BINARY,(const BYTE *)&st,sizeof(st));
            }
        }
        {
            DWORD dwTotalCount=0;
            DWORD dwSize=sizeof(dwTotalCount);
            RegQueryValueEx(hkCategory,TEXT("TotalCount"),NULL,NULL,(LPBYTE)&dwTotalCount,&dwSize);

            if(nMessages) {       
                dwTotalCount+=nMessages;
                RegSetValueEx(hkCategory,TEXT("TotalCount"),NULL,REG_DWORD,(const BYTE *)&dwTotalCount,sizeof(dwTotalCount));
            }
            if(pdwTotalCount) *pdwTotalCount=dwTotalCount;
        }
        {
            DWORD dwMonthCount=0;
            DWORD dwSize=sizeof(dwMonthCount);
            RegQueryValueEx(hkCategory,TEXT("MonthCount"),NULL,NULL,(LPBYTE)&dwMonthCount,&dwSize);

            if(bNewMonth) {
                TCHAR buff[30];
                _ltoa(dwMonthCount,buff,10);
                WriteToLog(LOG_STAT, TEXT("Monthly"), lpszCategory, buff);

                dwMonthCount=0;
            }

            if(bNewMonth || nMessages) {
                dwMonthCount+=nMessages;
                RegSetValueEx(hkCategory,TEXT("MonthCount"),NULL,REG_DWORD,(const BYTE *)&dwMonthCount,sizeof(dwMonthCount));
            }
            if(pdwMonthCount) *pdwMonthCount=dwMonthCount;
        }
        {
            DWORD dwDayCount=0;
            DWORD dwSize=sizeof(dwDayCount);
            RegQueryValueEx(hkCategory,TEXT("DayCount"),NULL,NULL,(LPBYTE)&dwDayCount,&dwSize);

            if(bNewDay) {
                TCHAR buff[30];
                _ltoa(dwDayCount,buff,10);
                WriteToLog(LOG_STAT, TEXT("Daily"), lpszCategory, buff);

                dwDayCount=0;
            }
       
            if(bNewDay || nMessages) {
                dwDayCount+=nMessages;
                RegSetValueEx(hkCategory,TEXT("DayCount"),NULL,REG_DWORD,(const BYTE *)&dwDayCount,sizeof(dwDayCount));
            }
            if(pdwDayCount) *pdwDayCount=dwDayCount;
        }
        RegCloseKey(hkCategory);
    }
}

void PrintfToLog(int type, const char *fmt, ...)
{
  if ( !fmt || !*fmt ) return;
  char buf[512]; size_t ccbuf = sizeof(buf)-1;
  buf[ccbuf] = '\0';

  va_list arglst;
  va_start( arglst, fmt );
  _vsnprintf( buf, ccbuf, fmt, arglst );
  va_end( arglst );

  WriteToLog(type,buf);
}
