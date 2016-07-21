#include "core/diffs/md5.h"
#include "helper/base64.h"


// Purpose: this utility calculates MD5 and outputs it in Base64 encoded.
//  This is required to check Update MD5 consistency

int main(int argc, char *argv[])
{
    if(argc != 2)
    {
        std::cout << "MD5 hash file calculator tool. This utility calculates file MD5 hash and outputs it as Base64 encoded string" << std::endl << std::endl
            << "Usage:" << std::endl
            << "  calculatorFileMD5.exe <file name>" << std::endl;
        return -1;
    }

    std::vector<unsigned char> md5;
    if(!calcMD5Hash(KLUPD::asciiToWideChar(argv[1]).c_str(), md5)
        || md5.empty())
    {
        std::cout << "MD5 hash calculation error for file " << argv[1] << std::endl;
        return -1;
    }


    std::vector<char> base64EncodedHash = CBase64::encodeBuffer(&md5[0], md5.size());
    if(base64EncodedHash.empty())
    {
        std::cout << "Failed to Base64 encode MD5 hash for file " << argv[1] << std::endl;
        return -2;
    }

    std::cout << &base64EncodedHash[0] << std::endl;
	return 0;
}
