#if !defined(AFX_CKAHAPPLICATIONRULES_H__771154B5_86A1_4BF2_A4FE_7D31326AFD34__INCLUDED_)
#define AFX_CKAHAPPLICATIONRULES_H__771154B5_86A1_4BF2_A4FE_7D31326AFD34__INCLUDED_

#include "../ckahrules/ckahrulesint.h"
#include "CKAHPacketRules.h"

bool FillApplicationRuleFromFLTLIST (const FLTLIST &flt_list, CApplicationRule &ApplicationRule);

bool CreateFLTLISTFromApplicationRule (IN const CApplicationRule &PacketRule, IN ULONG GroupID, OUT FLTLIST &flt_list);

#endif // !defined(AFX_CKAHAPPLICATIONRULES_H__771154B5_86A1_4BF2_A4FE_7D31326AFD34__INCLUDED_)
