#include "ini_journal.h"

IniJournal::IniJournal()
{
}

void IniJournal::publishMessage(const KLUPD::CoreError &code, const KLUPD::NoCaseString &parameter1, const KLUPD::NoCaseString &parameter2)
{
#ifdef WSTRING_SUPPORTED
    if(parameter1.empty())
        printf("\n%S", KLUPD::toString(code).toWideChar());
    else if(parameter2.empty())
        printf("\n%S '%S'", KLUPD::toString(code).toWideChar(), parameter1.toWideChar());
    else
        printf("\n%S '%S' '%S'", KLUPD::toString(code).toWideChar(), parameter1.toWideChar(), parameter2.toWideChar());
#else
    if(parameter1.empty())
        printf("\n%s", KLUPD::toString(code).toWideChar());
    else if(parameter2.empty())
        printf("\n%s '%s'", KLUPD::toString(code).toWideChar(), parameter1.toWideChar());
    else
        printf("\n%s '%s' '%s'", KLUPD::toString(code).toWideChar(), parameter1.toWideChar(), parameter2.toWideChar());
#endif
}

void IniJournal::publishRetranslationResult(const KLUPD::CoreError &code)
{
    #ifdef WSTRING_SUPPORTED
        printf("\nRetranslation operation result '%S'", KLUPD::toString(code).toWideChar());
    #else
        printf("\nRetranslation operation result '%s'", KLUPD::toString(code).toWideChar());
    #endif
}
void IniJournal::publishUpdateResult(const KLUPD::CoreError &code)
{
    #ifdef WSTRING_SUPPORTED
        printf("\nUpdate operation result '%S'", KLUPD::toString(code).toWideChar());
    #else
        printf("\nUpdate operation result '%s'", KLUPD::toString(code).toWideChar());
    #endif
}
void IniJournal::publishRollbackResult(const KLUPD::CoreError &code)
{
    #ifdef WSTRING_SUPPORTED
        printf("\nRollback operation result '%S'", KLUPD::toString(code).toWideChar());
    #else
        printf("\nRollback operation result '%s'", KLUPD::toString(code).toWideChar());
    #endif
}

