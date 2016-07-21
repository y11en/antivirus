#include <hashutil/hashutil.h>
#include <rpc/requestmanager.h>
#include <rpc/debugtrace.h>
#include <rpc_template.h>

using namespace PragueRPC;


bool _Stub ( Request& aRequest )
{
  ENTER;
  return true;
}


Method TemplateMethods [] = 
  { { iCountCRC32str ( "" ), (void*)&_Stub },
    { 0, 0 }, };

Interface TemplateInterface = { }, TemplateMethods };
