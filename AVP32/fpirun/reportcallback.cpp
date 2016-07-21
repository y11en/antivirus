// reportcallback.cpp -- FPI report callback function
//
// Copyright (c) 1997-2000 F-Secure Corporation

#include <stdio.h>
#include <string.h>

#ifndef UNIX
#include <conio.h>
#endif

#ifdef UNIX
#include <time.h>
#endif

#ifdef OS2
#include "myos2.h"
#endif

extern "C"
{
#include "fpiapi.h"
#include "fpirun.h"
}

static BOOL query(const char* actionstring);
static BOOL event_dbvalidation(WORD event_, WORD param_, DWORD flags_, LPCSTR message_, DWORD refdata_);
static BOOL event_error(WORD event_, WORD param_, DWORD flags_, LPCSTR message_, DWORD refdata_);
static void check_db_date(const char* dbname, DWORD date);

extern BOOL isFileClean;
extern BOOL isFileInfected;
extern BOOL isFileSuspicious;
extern BOOL isFileDisinfected;
extern BOOL isFileFailed;


BOOL  FPIFN FN_CALLBACK (WORD event_, WORD param_, DWORD flags_, LPCSTR message_, DWORD refdata_)
{
    char name[200];
    char fullname[500];

    char eventstring[100];
    char actionstring[100];
    char virusmessage[300];
    DWORD tickStart;
    DWORD tickNow;

    switch (event_)
    {
    case FPIEVENT_QUERYABORT:
#if defined _CONSOLE && !defined UNIX
        if (kbhit())
        {
            int c = getch();
            if (c=='x')
            {
                printboth("\n[STOPPED BY USER]\n");
                bExitScan = TRUE;
                return TRUE;
            }
        }
#endif

        tickStart = get_ticks_from_refdata(refdata_);
        tickNow = GetTickCount();
        if (tickStart != 0)
		{
			if (options.max_time > 0 && (double)(tickNow - tickStart) > options.max_time*1000.0)
			{
				printboth("\n[SINGLE-FILE TIME LIMIT EXCEEDED]\n");
				return TRUE;
			}
		}
        return FALSE;

    case FPIEVENT_QUERY:
	break;

    case FPIEVENT_INFECTION:
	break;

    case FPIEVENT_ERROR:
	break;

    case FPIEVENT_DBVALIDATION:
	return event_dbvalidation(event_, param_, flags_, message_, refdata_);

    default:
        printboth("%s:\tFPI violation: Invalid event (%hu)\n",
            fullname, event_);
        nApiErrors++;
        break;
    }

    get_name_from_refdata(refdata_, name, sizeof name);
    get_fullname_from_refdata(refdata_, fullname, sizeof fullname);
    FPIFILEHANDLE handle = get_handle_from_refdata(refdata_);

    isFileClean = FALSE;

    virusmessage[0] = '\0';

    if (event_ == FPIEVENT_QUERY || event_ == FPIEVENT_INFECTION)
    {
        strcpy(eventstring, "Infection");
        if (flags_ & FPIFLAG_SUSPECTED)
            strcat(eventstring, "/suspected");
        if ((flags_ & FPIFLAG_HEURISTIC) == FPIFLAG_HEURISTIC)
            strcat(eventstring, "/heuristic");
        if ((flags_ & FPIFLAG_INTEGRITY) == FPIFLAG_INTEGRITY)
            strcat(eventstring, "/integrity");

        if (flags_ & FPIFLAG_SUSPECTED)
        {
            isFileSuspicious = TRUE;
            if (flags_ & FPIFLAG_MSGEXTRA)
            {
                // message_ given, but no virus name available because it is only suspected
                strcat(virusmessage, message_);
            }
            else
            {
                // message_ ignored
            }
        }
        else
        {
            isFileInfected = TRUE;
            if (flags_ & FPIFLAG_MSGEXTRA)
            {
                // message_ contains actual virus name delimited with \x01
                const char* firstdelim;
                BOOL bNameUnknown = FALSE;
                firstdelim = strchr(message_, '\x01');
                if (firstdelim == NULL)
                {
                    strcat(virusmessage, "Name unknown - invalid message format (FPI violation)");
                    nApiErrors++;
                    bNameUnknown = TRUE;
                }
                else
                {
                    int len = strcspn(firstdelim+1, "\x01");
                    strncat(virusmessage, firstdelim+1, len);
                }

                if (options.fullmessage || bNameUnknown)
                {
                    strcat(virusmessage, "\tFull message:\t");
                    strcat(virusmessage, message_);
                }
            }
            else
            {
                // message_ *is* actual virus name
                strcpy(virusmessage, message_);
            }
        }
    }
    else if (event_ == FPIEVENT_ERROR)
    {
        strcpy(eventstring, "Error");
        isFileFailed = TRUE;
    }

    switch (event_)
    {
    case FPIEVENT_QUERY:
        if (options.interest_infections)
        {
            if (options.ask)
                printboth("%s:\t%s:\t%s\n",
                    fullname, eventstring, virusmessage);
            else
                printlog("%s:\t%s:\t%s\n",
                    fullname, eventstring, virusmessage);
            collect_sample("inf", FPICONTAINERHANDLE_NULL, fullname, name, handle);
        }
        break;
    case FPIEVENT_INFECTION:
        if (options.interest_infections)
        {
            printlog("%s:\t%s:\t%s\n",
                fullname, eventstring, virusmessage);
            collect_sample("inf", FPICONTAINERHANDLE_NULL, fullname, name, handle);
        }
        break;
    case FPIEVENT_ERROR:
        if (options.interest_errors)
        {
			if (param_!=FPIERROR_WRONGAPI || !options.terse)
			{
				printboth("%s:\t%s\t(%hu = %s) %s\n",
					fullname, eventstring, param_,
					(param_ <= FPIERROR_LASTERROR ? fpierror[param_] : "unknown"),
					(message_ == NULL ? "" : message_));
				collect_sample("err", FPICONTAINERHANDLE_NULL, fullname, name, handle);
			}
        }
        break;
    }

    if (event_ == FPIEVENT_INFECTION)
    {
        if (flags_ & FPIFLAG_SUSPECTED)
        {
            nSuspicious++;
            sumTicksSuspicious += GetTickCount() - tickStartFile;
        }
        else
        {
            nInfections++;
            sumTicksInfected += GetTickCount() - tickStartFile;
        }
    }

    if (event_ == FPIEVENT_ERROR)
    {
        nScanErrors++;
    }

    if (event_ == FPIEVENT_QUERY && param_ != FPIACTION_NONE)
    {
        if (param_ == FPIACTION_DISINFECT)
            strcpy(actionstring, "Disinfect");
        else
            sprintf(actionstring, "Invalid action (%hu)", param_);

        if (!options.ask)
        {
            if (options.interest_infections)
                printlog("\tQuery: %s (yes, without asking)\n", actionstring);
            return TRUE;
        }
        else
        {
            if (query(actionstring))
            {
                if (options.interest_infections)
                    printboth(" yes\n");
                return TRUE;
            }
            else
            {
                if (options.interest_infections)
                    printboth(" no\n");
                return FALSE;
            }
        }
    }

    if (event_ == FPIEVENT_INFECTION)
    {
        if (param_ == FPIACTION_NONE)
        {
            if (options.disinfect)
                strcpy(actionstring, "Nothing done");
            else
                strcpy(actionstring, "");
        }
        else if (param_ == FPIACTION_DISINFECT)
        {
            strcpy(actionstring, "Disinfected");
            isFileDisinfected = TRUE;
            nDisinfections++;
        }
        else
        {
            sprintf(actionstring, "Invalid action performed (%hu)", param_);
        }
        if (actionstring[0] != '\0')
        {
            if (options.interest_infections)
                printlog("\t%s\n", actionstring);
        }
    }

    return FALSE;
}




BOOL query(const char* actionstring)
{
    BOOL answer;
    printboth("\tQuery: %s, do it? (y/n) ", actionstring);

#ifdef _CONSOLE
    int c;
#ifdef UNIX
    c = getchar();
#else
    c = getch();
#endif
    answer = (c=='y' || c=='Y');
#else
    int ret;
    char querytext[100];
    sprintf(querytext, "Action==\"%s\", do it?", actionstring);
    ret = MessageBox(NULL, querytext, "FPIRUN Query",
        MB_YESNO | MB_ICONQUESTION);
    answer = (ret==IDYES);
#endif

    return answer;
}

int get_db_year(DWORD date)
{
    return date >> 16;
}

int get_db_month(DWORD date)
{
    return (date >> 8) & 0xff;
}

int get_db_day(DWORD date)
{
    return date & 0xff;
}

const char* get_db_state(WORD param_)
{
	switch (param_)
	{
	case FSAV_DB_VALID: return "valid";
	case FSAV_DB_NOTCHECKED: return "not checked";
	case FSAV_DB_DELETE: return "not in use, should be deleted";
	case FSAV_DB_MISSING: return "missing";
	case FSAV_DB_INVALID: return "not valid";
	case FSAV_DB_NOTDB: return "not a database file";
	case FSAV_DB_CORRUPT: return "corrupt";
	case FSAV_DB_VERSION: return "wrong version";
	default: return "?";
	}
}

void create_control_file(const char* name, DWORD year, WORD month, WORD day)
{
	const char* control_name;

	if (stricmp(name, "sign.def") == 0)
		control_name = "signdef-version";
	else if (stricmp(name, "fsmacro.def") == 0)
		control_name = "fsmacrodef-version";
	else
		return;

	FILE* f = fopen(control_name, "w");
	if (!f)
	{
		printboth("Cannot write control file %s\n", control_name);
	}

	fprintf(f, "%s version %lu-%02hu-%02hu\n", name, year, month, day);
	fclose(f);
}

BOOL event_dbvalidation(WORD event_, WORD param_, DWORD flags_, LPCSTR message_, DWORD refdata_)
{
    // param_ contains FPIDB_xxx
    // flags_ contains date
    // message_ contains name

#ifdef FSAV
    printboth("%s version %04lu-%02hu-%02hu",
	      message_, get_db_year(flags_), get_db_month(flags_), get_db_day(flags_));
    if (param_ != FPIDB_VALID)
	printboth(" %s", get_db_state(param_));
    printboth("\n");
    
    check_db_date(message_, flags_);
#else
    printboth(">> Database:\t\"%s\", status=%hu (%s), date=%04d-%02d-%02d\n",
	      message_, param_, get_db_state(param_),
	      get_db_year(flags_), get_db_month(flags_), get_db_day(flags_));

	if (options.control)
	{
		create_control_file(message_, get_db_year(flags_), get_db_month(flags_), get_db_day(flags_));
	}
#endif
    return TRUE;
}

#ifdef FSAV
void check_db_date(const char* dbname, DWORD date)
{
    struct tm t;
    time_t now, db;
    double diff;

    if (date == 0)
	return;

    now = time(NULL);

    t.tm_year = get_db_year(date) - 1900;
    t.tm_mon = get_db_month(date) - 1;
    t.tm_mday = get_db_day(date);
    t.tm_hour = 0;
    t.tm_min = 0;
    t.tm_sec = 0;
    db = mktime(&t);

    if (difftime(now, db) > 240*86400.0)
    {
	printboth("\n"
		  "Warning: The %s file is rather old and does not contain\n"
		  "information on a substantial number of new viruses.\n"
		  "To update your copy of F-Secure Anti-Virus, connect to the Internet\n"
		  "and run the fsavupdate utility.\n\n",
		  dbname);
    }
}
#endif
