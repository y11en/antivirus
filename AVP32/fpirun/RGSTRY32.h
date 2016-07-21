// rgstry32.h


#ifdef __cplusplus
extern "C" {
#endif

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
int GetRegistryValue(char * dest_, unsigned destlen_, const char * locator_);


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

int SetRegistryValue(const char * locator_, int flush_, int type_);


#ifdef __cplusplus
}
#endif

