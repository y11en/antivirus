ULONG    PlusAllocFreeFloodList();

BOOLEAN	CheckFlood(PSTAT_ELEMENT	Element, PLIST_ENTRY	ListHead, ULONG	FloodTime, ULONG	FloodCount)
{
	KL_TIME				CurrentTime;
	ULONG				CurrentFloodCount = 0;
	PSTAT_FLOOD_ELEMENT	FloodElement, TempFloodElement;

	// Добавить статистику ...	
	KlGetSystemTime(&CurrentTime);

	// добавим очередной Флуд-элемент в соответствующий лист ...
    if (IsListEmpty(&GlobalStore.Attacks.FloodProtectList))
    {
        if (PlusAllocFreeFloodList() == 0)
        {
            return TRUE;
        }
    }
	FloodElement = (PSTAT_FLOOD_ELEMENT)RemoveHeadList(&GlobalStore.Attacks.FloodProtectList);	
	FloodElement->TimeStamp = CurrentTime;	
	InsertTailList(ListHead, &FloodElement->Linkage);

	FloodElement = (PSTAT_FLOOD_ELEMENT)ListHead->Flink;
	// проверка статистики.
	while( !IsListEmpty( ListHead ) && FloodElement != (PSTAT_FLOOD_ELEMENT)ListHead ) 
	{
		if ( CurrentTime - FloodElement->TimeStamp < ONE_SECOND_TIME * FloodTime )
		{
			++CurrentFloodCount;
		}
		else
		{
			TempFloodElement = (PSTAT_FLOOD_ELEMENT)FloodElement->Linkage.Flink;
			
			RemoveEntryList(&FloodElement->Linkage);
			RtlZeroMemory(FloodElement, sizeof (PSTAT_FLOOD_ELEMENT));			
			InsertTailList (&GlobalStore.Attacks.FloodProtectList, &FloodElement->Linkage);
			
			FloodElement	 = TempFloodElement;
			continue;
		}
		FloodElement = (PSTAT_FLOOD_ELEMENT)FloodElement->Linkage.Flink;
	}
		
	if (CurrentFloodCount >= FloodCount)
	{	// в листе оказалось больше пакетов, чем должно быть - это атака.
		return TRUE;
	}

	return FALSE;
}

// Функция возвратит, сколько элементов дополнительны выделилось
ULONG    PlusAllocFreeFloodList()
{
    ULONG i;
    PSTAT_FLOOD_ELEMENT FloodElement;
    
    for (i=0; i<1000; ++i)
    {
        FloodElement = ExAllocatePoolWithTag(NonPagedPool, sizeof(STAT_FLOOD_ELEMENT), IDS_FLOOD_ELEMENT);
        if (FloodElement == NULL)
        {            
            break;
        }        
        InsertTailList (&GlobalStore.Attacks.FloodProtectList, &FloodElement->Linkage);
    }
    
    GlobalStore.Attacks.FloodProtectListCount += i;
    return i;
}
