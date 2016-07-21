#ifndef __CONTROL_H
#define __CONTROL_H

HRESULT
GetRulesListSize ( PULONG psize );

HRESULT
GetRulesList ( ULONG max_size,  PKLFLTDEV_RULES pdev_rules );

HRESULT
SetRule ( PWCHAR wcGuid, PWCHAR wcDevType, ULONG mask, PREPLUG_STATUS preplug_status );

HRESULT
SetRule2 ( PKLFLTDEV_RULE prule, ULONG rule_size, PREPLUG_STATUS preplug_status );

HRESULT
ApplyRules();

#endif