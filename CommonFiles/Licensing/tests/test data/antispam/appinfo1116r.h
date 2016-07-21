#include "apversion.h"
#include <appinfo_struct.h>

static _product_info_t PRODUCTS_LIST[] =
{
	{
		LIC_PROD_NUM, 
		LIC_PROD_STRING, 
		LIC_PROD_MAJORVERSION_STRING, 
		0 					 // publick key id
	}
};

_app_info_t AppInfo = 
{
	LIC_APP_DESCRIPTION_STRING,       // Description
	1,                                           // App info structure version
	LIC_APP_CREATE_DATE,                          // Date create
	aitApplicationInfo,                          // application info type (app or component)
	1116,                                        // Application Id
	"Kaspersky Personal Anti-Spam",          // Application Name
	"",                                          // Application version
	IS_RELEASE(),                                // 0 - beta, not 0 - release (app only)
	{  0, 0 },                                   // Components list (app only)
	{  0, 0 },                                   // Applications list (component only)
	{  1, PRODUCTS_LIST },                       // Products list
	{  0, 0 },                                   // Public key list
	{  0, 0 }                                    // Legacy applications compatibility info
};
