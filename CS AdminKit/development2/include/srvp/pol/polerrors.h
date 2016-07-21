/*!
 * (C) 2002 "Kaspersky Lab"
 *
 * \file POLErrors.h
 * \author Михаил Кармазин
 * \date 2002
 * \brief Описание кодов ошибок для event processing
 *
 */

#ifndef __POL_ERRORS_H__
#define __POL_ERRORS_H__

#include <std/err/errintervals.h>
#include <std/err/error.h>

namespace KLPOL 
{
    enum ErrorCodes {
        ERR_POLICY_PRODUCT_CHANGE4 = KLPOLERRSTART + 1,
        ERR_CANT_APPLY_PARENT_LOCKS1,
        ERR_CANNOT_LOCK_SYNC3,
        ERR_BAD_POLICY_FOR_UNDEFINED_GROUP,
        ERR_CANT_COPY_POLICY_TO_THE_SAME_GROUP,
        ERR_SERVER_POLICY_DELETE,
        ERR_CANT_MOVE_POLICY_TO_THE_SAME_GROUP
    };

    //! Перечисление кодов ошибок модуля 
    const struct KLERR::ErrorDescription c_errorDescriptions [] =
    {
        {ERR_POLICY_PRODUCT_CHANGE4, L"Change of policy product name (from \"%s\" to \"%s\") or version (from \"%s\" to \"%s\") is not allowed."},
        {ERR_CANT_APPLY_PARENT_LOCKS1, L"Cannot apply locks from parent policy (id=%d) to one or all children. Reason unknown."},
        {ERR_CANNOT_LOCK_SYNC3, L"Cannot lock policy file policy id %d, product %ls %ls."},
        {ERR_BAD_POLICY_FOR_UNDEFINED_GROUP, L"Only server policy can be added to undefined group (with id KLGRP::GROUPID_INVALID_ID)."},
        {ERR_CANT_COPY_POLICY_TO_THE_SAME_GROUP, L"Cannot copy policy to the same group with the same name."},
        {ERR_SERVER_POLICY_DELETE, L"Cannot delete server policy."},
        {ERR_CANT_MOVE_POLICY_TO_THE_SAME_GROUP, L"Cannot move policy to the same group."},
		{0,0}
    };
}

#endif // __POL_ERRORS_H__