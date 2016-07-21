#ifndef __KLPRCI_TASKS_H__DD162788_07A0_49ba_95D3_6590372BD18D
#define __KLPRCI_TASKS_H__DD162788_07A0_49ba_95D3_6590372BD18D

#include "producttaskscontrol.h"

namespace KLPRCI
{
    class KLSTD_NOVTABLE TasksControlInternal
        :   public KLSTD::KLBaseQI
    {
    public:
        virtual void AddTask(
                        long                lTaskId,
						const std::wstring& wstrTaskType,
						KLPAR::Params*      pParams,
						const std::wstring& asyncId=L"",
						long                timeout = 0,
                        KLSTD::precise_time_t   tmPublish = KLSTD::precise_time_t()) = 0;
    };
};

KLCSKCA_DECL long KLPRCI_AllocTaskId();

#endif //__KLPRCI_TASKS_H__DD162788_07A0_49ba_95D3_6590372BD18D
