#ifndef UPDATE_SOURCE_H_18496AB1_6ADF_4845_8896_5D145F637CE1
#define UPDATE_SOURCE_H_18496AB1_6ADF_4845_8896_5D145F637CE1

#if _MSC_VER > 1000
    #pragma once
#endif

#include <string>
#include <sstream>

/// the EUpdateStructure enumerates different structures of files on source for Updater
enum EUpdateStructure
{
    /// all files on source are structured into subfolders
    UPS_DIRS,
    /// all files on source are placed in single folder and not structured.
    /// Updater ignores structure specified into index and downloads all files from single folder
    UPS_PLAIN,

    /// forces plain structure usage if substitute primary index is used
    UPS_FORCE_PLAIN_FOR_SUBSTITUTE_INDEX,
};

inline bool isPlainUpdateStructure(const EUpdateStructure &updateStructure)
{
    return (updateStructure == UPS_PLAIN) || (updateStructure == UPS_FORCE_PLAIN_FOR_SUBSTITUTE_INDEX);
}

/// the EUpdateSourceType enumerates different source types
enum EUpdateSourceType
{
    UST_None = 0,
    /// update using AdminKit transport
    UST_AKServer,
    /// Update from Kaspersky Laboratory source. Kaspersky Laboratory server
    ///  addresses are loaded dynamically from configuration files
    UST_KLServer,
    /// updater from user specified source
    UST_UserURL,
    /// update using AdminKit transport from AdminKit Master server
    UST_AKMServer,

    UST_ENUM_SIZE
};

/// transforms EUpdateSourceType type to human-readable form
inline KLUPD::NoCaseString toString(const EUpdateSourceType &type)
{
    switch(type)
    {
    case UST_None:
        return L"no";
    case UST_AKServer:
        return L"AdminKit";
    case UST_KLServer:
        return L"Kaspersky Laboratory";
    case UST_UserURL:
        return L"user defined URL";
    case UST_AKMServer:
        return L"AdminKit Master";
    default:
        std::ostringstream stream;
        stream.imbue(std::locale::classic());
        stream << "undefined source type " << type;
        return KLUPD::asciiToWideChar(stream.str());
    }
}

#endif  // UPDATE_SOURCE_H_18496AB1_6ADF_4845_8896_5D145F637CE1
