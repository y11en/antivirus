#include "helpers.h"

std::string toStringSignCheckResult(const int resultCode)
{
    switch(resultCode)
    {
    case SIGN_OK:
        return "success";
    case SIGN_UNKNOWN_ERROR:
        return "unknown error";
    case SIGN_NOT_MATCH:
        return "signature does not match";
    case SIGN_BAD_OPEN_KEY:
        return "bad public key";
    case SIGN_NOT_ENOUGH_MEMORY:
        return "not enough memory";
    case SIGN_BAD_KEY_VERSION:
        return "bad key version";
    case SIGN_CANT_SIGN:
        return "cannot sign";
    case SIGN_BAD_PARAMETERS:
        return "bad parameter";
    case SIGN_CANT_READ:
        return "cannot read";
    case SIGN_CANT_WRITE:
        return "cannot write";
    case SIGN_ALREADY_SIGNED:
        return "already signed";
    case SIGN_BUFFER_HAS_NO_NOTARIES:
        return "buffer has no notaries";
    case SIGN_FILE_NOT_FOUND:
        return "file not found";
    case SIGN_BAD_FILE:
        return "bad file";
    case SIGN_BAD_PASSWORD:
        return "bad password";
    case SIGN_NOTARY_NOT_FOUND:
        return "notary not found";
    case SIGN_CALLBACK_FAILED:
        return "callback failed";
    case SIGN_BUFFER_TOO_SMALL:
        return "buffer is too small";
    default:
        break;
    }

    std::ostringstream stream;
    stream << "unknown result code " << resultCode;
    return stream.str();
}
bool Configuration::parse(int argc, char* argv[])
{
    if(argc < 2)
        return false;

    for(int i = 1; i < argc; ++i)
    {
        const std::string parameterType = argv[i];
        ++i;
        if(i == argc)
            return false;
        const std::string parameterValue = argv[i];

        if(parameterType == "-f")
            m_fileToCheck.fromAscii(parameterValue);
        else if(parameterType == "-p")
            m_filePublicKey.fromAscii(parameterValue);
    }
    return !m_fileToCheck.empty();
}

void Configuration::printUsage()
{
    std::cout << "Signature checker tool. This utility calculates allows to check file signature using public key" << std::endl << std::endl
        << "Usage:" << std::endl
        << "  signatureChecker -f <file to check> [-p <public key>]" << std::endl
        << "    -f - full path to signed file to check" << std::endl
        << "    -p - optional full path to public key, Kaspersky public key is used in case the parameter is ommited" << std::endl;
}

std::string Configuration::toString(const char *indent)const
{
    std::ostringstream stream;
    stream << indent << "file to check '" << m_fileToCheck.toAscii() << "'";
    if(useKasperskyPublicKey())
        stream << indent << "Kaspersky public key is used for check";
    else
        stream << indent << "public key '" << m_filePublicKey.toAscii() << "'";

    return stream.str();
}

bool Configuration::useKasperskyPublicKey()const
{
    return m_filePublicKey.empty();
}

