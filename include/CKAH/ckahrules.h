#if !defined(__CKAHRULES_H__)
#define __CKAHRULES_H__

#include "ckahdefs.h"
#include "ckahrulesdefs.h"
#include "ckahip.h"
#include "../../Windows/Hook/Plugins/Include/fw_ev_api.h"

#define CKAHRULESMETHOD_ CKAHRULESEXPORT
#define CKAHRULESMETHOD CKAHRULESMETHOD_ CKAHUM::OpResult 

namespace CKAHFW
{
	typedef LPVOID HPACKETRULE, HPACKETRULES;
	typedef LPVOID HRULEADDRESS, HRULEADDRESSES;
	typedef LPVOID HRULEPORT, HRULEPORTS;
    typedef LPVOID HRULETIMEELEMENT, HRULETIME, HRULETIMES;

	typedef LPVOID HAPPRULEELEMENT;
	typedef LPVOID HAPPRULE, HAPPRULES;

	//////////////////////////////////////////////////////////////////////////
	//
	//	RulePort interface - один порт или диапазон портов
	//
	//

	enum RulePortType
	{
		rptSingle,
		rptRange
	};

	CKAHRULESMETHOD_ HRULEPORT RulePort_Create (IN RulePortType PortType);
	CKAHRULESMETHOD RulePort_Delete (IN HRULEPORT hPort);
	CKAHRULESMETHOD RulePort_Duplicate (IN HRULEPORT hPort, OUT HRULEPORT *hDupPort);

	// read only
	CKAHRULESMETHOD RulePort_GetType (IN HRULEPORT hPort, OUT RulePortType *pPortType);

	// rptSingle
	CKAHRULESMETHOD RulePort_GetSinglePort (IN HRULEPORT hPort, OUT USHORT *puPort);
	CKAHRULESMETHOD RulePort_SetSinglePort (IN HRULEPORT hPort, IN USHORT uPort);

	// rptRange
	CKAHRULESMETHOD RulePort_GetPortRange (IN HRULEPORT hPort, OUT USHORT *puLoPort, OUT USHORT *puHiPort);
	CKAHRULESMETHOD RulePort_SetPortRange (IN HRULEPORT hPort, IN USHORT uLoPort, IN USHORT uHiPort);

	CKAHRULESMETHOD_ void RulePort_Dump (IN HRULEPORT hPort);

	//////////////////////////////////////////////////////////////////////////
	//
	//	RulePorts interface - список элементов типа RulePort
	//
	//

	CKAHRULESMETHOD_ HRULEPORTS RulePorts_Create ();
	CKAHRULESMETHOD RulePorts_Delete (IN HRULEPORTS hPorts);
	CKAHRULESMETHOD RulePorts_Duplicate (IN HRULEPORTS hPorts, OUT HRULEPORTS *hDupPorts);

	CKAHRULESMETHOD_ ULONG RulePorts_GetCount (IN HRULEPORTS hPorts);

	CKAHRULESMETHOD RulePorts_GetItem (IN HRULEPORTS hPorts, IN ULONG uIndex, OUT HRULEPORT *pRulePort);

	CKAHRULESMETHOD RulePorts_DeleteItem (IN HRULEPORTS hPorts, IN ULONG uIndex);

	CKAHRULESMETHOD RulePorts_InsertItem (IN HRULEPORTS hPorts, IN ULONG uIndex, IN HRULEPORT RulePort);

	CKAHRULESMETHOD RulePorts_AddItemToFront (IN HRULEPORTS hPorts, IN HRULEPORT RulePort);

	CKAHRULESMETHOD RulePorts_AddItemToBack (IN HRULEPORTS hPorts, IN HRULEPORT RulePort);

	CKAHRULESMETHOD_ void RulePorts_Dump (IN HRULEPORTS hPorts);

	//////////////////////////////////////////////////////////////////////////
	//
	//	RuleAddress interface - список адресов хоста, диапазон адресов, подсеть
	//
	//

	enum RuleAddressType
	{
		ratRange,
		ratHost,
		ratSubnet
	};

	CKAHRULESMETHOD_ HRULEADDRESS RuleAddress_Create (IN RuleAddressType AddressType);
	CKAHRULESMETHOD RuleAddress_Delete (IN HRULEADDRESS hAddress);
	CKAHRULESMETHOD RuleAddress_Duplicate (IN HRULEADDRESS hAddress, OUT HRULEADDRESS *hDupAddress);

	CKAHRULESMETHOD_ ULONG RuleAddress_GetName (IN HRULEADDRESS hAddress, OUT LPWSTR lpBuffer, IN ULONG uSize);
	CKAHRULESMETHOD RuleAddress_SetName (IN HRULEADDRESS hAddress, IN LPCWSTR lpBuffer);

	// read only
	CKAHRULESMETHOD RuleAddress_GetType (IN HRULEADDRESS hAddress, OUT RuleAddressType *pAddressType);

	// ratRange
    CKAHRULESMETHOD RuleAddress_GetAddressRange (IN HRULEADDRESS hAddress, OUT CKAHUM::IP *pLoAddress, OUT CKAHUM::IP *pHiAddress);
	CKAHRULESMETHOD RuleAddress_SetAddressRange (IN HRULEADDRESS hAddress, IN CKAHUM::IP *pLoAddress, IN CKAHUM::IP *pHiAddress);

	// ratHost
	CKAHRULESMETHOD RuleAddress_GetAddressHostIPCount (IN HRULEADDRESS hAddress, OUT ULONG *puCount);
	CKAHRULESMETHOD RuleAddress_GetAddressHostIPItem (IN HRULEADDRESS hAddress, IN ULONG uIndex, OUT CKAHUM::IP *pAddress);
	CKAHRULESMETHOD RuleAddress_ClearAddressHostIPs (IN HRULEADDRESS hAddress);
	CKAHRULESMETHOD RuleAddress_AddHostIP (IN HRULEADDRESS hAddress, IN CKAHUM::IP *pAddress);

	// ratSubnet
	CKAHRULESMETHOD RuleAddress_GetAddressSubnet (IN HRULEADDRESS hAddress, OUT CKAHUM::IP *pAddress, OUT CKAHUM::IP *pMask);
	CKAHRULESMETHOD RuleAddress_SetAddressSubnet (IN HRULEADDRESS hAddress, IN CKAHUM::IP *pAddress, IN CKAHUM::IP *pMask);

	CKAHRULESMETHOD_ void RuleAddress_Dump (IN HRULEADDRESS hAddress);

	//////////////////////////////////////////////////////////////////////////
	//
	//	RuleAddresses interface - список элементов типа RuleAddress
	//
	//

	CKAHRULESMETHOD_ HRULEADDRESSES RuleAddresses_Create ();
	CKAHRULESMETHOD RuleAddresses_Delete (IN HRULEADDRESSES hAddresses);
	CKAHRULESMETHOD RuleAddresses_Duplicate (IN HRULEADDRESSES hAddresses, OUT HRULEADDRESSES *hDupAddresses);

	CKAHRULESMETHOD_ ULONG RuleAddresses_GetCount (IN HRULEADDRESSES hAddresses);

	CKAHRULESMETHOD RuleAddresses_GetItem (IN HRULEADDRESSES hAddresses, IN ULONG uIndex, OUT HRULEADDRESS *pRuleAddress);

	CKAHRULESMETHOD RuleAddresses_DeleteItem (IN HRULEADDRESSES hAddresses, IN ULONG uIndex);

	CKAHRULESMETHOD RuleAddresses_InsertItem (IN HRULEADDRESSES hAddresses, IN ULONG uIndex, IN HRULEADDRESS RuleAddress);

	CKAHRULESMETHOD RuleAddresses_AddItemToFront (IN HRULEADDRESSES hAddresses, IN HRULEADDRESS RuleAddress);

	CKAHRULESMETHOD RuleAddresses_AddItemToBack (IN HRULEADDRESSES hAddresses, IN HRULEADDRESS RuleAddress);

	CKAHRULESMETHOD_ void RuleAddresses_Dump (IN HRULEADDRESSES hAddresses);

	//////////////////////////////////////////////////////////////////////////
	//
	//	RuleTimeElement interface - элемент времени срабатывания правила
	//
	//

	CKAHRULESMETHOD_ HRULETIMEELEMENT RuleTimeElement_Create ();
	CKAHRULESMETHOD RuleTimeElement_Delete (IN HRULETIMEELEMENT hTimeElement);
	CKAHRULESMETHOD RuleTimeElement_Duplicate (IN HRULETIMEELEMENT hTimeElement, OUT HRULETIMEELEMENT *hDupTimeElement);

	enum RuleTimeElementType
	{
		rtetSecond,         // second of the minute, 0 - 59
		rtetDaySecond,      // second of the day starting from 00:00:00, 0 - 86399
		rtetMinute,         // minute of the hour, 0 - 59
		rtetHour,           // hour of the day, 0 - 23
		rtetDay,            // day of the month, 1 - 31
		rtetWeekDay,        // day of the week, 0 (sunday) - 6 (saturday)
		rtetMonth,          // month of the year, 1 (january) - 12 (december) 
		rtetYear,           // year AD (Anno Domini) 
	};

	CKAHRULESMETHOD RuleTimeElement_GetType (IN HRULETIMEELEMENT hTimeElement, OUT RuleTimeElementType *pTimeElementType);
	CKAHRULESMETHOD RuleTimeElement_SetType (IN HRULETIMEELEMENT hTimeElement, IN RuleTimeElementType TimeElementType);

    CKAHRULESMETHOD RuleTimeElement_GetRange (IN HRULETIME hTime, OUT ULONG *pLoValue, OUT ULONG *pHiValue);
    CKAHRULESMETHOD RuleTimeElement_SetRange (IN HRULETIME hTime, IN ULONG LoValue, IN ULONG HiValue);

	CKAHRULESMETHOD_ void RuleTimeElement_Dump (IN HRULETIMEELEMENT hTimeElement);

	//////////////////////////////////////////////////////////////////////////
	//
	//	RuleTime interface - время срабатывания правила, список элементов типа RuleTimeElement
	//
	//

	CKAHRULESMETHOD_ HRULETIME RuleTime_Create ();
	CKAHRULESMETHOD RuleTime_Delete (IN HRULETIME hTime);
	CKAHRULESMETHOD RuleTime_Duplicate (IN HRULETIME hTime, OUT HRULETIME *hDupTime);

	CKAHRULESMETHOD_ ULONG RuleTime_GetCount (IN HRULETIME hTime);

	CKAHRULESMETHOD RuleTime_GetItem (IN HRULETIME hTime, IN ULONG uIndex, OUT HRULETIMEELEMENT *pRuleTimeElement);

	CKAHRULESMETHOD RuleTime_DeleteItem (IN HRULETIME hTime, IN ULONG uIndex);

	CKAHRULESMETHOD RuleTime_InsertItem (IN HRULETIME hTime, IN ULONG uIndex, IN HRULETIMEELEMENT RuleTimeElement);

	CKAHRULESMETHOD RuleTime_AddItemToFront (IN HRULETIME hTime, IN HRULETIMEELEMENT RuleTimeElement);

	CKAHRULESMETHOD RuleTime_AddItemToBack (IN HRULETIME hTime, IN HRULETIMEELEMENT RuleTimeElement);

	CKAHRULESMETHOD_ void RuleTime_Dump (IN HRULETIME hTime);

	//////////////////////////////////////////////////////////////////////////
	//
	//	RuleTimes interface - набор времен срабатывания правила, список элементов типа RuleTime
	//
	//

	CKAHRULESMETHOD_ HRULETIMES RuleTimes_Create ();
	CKAHRULESMETHOD RuleTimes_Delete (IN HRULETIMES hTimes);
	CKAHRULESMETHOD RuleTimes_Duplicate (IN HRULETIMES hTimes, OUT HRULETIMES *hDupTimes);

	CKAHRULESMETHOD_ ULONG RuleTimes_GetCount (IN HRULETIMES hTimes);

	CKAHRULESMETHOD RuleTimes_GetItem (IN HRULETIMES hTimes, IN ULONG uIndex, OUT HRULETIME *pRuleTime);

	CKAHRULESMETHOD RuleTimes_DeleteItem (IN HRULETIMES hTimes, IN ULONG uIndex);

	CKAHRULESMETHOD RuleTimes_InsertItem (IN HRULETIMES hTimes, IN ULONG uIndex, IN HRULETIME RuleTime);

	CKAHRULESMETHOD RuleTimes_AddItemToFront (IN HRULETIMES hTimes, IN HRULETIME RuleTime);

	CKAHRULESMETHOD RuleTimes_AddItemToBack (IN HRULETIMES hTimes, IN HRULETIME RuleTime);

	CKAHRULESMETHOD_ void RuleTimes_Dump (IN HRULETIMES hTimes);

	//////////////////////////////////////////////////////////////////////////
	//
	//	PacketRule interface - пакетное правило
	//
	//

	CKAHRULESMETHOD_ HPACKETRULE PacketRule_Create ();
	CKAHRULESMETHOD PacketRule_Delete (IN HPACKETRULE hRule);
	CKAHRULESMETHOD PacketRule_Duplicate (IN HPACKETRULE hRule, OUT HPACKETRULE *hDupRule);

	CKAHRULESMETHOD PacketRule_GetIsEnabled (IN HPACKETRULE hRule, OUT bool *pbIsEnabled);
	CKAHRULESMETHOD PacketRule_SetIsEnabled (IN HPACKETRULE hRule, IN bool bIsEnabled);

	// Параметры:
	//	hRule - идентификатор правила
	//	lpBuffer - буфер
	//	uSize - размер буфера в WCHAR
	// Возвращаемое значение:
	//	при успехе возвращает длину в WCHAR строки, скопированной в буфер, не включая 0 на конце
	//	если длина больше, чем размер переданного буфера, то возвращается требуемый размер
	//	при ошибке возвращает 0
	CKAHRULESMETHOD_ ULONG PacketRule_GetName (IN HPACKETRULE hRule, OUT LPWSTR lpBuffer, IN ULONG uSize);

	// Параметры:
	//	hRule - идентификатор правила
	//	lpBuffer - буфер
	// Возвращаемое значение:
	//	0 - при ошибке
	CKAHRULESMETHOD PacketRule_SetName (IN HPACKETRULE hRule, IN LPCWSTR lpBuffer);

	CKAHRULESMETHOD PacketRule_GetIsBlocking (IN HPACKETRULE hRule, OUT bool *pbIsBlocking);
	CKAHRULESMETHOD PacketRule_SetIsBlocking (IN HPACKETRULE hRule, IN bool bIsBlocking);

	enum PacketDirection
	{
		pdIncoming = 1,
		pdOutgoing = 2,
		pdBoth = 3
	};

	CKAHRULESMETHOD PacketRule_GetPacketDirection (IN HPACKETRULE hRule, OUT PacketDirection *pDirection);
	CKAHRULESMETHOD PacketRule_SetPacketDirection (IN HPACKETRULE hRule, IN PacketDirection Direction);

    enum StreamDirection
    {
        sdIncoming = 1,
        sdOutgoing = 2,
        sdBoth = 3
    };

	CKAHRULESMETHOD PacketRule_GetStreamDirection (IN HPACKETRULE hRule, OUT StreamDirection *pDirection);
	CKAHRULESMETHOD PacketRule_SetStreamDirection (IN HPACKETRULE hRule, IN StreamDirection Direction);

	enum WellKnownProtocol
	{
		PROTO_ALL = 0x0,
		PROTO_TCP = 0x6,
		PROTO_UDP = 0x11,
		PROTO_ICMP = 0x1,
        PROTO_ICMPV6 = 0x3a,
	};

	CKAHRULESMETHOD PacketRule_GetProtocol (IN HPACKETRULE hRule, OUT UCHAR *puProtocol);
	CKAHRULESMETHOD PacketRule_SetProtocol (IN HPACKETRULE hRule, IN UCHAR uProtocol);

	CKAHRULESMETHOD PacketRule_GetRemoteAddresses (IN HPACKETRULE hRule, OUT HRULEADDRESSES *pRuleAddresses);
	CKAHRULESMETHOD PacketRule_SetRemoteAddresses (IN HPACKETRULE hRule, IN HRULEADDRESSES RuleAddresses);

	CKAHRULESMETHOD PacketRule_GetLocalAddresses (IN HPACKETRULE hRule, OUT HRULEADDRESSES *pRuleAddresses);
	CKAHRULESMETHOD PacketRule_SetLocalAddresses (IN HPACKETRULE hRule, IN HRULEADDRESSES RuleAddresses);

	CKAHRULESMETHOD PacketRule_GetTCPUDPRemotePorts (IN HPACKETRULE hRule, OUT HRULEPORTS *pRulePorts);
	CKAHRULESMETHOD PacketRule_SetTCPUDPRemotePorts (IN HPACKETRULE hRule, IN HRULEPORTS RulePorts);

	CKAHRULESMETHOD PacketRule_GetTCPUDPLocalPorts (IN HPACKETRULE hRule, OUT HRULEPORTS *pRulePorts);
	CKAHRULESMETHOD PacketRule_SetTCPUDPLocalPorts (IN HPACKETRULE hRule, IN HRULEPORTS RulePorts);

	CKAHRULESMETHOD PacketRule_GetICMPType (IN HPACKETRULE hRule, OUT UCHAR *puICMPType);
	CKAHRULESMETHOD PacketRule_SetICMPType (IN HPACKETRULE hRule, IN UCHAR uICMPType);

	CKAHRULESMETHOD PacketRule_GetICMPCode (IN HPACKETRULE hRule, OUT UCHAR *puICMPCode);
	CKAHRULESMETHOD PacketRule_SetICMPCode (IN HPACKETRULE hRule, IN UCHAR uICMPCode);

    CKAHRULESMETHOD PacketRule_GetTimes (IN HPACKETRULE hRule, OUT HRULETIMES *pRuleTimes);
    CKAHRULESMETHOD PacketRule_SetTimes (IN HPACKETRULE hRule, IN HRULETIMES RuleTimes);

	CKAHRULESMETHOD PacketRule_GetIsNotify (IN HPACKETRULE hRule, OUT bool *pbIsNotify);
	CKAHRULESMETHOD PacketRule_SetIsNotify (IN HPACKETRULE hRule, IN bool bIsNotify);

	CKAHRULESMETHOD PacketRule_GetIsLog (IN HPACKETRULE hRule, OUT bool *pbIsLog);
	CKAHRULESMETHOD PacketRule_SetIsLog (IN HPACKETRULE hRule, IN bool bIsLog);

	CKAHRULESMETHOD_ ULONG PacketRule_GetUserData (IN HPACKETRULE hRule, OUT UCHAR *lpBuffer, IN ULONG uSize);
	CKAHRULESMETHOD PacketRule_SetUserData (IN HPACKETRULE hRule, IN UCHAR *lpBuffer, IN ULONG uSize);

	CKAHRULESMETHOD_ void PacketRule_Dump (IN HPACKETRULE hRule);

	//////////////////////////////////////////////////////////////////////////
	//
	//	PacketRules interface - список элементов типа PacketRule
	//
	//

	CKAHRULESMETHOD_ HPACKETRULES PacketRules_Create ();
	CKAHRULESMETHOD PacketRules_Delete (IN HPACKETRULES hRules);
	CKAHRULESMETHOD PacketRules_Duplicate (IN HPACKETRULES hRules, OUT HPACKETRULES *hDupRules);

	CKAHRULESMETHOD_ ULONG PacketRules_GetCount (IN HPACKETRULES hRules);

	CKAHRULESMETHOD PacketRules_GetItem (IN HPACKETRULES hRules, IN ULONG uIndex, OUT HPACKETRULE *phRule);

	CKAHRULESMETHOD PacketRules_DeleteItem (IN HPACKETRULES hRules, IN ULONG uIndex);

	CKAHRULESMETHOD PacketRules_InsertItem (IN HPACKETRULES hRules, IN ULONG uIndex, IN HPACKETRULE hRule);

	CKAHRULESMETHOD PacketRules_AddItemToFront (IN HPACKETRULES hRules, IN HPACKETRULE hRule);

	CKAHRULESMETHOD PacketRules_AddItemToBack (IN HPACKETRULES hRules, IN HPACKETRULE hRule);

    CKAHRULESMETHOD PacketRules_Serialize (IN HPACKETRULES hRules, IN LPVOID pBuffer, IN ULONG uSize, OUT ULONG * pRetSize);
    CKAHRULESMETHOD PacketRules_Deserialize (IN HPACKETRULES hRules, IN LPVOID pBuffer, IN ULONG uSize);

	CKAHRULESMETHOD_ void PacketRules_Dump (IN HPACKETRULES hRules);

	//////////////////////////////////////////////////////////////////////////
	//
	//	ApplicationRuleElement interface
	//
	//
	
	CKAHRULESMETHOD_ HAPPRULEELEMENT ApplicationRuleElement_Create ();
	CKAHRULESMETHOD ApplicationRuleElement_Delete (IN HAPPRULEELEMENT hElement);
	CKAHRULESMETHOD ApplicationRuleElement_Duplicate (IN HAPPRULEELEMENT hElement, OUT HAPPRULEELEMENT *hDupElement);

	CKAHRULESMETHOD ApplicationRuleElement_GetPacketDirection (IN HAPPRULEELEMENT hElement, OUT PacketDirection *pDirection);
	CKAHRULESMETHOD ApplicationRuleElement_SetPacketDirection (IN HAPPRULEELEMENT hElement, IN PacketDirection Direction);

	CKAHRULESMETHOD ApplicationRuleElement_GetStreamDirection (IN HAPPRULEELEMENT hElement, OUT StreamDirection *pDirection);
	CKAHRULESMETHOD ApplicationRuleElement_SetStreamDirection (IN HAPPRULEELEMENT hElement, IN StreamDirection Direction);

	CKAHRULESMETHOD ApplicationRuleElement_GetProtocol (IN HAPPRULEELEMENT hElement, OUT UCHAR *puProtocol);
	CKAHRULESMETHOD ApplicationRuleElement_SetProtocol (IN HAPPRULEELEMENT hElement, IN UCHAR uProtocol);

	CKAHRULESMETHOD ApplicationRuleElement_GetRemoteAddresses (IN HAPPRULEELEMENT hElement, OUT HRULEADDRESSES *pAddresses);
	CKAHRULESMETHOD ApplicationRuleElement_SetRemoteAddresses (IN HAPPRULEELEMENT hElement, IN HRULEADDRESSES Addresses);

	CKAHRULESMETHOD ApplicationRuleElement_GetRemotePorts (IN HAPPRULEELEMENT hElement, OUT HRULEPORTS *pPorts);
	CKAHRULESMETHOD ApplicationRuleElement_SetRemotePorts (IN HAPPRULEELEMENT hElement, IN HRULEPORTS Ports);

	CKAHRULESMETHOD ApplicationRuleElement_GetLocalPorts (IN HAPPRULEELEMENT hElement, OUT HRULEPORTS *pPorts);
	CKAHRULESMETHOD ApplicationRuleElement_SetLocalPorts (IN HAPPRULEELEMENT hElement, IN HRULEPORTS Ports);

	CKAHRULESMETHOD ApplicationRuleElement_GetIsActive (IN HAPPRULEELEMENT hElement, OUT bool *pbIsActive);
	CKAHRULESMETHOD ApplicationRuleElement_SetIsActive (IN HAPPRULEELEMENT hElement, IN bool bIsActive);

	CKAHRULESMETHOD_ void ApplicationRuleElement_Dump (IN HAPPRULEELEMENT hElement);

	//////////////////////////////////////////////////////////////////////////
	//
	//	ApplicationRule interface
	//
	//

	CKAHRULESMETHOD_ HAPPRULE ApplicationRule_Create ();
	CKAHRULESMETHOD ApplicationRule_Delete (IN HAPPRULE hRule);
	CKAHRULESMETHOD ApplicationRule_Duplicate (IN HAPPRULE hRule, OUT HAPPRULE *hDupRule);

	CKAHRULESMETHOD_ ULONG ApplicationRule_GetName (IN HAPPRULE hRule, OUT LPWSTR lpBuffer, IN ULONG uSize);
	CKAHRULESMETHOD ApplicationRule_SetName (IN HAPPRULE hRule, IN LPCWSTR lpBuffer);

	const WCHAR szwSystemAppName[] = L"System";

	CKAHRULESMETHOD_ ULONG ApplicationRule_GetApplicationName (IN HAPPRULE hRule, OUT LPWSTR lpBuffer, IN ULONG uSize);
	CKAHRULESMETHOD ApplicationRule_SetApplicationName (IN HAPPRULE hRule, IN LPCWSTR lpBuffer);

	CKAHRULESMETHOD ApplicationRule_GetIsCommandLine (IN HAPPRULE hRule, OUT bool *pbIsCommandLine);
	CKAHRULESMETHOD ApplicationRule_SetIsCommandLine (IN HAPPRULE hRule, IN bool bIsCommandLine);

	CKAHRULESMETHOD_ ULONG ApplicationRule_GetCommandLine (IN HAPPRULE hRule, OUT LPWSTR lpBuffer, IN ULONG uSize);
	CKAHRULESMETHOD ApplicationRule_SetCommandLine (IN HAPPRULE hRule, IN LPCWSTR lpBuffer);

	CKAHRULESMETHOD ApplicationRule_GetIsEnabled (IN HAPPRULE hRule, OUT bool *pbIsEnabled);
	CKAHRULESMETHOD ApplicationRule_SetIsEnabled (IN HAPPRULE hRule, IN bool bIsEnabled);

	CKAHRULESMETHOD ApplicationRule_GetIsBlocking (IN HAPPRULE hRule, OUT bool *pbIsBlocking);
	CKAHRULESMETHOD ApplicationRule_SetIsBlocking (IN HAPPRULE hRule, IN bool bIsBlocking);

	CKAHRULESMETHOD_ ULONG ApplicationRule_GetElementCount (IN HAPPRULE hRule);

	CKAHRULESMETHOD ApplicationRule_GetElement (IN HAPPRULE hRule, IN ULONG uIndex, OUT HAPPRULEELEMENT *phElement);

	CKAHRULESMETHOD ApplicationRule_DeleteElement (IN HAPPRULE hRule, IN ULONG uIndex);

	CKAHRULESMETHOD ApplicationRule_InsertElement (IN HAPPRULE hRule, IN ULONG uIndex, IN HAPPRULEELEMENT hElement);

	CKAHRULESMETHOD ApplicationRule_AddElementToFront (IN HAPPRULE hRule, IN HAPPRULEELEMENT hElement);

	CKAHRULESMETHOD ApplicationRule_AddElementToBack (IN HAPPRULE hRule, IN HAPPRULEELEMENT hElement);

    CKAHRULESMETHOD ApplicationRule_GetTimes (IN HAPPRULE hRule, OUT HRULETIMES *pRuleTimes);
    CKAHRULESMETHOD ApplicationRule_SetTimes (IN HAPPRULE hRule, IN HRULETIMES RuleTimes);

	CKAHRULESMETHOD ApplicationRule_GetIsNotify (IN HAPPRULE hRule, OUT bool *pbIsNotify);
	CKAHRULESMETHOD ApplicationRule_SetIsNotify (IN HAPPRULE hRule, IN bool bIsNotify);

	CKAHRULESMETHOD ApplicationRule_GetIsLog (IN HAPPRULE hRule, OUT bool *pbIsLog);
	CKAHRULESMETHOD ApplicationRule_SetIsLog (IN HAPPRULE hRule, IN bool bIsLog);

	CKAHRULESMETHOD_ ULONG ApplicationRule_GetUserData (IN HAPPRULE hRule, OUT UCHAR *lpBuffer, IN ULONG uSize);
	CKAHRULESMETHOD ApplicationRule_SetUserData (IN HAPPRULE hRule, IN UCHAR *lpBuffer, IN ULONG uSize);

	CKAHRULESMETHOD ApplicationRule_GetApplicationChecksum (IN HAPPRULE hRule, OUT UCHAR Checksum[FW_HASH_SIZE]);
	CKAHRULESMETHOD ApplicationRule_SetApplicationChecksum (IN HAPPRULE hRule, IN UCHAR Checksum[FW_HASH_SIZE]);

	CKAHRULESMETHOD ApplicationRule_GetCheckHash (IN HAPPRULE hRule, OUT bool *pbCheckHash);
	CKAHRULESMETHOD ApplicationRule_SetCheckHash (IN HAPPRULE hRule, IN bool bCheckHash);

	CKAHRULESMETHOD_ void ApplicationRule_Dump (IN HAPPRULE hRule);

	//////////////////////////////////////////////////////////////////////////
	//
	//	ApplicationRules interface - список элементов типа ApplicationRule
	//
	//

	CKAHRULESMETHOD_ HAPPRULES ApplicationRules_Create ();
	CKAHRULESMETHOD ApplicationRules_Delete (IN HAPPRULES hRules);
	CKAHRULESMETHOD ApplicationRules_Duplicate (IN HAPPRULES hRules, OUT HAPPRULES *hDupRules);

	CKAHRULESMETHOD_ ULONG ApplicationRules_GetCount (IN HAPPRULES hRules);

	CKAHRULESMETHOD ApplicationRules_GetItem (IN HAPPRULES hRules, IN ULONG uIndex, OUT HAPPRULE *phRule);

	CKAHRULESMETHOD ApplicationRules_DeleteItem (IN HAPPRULES hRules, IN ULONG uIndex);

	CKAHRULESMETHOD ApplicationRules_InsertItem (IN HAPPRULES hRules, IN ULONG uIndex, IN HAPPRULE hRule);

	CKAHRULESMETHOD ApplicationRules_AddItemToFront (IN HAPPRULES hRules, IN HAPPRULE hRule);

	CKAHRULESMETHOD ApplicationRules_AddItemToBack (IN HAPPRULES hRules, IN HAPPRULE hRule);

    CKAHRULESMETHOD ApplicationRules_Serialize (IN HAPPRULES hRules, IN LPVOID pBuffer, IN ULONG uSize, OUT ULONG * pRetSize);
    CKAHRULESMETHOD ApplicationRules_Deserialize (IN HAPPRULES hRules, IN LPVOID pBuffer, IN ULONG uSize);
    
	CKAHRULESMETHOD_ void ApplicationRules_Dump (IN HAPPRULES hRules);

	//////////////////////////////////////////////////////////////////////////
};

//////////////////////////////////////////////////////////////////////////

#endif // !defined(__CKAHRULES_H__)
