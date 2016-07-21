/*!
 * (C) 2002 "Kaspersky Lab"
 *
 * \file AgentErrors.h
 * \author Шияфетдинов Дамир
 * \date 2002
 * \brief Описание кодов ошибок для агента.
 *
 */


#ifndef KLAGINST_AGENTERRORS_H
#define KLAGINST_AGENTERRORS_H

#include "std/err/errintervals.h"
#include "std/err/error.h"

namespace KLAGINST {

	#define KLAGINST_MODULE_NAME L"KLAGINST"	

    enum ErrorCodes {
        ERR_COMPONENT_NOT_FOUND = KLAGINSTERRSTART + 1,
		ERR_COMPONENT_WRONG_SETTINGS,
		ERR_MULTIPLE_ALREADY_STARTED,
		ERR_AGENT_INSTANCE_ALREADY_EXIST,
		ERR_CANT_START,
		ERR_NOT_STARTED,
		ERR_COMPONENT_NOT_CANBESTARTED,
		ERR_CANT_START_TASK,
		ERR_CANT_START_MODULE
    };


    //! Перечисление кодов ошибок модуля 
    const struct KLERR::ErrorDescription c_errorDescriptions [] = {
        {ERR_COMPONENT_NOT_FOUND,		L"Component wasn't find in settings storage. Component name - '%ls'"},
		{ERR_COMPONENT_WRONG_SETTINGS,  L"Can't start component. Component has a wrong settings. Component name - '%ls'"},
		{ERR_MULTIPLE_ALREADY_STARTED,  L"Can't start component. Component without multiple running flag already started. Component name - '%ls'" },
		{ERR_CANT_START,				L"Agent couldn't start component application. Component name - '%ls'" },
		{ERR_AGENT_INSTANCE_ALREADY_EXIST, L"AgentInstance object already exist in system. Allows only one AgentInstance object."},
		{ERR_NOT_STARTED, L"AgentInstance object wasn't started."},
		{ERR_COMPONENT_NOT_CANBESTARTED, L"Agent couldn't start component application because component don't have 'CanBeSatrted' flag. Component name - '%ls'"},
		{ERR_CANT_START_TASK, L"Can't start taks '%1'."},
		{ERR_CANT_START_MODULE, L"Can't load binary module. Command line - '%1'. System error - '%2'"},
        {0,0}
    };

}

#endif // KLAGINST_AGENTERRORS_H
