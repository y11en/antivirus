LIST_ENTRY	TcpStealthTable;
SPIN_LOCK	TcpStealthTableLock;

LIST_ENTRY	UdpStealthTable;
SPIN_LOCK	UdpStealthTableLock;

LIST_ENTRY  FtpStealthTable;
SPIN_LOCK	FtpStealthTableLock;

ULONG TcpKillCounter = 0;
ULONG UdpKillCounter = 0;
ULONG FtpKillCounter = 0;

ULONG StealthModeOn = FALSE;

VERDICT	StealthProcessingInPacket(PETH_HEADER pEthernetHeader)
{
	VERDICT	Verdict;
	
    if ( StealthModeOn == FALSE )
    {
        return Verdict_Pass;
    }
	if (isMulticast(pEthernetHeader))
	{		
		return Verdict_Pass;
	}

    if ( htons(pEthernetHeader->Type) == ETH_P_IP )
    {
        PIP_HEADER IpHeader = ( PIP_HEADER ) ( ( PCHAR ) pEthernetHeader + sizeof ( ETH_HEADER ) );

		if (IpHeader->Flg_FragOffs & 0xFF1F)
		{
			return Verdict_NotFiltered;
		}
    
        if   ( IpHeader -> Protocol == 1 )      //--------------- ICMP conditions ------------------------
        {		
            PICMP_HEADER IcmpHeader = (PICMP_HEADER) ((PCHAR)IpHeader +  IpHeader->Ihl * sizeof (ULONG) );
            
            if (IcmpHeader->Type == 8		||	// Echo Request
                IcmpHeader->Type == 13		||	// TimeStamp
                IcmpHeader->Type == 15)			// Information Request
            {
                return Verdict_Discard;
            }
        }
		//-----------------------------------------------------------------------------------------------
        else if   ( IpHeader -> Protocol == 6 ) //--------------- TCP conditions -------------------------
        {
            PTCP_HEADER TcpHeader  = (PTCP_HEADER)  ((PCHAR)IpHeader +  IpHeader->Ihl * sizeof (ULONG) );

			if (TcpHeader->srcPort == htons(20))
				Verdict = isCheckElement(&FtpStealthTable, &FtpStealthTableLock, IpHeader->srcIP, 0);
			else
				Verdict = isCheckElement(&TcpStealthTable, &TcpStealthTableLock, IpHeader->srcIP, TcpHeader->dstPort);

			if (Verdict == Verdict_Discard)
			{
//				DbgPrint ("Block TCP %x:%x\n", IpHeader->srcIP, TcpHeader->dstPort);
			}
            return Verdict;			
        }
		//-------------------------------------------------------------------------------------------------

        else if ( IpHeader -> Protocol == 0x11 ) //--------------- UDP conditions -------------------------
        {
			PUDP_HEADER	UdpHeader  = (PUDP_HEADER)((PCHAR)IpHeader +  IpHeader->Ihl * sizeof (ULONG) );
			
			Verdict = isCheckElement(&UdpStealthTable, &UdpStealthTableLock, IpHeader->srcIP, UdpHeader->dstPort);
			if (Verdict == Verdict_Discard)
			{
//				DbgPrint ("Block UDP %x:%x\n", IpHeader->srcIP, UdpHeader->dstPort);
			}		
			
            return Verdict;
        }

    }
    return Verdict_Pass;
}

VERDICT StealthProcessingOutPacket(PETH_HEADER pEthernetHeader)
{
    if (StealthModeOn && isMulticast(pEthernetHeader) == FALSE)
    {
        if (htons(pEthernetHeader->Type) == ETH_P_IP)    
        {
            PIP_HEADER IpHeader   = (PIP_HEADER)  ((PCHAR)pEthernetHeader + sizeof (ETH_HEADER)); 
            
            if ( IpHeader->Protocol == 0x6 )
            {
                PTCP_HEADER TcpHeader  = (PTCP_HEADER) ((PCHAR)IpHeader +  IpHeader->Ihl * sizeof (ULONG) );

				if (TcpHeader->syn && TcpHeader->ack == 0)
				{
					// локальный порт - удаленный IP
					RegisterStealthElement(IpHeader->dstIP, TcpHeader->srcPort, &TcpStealthTable, &TcpStealthTableLock,  &TcpKillCounter);

					// если это FTP, то зарегистрируем легальным и 20-й порт
					if (TcpHeader->dstPort == htons(21))            
					{
						// регистрируем, что на dstIP ушел FTP запрос.
						RegisterStealthElement(IpHeader->dstIP, 0, &FtpStealthTable, &FtpStealthTableLock, &FtpKillCounter);
						RegisterStealthElement(IpHeader->dstIP, htons(21), &TcpStealthTable, &TcpStealthTableLock,  &TcpKillCounter);
					}
				}
            }
			
            else if ( IpHeader->Protocol == 0x11 )
            {
				PUDP_HEADER	UdpHeader = (PUDP_HEADER)((PCHAR)IpHeader +  IpHeader->Ihl * sizeof (ULONG) );
                
				RegisterStealthElement(IpHeader->dstIP, UdpHeader->srcPort, &UdpStealthTable, &UdpStealthTableLock,  &UdpKillCounter);
				if (UdpHeader->srcPort == htons(138) ||
					UdpHeader->srcPort == htons(139) ||
					UdpHeader->srcPort == htons(137))
				{
					RegisterStealthElement(IpHeader->dstIP, htons(138), &UdpStealthTable, &UdpStealthTableLock,  &UdpKillCounter);
					RegisterStealthElement(IpHeader->dstIP, htons(139), &UdpStealthTable, &UdpStealthTableLock,  &UdpKillCounter);
					RegisterStealthElement(IpHeader->dstIP, htons(137), &UdpStealthTable, &UdpStealthTableLock,  &UdpKillCounter);
				}
            }
        }
    }
	return Verdict_Pass;
}
//-------------------------------------------------------------------------------------------------
NTSTATUS	InitializeStealthMode()
{
	InitializeListHead(&TcpStealthTable);
	AllocateSpinLock  (&TcpStealthTableLock);

	InitializeListHead(&UdpStealthTable);
	AllocateSpinLock  (&UdpStealthTableLock);

	InitializeListHead(&FtpStealthTable);
	AllocateSpinLock  (&FtpStealthTableLock);

	return STATUS_SUCCESS;
}

PSTEALTH_ELEMENT isStealthElementExist(PLIST_ENTRY	ListHead, ULONG	IpAddr, USHORT PortValue)
{
	PSTEALTH_ELEMENT StealthElement = (PSTEALTH_ELEMENT)ListHead->Flink;
    if (IsListEmpty(ListHead))
        return NULL;
    
    while( StealthElement != (PSTEALTH_ELEMENT)ListHead ) 
    {
        if (StealthElement->IpAddr == IpAddr && StealthElement->PortValue == PortValue)
        {            
            return  StealthElement;
        }
        StealthElement = (PSTEALTH_ELEMENT)StealthElement->Linkage.Flink;
    }
    return NULL;
}

VERDICT isCheckElement(PLIST_ENTRY	ListHead, SPIN_LOCK * pLock, ULONG	IpAddr, USHORT PortValue)
{
    PSTEALTH_ELEMENT pItem = isStealthElementExist ( ListHead, IpAddr, PortValue );

    if ( pItem )
    {
        KL_TIME CurrentTime;
        KlGetSystemTime(&CurrentTime);

        if (pItem->TimeStamp + ONE_SECOND_TIME * 120 < CurrentTime)
        {
			// Если в течение 120 сек. связи с хостом не было - считаем, что связь разорвана
            DeregisterStealthElement(pItem);			
        }
		else
		{
			// иначе будем поддерживать связь .
			pItem->TimeStamp = CurrentTime;
			return Verdict_Pass;
		}        
    }
    return Verdict_Discard;
}


PSTEALTH_ELEMENT    AllocateStealthElement(ULONG    IpAddr, USHORT PortValue)
{
    PSTEALTH_ELEMENT    Element;
    Element = ExAllocatePoolWithTag(NonPagedPool, sizeof (STEALTH_ELEMENT), 'slts');
    if (Element)
    {
        Element->IpAddr		= IpAddr;
		Element->PortValue	= PortValue;
        KlGetSystemTime(&Element->TimeStamp);
    }
    return Element;
}

VOID    FreeStealthElement(PSTEALTH_ELEMENT Element)
{
    ExFreePool(Element);
}

VOID RegisterStealthElement(ULONG IpAddr, USHORT PortValue, PLIST_ENTRY pListHead, SPIN_LOCK * pLock, ULONG * pKillCounter)
{
    PSTEALTH_ELEMENT    pElement;
    KL_TIME             CurrentTime;
    IRQL                Irql;
    
    KlGetSystemTime(&CurrentTime);
    
    if (*pKillCounter>5000)
    {
        PSTEALTH_ELEMENT pElement = (PSTEALTH_ELEMENT)pListHead->Flink;
        AcquireSpinLock(pLock, &Irql);
        
        while( pElement != (PSTEALTH_ELEMENT)pListHead ) 
        {                                    
            if (pElement->TimeStamp + ONE_SECOND_TIME * 120 < CurrentTime)
            {   
                PSTEALTH_ELEMENT pNextElement = (PSTEALTH_ELEMENT)pElement->Linkage.Flink;
                DeregisterStealthElement( pElement );
                pElement = pNextElement;                
                continue;
            }
            pElement = (PSTEALTH_ELEMENT)pElement->Linkage.Flink;
        }
        ReleaseSpinLock(pLock, Irql);
        *pKillCounter = 0;
    }
    AcquireSpinLock(pLock, &Irql);
    pElement = isStealthElementExist(pListHead, IpAddr, PortValue);
    ReleaseSpinLock(pLock, Irql);
    
    if (pElement)
    {
        pElement->TimeStamp = CurrentTime;
    }
    else
    {
        pElement = AllocateStealthElement(IpAddr, PortValue);
        
        if ( pElement )
        {
            AcquireSpinLock(pLock, &Irql);
            InsertTailList (pListHead, &pElement->Linkage);
            ReleaseSpinLock(pLock, Irql);
        }
    }
}

VOID RegisterTcpStealthElement(ULONG IpAddr, USHORT PortValue)
{
    RegisterStealthElement(IpAddr, PortValue, &TcpStealthTable, &TcpStealthTableLock, &TcpKillCounter);
}

VOID RegisterUdpStealthElement(ULONG IpAddr, USHORT PortValue)
{
    RegisterStealthElement(IpAddr, PortValue, &UdpStealthTable, &UdpStealthTableLock, &UdpKillCounter);
}

VOID DeregisterStealthElement(PSTEALTH_ELEMENT Element)
{
    RemoveEntryList(&Element->Linkage);
    FreeStealthElement(Element);
}