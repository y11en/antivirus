PTRUST_ELEMENT	CheckTrusted(ULONG	IpAddr, USHORT	PortValue)
{
	// просмотреть лист. Если элемент есть, то адрес
	PTRUST_ELEMENT	Element;

	if (IsListEmpty( &IdsTrustList ))
		return NULL;
	
	Element = (PTRUST_ELEMENT)IdsTrustList.Flink;
	while( Element != (PTRUST_ELEMENT)&IdsTrustList ) 	
	{
		if (Element->IpAddr == IpAddr && Element->PortValue == PortValue)
		{	
			return Element;
		}
		Element = (PTRUST_ELEMENT)Element->Linkage.Flink;
	}
	
	return NULL;
}

VOID	MakeTrusted(PETH_HEADER	EthernetHeader)
/*
	Данная функция предназначена только для OUTGOING пакетов.
	в этом случае DstIP - удаленный адрес а SrcPort - локальный порт.
*/
{
	PTRUST_ELEMENT	Element;

	if ( htons(EthernetHeader->Type) == ETH_P_IP)
	{
		PIP_HEADER	IpHeader = (PIP_HEADER)((PCHAR)EthernetHeader + sizeof (ETH_HEADER));
		if (IpHeader->Protocol == 0x11)
		{
			PUDP_HEADER	UdpHeader = (PUDP_HEADER) ((PCHAR)IpHeader +  IpHeader->Ihl * sizeof (ULONG) );

			Element = CheckTrusted(IpHeader->dstIP, UdpHeader->srcPort);
			if (Element == NULL)
			{
				Element = ExAllocatePoolWithTag(NonPagedPool, sizeof (TRUST_ELEMENT), 'lert');
				Element->IpAddr		= IpHeader->dstIP;
				Element->PortValue	= UdpHeader->srcPort;
				KlGetSystemTime(&Element->TimeStamp);
								
				InsertTailList(&IdsTrustList, &Element->Linkage);
                Element->Status = TRUST_ELEMENT_INSERTED;
			}
			else
			{
				KlGetSystemTime(&Element->TimeStamp);
			}
		}
	}

    Element = (PTRUST_ELEMENT)IdsTrustList.Flink;
    while( Element != (PTRUST_ELEMENT)&IdsTrustList ) 	
    {
        	
        if (Element->Status == TRUST_ELEMENT_MARKED_DELETE)
        {
            RemoveEntryList(&Element->Linkage);
            ExFreePool(Element);
            return;
        }
        
        Element = (PTRUST_ELEMENT)Element->Linkage.Flink;
    }
}

VOID	RefreshTrustedList()
{
	PTRUST_ELEMENT	Element;
	KL_TIME			CurrentTime;
	
	KlGetSystemTime(&CurrentTime);
	Element = (PTRUST_ELEMENT)IdsTrustList.Flink;
		
	Element = (PTRUST_ELEMENT)IdsTrustList.Flink;
	while( Element != (PTRUST_ELEMENT)&IdsTrustList ) 	
	{
		if (CurrentTime - Element->TimeStamp > DEFAULT_TRUSTED_LIST_REFRESH_TIME)
		{	
            Element->Status = TRUST_ELEMENT_MARKED_DELETE;			
			return;
		}
		Element = (PTRUST_ELEMENT)Element->Linkage.Flink;
	}
}