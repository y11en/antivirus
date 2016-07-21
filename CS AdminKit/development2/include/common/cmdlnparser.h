#ifndef __CMDLNPARSER_H__
#define __CMDLNPARSER_H__

#include <vector>

/*
    {-<key> {[param]}}
*/

template<class T, class F>
class CmdlnParser
{
public:
    bool Process(T const * const * argv, F& f)
    {
        if(!argv[0])
            return true;
        
        bool                    bResult = true;
        const T*                pLastKey = NULL;
        std::vector<const T*>   vecArguments;

        for(size_t i = 1; argv[i-1]; ++i)
        {
            const T * pString = argv[i];
            if( !pString ||
                pString[0] == T('-')
#ifdef _WIN32
                || pString[0] == T('/')
#endif
                )
            {//new key found or end of cmdline reached
                if(pLastKey || vecArguments.size())
                {
                    const T szEmpyStr[1] = {0};
                    if(!f(  ((pLastKey && pLastKey[0])?(&pLastKey[1]):szEmpyStr), 
                            vecArguments))
                    {
                        bResult = false;
                        break;
                    };
                    pLastKey = NULL;
                    vecArguments.resize(0);
                };
                pLastKey = pString;
            }
            else
            {//new param found
                if(0 == vecArguments.capacity())
                    vecArguments.reserve(10);
                vecArguments.push_back(pString);
            };
        };
        return bResult;
    };
};

#endif //__CMDLNPARSER_H__
