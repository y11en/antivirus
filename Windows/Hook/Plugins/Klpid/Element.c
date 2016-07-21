//-----------------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------
// –абота с портами 

// Allocate/Free STAT_PORT_ELEMENT  List
#ifdef _DEBUG
VOID	FreePortList(PLIST_ENTRY	ListHead)
{	
	PSTAT_PORT_ELEMENT	PortElement;
	
	while (!IsListEmpty(ListHead))
	{
		PortElement = (PSTAT_PORT_ELEMENT)RemoveHeadList(ListHead);
		ExFreePool(PortElement);
	}
}
#else if
#define FreePortList(_ListHead_)  { 									\
	PSTAT_PORT_ELEMENT	PortElement;									\
	while (!IsListEmpty(_ListHead_))									\
	{																	\
		PortElement = (PSTAT_PORT_ELEMENT)RemoveHeadList(_ListHead_);	\
		ExFreePool(PortElement);										\
	}																	\
}
#endif // _DEBUG

NTSTATUS	AllocatePortList(PLIST_ENTRY	ListHead, ULONG	ListSize)
{
	ULONG				i;
	PSTAT_PORT_ELEMENT	PortElement;
	
	for (i=0;i<ListSize;++i)
	{		
		PortElement = ExAllocatePoolWithTag(NonPagedPool, sizeof(STAT_PORT_ELEMENT), IDS_PORT_ELEMENT);
		if (PortElement == NULL)
		{            
			FreePortList(ListHead);
			return STATUS_UNSUCCESSFUL;
		}        
		InsertTailList (ListHead, &PortElement->Linkage);
	}
    
	return STATUS_SUCCESS;
}

// Allocate/Free STAT_FLOOD_ELEMENT  List
#ifdef _DEBUG
VOID	FreeFloodElementList(PLIST_ENTRY	ListHead)
{	
	PSTAT_FLOOD_ELEMENT	PortElement;
	
	while (!IsListEmpty(ListHead))
	{
		PortElement = (PSTAT_FLOOD_ELEMENT)RemoveHeadList(ListHead);
		ExFreePool(PortElement);
	}
}
#else if
#define FreeFloodElementList(_ListHead_)  { 							\
	PSTAT_FLOOD_ELEMENT	PortElement;									\
	while (!IsListEmpty(_ListHead_))									\
	{																	\
		PortElement = (PSTAT_FLOOD_ELEMENT)RemoveHeadList(_ListHead_);	\
		ExFreePool(PortElement);										\
	}																	\
}
#endif _DEBUG

NTSTATUS	AllocateFloodElementList(PLIST_ENTRY	ListHead, ULONG	ListSize)
{
	ULONG				i;
	PSTAT_FLOOD_ELEMENT	FloodElement;
	
	for (i=0;i<ListSize;++i)
	{		
		FloodElement = ExAllocatePoolWithTag(NonPagedPool, sizeof(STAT_FLOOD_ELEMENT), IDS_FLOOD_ELEMENT);
		if (FloodElement == NULL)
		{	
			FreeFloodElementList(ListHead);
			return STATUS_UNSUCCESSFUL;
		}
		InsertTailList (ListHead, &FloodElement->Linkage);
	}
    
	return STATUS_SUCCESS;
}

NTSTATUS	PlusAllocFloodElementList(PLIST_ENTRY	ListHead, ULONG	PlusAllocSize)
{
	ULONG				i;
	PSTAT_FLOOD_ELEMENT	FloodElement;
	
	for (i=0;i<PlusAllocSize;++i)
	{		
		FloodElement = ExAllocatePoolWithTag(NonPagedPool, sizeof(STAT_FLOOD_ELEMENT), IDS_FLOOD_ELEMENT);
		if (FloodElement == NULL)
		{			
			FreeFloodElementList(ListHead);
			return STATUS_UNSUCCESSFUL;
		}
		InsertTailList (ListHead, &FloodElement->Linkage);
	}
	return STATUS_SUCCESS;
}
//---------------

#ifdef	_DEBUG
VOID	RefreshFloodList(PLIST_ENTRY	ListHead, PSTAT_ELEMENT	Element,PKL_TIME	pCurrentTime, ULONG	TimeBarrier)
{
	PSTAT_FLOOD_ELEMENT	TempFloodElement, 
						FloodElement = (PSTAT_FLOOD_ELEMENT)ListHead->Flink;	

	while( !IsListEmpty( ListHead ) && FloodElement != (PSTAT_FLOOD_ELEMENT)ListHead ) 
	{
		if (*pCurrentTime - FloodElement->TimeStamp > TimeBarrier * ONE_SECOND_TIME )		
		{
			TempFloodElement = (PSTAT_FLOOD_ELEMENT)FloodElement->Linkage.Flink;
			RemoveEntryList(&FloodElement->Linkage);
			RtlZeroMemory(FloodElement, sizeof (PSTAT_FLOOD_ELEMENT));			
			InsertTailList (&GlobalStore.Attacks.FloodProtectList, &FloodElement->Linkage);
			FloodElement = TempFloodElement;
			continue;
		}
		FloodElement = (PSTAT_FLOOD_ELEMENT)FloodElement->Linkage.Flink;
	}
}
#else if
#define RefreshFloodList(_ListHead_, _Element_, _pCurrentTime_, _TimeBarrier_)	{				\
	PSTAT_FLOOD_ELEMENT	TempFloodElement,														\
	FloodElement = (PSTAT_FLOOD_ELEMENT)(_ListHead_)->Flink;									\
	while( !IsListEmpty( _ListHead_ ) && FloodElement != (PSTAT_FLOOD_ELEMENT)(_ListHead_) )		\
	{																							\
		if (*(_pCurrentTime_) - FloodElement->TimeStamp > (_TimeBarrier_) * ONE_SECOND_TIME  ) {	\
			TempFloodElement = (PSTAT_FLOOD_ELEMENT)FloodElement->Linkage.Flink;				\
			RemoveEntryList(&FloodElement->Linkage);											\
			RtlZeroMemory(FloodElement, sizeof (PSTAT_FLOOD_ELEMENT));							\
			InsertTailList (&GlobalStore.Attacks.FloodProtectList, &FloodElement->Linkage);					\
			FloodElement = TempFloodElement;													\
			continue;																			\
		}																						\
		FloodElement = (PSTAT_FLOOD_ELEMENT)FloodElement->Linkage.Flink;						\
	}																							\
}
#endif // _DEBUG

#ifdef _DEBUG
VOID	RefreshPortCount(PLIST_ENTRY ListHead, PLIST_ENTRY FreeListHead, PSTAT_ELEMENT Element, PKL_TIME pCurrentTime, ULONG	TimeBarrier)
{
	PSTAT_PORT_ELEMENT	TempPortElement,
						PortElement = (PSTAT_PORT_ELEMENT)ListHead->Flink;	

	while( !IsListEmpty( ListHead ) && PortElement != (PSTAT_PORT_ELEMENT)ListHead ) 
	{
		if (*pCurrentTime - PortElement->TimeStamp > TimeBarrier * ONE_SECOND_TIME )		
		{
			TempPortElement = (PSTAT_PORT_ELEMENT)PortElement->Linkage.Flink;
			RemoveEntryList(&PortElement->Linkage);
			RtlZeroMemory(PortElement, sizeof (PSTAT_PORT_ELEMENT));			
			InsertTailList (FreeListHead, &PortElement->Linkage);
			PortElement = TempPortElement;
			continue;
		}
		PortElement = (PSTAT_PORT_ELEMENT)PortElement->Linkage.Flink;
	}
}
#else if
#define RefreshPortCount(_ListHead_, _FreeListHead_, _Element_, _pCurrentTime_, _TimeBarrier_)	{				\
	PSTAT_PORT_ELEMENT	TempPortElement,														\
	PortElement = (PSTAT_PORT_ELEMENT)(_ListHead_)->Flink;										\
	while( !IsListEmpty( (_ListHead_) ) && PortElement != (PSTAT_PORT_ELEMENT)(_ListHead_) )	\
	{																							\
		if (*(_pCurrentTime_) - PortElement->TimeStamp > (_TimeBarrier_) * ONE_SECOND_TIME )	\
		{																						\
			TempPortElement = (PSTAT_PORT_ELEMENT)PortElement->Linkage.Flink;					\
			RemoveEntryList(&PortElement->Linkage);												\
			RtlZeroMemory(PortElement, sizeof (PSTAT_PORT_ELEMENT));							\
			InsertTailList (_FreeListHead_, &PortElement->Linkage);					\
			PortElement = TempPortElement;														\
			continue;																			\
		}																						\
		PortElement = (PSTAT_PORT_ELEMENT)PortElement->Linkage.Flink;							\
	}																							\
}
#endif //_DEBUG


ULONG
GetListCount(PLIST_ENTRY	ListHead)
{
	PLIST_ENTRY	Entry = ListHead->Flink;
	ULONG		Count = 0;
	
	if (IsListEmpty (ListHead))
		return 0;
	
	while( Entry != ListHead ) 
	{
		++Count;
		Entry = Entry->Flink;
	}
	return Count;
}

PSTAT_PORT_ELEMENT
isPortExist(PLIST_ENTRY	ListHead, USHORT	PortValue)
{
	PSTAT_PORT_ELEMENT	PortElement = (PSTAT_PORT_ELEMENT)ListHead->Flink;
	
	if (IsListEmpty (ListHead))
		return NULL;
	
	while( PortElement != (PSTAT_PORT_ELEMENT)ListHead ) 
	{
		if (PortElement->PortValue == PortValue)
			return PortElement;
		PortElement = (PSTAT_PORT_ELEMENT)PortElement->Linkage.Flink;
	}
	return NULL;
}

NTSTATUS	PlusAllocPorts(PSTAT_ELEMENT	Element, PLIST_ENTRY	SourceListHead, UCHAR	Protocol, ULONG	Count)
{
	PSTAT_PORT_ELEMENT	PortElement;
	ULONG				i;
	
	for ( i = Count; i>0; --i )
	{	
		PortElement = ExAllocatePoolWithTag(NonPagedPool, sizeof(STAT_PORT_ELEMENT), IDS_PORT_ELEMENT);
		
		if ( PortElement )
		{	
			RtlZeroMemory (PortElement, sizeof(STAT_PORT_ELEMENT));
			InsertTailList (SourceListHead, &PortElement->Linkage);		
		}		
		else
			return STATUS_UNSUCCESSFUL;
	}
	return STATUS_SUCCESS;
}

BOOLEAN	
RegisterPort(PSTAT_ELEMENT	Element,			// — какого IP пришел пакет
			 PLIST_ENTRY	ListHead,			// Ћист портов, которые под подозрением
			 PLIST_ENTRY	SourceListHead,		// "свободна€ пам€ть, дл€ ускорени€ работы
			 USHORT			LocalPort,			// порт, на который пришел пакет
			 UCHAR			Protocol)			// протокол
{
	ULONG				i=0;
	PSTAT_PORT_ELEMENT	PortElement;

	PortElement = isPortExist(ListHead, LocalPort);
	if ( PortElement )
	{	// ≈сли порт существует, просто обновим статистику
		KlGetSystemTime(&PortElement->TimeStamp); 		
		return TRUE;
	}
	else
	{		
		if (IsListEmpty(SourceListHead))
		{	// »сточник FloodElement-ов пуст. => Ќевозможно зарегистрировать порт 
			// ¬ыделим дополнительно порт, а проверка на сканирование портов позднее покажет, что это было ...
			if (STATUS_SUCCESS != PlusAllocPorts(Element, SourceListHead, Protocol, 1))
				return FALSE;
		}

		PortElement = (PSTAT_PORT_ELEMENT)RemoveHeadList(SourceListHead);
		ASSERT(PortElement);
		
		PortElement->PortValue = LocalPort;
		PortElement->Protocol  = Protocol;
		KlGetSystemTime(&PortElement->TimeStamp);
		InsertTailList(ListHead, (PLIST_ENTRY)PortElement);
		return TRUE;
	}

	return FALSE;
}

//---------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------

// работа со STAT_ELEMENT-ами
PSTAT_ELEMENT	RegisterElement(ULONG	IpAddr, ULONG	TcpPortListSize, ULONG	UdpPortListSize, ULONG	FloodListCount)
/*
	«арегистрировать хост. ≈сли невозможно зарегистрировать новый хост - вернуть NULL
*/
{
	PSTAT_ELEMENT	Element;
	NTSTATUS		ntStatus;

	// возьмем свободный элемент из списка заранее выделенных структур.
	
	if (IsListEmpty(&GlobalStore.AttacksStorage.ProtectList))
	{
		return NULL;
	}
	Element = (PSTAT_ELEMENT)RemoveHeadList(&GlobalStore.AttacksStorage.ProtectList);	
	ASSERT(Element);
	RtlZeroMemory (Element, sizeof (STAT_ELEMENT));	
	
	InitializeListHead(&Element->TcpPortList);
	InitializeListHead(&Element->UdpPortList);
	
	InitializeListHead(&Element->TcpPortListFree);
	InitializeListHead(&Element->UdpPortListFree);

	InitializeListHead(&Element->SynList);
	InitializeListHead(&Element->UdpList);
	InitializeListHead(&Element->IcmpList);	
//	InitializeListHead(&Element->FloodList);
	
	InitializeListHead(&Element->BanReasonList);
	
	Element->IpAddr			= IpAddr;
	Element->TcpPortCount	= TcpPortListSize;
	Element->UdpPortCount	= UdpPortListSize;

	ntStatus  = AllocatePortList(&Element->TcpPortListFree, 2);	
	ntStatus |= AllocatePortList(&Element->UdpPortListFree, 2);
//	ntStatus |= AllocateFloodElementList(&Element->FloodList, FloodListCount);	

	if (ntStatus != STATUS_SUCCESS)
	{
		FreePortList(&Element->TcpPortListFree);
		FreePortList(&Element->UdpPortListFree);
//		FreeFloodElementList(&Element->FloodList);
		InsertHeadList(&GlobalStore.AttacksStorage.ProtectList, &Element->Linkage);
		return NULL;
	}
	
	KlGetSystemTime(&Element->StartTime);
	KlGetSystemTime(&Element->TimeStamp);

	Element->isRegistered = TRUE;
	
	// «анесем в список, чтоб потом провер€ть этот хост
	++GlobalStore.Attacks.ProtectListCount;
	InsertHeadList(&(GlobalStore.Attacks.ProtectList), &Element->Linkage);
	
	return Element;
}

VOID	DeregisterElement(PSTAT_ELEMENT		Element)
{
	RemoveEntryList(&Element->Linkage);
	
	FreePortList(&Element->TcpPortList);
	FreePortList(&Element->TcpPortListFree);
	FreePortList(&Element->UdpPortList);
	FreePortList(&Element->UdpPortListFree);
	
//	FreeFloodElementList(&Element->FloodList);	
	
	RtlZeroMemory (Element, sizeof (STAT_ELEMENT));
	--GlobalStore.Attacks.ProtectListCount;
	InsertTailList(&GlobalStore.AttacksStorage.ProtectList, &Element->Linkage);		
}

VOID	DeregisterAllElements()
{
	PLIST_ENTRY		ListHead = &GlobalStore.Attacks.ProtectList;	
	PSTAT_ELEMENT	Element;
	while (!IsListEmpty(ListHead))
	{
		Element = (PSTAT_ELEMENT)RemoveHeadList(ListHead);
		DeregisterElement(Element);
	}
}

PSTAT_ELEMENT	
isElementExist(PLIST_ENTRY	ListHead, ULONG	Ip)
{
	PSTAT_ELEMENT	Element = (PSTAT_ELEMENT)ListHead->Flink;
	if (IsListEmpty (ListHead))
		return NULL;
	
	while( Element != (PSTAT_ELEMENT)ListHead ) 
	{
		if (Element->IpAddr == Ip)
			return Element;
		Element = (PSTAT_ELEMENT)Element->Linkage.Flink;
	}
	return NULL;
}

BOOLEAN
CheckReason(PSTAT_ELEMENT Element, ULONG Reason)
/*
	≈сли нас уже атаковали по причине Reason, то вернуть TRUE. иначе FALSE
*/
{
	PBAN_REASON		BanReason;

	// ≈сли бан-лист пустой, то Reason - отсутствует
	if (IsListEmpty(&Element->BanReasonList))
		return FALSE;

	BanReason = (PBAN_REASON)Element->BanReasonList.Flink;

	while( BanReason != (PBAN_REASON)&Element->BanReasonList ) 
	{
		if (BanReason->Reason == Reason)
		{
			return TRUE;
		}
		BanReason = (PBAN_REASON)BanReason->Linkage.Flink;
	}		
	
	return FALSE;
}

//#define isNotBanned(_Element_) IsListEmpty(&(_Element_)->BanReasonList)


BOOLEAN	
isNotBanned(PSTAT_ELEMENT Element)
{
	// ≈сли лист пуст, то элемент не забанен
//	return IsListEmpty(&Element->BanReasonList);

	if (IsListEmpty(&Element->BanReasonList))
	{
		// ≈сли лист пустой, то мы не забанены ...
		return TRUE;
	}
	
	return FALSE;
}

VOID
RefreshElement(PSTAT_ELEMENT	Element)
/*
	≈сли нас не атакуют и вообще нет никаких известий от хоста, то его можно удалить и 
	не засор€ть очередь.
*/
{	
	PBAN_REASON		BanReason;
	KL_TIME			CurrentTime;		
	KlGetSystemTime(&CurrentTime);
	
	// ≈сли Ёлемент не забанен, то ничего с ним делать на надо.
	if (!isNotBanned(Element))
	{	
		// элемент забанен.		

		// ѕереберем все причины, по которым Ёлемент забанен
		BanReason = (PBAN_REASON)Element->BanReasonList.Flink;
		while( !IsListEmpty( &Element->BanReasonList ) && BanReason != (PBAN_REASON)&Element->BanReasonList ) 
		{
			if (CurrentTime > BanReason->StopTime)
			{   // ѕричина по которой элемент был забанен устранилась ! 
				PBAN_REASON		TempBanReason;
				
				TempBanReason = (PBAN_REASON)BanReason->Linkage.Flink;
				RemoveEntryList(&BanReason->Linkage);				
				ExFreePool (BanReason);
				BanReason = TempBanReason;
				continue;
			}
			BanReason = (PBAN_REASON)BanReason->Linkage.Flink;
		}
		return;
	}
	
	// Ёлемент не забанен	
	if (isNotBanned(Element))
	{
		RefreshFloodList(&Element->SynList, Element, &CurrentTime, AttackOptions.SynFloodTime);
		RefreshFloodList(&Element->UdpList, Element, &CurrentTime, AttackOptions.UdpFloodTime);
		RefreshFloodList(&Element->IcmpList,Element, &CurrentTime, AttackOptions.IcmpFloodTime);

		RefreshPortCount(&Element->TcpPortList, &Element->TcpPortListFree, Element, &CurrentTime, AttackOptions.TcpRefreshTime);
		RefreshPortCount(&Element->UdpPortList, &Element->UdpPortListFree, Element, &CurrentTime, AttackOptions.UdpRefreshTime);

		if (IsListEmpty(&Element->TcpPortList) &&
			IsListEmpty(&Element->UdpPortList) &&		
			IsListEmpty(&Element->IcmpList))
			
		{
			// нас не атакуют и ваще никаких известий от хоста ... убьем 
			if (CurrentTime - Element->TimeStamp > DEFAULT_REFRESH_TIME * (KL_TIME)ONE_SECOND_TIME)
				DeregisterElement(Element);
		}
	}
}

VOID	RefreshAllElements()
{
	PSTAT_ELEMENT	Element, TempElement;
	Element = (PSTAT_ELEMENT)GlobalStore.Attacks.ProtectList.Flink;

	while( !IsListEmpty( &GlobalStore.Attacks.ProtectList )					&& // пока лист непуст (мы можем из него что-то удал€ть !!!)
			Element != (PSTAT_ELEMENT)&GlobalStore.Attacks.ProtectList )  // и пока мы не вернемс€ к голове списка
	{
		TempElement = (PSTAT_ELEMENT)Element->Linkage.Flink;
		RefreshElement(Element);
		Element = TempElement;
	}
}

VOID	RefreshSomeElements(ULONG	Count)
{
	ULONG			i = Count;
	PSTAT_ELEMENT	TempElement, Element = (PSTAT_ELEMENT)GlobalStore.Attacks.ProtectList.Flink;
	
	while( !IsListEmpty( &GlobalStore.Attacks.ProtectList )					&& // пока лист непуст (мы можем из него что-то удал€ть !!!)
		Element != (PSTAT_ELEMENT)&GlobalStore.Attacks.ProtectList		    && // и пока мы не вернемс€ к голове списка
		i													)				   // i != 0   будем перебирать Count или меньше элементов

	{
		TempElement = (PSTAT_ELEMENT)Element->Linkage.Flink;
		RefreshElement(Element);
		Element = TempElement;
		--i;
	}
}

VOID	BanElement(PSTAT_ELEMENT	Element, ULONG	TimeToBan, ULONG	Reason)
// time in seconds
// Ќужно забанить элемент по указанной причине
{	
	PBAN_REASON		BanReason;	

	// ≈сли BanTime == банить не будем.
	if (TimeToBan == 0)
		return;

	if (CheckReason(Element, Reason))
	{
		// ≈сли элемент уже забанен, то просто обновим статистику забанненности ...
		BanReason = (PBAN_REASON)Element->BanReasonList.Flink;
		while( BanReason != (PBAN_REASON)&Element->BanReasonList ) 
		{
			if (BanReason->Reason == Reason)
			{
				
				KlGetSystemTime(&BanReason->StartTime);
				BanReason->StopTime = BanReason->StartTime + (KL_TIME) (ONE_SECOND_TIME * TimeToBan);
				return;
			}
			BanReason = (PBAN_REASON)BanReason->Linkage.Flink;
		}
	}

	BanReason = ExAllocatePoolWithTag (NonPagedPool, sizeof (BAN_REASON), IDS_BAN_REASON);
	if (BanReason == NULL)
	{		
		return;
	}
	
	KlGetSystemTime(&BanReason->StartTime);	
	BanReason->StopTime = BanReason->StartTime + (ONE_SECOND_TIME * (KL_TIME)TimeToBan);
	BanReason->Reason	= Reason;	
	InsertTailList(&Element->BanReasonList, &BanReason->Linkage);	
}
