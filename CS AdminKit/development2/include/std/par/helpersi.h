/*!
 * (C) 2003 Kaspersky Lab 
 * 
 * \file	helpersi.h
 * \author	Andrew Kazachkov
 * \date	25.05.2005 15:26:34
 * \brief	internal helper functions
 * 
 */

#ifndef __KLPAR_HELPERSI_H__
#define __KLPAR_HELPERSI_H__

#include <std/par/params.h>
#include <std/time/kltime.h>

namespace KLPAR
{

	KLSTD_INLINEONLY time_t mkgmtime(struct tm *tb)
    {
        return KLSTD_mkgmtime(tb);
    };    

    KLCSC_DECL void Write(ValuesFactory* pFactory, Params* pOldParams, Params* pNewParams, AVP_dword dwFlags);

    class KLSTD_NOVTABLE SelectValueCallback : public KLSTD::KLBaseQI
    {
    public:
        virtual KLSTD::CAutoPtr<Value> DoSelectValue(
                        const wchar_t*                      szwName,
                        const std::vector<KLPAR::Value*>&   vecValues) = 0;
    };
    
    class KLSTD_NOVTABLE NodeActionCallback : public KLSTD::KLBaseQI
    {
    public:
        /*!
            result value means whether to create conatiner (default 
            behaviour) in the resulting tree
        */
        virtual bool DoSelectAction(
                        const wchar_t*                      szwName,
                        const std::vector<KLPAR::Params*>&  vecNodes) = 0;
    };
    
    class KLSTD_NOVTABLE SelectValueCallback2 : public KLSTD::KLBaseQI
    {
    public:
        virtual KLSTD::CAutoPtr<Value> DoSelectValue2(
                        const wchar_t*                      szwName,
                        const std::vector<KLPAR::Params*>&  vecParents,
                        const std::vector<KLPAR::Value*>&   vecValues) = 0;
    };

    /*!
        Рекурсивно (по отношению к ParamsValue) обрабатывает массив
        контейнеров Params: для каждой переменной вызывается метод
        SelectValueCallback::DoSelectValue, на вход которого передаётся
        значения переменной для каждого из котнейнеров Params, если
        какой-либо контейнер не содержит переменной с i-м именем,
        соответствующий указатель Value* во входном массиве будет равен
        NULL. В качестве результата метод возвращает то значение, которое
        должно быть включено в результирующий Params, если функция вернёт
        NULL, то в результирующий Params ничего не будет включено.
    */
    KLCSC_DECL void ProcessParamsRecursively(
                        const std::vector<Params*>& vecData,
                        SelectValueCallback*        pCallback,
                        Params*                     pResult,
                        bool                        bRemoveEmptyParams=false);
};

#endif //__KLPAR_HELPERSI_H__
