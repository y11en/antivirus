#include "updater.h"
#include "diffs.h"

#include "diffs/md5.h"

bool KLUPD::Updater::makeDifferenceFileExtension(const Path &sourceFileFullPath, Path &differenceFileExtension)const
{
    std::vector<unsigned char> md5;
    if(!calcMD5Hash(sourceFileFullPath, md5))
        return false;

    // base64
    char ext[16];
    memset(ext, 0, sizeof(ext));
    size_t encoded = 0;

    Base64P().encodeBase64(&md5[0], 3, reinterpret_cast<unsigned char *>(ext), 12, &encoded, false);

    ext[3] = 0;
    if(ext[0] >= 'A' && ext[0] <= 'Z')
        ext[0] += 'a' - 'A';
    if(ext[0] == '+')
        ext[0] = '-';
    if(ext[0] == '/')
        ext[0] = '_';
    if(ext[1] >= 'A' && ext[1] <= 'Z')
        ext[1] += 'a' - 'A';
    if(ext[1] == '+')
        ext[1] = '-';
    if(ext[1] == '/')
        ext[1] = '_';
    if(ext[2] >= 'A' && ext[2] <= 'Z')
        ext[2] += 'a' - 'A';
    if(ext[2] == '+')
        ext[2] = '-';
    if(ext[2] == '/')
        ext[2] = '_';

    differenceFileExtension = asciiToWideChar(ext);
    return true;
}

void KLUPD::Updater::makeKlzCompressionFileName(const Path &relativeURLPath,
                                                const Path &baseFileName,
                                                Path &KLZPath,
                                                Path &KLZFile)const
{
    KLZPath = relativeURLPath;
    KLZFile = baseFileName + L".klz";
}
