//  rgstry32.cpp
//  Urmas Rahu, Data Fellows, 1996

//  Win32 registry functions


#include <windows.h>
//Kikka970306{
#include <stdio.h>
//Kikka970306}

#include <winreg.h>
#include "rgstry32.h"


static struct {HKEY fMainKeyId; const char * fMainKeyName;} gsMainKeysTable[] =
    {
        {HKEY_CLASSES_ROOT,     "HKEY_CLASSES_ROOT"},
        {HKEY_CURRENT_USER,     "HKEY_CURRENT_USER"},
        {HKEY_LOCAL_MACHINE,    "HKEY_LOCAL_MACHINE"},
        {HKEY_USERS,            "HKEY_USERS"},

        {0, 0}
    };

static HKEY GetKeyId(const char * keyname_)
    {
    for (int i=0; gsMainKeysTable[i].fMainKeyName; i++)
        if (!strcmpi(gsMainKeysTable[i].fMainKeyName, keyname_))
            return gsMainKeysTable[i].fMainKeyId;

    return 0;
    }

int ParseRegistryLocator(HKEY * key_, char ** valuename_, char ** valuedata_, int * mainkey_, const char * locator_, int mode_);


/*
    GetRegistryValue()

    Fetches a velue from registry, described by <locator_>.  The locator
    format is:

        "MAINKEY[\subkey]\\[valuename]"

        MAINKEY     : one of: "HKEY_CLASSES_ROOT", "HKEY_CURRENT_USER", "HKEY_LOCAL_MACHINE", "HKEY_USERS"
        subkey      : subkey, eg "Network\Logon"
        valuename   : value name

    Returns:
          0         : ok
          1         : specified key not found
          2         : specified value not found
          3         : value too long
          4         : invalid locator format
        100         : out of memory

    Locator examples:
        "HKEY_LOCAL_MACHINE\Network\Logon\\username"
        "HKEY_LOCAL_MACHINE\\user-name"
        "HKEY_LOCAL_MACHINE\Network\Logon\\"
        "HKEY_LOCAL_MACHINE\\"


*/
int GetRegistryValue(char * dest_, unsigned destlen_, const char * locator_)
    {
    int ret=0;
    LONG rc;

    HKEY key;
    char * valuename;
    int mainkey;

    DWORD datalen, valuetype;


    rc = ParseRegistryLocator(&key, &valuename, 0, &mainkey, locator_, 0);

    if (!rc)
        {
        datalen=destlen_;

        rc = RegQueryValueEx(key, valuename, 0, &valuetype, (LPBYTE)dest_, &datalen);
        if (rc == ERROR_MORE_DATA)
            ret = 3;                                // 3: value too long
        else if (rc != ERROR_SUCCESS)
            ret = 2;                                // 2: no such value

        if (!mainkey)
            RegCloseKey(key);                       // (must not close main key)

        free(valuename);                            // must free then
        }
    else
        ret = (int)rc;                              // pass back error code

    return ret;
    }



/*
    SetRegistryValue()

    Sets a string value in registry, described by <locator_>.  The locator
    format is:

        "MAINKEY[\subkey]\\[valuename]|[valuedata]"

        MAINKEY     : one of: "HKEY_CLASSES_ROOT", "HKEY_CURRENT_USER", "HKEY_LOCAL_MACHINE", "HKEY_USERS"
        subkey      : subkey, eg "Network\Logon"
        valuename   : value name
        valuedata   : value data
        type        : type value for the key, if 0 REG_SZ is used
                      other possible values:
                      2, REG_EXPAND_SZ
                      3, REG_BINARY
                      4, REG_DWORD
                      etc. see winnt.h for more values

    Note: the value will be deleted if valuedata is "".

    Returns:
          0         : ok
          1         : specified key not found
          2         : error setting value
          4         : invalid locator format
        100         : out of memory

    Locator examples:
        "HKEY_LOCAL_MACHINE\Network\Logon\\username|urr"
        "HKEY_LOCAL_MACHINE\\user-name|urr"
        "HKEY_LOCAL_MACHINE\Network\Logon\\|urr"
        "HKEY_LOCAL_MACHINE\\|urr"


*/

//urr960621{:
//Kikka970306{:
int SetRegistryValue(const char * locator_, int flush_, int type_)
//int SetRegistryValue(const char * locator_, int flush_)
//Kikka970306}:
//int SetRegistryValue(const char * locator_)
//urr960621}:
	{
	int ret=0;
	LONG rc;

	HKEY key;
	char * valuename, *valuedata;
	int mainkey;
//Kikka970306{
    int wintype = REG_SZ;
    if (type_)
        wintype = type_;
//Kikka970306}


	rc = ParseRegistryLocator(&key, &valuename, &valuedata, &mainkey, locator_, 1);

	if (!rc)
		{
		if (valuedata)
			{                                 
			if (*valuedata)
//Kikka970306{:
            {
                if (REG_DWORD==wintype)
                {
                    DWORD dwtmp = atol(valuedata);
    				rc = RegSetValueEx(key, valuename, 0, REG_DWORD, (const BYTE *)&dwtmp, sizeof(DWORD));
                }
                else
    				rc = RegSetValueEx(key, valuename, 0, wintype, (const BYTE *)valuedata, strlen(valuedata) + 1);

//				rc = RegSetValueEx(key, valuename, 0, REG_SZ, (const BYTE *)valuedata, strlen(valuedata) + 1);
            }
//Kikka970306}:
			else
				rc = RegDeleteValue(key, valuename);

			if (rc != ERROR_SUCCESS)
				ret = 2;                            // 2: error setting value

			free(valuedata);
			}
		else
			{                                       // no value data was present after locator
			ret = 4;                                // 4: invalid locator
			}

		if (!mainkey)
			{
//urr960621{
			if (flush_)
				RegFlushKey(key);
//urr960621}
			RegCloseKey(key);                       // (must not close main key)
			}

        free(valuename);                            // must free then
        }
    else
        ret = (int)rc;                              // pass back error code

    return ret;
    }

//Kikka961231{
int DeleteRegistryKey(const char * locator_, int flush_)
{
	int ret=0;
	LONG rc;

	HKEY key;
	char * subkeyname;
	int mainkey;

    rc = ParseRegistryLocator(&key, &subkeyname, 0, &mainkey, locator_, 1);

	if (!rc && *subkeyname)
    {
        rc = RegDeleteKey(key, subkeyname);

//urr970526{:
	    if (rc != ERROR_SUCCESS && rc != ERROR_FILE_NOT_FOUND)  // don't consider not found an error!
//	    if (rc != ERROR_SUCCESS)
//urr970526}:
		    ret = 2;                            // 2: error setting value

        free(subkeyname);

		if (flush_)
		    RegFlushKey(key);

        if (!mainkey)
        	RegCloseKey(key);                       // (must not close main key)
	}
    else
        ret = (int)rc;                              // pass back error code

    return ret;
}
//Kikka961231}

int ParseRegistryLocator(HKEY * key_, char ** valuename_, char ** valuedata_, int * mainkey_, const char * locator_, int mode_)
    {
    int ret = 0;

    LONG rc;
    HKEY key, mainkey;
    DWORD disposition;

    char * locator = strdup(locator_);
    if (!locator)
        return 100;                                 // 100: out of memory

    char * subkey = strchr(locator, '\\');
    char * valuename, * valuedata, *ptr;
    int i;

    int writemode = mode_ == 1;

    *valuename_ = 0;
    if (valuedata_)
        *valuedata_ = 0;

    if (!subkey)                                    // point <subkey> to subkey
        goto invalidloc;

    *subkey++ = '\0';                               // place '\0' so that <locator> points to MAINKEY now

    valuename = subkey;                             // find end of subkey and start of value name

    while (*valuename != '\\')                      // loop until value name found
        {
        if (*valuename == '\0')                     // locator terminated before value name encountered
            goto invalidloc;

        valuename = strchr(valuename, '\\');
        if (!valuename)
            goto invalidloc;                        // locator terminated before value name encountered

        ++valuename;
        }

    *--valuename = '\0';                            // terminate subkey, removing the last backslash

    ptr = valuename += 2;                           // point to value name

    valuedata = strchr(valuename, '|');
    if (valuedata)
        {
        *valuedata++ = '\0';                        // point to value data
        ptr = valuedata;
        }

    i = strlen(ptr);
    for (--i; i>=0 && ptr[i] == ' '; i--)           // strip trailing spaces
        ptr[i] = '\0';

    if (0 == (mainkey = GetKeyId(locator)))
        {                                           // main key not recognized
        invalidloc:

        ret = 4;                                    // 4: invalid locator

        goto freeloc;
        }


/*
    rc = RegOpenKeyEx(
      HKEY_LOCAL_MACHINE,
      "Network\\Logon\\",
      0,
      KEY_READ,
      &key);
*/

    if (*subkey != '\\')
        {                                           // there was a subkey
        *mainkey_ = 0;
        if (writemode)
            rc = RegCreateKeyEx(mainkey, subkey, 0, 0, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, 0, &key, &disposition);
        else
            rc = RegOpenKeyEx(mainkey, subkey, 0, KEY_READ, &key);

        if (ERROR_SUCCESS != rc)
            {
            ret = 1;                                // 1: no such key
            goto freeloc;
            }
        }
    else
        {                                           // no subkey
        *mainkey_ = 1;
        key = mainkey;
        }


    *key_ = key;

    *valuename_ = strdup(valuename);                // strdup these, must be free()d by the caller
    if (valuedata && valuedata_)
        *valuedata_ = strdup(valuedata);


    freeloc:
    free(locator);

    return ret;
    }


