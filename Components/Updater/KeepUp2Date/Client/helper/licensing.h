#ifndef LICENSING_H_INCLUDED_3C5900BE_EC0A_4ab0_8D7A_58775A0E1A1B
#define LICENSING_H_INCLUDED_3C5900BE_EC0A_4ab0_8D7A_58775A0E1A1B

#include "comdefs.h"
#include "path.h"

namespace KLUPD {

class CallbackInterface;
struct FileInfo;
struct FileVector;
class Log;

//////////////////////////////////////////////////////////////////////////
/// signature in format 5

/// check if file is inside signature 5 is valid
extern bool checkInsideSignature5(const Path &fullFileName);
/// check if buffer is inside signature 5 is valid
extern bool checkBufferSignature5(const std::vector<unsigned char> &);


/// validate file signature agains predefined one
extern bool checkInsideSignature5AgainstEtalon(const Path &fullFileName,
                                               const KLUPD::NoCaseString &signatureEtalonToCheckAgainst, Log *);

//////////////////////////////////////////////////////////////////////////
/// signature in format 6
class KAVUPDCORE_API Signature6Checker
{
public:
    Signature6Checker(Log *);
    ~Signature6Checker();

    // add data for DSMK library
    void addDataForSignature6Check(const std::vector<unsigned char> &);

    // checks file signature using current data passed with the addDataForDskm() function
    // param insideSignature [out] - return if file is signed with inside signature 6
    bool checkSignature(const Path &fullFileName, bool &insideSignature, NoCaseString &errorMessage)const;
    bool checkSignature(const std::vector<unsigned char> &buffer, const Path &fileNameForHash, bool &insideSignature, NoCaseString &errorMessage)const;


    // parses index file for registry
    void loadKlAndOperableKeysFromPrimaryIndex(const NoCaseString& strFilename, const std::vector<unsigned char> &fileBuffer, CallbackInterface &);

    // represents Dskm library error in human readable form
    static NoCaseString toStringDskmCode(const unsigned long code);

    void SetObjectType(const char* szObjectName, const char* szObjectType);

    // public, because sometimes trace is needed to be suppressed
    Log *pLog;

private:
    // disable copy operations
    Signature6Checker &operator=(const Signature6Checker &);
    Signature6Checker(const Signature6Checker &);

    class Implementation;
    Implementation *m_implementation;
};



// check if file has correct signature 6 or 5 signature, or md5
extern bool checkSignature5And6AndMd5(const Path &fullFileName, const FileInfo &etalon,
                                      Signature6Checker &, NoCaseString &signature6ErrorMessage);


// the enumeration is introduced for code readability
enum InsideSignatureCheckBehaviour
{
    // signature check fails in case inside-signature is used
    disableInsideSignature,
    // signature check does not fail in case inside-signature is used
    enableInsideSignature,
    // signature check does not fail in case inside-signature is used.
     // KL and Operable keys will be tried to be loaded from file
    enableInsideSignatureWithLoadKey,
};

extern bool checkSignature5And6AndMd5AgainstEtalon(const Path &fullFileName, const FileInfo &etalon,
                                                   Signature6Checker &, NoCaseString &signature6ErrorMessage,
                                                   const InsideSignatureCheckBehaviour &, CallbackInterface &, Log *);

extern bool checkFileMd5OrPrimaryIndexSignature5And6(const Path &fullFileName, const FileInfo &etalon,
                                                     Signature6Checker &, NoCaseString &signature6ErrorMessage, Log *);

// Check if black list presence and return black list date limit.
// blackListDateLimit [out] - black list limit. The date to check against 'BlackDate' tag in index files
extern bool KAVUPDCORE_API checkBlackListPresence(const FileVector &, unsigned long &blackListDateLimit, Log *);

}   // namespace KLUPD

#endif  // #ifndef LICENSING_H_INCLUDED_3C5900BE_EC0A_4ab0_8D7A_58775A0E1A1B
