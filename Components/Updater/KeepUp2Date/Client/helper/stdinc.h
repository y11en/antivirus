#if !defined(STDINC_H_INCLUDED_A943ADFF_92BC_4967_BCDD_91C389F3A680)
#define STDINC_H_INCLUDED_A943ADFF_92BC_4967_BCDD_91C389F3A680


#include "comdefs.h"


// =================================================================================================
#ifndef LAX_NO_IMPLEMENTATION
    #define LAX_NO_IMPLEMENTATION
#endif

#include <LAX.h>    // XML-parser from CommonFiles
    typedef LAX::UTF8_UTF8 <LAX::STD_STRING_ADAPTER <char, std::string> > XmlTransform;

typedef LAX::XmlUseExceptions<LAX::XmlException> XmlExceptionPolicy;
typedef LAX::XmlAttrMap <XmlTransform> XmlAttrMap;
typedef LAX::XmlReader <XmlTransform, LAX::STD_ATTR_MAP_ADAPTER<char, XmlAttrMap>, XmlExceptionPolicy> XmlReader;
typedef XmlReader::Element XmlElement;
// =================================================================================================


/// basic functionality
#include "map_wrapper.h"
#include "../include/stringParser.h"
#include "local_file.h"

#include "../core/diffs/md5.h"
#include "base64.h"

/// basic structures
#include "../include/updateSchedule.h"
#include "fileInfo.h"
#include "../include/update_source.h"

#include "../include/cfg_updater.h"    // includes cfg_updater2.h, sourceDescription.h

/// basic operations
#include "licensing.h"


/// interfaces
#include "../include/DownloadProgress.h"
#include "../include/callback_iface.h"
#include "../include/journal_iface.h"

namespace KLUPD {

    inline std::string toCString(const KLUPD::CoreError &code)
    {
        for(size_t index = 0; index < sizeof(g_errorCodeLocalizedDescriptions); ++index)
        {
            if(g_errorCodeLocalizedDescriptions[index].m_code == code)
                return g_errorCodeLocalizedDescriptions[index].m_description;
        }

        std::ostringstream stream;
        stream.imbue(std::locale::classic());
        stream << "unknown error code " << code;
        return stream.str();
    }

    inline NoCaseString toString(const KLUPD::CoreError &code)
    {
        return asciiToWideChar(toCString(code));
    }
}   // namespace KLUPD

#endif  // #if !defined(STDINC_H_INCLUDED_A943ADFF_92BC_4967_BCDD_91C389F3A680)
