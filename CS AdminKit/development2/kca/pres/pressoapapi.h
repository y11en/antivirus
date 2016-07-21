/*!
 * (C) 2002 "Kaspersky Lab"
 *
 * \file PRTS/pressoapapi.h
 * \author Андрей Брыксин
 * \date 2002
 * \brief SOAP-интерфейс EventsStorage
 *
 */

int KLPRES_ResetSubscriptionsIterator(xsd__wstring sServerObjectID,
									  SOAPComponentId cidFilter,
                                      xsd__int  nTimeout,
									  struct KLPRES_RESET_SUBSCRIPTIONS_ITERATOR_RESPONSE{ 
											xsd__wstring sSubscrIteratorId; 
											struct param_error rcError; } &rc);

int KLPRES_ReleaseSubscriptionsIterator(xsd__wstring sServerObjectID,
									  xsd__wstring sSubscrIteratorId,
									  struct KLPRES_RELEASE_SUBSCRIPTIONS_ITERATOR_RESPONSE{ 
											struct param_error rcError; } &rc);

int KLPRES_GetNextSubscription( xsd__wstring sServerObjectID,
							    xsd__wstring sSubscrIteratorId,
							    struct KLPRES_GET_NEXT_SUBSCRIPTION_RESPONSE{ 
                                    param__params parSubscription;
                                    xsd__wstring sSubscrId;
                                    xsd__boolean bOk;
								    struct param_error rcError; } &rc);

int KLPRES_AddSubscription(xsd__wstring sServerObjectID,
						   xsd__wstring sSubscriptionID,
						   bool bPersistent, 
						   param__params parSubscription,
						   struct KLPRES_ADD_SUBSCRIPTION_RESPONSE{ 
								xsd__wstring sSubscriptionID; 
								struct param_error rcError; } &rc);

int KLPRES_UpdateSubscription(xsd__wstring sServerObjectID,
							  xsd__wstring sSubscriptionID,
							  param__params parSubscription,
							  struct param_error& rc);

int KLPRES_PauseSubscription(xsd__wstring sServerObjectID,
							  xsd__wstring sSubscriptionID,
							  struct param_error& rc);
								
int KLPRES_ResumeSubscription(xsd__wstring sServerObjectID,
							  xsd__wstring sSubscriptionID,
							  struct param_error& rc);

int KLPRES_RemoveSubscription(xsd__wstring sServerObjectID,
							  xsd__wstring sSubscriptionID, 
							  struct param_error& rc);
// Iterators

int KLPRES_ResetEventsIterator(xsd__wstring sServerObjectID,
							   xsd__wstring sSubscriptionID,
							   struct param_error &rc);

int KLPRES_SetEventsIterator(xsd__wstring sServerObjectID,
							 xsd__wstring sSubscriptionID,
							 long eventID,
							 struct param_error &rc);

int KLPRES_ResetEventsIteratorEx(xsd__wstring sServerObjectID,
								 xsd__wstring sSubscriptionID,
								 struct KLPRES_RESET_EVENTS_ITERATOR_EX_RESPONSE{
									xsd__wstring sJournalID;
									struct param_error rcError; } &rc);

int KLPRES_SetEventsIteratorEx(xsd__wstring sServerObjectID,
							   xsd__wstring sSubscriptionID,
							   long eventID,
							   struct KLPRES_SET_EVENTS_ITERATOR_EX_RESPONSE{
									xsd__wstring sJournalID;
									struct param_error rcError; } &rc);

int KLPRES_MoveToEventWithIndex(xsd__wstring sServerObjectID,
							    xsd__wstring sSubscriptionID,
							    long nIndex,
								struct param_error &rc);

// end of iterators

int KLPRES_GetEventsCount(xsd__wstring sServerObjectID,
						  xsd__wstring sSubscriptionID,
						  struct KLPRES_GET_EVENTS_COUNT_RESPONSE{ 
								xsd__long nCount; 
								struct param_error rcError; } &rc);

int KLPRES_GetNextEventsChunk(xsd__wstring sServerObjectID,
							  xsd__wstring sSubscriptionID,
							  struct KLPRES_GET_NEXT_EVENTS_CHUNK_RESPONSE{ 
									param__params pparEventsParamsOut; 
									struct param_error rcError; } &rc);

int KLPRES_ClearEventsList(xsd__wstring sServerObjectID,
						   xsd__wstring sSubscriptionID, 
						   struct param_error& rc);

int KLPRES_DeleteEvents(xsd__wstring sServerObjectID,
						xsd__wstring sSubscriptionID, 
						long nStopEventID, 
						struct param_error& rc);

int KLPRES_DeleteOneEventByID(xsd__wstring sServerObjectID,
							  xsd__wstring sSubscriptionID, 
							  long nEventID, 
						      struct param_error& rc);

int KLPRES_DeleteRanges(xsd__wstring sServerObjectID,
						xsd__wstring sSubscriptionID, 
						param__params parRanges, 
                        struct param_error& rc);

int KLPRES_RemoveExpiredEvents(xsd__wstring sServerObjectID,
							   xsd__wstring sSubscriptionID, 
							   struct param_error& rc);

int KLPRES_GetEventStorageFileSize(xsd__wstring sServerObjectID,
									xsd__wstring sSubscriptionID, 
									struct KLPRES_GET_EVENT_STORAGE_FILE_SIZE_RESPONSE{
										xsd__long llStorageSize;
										struct param_error rcError;	}& rc);