#include "helpers.h"
#include "signatureChecker.h"


int main(int argc, char *argv[])
{
    Configuration configuration;
    if(configuration.parse(argc, argv))
        std::cout << "Configuration: " << configuration.toString() << std::endl;
    else
    {
        Configuration::printUsage();
        return -1;
    }

    int signCheckResult = -11;
    if(configuration.useKasperskyPublicKey())
        signCheckResult = sign_check_file(configuration.m_fileToCheck.toWideChar(), 1, 0, 0, 0);
    else
    {
        std::vector<unsigned char> publicKeyBuffer;
        if(!KLUPD::isSuccess(KLUPD::LocalFile(configuration.m_filePublicKey).read(publicKeyBuffer))
            || publicKeyBuffer.empty())
        {
            std::wcout << L"error: failed to read file '" << configuration.m_filePublicKey.toWideChar() << L"'" << std::endl;
            return -12;
        }

        void *publicKeys[1];
        publicKeys[0] = &publicKeyBuffer[0];
        signCheckResult = sign_check_file(configuration.m_fileToCheck.toWideChar(),
            0,
            publicKeys,
            1,
            0);
    }

    std::cout << "Signature check result " << toStringSignCheckResult(signCheckResult) << std::endl;

    return (signCheckResult == SIGN_OK)
        ? 0 : -13;
}
