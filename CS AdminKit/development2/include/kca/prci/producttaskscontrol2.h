#ifndef __PRODUCTTASKSCONTROL2_H__
#define __PRODUCTTASKSCONTROL2_H__

#include <kca/prci/producttaskscontrol.h>

namespace KLPRCI
{
    class KLSTD_NOVTABLE ProductTasksControl2
        :   public ProductTasksControl
    {
    public:
        virtual void SetTaskState(
                        long taskId, 
                        TaskState state, 
                        KLPAR::Params* results = NULL,
                        KLSTD::precise_time_t   tmPublish = KLSTD::precise_time_t())  = 0;
    };
    

    typedef void  (*RunMethodCallback)(
					void*					pContext,
					const std::wstring&		wstrMethodName,
                    KLPAR::Params*	        pInParams,
                    long					lTimeout,
                    KLPAR::Params**         pOutParams) ;


    class KLSTD_NOVTABLE ProductTasksControl3
        :   public ProductTasksControl2
    {
    public:
       virtual void SetRunMethodsCallback( 
						void*				        context,
                        KLPRCI::RunMethodCallback   callback )  = 0;

    };
};

#endif //__PRODUCTTASKSCONTROL2_H__
