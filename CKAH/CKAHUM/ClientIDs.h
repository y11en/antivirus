#ifndef __CLIENTIDS_H__
#define __CLIENTIDS_H__

#define CLIENT_APP_ID_IDS				(PersonalFireWall + 10)
#define CLIENT_APP_ID_FW_PACKET			(PersonalFireWall + 11)
#define CLIENT_APP_ID_FW_APPLICATION	(PersonalFireWall + 12)

// фильтр для выгрузки Personal деинсталятором
#define CLIENT_APP_ID_PERSONAL_UNLOAD_WATCHER (PersonalFireWall + 90)
// Personal - первый старт (определение разрешенности драйверов)
#define CLIENT_APP_ID_PERSONAL_KLCKAH_BASEDRV_CHECK	(PersonalFireWall + 91)
// Personal - определения запросов на перезагрузку в ckahum
#define CLIENT_APP_ID_REBOOTHLP			(PersonalFireWall + 99)

#endif