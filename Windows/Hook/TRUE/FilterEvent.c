#include "StdAfx.h"
#include "ParseTables.h"
#include "ErrCodes.h"
#include <Hook/HookSpec.h>
#include "HOOK\PIDExternalAPI.H"

ULONG FindNextKey(LPTraversalState pTS){
	ULONG	i = 0;
	if( (i = pTS->dwIndex) > pTS->Table[0].dwKEY )
		return 0;

	while(	( i <= pTS->Table[0].dwKEY				  )	&& 
			( pTS->dwLookUpKey != pTS->Table[i].dwKEY )	&& 
			( pTS->dwLookUpKey						  )	&& 
			( pTS->Table[i].dwKEY					  ) )
		i++ ;

	if( i > pTS->Table[0].dwKEY)
		return 0;
	else
		return i;
}

VERDICT __stdcall FilterEvent(LPNetInfoHeader pNetInfoHeader)
{
	CHAR ExchangeArea[MAX_EXCHANGE_AREA_SIZE + sizeof(FILTER_EVENT_PARAM)];
	
	TraversalState state =  {
								{	pNetInfoHeader->m_lpPacket, 
									pNetInfoHeader->m_dwPacketSize
								},

								{
									(PCHAR)ExchangeArea+sizeof(FILTER_EVENT_PARAM), 
									MAX_EXCHANGE_AREA_SIZE
								},
								ROOT,
								1,
								pNetInfoHeader->m_dwMediaType,
								error
							};
	TraversalState		store;
	ParseShuttle		ps;
	PFILTER_EVENT_PARAM pTransmit = (PFILTER_EVENT_PARAM)ExchangeArea;
	ULONG				index;
	PARSE_REPLY			ret;
	ULONG				dwDeep = 0;
	ULONG				dwSteps = 0;
	
/*
	if(!IsNeedFiltering(FLTTYPE_PID, pNetInfoHeader->m_wDirection, 0))
	{
		return Verdict_NotFiltered;
	}
*/
	
	RtlZeroMemory(ExchangeArea, MAX_EXCHANGE_AREA_SIZE + sizeof(FILTER_EVENT_PARAM));

	pTransmit->ParamsCount	= 0;
	ps.pEventTransmit		= pTransmit;
	ps.pIH					= pNetInfoHeader;
	Message(DL_INFO, DM_UNWIND, ("TRUE: ---------------Start parse...-------------\n"));
	
	while(  ( dwDeep < MAX_ENCAPSULATE_LEVEL ) && ( dwSteps < MAX_RECOGNIZE_STEPS )  )
	{
		index = FindNextKey(&state);
		dwSteps++;
		if ( index )
		{
			state.dwIndex		= index;
			ps.Frame			= state.Frame;
			ps.Buff				= state.Buff;
			ps.dwError			= 0;
			ps.dwKey			= 0;
			ps.dwParamsCount	= 0;

			Message(DL_FULL_INFO, DM_UNWIND, ("TRUE: Find table key %#x\n", index));

			if ( state.Table[state.dwIndex].fParse )
				ret = state.Table[state.dwIndex].fParse(&ps);
			else
				ret = maybe;

			switch( ret )
			{
			case ok:
					//first we change state, then synchronize (save new state)
					store.Frame				= state.Frame				= ps.Frame;
					store.Buff				= state.Buff				= ps.Buff;
					store.dwLookUpKey		= state.dwLookUpKey			= ps.dwKey;
					store.LastRet			= state.LastRet				= ok;
					store.dwLastParamsCount = state.dwLastParamsCount	= ps.dwParamsCount;
					

					//FILTER_EVENT params counter
					pTransmit->ParamsCount += ps.dwParamsCount;
					 
					//go to next table or end of parse
					if( state.Table[state.dwIndex].pNextEncapsulate )
					{
						state.Table = state.Table[state.dwIndex].pNextEncapsulate;
						state.dwIndex = 1;
						dwDeep++;
						Message(DL_INFO, DM_UNWIND, ("TRUE: Parse success with \"ok\", go to next table %#x...\n", state.Table));
					}
					else
					{
						Message(DL_INFO, DM_UNWIND, ("TRUE: Parse end.\n"));
						goto END_PARSE;
					}
					break;
			case maybe: 
						//state synchronize - save current state
						store = state;

						//state changes
						state.Frame				= ps.Frame;
						state.Buff				= ps.Buff;
						state.dwLookUpKey		= ps.dwKey;
						state.LastRet			= maybe;
						state.dwLastParamsCount = ps.dwParamsCount;

						//FILTER_EVENT params counter
						pTransmit->ParamsCount+=ps.dwParamsCount;

						//go to next table or end of parse
						if  ( state.Table[state.dwIndex].pNextEncapsulate )
						{						
							state.Table = state.Table[state.dwIndex].pNextEncapsulate;
							state.dwIndex = 1;
							dwDeep++;
							Message(DL_INFO, DM_UNWIND, ("TRUE: Parse success with \"maybe\", go to next table %#x\n", state.Table));
						}
						else
						{
							Message(DL_INFO, DM_UNWIND, ("TRUE: Parse end.\n"));
							goto END_PARSE;
						}
						break;
			case wrong: 
				state.dwIndex++; 
				Message(DL_PARSE_MISS, DM_UNWIND, ("TRUE: Parse miss with \"wrong\" for table %#x, rollback...\n", state.Table));
				break;
			case error: 
				Message(DL_ERROR, DM_UNWIND, ("TRUE: Parse error occured. Go to end.\n"));
				state.dwLookUpKey = Verdict_NotFiltered;
				goto END_PARSE;
			}
		}
		else
		{
			Message(DL_INFO, DM_UNWIND, ("TRUE: Cannot find key...\n"));
			if ( state.Table[0].pNextEncapsulate )
			{
				Message(DL_INFO, DM_UNWIND, ("TRUE: Go to default table %#x for current table %#x\n", state.Table[0].pNextEncapsulate, state.Table));
				state.Table = state.Table[0].pNextEncapsulate;
				state.dwIndex = 1;
				dwDeep++;
			}
			else
			{

				switch( state.LastRet )
				{
					case ok: 
						Message(DL_BRIEF, DM_UNWIND, ("TRUE: Parse end with success, ret code: %d", state.dwLookUpKey));
						goto END_PARSE;
						
					case maybe: 
						Message(DL_PARSE_MISS, DM_UNWIND, ("TRUE: Trying to find more fit protocol (\"maybe\") and rollback params...\n"));
						pTransmit->ParamsCount-=state.dwLastParamsCount;
						store.LastRet = error;
						store.dwIndex++;
						state = store;
						dwDeep--; break;
					case wrong:
					case error:
						Message(DL_ERROR, DM_UNWIND, ("TRUE: Parse error occured. Go to end.\n"));
						state.dwLookUpKey = Verdict_NotFiltered;
						goto END_PARSE;
				}
			}
		}
	}
	Message(DL_PARSE_MISS, DM_UNWIND, ("TRUE: Cannot parse this packet(Steps:%d, Deep:%d)...Discard", dwSteps, dwDeep));
	pNetInfoHeader->m_dwAction  = NET_IMPLEMENTATION | __LINE__;
	return Verdict_Discard;

END_PARSE:	
			pNetInfoHeader->m_dwAction  = NET_NOTIFICATION | __LINE__;
			Message(DL_BRIEF, DM_UNWIND, ("TRUE: END_PARSE Verdict...%d\n", state.dwLookUpKey));

			return (VERDICT)state.dwLookUpKey;
}


					
	




