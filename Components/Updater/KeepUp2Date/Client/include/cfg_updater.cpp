#include "cfg_updater.h"

#include "../helper/licensing.h"

#include "../helper/base64.h"

#include "../helper/updaterStaff.h"

#include <boost/regex.hpp>

#include "ver_mod.h"


namespace {
    const KLUPD::NoCaseString g_authorizationPrefixBasic = L"Basic";
    const KLUPD::NoCaseString g_authorizationPrefixNTLM = L"NTLM";
}


KLUPD::NoCaseString KLUPD::toString(const AuthorizationType &type)
{
    switch(type)
    {
    case noAuthorization:
        return L"no authorization";
    case basicAuthorization:
        return L"Basic authorization";
    case ntlmAuthorization:
        return L"NTLM authorization";
    case ntlmAuthorizationWithCredentials:
        return L"NTLM authorization with credentials";

    default:
        {
            std::ostringstream stream;
            stream.imbue(std::locale::classic());
            stream << "unknown authorization type " << static_cast<const int>(type);
            return asciiToWideChar(stream.str());
        }
    }
}

KLUPD::AuthorizationType KLUPD::fromStringAuthorization(const NoCaseString &input)
{
    // check against NTLM
    if(g_authorizationPrefixNTLM.size() <= input.size())
    {
        const NoCaseString sizedRequest(input.toWideChar(), g_authorizationPrefixNTLM.size());
        if(sizedRequest == g_authorizationPrefixNTLM)
            return ntlmAuthorization;
    }

    // check against Basic
    if(g_authorizationPrefixBasic.size() <= input.size())
    {
        const NoCaseString sizedRequest(input.toWideChar(), g_authorizationPrefixBasic.size());
        if(sizedRequest == g_authorizationPrefixBasic)
            return basicAuthorization;
    }

    return noAuthorization;
}

extern bool KLUPD::credentialsRequired(const AuthorizationType &authorizationType)
{
    return (authorizationType == basicAuthorization)
        || (authorizationType == ntlmAuthorizationWithCredentials);
}


KLUPD::NoCaseString KLUPD::toString(const AuthorizationTypeList &authorizationTypeList)
{
    NoCaseString result;
    for(AuthorizationTypeList::const_iterator authorizationIter = authorizationTypeList.begin(); authorizationIter != authorizationTypeList.end(); ++authorizationIter)
    {
        result += toString(*authorizationIter);
        if(authorizationIter + 1 != authorizationTypeList.end())
            result += L", ";
    }
    return result;
}

KLUPD::NoCaseString KLUPD::toString(const KLUPD::Protocol &protocol)
{
    switch(protocol)
    {
    case ftpTransport:
        return L"FTP";
    case httpTransport:
        return L"HTTP";
    case administrationKitTransport:
        return L"Adminitration Kit";
    case fileTransport:
        return L"File transfer protocol";
    default:
        {
            std::ostringstream stream;
            stream.imbue(std::locale::classic());
            stream << "unknown protocol " << protocol;
            return asciiToWideChar(stream.str());
        }
    }
}

KLUPD::Path KLUPD::toProtocolPrefix(const KLUPD::Protocol &protocol)
{
    switch(protocol)
    {
    case ftpTransport:
        return L"ftp://";
    case httpTransport:
        return L"http://";
    default:
        return Path();
    }
}

////////////////////////////////////////////////////////////////////////////////
/// ComponentIdentefiers

KLUPD::NoCaseString KAVUPDCORE_API KLUPD::toString(const ComponentIdentefiers &componentIdentefiers)
{
    NoCaseString result;
    for(ComponentIdentefiers::const_iterator componentIter = componentIdentefiers.begin(); componentIter != componentIdentefiers.end(); ++componentIter)
    {
         result += *componentIter;
         if(componentIter + 1 != componentIdentefiers.end())
             result += L",";
    }
    return result;
}

KLUPD::ComponentIdentefiers KAVUPDCORE_API KLUPD::fromString(const NoCaseString &input)
{
    return KLUPD::StringParser::splitString(input, COMPONENT_DELIMITER);
}


////////////////////////////////////////////////////////////////////////////////
/// UpdatedSettings
KLUPD::NoCaseString KLUPD::UpdatedSettings::toString()const
{
    NoCaseString result = NoCaseString(L"name '") + m_name + L"'";
    result += NoCaseString(L", type '") + m_type + L"'";
    result += NoCaseString(L", value '") + m_value + L"'";
    return result;
}


KLUPD::NoCaseString KLUPD::UpdaterConfiguration::OS::toString()const
{
    std::ostringstream stream;
    stream.imbue(std::locale::classic());
    stream << "OS " << os.toAscii() << " " << version.toAscii();
    return asciiToWideChar(stream.str());
}

//////////////////////////////////////////////////////////////////////////
/// UpdaterConfiguration::SRetranslatedObjects

KLUPD::UpdaterConfiguration::SRetranslatedObjects::SRetranslatedObjects(const bool componentsWhiteListMode, const bool applicationsWhiteListMode)
 : Components_WhiteListMode(componentsWhiteListMode),
   Applications_WhiteListMode(applicationsWhiteListMode)
{
}

KLUPD::NoCaseString KLUPD::UpdaterConfiguration::SRetranslatedObjects::toString(const char *indent)const
{
    std::ostringstream stream;
    stream.imbue(std::locale::classic());
    stream << indent << "component " << (Components_WhiteListMode ? "while " : "black ") << "list: "
        << KLUPD::toString(m_componentsToRetranslate).toAscii() << std::endl;

    stream << indent << "application " << (Applications_WhiteListMode ? "while " : "black ") << "list: "
        << Applications.toString().toAscii();

    return asciiToWideChar(stream.str());
}

//////////////////////////////////////////////////////////////////////////
/// UpdaterConfiguration::ListFlags
KLUPD::NoCaseString KLUPD::UpdaterConfiguration::ListFlags::toString()const
{
    std::ostringstream stream;
    stream.imbue(std::locale::classic());
    if(empty())
        stream << "no object to update";
    else
    {
        stream << (update_bases ? "update bases enabled" : "");

        if(automatically_apply_available_updates)
            stream << ", update patches enabled";
        else if(apply_urgent_updates)
            stream << ", update only urgent patches";
    }
    return asciiToWideChar(stream.str());
}

bool KLUPD::UpdaterConfiguration::ListFlags::empty()const
{
    return !update_bases && !apply_urgent_updates && !automatically_apply_available_updates;
}


//////////////////////////////////////////////////////////////////////////
/// UpdaterConfiguration

const size_t KLUPD::UpdaterConfiguration::s_defaultTimeout = 30;

KLUPD::UpdaterConfiguration::UpdaterConfiguration(const size_t applicationIdentifier,
                                                  const size_t installationIdentifier,
                                                  const NoCaseString &buildNumberIdentifier)
 : self_region(SS_KEY_UseDefaultRegion),
   m_bypassProxyServerForLocalAddresses(false),
   proxy_port(0),
   use_ie_proxy(false),
   updates_retranslation(false),
   retranslate_only_listed_apps(false),
   m_applicationIdentifier(applicationIdentifier),
   m_buildNumberIdentifier(buildNumberIdentifier),
   m_installationIdentifier(installationIdentifier),
   passive_ftp(true),
   m_tryActiveFtpIfPassiveFails(false),
   connect_tmo(s_defaultTimeout)
{
    srand(static_cast<unsigned int>(time(0)));
    m_sessionIdentifier = rand();
}

bool KLUPD::UpdaterConfiguration::updateRequired()const
{
    return !UpdaterListFlags.empty();
}
bool KLUPD::UpdaterConfiguration::retranslationRequired()const
{
    return updates_retranslation && !RetranslationListFlags.empty();
}


KLUPD::NoCaseString KLUPD::UpdaterConfiguration::toString(const char *indent)const
{
    std::ostringstream stream;
    stream.imbue(std::locale::classic());

#ifdef WIN32
	char buffer[256];
	DWORD size = 250;
	memset(buffer, 0, 256);
	if(GetUserNameA(buffer, &size))
		stream << indent << "current user: " << buffer << std::endl;
	else
    {
        const int lastError = GetLastError();
		stream << indent << "unable to determine current user, error "
            << errnoToString(lastError, windowsStyle).toAscii() << std::endl;
    }
#endif

    stream << indent << "local region: " << self_region.toAscii() << std::endl
        << indent << "OS: " << os.toString().toAscii() << std::endl;

    if(!product_lang.empty())
    {
        stream << indent << "language filter(s):";
        for(std::vector<NoCaseString>::const_iterator languageIter = product_lang.begin(); languageIter != product_lang.end(); ++languageIter)
            stream << " '" << languageIter->toAscii() << "'";
        stream << std::endl;
    }


    stream << indent << "Update objects: " << UpdaterListFlags.toString().toAscii() << std::endl;
    if(!m_componentsToUpdate.empty())
    {
        stream << indent << "Update white list component filter: "
            << KLUPD::toString(m_componentsToUpdate).toAscii() << std::endl;
    }

    if(retranslationRequired())
    {
        stream << indent << "Retranslation objects: " << RetranslationListFlags.toString().toAscii() << std::endl;
        if(retranslate_only_listed_apps)
            stream << indent << "Retranslation filters: " << std::endl << RetranslatedObjects.toString("\t\t").toAscii() << std::endl;
        else
            stream << indent << "Retranslate only listed applications mode disabled" << std::endl;
    }
    else
	    stream << indent << "Retranslation is not required" << std::endl;


    stream << indent << "retranslation folder: " << retranslation_dir.toAscii() << std::endl
        << indent << "temporary folder: " << temporary_dir.toAscii() << std::endl;


    #ifdef DISABLE_AK_FILETRANSFER
        stream << indent << "Administration Kit transport is disabled" << std::endl;
    #endif
    #ifdef KLUPD_ENABLE_SIGNATURE_5
        stream << indent << "Signature 5th support is enabled" << std::endl;
    #endif

    stream << indent << "network timeout: " << connect_tmo << std::endl
        << indent << "FTP works in " << (passive_ftp ? "passive mode " : "active mode") << std::endl;
    if(passive_ftp && m_tryActiveFtpIfPassiveFails)
        stream << indent << indent << "FTP active mode will be tried in case passive mode fails" << std::endl;

    if(m_bypassProxyServerForLocalAddresses)
    	stream << indent << "bypass proxy server for local addresses mode" << std::endl;

    if(use_ie_proxy)
    	stream << indent << "obtain proxy server settings automatically" << std::endl;
    else
        stream << indent << "proxy server address: " << proxy_url.toAscii() << ":" << proxy_port << std::endl;

    if(!m_proxyAuthorizationCredentials.userName().empty())
        stream << indent << "proxy user name for authorization: " << m_proxyAuthorizationCredentials.userName().toAscii() << std::endl;
    stream << indent << "authorization methods: " << KLUPD::toString(m_httpAuthorizationMethods).toAscii() << std::endl;


    stream << indent << "Update build number filter: '" << pID.toAscii() << "'" << std::endl
        << indent << "version: '" << pVer.toAscii() << "'" << std::endl;

    stream << indent << "  ** Data for statistics (not used for filtering) **" << std::endl
        << indent << indent << "application: " << m_applicationIdentifier << std::endl
        << indent << indent << "build identifier: " << m_buildNumberIdentifier.toAscii() << std::endl
        << indent << indent << "installation identifier: " << m_installationIdentifier << std::endl
        << indent << indent << "session identifier: " << m_sessionIdentifier << std::endl
        << indent << indent << "serial number: " << m_serialNumber.toAscii() << std::endl
    // Note: the last end-line is absent
        << indent << indent << "updater identity string: " << composeIdentityInfoString().toAscii();

    return asciiToWideChar(stream.str());
}

KLUPD::NoCaseString KLUPD::UpdaterConfiguration::composeIdentityInfoString()const
{
    const NoCaseString defaultResult = NoCaseString(L"Updater") + asciiToWideChar(UPD_VER_MAIN).c_str();

    // some field with unclear meaning, not used any more
    const size_t diffFormatsSupported = 0;

    long Serial1 = 0;
    long Serial2 = 0;
    long Serial3 = 0;
    convertSerialToLongs(Serial1, Serial2, Serial3);

    const int buf_len = 512;
    unsigned char buf[buf_len + 1];
    memset(buf, 0, sizeof(buf));

    size_t filled = 0;
    size_t bytes_written = 0;

    Base64P base64p;
    const size_t applicationID = m_applicationIdentifier & 0xFFFFFFFF;
    if(!base64p.encodeBase64PackedLongWithUpdaterAlphabet(applicationID, buf + filled, buf_len - filled, &bytes_written))
        return defaultResult;
    filled += bytes_written;
    bytes_written = 0;

    if(!base64p.encodeBase64PackedLongWithUpdaterAlphabet(Serial1, buf + filled, buf_len - filled, &bytes_written))
        return defaultResult;
    filled += bytes_written;
    bytes_written = 0;

    if(!base64p.encodeBase64PackedLongWithUpdaterAlphabet(Serial2, buf + filled, buf_len - filled, &bytes_written))
        return defaultResult;
    filled += bytes_written;
    bytes_written = 0;

    if(!base64p.encodeBase64PackedLongWithUpdaterAlphabet(Serial3, buf + filled, buf_len - filled, &bytes_written))
        return defaultResult;
    filled += bytes_written;
    bytes_written = 0;

    const size_t installationID = m_installationIdentifier & 0x3FFFF;
    if(!base64p.encodeBase64PackedLongWithUpdaterAlphabet(installationID, buf + filled, buf_len - filled, &bytes_written))
        return defaultResult;
    filled += bytes_written;
    bytes_written = 0;

    const size_t sessionID = m_sessionIdentifier & 0x3F;
    if(!base64p.encodeBase64PackedLongWithUpdaterAlphabet(sessionID, buf + filled, buf_len - filled, &bytes_written))
        return defaultResult;
    filled += bytes_written;
    bytes_written = 0;

    const size_t diffFormats = diffFormatsSupported & 0xFFF;
    if(!base64p.encodeBase64PackedLongWithUpdaterAlphabet(diffFormats, buf + filled, buf_len - filled, &bytes_written))
        return defaultResult;
    filled += bytes_written;
    bytes_written = 0;

    filled += encodeStringForIdentity(buf + filled, buf_len - filled, m_buildNumberIdentifier);
    
    // Note: if more fields are added, then you must implement check if addVariableSizeBuffer()
     // returns 0, because server expects fixed format, and field can not be skipped

    return asciiToWideChar(reinterpret_cast<const char *>(buf)).c_str();
}

const KLUPD::UpdaterConfiguration::ListFlags &KLUPD::UpdaterConfiguration::filterFlags(const bool retranslationMode)const
{
    return retranslationMode ? RetranslationListFlags : UpdaterListFlags;
}
const KLUPD::ComponentIdentefiers &KLUPD::UpdaterConfiguration::filterComponents(const bool retranslationMode)const
{
    return retranslationMode ? RetranslatedObjects.m_componentsToRetranslate : m_componentsToUpdate;
}

size_t KLUPD::UpdaterConfiguration::encodeStringForIdentity(unsigned char *outBuffer, const size_t outSizeIn, const NoCaseString &stringForIdentity)
{
    if(!outBuffer || !outSizeIn)
        return 0;

    size_t sizeRest = outSizeIn;

    const std::vector<char> buffer = CBase64::encodeBuffer(
        reinterpret_cast<const unsigned char *>(stringForIdentity.toAscii().c_str()), stringForIdentity.size());

    size_t sizeWrittenSize = 0;
    if(!Base64P().encodeBase64PackedLongWithUpdaterAlphabet(buffer.size(), outBuffer, sizeRest, &sizeWrittenSize))
    {
        *outBuffer = 0;
        return 0;
    }
    sizeRest -= sizeWrittenSize;

    // find minimum
    const size_t sizeToCopy = (sizeRest < buffer.size()) ? sizeRest : buffer.size();
    if(!sizeToCopy)
    {
        *outBuffer = 0;
        return 0;
    }

    memcpy(outBuffer + sizeWrittenSize, &buffer[0], sizeToCopy);
    return sizeWrittenSize + sizeToCopy;
}

void KLUPD::UpdaterConfiguration::convertSerialToLongs(long &ser1, long &ser2, long &ser3)const
{
    // example: 0038-00006d-0008e316
    ser1 = ser2 = ser3 = 0;
    if(m_serialNumber.size() < 20)
        return;

    const NoCaseString tmp1 = m_serialNumber.substr(0, 4);
    ser1 = tmp1.hexStringToLong();

    const NoCaseString tmp2 = m_serialNumber.substr(5, 6);
    ser2 = tmp2.hexStringToLong();

    const NoCaseString tmp3 = m_serialNumber.substr(12, 8);
    ser3 = tmp3.hexStringToLong();
}

//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////

KLUPD::Credentials::Credentials(const KLUPD::NoCaseString &userNameIn, const KLUPD::NoCaseString &passwordIn):
	m_cancelled( false )
{
    userName(userNameIn);
    password(passwordIn);
}

KLUPD::NoCaseString KLUPD::Credentials::toString()const
{
    // Warning: password should not be shown to user or output to trace
    return m_userName;
}

void KLUPD::Credentials::clear()
{
    m_userName.erase();
    m_password.erase();
}

bool KLUPD::Credentials::empty()const
{
    return m_userName.empty();
}

///
/// Mark credentials as canceled
///
void KLUPD::Credentials::cancel( const bool canceled )
{
	m_cancelled = canceled;
	if ( m_cancelled )
	{
		clear();
	}
}

///
/// Check if credentials are marked as canceled
///
bool KLUPD::Credentials::cancelled() const
{
	return m_cancelled;
}

void KLUPD::Credentials::userName(const NoCaseString &userNameIn)
{
    // check if "user@domain" format is used
    const size_t atSymbolOffset = userNameIn.find(L"@");
    if(atSymbolOffset == NoCaseStringImplementation::npos)
        m_userName = userNameIn;
    else
    {
        m_userName = userNameIn.substr(atSymbolOffset + 1);
        m_userName += '\\';
        m_userName += userNameIn.substr(0, atSymbolOffset);
    }
}

KLUPD::NoCaseString KLUPD::Credentials::userName()const
{
    return m_userName;
}
void KLUPD::Credentials::password(const NoCaseString &password)
{
    m_password = password;
}
KLUPD::NoCaseString KLUPD::Credentials::password()const
{
    return m_password;
}

//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////

KLUPD::Address::Address(const Path &urlToParse)
{
    parse(urlToParse);
}

bool KLUPD::Address::parse(const Path &urlToParse)
{
    clear();

    // URI parsing perl-type regular expression:
    boost::regex regex("^(([^:/?#]+):)?(//([^/?#]*))?(([^?#]*)(\\?([^#]*))?)(#(.*))?$");
    enum    // these numbers are hardly related to the expression used
    {
        PROTO = 2,
        HOST = 4,
        PATH_REQUEST = 5,
        PATH = 6,
        REQUEST = 8,
        BOOKMARK = 10,
        TOTAL_MATCHES = 11
    };
    boost::smatch match_res;

	const std::string str = urlToParse.toAscii();
    if( !boost::regex_match(str, match_res, regex) )
        return false;

    if( match_res.size() != TOTAL_MATCHES ) // error: invalid expression
        return false;                       // TODO: place assert here

    if( !setProtocol( NoCaseString().fromAscii(std::string(match_res[PROTO].first, match_res[PROTO].second)) ) )
    {
        m_path = urlToParse;
        return true;
    }

    if( match_res[HOST].matched &&
            !parseHostGroup( NoCaseString().fromAscii(std::string(match_res[HOST].first, match_res[HOST].second)) ) )
        return false;

    if( match_res[PATH_REQUEST].matched &&
            !parsePath( NoCaseString().fromAscii(std::string(match_res[PATH_REQUEST].first, match_res[PATH_REQUEST].second)) ) )
        return false;

    return true;
}

KLUPD::NoCaseString KLUPD::Address::toString()const
{
    return BuildURI();
}

void KLUPD::Address::clear()
{
    m_protocol = fileTransport;
    m_credentials.clear();
    m_hostname.erase();
    m_service.erase();
    m_path = L"/";
    m_fileName.erase();
}

bool KLUPD::Address::operator==(const Address &left)const
{
    return m_protocol == left.m_protocol &&
        m_credentials.userName() == left.m_credentials.userName() &&
        m_credentials.password() == left.m_credentials.password() &&
        m_hostname == left.m_hostname &&
        m_service == left.m_service &&
        m_path == left.m_path &&
        m_fileName == left.m_fileName;
}
bool KLUPD::Address::operator!=(const Address &left)const
{
	return !operator==(left);
}
bool KLUPD::Address::empty()const
{
    return m_hostname.empty() && (m_path.empty() || m_path == L"/") && m_fileName.empty();
}

bool KLUPD::Address::setProtocol(const NoCaseString &strProto)
{
    // TODO: implement administration Kit protocol support

    if(strProto == L"ftp")
    {
        m_protocol = ftpTransport;
        m_service = L"21";
    }
    else if(strProto == L"http")
    {
        m_protocol = httpTransport;
        m_service = L"80";
    }
    else
    {
        // by default it is file transport protocol
        m_protocol = fileTransport;
        return false;
    }

    return true;
}

bool KLUPD::Address::parsePath(const NoCaseString &strPath)
{
    // parsing perl-type regular expression:
    boost::regex regex("^(/([^/]+/)*)((.*?)(\\.([^.]*))?)?$");
    enum    // these numbers are hardly related to the expression used
    {
        PATH            = 1,    // folder list
        FILENAME        = 3,
        FILENAME_SHORT  = 4,    // file name without extension
        EXT             = 6,    // extension
        TOTAL_MATCHES   = 7
    };
    boost::smatch match_res;

	const std::string str = strPath.toAscii();
	if( !boost::regex_match(str, match_res, regex) )
       return false;

    if( match_res.size() != TOTAL_MATCHES ) // error: invalid expression
        return false;                       // TODO: place assert here

    // extract path
    if( match_res[PATH].matched )
        m_path.m_value.fromAscii(std::string(match_res[PATH].first, match_res[PATH].second));

    // extract filename
    if( match_res[FILENAME].matched )
        m_fileName.m_value.fromAscii(std::string(match_res[FILENAME].first, match_res[FILENAME].second));

    return true;
}

bool KLUPD::Address::parseHostGroup(const NoCaseString &strHost)
{
    // parsing perl-type regular expression:
    boost::regex regex("^(([^:@]*)(:([^@]*))?@)?((\\[([^\\]]*)\\])|([^:]+))(:(.*))?$");
    enum    // these numbers are hardly related to the expression used
    {
        USERNAME    = 2,
        PASSWORD    = 4,
        HOSTNAME    = 5,    
//        HOSTNAME1   = 6,    // braced host name
//        HOSTNAME2   = 8,    // simple host name
        SERVICE     = 10,   // service name or port number
        TOTAL_MATCHES = 11
    };
    boost::smatch match_res;

	const std::string str = strHost.toAscii();
    if( !boost::regex_match(str, match_res, regex) )
        return false;

    if( match_res.size() != TOTAL_MATCHES ) // error: invalid expression
        return false;                       // TODO: place assert here

    // extract hostname
    if( match_res[HOSTNAME].matched )
        m_hostname.m_value.fromAscii(std::string(match_res[HOSTNAME].first, match_res[HOSTNAME].second));
    else
        return false;                       // TODO: place assert here

    // extract service name
    if( match_res[SERVICE].matched )
        m_service.m_value.fromAscii(std::string(match_res[SERVICE].first, match_res[SERVICE].second));

    // extract username and password
    if( match_res[USERNAME].matched )
    {
        m_credentials.userName( NoCaseString().fromAscii(std::string(match_res[USERNAME].first, match_res[USERNAME].second)) );
        if( match_res[PASSWORD].matched )
            m_credentials.password( NoCaseString().fromAscii(std::string(match_res[PASSWORD].first, match_res[PASSWORD].second)) );
    }

    return true;
}

KLUPD::NoCaseString& KLUPD::Address::BuildURI(NoCaseString& str) const
{
    switch(m_protocol)
    {
    case ftpTransport: str += L"ftp:"; break;
    case httpTransport: str += L"http:"; break;
    }
    if( !m_hostname.empty() )
    {
        str += L"//";
        if( !m_credentials.empty() )
        {
            str += m_credentials.toString();
            str += L"@";
        }
        str += m_hostname.m_value;

        if( !m_service.empty() )
        {
            str += L":";
            str += m_service.m_value;
        }
    }
    str += m_path.m_value;
    str += m_fileName.m_value;

    return str;
}
