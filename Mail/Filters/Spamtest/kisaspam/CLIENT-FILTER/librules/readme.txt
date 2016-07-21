LibRules - библиотека интерпретатора формальных правил.

Вызывает функции библиотеки контентной фильтрации.
Вызывавается из SpamTest.dll

Используемые библиотеки:

#include <lib/pcommon.link.h>
#include "MIMEParser/MIMEParser.link.h"
#include <ZTools/ExpatUtils/ExpatUtils.link.h>
#include <ZTools/ZUtils/ZUtils.link.h>
#include <gnu/libexpat.link.h>
#include <gnu/libdb1.link.h>
#include <gnu/libregex.link.h>
#include <gnu/libnet.link.h>

#include <smtp-filter/adc/adc.link.h>
#include <smtp-filter/contrib/libpatricia/libpatricia.link.h>
#include <smtp-filter/adc/adc.link.h>
#include <smtp-filter/paths/paths.link.h>
#include <smtp-filter/filter-config/xmlfiles/xmlfiles.link.h>
#include <smtp-filter/common/common.link.h>

#include <gnu/libmd5a.link.h>
#include <smtp-filter/libsigs/libsigs.link.h>

#include <smtp-filter/libgsg2/libgsg2.link.h>
#include <smtp-filter/ungif/ungif.link.h>
#include <gnu/libjpeg.link.h>

Процедура сборки библиотек smtp-filter:

cd smtp-filter
upkasp11_libs.cmd - берет фиксированную версию библиотек по сvs tag'у
kasplibs.dsw -> Batch Build/Build All
