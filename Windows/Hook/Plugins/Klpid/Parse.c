//-------------------------- ѕј–—»Ќ√ ѕј ≈“ќ¬ -----------------------------------------------------
VERDICT
FilterTcp(PTCP_HEADER	TcpHeader, PVOID	Packet)
{	
	PIP_HEADER		IpHeader = (PIP_HEADER) ((PCHAR)Packet + sizeof (ETH_HEADER));
	PSTAT_ELEMENT	Element  = NULL;
	KL_TIME			CurrentTime;
	PCHAR 			TcpData = (PCHAR)TcpHeader + sizeof(PVOID) * TcpHeader->DataOffset;

	KlGetSystemTime(&CurrentTime);	

	Element = isElementExist(&GlobalStore.Attacks.ProtectList, IpHeader->srcIP);
	if (Element == NULL)
	{	// ’ост не найден. —оздадим и добавим в список
		Element = RegisterElement(IpHeader->srcIP, AttackOptions.MaxOpenedTcpPorts, AttackOptions.MaxOpenedUdpPorts, DEFAULT_FLOOD_LIST_COUNT);			
	}

	//  --- SMB_DIE ---
	if (AttackOptions.AttackSwitch.SmbDie && CheckSmbDie(Packet) == Verdict_Discard)
	{
		if (!CheckReason(Element, ATTACK_SMBDIE_ID))
			NotifyAttack(ATTACK_SMBDIE_ID, IpHeader->srcIP, &GlobalStore.Attacks.SmbDieNotificationTime);
		BanElement(Element, AttackOptions.BanTime, ATTACK_SMBDIE_ID);
		return Verdict_Discard;
	}	

    //  --- LoveSan ---
	if (AttackOptions.AttackSwitch.LoveSan && CheckLoveSanAttack(Packet) == Verdict_Discard)
	{
		if (!CheckReason(Element, ATTACK_LOVESAN_ID))
			NotifyAttack(ATTACK_LOVESAN_ID, IpHeader->srcIP, &GlobalStore.Attacks.LoveSanNotificationTime);
		BanElement(Element, AttackOptions.BanTime, ATTACK_LOVESAN_ID);
		return Verdict_Discard;
	}
    
	
	if (TcpHeader->syn)
	{
		
		if (TcpHeader->ack == 0)
		{
			BOOLEAN	 Status;
			
			if (Element == NULL)
			{	// нас перефлудили !!! нужно отключить сеть 	
				
				return Verdict_Discard;
			}
			
			RefreshPortCount(&Element->TcpPortList, &Element->TcpPortListFree, Element, &CurrentTime, AttackOptions.TcpRefreshTime );

			Status = RegisterPort (Element, &Element->TcpPortList, &Element->TcpPortListFree, TcpHeader->dstPort, IpHeader->Protocol);
			if ( Status == FALSE )
			{
				
				return Verdict_Discard;
			}				
			
			// TCP SCAN
			if (AttackOptions.AttackSwitch.TcpScan   && 
			CheckTcpScan( Element )
			)
			{				
				if (!CheckReason(Element, ATTACK_TCP_SCAN))				
					NotifyAttack(ATTACK_TCP_SCAN, IpHeader->srcIP, &GlobalStore.Attacks.TcpScanNotificationTime);
				BanElement(Element, AttackOptions.BanTime, ATTACK_TCP_SCAN);
				return Verdict_Discard;
			}
			
			// SYN FLOOD				
			if (AttackOptions.AttackSwitch.SynFlood  &&
				CheckFlood(Element, &Element->SynList, AttackOptions.SynFloodTime, AttackOptions.SynFloodCount)
				)
			{				
				if (!CheckReason(Element, ATTACK_SYN_FLOOD_ID))
					NotifyAttack(ATTACK_SYN_FLOOD_ID, IpHeader->srcIP, &GlobalStore.Attacks.SynFloodNotificationTime);
				BanElement(Element, AttackOptions.BanTime, ATTACK_SYN_FLOOD_ID);
				return Verdict_Discard;
			}
			
		}
	}	

	return Verdict_NotFiltered;
}

VERDICT
FilterUdp(PUDP_HEADER	UdpHeader, PVOID Packet)
{
	PIP_HEADER		IpHeader = (PIP_HEADER) ((PCHAR)Packet + sizeof (ETH_HEADER));
	PSTAT_ELEMENT	Element  = NULL;
	BOOLEAN			Status;
	KL_TIME			CurrentTime;
	
	KlGetSystemTime(&CurrentTime);

	RefreshTrustedList();
	if (CheckTrusted(IpHeader->srcIP, UdpHeader->dstPort))
		return Verdict_NotFiltered;
	
	Element = isElementExist(&GlobalStore.Attacks.ProtectList, IpHeader->srcIP);
	if (Element == NULL)
	{	// ’ост не найден. —оздадим и добавим в список
		Element = RegisterElement(IpHeader->srcIP, AttackOptions.MaxOpenedTcpPorts, AttackOptions.MaxOpenedUdpPorts , DEFAULT_FLOOD_LIST_COUNT);		
	}		
	
	if (Element == NULL)
	{	// нас перефлудили !!! нужно отключить сеть 
		
		return Verdict_Discard;
	}
	
	RefreshPortCount(&Element->UdpPortList, &Element->UdpPortListFree, Element, &CurrentTime, AttackOptions.UdpRefreshTime );

	Status = RegisterPort (Element, &Element->UdpPortList, &Element->UdpPortListFree, UdpHeader->dstPort, IpHeader->Protocol);	
	if (Status == FALSE )
	{
		
		return Verdict_Discard;
	}
	
	// UDP SCAN
	if (AttackOptions.AttackSwitch.UdpScan && 
		CheckUdpScan( Element )
		)
	{		
		if (!CheckReason(Element, ATTACK_UDP_SCAN))
			NotifyAttack(ATTACK_UDP_SCAN, IpHeader->srcIP, &GlobalStore.Attacks.UdpScanNotificationTime);
		BanElement(Element, AttackOptions.BanTime, ATTACK_UDP_SCAN);
		return Verdict_Discard;
	}

	// UDP_FLOOD
	if (AttackOptions.AttackSwitch.UdpFlood &&
		CheckFlood(Element, &Element->UdpList, AttackOptions.UdpFloodTime, AttackOptions.UdpFloodCount)
		)
	{	
				
		if (!CheckReason(Element, ATTACK_UDP_FLOOD_ID))
			NotifyAttack(ATTACK_UDP_FLOOD_ID, IpHeader->srcIP, &GlobalStore.Attacks.UdpFloodNotificationTime);
		BanElement(Element, AttackOptions.BanTime, ATTACK_UDP_FLOOD_ID);
		return Verdict_Discard;		
	}

	// HELKERN
	if (AttackOptions.AttackSwitch.Helkern && CheckHelkernAttack(Packet) == Verdict_Discard)
	{
		if (!CheckReason(Element, ATTACK_HELKERN_ID))
			NotifyAttack(ATTACK_HELKERN_ID, IpHeader->srcIP, &GlobalStore.Attacks.HelkernNotificationTime);
		BanElement(Element, AttackOptions.BanTime, ATTACK_HELKERN_ID);
		return Verdict_Discard;
	}
	
	
	return Verdict_NotFiltered;
}

VERDICT
FilterIcmp(PICMP_HEADER	IcmpHeader, PVOID	Packet)
{	
	PIP_HEADER		IpHeader = (PIP_HEADER) ((PCHAR)Packet + sizeof (ETH_HEADER));
	PSTAT_ELEMENT	Element = NULL;	
	USHORT			FragmentOffset = htons(IpHeader->Flg_FragOffs & 0xFF1F);

	Element = isElementExist(&GlobalStore.Attacks.ProtectList, IpHeader->srcIP);
	if (Element == NULL)
	{	// ’ост не найден. —оздадим и добавим в список
		Element = RegisterElement(IpHeader->srcIP, AttackOptions.MaxOpenedTcpPorts, AttackOptions.MaxOpenedUdpPorts , DEFAULT_FLOOD_LIST_COUNT);
	}
	
	if (Element == NULL)
	{	// нас перефлудили !!! нужно отключить сеть 		
		
		return Verdict_Discard;
	}

	//--------- Ping Of Death ----------------
	if (AttackOptions.AttackSwitch.PingOfDeath		&&
		FragmentOffset + htons(IpHeader->TotalLength) > 0xFFFF)
	{					
		if (!CheckReason(Element, ATTACK_PING_OF_DEATH_ID))
			NotifyAttack(ATTACK_PING_OF_DEATH_ID, IpHeader->srcIP, &GlobalStore.Attacks.PingOfDeathNotificationTime);
		BanElement(Element, AttackOptions.BanTime, ATTACK_PING_OF_DEATH_ID);
		return Verdict_Discard;		
	}

	// --------- ICMP Flood ------------------
	if (IcmpHeader->Type == 0x8 && IcmpHeader->Code == 0 && 
		AttackOptions.AttackSwitch.IcmpFlood			 &&
		CheckFlood(Element, &Element->IcmpList, AttackOptions.IcmpFloodTime, AttackOptions.IcmpFloodCount))
		{			
			if (!CheckReason(Element, ATTACK_ICMP_FLOOD))
			{								
				NotifyAttack(ATTACK_ICMP_FLOOD, IpHeader->srcIP, &GlobalStore.Attacks.IcmpFloodNotificationTime);
			}
			BanElement(Element, AttackOptions.BanTime, ATTACK_ICMP_FLOOD);
			return Verdict_Discard;					
		}

	return Verdict_NotFiltered;
}
//-----------------------------------------------------------------------------------------
VERDICT
FilterIp(PIP_HEADER	IpHeader, PVOID	Packet)
{
	VERDICT			Verdict = Verdict_NotFiltered;
	ULONG			HeaderSize	= IpHeader->Ihl * sizeof (ULONG);
	PSTAT_ELEMENT	Element		= isElementExist(&GlobalStore.Attacks.ProtectList, IpHeader->srcIP);

	if (Element == NULL)
	{	// ’ост не найден. —оздадим и добавим в список
		Element = RegisterElement(IpHeader->srcIP, AttackOptions.MaxOpenedTcpPorts, AttackOptions.MaxOpenedUdpPorts , DEFAULT_FLOOD_LIST_COUNT);
	}

	if (Element == NULL)
	{
		
		RefreshAllElements();
		Element = RegisterElement(IpHeader->srcIP, AttackOptions.MaxOpenedTcpPorts, AttackOptions.MaxOpenedUdpPorts , DEFAULT_FLOOD_LIST_COUNT);
		
		if (Element == NULL)
		{
			KL_TIME			CurrentTime;
			// нас перефлудили !!! нужно отключить сеть 
			bBlockALL = TRUE;
			KlGetSystemTime(&CurrentTime);
			BlockALL_StopTime = CurrentTime + ONE_SECOND_TIME * AttackOptions.BanTime;
			
			NotifyAttack(ATTACK_DDOS, 0, &GlobalStore.Attacks.DDOSAttackNotificationTime);
			
			return Verdict_Discard;
		}
	}

	ASSERT(Element);
	RefreshElement(Element); // Ёлемент может быть удален после рефреша !!!
	
	if (Element)
		KlGetSystemTime(&Element->TimeStamp);

	//------------- Land ---------------------
	if (IpHeader->srcIP == IpHeader->dstIP)
	{
		if (AttackOptions.AttackSwitch.Land)
		{
			KL_TIME	SomeTime;
			KlGetSystemTime(&SomeTime);
			
			// в листе причин по забаниванию этой атаки нет и быть не может.
			// специально дл€ нее создаем механизм, чтоб не чаще, чем раз в DEFAULT_LAND_NOTIFY_TIME посылать нотификацию
			NotifyAttack(ATTACK_LAND_ID, IpHeader->dstIP, &GlobalStore.Attacks.LandAttackNotificationTime);

			return Verdict_Discard;
		}
	}	

	switch (IpHeader->Protocol)
	{
	case 0x6:
		Verdict =  FilterTcp ( (PTCP_HEADER) ((PCHAR)IpHeader +  HeaderSize ), Packet) ;
		break;
	case 0x11:
		Verdict =  FilterUdp ( (PUDP_HEADER) ((PCHAR)IpHeader +  HeaderSize ), Packet) ;
		break;
	case 0x1:
		Verdict =  FilterIcmp( (PICMP_HEADER) ((PCHAR)IpHeader +  HeaderSize ), Packet) ;
		break;
	default:
		break;
	}

	if ( Element->isRegistered && !isNotBanned(Element) )
		return Verdict_Discard;	

	return Verdict;
}

VERDICT
FilterArp()
{
	return Verdict_NotFiltered;
}
//-------------------------------------------------------------------------------------------
BOOLEAN	isMulticast(PETH_HEADER	EthernetHeader)
{
	if (EthernetHeader->dstMac[0] == 0xFF &&
		EthernetHeader->dstMac[1] == 0xFF &&
		EthernetHeader->dstMac[2] == 0xFF &&
		EthernetHeader->dstMac[3] == 0xFF &&
		EthernetHeader->dstMac[4] == 0xFF &&
		EthernetHeader->dstMac[5] == 0xFF)
		return TRUE;
	
	return FALSE;

}

VERDICT	
FilterEthernet(PETH_HEADER	EthernetHeader)
{
	VERDICT	Verdict;
	
#ifdef isWIN9x
	if (IDS_ResetFlag)
	{
		DeregisterAllElements();
		IDS_ResetFlag = FALSE;
	}
#endif

	if (isMulticast(EthernetHeader))
	{		
		return Verdict_NotFiltered;
	}

	switch (htons(EthernetHeader->Type))
	{
	case ETH_P_ARP:
		break;
	case ETH_P_IP:
		Verdict = FilterIp( (PIP_HEADER)((PCHAR)EthernetHeader + sizeof (ETH_HEADER)), EthernetHeader ); 		
		return Verdict;				
	default:
		break;
	}	
	
	return Verdict_NotFiltered;
}
