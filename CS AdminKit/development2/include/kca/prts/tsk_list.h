#ifndef __KLPRTS_TSK_LIST_H__
#define __KLPRTS_TSK_LIST_H__

#include <string>
#include <set>

namespace TSKLIST
{
    KLCSKCA_DECL void GetTasksFilesList(
            const std::wstring& wstrFolder,
            std::set<std::wstring>&  setNames);

};

#endif //__KLPRTS_TSK_LIST_H__
