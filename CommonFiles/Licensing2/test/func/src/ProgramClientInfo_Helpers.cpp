#include "ProgramClientInfo_Helpers.h"

namespace KasperskyLicensing  {
namespace Test  {

void Fill_ProgramClientInfo_WksBeta(ProgramClientInfo& clientInfo)
{
	clientInfo.version_type = ProgramClientInfo::VER_TYPE_BETA;
	clientInfo.product_info.product_id.id				= 1000;  // ?
	clientInfo.product_info.product_id.type				= LicenseObjectId::OBJ_TYPE_PRODUCT;
	clientInfo.product_info.product_id.major_version	= 6;
	clientInfo.product_info.product_id.minor_version	= 0;
	clientInfo.product_info.market_sector_id			= 1;  // ?
	clientInfo.product_info.sales_channel_id			= 1;  // ?
	clientInfo.product_info.localization_id				= 1;  // ?
	clientInfo.product_info.package						= 1;  // ?
	clientInfo.platform_id								= PT_WIN_WORKSTATION;
}

void Fill_ProgramClientInfo_WksRelease(ProgramClientInfo& clientInfo)
{
	clientInfo.version_type = ProgramClientInfo::VER_TYPE_RELEASE;
	clientInfo.product_info.product_id.id				= 1000;  // ?
	clientInfo.product_info.product_id.type				= LicenseObjectId::OBJ_TYPE_PRODUCT;
	clientInfo.product_info.product_id.major_version	= 6;
	clientInfo.product_info.product_id.minor_version	= 0;
	clientInfo.product_info.market_sector_id			= 1;  // ?
	clientInfo.product_info.sales_channel_id			= 1;  // ?
	clientInfo.product_info.localization_id				= 1;  // ?
	clientInfo.product_info.package						= 1;  // ?
	clientInfo.platform_id								= PT_WIN_WORKSTATION;
}

void Fill_ProgramClientInfo_PerBeta(ProgramClientInfo& clientInfo)
{
	clientInfo.version_type = ProgramClientInfo::VER_TYPE_BETA;
	clientInfo.product_info.product_id.id				= 1000;  // ?
	clientInfo.product_info.product_id.type				= LicenseObjectId::OBJ_TYPE_PRODUCT;
	clientInfo.product_info.product_id.major_version	= 6;
	clientInfo.product_info.product_id.minor_version	= 0;
	clientInfo.product_info.market_sector_id			= 1;  // ?
	clientInfo.product_info.sales_channel_id			= 1;  // ?
	clientInfo.product_info.localization_id				= 1;  // ?
	clientInfo.product_info.package						= 1;  // ?
	clientInfo.platform_id								= PT_WIN_PERSONAL;
}


void Fill_ProgramClientInfo_PerRelease(ProgramClientInfo& clientInfo)
{
	clientInfo.version_type = ProgramClientInfo::VER_TYPE_RELEASE;
	clientInfo.product_info.product_id.id				= 1000;  // ?
	clientInfo.product_info.product_id.type				= LicenseObjectId::OBJ_TYPE_PRODUCT;
	clientInfo.product_info.product_id.major_version	= 6;
	clientInfo.product_info.product_id.minor_version	= 0;
	clientInfo.product_info.market_sector_id			= 1;  // ?
	clientInfo.product_info.sales_channel_id			= 1;  // ?
	clientInfo.product_info.localization_id				= 1;  // ?
	clientInfo.product_info.package						= 1;  // ?
	clientInfo.platform_id								= PT_WIN_PERSONAL;
}


void Fill_ProgramClientInfo_Unix(ProgramClientInfo& clientInfo)
{
	clientInfo.version_type = ProgramClientInfo::VER_TYPE_RELEASE;
	clientInfo.product_info.product_id.id				= 1000;  // ?
	clientInfo.product_info.product_id.type				= LicenseObjectId::OBJ_TYPE_PRODUCT;
	clientInfo.product_info.product_id.major_version	= 6;
	clientInfo.product_info.product_id.minor_version	= 0;
	clientInfo.product_info.market_sector_id			= 1;  // ?
	clientInfo.product_info.sales_channel_id			= 1;  // ?
	clientInfo.product_info.localization_id				= 1;  // ?
	clientInfo.product_info.package						= 1;  // ?
	clientInfo.platform_id								= PT_UNIX_FAMILY;
}


void Fill_ProgramClientInfo_WrongOL_product(ProgramClientInfo& clientInfo)
{
	clientInfo.version_type = ProgramClientInfo::VER_TYPE_RELEASE;
	clientInfo.product_info.product_id.id				= 10000;  // ? in key 1000
	clientInfo.product_info.product_id.type				= LicenseObjectId::OBJ_TYPE_PRODUCT;
	clientInfo.product_info.product_id.major_version	= 6;
	clientInfo.product_info.product_id.minor_version	= 0;
	clientInfo.product_info.market_sector_id			= 1;  // ?
	clientInfo.product_info.sales_channel_id			= 1;  // ?
	clientInfo.product_info.localization_id				= 1;  // ?
	clientInfo.product_info.package						= 1;  // ?
	clientInfo.platform_id								= PT_WIN_WORKSTATION;
}

void Fill_ProgramClientInfo_WrongOL_c_a(ProgramClientInfo& clientInfo)
{
	clientInfo.version_type = ProgramClientInfo::VER_TYPE_RELEASE;
	clientInfo.product_info.product_id.id				= 1000;  // ?
	clientInfo.product_info.product_id.type				= LicenseObjectId::OBJ_TYPE_PRODUCT;
	clientInfo.product_info.product_id.major_version	= 6;
	clientInfo.product_info.product_id.minor_version	= 0;
	clientInfo.product_info.market_sector_id			= 1;  // ?
	clientInfo.product_info.sales_channel_id			= 1;  // ?
	clientInfo.product_info.localization_id				= 1;  // ?
	clientInfo.product_info.package						= 1;  // ?
	clientInfo.platform_id								= PT_WIN_WORKSTATION;
}

void Fill_ProgramClientInfo_LocalizationId(ProgramClientInfo& clientInfo)
{
	clientInfo.version_type = ProgramClientInfo::VER_TYPE_RELEASE;
	clientInfo.product_info.product_id.id				= 1000;  // ?
	clientInfo.product_info.product_id.type				= LicenseObjectId::OBJ_TYPE_PRODUCT;
	clientInfo.product_info.product_id.major_version	= 6;
	clientInfo.product_info.product_id.minor_version	= 0;
	clientInfo.product_info.market_sector_id			= 1;  // ?
	clientInfo.product_info.sales_channel_id			= 1;  // ?
	clientInfo.product_info.localization_id				= 2;  // in key 1
	clientInfo.product_info.package						= 1;  // ?
	clientInfo.platform_id								= PT_WIN_WORKSTATION;
}

void Fill_ProgramClientInfo_SalesChannel(ProgramClientInfo& clientInfo)
{
	clientInfo.version_type = ProgramClientInfo::VER_TYPE_RELEASE;
	clientInfo.product_info.product_id.id				= 1000;  // ?
	clientInfo.product_info.product_id.type				= LicenseObjectId::OBJ_TYPE_PRODUCT;
	clientInfo.product_info.product_id.major_version	= 6;
	clientInfo.product_info.product_id.minor_version	= 0;
	clientInfo.product_info.market_sector_id			= 1;  // ?
	clientInfo.product_info.sales_channel_id			= 2;  // in key 1
	clientInfo.product_info.localization_id				= 1;  // ?
	clientInfo.product_info.package						= 1;  // ?
	clientInfo.platform_id								= PT_WIN_WORKSTATION;
}

void Fill_ProgramClientInfo_MarketSector(ProgramClientInfo& clientInfo)
{
	clientInfo.version_type = ProgramClientInfo::VER_TYPE_RELEASE;
	clientInfo.product_info.product_id.id				= 1000;  // ?
	clientInfo.product_info.product_id.type				= LicenseObjectId::OBJ_TYPE_PRODUCT;
	clientInfo.product_info.product_id.major_version	= 6;
	clientInfo.product_info.product_id.minor_version	= 0;
	clientInfo.product_info.market_sector_id			= 2;  // in key 1
	clientInfo.product_info.sales_channel_id			= 1;  // ?
	clientInfo.product_info.localization_id				= 1;  // ?
	clientInfo.product_info.package						= 1;  // ?
	clientInfo.platform_id								= PT_WIN_WORKSTATION;
}

void Fill_ProgramClientInfo_Package(ProgramClientInfo& clientInfo)
{
	clientInfo.version_type = ProgramClientInfo::VER_TYPE_RELEASE;
	clientInfo.product_info.product_id.id				= 1000;  // ?
	clientInfo.product_info.product_id.type				= LicenseObjectId::OBJ_TYPE_PRODUCT;
	clientInfo.product_info.product_id.major_version	= 6;
	clientInfo.product_info.product_id.minor_version	= 0;
	clientInfo.product_info.market_sector_id			= 1;  // ?
	clientInfo.product_info.sales_channel_id			= 1;  // ?
	clientInfo.product_info.localization_id				= 1;  // ?
	clientInfo.product_info.package						= 2;  // in key 1
	clientInfo.platform_id								= PT_WIN_WORKSTATION;
}

void Fill_ProgramClientInfo_IdenticalSerials_step1(ProgramClientInfo& clientInfo)
{
	clientInfo.version_type = ProgramClientInfo::VER_TYPE_RELEASE;
	clientInfo.product_info.product_id.id				= 1000;  // ?
	clientInfo.product_info.product_id.type				= LicenseObjectId::OBJ_TYPE_PRODUCT;
	clientInfo.product_info.product_id.major_version	= 6;
	clientInfo.product_info.product_id.minor_version	= 0;
	clientInfo.product_info.market_sector_id			= 1;  // ?
	clientInfo.product_info.sales_channel_id			= 1;  // ?
	clientInfo.product_info.localization_id				= 1;  // ?
	clientInfo.product_info.package						= 1;  // ?
	clientInfo.platform_id								= PT_WIN_WORKSTATION;
}

void Fill_ProgramClientInfo_IdenticalSerials_step2(ProgramClientInfo& clientInfo)
{
	clientInfo.version_type = ProgramClientInfo::VER_TYPE_RELEASE;
	clientInfo.product_info.product_id.id				= 1000;  // ?
	clientInfo.product_info.product_id.type				= LicenseObjectId::OBJ_TYPE_PRODUCT;
	clientInfo.product_info.product_id.major_version	= 6;
	clientInfo.product_info.product_id.minor_version	= 0;
	clientInfo.product_info.market_sector_id			= 1;  // ?
	clientInfo.product_info.sales_channel_id			= 1;  // ?
	clientInfo.product_info.localization_id				= 1;  // ?
	clientInfo.product_info.package						= 1;  // ?
	clientInfo.platform_id								= PT_WIN_WORKSTATION;
}

void Fill_ProgramClientInfo_GetInstalledLicenseKeys1(ProgramClientInfo& clientInfo)
{
	clientInfo.version_type = ProgramClientInfo::VER_TYPE_RELEASE;
	clientInfo.product_info.product_id.id				= 1000;  // ?
	clientInfo.product_info.product_id.type				= LicenseObjectId::OBJ_TYPE_PRODUCT;
	clientInfo.product_info.product_id.major_version	= 6;
	clientInfo.product_info.product_id.minor_version	= 0;
	clientInfo.product_info.market_sector_id			= 1;  // ?
	clientInfo.product_info.sales_channel_id			= 1;  // ?
	clientInfo.product_info.localization_id				= 1;  // ?
	clientInfo.product_info.package						= 1;  // ?
	clientInfo.platform_id								= PT_WIN_WORKSTATION;
}
}  // namespace Test
}  // namespace KasperskyLicensing
