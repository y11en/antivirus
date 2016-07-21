#include "PragueHelpers.h"

#include <ProductCore/plugin/p_cryptohelper.h>
#include <Prague/pr_serializable.h>
#include <Prague/iface/e_ktime.h>

#include <ProductCore/iface/i_licensing.h>  // error code definitions
#include <AV/iface/i_avs.h>        // error code definitions


#include "../../KeepUp2Date/Client/PragueWrapper/UpdaterConfigurator/i_updaterconfigurator.h"
#include "PragueLog.h"


tERROR PRAGUE_HELPERS::translateUpdaterResultCodeToPrague(const KLUPD::CoreError &code)
{
    const tERROR offset = KLUPD::isSuccessOrWarningForPrague(code)
        ? PR_MAKE_DECL_WARN(IID_UPDATERCONFIGURATOR, 0) // offset is: 0x05DC0000
        : PR_MAKE_DECL_ERR(IID_UPDATERCONFIGURATOR, 0); // offset is: 0x85DC0000

    return code + offset;
}

KLUPD::NoCaseString PRAGUE_HELPERS::cryptPassword(const CryptType &cryptType,
    cTaskManager *taskManager, const cStringObj &password, std::string &resultExplanation)
{
    // decrypt proxy server password
    if(password.empty())
        return KLUPD::NoCaseString();

    cCryptoHelper *cryptoHelper = 0;
    const tERROR getProxyPasswordResult = taskManager->GetService(0, 0, CRYPTOHELPER_SERVICE_NAME, (hOBJECT*)&cryptoHelper);
    if(!cryptoHelper)
    {
        resultExplanation = std::string("Failed crypt password, because crypto helper service is zero, result ")
            + PRAGUE_HELPERS::toStringPragueResult(getProxyPasswordResult).c_str();
        return KLUPD::NoCaseString();
    }
	if(PR_FAIL(getProxyPasswordResult))
    {
    	taskManager->ReleaseService(0, (hOBJECT)cryptoHelper);
        resultExplanation = std::string("Failed to crypt password, result ") + PRAGUE_HELPERS::toStringPragueResult(getProxyPasswordResult).c_str();
        return KLUPD::NoCaseString();
    }

	const tDWORD flags = (cryptType == encrypt) ? 0 : PSWD_DECRYPT;

    cStrObj decrypted = password;
	cSerString tmpString(decrypted);
    const tERROR decryptPasswordResult = cryptoHelper->CryptPassword(&tmpString, flags);
	taskManager->ReleaseService(0, (hOBJECT)cryptoHelper);

	if(PR_FAIL(decryptPasswordResult))
    {
        resultExplanation = std::string("Failed to crypt password, result '") + PRAGUE_HELPERS::toStringPragueResult(decryptPasswordResult) + "'";
        return KLUPD::NoCaseString();
    }

    return tmpString.m_str.data();
}

size_t PRAGUE_HELPERS::networkTimeoutSeconds(PragueEnvironmentWrapper &pragueEnvironmentWrapper, const size_t defaultTimeout)
{
    size_t result = 0;

    // user-specified network timeout. Read from registry, because in some specific
     // configuration it may be needed to adjust timeout and there is no GUI settings
    KLUPD::NoCaseString userSpecifiedNetworkTimeout;
    if(pragueEnvironmentWrapper.expandEnvironmentString(SS_KEY_connect_tmo, userSpecifiedNetworkTimeout, KLUPD::StringParser::byProductFirst)
        && !userSpecifiedNetworkTimeout.empty())
    {
        userSpecifiedNetworkTimeout.toLong(result);
    }

    return result ? result : defaultTimeout;
}

std::string PRAGUE_HELPERS::toStringPragueResult(const tERROR &code)
{
    switch(code)
    {
    case errOK:
        return "success";

    case errUNEXPECTED:
        return "unexpected error";

    case errNOT_ENOUGH_MEMORY:
        return "not enough memory";

    case errNOT_SUPPORTED:
        return "not supported";

    case errNOT_IMPLEMENTED:
        return "not implemented";

    case errPARAMETER_INVALID:
        return "incorrect parameter";

    case errOBJECT_NOT_FOUND:
        return "object is not found";

    case errINTERFACE_NOT_FOUND:
        return "interface is not found";

    case errBLACKLIST_CORRUPTED:
        return "black list is corrupted";

    case errKEY_IS_BLACKLISTED:
        return "key is blacklisted";

    case errNO_LICENCE:
        return "no licence or licence date check failed";

    case errAVS_BASES_CONFIG_INVALID:
        return "failed to load bases, bases are corrupted";

    case errOPERATION_CANCELED:
        return "operation canceled";

    case errBUFFER_TOO_SMALL:
        return "buffer is too small";

    default:
        {
            std::ostringstream stream;
            stream.imbue(std::locale::classic());
            stream << "unrecognized result 0x" << std::hex << code;
            return stream.str();
        }
    }
}

std::string PRAGUE_HELPERS::toString(const cProxySettings &proxySettings)
{
	if(!proxySettings.m_bUseProxy)
        return "proxy server disabled";

    std::ostringstream stream;
    stream.imbue(std::locale::classic());

    if(!!proxySettings.m_bUseIEProxySettings)
        stream << "Automatically detect proxy server address";
    else
    {
        stream << static_cast<const char *>(proxySettings.m_strProxyHost.c_str(cCP_ANSI))
            << ":" << proxySettings.m_nProxyPort;
    }

    if(!!proxySettings.m_bypassProxyServerForLocalAddresses)
        stream << ", bypass proxy server for local addresses";

    if(!proxySettings.m_strProxyLogin.empty())
        stream << ", user name '" << static_cast<const char *>(proxySettings.m_strProxyLogin.c_str(cCP_ANSI)) << "'";

	if(!!proxySettings.m_bProxyAuth)
        stream << ", use credentials for authorization";

    return stream.str();
}

bool PRAGUE_HELPERS::checkDigits(const char *s, const size_t size)
{
    return std::find_if(s, s + size, std::not1(std::ptr_fun(isdigit))) == s + size;
}
bool PRAGUE_HELPERS::convertStringToDatetime(const KLUPD::NoCaseString &dateIn, tDATETIME &dt)
{
    if(dateIn.size() < 13)
        return false;

    if(!checkDigits(dateIn.toAscii().c_str(), 8))
        return false;

    if(*(dateIn.toAscii().c_str() + 8) != ' ')
        return false;
    if(!checkDigits(dateIn.toAscii().c_str() + 9, 4))
        return false;

    size_t day = 0;
    KLUPD::NoCaseString(dateIn.toWideChar(), 2).toLong(day);
    size_t mon = 0;
    KLUPD::NoCaseString(dateIn.toWideChar() + 2, 2).toLong(mon);
    size_t year = 0;
    KLUPD::NoCaseString(dateIn.toWideChar() + 4, 4).toLong(year);
    size_t hr = 0;
    KLUPD::NoCaseString(dateIn.toWideChar() + 9, 2).toLong(hr);
    size_t mn = 0;
    KLUPD::NoCaseString(dateIn.toWideChar() + 11, 2).toLong(mn);

    return PR_SUCC(DTSet(&dt,
        static_cast<const tDWORD>(year),
        static_cast<const tDWORD>(mon),
        static_cast<const tDWORD>(day),
        static_cast<const tDWORD>(hr),
        static_cast<const tDWORD>(mn),
        0, 0));
}

