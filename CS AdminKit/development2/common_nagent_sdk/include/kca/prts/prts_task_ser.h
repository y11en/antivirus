#ifndef __PRTS_TASK_SER_H__
#define __PRTS_TASK_SER_H__

/*
    Task format settings

        SS_VALUE_NAME_TASK_NAME                 STRING_T
        SS_VALUE_NAME_TASK_ADDITIONAL_PARAMS    PARAMS_T
        SS_VALUE_NAME_TASK_INFO_PARAMS          PARAMS_T
    
        //Scheduled task
        SS_VALUE_NAME_TASK_CLASS_ID             KLSCH::Task::GetClassId             INT_T
        SS_VALUE_NAME_TASK_PRIORITY             KLSCH::Task::GetTaskThreadPriority  INT_T
        SS_VALUE_NAME_TASK_MAX_EXEC_TIME        KLSCH::Task::GetMaxExecutionTime    INT_T
        SS_VALUE_NAME_TASK_DEL_AFTER_RUN_FLAG   KLSCH::Task::GetDelAfterRunFlag     BOOL_T
        SS_VALUE_NAME_TASK_LAST_EXEC_TIME       KLSCH::Task::GetLastExecutionTime   DATETIME_T
        SS_VALUE_NAME_TASK_LAST_EXEC_TIME_SEC   KLSCH::Task::GetLastExecutionTime   INT_T
        SS_VALUE_NAME_TASK_START_DELTA          KLSCH::Task::GetStartDeltaTimeout   INT_T
        SS_VALUE_NAME_TASK_PREPARATIVE_START    KLSCH::Task::GetPreparativeStartTimeout INT_T
    
        //Schedule
        SS_VALUE_NAME_TASKSCH_RUN_MISSED_FLAG           KLSCH::Schedule::TaskGetRunMissedFlag   BOOL_T
        SS_VALUE_NAME_TASKSCH_LIFETIME                  KLSCH::Schedule::GetLifetime            DATETIME_T
        SS_VALUE_NAME_TASKSCH_FIRST_EXECUTION_TIME      KLSCH::Schedule::GetFirstExecutionTime  DATETIME_T
        SS_VALUE_NAME_TASKSCH_FIRST_EXECUTION_TIME_SEC  KLSCH::Schedule::GetFirstExecutionTime  INT_T
        SS_VALUE_NAME_TASKSCH_MS_PERIOD                 KLSCH::Schedule::GetPeriod              INT_T
        SS_VALUE_NAME_TASKSCH_TYPE                      KLSCH::Schedule::GetType                INT_T

        // Scheduler type specific settings
    SCH_TYPE_EVERYDAY:
        SS_VALUE_NAME_TASKSCH_ED_HOURS  KLSCH::EverydaySchedule::GetExecutionTime   INT_T
        SS_VALUE_NAME_TASKSCH_ED_MINS   KLSCH::EverydaySchedule::GetExecutionTime   INT_T
        SS_VALUE_NAME_TASKSCH_ED_SECS   KLSCH::EverydaySchedule::GetExecutionTime   INT_T
    SCH_TYPE_EVERYWEEK:
        SS_VALUE_NAME_TASKSCH_EW_DAY    KLSCH::EveryWeekSchedule::GetExecutionTime  INT_T
        SS_VALUE_NAME_TASKSCH_EW_HOURS  KLSCH::EveryWeekSchedule::GetExecutionTime  INT_T
        SS_VALUE_NAME_TASKSCH_EW_MINS   KLSCH::EveryWeekSchedule::GetExecutionTime  INT_T
        SS_VALUE_NAME_TASKSCH_EW_SECS   KLSCH::EveryWeekSchedule::GetExecutionTime  INT_T
    SCH_TYPE_EVERYMONTH:
        SS_VALUE_NAME_TASKSCH_EM_DAY    KLSCH::EveryMonthSchedule::GetExecutionTime INT_T
        SS_VALUE_NAME_TASKSCH_EM_HOURS  KLSCH::EveryMonthSchedule::GetExecutionTime INT_T
        SS_VALUE_NAME_TASKSCH_EM_MINS   KLSCH::EveryMonthSchedule::GetExecutionTime INT_T
        SS_VALUE_NAME_TASKSCH_EM_SECS   KLSCH::EveryMonthSchedule::GetExecutionTime INT_T
*/


/*!
  \brief	Serialization task into params container

  \param	szwProductName      [in]  product name
  \param	szwProductVersion   [in]  product short version
  \param	szwTaskType         [in]  task type name
  \param	pTaskSch            [in]  task schedule
  \param	pParams             [in]  task parameters
  \param	pParamsTaskInfo     [in]  task extra parameters
  \param	ppData              [out] resulting data
*/
KLCSKCA_DECL void KLPRTS_SerializeTaskToParams(
                    const wchar_t*          szwProductName,
                    const wchar_t*          szwProductVersion,
                    const wchar_t*          szwTaskType,
                    const KLSCH::Task*      pTaskSch,
                    const KLPAR::Params*    pParams,
                    const KLPAR::Params*    pParamsTaskInfo,
                    KLPAR::Params**         ppData);


/*!
  \brief	Serialization task from params container

  \param	pData               [in]  serialized data
  \param	pszwProductName     [out] product name
  \param	pszwProductVersion  [out] product short version
  \param	pszwTaskType        [out] task type name
  \param	ppTaskSch           [out] task schedule
  \param	ppParams            [out] task parameters
  \param	ppParamsTaskInfo    [out] task extra parameters
*/
KLCSKCA_DECL void KLPRTS_DeserializeTaskFromParams(
                    const KLPAR::Params*    pData,
                    KLSTD::AKWSTR*          pszwProductName,
                    KLSTD::AKWSTR*          pszwProductVersion,
                    KLSTD::AKWSTR*          pszwTaskType,
                    KLSCH::Task**           ppTaskSch,
                    KLPAR::Params**         ppParams,
                    KLPAR::Params**         ppParamsTaskInfo);

#endif //__PRTS_TASK_SER_H__
