#ifndef ISWIN9X

VOID	DelayedNotify(PNDIS_WORK_ITEM	pWorkItem, PNOTIFY_ATTACK_CONTEXT	Context)
{
	if (FilterEventExt) 
	{			
		ULONG				ReqDataSize;
		PSINGLE_PARAM		pSingleParam;
		PFILTER_EVENT_PARAM pParam;
		PVOID	RequestData;	
		ULONG	BytesRet = 0;				
		
		ReqDataSize = sizeof(FILTER_EVENT_PARAM);
		ReqDataSize += sizeof (SINGLE_PARAM) + sizeof (ULONG);   // sizeof(AttackId)
		
		switch (Context->AttackID)
		{
		case ATTACK_PING_OF_DEATH_ID:			
		case ATTACK_SYN_FLOOD_ID:							
		case ATTACK_UDP_FLOOD_ID:			
		case ATTACK_ADDRESS_SCAN:			
		case ATTACK_TCP_SCAN:			
		case ATTACK_UDP_SCAN:			
		case ATTACK_ICMP_FLOOD:
		case ATTACK_SMBDIE_ID:
		case ATTACK_HELKERN_ID:
        case ATTACK_LOVESAN_ID:
			ReqDataSize += sizeof (SINGLE_PARAM) + sizeof(ULONG); // SRC_ADDRESS
			break;
		}
		
		ReqDataSize += sizeof (SINGLE_PARAM) + __SID_LENGTH;	
		
		RequestData = ExAllocatePoolWithTag(NonPagedPool, ReqDataSize, COMMON_TAG);
		
		pParam = (PFILTER_EVENT_PARAM)RequestData;
		if (pParam == NULL) {
			return;
		} 
		
		INITIALIZE_PARAM(pParam, MJ_NET_ATTACK, "System", FLTTYPE_IDS);
		
		ADD_ULONG_PARAM(pSingleParam, pParam, ID_NET_ATTACK, Context->AttackID );
		
		switch (Context->AttackID)
		{
		case ATTACK_LAND_ID:
			
			break;
		case ATTACK_PING_OF_DEATH_ID:			
		case ATTACK_SYN_FLOOD_ID:							
		case ATTACK_UDP_FLOOD_ID:			
		case ATTACK_ADDRESS_SCAN:			
		case ATTACK_TCP_SCAN:
		case ATTACK_UDP_SCAN:			
		case ATTACK_ICMP_FLOOD:
		case ATTACK_SMBDIE_ID:
		case ATTACK_HELKERN_ID:
        case ATTACK_LOVESAN_ID:			
			ADD_ULONG_PARAM(pSingleParam, pParam, ID_IP_SRC_ADDRESS, htonl(Context->AttackerIp ));				
			break;
		}
		
		FilterEventExt(pParam, NULL);				
		ExFreePool(RequestData);		
	}
    
	ExInterlockedInsertTailList(&GlobalStore.Attacks.NotifyAttackList, &Context->Linkage, &GlobalStore.Attacks.NotifyAttackListLock);
}

VOID	NotifyAttack(ULONG	AttackID, ULONG	AttackerIp, KL_TIME*	LastDetectedTime)
{
	KL_TIME					CurrentTime;
	PNOTIFY_ATTACK_CONTEXT	Context;
	NTSTATUS				Status;

	KlGetSystemTime(&CurrentTime);

	if (CurrentTime < HALF_SECOND_TIME + *LastDetectedTime)
		return;

	*LastDetectedTime = CurrentTime;

	Context = (PNOTIFY_ATTACK_CONTEXT)ExInterlockedRemoveHeadList(&GlobalStore.Attacks.NotifyAttackList, 
																	&GlobalStore.Attacks.NotifyAttackListLock);
	if (Context)
	{	
		NdisInitializeWorkItem(&Context->WorkQueueItem, DelayedNotify, Context);
		Context->AttackID	= AttackID;
		Context->AttackerIp = AttackerIp;
		Status = NdisScheduleWorkItem (&Context->WorkQueueItem);
	}
}
#else // isWIN9x

VOID	NotifyAttack(ULONG	AttackID, ULONG	AttackerIp, KL_TIME*	LastDetectedTime)
{
	KL_TIME					CurrentTime;
	KlGetSystemTime(&CurrentTime);
	
	if (CurrentTime < HALF_SECOND_TIME + *LastDetectedTime)
		return;

	*LastDetectedTime = CurrentTime;

	if (FilterEventExt) 
	{			
		ULONG				ReqDataSize;
		PSINGLE_PARAM		pSingleParam;
		PFILTER_EVENT_PARAM pParam;
		PVOID				RequestData;	
		ULONG				BytesRet = 0;				
		
		ReqDataSize = sizeof(FILTER_EVENT_PARAM);
		ReqDataSize += sizeof (SINGLE_PARAM) + sizeof (ULONG);   // sizeof(AttackId)
		
		switch (AttackID)
		{
		case ATTACK_PING_OF_DEATH_ID:			
		case ATTACK_SYN_FLOOD_ID:							
		case ATTACK_UDP_FLOOD_ID:			
		case ATTACK_ADDRESS_SCAN:			
		case ATTACK_TCP_SCAN:			
		case ATTACK_UDP_SCAN:			
		case ATTACK_ICMP_FLOOD:
		case ATTACK_SMBDIE_ID:
		case ATTACK_HELKERN_ID:
        case ATTACK_LOVESAN_ID:            
			ReqDataSize += sizeof (SINGLE_PARAM) + sizeof(ULONG); // SRC_ADDRESS
			break;
		}
		
		ReqDataSize += sizeof (SINGLE_PARAM) + __SID_LENGTH;	
		
		RequestData = ExAllocatePoolWithTag(NonPagedPool, ReqDataSize, COMMON_TAG);
		
		pParam = (PFILTER_EVENT_PARAM)RequestData;
		if (pParam == NULL) {
			return;
		} 
		
		INITIALIZE_PARAM(pParam, MJ_NET_ATTACK, "System", FLTTYPE_IDS);
		
		ADD_ULONG_PARAM(pSingleParam, pParam, ID_NET_ATTACK, AttackID );
		
		switch (AttackID)
		{
		case ATTACK_LAND_ID:
			
			break;
		case ATTACK_PING_OF_DEATH_ID:			
		case ATTACK_SYN_FLOOD_ID:							
		case ATTACK_UDP_FLOOD_ID:			
		case ATTACK_ADDRESS_SCAN:			
		case ATTACK_TCP_SCAN:
		case ATTACK_UDP_SCAN:			
		case ATTACK_ICMP_FLOOD:
		case ATTACK_SMBDIE_ID:
		case ATTACK_HELKERN_ID:
        case ATTACK_LOVESAN_ID:			
			ADD_ULONG_PARAM(pSingleParam, pParam, ID_IP_SRC_ADDRESS, htonl(AttackerIp ));				
			break;
		}
		
		
		FilterEventExt(pParam, NULL);				
		ExFreePool(RequestData);		
	}
}


#endif // isWIN9x


//-----------------------------------------------------------------------------------------------