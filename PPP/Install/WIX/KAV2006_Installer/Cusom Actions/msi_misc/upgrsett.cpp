#include "upgrsett.h"

using namespace AVPUpgradeSettings;

class AVPUpgrade
{
public:
    AVPUpgrade(CRegKey& srcrootkey, CRegKey& dstrootkey, IUpgrTracer *tracer);

protected:
    bool translate_profile(const std::wstring& profilepath, std::wstring& dstprofilepath);

    bool filter_profile_values(const std::wstring& profilepath, const std::wstring& valuename);
    bool filter_schedule(const std::wstring& profilepath, const std::wstring& valuename);
    bool filter_smode_values(const std::wstring& profilepath, const std::wstring& valuename);
    bool filter_smode_keys(const std::wstring& profilepath, const std::wstring& valuename);

    bool translate_settings(const std::wstring& profilepath, const std::wstring& settingpath, const std::wstring& settingname,
                            std::wstring& dstprofilepath, std::wstring& dstsettingpath, std::wstring& dstsettingname);

    CRegKey create_profile(CRegKey& rootkey, const std::wstring& profilepath);
    CRegKey create_settings(CRegKey& rootkey, const std::wstring& profilepath, const std::wstring& settingpath);

    CRegKey get_profile(CRegKey& rootkey, const std::wstring& profilepath);
    CRegKey get_settings(CRegKey& rootkey, const std::wstring& profilepath, const std::wstring& settingpath);

    void upgrade_settings_value(const std::wstring& profilepath, const std::wstring& settingpath, const std::wstring& settingname, CRegValue& srcvalue);
    void upgrade_settings_key(const std::wstring& profilepath, const std::wstring& settingpath);
    void upgrade_settings_values(const std::wstring& profilepath, const std::wstring& settingpath, CRegKey& srckey);
    void upgrade_settings(const std::wstring& profilepath, CRegKey& srckey);
    void upgrade_schedule(const std::wstring& profilepath, CRegKey& srckey);
    void upgrade_smode(const std::wstring& profilepath, CRegKey& srckey);
    void upgrade_profile_values(const std::wstring& profilepath, CRegKey& srckey);
    void upgrade_profile(const std::wstring& profilepath, CRegKey& srckey);

    unsigned int get_settings_dword(const std::wstring& profilepath, const std::wstring& settingpath, const std::wstring& settingname);
    void set_settings_dword(const std::wstring& profilepath, const std::wstring& settingpath, const std::wstring& settingname, unsigned int value);

    void dumpkey(CRegKey& key, int level = 0);
    void dumpvalue(CRegValue& val, int level = 0);

    void setversion();

    int m_vermajor;
    int m_verminor;
    int m_vermpack;
    int m_verbuild;

    CRegKey& m_srcrootkey;
    CRegKey& m_dstrootkey;
    IUpgrTracer *m_tracer;
};

#define UPGR_TRACE(LIST) { if (m_tracer) m_tracer->printf LIST; }

// -----------------------------------------------------------------------------------------

bool AVPUpgrade::translate_profile(const std::wstring& profilepath, std::wstring& dstprofilepath)
{
    // default translation
    bool pass = true;
    dstprofilepath = profilepath;

    // specific translation
    if      (profilepath == L"Behavior_Blocking\\og") pass = false;
    else if (profilepath == L"Retranslation")         pass = false;

    else if (profilepath == L"Anti_Spy\\AdBlocker") dstprofilepath = L"Anti_Hacker\\AdBlocker";
    else if (profilepath == L"Anti_Spy\\popupchk")  dstprofilepath = L"Anti_Hacker\\popupchk";

    return pass;
}

bool AVPUpgrade::filter_profile_values(const std::wstring& profilepath, const std::wstring& valuename)
{
    if (valuename == L"enabled") return true;
    if (valuename == L"level") return true;

    return false;
}

bool AVPUpgrade::filter_schedule(const std::wstring& profilepath, const std::wstring& valuename)
{
    return true;
}

bool AVPUpgrade::filter_smode_values(const std::wstring& profilepath, const std::wstring& valuename)
{
    return true;
}

bool AVPUpgrade::filter_smode_keys(const std::wstring& profilepath, const std::wstring& valuename)
{
    return true;
}

bool AVPUpgrade::translate_settings(
    const std::wstring& profilepath, 
    const std::wstring& settingpath, 
    const std::wstring& settingname,
    std::wstring& dstprofilepath, 
    std::wstring& dstsettingpath, 
    std::wstring& dstsettingname)
{
    // default translation
    bool pass = translate_profile(profilepath, dstprofilepath);
    dstsettingpath = settingpath;
    dstsettingname = settingname;


    // specific translation
    if (profilepath == L"" && settingpath == L"" && settingname == L"LicActivationLink") 
        pass = false;

    if (profilepath == L"" && (settingpath == L"NSettings" || settingpath.find(L"NSettings\\") == 0))
        pass = false;

    if (profilepath == L"Behavior_Blocking\\pdm" && settingpath == L"Set")
    {
        if (m_vermajor == 6 && m_vermpack == 0)
        {
            set_settings_dword (L"Behavior_Blocking\\pdm", L"Set\\0007", L"SetType",        8);
            set_settings_dword (L"Behavior_Blocking\\pdm", L"Set\\0007", L"bEnabled",       0);
            set_settings_dword (L"Behavior_Blocking\\pdm", L"Set\\0007", L"Action",         4);
            set_settings_dword (L"Behavior_Blocking\\pdm", L"Set\\0007", L"bLog",           1);
            set_settings_dword (L"Behavior_Blocking\\pdm", L"Set\\0007", L"bQuarantine",    0);
            set_settings_dword (L"Behavior_Blocking\\pdm", L"Set\\0007", L"bAutoRollback",  0);
            set_settings_dword (L"Behavior_Blocking\\pdm", L"Set\\0007", L"Timeout",       14);

            set_settings_dword (L"Behavior_Blocking\\pdm", L"Set\\0008", L"SetType",        9);
            set_settings_dword (L"Behavior_Blocking\\pdm", L"Set\\0008", L"bEnabled",       0);
            set_settings_dword (L"Behavior_Blocking\\pdm", L"Set\\0008", L"Action",         2);
            set_settings_dword (L"Behavior_Blocking\\pdm", L"Set\\0008", L"bLog",           0);
            set_settings_dword (L"Behavior_Blocking\\pdm", L"Set\\0008", L"bQuarantine",    0);
            set_settings_dword (L"Behavior_Blocking\\pdm", L"Set\\0008", L"bAutoRollback",  0);
            set_settings_dword (L"Behavior_Blocking\\pdm", L"Set\\0008", L"Timeout",       14);
        }
    }

    if (profilepath == L"Spamtest" && settingpath == L"" && settingname == L"BaseFolder")
        pass = false;

    if (profilepath == L"Updater" && settingpath == L"")
    {
        if      (settingname == L"UseProxy")            dstprofilepath = L"", 
                                                        dstsettingpath = L"ProxySettings", 
                                                        dstsettingname = L"UseProxy";

        else if (settingname == L"UseIEProxySettings")  dstprofilepath = L"", 
                                                        dstsettingpath = L"ProxySettings", 
                                                        dstsettingname = L"UseIEProxySettings";

        else if (settingname == L"ProxyHost")           dstprofilepath = L"", 
                                                        dstsettingpath = L"ProxySettings", 
                                                        dstsettingname = L"ProxyHost";

        else if (settingname == L"ProxyPort")           dstprofilepath = L"", 
                                                        dstsettingpath = L"ProxySettings", 
                                                        dstsettingname = L"ProxyPort";

        else if (settingname == L"ProxyLogin")          dstprofilepath = L"", 
                                                        dstsettingpath = L"ProxySettings", 
                                                        dstsettingname = L"ProxyLogin";

        else if (settingname == L"ProxyPassword")       dstprofilepath = L"", 
                                                        dstsettingpath = L"ProxySettings", 
                                                        dstsettingname = L"ProxyPassword";

        else if (settingname == L"BypassProxyForLocalServer") 
                                                        dstprofilepath = L"", 
                                                        dstsettingpath = L"ProxySettings", 
                                                        dstsettingname = L"BypassProxyServerForLocalAddresses";

        else if (settingname == L"UseProxyCredentials")
        {
            if (m_vermpack == 2)
            {
                dstprofilepath = L"", 
                dstsettingpath = L"ProxySettings", 
                dstsettingname = L"ProxyAuth";
            }
            else
                pass = false;
        }
        else if (settingname == L"ProxyAuth")
        {
            if (m_vermpack < 2)
            {
                if (get_settings_dword(profilepath, settingpath, L"ProxyAuth") == 1 &&
                    get_settings_dword(profilepath, settingpath, L"UseNTLMAuth") == 0)
                {
                    set_settings_dword(L"", L"ProxySettings", L"ProxyAuth", 1);
                }
                else
                {
                    set_settings_dword(L"", L"ProxySettings", L"ProxyAuth", 0);
                }
                pass = false;
            }
            else
                pass = false;
        }

        else if (settingname == L"UseNTLMAuth")         dstprofilepath = L"", 
                                                        dstsettingpath = L"ProxySettings", 
                                                        dstsettingname = L"UseNTLMAuth";

    }

    return pass;
}



// -----------------------------------------------------------------------------------------

void AVPUpgrade::dumpvalue(CRegValue& val, int level)
{
    char space[1024];
    for(int l = level; l; --l)
        strcat(space, " ");

    switch(val.gettype())
    {
    case ERegTypeBinary:
        UPGR_TRACE(("%s%ls(REG_BINARY)", space, val.getname().c_str()));
        break;
    case ERegTypeDword:
        UPGR_TRACE(("%s%ls(REG_DWORD) = 0x%x (%u)", space, val.getname().c_str(), val.getdword(), val.getdword()));
        break;
    case ERegTypeQword:
        UPGR_TRACE(("%s%ls(REG_QWORD) = %%u", space, val.getname().c_str(), val.getqword()));
        break;
    case ERegTypeString:
        UPGR_TRACE(("%s%ls(REG_SZ) = %ls", space, val.getname().c_str(), val.getstring().c_str()));
        break;
    case ERegTypeExpandString:
        UPGR_TRACE(("%s%ls(REG_EXPAND_SZ) = %ls", space, val.getname().c_str(), val.getstring().c_str()));
        break;
    case ERegTypeNone:
        UPGR_TRACE(("%s%ls(REG_NONE)", space, val.getname().c_str()));
        break;
    }
}

void AVPUpgrade::dumpkey(CRegKey& key, int level)
{
    char space[1024];
    for(int l = level; l; --l)
        strcat(space, " ");

    UPGR_TRACE(("%s[%ls]", space, key.getname().c_str()));

    for (CRegValueIterator i = key.enumvalues(); i; ++i)
    {
        dumpvalue(i.get(), level + 1);
    }

    for (CRegKeyIterator i = key.enumkeys(); i; ++i)
    {
        dumpkey(i.get(), level + 1);
    }
}

// -----------------------------------------------------------------------------------------

unsigned int AVPUpgrade::get_settings_dword(const std::wstring& profilepath, const std::wstring& settingpath, const std::wstring& settingname)
{
    CRegKey key_setting = get_settings(m_srcrootkey, profilepath, settingpath);
    if (key_setting)
    {
        CRegValue value_setting = key_setting.getvalue(settingname.c_str());
        if (value_setting)
            return value_setting.getdword();
        else
            return 0;
    }
    else
        return 0;
}

void AVPUpgrade::set_settings_dword(const std::wstring& profilepath, const std::wstring& settingpath, const std::wstring& settingname, unsigned int value)
{
    CRegKey key_setting = create_settings(m_dstrootkey, profilepath, settingpath);
    if (key_setting)
    {
        key_setting.setvalue(settingname.c_str(), ERegTypeDword, &value, sizeof(value));
    }
}

// -----------------------------------------------------------------------------------------

std::vector<std::wstring> path_wstring_to_vect(std::wstring path_str)
{
    std::vector<std::wstring> path_vect;
    if (!path_str.empty())
    {
        size_t curr = 0;
        do
        {
            size_t next = path_str.find(L'\\', curr);
            if (next != std::wstring::npos)
            {
                path_vect.push_back(path_str.substr(curr, next - curr));
                curr = next + 1;
            }
            else
            {
                path_vect.push_back(path_str.substr(curr));
                curr = next;
            }
        }
        while (curr != std::wstring::npos);
    }
    return path_vect;
}

CRegKey AVPUpgrade::create_profile(CRegKey& rootkey, const std::wstring& profilepath)
{
    std::vector<std::wstring> profilepath_vect = path_wstring_to_vect(profilepath);

    CRegKey currkey = rootkey;
    for (std::vector<std::wstring>::iterator i = profilepath_vect.begin(); i != profilepath_vect.end(); ++i)
    {
        if (currkey) currkey = currkey.createkey(L"profiles");
        if (currkey) currkey = currkey.createkey(i->c_str());
    }

    return currkey;
}

CRegKey AVPUpgrade::create_settings(CRegKey& rootkey, const std::wstring& profilepath, const std::wstring& settingpath)
{
    CRegKey currkey = create_profile(rootkey, profilepath);
    if (currkey)
    {
        currkey = currkey.createkey(L"settings");
        if (currkey)
        {
            std::vector<std::wstring> settingpath_vect = path_wstring_to_vect(settingpath);

            for (std::vector<std::wstring>::iterator i = settingpath_vect.begin(); i != settingpath_vect.end(); ++i)
            {
                if (currkey) currkey = currkey.createkey(i->c_str());
            }
        }
    }
    return currkey;
}

CRegKey AVPUpgrade::get_profile(CRegKey& rootkey, const std::wstring& profilepath)
{
    std::vector<std::wstring> profilepath_vect = path_wstring_to_vect(profilepath);

    CRegKey currkey = rootkey;
    for (std::vector<std::wstring>::iterator i = profilepath_vect.begin(); i != profilepath_vect.end(); ++i)
    {
        if (currkey) currkey = currkey.getkey(L"profiles");
        if (currkey) currkey = currkey.getkey(i->c_str());
    }

    return currkey;
}

CRegKey AVPUpgrade::get_settings(CRegKey& rootkey, const std::wstring& profilepath, const std::wstring& settingpath)
{
    CRegKey currkey = get_profile(rootkey, profilepath);
    if (currkey)
    {
        currkey = currkey.getkey(L"settings");
        if (currkey)
        {
            std::vector<std::wstring> settingpath_vect = path_wstring_to_vect(settingpath);

            for (std::vector<std::wstring>::iterator i = settingpath_vect.begin(); i != settingpath_vect.end(); ++i)
            {
                if (currkey) currkey = currkey.getkey(i->c_str());
            }
        }
    }
    return currkey;
}

void AVPUpgrade::upgrade_settings_value(const std::wstring& profilepath, const std::wstring& settingpath, const std::wstring& settingname, CRegValue& srcvalue)
{
    std::wstring dstprofilepath;
    std::wstring dstsettingpath;
    std::wstring dstsettingname;

    if (translate_settings(profilepath, settingpath, settingname, 
                           dstprofilepath, dstsettingpath, dstsettingname))
    {
        UPGR_TRACE(("upgrade_settings_value: %ls.%ls.%ls -> %ls.%ls.%ls", profilepath.c_str(), settingpath.c_str(), settingname.c_str(), dstprofilepath.c_str(), dstsettingpath.c_str(), dstsettingname.c_str()));
    
        CRegKey dstkey = create_settings(m_dstrootkey, dstprofilepath, dstsettingpath);
        if (dstkey)
        {
            srcvalue.copyto(dstkey, dstsettingname.c_str());
        }
    }
    else
        UPGR_TRACE(("upgrade_settings_value: %ls.%ls.%ls filered out", profilepath.c_str(), settingpath.c_str(), settingname.c_str()));
}

void AVPUpgrade::upgrade_settings_key(const std::wstring& profilepath, const std::wstring& settingpath)
{
    std::wstring dstprofilepath;
    std::wstring dstsettingpath;
    std::wstring dstsettingname;

    if (translate_settings(profilepath, settingpath, L"",
                           dstprofilepath, dstsettingpath, dstsettingname))
    {
        UPGR_TRACE(("upgrade_settings_key:   %ls.%ls -> %ls.%ls", profilepath.c_str(), settingpath.c_str(), dstprofilepath.c_str(), dstsettingpath.c_str()));

        create_settings(m_dstrootkey, dstprofilepath, dstsettingpath);
    }
    else
        UPGR_TRACE(("upgrade_settings_key:   %ls.%ls filered out", profilepath.c_str(), settingpath.c_str()));
}

void AVPUpgrade::upgrade_settings_values(const std::wstring& profilepath, const std::wstring& settingpath, CRegKey& srckey)
{
    upgrade_settings_key(profilepath, settingpath);

    for (CRegValueIterator i = srckey.enumvalues(); i; ++i)
    {
        upgrade_settings_value(profilepath, settingpath, i.getname(), i.get());
    }

    for (CRegKeyIterator i = srckey.enumkeys(); i; ++i)
    {
        if (i.getname() != L"def" &&
            i.getname() != L"0" && i.getname() != L"1" &&
            i.getname() != L"2" && i.getname() != L"3" &&
            i.getname() != L"4" && i.getname() != L"5")
        {
            upgrade_settings_values(profilepath, 
                                    settingpath.empty() ? i.getname() : 
                                                          settingpath + L"\\" + i.getname(),
                                    i.get());
        }
    }
}

void AVPUpgrade::upgrade_settings(const std::wstring& profilepath, CRegKey& srckey)
{
    CRegKey srckey_settings = srckey.getkey(L"settings");
    if (srckey_settings)
    {
        UPGR_TRACE(("upgrade_settings: %ls", profilepath.c_str()));

        upgrade_settings_values(profilepath, L"", srckey_settings);
    }
}

void AVPUpgrade::upgrade_schedule(const std::wstring& profilepath, CRegKey& srckey)
{
    CRegKey srckey_schedule = srckey.getkey(L"schedule");
    if (srckey_schedule)
    {
        UPGR_TRACE(("upgrade_schedule: %ls", profilepath.c_str()));

        std::wstring dstprofilepath;
        if (translate_profile(profilepath, dstprofilepath))
        {
            CRegKey dstkey_profile = create_profile(m_dstrootkey, dstprofilepath);
            if (dstkey_profile)
            {
                CRegKey dstkey_schedule = dstkey_profile.createkey(L"schedule");
                if (dstkey_schedule)
                {
                    for (CRegValueIterator i = srckey_schedule.enumvalues(); i; ++i)
                    {
                        if (filter_schedule(profilepath, i.getname()))
                        {
                            if (!i.get().copyto(dstkey_schedule))
                                UPGR_TRACE(("upgrade_schedule: can't copy value %ls", i.getname().c_str()));
                        }
                        else
                            UPGR_TRACE(("upgrade_schedule: value %ls filered out", i.getname().c_str()));
                    }
                }
                else
                    UPGR_TRACE(("upgrade_schedule: unable to create schedule key"));
            }
        }
    }
}

void AVPUpgrade::upgrade_smode(const std::wstring& profilepath, CRegKey& srckey)
{
    CRegKey srckey_smode = srckey.getkey(L"smode");
    if (srckey_smode)
    {
        UPGR_TRACE(("upgrade_smode: %ls", profilepath.c_str()));

        std::wstring dstprofilepath;
        if (translate_profile(profilepath, dstprofilepath))
        {
            CRegKey dstkey_profile = create_profile(m_dstrootkey, dstprofilepath);
            if (dstkey_profile)
            {
                CRegKey dstkey_smode = dstkey_profile.createkey(L"smode");
                if (dstkey_smode)
                {
                    for (CRegValueIterator i = srckey_smode.enumvalues(); i; ++i)
                    {
                        if (filter_smode_values(profilepath, i.getname()))
                        {
                            if (!i.get().copyto(dstkey_smode))
                                UPGR_TRACE(("upgrade_smode: can't copy value %ls", i.getname().c_str()));
                        }
                        else
                            UPGR_TRACE(("upgrade_smode: value %ls filered out", i.getname().c_str()));
                    }

                    for (CRegKeyIterator i = srckey_smode.enumkeys(); i; ++i)
                    {
                        if (filter_smode_keys(profilepath, i.getname()))
                        {
                            CRegKey dstkey = dstkey_smode.createkey(i.getname().c_str());
                            if (dstkey)
                            {
                                if (!i.get().copyto(dstkey_smode))
                                    UPGR_TRACE(("upgrade_smode: can't copy key %ls", i.getname().c_str()));
                            }
                            else
                                UPGR_TRACE(("upgrade_smode: can't create key %ls", i.getname().c_str()));
                        }
                        else
                            UPGR_TRACE(("upgrade_smode: key %ls filered out", i.getname().c_str()));
                    }
                }
                else
                    UPGR_TRACE(("upgrade_smode: unable to create smode key"));
            }
        }
    }
}

void AVPUpgrade::upgrade_profile_values(const std::wstring& profilepath, CRegKey& srckey)
{
    std::wstring dstprofilepath;
    if (translate_profile(profilepath, dstprofilepath))
    {
        CRegKey dstkey_profile = create_profile(m_dstrootkey, dstprofilepath);
        if (dstkey_profile)
        {
            for (CRegValueIterator i = srckey.enumvalues(); i; ++i)
            {
                if (filter_profile_values(profilepath, i.getname()))
                {
                    if (!i.get().copyto(dstkey_profile))
                        UPGR_TRACE(("upgrade_profile_values: can't copy value %ls", i.getname().c_str()));
                }
                else
                    UPGR_TRACE(("upgrade_profile_values: value %ls filered out", i.getname().c_str()));
            }
        }
    }
}

bool isuserprofile(CRegKey& key)
{
    CRegValue origin = key.getvalue(L"origin");

    if (origin && origin.getdword() == 2)
        return true;
    else
        return false;
}

void AVPUpgrade::upgrade_profile(const std::wstring& profilepath, CRegKey& srckey)
{
    UPGR_TRACE(("upgrade_profile: system: '%ls'", profilepath.c_str()));

    if (! isuserprofile(srckey)) // system profile
    {
        UPGR_TRACE(("upgrade_profile: system profile"));

        upgrade_profile_values(profilepath, srckey);

        upgrade_settings(profilepath, srckey);

        upgrade_schedule(profilepath, srckey);

        upgrade_smode(profilepath, srckey);

        CRegKey srckey_profiles = srckey.getkey(L"profiles");
        if (srckey_profiles)
        {
            for (CRegKeyIterator i = srckey_profiles.enumkeys(); i; ++i)
            {
                std::wstring profilename = i.getname();
                upgrade_profile(profilepath.empty() ? profilename : 
                                                      profilepath + L"\\" + profilename, 
                                i.get());
            }
        }
    }
    else // user profile
    {
        UPGR_TRACE(("upgrade_profile: user profile"));

        CRegKey dstkey_profile = create_profile(m_dstrootkey, profilepath);
        if (dstkey_profile)
        {
            if (!srckey.copyto(dstkey_profile))
                UPGR_TRACE(("upgrade_profile: can't copy key %ls to %ls", srckey.getname().c_str(), dstkey_profile.getname().c_str()));
        }
    }
}

void AVPUpgrade::setversion()
{
    CRegKey srckey_environment = m_srcrootkey.getkey(L"environment");
    if (srckey_environment)
    {
        CRegValue productversion = srckey_environment.getvalue(L"ProductVersion");
        if (productversion)
        {
            std::wstring version = productversion.getstring();
            swscanf(version.c_str(), L"%d.%d.%d.%d", &m_vermajor, &m_verminor, &m_vermpack, &m_verbuild);
        }
    }
}

AVPUpgrade::AVPUpgrade(CRegKey& srcrootkey, CRegKey& dstrootkey, IUpgrTracer *tracer)
:   m_srcrootkey(srcrootkey),
    m_dstrootkey(dstrootkey),
    m_tracer(tracer),
    m_vermajor(0),
    m_verminor(0),
    m_vermpack(0),
    m_verbuild(0)
{
    setversion();
    upgrade_profile(std::wstring(), m_srcrootkey);
}

void upgradesettings_avp6_avp7(CRegKey& srckey, CRegKey& dstkey, IUpgrTracer* tracer)
{
    AVPUpgrade upgrade(srckey, dstkey, tracer);
}

#ifdef CONSOLETEST
#include "../common/cregreg.h"
#include <stdio.h>
#include <stdarg.h>

class CConsoleUpgrTracer : public IUpgrTracer
{
public:
    virtual ~CConsoleUpgrTracer () {};

    virtual void printf(const char * str...) 
    {
        va_list list;
        va_start(list, str);
        vprintf(str, list); 
        va_end(list);
        putchar('\n');
    }
};

void main()
{
    CRegRegRoot root;
    AVPUpgradeSettings::CRegKey avp6key = root.getkey(HKEY_LOCAL_MACHINE, L"Software\\KasperskyLab\\AVP6_Test");
    AVPUpgradeSettings::CRegKey avp7key = root.getkey(HKEY_LOCAL_MACHINE, L"Software\\KasperskyLab\\protected\\AVP7_Test");
    AVPUpgradeSettings::CRegKey avp8key = root.getkey(HKEY_LOCAL_MACHINE, L"Software\\KasperskyLab\\protected\\AVP8_Test");
    if (avp6key)
    {
        if (avp7key)
        {
            CConsoleUpgrTracer tracer;
            upgradesettings_avp6_avp7(avp6key, avp7key, &tracer);
        }
    }
}
#endif