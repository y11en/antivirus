#ifndef __COMPONENTINSTANCE2_H__
#define __COMPONENTINSTANCE2_H__

#include <std/time/kltime.h>
#include <kca/prci/componentinstance.h>

namespace KLPRCI
{
    class KLSTD_NOVTABLE ComponentInstance2
        :   public ComponentInstance
    {
    public:
        virtual void SetInstanceState(
                        ComponentInstanceState  state, 
                        KLSTD::precise_time_t   tmPublish = KLSTD::precise_time_t(),
                        KLPAR::Params*          pExtraData = NULL)  = 0;

		virtual void PublishEvent(
				const std::wstring&	    eventType, 
				KLPAR::Params*		    eventBody,
                KLSTD::precise_time_t   tmPublish = KLSTD::precise_time_t()) = 0;
    };
};

#endif //__COMPONENTINSTANCE2_H__
