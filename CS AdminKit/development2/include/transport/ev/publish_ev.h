#ifndef __PUBLISH_EV_H__T10fsS6cc0KhZuzU8VMtH3
#define __PUBLISH_EV_H__T10fsS6cc0KhZuzU8VMtH3

#include <string>
#include <std/base/kldefs.h>
#include <std/base/klbase.h>
#include <kca/prci/componentid.h>
#include <std/par/params.h>

KLCSTR_DECL KLSTD_NOTHROW void KLEV_PublishEvent(
                    const KLPRCI::ComponentId&  idPublisher, 
                    const std::wstring&         wstrEventType, 
                    KLPAR::Params*			    pEventBody, 
                    long                        lLifeTime = 0, 
                    long                        lSeverity = -1,
                    KLSTD::precise_time_t       tmPublish = KLSTD::precise_time_t()) throw();

KLCSTR_DECL KLSTD_NOTHROW void KLEV_PublishEvent2(
                    const KLPRCI::ComponentId&  idPublisher, 
                    const std::wstring&         wstrEventType, 
                    KLPAR::param_entry_t*       pData,
			        size_t                      nData,
                    long                        lLifeTime = 0, 
                    long                        lSeverity = -1,
                    KLSTD::precise_time_t       tmPublish = KLSTD::precise_time_t()) throw();


#endif //__PUBLISH_EV_H__T10fsS6cc0KhZuzU8VMtH3
