#include "stdafx.h"
#include "ckahrulesint.h"

extern CProgramExecutionLog log;

//////////////////////////////////////////////////////////////////////////

#define VALIDATE_HANDLE(h, cls) \
	if (h && !(h = dynamic_cast<cls *>((cls *)h))) \
	{ \
		log.WriteFormat (_T("Bad handle passed: '%s *' expected "), PEL_ERROR, _T(#cls));\
		__asm int 3\
	}



//////////////////////////////////////////////////////////////////////////

#define BASEPORTPTR(hPort) ((*(RulePortItem *)hPort).operator -> ())
#define SINGLEPORTPTR(hPort) ((CRuleSinglePort *)BASEPORTPTR (hPort))
#define PORTRANGEPTR(hPort) ((CRulePortRange *)BASEPORTPTR (hPort))

CKAHFW::HRULEPORT CKAHFW::RulePort_Create (IN CKAHFW::RulePortType PortType)
{
	return (PortType == rptRange) ? 
				(HRULEPORT)new RulePortItem (new CRulePortRange) : 
				(HRULEPORT)new RulePortItem (new CRuleSinglePort);
}

CKAHUM::OpResult CKAHFW::RulePort_Delete (IN HRULEPORT hPort)
{
	VALIDATE_HANDLE (hPort, RulePortItem);
    return delete (RulePortItem *)hPort, CKAHUM::srOK;
}

CKAHUM::OpResult CKAHFW::RulePort_Duplicate (IN HRULEPORT hPort, OUT HRULEPORT *hDupPort)
{
	VALIDATE_HANDLE (hPort, RulePortItem);
	return (hPort && hDupPort) ? (*hDupPort = (BASEPORTPTR(hPort)->GetType () == rptRange) ?
		(HRULEPORT)new RulePortItem (new CRulePortRange (*PORTRANGEPTR(hPort))) :
		(HRULEPORT)new RulePortItem (new CRuleSinglePort (*SINGLEPORTPTR(hPort))), CKAHUM::srOK) : CKAHUM::srInvalidArg;
}

CKAHUM::OpResult CKAHFW::RulePort_GetType (IN HRULEPORT hPort, OUT CKAHFW::RulePortType *pPortType)
{
	VALIDATE_HANDLE (hPort, RulePortItem);
	return (hPort && pPortType) ? (*pPortType = BASEPORTPTR(hPort)->GetType (), CKAHUM::srOK) : CKAHUM::srInvalidArg;
}

CKAHUM::OpResult CKAHFW::RulePort_GetSinglePort (IN HRULEPORT hPort, OUT USHORT *puPort)
{
	VALIDATE_HANDLE (hPort, RulePortItem);
	return (hPort && puPort && BASEPORTPTR(hPort)->GetType () == CKAHFW::rptSingle) ?
				(*puPort = SINGLEPORTPTR(hPort)->m_Port, CKAHUM::srOK) : CKAHUM::srInvalidArg;
}

CKAHUM::OpResult CKAHFW::RulePort_SetSinglePort (IN HRULEPORT hPort, IN USHORT uPort)
{
	VALIDATE_HANDLE (hPort, RulePortItem);
	return (hPort && BASEPORTPTR(hPort)->GetType () == CKAHFW::rptSingle) ?
				(SINGLEPORTPTR(hPort)->m_Port = uPort, CKAHUM::srOK) : CKAHUM::srInvalidArg;
}

CKAHUM::OpResult CKAHFW::RulePort_GetPortRange (IN HRULEPORT hPort, OUT USHORT *puLoPort, OUT USHORT *puHiPort)
{
	VALIDATE_HANDLE (hPort, RulePortItem);
	return (hPort && puLoPort && puHiPort && BASEPORTPTR(hPort)->GetType () == CKAHFW::rptRange) ?
				(*puLoPort = PORTRANGEPTR(hPort)->m_LoPort, 
					*puHiPort = PORTRANGEPTR(hPort)->m_HiPort, CKAHUM::srOK) : CKAHUM::srInvalidArg;
}

CKAHUM::OpResult CKAHFW::RulePort_SetPortRange (IN HRULEPORT hPort, IN USHORT uLoPort, IN USHORT uHiPort)
{
	VALIDATE_HANDLE (hPort, RulePortItem);
	return (hPort && BASEPORTPTR (hPort)->GetType () == CKAHFW::rptRange) ?
			 (PORTRANGEPTR (hPort)->m_LoPort = min (uLoPort, uHiPort),
				PORTRANGEPTR (hPort)->m_HiPort = max (uLoPort, uHiPort), CKAHUM::srOK) : CKAHUM::srInvalidArg;
}

void CKAHFW::RulePort_Dump (IN HRULEPORT hPort)
{
	VALIDATE_HANDLE (hPort, RulePortItem);
	if (hPort)
		BASEPORTPTR (hPort)->Dump (log);
}

//////////////////////////////////////////////////////////////////////////

#define BASEPORTSPTR(hPort) ((*(RulePortsItem *)hPort).operator -> ())

CKAHFW::HRULEPORTS CKAHFW::RulePorts_Create ()
{
	return (HRULEPORTS)new RulePortsItem (new CRulePorts);
}

CKAHUM::OpResult CKAHFW::RulePorts_Delete (IN HRULEPORTS hPorts)
{
	VALIDATE_HANDLE (hPorts, RulePortsItem);
	return delete (RulePortsItem *)hPorts, CKAHUM::srOK;	
}

CKAHUM::OpResult CKAHFW::RulePorts_Duplicate (IN HRULEPORTS hPorts, OUT HRULEPORTS *hDupPorts)
{
	VALIDATE_HANDLE (hPorts, RulePortsItem);
	return (hPorts && hDupPorts) ? 
		(*hDupPorts =(HRULEPORTS)new RulePortsItem (new CRulePorts (*BASEPORTSPTR(hPorts))), CKAHUM::srOK) : CKAHUM::srInvalidArg;
}

ULONG CKAHFW::RulePorts_GetCount (IN HRULEPORTS hPorts)
{
	VALIDATE_HANDLE (hPorts, RulePortsItem);
	return hPorts ? BASEPORTSPTR (hPorts)->m_Ports.size () : 0;
}

CKAHUM::OpResult CKAHFW::RulePorts_GetItem (IN HRULEPORTS hPorts, IN ULONG uIndex, OUT HRULEPORT *pRulePort)
{
	VALIDATE_HANDLE (hPorts, RulePortsItem);
	if (!hPorts || !pRulePort || uIndex < 0 || uIndex >= RulePorts_GetCount (hPorts))
		return CKAHUM::srInvalidArg;

	ULONG j = 0;
	RulePortList::const_iterator i;
	for (i = BASEPORTSPTR (hPorts)->m_Ports.begin(); j < uIndex; ++i, ++j)
		;

	*pRulePort = (HRULEPORT)new RulePortItem (*i);

	return CKAHUM::srOK;
}

CKAHUM::OpResult CKAHFW::RulePorts_DeleteItem (IN HRULEPORTS hPorts, IN ULONG uIndex)
{
	VALIDATE_HANDLE (hPorts, RulePortsItem);
	if (!hPorts || uIndex < 0 || uIndex >= RulePorts_GetCount (hPorts))
		return CKAHUM::srInvalidArg;

	ULONG j = 0;
	RulePortList::iterator i;
	for (i = BASEPORTSPTR (hPorts)->m_Ports.begin(); j < uIndex; ++i, ++j)
		;

	BASEPORTSPTR(hPorts)->m_Ports.erase (i);

	return CKAHUM::srOK;
}

CKAHUM::OpResult CKAHFW::RulePorts_InsertItem (IN HRULEPORTS hPorts, IN ULONG uIndex, IN HRULEPORT RulePort)
{
	VALIDATE_HANDLE (hPorts, RulePortsItem);
	VALIDATE_HANDLE (RulePort, RulePortItem);
	if (!hPorts || !RulePort || uIndex < 0 || uIndex > RulePorts_GetCount (hPorts))
		return CKAHUM::srInvalidArg;

	ULONG j = 0;
	RulePortList::iterator i;
	for (i = BASEPORTSPTR (hPorts)->m_Ports.begin(); j < uIndex; ++i, ++j)
		;

	BASEPORTSPTR(hPorts)->m_Ports.insert (i, *(RulePortItem *)RulePort);

	return CKAHUM::srOK;
}

CKAHUM::OpResult CKAHFW::RulePorts_AddItemToFront (IN HRULEPORTS hPorts, IN HRULEPORT RulePort)
{
	VALIDATE_HANDLE (hPorts, RulePortsItem);
	VALIDATE_HANDLE (RulePort, RulePortItem);
	return (hPorts && RulePort) ?
		(BASEPORTSPTR(hPorts)->m_Ports.push_front (*(RulePortItem *)RulePort), CKAHUM::srOK) :
			CKAHUM::srInvalidArg;
}

CKAHUM::OpResult CKAHFW::RulePorts_AddItemToBack (IN HRULEPORTS hPorts, IN HRULEPORT RulePort)
{
	VALIDATE_HANDLE (hPorts, RulePortsItem);
	VALIDATE_HANDLE (RulePort, RulePortItem);
	return (hPorts && RulePort) ?
		(BASEPORTSPTR(hPorts)->m_Ports.push_back (*(RulePortItem *)RulePort), CKAHUM::srOK) :
			CKAHUM::srInvalidArg;
}

void CKAHFW::RulePorts_Dump (IN HRULEPORTS hPorts)
{
	VALIDATE_HANDLE (hPorts, RulePortsItem);
	if (hPorts)
		BASEPORTSPTR(hPorts)->Dump (log);
}

//////////////////////////////////////////////////////////////////////////

#define BASEADDRPTR(hAddress) ((*(RuleAddressItem *)hAddress).operator -> ())
#define ADDRRANGEPTR(hAddress) ((CRuleAddressRange *)BASEADDRPTR (hAddress))
#define HOSTADDRPTR(hAddress) ((CRuleHostAddress *)BASEADDRPTR (hAddress))
#define ADDRSUBNETPTR(hAddress) ((CRuleAddressSubnet *)BASEADDRPTR (hAddress))

CKAHFW::HRULEADDRESS CKAHFW::RuleAddress_Create (IN RuleAddressType AddressType)
{
	switch (AddressType)
	{
	case ratRange:
		return (HRULEADDRESS)new RuleAddressItem (new CRuleAddressRange);
	case ratHost:
		return (HRULEADDRESS)new RuleAddressItem (new CRuleHostAddress);
	case ratSubnet:
		return (HRULEADDRESS)new RuleAddressItem (new CRuleAddressSubnet);
	}

	return NULL;
}

CKAHUM::OpResult CKAHFW::RuleAddress_Delete (IN HRULEADDRESS hAddress)
{
	VALIDATE_HANDLE (hAddress, RuleAddressItem);
    return delete (RuleAddressItem *)hAddress, CKAHUM::srOK;
}

CKAHUM::OpResult CKAHFW::RuleAddress_Duplicate (IN HRULEADDRESS hAddress, OUT HRULEADDRESS *hDupAddress)
{
	VALIDATE_HANDLE (hAddress, RuleAddressItem);
	if (!hAddress || !hDupAddress)
		return CKAHUM::srInvalidArg;

	switch (BASEADDRPTR(hAddress)->GetType ())
	{
	case ratRange:
		*hDupAddress = (HRULEADDRESS)new RuleAddressItem (new CRuleAddressRange (*ADDRRANGEPTR (hAddress)));
		break;
	case ratHost:
		*hDupAddress = (HRULEADDRESS)new RuleAddressItem (new CRuleHostAddress (*HOSTADDRPTR (hAddress)));
		break;
	case ratSubnet:
		*hDupAddress = (HRULEADDRESS)new RuleAddressItem (new CRuleAddressSubnet (*ADDRSUBNETPTR (hAddress)));
		break;
	default:
		return CKAHUM::srInvalidArg;
	}

	return CKAHUM::srOK;
}

// read only
CKAHUM::OpResult CKAHFW::RuleAddress_GetType (IN HRULEADDRESS hAddress, OUT RuleAddressType *pAddressType)
{
	VALIDATE_HANDLE (hAddress, RuleAddressItem);
	return (hAddress && pAddressType) ? (*pAddressType = BASEADDRPTR(hAddress)->GetType (), CKAHUM::srOK) : CKAHUM::srInvalidArg;
}

CKAHUM::OpResult CKAHFW::RuleAddress_GetAddressRange (IN HRULEADDRESS hAddress, OUT CKAHUM::IP *pLoAddress, OUT CKAHUM::IP *pHiAddress)
{
	VALIDATE_HANDLE (hAddress, RuleAddressItem);
	return (hAddress && pLoAddress && pHiAddress && BASEADDRPTR (hAddress)->GetType () == CKAHFW::ratRange) ?
				(*pLoAddress = ADDRRANGEPTR (hAddress)->m_LoAddress,
				 *pHiAddress = ADDRRANGEPTR (hAddress)->m_HiAddress, CKAHUM::srOK) : CKAHUM::srInvalidArg;
}

CKAHUM::OpResult CKAHFW::RuleAddress_SetAddressRange (IN HRULEADDRESS hAddress, IN CKAHUM::IP *pLoAddress, IN CKAHUM::IP *pHiAddress)
{
	VALIDATE_HANDLE (hAddress, RuleAddressItem);
    return (hAddress && pLoAddress && pHiAddress && 
                pLoAddress->Version == pHiAddress->Version &&
                (pLoAddress->Isv6() ? pLoAddress->v6.Zone == pHiAddress->v6.Zone : true) &&
                BASEADDRPTR (hAddress)->GetType () == CKAHFW::ratRange) ?
				(ADDRRANGEPTR (hAddress)->m_LoAddress = min (*pLoAddress, *pHiAddress),
				 ADDRRANGEPTR (hAddress)->m_HiAddress = max (*pLoAddress, *pHiAddress), CKAHUM::srOK) : CKAHUM::srInvalidArg;
}

ULONG CKAHFW::RuleAddress_GetName (IN HRULEADDRESS hAddress, OUT LPWSTR lpBuffer, IN ULONG uSize)
{
	VALIDATE_HANDLE (hAddress, RuleAddressItem);
	if (!hAddress)
		return CKAHUM::srInvalidArg;

	CRuleAddressBase *pAddr = BASEADDRPTR (hAddress);

	ULONG uRequiredSize = pAddr->m_Name.size () + 1;

	if (uSize < uRequiredSize)
		return uRequiredSize;

	wcscpy (lpBuffer, pAddr->m_Name.c_str ());

	return wcslen (lpBuffer);
}

CKAHUM::OpResult CKAHFW::RuleAddress_SetName (IN HRULEADDRESS hAddress, IN LPCWSTR lpBuffer)
{
	VALIDATE_HANDLE (hAddress, RuleAddressItem);
	return (hAddress) ?
				(BASEADDRPTR (hAddress)->m_Name = lpBuffer, CKAHUM::srOK) : CKAHUM::srInvalidArg;
}

CKAHUM::OpResult CKAHFW::RuleAddress_GetAddressHostIPCount (IN HRULEADDRESS hAddress, OUT ULONG *puCount)
{
	VALIDATE_HANDLE (hAddress, RuleAddressItem);
	return (hAddress && puCount && BASEADDRPTR (hAddress)->GetType () == CKAHFW::ratHost) ?
				(*puCount = HOSTADDRPTR (hAddress)->m_IPs.size (), CKAHUM::srOK) : CKAHUM::srInvalidArg;
}

CKAHUM::OpResult CKAHFW::RuleAddress_GetAddressHostIPItem (IN HRULEADDRESS hAddress, IN ULONG uIndex, OUT CKAHUM::IP *pAddress)
{
	VALIDATE_HANDLE (hAddress, RuleAddressItem);
	return (hAddress && pAddress && BASEADDRPTR (hAddress)->GetType () == CKAHFW::ratHost && 
			uIndex >= 0 && uIndex < HOSTADDRPTR (hAddress)->m_IPs.size ()) ?
				(*pAddress = HOSTADDRPTR (hAddress)->m_IPs[uIndex], CKAHUM::srOK) : CKAHUM::srInvalidArg;
}

CKAHUM::OpResult CKAHFW::RuleAddress_ClearAddressHostIPs (IN HRULEADDRESS hAddress)
{
	VALIDATE_HANDLE (hAddress, RuleAddressItem);
	return (hAddress && BASEADDRPTR (hAddress)->GetType () == CKAHFW::ratHost) ? 
				(HOSTADDRPTR (hAddress)->m_IPs.clear (), CKAHUM::srOK) : CKAHUM::srInvalidArg;
}

CKAHUM::OpResult CKAHFW::RuleAddress_AddHostIP (IN HRULEADDRESS hAddress, IN CKAHUM::IP *pAddress)
{
	VALIDATE_HANDLE (hAddress, RuleAddressItem);
	return (hAddress && pAddress && BASEADDRPTR (hAddress)->GetType () == CKAHFW::ratHost) ? 
				(HOSTADDRPTR (hAddress)->m_IPs.push_back (*pAddress), CKAHUM::srOK) : CKAHUM::srInvalidArg;
}

CKAHUM::OpResult CKAHFW::RuleAddress_GetAddressSubnet (IN HRULEADDRESS hAddress, OUT CKAHUM::IP *pAddress, OUT CKAHUM::IP *pMask)
{
	VALIDATE_HANDLE (hAddress, RuleAddressItem);
	return (hAddress && pAddress && pMask && BASEADDRPTR (hAddress)->GetType () == CKAHFW::ratSubnet) ?
				(*pAddress = ADDRSUBNETPTR (hAddress)->m_Address,
					*pMask = ADDRSUBNETPTR (hAddress)->m_Mask, CKAHUM::srOK) : CKAHUM::srInvalidArg;
}

CKAHUM::OpResult CKAHFW::RuleAddress_SetAddressSubnet (IN HRULEADDRESS hAddress, IN CKAHUM::IP *pAddress, IN CKAHUM::IP *pMask)
{
	VALIDATE_HANDLE (hAddress, RuleAddressItem);
	return (hAddress && pAddress && pMask && pAddress->Version == pMask->Version &&
                BASEADDRPTR (hAddress)->GetType () == CKAHFW::ratSubnet) ?
				(ADDRSUBNETPTR (hAddress)->m_Address = *pAddress,
					ADDRSUBNETPTR (hAddress)->m_Mask = *pMask, CKAHUM::srOK) : CKAHUM::srInvalidArg;
}

void CKAHFW::RuleAddress_Dump (IN HRULEADDRESS hAddress)
{
	VALIDATE_HANDLE (hAddress, RuleAddressItem);
	if (hAddress)
		BASEADDRPTR (hAddress)->Dump (log);
}

//////////////////////////////////////////////////////////////////////////

#define BASEADDRSPTR(hAddresses) ((*(RuleAddressesItem *)hAddresses).operator -> ())

CKAHFW::HRULEADDRESSES CKAHFW::RuleAddresses_Create ()
{
	return (HRULEADDRESSES)new RuleAddressesItem (new CRuleAddresses);
}

CKAHUM::OpResult CKAHFW::RuleAddresses_Delete (IN HRULEADDRESSES hAddresses)
{
	VALIDATE_HANDLE (hAddresses, RuleAddressesItem);
	return delete (RuleAddressesItem *)hAddresses, CKAHUM::srOK;
}

CKAHUM::OpResult CKAHFW::RuleAddresses_Duplicate (IN HRULEADDRESSES hAddresses, OUT HRULEADDRESSES *hDupAddresses)
{
	VALIDATE_HANDLE (hAddresses, RuleAddressesItem);
	return (hAddresses && hDupAddresses) ? 
		(*hDupAddresses = (HRULEADDRESSES)new RuleAddressesItem (new CRuleAddresses (*BASEADDRSPTR (hAddresses))), CKAHUM::srOK) : CKAHUM::srInvalidArg;
}

ULONG CKAHFW::RuleAddresses_GetCount (IN HRULEADDRESSES hAddresses)
{
	VALIDATE_HANDLE (hAddresses, RuleAddressesItem);
	return hAddresses ? BASEADDRSPTR (hAddresses)->m_Addresses.size () : 0;
}

CKAHUM::OpResult CKAHFW::RuleAddresses_GetItem (IN HRULEADDRESSES hAddresses, IN ULONG uIndex, OUT HRULEADDRESS *pRuleAddress)
{
	VALIDATE_HANDLE (hAddresses, RuleAddressesItem);
	if (!hAddresses || !pRuleAddress || uIndex < 0 || uIndex >= RuleAddresses_GetCount (hAddresses))
		return CKAHUM::srInvalidArg;

	ULONG j = 0;
	RuleAddressList::const_iterator i;
	for (i = BASEADDRSPTR (hAddresses)->m_Addresses.begin(); j < uIndex; ++i, ++j)
		;

	*pRuleAddress = (HRULEPORT)new RuleAddressItem (*i);

	return CKAHUM::srOK;
}

CKAHUM::OpResult CKAHFW::RuleAddresses_DeleteItem (IN HRULEADDRESSES hAddresses, IN ULONG uIndex)
{
	VALIDATE_HANDLE (hAddresses, RuleAddressesItem);
	if (!hAddresses || uIndex < 0 || uIndex >= RuleAddresses_GetCount (hAddresses))
		return CKAHUM::srInvalidArg;

	ULONG j = 0;
	RuleAddressList::iterator i;
	for (i = BASEADDRSPTR (hAddresses)->m_Addresses.begin(); j < uIndex; ++i, ++j)
		;

	BASEADDRSPTR (hAddresses)->m_Addresses.erase (i);

	return CKAHUM::srOK;
}

CKAHUM::OpResult CKAHFW::RuleAddresses_InsertItem (IN HRULEADDRESSES hAddresses, IN ULONG uIndex, IN HRULEADDRESS RuleAddress)
{
	VALIDATE_HANDLE (hAddresses, RuleAddressesItem);
	VALIDATE_HANDLE (RuleAddress, RuleAddressItem);
	if (!hAddresses || !RuleAddress || uIndex < 0 || uIndex > RuleAddresses_GetCount (hAddresses))
		return CKAHUM::srInvalidArg;

	ULONG j = 0;
	RuleAddressList::iterator i;
	for (i = BASEADDRSPTR (hAddresses)->m_Addresses.begin(); j < uIndex; ++i, ++j)
		;

	BASEADDRSPTR (hAddresses)->m_Addresses.insert (i, *(RuleAddressItem *)RuleAddress);

	return CKAHUM::srOK;
}

CKAHUM::OpResult CKAHFW::RuleAddresses_AddItemToFront (IN HRULEADDRESSES hAddresses, IN HRULEADDRESS RuleAddress)
{
	VALIDATE_HANDLE (hAddresses, RuleAddressesItem);
	VALIDATE_HANDLE (RuleAddress, RuleAddressItem);
	return (hAddresses && RuleAddress) ?
		(BASEADDRSPTR (hAddresses)->m_Addresses.push_front (*(RuleAddressItem *)RuleAddress), CKAHUM::srOK) :
			CKAHUM::srInvalidArg;
}

CKAHUM::OpResult CKAHFW::RuleAddresses_AddItemToBack (IN HRULEADDRESSES hAddresses, IN HRULEADDRESS RuleAddress)
{
	VALIDATE_HANDLE (hAddresses, RuleAddressesItem);
	VALIDATE_HANDLE (RuleAddress, RuleAddressItem);
	return (hAddresses && RuleAddress) ?
		(BASEADDRSPTR (hAddresses)->m_Addresses.push_back (*(RuleAddressItem *)RuleAddress), CKAHUM::srOK) :
			CKAHUM::srInvalidArg;
}

void CKAHFW::RuleAddresses_Dump (IN HRULEADDRESSES hAddresses)
{
	VALIDATE_HANDLE (hAddresses, RuleAddressesItem);
	if (hAddresses)
		BASEADDRSPTR (hAddresses)->Dump (log);
}

//////////////////////////////////////////////////////////////////////////

#define BASETIMEELTPTR(hTimeElement) ((*(RuleTimeElementItem *)hTimeElement).operator -> ())

CKAHFW::HRULETIMEELEMENT CKAHFW::RuleTimeElement_Create ()
{
	return (HRULETIMEELEMENT)new RuleTimeElementItem (new CRuleTimeElement);
}

CKAHUM::OpResult CKAHFW::RuleTimeElement_Delete (IN HRULETIMEELEMENT hTimeElement)
{
	VALIDATE_HANDLE (hTimeElement, RuleTimeElementItem);
	return delete (RuleTimeElementItem *)hTimeElement, CKAHUM::srOK;
}

CKAHUM::OpResult CKAHFW::RuleTimeElement_Duplicate (IN HRULETIMEELEMENT hTimeElement, OUT HRULETIMEELEMENT *hDupTimeElement)
{
	VALIDATE_HANDLE (hTimeElement, RuleTimeElementItem);
	return (hTimeElement && hDupTimeElement) ? 
		(*hDupTimeElement = (HRULETIMEELEMENT)new RuleTimeElementItem (new CRuleTimeElement (*BASETIMEELTPTR (hTimeElement))), CKAHUM::srOK) : CKAHUM::srInvalidArg;
}

CKAHUM::OpResult CKAHFW::RuleTimeElement_GetType (IN HRULETIMEELEMENT hTimeElement, OUT RuleTimeElementType *pTimeElementType)
{
	VALIDATE_HANDLE (hTimeElement, RuleTimeElementItem);
	return (hTimeElement && pTimeElementType) ? (*pTimeElementType = BASETIMEELTPTR(hTimeElement)->m_Type, CKAHUM::srOK) : CKAHUM::srInvalidArg;
}

CKAHUM::OpResult CKAHFW::RuleTimeElement_SetType (IN HRULETIMEELEMENT hTimeElement, IN RuleTimeElementType TimeElementType)
{
	VALIDATE_HANDLE (hTimeElement, RuleTimeElementItem);
    return (hTimeElement) ? (BASETIMEELTPTR(hTimeElement)->m_Type = TimeElementType, CKAHUM::srOK) : CKAHUM::srInvalidArg;
}

CKAHUM::OpResult CKAHFW::RuleTimeElement_GetRange (IN HRULETIMEELEMENT hTimeElement, OUT ULONG *pLoValue, OUT ULONG *pHiValue)
{
	VALIDATE_HANDLE (hTimeElement, RuleTimeElementItem);
	return (hTimeElement && pLoValue && pHiValue) ?
				(*pLoValue = BASETIMEELTPTR(hTimeElement)->m_LoValue, 
					*pHiValue = BASETIMEELTPTR(hTimeElement)->m_HiValue, CKAHUM::srOK) : CKAHUM::srInvalidArg;
}

CKAHUM::OpResult CKAHFW::RuleTimeElement_SetRange (IN HRULETIMEELEMENT hTimeElement, IN ULONG LoValue, IN ULONG HiValue)
{
	VALIDATE_HANDLE (hTimeElement, RuleTimeElementItem);
	return (hTimeElement) ?
			 (BASETIMEELTPTR (hTimeElement)->m_LoValue = min (LoValue, HiValue),
				BASETIMEELTPTR (hTimeElement)->m_HiValue = max (LoValue, HiValue), CKAHUM::srOK) : CKAHUM::srInvalidArg;
}


//////////////////////////////////////////////////////////////////////////

#define BASETIMEPTR(hTime) ((*(RuleTimeItem *)hTime).operator -> ())

CKAHFW::HRULETIME CKAHFW::RuleTime_Create ()
{
	return (HRULETIME)new RuleTimeItem (new CRuleTime);
}

CKAHUM::OpResult CKAHFW::RuleTime_Delete (IN HRULETIME hTime)
{
	VALIDATE_HANDLE (hTime, RuleTimeItem);
	return delete (RuleTimeItem *)hTime, CKAHUM::srOK;
}

CKAHUM::OpResult CKAHFW::RuleTime_Duplicate (IN HRULETIME hTime, OUT HRULETIME *hDupTime)
{
	VALIDATE_HANDLE (hTime, RuleTimeItem);
	return (hTime && hDupTime) ? 
		(*hDupTime = (HRULETIME)new RuleTimeItem (new CRuleTime (*BASETIMEPTR (hTime))), CKAHUM::srOK) : CKAHUM::srInvalidArg;
}

ULONG CKAHFW::RuleTime_GetCount (IN HRULETIME hTime)
{
	VALIDATE_HANDLE (hTime, RuleTimeItem);
	return hTime ? BASETIMEPTR (hTime)->m_TimeElements.size () : 0;
}

CKAHUM::OpResult CKAHFW::RuleTime_GetItem (IN HRULETIME hTime, IN ULONG uIndex, OUT HRULETIMEELEMENT *pRuleTimeElement)
{
	VALIDATE_HANDLE (hTime, RuleTimeItem);
	if (!hTime || !pRuleTimeElement || uIndex < 0 || uIndex >= RuleTime_GetCount (hTime))
		return CKAHUM::srInvalidArg;

	ULONG j = 0;
	RuleTimeElementList::const_iterator i;
	for (i = BASETIMEPTR (hTime)->m_TimeElements.begin(); j < uIndex; ++i, ++j)
		;

	*pRuleTimeElement = (HRULEPORT)new RuleTimeElementItem (*i);

	return CKAHUM::srOK;
}

CKAHUM::OpResult CKAHFW::RuleTime_DeleteItem (IN HRULETIME hTime, IN ULONG uIndex)
{
	VALIDATE_HANDLE (hTime, RuleTimeItem);
	if (!hTime || uIndex < 0 || uIndex >= RuleTime_GetCount (hTime))
		return CKAHUM::srInvalidArg;

	ULONG j = 0;
	RuleTimeElementList::iterator i;
	for (i = BASETIMEPTR (hTime)->m_TimeElements.begin(); j < uIndex; ++i, ++j)
		;

	BASETIMEPTR (hTime)->m_TimeElements.erase (i);

	return CKAHUM::srOK;
}

CKAHUM::OpResult CKAHFW::RuleTime_InsertItem (IN HRULETIME hTime, IN ULONG uIndex, IN HRULETIMEELEMENT RuleTimeElement)
{
	VALIDATE_HANDLE (hTime, RuleTimeItem);
	VALIDATE_HANDLE (RuleTimeElement, RuleTimeElementItem);
	if (!hTime || !RuleTimeElement || uIndex < 0 || uIndex > RuleTime_GetCount (hTime))
		return CKAHUM::srInvalidArg;

	ULONG j = 0;
	RuleTimeElementList::iterator i;
	for (i = BASETIMEPTR (hTime)->m_TimeElements.begin(); j < uIndex; ++i, ++j)
		;

	BASETIMEPTR (hTime)->m_TimeElements.insert (i, *(RuleTimeElementItem *)RuleTimeElement);

	return CKAHUM::srOK;
}

CKAHUM::OpResult CKAHFW::RuleTime_AddItemToFront (IN HRULETIME hTime, IN HRULETIMEELEMENT RuleTimeElement)
{
	VALIDATE_HANDLE (hTime, RuleTimeItem);
	VALIDATE_HANDLE (RuleTimeElement, RuleTimeElementItem);
	return (hTime && RuleTimeElement) ?
		(BASETIMEPTR (hTime)->m_TimeElements.push_front (*(RuleTimeElementItem *)RuleTimeElement), CKAHUM::srOK) :
			CKAHUM::srInvalidArg;
}

CKAHUM::OpResult CKAHFW::RuleTime_AddItemToBack (IN HRULETIME hTime, IN HRULETIMEELEMENT RuleTimeElement)
{
	VALIDATE_HANDLE (hTime, RuleTimeItem);
	VALIDATE_HANDLE (RuleTimeElement, RuleTimeElementItem);
	return (hTime && RuleTimeElement) ?
		(BASETIMEPTR (hTime)->m_TimeElements.push_back (*(RuleTimeElementItem *)RuleTimeElement), CKAHUM::srOK) :
			CKAHUM::srInvalidArg;
}

void CKAHFW::RuleTime_Dump (IN HRULETIME hTime)
{
	VALIDATE_HANDLE (hTime, RuleTimeItem);
	if (hTime)
		BASETIMEPTR (hTime)->Dump (log);
}

//////////////////////////////////////////////////////////////////////////

#define BASETIMESPTR(hTimes) ((*(RuleTimesItem *)hTimes).operator -> ())

CKAHFW::HRULETIMES CKAHFW::RuleTimes_Create ()
{
	return (HRULETIMES)new RuleTimesItem (new CRuleTimes);
}

CKAHUM::OpResult CKAHFW::RuleTimes_Delete (IN HRULETIMES hTimes)
{
	VALIDATE_HANDLE (hTimes, RuleTimesItem);
	return delete (RuleTimesItem *)hTimes, CKAHUM::srOK;
}

CKAHUM::OpResult CKAHFW::RuleTimes_Duplicate (IN HRULETIMES hTimes, OUT HRULETIMES *hDupTimes)
{
	VALIDATE_HANDLE (hTimes, RuleTimesItem);
	return (hTimes && hDupTimes) ? 
		(*hDupTimes = (HRULETIMES)new RuleTimesItem (new CRuleTimes (*BASETIMESPTR (hTimes))), CKAHUM::srOK) : CKAHUM::srInvalidArg;
}

ULONG CKAHFW::RuleTimes_GetCount (IN HRULETIMES hTimes)
{
	VALIDATE_HANDLE (hTimes, RuleTimesItem);
	return hTimes ? BASETIMESPTR (hTimes)->m_Times.size () : 0;
}

CKAHUM::OpResult CKAHFW::RuleTimes_GetItem (IN HRULETIMES hTimes, IN ULONG uIndex, OUT HRULETIME *pRuleTime)
{
	VALIDATE_HANDLE (hTimes, RuleTimesItem);
	if (!hTimes || !pRuleTime || uIndex < 0 || uIndex >= RuleTimes_GetCount (hTimes))
		return CKAHUM::srInvalidArg;

	ULONG j = 0;
	RuleTimeList::const_iterator i;
	for (i = BASETIMESPTR (hTimes)->m_Times.begin(); j < uIndex; ++i, ++j)
		;

	*pRuleTime = (HRULEPORT)new RuleTimeItem (*i);

	return CKAHUM::srOK;
}

CKAHUM::OpResult CKAHFW::RuleTimes_DeleteItem (IN HRULETIMES hTimes, IN ULONG uIndex)
{
	VALIDATE_HANDLE (hTimes, RuleTimesItem);
	if (!hTimes || uIndex < 0 || uIndex >= RuleTimes_GetCount (hTimes))
		return CKAHUM::srInvalidArg;

	ULONG j = 0;
	RuleTimeList::iterator i;
	for (i = BASETIMESPTR (hTimes)->m_Times.begin(); j < uIndex; ++i, ++j)
		;

	BASETIMESPTR (hTimes)->m_Times.erase (i);

	return CKAHUM::srOK;
}

CKAHUM::OpResult CKAHFW::RuleTimes_InsertItem (IN HRULETIMES hTimes, IN ULONG uIndex, IN HRULETIME RuleTime)
{
	VALIDATE_HANDLE (hTimes, RuleTimesItem);
	VALIDATE_HANDLE (RuleTime, RuleTimeItem);
	if (!hTimes || !RuleTime || uIndex < 0 || uIndex > RuleTimes_GetCount (hTimes))
		return CKAHUM::srInvalidArg;

	ULONG j = 0;
	RuleTimeList::iterator i;
	for (i = BASETIMESPTR (hTimes)->m_Times.begin(); j < uIndex; ++i, ++j)
		;

	BASETIMESPTR (hTimes)->m_Times.insert (i, *(RuleTimeItem *)RuleTime);

	return CKAHUM::srOK;
}

CKAHUM::OpResult CKAHFW::RuleTimes_AddItemToFront (IN HRULETIMES hTimes, IN HRULETIME RuleTime)
{
	VALIDATE_HANDLE (hTimes, RuleTimesItem);
	VALIDATE_HANDLE (RuleTime, RuleTimeItem);
	return (hTimes && RuleTime) ?
		(BASETIMESPTR (hTimes)->m_Times.push_front (*(RuleTimeItem *)RuleTime), CKAHUM::srOK) :
			CKAHUM::srInvalidArg;
}

CKAHUM::OpResult CKAHFW::RuleTimes_AddItemToBack (IN HRULETIMES hTimes, IN HRULETIME RuleTime)
{
	VALIDATE_HANDLE (hTimes, RuleTimesItem);
	VALIDATE_HANDLE (RuleTime, RuleTimeItem);
	return (hTimes && RuleTime) ?
		(BASETIMESPTR (hTimes)->m_Times.push_back (*(RuleTimeItem *)RuleTime), CKAHUM::srOK) :
			CKAHUM::srInvalidArg;
}

void CKAHFW::RuleTimes_Dump (IN HRULETIMES hTimes)
{
	VALIDATE_HANDLE (hTimes, RuleTimesItem);
	if (hTimes)
		BASETIMESPTR (hTimes)->Dump (log);
}

//////////////////////////////////////////////////////////////////////////

#define BASEPACKETRULEPTR(hRule) ((*(PacketRuleItem *)hRule).operator -> ())

CKAHFW::HPACKETRULE CKAHFW::PacketRule_Create ()
{
	return (HPACKETRULE)new PacketRuleItem (new CPacketRule);
}

CKAHUM::OpResult CKAHFW::PacketRule_Delete (IN HPACKETRULE hRule)
{
	VALIDATE_HANDLE (hRule, PacketRuleItem);
	return delete (PacketRuleItem *)hRule ,CKAHUM::srOK;
}

CKAHUM::OpResult CKAHFW::PacketRule_Duplicate (IN HPACKETRULE hRule, OUT HPACKETRULE *hDupRule)
{
	VALIDATE_HANDLE (hRule, PacketRuleItem);
	return (hRule && hDupRule) ? 
		(*hDupRule = (HPACKETRULE)new PacketRuleItem (new CPacketRule (*BASEPACKETRULEPTR (hRule))), CKAHUM::srOK) : CKAHUM::srInvalidArg;
}

CKAHUM::OpResult CKAHFW::PacketRule_GetIsEnabled (IN HPACKETRULE hRule, OUT bool *pbIsEnabled)
{
	VALIDATE_HANDLE (hRule, PacketRuleItem);
	return (hRule && pbIsEnabled) ? (*pbIsEnabled = BASEPACKETRULEPTR (hRule)->m_bIsEnabled, CKAHUM::srOK) : CKAHUM::srInvalidArg;
}

CKAHUM::OpResult CKAHFW::PacketRule_SetIsEnabled (IN HPACKETRULE hRule, IN bool bIsEnabled)
{
	VALIDATE_HANDLE (hRule, PacketRuleItem);
	return (hRule) ? (BASEPACKETRULEPTR (hRule)->m_bIsEnabled = bIsEnabled, CKAHUM::srOK) : CKAHUM::srInvalidArg;
}

ULONG CKAHFW::PacketRule_GetName (IN HPACKETRULE hRule, OUT LPWSTR lpBuffer, IN ULONG uSize)
{
	VALIDATE_HANDLE (hRule, PacketRuleItem);
	if (!hRule)
		return CKAHUM::srInvalidArg;

	CPacketRule *pPacketRule = BASEPACKETRULEPTR (hRule);

	ULONG uRequiredSize = pPacketRule->m_Name.size () + 1;

	if (uSize < uRequiredSize)
		return uRequiredSize;

	wcscpy (lpBuffer, pPacketRule->m_Name.c_str ());

	return wcslen (lpBuffer);
}

CKAHUM::OpResult CKAHFW::PacketRule_SetName (IN HPACKETRULE hRule, IN LPCWSTR lpBuffer)
{
	VALIDATE_HANDLE (hRule, PacketRuleItem);
	return (hRule) ? (BASEPACKETRULEPTR (hRule)->m_Name = lpBuffer, CKAHUM::srOK) : CKAHUM::srInvalidArg;
}

CKAHUM::OpResult CKAHFW::PacketRule_GetIsBlocking (IN HPACKETRULE hRule, OUT bool *pbIsBlocking)
{
	VALIDATE_HANDLE (hRule, PacketRuleItem);
	return (hRule && pbIsBlocking) ? (*pbIsBlocking = BASEPACKETRULEPTR (hRule)->m_bIsBlocking, CKAHUM::srOK) : CKAHUM::srInvalidArg;
}

CKAHUM::OpResult CKAHFW::PacketRule_SetIsBlocking (IN HPACKETRULE hRule, IN bool bIsBlocking)
{
	VALIDATE_HANDLE (hRule, PacketRuleItem);
	return (hRule) ? (BASEPACKETRULEPTR (hRule)->m_bIsBlocking = bIsBlocking, CKAHUM::srOK) : CKAHUM::srInvalidArg;
}

CKAHUM::OpResult CKAHFW::PacketRule_GetPacketDirection (IN HPACKETRULE hRule, OUT PacketDirection *pDirection)
{
	VALIDATE_HANDLE (hRule, PacketRuleItem);
	return (hRule && pDirection) ? (*pDirection = BASEPACKETRULEPTR (hRule)->m_PacketDirection, CKAHUM::srOK) : CKAHUM::srInvalidArg;
}

CKAHUM::OpResult CKAHFW::PacketRule_SetPacketDirection (IN HPACKETRULE hRule, IN PacketDirection Direction)
{
	VALIDATE_HANDLE (hRule, PacketRuleItem);
	return (hRule) ? (BASEPACKETRULEPTR (hRule)->m_PacketDirection = Direction, CKAHUM::srOK) : CKAHUM::srInvalidArg;
}

CKAHUM::OpResult CKAHFW::PacketRule_GetStreamDirection (IN HPACKETRULE hRule, OUT StreamDirection *pDirection)
{
	VALIDATE_HANDLE (hRule, PacketRuleItem);
	return (hRule && pDirection) ? (*pDirection = BASEPACKETRULEPTR (hRule)->m_StreamDirection, CKAHUM::srOK) : CKAHUM::srInvalidArg;
}

CKAHUM::OpResult CKAHFW::PacketRule_SetStreamDirection (IN HPACKETRULE hRule, IN StreamDirection Direction)
{
	VALIDATE_HANDLE (hRule, PacketRuleItem);
	return (hRule) ? (BASEPACKETRULEPTR (hRule)->m_StreamDirection = Direction, CKAHUM::srOK) : CKAHUM::srInvalidArg;
}

CKAHUM::OpResult CKAHFW::PacketRule_GetProtocol (IN HPACKETRULE hRule, OUT UCHAR *puProtocol)
{
	VALIDATE_HANDLE (hRule, PacketRuleItem);
	return (hRule && puProtocol) ? (*puProtocol = BASEPACKETRULEPTR (hRule)->m_Proto, CKAHUM::srOK) : CKAHUM::srInvalidArg;
}

CKAHUM::OpResult CKAHFW::PacketRule_SetProtocol (IN HPACKETRULE hRule, IN UCHAR uProtocol)
{
	VALIDATE_HANDLE (hRule, PacketRuleItem);
	return (hRule) ? (BASEPACKETRULEPTR (hRule)->m_Proto = uProtocol, CKAHUM::srOK) : CKAHUM::srInvalidArg;
}

CKAHUM::OpResult CKAHFW::PacketRule_GetRemoteAddresses (IN HPACKETRULE hRule, OUT HRULEADDRESSES *pRuleAddresses)
{
	VALIDATE_HANDLE (hRule, PacketRuleItem);
	return (hRule && pRuleAddresses) ? 
		(*pRuleAddresses = (HRULEADDRESSES)new RuleAddressesItem (BASEPACKETRULEPTR (hRule)->m_RemoteAddresses), CKAHUM::srOK) : CKAHUM::srInvalidArg;
}

CKAHUM::OpResult CKAHFW::PacketRule_SetRemoteAddresses (IN HPACKETRULE hRule, IN HRULEADDRESSES RuleAddresses)
{
	VALIDATE_HANDLE (hRule, PacketRuleItem);
	VALIDATE_HANDLE (RuleAddresses, RuleAddressesItem);
	return (hRule) ?
		(BASEPACKETRULEPTR (hRule)->m_RemoteAddresses = *(RuleAddressesItem *)RuleAddresses, CKAHUM::srOK) : CKAHUM::srInvalidArg;
}

CKAHUM::OpResult CKAHFW::PacketRule_GetLocalAddresses (IN HPACKETRULE hRule, OUT HRULEADDRESSES *pRuleAddresses)
{
	VALIDATE_HANDLE (hRule, PacketRuleItem);
	return (hRule && pRuleAddresses) ? 
		(*pRuleAddresses = (HRULEADDRESSES)new RuleAddressesItem (BASEPACKETRULEPTR (hRule)->m_LocalAddresses), CKAHUM::srOK) : CKAHUM::srInvalidArg;
}

CKAHUM::OpResult CKAHFW::PacketRule_SetLocalAddresses (IN HPACKETRULE hRule, IN HRULEADDRESSES RuleAddresses)
{
	VALIDATE_HANDLE (hRule, PacketRuleItem);
	VALIDATE_HANDLE (RuleAddresses, RuleAddressesItem);
	return (hRule) ?
		(BASEPACKETRULEPTR (hRule)->m_LocalAddresses = *(RuleAddressesItem *)RuleAddresses, CKAHUM::srOK) : CKAHUM::srInvalidArg;
}

CKAHUM::OpResult CKAHFW::PacketRule_GetTCPUDPRemotePorts (IN HPACKETRULE hRule, OUT HRULEPORTS *pRulePorts)
{
	VALIDATE_HANDLE (hRule, PacketRuleItem);
	return (hRule && pRulePorts) ? 
		(*pRulePorts = (HRULEPORTS)new RulePortsItem (BASEPACKETRULEPTR (hRule)->m_TCPUDPRemotePorts), CKAHUM::srOK) : CKAHUM::srInvalidArg;
}

CKAHUM::OpResult CKAHFW::PacketRule_SetTCPUDPRemotePorts (IN HPACKETRULE hRule, IN HRULEPORTS RulePorts)
{
	VALIDATE_HANDLE (hRule, PacketRuleItem);
	VALIDATE_HANDLE (RulePorts, RulePortsItem);
	return (hRule) ?
		(BASEPACKETRULEPTR (hRule)->m_TCPUDPRemotePorts = *(RulePortsItem *)RulePorts, CKAHUM::srOK) : CKAHUM::srInvalidArg;
}

CKAHUM::OpResult CKAHFW::PacketRule_GetTCPUDPLocalPorts (IN HPACKETRULE hRule, OUT HRULEPORTS *pRulePorts)
{
	VALIDATE_HANDLE (hRule, PacketRuleItem);
	return (hRule && pRulePorts) ? 
		(*pRulePorts = (HRULEPORTS)new RulePortsItem (BASEPACKETRULEPTR (hRule)->m_TCPUDPLocalPorts), CKAHUM::srOK) : CKAHUM::srInvalidArg;
}

CKAHUM::OpResult CKAHFW::PacketRule_SetTCPUDPLocalPorts (IN HPACKETRULE hRule, IN HRULEPORTS RulePorts)
{
	VALIDATE_HANDLE (hRule, PacketRuleItem);
	VALIDATE_HANDLE (RulePorts, RulePortsItem);
	return (hRule) ?
		(BASEPACKETRULEPTR (hRule)->m_TCPUDPLocalPorts = *(RulePortsItem *)RulePorts, CKAHUM::srOK) : CKAHUM::srInvalidArg;
}

CKAHUM::OpResult CKAHFW::PacketRule_GetICMPType (IN HPACKETRULE hRule, OUT UCHAR *puICMPType)
{
	VALIDATE_HANDLE (hRule, PacketRuleItem);
	return (hRule && puICMPType) ? (*puICMPType = BASEPACKETRULEPTR (hRule)->m_ICMPType, CKAHUM::srOK) : CKAHUM::srInvalidArg;
}

CKAHUM::OpResult CKAHFW::PacketRule_SetICMPType (IN HPACKETRULE hRule, IN UCHAR uICMPType)
{
	VALIDATE_HANDLE (hRule, PacketRuleItem);
	return (hRule) ? (BASEPACKETRULEPTR (hRule)->m_ICMPType = uICMPType, CKAHUM::srOK) : CKAHUM::srInvalidArg;
}

CKAHUM::OpResult CKAHFW::PacketRule_GetICMPCode (IN HPACKETRULE hRule, OUT UCHAR *puICMPCode)
{
	VALIDATE_HANDLE (hRule, PacketRuleItem);
	return (hRule && puICMPCode) ? (*puICMPCode = BASEPACKETRULEPTR (hRule)->m_ICMPCode, CKAHUM::srOK) : CKAHUM::srInvalidArg;
}

CKAHUM::OpResult CKAHFW::PacketRule_SetICMPCode (IN HPACKETRULE hRule, IN UCHAR uICMPCode)
{
	VALIDATE_HANDLE (hRule, PacketRuleItem);
	return (hRule) ? (BASEPACKETRULEPTR (hRule)->m_ICMPCode = uICMPCode, CKAHUM::srOK) : CKAHUM::srInvalidArg;
}

CKAHUM::OpResult CKAHFW::PacketRule_GetTimes (IN HPACKETRULE hRule, OUT HRULETIMES *pRuleTimes)
{
	VALIDATE_HANDLE (hRule, PacketRuleItem);
	return (hRule && pRuleTimes) ? 
		(*pRuleTimes = (HRULETIMES)new RuleTimesItem (BASEPACKETRULEPTR (hRule)->m_Times), CKAHUM::srOK) : CKAHUM::srInvalidArg;
}

CKAHUM::OpResult CKAHFW::PacketRule_SetTimes (IN HPACKETRULE hRule, IN HRULETIMES RuleTimes)
{
	VALIDATE_HANDLE (hRule, PacketRuleItem);
	VALIDATE_HANDLE (RuleTimes, RuleTimesItem);
	return (hRule) ?
		(BASEPACKETRULEPTR (hRule)->m_Times = *(RuleTimesItem *)RuleTimes, CKAHUM::srOK) : CKAHUM::srInvalidArg;
}

CKAHUM::OpResult CKAHFW::PacketRule_GetIsNotify (IN HPACKETRULE hRule, OUT bool *pbIsNotify)
{
	VALIDATE_HANDLE (hRule, PacketRuleItem);
	return (hRule && pbIsNotify) ? (*pbIsNotify = BASEPACKETRULEPTR (hRule)->m_bNotify, CKAHUM::srOK) : CKAHUM::srInvalidArg;
}

CKAHUM::OpResult CKAHFW::PacketRule_SetIsNotify (IN HPACKETRULE hRule, IN bool bIsNotify)
{
	VALIDATE_HANDLE (hRule, PacketRuleItem);
	return (hRule) ? (BASEPACKETRULEPTR (hRule)->m_bNotify = bIsNotify, CKAHUM::srOK) : CKAHUM::srInvalidArg;
}

CKAHUM::OpResult CKAHFW::PacketRule_GetIsLog (IN HPACKETRULE hRule, OUT bool *pbIsLog)
{
	VALIDATE_HANDLE (hRule, PacketRuleItem);
	return (hRule && pbIsLog) ? (*pbIsLog = BASEPACKETRULEPTR (hRule)->m_bLog, CKAHUM::srOK) : CKAHUM::srInvalidArg;
}

CKAHUM::OpResult CKAHFW::PacketRule_SetIsLog (IN HPACKETRULE hRule, IN bool bIsLog)
{
	VALIDATE_HANDLE (hRule, PacketRuleItem);
	return (hRule) ? (BASEPACKETRULEPTR (hRule)->m_bLog = bIsLog, CKAHUM::srOK) : CKAHUM::srInvalidArg;
}

ULONG CKAHFW::PacketRule_GetUserData (IN HPACKETRULE hRule, OUT UCHAR *lpBuffer, IN ULONG uSize)
{
	VALIDATE_HANDLE (hRule, PacketRuleItem);
	if (!hRule)
		return CKAHUM::srInvalidArg;

	std::vector<UCHAR> &vec = BASEPACKETRULEPTR (hRule)->m_UserData;

	if (uSize < vec.size ())
		return vec.size ();

	memcpy (lpBuffer, &vec[0], vec.size ());

	return vec.size ();
}

CKAHUM::OpResult CKAHFW::PacketRule_SetUserData (IN HPACKETRULE hRule, IN UCHAR *lpBuffer, IN ULONG uSize)
{
	VALIDATE_HANDLE (hRule, PacketRuleItem);
	return (hRule) ? (BASEPACKETRULEPTR (hRule)->m_UserData.assign (lpBuffer, lpBuffer + uSize), CKAHUM::srOK) : CKAHUM::srInvalidArg;
}

void CKAHFW::PacketRule_Dump (IN HPACKETRULE hRule)
{
	VALIDATE_HANDLE (hRule, PacketRuleItem);
	if (hRule)
		BASEPACKETRULEPTR (hRule)->Dump (log);
}

//////////////////////////////////////////////////////////////////////////

#define BASEPRULESPTR(hRules) ((*(PacketRulesItem *)hRules).operator -> ())

CKAHFW::HPACKETRULES CKAHFW::PacketRules_Create ()
{
	return (HPACKETRULES)new PacketRulesItem (new CPacketRules);
}

CKAHUM::OpResult CKAHFW::PacketRules_Delete (IN HPACKETRULES hRules)
{
	VALIDATE_HANDLE (hRules, PacketRulesItem);
	return delete (PacketRulesItem *)hRules, CKAHUM::srOK;
}

CKAHUM::OpResult CKAHFW::PacketRules_Duplicate (IN HPACKETRULES hRules, OUT HPACKETRULES *hDupRules)
{
	VALIDATE_HANDLE (hRules, PacketRulesItem);
	return (hRules && hDupRules) ? 
		(*hDupRules = (HPACKETRULES)new PacketRulesItem (new CPacketRules (*BASEPRULESPTR (hRules))), CKAHUM::srOK) : CKAHUM::srInvalidArg;
}

ULONG CKAHFW::PacketRules_GetCount (IN HPACKETRULES hRules)
{
	VALIDATE_HANDLE (hRules, PacketRulesItem);
	return hRules ? BASEPRULESPTR (hRules)->m_Rules.size () : 0;
}

CKAHUM::OpResult CKAHFW::PacketRules_GetItem (IN HPACKETRULES hRules, IN ULONG uIndex, OUT HPACKETRULE *phRule)
{
	VALIDATE_HANDLE (hRules, PacketRulesItem);
	if (!hRules || !phRule || uIndex < 0 || uIndex >= PacketRules_GetCount (hRules))
		return CKAHUM::srInvalidArg;

	ULONG j = 0;
	PacketRuleList::const_iterator i;
	for (i = BASEPRULESPTR (hRules)->m_Rules.begin(); j < uIndex; ++i, ++j)
		;

	*phRule = (HPACKETRULE)new PacketRuleItem (*i);

	return CKAHUM::srOK;
}

CKAHUM::OpResult CKAHFW::PacketRules_DeleteItem (IN HPACKETRULES hRules, IN ULONG uIndex)
{
	VALIDATE_HANDLE (hRules, PacketRulesItem);
	if (!hRules || uIndex < 0 || uIndex >= PacketRules_GetCount (hRules))
		return CKAHUM::srInvalidArg;

	ULONG j = 0;
	PacketRuleList::iterator i;
	for (i = BASEPRULESPTR (hRules)->m_Rules.begin(); j < uIndex; ++i, ++j)
		;

	BASEPRULESPTR (hRules)->m_Rules.erase (i);

	return CKAHUM::srOK;
}

CKAHUM::OpResult CKAHFW::PacketRules_InsertItem (IN HPACKETRULES hRules, IN ULONG uIndex, IN HPACKETRULE hRule)
{
	VALIDATE_HANDLE (hRules, PacketRulesItem);
	VALIDATE_HANDLE (hRule, PacketRuleItem);
	if (!hRules || !hRule || uIndex < 0 || uIndex > PacketRules_GetCount (hRules))
		return CKAHUM::srInvalidArg;

	ULONG j = 0;
	PacketRuleList::iterator i;
	for (i = BASEPRULESPTR (hRules)->m_Rules.begin(); j < uIndex; ++i, ++j)
		;

	BASEPRULESPTR (hRules)->m_Rules.insert (i, *(PacketRuleItem *)hRule);

	return CKAHUM::srOK;
}

CKAHUM::OpResult CKAHFW::PacketRules_AddItemToFront (IN HPACKETRULES hRules, IN HPACKETRULE hRule)
{
	VALIDATE_HANDLE (hRules, PacketRulesItem);
	VALIDATE_HANDLE (hRule, PacketRuleItem);
	return (hRules && hRule) ?
		(BASEPRULESPTR (hRules)->m_Rules.push_front (*(PacketRuleItem *)hRule), CKAHUM::srOK) :
			CKAHUM::srInvalidArg;
}

CKAHUM::OpResult CKAHFW::PacketRules_AddItemToBack (IN HPACKETRULES hRules, IN HPACKETRULE hRule)
{
	VALIDATE_HANDLE (hRules, PacketRulesItem);
	VALIDATE_HANDLE (hRule, PacketRuleItem);
	return (hRules && hRule) ?
		(BASEPRULESPTR (hRules)->m_Rules.push_back (*(PacketRuleItem *)hRule), CKAHUM::srOK) :
			CKAHUM::srInvalidArg;
}

CKAHUM::OpResult CKAHFW::PacketRules_Serialize (IN HPACKETRULES hRules, IN LPVOID pBuffer, IN ULONG uSize, OUT ULONG * pRetSize)
{
    VALIDATE_HANDLE (hRules, PacketRulesItem);
    if (!hRules) return CKAHUM::srInvalidArg;

	CMemStorage mems;
	PoliType (mems, false) << *BASEPRULESPTR (hRules);

    if (pBuffer)
    {
        if (mems.size_ () > uSize) return CKAHUM::srInsufficientBuffer;
        memcpy(pBuffer, mems.get_lin_space_ (), mems.size_ ());
    }

    *pRetSize = mems.size_ ();

    return CKAHUM::srOK;
}

CKAHUM::OpResult CKAHFW::PacketRules_Deserialize (IN HPACKETRULES hRules, IN LPVOID pBuffer, IN ULONG uSize)
{
    VALIDATE_HANDLE (hRules, PacketRulesItem);
    return  (hRules && pBuffer && uSize) ? 
        (PoliType (CMemStorage (pBuffer, uSize)) << *BASEPRULESPTR (hRules), CKAHUM::srOK) :
            CKAHUM::srInvalidArg;
}


void CKAHFW::PacketRules_Dump (IN HPACKETRULES hRules)
{
	VALIDATE_HANDLE (hRules, PacketRulesItem);
	if (hRules)
		BASEPRULESPTR (hRules)->Dump (log);
}

//////////////////////////////////////////////////////////////////////////

#define APPRULEELEM(hElement) ((*(CApplicationRuleElementItem *)hElement).operator -> ())

CKAHFW::HAPPRULEELEMENT CKAHFW::ApplicationRuleElement_Create ()
{
	return (HAPPRULEELEMENT)new CApplicationRuleElementItem (new CApplicationRuleElement);
}

CKAHUM::OpResult CKAHFW::ApplicationRuleElement_Delete (IN HAPPRULEELEMENT hElement)
{
	VALIDATE_HANDLE (hElement, CApplicationRuleElementItem);
	return delete (CApplicationRuleElementItem *)hElement, CKAHUM::srOK;
}

CKAHUM::OpResult CKAHFW::ApplicationRuleElement_Duplicate (IN HAPPRULEELEMENT hElement, OUT HAPPRULEELEMENT *hDupElement)
{
	VALIDATE_HANDLE (hElement, CApplicationRuleElementItem);
	return (hElement && hDupElement) ? 
		(*hDupElement = (HAPPRULEELEMENT)new CApplicationRuleElementItem (new CApplicationRuleElement (*APPRULEELEM (hElement))), CKAHUM::srOK) : CKAHUM::srInvalidArg;
}

CKAHUM::OpResult CKAHFW::ApplicationRuleElement_GetPacketDirection (IN HAPPRULEELEMENT hElement, OUT PacketDirection *pDirection)
{
	VALIDATE_HANDLE (hElement, CApplicationRuleElementItem);
	return (hElement && pDirection) ? (*pDirection = APPRULEELEM (hElement)->m_PacketDirection, CKAHUM::srOK) : CKAHUM::srInvalidArg;
}

CKAHUM::OpResult CKAHFW::ApplicationRuleElement_SetPacketDirection (IN HAPPRULEELEMENT hElement, IN PacketDirection Direction)
{
	VALIDATE_HANDLE (hElement, CApplicationRuleElementItem);
	return (hElement) ? (APPRULEELEM (hElement)->m_PacketDirection = Direction, CKAHUM::srOK) : CKAHUM::srInvalidArg;
}

CKAHUM::OpResult CKAHFW::ApplicationRuleElement_GetStreamDirection (IN HAPPRULEELEMENT hElement, OUT StreamDirection *pDirection)
{
	VALIDATE_HANDLE (hElement, CApplicationRuleElementItem);
	return (hElement && pDirection) ? (*pDirection = APPRULEELEM (hElement)->m_StreamDirection, CKAHUM::srOK) : CKAHUM::srInvalidArg;
}

CKAHUM::OpResult CKAHFW::ApplicationRuleElement_SetStreamDirection (IN HAPPRULEELEMENT hElement, IN StreamDirection Direction)
{
	VALIDATE_HANDLE (hElement, CApplicationRuleElementItem);
	return (hElement) ? (APPRULEELEM (hElement)->m_StreamDirection = Direction, CKAHUM::srOK) : CKAHUM::srInvalidArg;
}

CKAHUM::OpResult CKAHFW::ApplicationRuleElement_GetProtocol (IN HAPPRULEELEMENT hElement, OUT UCHAR *puProtocol)
{
	VALIDATE_HANDLE (hElement, CApplicationRuleElementItem);
	return (hElement && puProtocol) ? (*puProtocol = APPRULEELEM (hElement)->m_Proto, CKAHUM::srOK) : CKAHUM::srInvalidArg;
}

CKAHUM::OpResult CKAHFW::ApplicationRuleElement_SetProtocol (IN HAPPRULEELEMENT hElement, IN UCHAR uProtocol)
{
	VALIDATE_HANDLE (hElement, CApplicationRuleElementItem);
	return (hElement) ? (APPRULEELEM (hElement)->m_Proto = uProtocol, CKAHUM::srOK) : CKAHUM::srInvalidArg;
}

CKAHUM::OpResult CKAHFW::ApplicationRuleElement_GetRemoteAddresses (IN HAPPRULEELEMENT hElement, OUT HRULEADDRESSES *pAddresses)
{
	VALIDATE_HANDLE (hElement, CApplicationRuleElementItem);
	return (hElement && pAddresses) ? 
		(*pAddresses = (HRULEADDRESSES)new RuleAddressesItem (APPRULEELEM (hElement)->m_RemoteAddresses), CKAHUM::srOK) : CKAHUM::srInvalidArg;
}

CKAHUM::OpResult CKAHFW::ApplicationRuleElement_SetRemoteAddresses (IN HAPPRULEELEMENT hElement, IN HRULEADDRESSES Addresses)
{
	VALIDATE_HANDLE (hElement, CApplicationRuleElementItem);
	VALIDATE_HANDLE (Addresses, RuleAddressesItem);
	return (hElement) ?
		(APPRULEELEM (hElement)->m_RemoteAddresses = *(RuleAddressesItem *)Addresses, CKAHUM::srOK) : CKAHUM::srInvalidArg;
}

CKAHUM::OpResult CKAHFW::ApplicationRuleElement_GetRemotePorts (IN HAPPRULEELEMENT hElement, OUT HRULEPORTS *pPorts)
{
	VALIDATE_HANDLE (hElement, CApplicationRuleElementItem);
	return (hElement && pPorts) ? 
		(*pPorts = (HRULEPORTS)new RulePortsItem (APPRULEELEM (hElement)->m_RemotePorts), CKAHUM::srOK) : CKAHUM::srInvalidArg;
}

CKAHUM::OpResult CKAHFW::ApplicationRuleElement_SetRemotePorts (IN HAPPRULEELEMENT hElement, IN HRULEPORTS Ports)
{
	VALIDATE_HANDLE (hElement, CApplicationRuleElementItem);
	VALIDATE_HANDLE (Ports, RulePortsItem);
	return (hElement) ?
		(APPRULEELEM (hElement)->m_RemotePorts = *(RulePortsItem *)Ports, CKAHUM::srOK) : CKAHUM::srInvalidArg;
}

CKAHUM::OpResult CKAHFW::ApplicationRuleElement_GetLocalPorts (IN HAPPRULEELEMENT hElement, OUT HRULEPORTS *pPorts)
{
	VALIDATE_HANDLE (hElement, CApplicationRuleElementItem);
	return (hElement && pPorts) ? 
		(*pPorts = (HRULEPORTS)new RulePortsItem (APPRULEELEM (hElement)->m_LocalPorts), CKAHUM::srOK) : CKAHUM::srInvalidArg;
}

CKAHUM::OpResult CKAHFW::ApplicationRuleElement_SetLocalPorts (IN HAPPRULEELEMENT hElement, IN HRULEPORTS Ports)
{
	VALIDATE_HANDLE (hElement, CApplicationRuleElementItem);
	VALIDATE_HANDLE (Ports, RulePortsItem);
	return (hElement) ?
		(APPRULEELEM (hElement)->m_LocalPorts = *(RulePortsItem *)Ports, CKAHUM::srOK) : CKAHUM::srInvalidArg;
}

CKAHUM::OpResult CKAHFW::ApplicationRuleElement_GetIsActive (IN HAPPRULEELEMENT hElement, OUT bool *pbIsActive)
{
	VALIDATE_HANDLE (hElement, CApplicationRuleElementItem);
	return (hElement && pbIsActive) ? (*pbIsActive = APPRULEELEM (hElement)->m_bIsActive, CKAHUM::srOK) : CKAHUM::srInvalidArg;
}

CKAHUM::OpResult CKAHFW::ApplicationRuleElement_SetIsActive (IN HAPPRULEELEMENT hElement, IN bool bIsActive)
{
	VALIDATE_HANDLE (hElement, CApplicationRuleElementItem);
	return (hElement) ? (APPRULEELEM (hElement)->m_bIsActive = bIsActive, CKAHUM::srOK) : CKAHUM::srInvalidArg;
}

void CKAHFW::ApplicationRuleElement_Dump (IN HAPPRULEELEMENT hElement)
{
	VALIDATE_HANDLE (hElement, CApplicationRuleElementItem);
	if (hElement)	
		APPRULEELEM (hElement)->Dump (log);
}

//////////////////////////////////////////////////////////////////////////

#define BASEAPPRULEPTR(hRule) ((*(ApplicationRuleItem *)hRule).operator -> ())

CKAHFW::HAPPRULE CKAHFW::ApplicationRule_Create ()
{
	return (HAPPRULE)new ApplicationRuleItem (new CApplicationRule);
}

CKAHUM::OpResult CKAHFW::ApplicationRule_Delete (IN HAPPRULE hRule)
{
	VALIDATE_HANDLE (hRule, ApplicationRuleItem);
	return delete (ApplicationRuleItem *)hRule ,CKAHUM::srOK;
}

CKAHUM::OpResult CKAHFW::ApplicationRule_Duplicate (IN HAPPRULE hRule, OUT HAPPRULE *hDupRule)
{
	VALIDATE_HANDLE (hRule, ApplicationRuleItem);
	return (hRule && hDupRule) ? 
		(*hDupRule = (HAPPRULE)new ApplicationRuleItem (new CApplicationRule (*BASEAPPRULEPTR (hRule))), CKAHUM::srOK) : CKAHUM::srInvalidArg;
}

ULONG CKAHFW::ApplicationRule_GetName (IN HAPPRULE hRule, OUT LPWSTR lpBuffer, IN ULONG uSize)
{
	VALIDATE_HANDLE (hRule, ApplicationRuleItem);
	if (!hRule)
		return CKAHUM::srInvalidArg;

	CApplicationRule *pApplicationRule = BASEAPPRULEPTR (hRule);

	ULONG uRequiredSize = pApplicationRule->m_Name.size () + 1;

	if (uSize < uRequiredSize)
		return uRequiredSize;

	wcscpy (lpBuffer, pApplicationRule->m_Name.c_str ());

	return wcslen (lpBuffer);
}

CKAHUM::OpResult CKAHFW::ApplicationRule_SetName (IN HAPPRULE hRule, IN LPCWSTR lpBuffer)
{
	VALIDATE_HANDLE (hRule, ApplicationRuleItem);
	return (hRule) ? (BASEAPPRULEPTR (hRule)->m_Name = lpBuffer, CKAHUM::srOK) : CKAHUM::srInvalidArg;
}

ULONG CKAHFW::ApplicationRule_GetApplicationName (IN HAPPRULE hRule, OUT LPWSTR lpBuffer, IN ULONG uSize)
{
	VALIDATE_HANDLE (hRule, ApplicationRuleItem);
	if (!hRule)
		return CKAHUM::srInvalidArg;

	CApplicationRule *pApplicationRule = BASEAPPRULEPTR (hRule);

	ULONG uRequiredSize = pApplicationRule->m_ApplicationName.size () + 1;

	if (uSize < uRequiredSize)
		return uRequiredSize;

	wcscpy (lpBuffer, pApplicationRule->m_ApplicationName.c_str ());

	return wcslen (lpBuffer);
}

CKAHUM::OpResult CKAHFW::ApplicationRule_SetApplicationName (IN HAPPRULE hRule, IN LPCWSTR lpBuffer)
{
	VALIDATE_HANDLE (hRule, ApplicationRuleItem);
	return (hRule) ? (BASEAPPRULEPTR (hRule)->m_ApplicationName = lpBuffer, CKAHUM::srOK) : CKAHUM::srInvalidArg;
}

CKAHUM::OpResult CKAHFW::ApplicationRule_GetIsCommandLine (IN HAPPRULE hRule, OUT bool *pbIsCommandLine)
{
	VALIDATE_HANDLE (hRule, ApplicationRuleItem);
	return (hRule && pbIsCommandLine) ? (*pbIsCommandLine = BASEAPPRULEPTR (hRule)->m_bIsCommandLine, CKAHUM::srOK) : CKAHUM::srInvalidArg;
}

CKAHUM::OpResult CKAHFW::ApplicationRule_SetIsCommandLine (IN HAPPRULE hRule, IN bool bIsCommandLine)
{
	VALIDATE_HANDLE (hRule, ApplicationRuleItem);
	return (hRule) ? (BASEAPPRULEPTR (hRule)->m_bIsCommandLine = bIsCommandLine, CKAHUM::srOK) : CKAHUM::srInvalidArg;
}

ULONG CKAHFW::ApplicationRule_GetCommandLine (IN HAPPRULE hRule, OUT LPWSTR lpBuffer, IN ULONG uSize)
{
	VALIDATE_HANDLE (hRule, ApplicationRuleItem);
	if (!hRule)
		return CKAHUM::srInvalidArg;

	CApplicationRule *pApplicationRule = BASEAPPRULEPTR (hRule);

	ULONG uRequiredSize = pApplicationRule->m_CommandLine.size () + 1;

	if (uSize < uRequiredSize)
		return uRequiredSize;

	wcscpy (lpBuffer, pApplicationRule->m_CommandLine.c_str ());

	return wcslen (lpBuffer);
}

CKAHUM::OpResult CKAHFW::ApplicationRule_SetCommandLine (IN HAPPRULE hRule, IN LPCWSTR lpBuffer)
{
	VALIDATE_HANDLE (hRule, ApplicationRuleItem);
	return (hRule) ? (BASEAPPRULEPTR (hRule)->m_CommandLine = lpBuffer, CKAHUM::srOK) : CKAHUM::srInvalidArg;
}

CKAHUM::OpResult CKAHFW::ApplicationRule_GetIsEnabled (IN HAPPRULE hRule, OUT bool *pbIsEnabled)
{
	VALIDATE_HANDLE (hRule, ApplicationRuleItem);
	return (hRule && pbIsEnabled) ? (*pbIsEnabled = BASEAPPRULEPTR (hRule)->m_bIsEnabled, CKAHUM::srOK) : CKAHUM::srInvalidArg;
}

CKAHUM::OpResult CKAHFW::ApplicationRule_SetIsEnabled (IN HAPPRULE hRule, IN bool bIsEnabled)
{
	VALIDATE_HANDLE (hRule, ApplicationRuleItem);
	return (hRule) ? (BASEAPPRULEPTR (hRule)->m_bIsEnabled = bIsEnabled, CKAHUM::srOK) : CKAHUM::srInvalidArg;
}

CKAHUM::OpResult CKAHFW::ApplicationRule_GetIsBlocking (IN HAPPRULE hRule, OUT bool *pbIsBlocking)
{
	VALIDATE_HANDLE (hRule, ApplicationRuleItem);
	return (hRule && pbIsBlocking) ? (*pbIsBlocking = BASEAPPRULEPTR (hRule)->m_bIsBlocking, CKAHUM::srOK) : CKAHUM::srInvalidArg;
}

CKAHUM::OpResult CKAHFW::ApplicationRule_SetIsBlocking (IN HAPPRULE hRule, IN bool bIsBlocking)
{
	VALIDATE_HANDLE (hRule, ApplicationRuleItem);
	return (hRule) ? (BASEAPPRULEPTR (hRule)->m_bIsBlocking = bIsBlocking, CKAHUM::srOK) : CKAHUM::srInvalidArg;
}

ULONG CKAHFW::ApplicationRule_GetElementCount (IN HAPPRULE hRule)
{
	VALIDATE_HANDLE (hRule, ApplicationRuleItem);
	return (hRule) ? BASEAPPRULEPTR (hRule)->m_Elements.size () : 0;
}

CKAHUM::OpResult CKAHFW::ApplicationRule_GetElement (IN HAPPRULE hRule, IN ULONG uIndex, OUT HAPPRULEELEMENT *phElement)
{
	VALIDATE_HANDLE (hRule, ApplicationRuleItem);
	if (!hRule || !phElement || uIndex < 0 || uIndex >= ApplicationRule_GetElementCount (hRule))
		return CKAHUM::srInvalidArg;

	ULONG j = 0;
	CApplicationRuleElementList::const_iterator i;
	for (i = BASEAPPRULEPTR (hRule)->m_Elements.begin(); j < uIndex; ++i, ++j)
		;

	*phElement = (HAPPRULEELEMENT)new CApplicationRuleElementItem (*i);

	return CKAHUM::srOK;
}

CKAHUM::OpResult CKAHFW::ApplicationRule_DeleteElement (IN HAPPRULE hRule, IN ULONG uIndex)
{
	VALIDATE_HANDLE (hRule, ApplicationRuleItem);
	if (!hRule || uIndex < 0 || uIndex >= ApplicationRule_GetElementCount (hRule))
		return CKAHUM::srInvalidArg;

	ULONG j = 0;
	CApplicationRuleElementList::iterator i;
	for (i = BASEAPPRULEPTR (hRule)->m_Elements.begin(); j < uIndex; ++i, ++j)
		;

	BASEAPPRULEPTR (hRule)->m_Elements.erase (i);

	return CKAHUM::srOK;
}

CKAHUM::OpResult CKAHFW::ApplicationRule_InsertElement (IN HAPPRULE hRule, IN ULONG uIndex, IN HAPPRULEELEMENT hElement)
{
	VALIDATE_HANDLE (hRule, ApplicationRuleItem);
	VALIDATE_HANDLE (hElement, CApplicationRuleElementItem);
	if (!hRule || !hElement || uIndex < 0 || uIndex > ApplicationRule_GetElementCount (hRule))
		return CKAHUM::srInvalidArg;

	ULONG j = 0;
	CApplicationRuleElementList::iterator i;
	for (i = BASEAPPRULEPTR (hRule)->m_Elements.begin(); j < uIndex; ++i, ++j)
		;

	BASEAPPRULEPTR (hRule)->m_Elements.insert (i, *(CApplicationRuleElementItem *)hElement);

	return CKAHUM::srOK;
}

CKAHUM::OpResult CKAHFW::ApplicationRule_AddElementToFront (IN HAPPRULE hRule, IN HAPPRULEELEMENT hElement)
{
	VALIDATE_HANDLE (hRule, ApplicationRuleItem);
	VALIDATE_HANDLE (hElement, CApplicationRuleElementItem);
	return (hRule && hElement) ?
		(BASEAPPRULEPTR (hRule)->m_Elements.push_front (*(CApplicationRuleElementItem *)hElement), CKAHUM::srOK) :
			CKAHUM::srInvalidArg;
}

CKAHUM::OpResult CKAHFW::ApplicationRule_AddElementToBack (IN HAPPRULE hRule, IN HAPPRULEELEMENT hElement)
{
	VALIDATE_HANDLE (hRule, ApplicationRuleItem);
	VALIDATE_HANDLE (hElement, CApplicationRuleElementItem);
	return (hRule && hElement) ?
		(BASEAPPRULEPTR (hRule)->m_Elements.push_back (*(CApplicationRuleElementItem *)hElement), CKAHUM::srOK) :
			CKAHUM::srInvalidArg;
}

CKAHUM::OpResult CKAHFW::ApplicationRule_GetTimes (IN HAPPRULE hRule, OUT HRULETIMES *pRuleTimes)
{
	VALIDATE_HANDLE (hRule, ApplicationRuleItem);
	return (hRule && pRuleTimes) ? 
		(*pRuleTimes = (HRULETIMES)new RuleTimesItem (BASEAPPRULEPTR (hRule)->m_Times), CKAHUM::srOK) : CKAHUM::srInvalidArg;
}

CKAHUM::OpResult CKAHFW::ApplicationRule_SetTimes (IN HAPPRULE hRule, IN HRULETIMES RuleTimes)
{
	VALIDATE_HANDLE (hRule, ApplicationRuleItem);
	VALIDATE_HANDLE (RuleTimes, RuleTimesItem);
	return (hRule) ?
		(BASEAPPRULEPTR (hRule)->m_Times = *(RuleTimesItem *)RuleTimes, CKAHUM::srOK) : CKAHUM::srInvalidArg;
}

CKAHUM::OpResult CKAHFW::ApplicationRule_GetIsNotify (IN HAPPRULE hRule, OUT bool *pbIsNotify)
{
	VALIDATE_HANDLE (hRule, ApplicationRuleItem);
	return (hRule && pbIsNotify) ? (*pbIsNotify = BASEAPPRULEPTR (hRule)->m_bNotify, CKAHUM::srOK) : CKAHUM::srInvalidArg;
}

CKAHUM::OpResult CKAHFW::ApplicationRule_SetIsNotify (IN HAPPRULE hRule, IN bool bIsNotify)
{
	VALIDATE_HANDLE (hRule, ApplicationRuleItem);
	return (hRule) ? (BASEAPPRULEPTR (hRule)->m_bNotify = bIsNotify, CKAHUM::srOK) : CKAHUM::srInvalidArg;
}

CKAHUM::OpResult CKAHFW::ApplicationRule_GetIsLog (IN HAPPRULE hRule, OUT bool *pbIsLog)
{
	VALIDATE_HANDLE (hRule, ApplicationRuleItem);
	return (hRule && pbIsLog) ? (*pbIsLog = BASEAPPRULEPTR (hRule)->m_bLog, CKAHUM::srOK) : CKAHUM::srInvalidArg;
}

CKAHUM::OpResult CKAHFW::ApplicationRule_SetIsLog (IN HAPPRULE hRule, IN bool bIsLog)
{
	VALIDATE_HANDLE (hRule, ApplicationRuleItem);
	return (hRule) ? (BASEAPPRULEPTR (hRule)->m_bLog = bIsLog, CKAHUM::srOK) : CKAHUM::srInvalidArg;
}

ULONG CKAHFW::ApplicationRule_GetUserData (IN HAPPRULE hRule, OUT UCHAR *lpBuffer, IN ULONG uSize)
{
	VALIDATE_HANDLE (hRule, ApplicationRuleItem);
	if (!hRule)
		return CKAHUM::srInvalidArg;

	std::vector<UCHAR> &vec = BASEAPPRULEPTR (hRule)->m_UserData;

	if (uSize < vec.size ())
		return vec.size ();

	memcpy (lpBuffer, &vec[0], vec.size ());

	return vec.size ();
}

CKAHUM::OpResult CKAHFW::ApplicationRule_SetUserData (IN HAPPRULE hRule, IN UCHAR *lpBuffer, IN ULONG uSize)
{
	VALIDATE_HANDLE (hRule, ApplicationRuleItem);
	return (hRule) ? (BASEAPPRULEPTR (hRule)->m_UserData.assign (lpBuffer, lpBuffer + uSize), CKAHUM::srOK) : CKAHUM::srInvalidArg;
}

void CKAHFW::ApplicationRule_Dump (IN HAPPRULE hRule)
{
	VALIDATE_HANDLE (hRule, ApplicationRuleItem);
	if (hRule)	
		BASEAPPRULEPTR (hRule)->Dump (log);
}

CKAHUM::OpResult CKAHFW::ApplicationRule_GetApplicationChecksum (IN HAPPRULE hRule, OUT UCHAR Checksum[FW_HASH_SIZE])
{
	VALIDATE_HANDLE (hRule, ApplicationRuleItem);
	if (!hRule)
	{
		memset (Checksum, 0, sizeof (Checksum));
		return CKAHUM::srInvalidArg;
	}

	std::vector<UCHAR> &vec = BASEAPPRULEPTR (hRule)->m_AppHash;
	
	if (vec.size() != FW_HASH_SIZE)
	{
		memset (Checksum, 0, sizeof (Checksum));
		return CKAHUM::srOpFailed;
	}
	
	memcpy (Checksum, &vec[0], vec.size());

	return CKAHUM::srOK;
}

CKAHUM::OpResult CKAHFW::ApplicationRule_SetApplicationChecksum (IN HAPPRULE hRule, IN UCHAR Checksum[FW_HASH_SIZE])
{
	VALIDATE_HANDLE (hRule, ApplicationRuleItem);
	return (hRule) ? (BASEAPPRULEPTR (hRule)->m_AppHash.assign (Checksum, Checksum + FW_HASH_SIZE), CKAHUM::srOK) : CKAHUM::srInvalidArg;
}

CKAHUM::OpResult CKAHFW::ApplicationRule_GetCheckHash (IN HAPPRULE hRule, OUT bool *pbCheckHash)
{
	VALIDATE_HANDLE (hRule, ApplicationRuleItem);
	return (hRule && pbCheckHash) ? (*pbCheckHash = BASEAPPRULEPTR (hRule)->m_bCheckHash, CKAHUM::srOK) : CKAHUM::srInvalidArg;
}

CKAHUM::OpResult CKAHFW::ApplicationRule_SetCheckHash (IN HAPPRULE hRule, IN bool bCheckHash)
{
	VALIDATE_HANDLE (hRule, ApplicationRuleItem);
	return (hRule) ? (BASEAPPRULEPTR (hRule)->m_bCheckHash = bCheckHash, CKAHUM::srOK) : CKAHUM::srInvalidArg;
}


//////////////////////////////////////////////////////////////////////////

#define BASEAPPRULESPTR(hRules) ((*(ApplicationRulesItem *)hRules).operator -> ())

CKAHFW::HAPPRULES CKAHFW::ApplicationRules_Create ()
{
	return (HAPPRULES)new ApplicationRulesItem (new CApplicationRules);
}

CKAHUM::OpResult CKAHFW::ApplicationRules_Delete (IN HAPPRULES hRules)
{
	VALIDATE_HANDLE (hRules, ApplicationRulesItem);
	return delete (ApplicationRulesItem *)hRules, CKAHUM::srOK;
}

CKAHUM::OpResult CKAHFW::ApplicationRules_Duplicate (IN HAPPRULES hRules, OUT HAPPRULES *hDupRules)
{
	VALIDATE_HANDLE (hRules, ApplicationRulesItem);
	return (hRules && hDupRules) ? 
		(*hDupRules = (HAPPRULES)new ApplicationRulesItem (new CApplicationRules (*BASEAPPRULESPTR (hRules))), CKAHUM::srOK) : CKAHUM::srInvalidArg;
}

ULONG CKAHFW::ApplicationRules_GetCount (IN HAPPRULES hRules)
{
	VALIDATE_HANDLE (hRules, ApplicationRulesItem);
	return hRules ? BASEAPPRULESPTR (hRules)->m_Rules.size () : 0;
}

CKAHUM::OpResult CKAHFW::ApplicationRules_GetItem (IN HAPPRULES hRules, IN ULONG uIndex, OUT HAPPRULE *phRule)
{
	VALIDATE_HANDLE (hRules, ApplicationRulesItem);
	if (!hRules || !phRule || uIndex < 0 || uIndex >= ApplicationRules_GetCount (hRules))
		return CKAHUM::srInvalidArg;

	ULONG j = 0;
	ApplicationRuleList::const_iterator i;
	for (i = BASEAPPRULESPTR (hRules)->m_Rules.begin(); j < uIndex; ++i, ++j)
		;

	*phRule = (HAPPRULE)new ApplicationRuleItem (*i);

	return CKAHUM::srOK;
}

CKAHUM::OpResult CKAHFW::ApplicationRules_DeleteItem (IN HAPPRULES hRules, IN ULONG uIndex)
{
	VALIDATE_HANDLE (hRules, ApplicationRulesItem);
	if (!hRules || uIndex < 0 || uIndex >= ApplicationRules_GetCount (hRules))
		return CKAHUM::srInvalidArg;

	ULONG j = 0;
	ApplicationRuleList::iterator i;
	for (i = BASEAPPRULESPTR (hRules)->m_Rules.begin(); j < uIndex; ++i, ++j)
		;

	BASEAPPRULESPTR (hRules)->m_Rules.erase (i);

	return CKAHUM::srOK;
}

CKAHUM::OpResult CKAHFW::ApplicationRules_InsertItem (IN HAPPRULES hRules, IN ULONG uIndex, IN HAPPRULE hRule)
{
	VALIDATE_HANDLE (hRules, ApplicationRulesItem);
	VALIDATE_HANDLE (hRule, ApplicationRuleItem);
	if (!hRules || !hRule || uIndex < 0 || uIndex > ApplicationRules_GetCount (hRules))
		return CKAHUM::srInvalidArg;

	ULONG j = 0;
	ApplicationRuleList::iterator i;
	for (i = BASEAPPRULESPTR (hRules)->m_Rules.begin(); j < uIndex; ++i, ++j)
		;

	BASEAPPRULESPTR (hRules)->m_Rules.insert (i, *(ApplicationRuleItem *)hRule);

	return CKAHUM::srOK;
}

CKAHUM::OpResult CKAHFW::ApplicationRules_AddItemToFront (IN HAPPRULES hRules, IN HAPPRULE hRule)
{
	VALIDATE_HANDLE (hRules, ApplicationRulesItem);
	VALIDATE_HANDLE (hRule, ApplicationRuleItem);
	return (hRules && hRule) ?
		(BASEAPPRULESPTR (hRules)->m_Rules.push_front (*(ApplicationRuleItem *)hRule), CKAHUM::srOK) :
			CKAHUM::srInvalidArg;
}

CKAHUM::OpResult CKAHFW::ApplicationRules_AddItemToBack (IN HAPPRULES hRules, IN HAPPRULE hRule)
{
	VALIDATE_HANDLE (hRules, ApplicationRulesItem);
	VALIDATE_HANDLE (hRule, ApplicationRuleItem);
	return (hRules && hRule) ?
		(BASEAPPRULESPTR (hRules)->m_Rules.push_back (*(ApplicationRuleItem *)hRule), CKAHUM::srOK) :
			CKAHUM::srInvalidArg;
}

CKAHUM::OpResult CKAHFW::ApplicationRules_Serialize (IN HAPPRULES hRules, IN LPVOID pBuffer, IN ULONG uSize, OUT ULONG * pRetSize)
{
    VALIDATE_HANDLE (hRules, ApplicationRulesItem);
    if (!hRules) return CKAHUM::srInvalidArg;

	CMemStorage mems;
	PoliType (mems, false) << *BASEAPPRULESPTR (hRules);

    if (pBuffer)
    {
        if (mems.size_ () > uSize) return CKAHUM::srInsufficientBuffer;
        memcpy(pBuffer, mems.get_lin_space_ (), mems.size_ ());
    }

    *pRetSize = mems.size_ ();

    return CKAHUM::srOK;
}

CKAHUM::OpResult CKAHFW::ApplicationRules_Deserialize (IN HAPPRULES hRules, IN LPVOID pBuffer, IN ULONG uSize)
{
    VALIDATE_HANDLE (hRules, ApplicationRulesItem);
    return  (hRules && pBuffer && uSize) ? 
        (PoliType (CMemStorage (pBuffer, uSize)) << *BASEAPPRULESPTR (hRules), CKAHUM::srOK) :
            CKAHUM::srInvalidArg;
}

void CKAHFW::ApplicationRules_Dump (IN HAPPRULES hRules)
{
	VALIDATE_HANDLE (hRules, ApplicationRulesItem);
	if (hRules)	
		BASEAPPRULESPTR (hRules)->Dump (log);
}

//////////////////////////////////////////////////////////////////////////


