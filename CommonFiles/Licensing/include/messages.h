#if !defined (__MESSAGES_H__)
#define __MESSAGES_H__

#include "lic_defs.h"

const _TCHAR* IDS_KeyValid              = _T ("License key is valid");
const _TCHAR* IDS_KeyIsExpired          = _T ("License key has expired");
const _TCHAR* IDS_KeyIsCorrupted        = _T ("License key is corrupted");
const _TCHAR* IDS_KeyIsNotSigned        = _T ("Digital signature not found, corrupted, or does not match the Kaspersky Lab digital signature");
const _TCHAR* IDS_KeyIsWrongForProduct  = _T ("This license key is not for this product");
const _TCHAR* IDS_KeyNotFound           = _T ("License key not found");
const _TCHAR* IDS_KeyIsBlacklisted      = _T ("License key is blocked by Kaspersky Lab");
const _TCHAR* IDS_TrialIsAlreadyUsed    = _T ("Trial key has already been used on this computer");
const _TCHAR* IDS_IllegalBaseUpdate     = _T ("Anti-virus database was manually updated (licensing policy violated)");
const _TCHAR* IDS_BasesCorrupted        = _T ("Anti-virus database is corrupted");
const _TCHAR* IDS_CannotInstallSecondTrial = _T ("Cannot register trial key while another trial key is in use");
const _TCHAR* IDS_KeyCreationDateInvalid  = _T ("Incorrect license key creation date. Check your system time and date");
const _TCHAR* IDS_InvalidBlacklist      = _T ("Blacklist is not found or corrupted");
const _TCHAR* IDS_InvalidUpdateDescription  = _T ("Anti-virus bases update description file not found or corrupted");

#endif
