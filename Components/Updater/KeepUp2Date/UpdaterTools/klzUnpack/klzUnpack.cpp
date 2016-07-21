#include <iostream>
#include <string>

#include "core/diffs.h"

void usage()
{
    std::cerr << "klzUnpack.exe tool packs/unpacks files with klz-algorithm" << std::endl
        << std::endl << "usage: klzUnpack.exe {-p|-u} original.in [destination]" << std::endl
        << "\t: {-p|-u} - pack/unpack" << std::endl
        << "\t: original.in - input file" << std::endl
        << "\t: destination - optional destination file name parameter. If ommited then '.packed/.unpacked' extention is prepended to original file" << std::endl;
}

int main(int argc, char *argv[])
{
    if(argc < 3)
    {
        usage();
        return -1;
    }

    bool compressMode = true;
    std::string operationString;
    if(std::string(argv[1]) == "-p")
    {
        compressMode = true;
        operationString = "pack";
    }
    else if(std::string(argv[1]) == "-u")
    {
        compressMode = false;
        operationString = "unpack";
    }
    else
    {
        std::cout << "invalid mode parameter '" << argv[1] << "' (should be either '-p' or '-u'" << std::endl;
        usage();
        return -1;
    }


    std::string destination;
    if(argc == 3)
    {
        destination = argv[2];
        destination += compressMode ? ".packed" : ".unpacked";
    }
    else
        destination = argv[3];

    if(compressMode)
    {
        if(packSingleKLZ(KLUPD::asciiToWideChar(argv[2]).c_str(),
            KLUPD::asciiToWideChar(destination).c_str(),
            0))
        {
            std::cout << argv[2] << " " << operationString << " succesfully to " << destination << std::endl;
            return 0;
        }
    }
    else
    {
        if(unpackSingleKLZ(KLUPD::asciiToWideChar(argv[2]).c_str(),
            KLUPD::asciiToWideChar(destination).c_str(),
            0))
        {
            std::cout << argv[2] << " " << operationString << " succesfully to " << destination << std::endl;
            return 0;
        }
    }

    std::cout << "failed to " << operationString << " file " << argv[2] << std::endl;
    return -1;
}
