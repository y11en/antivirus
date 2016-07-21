#if !defined(AFX_CKAHPACKETRULES_H__52AA30FF_E08F_4C05_84A4_856E4E1B08CB__INCLUDED_)
#define AFX_CKAHPACKETRULES_H__52AA30FF_E08F_4C05_84A4_856E4E1B08CB__INCLUDED_

#include "../ckahrules/ckahrulesint.h"

bool ExportRuleIPList (const RuleAddressList &addr_list, FLTLIST &flt_list, bool bIsRemote);
bool ExportRulePortList (const RulePortList &port_list, FLTLIST &flt_list, bool bIsRemote);
bool ExportRuleTimeList (const RuleTimeList &time_list, FLTLIST &flt_list);

void ReGetFilterListByID (UUID uid, FLTLIST &ret_list, FLTLIST &in_list);
bool FillPacketRuleFromFLTLIST (const FLTLIST &flt_list, CPacketRule &PacketRule);

bool CreateFLTLISTFromPacketRule (IN const CPacketRule &PacketRule, OUT FLTLIST &flt_list);

#endif // !defined(AFX_CKAHPACKETRULES_H__52AA30FF_E08F_4C05_84A4_856E4E1B08CB__INCLUDED_)
