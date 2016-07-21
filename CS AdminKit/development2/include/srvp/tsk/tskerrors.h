/*!
 * (C) 2002 "Kaspersky Lab"
 *
 * \file TSKErrors.h
 * \author Михаил Кармазин
 * \date 2002
 * \brief Описание кодов ошибок для group tasks
 *
 */

#ifndef __TSK_ERRORS_H__
#define __TSK_ERRORS_H__

#include <std/err/errintervals.h>
#include <std/err/error.h>

namespace KLTSK {

    enum ErrorCodes {
        ERR_NO_SUCH_TASK1 = KLTSKERRSTART + 1,
        ERR_TASK_IS_RUNNING1,
        ERR_TASK_DOES_NOT_SET_FOR_HOST3,
        ERR_TASK_NAME_EMPTY,
        ERR_CANNOT_COPY_SERVICE_EXEC4,
        ERR_CANNOT_COPY_LOGIN_SCRIPT_EXEC4,
        ERR_CANNOT_CHANGE_TASK_NAME,
        ERR_CANNOT_CHANGE_COMPONENT_ID,
        ERR_LOGIN_ALREADY_USED2,
        ERR_MACHINE_LIST_CREATE1,
        ERR_MACHINE_LIST_WRITE1,
        ERR_NOT_CORRESPONDING_DOMAIN,
        //ERR_NOT_REMOTE_TASK_NOT_FOR_GROUP1,
        ERR_SHALL_HAS_SYNC_ID1,
        ERR_CANNOT_LOCK_SYNC2,
        ERR_CANNOT_ADD_SERVER_AGENT_TASK_TO_GROUP,
        ERR_LOGIN_SCRIPT_RI_TASK_ON_GROUP,
		ERR_CANNOT_FIND_CLEANER_INIFILE
    };

    //! Перечисление кодов ошибок модуля 
    const struct KLERR::ErrorDescription c_errorDescriptions [] = {
        {ERR_NO_SUCH_TASK1,		L"Group task with id=%d not found."},
        {ERR_TASK_IS_RUNNING1, L"Task \"%ls\" is running now. Requested operation cannot be completed while the task is running."},
        {ERR_TASK_DOES_NOT_SET_FOR_HOST3, L"Group task %d does not set to run for host %ls\\%ls."},
        {ERR_TASK_NAME_EMPTY, L"Task type name cannot be empty."},
        {ERR_CANNOT_COPY_SERVICE_EXEC4, L"Cannot copy service executable from \"%ls\" to \"%ls\". Error code: %d (%ls)."},
        {ERR_CANNOT_COPY_LOGIN_SCRIPT_EXEC4, L"Cannot copy login script executable from \"%ls\" to \"%ls\". Error code: %d (%ls)."},
        {ERR_CANNOT_CHANGE_TASK_NAME, L"Task type name cannot be changed."},
        {ERR_CANNOT_CHANGE_COMPONENT_ID, L"Component ID for task cannot be changed."},
        {ERR_LOGIN_ALREADY_USED2, L"Login %ls\\%ls is already used in login-script installation task."},
        {ERR_MACHINE_LIST_CREATE1, L"Cannot create machines list file \"%ls\""},
        {ERR_MACHINE_LIST_WRITE1,  L"Cannot write an entry to machines list file \"%ls\""},
        {ERR_NOT_CORRESPONDING_DOMAIN, L"User and machine domains shall correspond each other."},
        //{ERR_NOT_REMOTE_TASK_NOT_FOR_GROUP1, L"Task \"%ls\" looks like group task for remote machines, but task's eStorageType is not group (Common?)." },
        {ERR_SHALL_HAS_SYNC_ID1, L"Task \"%ls\" looks like group task for remote machines, but doesn't have synchromization id in the database." },
        {ERR_CANNOT_LOCK_SYNC2, L"Cannot lock TS file for task %d (TS id is %ls)."},
        {ERR_CANNOT_ADD_SERVER_AGENT_TASK_TO_GROUP, L"This task will run on server's agent, thus it cannot be added to group. Can add such tasks only to common task storage."},
        {ERR_LOGIN_SCRIPT_RI_TASK_ON_GROUP, L"Cannot add login script task to group."},
		{ERR_CANNOT_FIND_CLEANER_INIFILE, L"Cannot find ini-file for competitor product."},
		{0,0}
    };

}

#endif // __TSK_ERRORS_H__