#if !defined(__KL_LICENSING2_TEST_PROGRAMCLIENTINFO_HELPERS_H)
#define __KL_LICENSING2_TEST_PROGRAMCLIENTINFO_HELPERS_H

#include <Licensing2/include/LicensingInterface.h>

namespace KasperskyLicensing  {
namespace Test  {

void Fill_ProgramClientInfo_WksBeta(ProgramClientInfo& clientInfo);
void Fill_ProgramClientInfo_WksRelease(ProgramClientInfo& clientInfo);
void Fill_ProgramClientInfo_PerBeta(ProgramClientInfo& clientInfo);
void Fill_ProgramClientInfo_PerRelease(ProgramClientInfo& clientInfo);
void Fill_ProgramClientInfo_Unix(ProgramClientInfo& clientInfo);
void Fill_ProgramClientInfo_WrongOL_product(ProgramClientInfo& clientInfo);
void Fill_ProgramClientInfo_WrongOL_c_a(ProgramClientInfo& clientInfo);
void Fill_ProgramClientInfo_LocalizationId(ProgramClientInfo& clientInfo);
void Fill_ProgramClientInfo_SalesChannel(ProgramClientInfo& clientInfo);
void Fill_ProgramClientInfo_MarketSector(ProgramClientInfo& clientInfo);
void Fill_ProgramClientInfo_Package(ProgramClientInfo& clientInfo);
void Fill_ProgramClientInfo_IdenticalSerials_step1(ProgramClientInfo& clientInfo);
void Fill_ProgramClientInfo_IdenticalSerials_step2(ProgramClientInfo& clientInfo);

}  // namespace Test
}  // namespace KasperskyLicensing

#endif  // !defined(__KL_LICENSING2_TEST_PROGRAMCLIENTINFO_HELPERS_H)
