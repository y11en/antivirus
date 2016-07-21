/*!
 * (C) 2002 "Kaspersky Lab"
 *
 * \file PRTS/prtssoapapi.h
 * \author Андрей Брыксин
 * \date 2002
 * \brief SOAP-интерфейс TaskStorage
 *
 */

int KLPRTS_ResetTasksIterator(xsd__wstring sServerObjectID,
							  SOAPComponentId cidFilter, 
							  xsd__wstring sTaskNameFilter,
							  struct KLPRTS_RESET_TASKS_ITERATOR_RESPONSE{ 
									param__params parTaskListParamsOut; 
									struct param_error rcError; } &rc);

int KLPRTS_GetTaskByID(xsd__wstring sServerObjectID,
					   xsd__wstring TaskID, 
					   struct KLPRTS_GET_TASK_BY_ID_RESPONSE{ 
							param__params parTaskParamsOut; 
							struct param_error rcError; } &rc);

int KLPRTS_GetTaskByIDWithPolicyApplied(xsd__wstring sServerObjectID,
					   xsd__wstring TaskID, 
					   struct KLPRTS_GET_TASK_BY_ID_WPA_RESPONSE{ 
							param__params parTaskParamsOut; 
							struct param_error rcError; } &rc);

int KLPRTS_AddTask(xsd__wstring sServerObjectID,
				   param__params parTaskParams, 
				   struct KLPRTS_ADD_TASK_RESPONSE{ 
						xsd__wstring TaskID; 
						struct param_error rcError; } &rc);

int KLPRTS_UpdateTask(xsd__wstring sServerObjectID,
					  xsd__wstring TaskID, 
					  param__params parTaskParams, 
					  struct param_error& rc);

int KLPRTS_DeleteTask(xsd__wstring sServerObjectID, 
					  xsd__wstring TaskID, 
					  struct param_error& rc);

int KLPRTS_SetTaskStartEvent(xsd__wstring sServerObjectID,
							 xsd__wstring TaskID, 
							 SOAPComponentId cidEventFilter, 
							 xsd__wstring sEventType, 
							 param__params parBodyFilter, 
							 struct param_error& rc);

int KLPRTS_GetTaskStartEvent(xsd__wstring sServerObjectID,
							 xsd__wstring TaskID, 
							 struct KLPRTS_GET_TASK_START_EVENT_RESPONSE {
								 SOAPComponentId cidEventFilter;
								 xsd__wstring sEventType;
								 param__params parBodyFilterOut;
								 struct param_error rcError;
							 } &rc);

int KLPRTS_UpdateTask2(xsd__wstring sServerObjectID,
					   xsd__wstring TaskID, 
					   param__params parTaskParams, 
					   struct param_error& rc);

int KLPRTS_ReplaceTask(xsd__wstring sServerObjectID,
					   xsd__wstring TaskID, 
					   param__params parTaskParams, 
					   struct param_error& rc);
