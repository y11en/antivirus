/*!
 * (C) 2002 "Kaspersky Lab"
 *
 * \file PRCI/soapapi.h
 * \author Андрей Казачков
 * \date 2002
 * \brief SOAP-интерфейс Компонента
 *
 */

struct klprci_tasks_t
{
	xsd__int*	__ptr;
	int			__size;
};

int klprci_GetTasksParams(
						xsd__wstring	ID,
						xsd__int		idtask,
						param__params	filter,
						struct klprci_GetTasksParamsResponse
						{
							param__params		pars;
							struct param_error	error;
						}	&r);

int klprci_GetPingTimeout	(xsd__wstring ID,
							 struct klprci_GetPingTimeoutResponse{xsd__int timeout; struct param_error error;} &r);

int klprci_GetState			(xsd__wstring ID,
							 struct klprci_GetStateResponse{xsd__int state; struct param_error error;} &r);

int klprci_GetStatistics	(xsd__wstring ID,
							 param__params	statisticsIn,
							 struct klprci_GetStatisticsResponse{param__params statisticsOut; struct param_error error;} &r);

int klprci_Stop				(xsd__wstring ID,
							 struct klprci_StopResponse{struct param_error error;} &r);

int klprci_Suspend			(xsd__wstring ID,
							 struct klprci_SuspendResponse{struct param_error error;} &r);

int klprci_Resume			(xsd__wstring ID,
							 struct klprci_ResumeResponse{struct param_error error;} &r);

int klprci_RunTask			(xsd__wstring ID,
							 xsd__wstring name,
							 param__params settings,
							 xsd__wstring asyncID,
							 xsd__int timeout,
							 struct klprci_RunTaskResponse{
										xsd__int	taskId;
										struct param_error error;} &r);
                                        
int klprci_RunMethod(        xsd__wstring ID,
                             xsd__wstring name,
							 param__params settings,
							 xsd__int timeout,
							 struct klprci_RunMethodResponse{
                                        param__params res;
										struct param_error error;} &r);

int klprci_GetTasksList		(xsd__wstring ID,
							 struct klprci_GetTasksListResponse{
										struct klprci_tasks_t	taskslist;
										struct param_error		error;} &r);

int klprci_GetTaskName		(xsd__wstring ID,
							 xsd__int idTask,
							struct klprci_GetTaskNameResponse
							{
								struct param_error error;
								xsd__wstring name;
							} &r);


int klprci_TaskStop			(xsd__wstring ID,
							 xsd__int idTask,
							 struct klprci_TaskStopResponse{struct param_error error;} &r);

int klprci_TaskSuspend		(xsd__wstring ID,
							 xsd__int idTask,
							 struct klprci_TaskSuspendResponse{struct param_error error;} &r);

int klprci_TaskResume		(xsd__wstring ID,
							 xsd__int idTask,
							 struct klprci_TaskResumeResponse{struct param_error error;} &r);

int klprci_TaskGetState		(xsd__wstring ID,
							 xsd__int idTask,
							 struct klprci_GetTaskStateResponse{xsd__int state; struct param_error error;}& r);

int klprci_TaskGetCompletion(xsd__wstring ID,
							 xsd__int idTask,
							 struct klprci_GetTaskCompletionResponse{xsd__int percent; struct param_error error;}& r);

int klprci_TaskReload(
                        xsd__wstring    ID,
						xsd__int        idTask,
                        param__params   settings,
						struct klprci_TaskReloadResponse{
                            struct param_error  error;
                        }& r);
