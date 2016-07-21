#ifndef PRAGUEHELPERS_H_INCLUDED_BA908DB8_0651_4be6_BAF6_20090E64B086
#define PRAGUEHELPERS_H_INCLUDED_BA908DB8_0651_4be6_BAF6_20090E64B086

#if _MSC_VER > 1000
    #pragma once
#endif // _MSC_VER > 1000

#include <Prague/prague.h>
#include <ProductCore/iface/i_taskmanager.h>
#include <Prague/pr_cpp.h>
#include <Updater/transport.h>

#include "../../KeepUp2Date/Client/helper/localize.h"

#include "PragueEnvironmentWrapper.h"

namespace PRAGUE_HELPERS {

// translate Update code to Prague understandable one
extern tERROR translateUpdaterResultCodeToPrague(const KLUPD::CoreError &);

// decrypt proxy server password
enum CryptType
{
    encrypt,
    decrypt,
};
extern KLUPD::NoCaseString cryptPassword(const CryptType &, cTaskManager *, const cStringObj &password, std::string &resultExplanation);

extern size_t networkTimeoutSeconds(PragueEnvironmentWrapper &, const size_t defaultTimeout = 60);

// convert Prague error code into human-readable string
extern std::string toStringPragueResult(const tERROR &);
// convert cProxySettings structure into human-readable string
extern std::string toString(const cProxySettings &);


////// string to date convert helper functions //////
 // check that all symbols in string is digit
extern bool checkDigits(const char *, const size_t);
 // convert string to tDATETIME
extern bool convertStringToDatetime(const KLUPD::NoCaseString &, tDATETIME &);


}   // namespace PRAGUE_HELPERS

#endif  // PRAGUEHELPERS_H_INCLUDED_BA908DB8_0651_4be6_BAF6_20090E64B086
