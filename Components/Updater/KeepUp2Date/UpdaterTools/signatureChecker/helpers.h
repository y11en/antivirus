#ifndef HELPERS_H_INCLUDED_09D3BE56_B202_4cf9_9D3A_4A420AB13934
#define HELPERS_H_INCLUDED_09D3BE56_B202_4cf9_9D3A_4A420AB13934

#include <Sign/SIGN.h>

#define LIBS_STATIC_LINKING
#include <helper/local_file.h>

extern std::string toStringSignCheckResult(const int);

struct Configuration
{
    bool parse(int argc, char* argv[]);
    static void printUsage();

    std::string toString(const char *indent = "\n\t")const;

    bool useKasperskyPublicKey()const;

    KLUPD::Path m_fileToCheck;
    KLUPD::Path m_filePublicKey;
};

#endif   // HELPERS_H_INCLUDED_09D3BE56_B202_4cf9_9D3A_4A420AB13934
