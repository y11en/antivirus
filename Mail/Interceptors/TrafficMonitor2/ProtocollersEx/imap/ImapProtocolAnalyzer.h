/*
*/

#if !defined(__KL_IMAPPROTOCOLANALYZER_H)
#define __KL_IMAPPROTOCOLANALYZER_H

#include "../common/ProtocolAnalyzerImplAdapter.h"
#include "ImapProtocolAnalyzerImpl.h"

namespace TrafficProtocoller
{

typedef Util::ProtocolAnalyzerImplAdapter<ImapProtocolAnalyzerImpl> ImapProtocolAnalyzer;

}  // namespace TrafficProtocoller

#endif   // !defined(__KL_IMAPPROTOCOLANALYZER_H)
