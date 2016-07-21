/*!
 * (C) 2002 "Kaspersky Lab"
 *
 * \file PackagesErrors.h
 * \author Михаил Кармазин
 * \date 2002
 * \brief Описание кодов ошибок для пакетов инсталляции
 *
 */

#ifndef __PACKAGESERRORS_H__9A5E37E6_DD84_40b3_BC0F_37BD12A38800
#define __PACKAGESERRORS_H__9A5E37E6_DD84_40b3_BC0F_37BD12A38800

#include <std/err/errintervals.h>
#include <std/err/error.h>

namespace KLPKG {

    enum ErrorCodes {
        ERR_FOLDER_WITH_RECORDED_PACKAGE_NOT_FOUND3 = KLPKGERRSTART + 1,
		ERR_PACKAGE_NOT_FOUND1,
		ERR_INVALID_NETWORK_PATH_FOR_PACKAGE1,
		ERR_CANNOT_MAP_NETWORK_FOLDER_TO_LOCAL_FOLDER3,
		ERR_SERVER_NOT_AVAILABLE0,
        ERR_NO_PATH_DATA_IN_SS,
        ERR_BAD_NETWORK_PATH1,
        ERR_COMPUTER_NAME1,
        ERR_PACKAGE_USED_BY_TASK1,
        ERR_PACKAGE_USED_BY_TASKS2
    };


    //! Перечисление кодов ошибок модуля 
    const struct KLERR::ErrorDescription c_errorDescriptions [] = {
        {ERR_FOLDER_WITH_RECORDED_PACKAGE_NOT_FOUND3,		L"Want distributive of \"%s\" with version \"%s\" in folder \"%s\", but folder not found."},
        {ERR_PACKAGE_NOT_FOUND1,		L"Package %d not found."},
		{ERR_INVALID_NETWORK_PATH_FOR_PACKAGE1, L"Invalid network path for package: %s."},
		{ERR_CANNOT_MAP_NETWORK_FOLDER_TO_LOCAL_FOLDER3, L"Cannot map network folder \"%s\" for \"%s\" with version \"%s\" to local folder of the server."},
		{ERR_SERVER_NOT_AVAILABLE0, L"Server not available."},
        {ERR_NO_PATH_DATA_IN_SS, L"Settings storage doesn't contain any information about path to shared folder used by server."},
        {ERR_BAD_NETWORK_PATH1, L"Path for server needs specified in setting storage as existing network path does not exists or invalid (\"%s\")."},
        {ERR_COMPUTER_NAME1, L"Cannot get computer name, error code %d."},
        {ERR_PACKAGE_USED_BY_TASK1, L"Package %d used by task."},
        {ERR_PACKAGE_USED_BY_TASKS2, L"Package %d used by task(s): %s."},
		{0,0}
    };
}



#endif // __PACKAGESERRORS_H__9A5E37E6_DD84_40b3_BC0F_37BD12A38800


