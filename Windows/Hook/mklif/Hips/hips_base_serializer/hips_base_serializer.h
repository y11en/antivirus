#include <structs/s_hips.h>

extern "C" {



#ifdef HIPS_BASE_SERIALIZER_DLL_EXPORTS
#define HIPS_BASE_SERIALIZER_DLL_EXPORTS __declspec(dllexport)
#else
#define HIPS_BASE_SERIALIZER_DLL_EXPORTS __declspec(dllimport)
#endif

HIPS_BASE_SERIALIZER_DLL_EXPORTS
bool InitializeConst(
             int * l_HIPS_FLAG_ALLOW,
             int * l_HIPS_FLAG_ASK,
             int * l_HIPS_FLAG_DENY,
             int * l_HIPS_FLAG_INHERIT,
             int * l_HIPS_POS_WRITE,
             int * l_HIPS_POS_PERMIS,
             int * l_HIPS_POS_READ,
             int * l_HIPS_POS_ENUM,
             int * l_HIPS_POS_DELETE,
             int * l_HIPS_LOG_OFF,
             int * l_HIPS_LOG_ON
					 );

HIPS_BASE_SERIALIZER_DLL_EXPORTS
bool Initialize();

HIPS_BASE_SERIALIZER_DLL_EXPORTS
bool UnInitialize();

HIPS_BASE_SERIALIZER_DLL_EXPORTS
bool AddResourceGroup(DWORD l_GrID, DWORD l_ParentGrID, wchar_t * l_ResGrName, DWORD l_ExFlags);

HIPS_BASE_SERIALIZER_DLL_EXPORTS
bool AddAppGroup(DWORD l_GrID, DWORD l_ParentGrID, wchar_t * l_AppGrName, DWORD l_ServStrType, wchar_t * l_ServStr);

HIPS_BASE_SERIALIZER_DLL_EXPORTS
bool AddSimpleResource(bool l_IsEnabled, DWORD l_ExFlags, DWORD l_ResID, DWORD l_ResGrID, DWORD l_ResType, wchar_t * l_ResDescr, wchar_t * l_Param1, wchar_t * l_Param2);

HIPS_BASE_SERIALIZER_DLL_EXPORTS
bool AddApp(DWORD l_AppID, DWORD l_AppGrID, wchar_t * l_AppName, wchar_t * l_AppHash, DWORD l_AppFlags);

HIPS_BASE_SERIALIZER_DLL_EXPORTS
bool AddWebService(bool l_IsEnabled, DWORD l_WebServID, DWORD l_GrID, wchar_t * l_Desc,
				   bool l_UseProtocol, DWORD l_Prot,
				   bool l_UseType, DWORD l_IType,
				   bool l_UseCode, DWORD l_ICode,
				   DWORD l_Dir, wchar_t * l_RemPorts, wchar_t * l_LocPorts);

HIPS_BASE_SERIALIZER_DLL_EXPORTS
bool AddRule(DWORD l_RuleID, DWORD l_RuleState, DWORD l_RuleType, DWORD l_AppId, DWORD l_AppGrId, DWORD l_AppFlag, DWORD l_ResId, DWORD l_Res2Id, DWORD l_AccessFlag);

HIPS_BASE_SERIALIZER_DLL_EXPORTS
bool CompileBase(wchar_t * l_pBaseFileName);

HIPS_BASE_SERIALIZER_DLL_EXPORTS
DWORD SetBlockPos(DWORD am, bool is_log, bool is_inh, DWORD pos, DWORD val);

HIPS_BASE_SERIALIZER_DLL_EXPORTS
DWORD GetInh(DWORD pos, DWORD val);

HIPS_BASE_SERIALIZER_DLL_EXPORTS
DWORD GetLog(DWORD pos, DWORD val);

HIPS_BASE_SERIALIZER_DLL_EXPORTS
DWORD GetAM(DWORD pos, DWORD val);

}