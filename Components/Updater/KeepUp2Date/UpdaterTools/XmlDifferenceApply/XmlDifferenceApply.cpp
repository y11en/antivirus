#include <iostream>
#include <string>

#include "core/diffs.h"

int main(int argc, char *argv[])
{
    if(argc < 3)
    {
        std::cerr << "XmlDifferenceApply.exe tool applies XML-difference on XML file" << std::endl
            << std::endl << "usage: XmlDifferenceApply.exe source.xml difference.dif [result.xml]" << std::endl
            << "\tsource.xml - original XML file" << std::endl
            << "\tdifference.dif - difference file" << std::endl
            << "\tresult.xml - optional file name parameter to place result. If ommited then '.unpacked' extention is prepended to original file" << std::endl;
        return 0;
    }

    std::string destination;
    if(argc == 3)
    {
        destination = argv[1];
        destination += ".unpacked";
    }
    else
        destination = argv[3];

    if(applyDifferenceOnIndexFile(KLUPD::asciiToWideChar(argv[1]).c_str(),
        KLUPD::asciiToWideChar(argv[2]).c_str(),
        KLUPD::asciiToWideChar(destination).c_str(),
        0))
    {
        std::cout << argv[1] << " difference is applied succesfully, result " << destination << std::endl;
        return 0;
    }

    std::cout << "failed to apply difference on " << argv[1] << std::endl;
    return -1;
}
