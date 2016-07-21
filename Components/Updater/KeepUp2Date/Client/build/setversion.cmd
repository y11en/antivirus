rem *** forming new version info files

del /f %1\commonfiles\Version\ver_av50.h
echo #ifndef __VER_AV50_H							>> %1\commonfiles\Version\ver_av50.h
	echo #define __VER_AV50_H						>> %1\commonfiles\Version\ver_av50.h
	echo #define VER_PRODUCTNAME_STR                "Updater for SCM4Exchange" >> %1\commonfiles\Version\ver_av50.h
	echo #define VER_PRODUCTVERSION_HIGH             1			>> %1\commonfiles\Version\ver_av50.h
	echo #define VER_PRODUCTVERSION_LOW              0			>> %1\commonfiles\Version\ver_av50.h
	echo #define VER_PRODUCTVERSION_BUILD            0			>> %1\commonfiles\Version\ver_av50.h
	echo #define VER_PRODUCTVERSION_COMPILATION      %KL_BUILDNO%		>> %1\commonfiles\Version\ver_av50.h
	echo #define VER_FILEVERSION_BUILD               0			>> %1\commonfiles\Version\ver_av50.h
	echo #define VER_FILEVERSION_COMPILATION         %KL_BUILDNO%		>> %1\commonfiles\Version\ver_av50.h
	echo #include "ver_kl.h"						>> %1\commonfiles\Version\ver_av50.h
echo #endif//__VER_AV50_H							>> %1\commonfiles\Version\ver_av50.h

del /f %1\commonfiles\Version\ver_product.h
echo #ifndef __VER_PRODUCT_H							>> %1\commonfiles\Version\ver_product.h
	echo #define __VER_PRODUCT_H						>> %1\commonfiles\Version\ver_product.h
	echo #define VER_PRODUCTNAME_STR                "Updater for SCM4Exchange" >> %1\commonfiles\Version\ver_product.h
	echo #define VER_PRODUCTVERSION_HIGH             1			>> %1\commonfiles\Version\ver_product.h
	echo #define VER_PRODUCTVERSION_LOW              0			>> %1\commonfiles\Version\ver_product.h
	echo #define VER_PRODUCTVERSION_BUILD            0			>> %1\commonfiles\Version\ver_product.h
	echo #define VER_PRODUCTVERSION_COMPILATION      %KL_BUILDNO%		>> %1\commonfiles\Version\ver_product.h
	echo #define VER_FILEVERSION_BUILD               0			>> %1\commonfiles\Version\ver_product.h
	echo #define VER_FILEVERSION_COMPILATION         %KL_BUILDNO%		>> %1\commonfiles\Version\ver_product.h
	echo #include "ver_kl.h"						>> %1\commonfiles\Version\ver_product.h
echo #endif//__VER_PRODUCT_H							>> %1\commonfiles\Version\ver_product.h