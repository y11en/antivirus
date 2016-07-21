#include <std/base/klstd.h>
#include <std/par/params.h>
#include <kca/prts/taskinfo.h>

KLCSKCA_DECL void KLPRTS_SerializeTaskToParams(
                    const wchar_t*          szwProductName,
                    const wchar_t*          szwProductVersion,
                    const wchar_t*          szwTaskType,
                    const KLSCH::Task*      pTaskSch,
		            const KLPAR::Params*    pParams,
		            const KLPAR::Params*    pParamsTaskInfo,
                    KLPAR::Params**         ppData)
{
    KLSTD_CHK(szwProductName, szwProductName && szwProductName[0]);
    KLSTD_CHK(szwProductVersion, szwProductVersion && szwProductVersion[0]);
    KLSTD_CHK(szwTaskType, szwTaskType && szwTaskType[0]);
    KLSTD_CHKINPTR(pTaskSch);
    KLSTD_CHKINPTR(pParams);
    KLSTD_CHKINPTR(pParamsTaskInfo);
    KLSTD_CHKOUTPTR(ppData);
    ;
    const KLPRCI::ComponentId idPrototype(
                            szwProductName,
                            szwProductVersion,
                            L"",
                            L"");

    KLPRTS::CTaskInfoInternal ti(
                            L"",
                            idPrototype,
                            szwTaskType,
                            pTaskSch,
                            pParams,
                            pParamsTaskInfo);
    KLSTD::CAutoPtr<KLPAR::Params> pData;
    KLPAR_CreateParams(&pData);
    ti.Serialize(pData);
    pData.CopyTo(ppData);
};

KLCSKCA_DECL void KLPRTS_DeserializeTaskFromParams(
                    const KLPAR::Params*    pData,
                    KLSTD::AKWSTR*          pszwProductName,
                    KLSTD::AKWSTR*          pszwProductVersion,
                    KLSTD::AKWSTR*          pszwTaskType,
                    KLSCH::Task**           ppTaskSch,
		            KLPAR::Params**         ppParams,
		            KLPAR::Params**         ppParamsTaskInfo)
{
    KLSTD_CHKINPTR(pData);
    KLPRTS::CTaskInfoInternal ti(pData);
    const KLPRCI::ComponentId& idComponent = ti.GetTaskCID();
    KLSTD::klwstr_t wstrProductName = idComponent.productName.c_str();
    KLSTD::klwstr_t wstrProductVersion = idComponent.version.c_str();
    KLSTD::klwstr_t wstrTaskType = ti.GetTaskName().c_str();
    ;
    if(pszwProductName)
        *pszwProductName = wstrProductName.detach();
    if(pszwProductVersion)
        *pszwProductVersion = wstrProductVersion.detach();
    if(pszwTaskType)
        *pszwTaskType = wstrTaskType.detach();
    if(ppTaskSch && ti.GetTask())
        ti.GetTask()->Clone(ppTaskSch);
    if(ppParams && ti.GetTaskParams())
        ti.GetTaskParams().CopyTo(ppParams);
    if(ppParamsTaskInfo && ti.GetTaskInfo())
        ti.GetTaskInfo().CopyTo(ppParamsTaskInfo);
};
