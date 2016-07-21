#include <iostream>
#include <string>

#include "helper/base64.h"

enum OutputType
{
    hex,
    dec,
    suppressOutput
};

bool decodeSingleBase64pNumber(const std::string &decoded, unsigned long &currentOffset, const char *message, unsigned long &resultNumber, const OutputType &outputType)
{
    size_t bytesRead = 0;
    if(!Base64P().decodeBase64PackedLongWithUpdaterAlphabet(&resultNumber, reinterpret_cast<const unsigned char *>(decoded.c_str()) + currentOffset,
        decoded.size() - currentOffset, &bytesRead))
    {
        std::cout << "Failed to decode " << message << std::endl;
        return false;
    }
    currentOffset += static_cast<unsigned long>(bytesRead);
    if(outputType == hex)
        std::cout << message << ": " << std::hex << resultNumber << std::dec << std::endl;
    else if(outputType == dec)
        std::cout << message << ": " << resultNumber << std::endl;
    return true;
}

int main(int argc, char *argv[])
{
    // indent
    std::cout << std::endl;

    bool outputSerialInHex = false;
    std::string encoded;
    if(argc == 2)
        encoded = argv[1];
    else if(argc == 3)
    {
        encoded = argv[2];
        if(std::string(argv[1]) == "-hexSerial")
            outputSerialInHex = true;
    }
    else
    {
        std::cerr << "UpdaterIdentityDecoder.exe tool extracts from decoded string updater identity" << std::endl
            << std::endl << "usage: XmlDifferenceApply.exe [-hexSerial] <encodedString>" << std::endl
            << "\t[-hexSerial] - optional parameter to output serial number in hexadecimal base format" << std::endl
            << "\t<encodedString> - string to decode, obtainded from updater" << std::endl;
        return 0;
    }

    // remove '@' symbol which is placed for FTP protocol
    if(encoded[encoded.size() - 1] == '@')
        encoded.resize(encoded.size() - 1);

    unsigned long currentOffset = 0;
    unsigned long resultNumber = 0;

    if(!decodeSingleBase64pNumber(encoded, currentOffset, "Application Identifier", resultNumber, dec))
        return -1;

    {
        unsigned long serial1 = 0;
        if(!decodeSingleBase64pNumber(encoded, currentOffset, "Serial 1", serial1, suppressOutput))
            return -1;
        unsigned long serial2 = 0;
        if(!decodeSingleBase64pNumber(encoded, currentOffset, "Serial 2", serial2, suppressOutput))
            return -1;
        unsigned long serial3 = 0;
        if(!decodeSingleBase64pNumber(encoded, currentOffset, "Serial 3", serial3, suppressOutput))
            return -1;

        std::ostringstream serialStream;
        serialStream.imbue(std::locale::classic());
        serialStream.fill('0');
        if(outputSerialInHex)
            serialStream << "(hexadecimal base) " << std::hex;
        else
            serialStream << "(decimal base) ";

        serialStream << std::setw(4) << serial1 << "-" << std::setw(6) << serial2 << "-" << std::setw(8) << serial3;
        std::cout << "Serial number: " << serialStream.str() << std::endl;
    }

    if(!decodeSingleBase64pNumber(encoded, currentOffset, "Installation Identifier", resultNumber, dec))
        return -1;
    if(!decodeSingleBase64pNumber(encoded, currentOffset, "Session Identifier", resultNumber, dec))
        return -1;
    if(!decodeSingleBase64pNumber(encoded, currentOffset, "Difference Support", resultNumber, dec))
        return -1;


    unsigned long buildIdentifierEncodedSize = 0;
    const unsigned long saveCurrentOffsetAgainstBug = currentOffset;
    if(!decodeSingleBase64pNumber(encoded, currentOffset, "\tSize of Build Identifier base64-encoded string", buildIdentifierEncodedSize, dec))
        return -1;

    // bug in 620 build (size field is missed)
    if((buildIdentifierEncodedSize + currentOffset - 1) == (encoded.size() + 1))
    {
        std::cout << "\t(Warning) try use algorithm for 6.0.2.620 build" << std::endl;
        currentOffset = saveCurrentOffsetAgainstBug;
    }

    std::vector<unsigned char> buildNumber = CBase64::decodeBuffer(encoded.c_str() + currentOffset);
    buildNumber.push_back(0);
    std::cout << "Build: " << reinterpret_cast<const char *>(&buildNumber[0]) << std::endl;
    return 0;
}
