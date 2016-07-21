#include <std/base/klstd.h>
#include <std/par/params.h>
#include <common/measurer.h>
#include <kca/prss/settingsstorage.h>
#include <kca/prts/prtsdefs.h>
#include <kca/prts/prxsmacros.h>
#include <kca/prts/taskinfo.h>
#include <kca/prts/taskstorageserver.h>
#include <kca/prts/tasksstorage.h>
#include <kca/prts/tsk_list.h>

#include <string>
#include <set>
#include <vector>


#define KLCS_MODULENAME L"TSKLIST"

using namespace KLSTD;
using namespace KLPAR;

namespace TSKLIST
{
    inline std::wstring MakeRealName(const std::wstring& wstrName)
    {
        std::wstring wstrResult;
        if(!wstrName.empty())
        {
            if( L'~' == wstrName[0] )
                wstrResult.assign(&wstrName[1], wstrName.size()-1);
            else
                wstrResult = wstrName;
            
            const size_t nSize = wstrResult.size();
            if(nSize > 0 && L'~' == wstrResult[nSize-1])
            {
                wstrResult.resize(nSize-1);
            }
            else if(    nSize > 1 && 
                        L'2' == wstrResult[nSize-1] && 
                        L'~' == wstrResult[nSize-2] )
            {
                wstrResult.resize(nSize-2);
            };
        };
        return wstrResult;
    };

    KLCSKCA_DECL void GetTasksFilesList(
            const std::wstring& wstrFolder,
            std::set<std::wstring>&  setNames)
    {
    KL_TMEASURE_BEGIN(L"TSKLIST::GetTasksFilesList", 4)
        std::vector<std::wstring> vecNames;
        std::wstring wstrMask;
        KLSTD_PathAppend(
                    wstrFolder, 
                    std::wstring(KLPRTS::TASK_STORAGE_FILE_MASK)+ L"*", 
                    wstrMask, 
                    true);
        KLSTD_TRACE1(3, L"Getting files by mask '%ls'\n", wstrMask.c_str());
        KLSTD_GetFilesByMask(wstrMask, vecNames);
        for(std::vector<std::wstring>::iterator it=vecNames.begin(); it != vecNames.end(); ++it)
        {
            const std::wstring& wstrCurrent = *it;
            if(wstrCurrent.empty())
                continue;
            setNames.insert(MakeRealName(wstrCurrent));
        };
        int nTracelevel = 0;
        if(IsTraceStarted(&nTracelevel) && nTracelevel >= 4)
        {
            for(    std::set<std::wstring>::iterator it=setNames.begin(); 
                    it != setNames.end(); 
                    ++it)
            {
                KLSTD_TRACE1(5, L"Task file found: '%ls'\n", (*it).c_str());
            };
        };
    KL_TMEASURE_END()
    };
};
