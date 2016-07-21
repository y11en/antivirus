/*!
 * (C) 2002 "Kaspersky Lab"
 *
 * \file PRSS/SettingsStorage.h
 * \author Андрей Казачков
 * \date 2002
 * \brief Главный интерфейс модуля Product Settings Storage.
 *
 */

#ifndef KLPRSS_SETTINGSSTORAGE_H
#define KLPRSS_SETTINGSSTORAGE_H


namespace KLPRSS
{

	/*!
		\brief	Максимальная длина, в символах, строки имени продукта,
				версии, секции (без нулевого символа).
	*/
	const int c_nMaxNameLength=31;


	//! Информация о лицензионных ключах в Product Info Settings Storage
	#define KLPRSS_SECTION_LIC_INFO			L"LicensingInfo"
	    //! информация об активном лицензионном ключе
	    #define KLPRSS_VAL_LIC_KEY_CURRENT		L"CurrentKey"
	    //! информация о резервном лицензионном ключе
	    #define KLPRSS_VAL_LIC_KEY_NEXT			L"NextKey"

    /*! application components-to-update list section KLPRSS_SECTION_PRODCOMP
        Section contains KLPRSS_VAL_PRODCOMPS variable which is array of params
        Each array item has at least two variables:
            KLPRSS_VAL_NAME     component name
            KLPRSS_VAL_VERSION  component version
    */
    #define KLPRSS_SECTION_PRODCOMP			L"ProductComponents"
        //!array of params that contain components info
        #define KLPRSS_VAL_PRODCOMPS        L"Components"   //ARRAY_T|PARAMS_T

    #define KLPRSS_SECTION_PRODFIXES		L"ProductFixes"
            //KLCONN_VAL_FIXES


	/*!
		\brief	Стандартные переменные.
	*/

	#define KLPRSS_VAL_NAME					L"Name"
	#define KLPRSS_VAL_VERSION				L"Version"
	#define KLPRSS_VAL_INSTALLTIME			L"InstallTime"
	#define KLPRSS_VAL_MODULETYPE			L"ModuleType"
	#define KLPRSS_VAL_STARTFLAGS			L"StartFlags"
	#define KLPRSS_VAL_FILENAME				L"FileName"
	#define KLPRSS_VAL_FILEPATH				L"FilePath"
	#define KLPRSS_VAL_CUSTOMNAME			L"CustomName"
	#define KLPRSS_VAL_WELLKNOWNNAME		L"WellKnown"
	#define KLPRSS_VAL_TASKS				L"Tasks"
    #define KLPRSS_VAL_TASKS_COMPLEMENTED	L"TasksComplemented"
	#define KLPRSS_VAL_EVENTS				L"Events"
	#define KLPRSS_VAL_KILLTIMEOUT			L"KillTimeout"
    #define KLPRSS_VAL_PINGTIMEOUT			L"PingTimeout"
    #define KLPRSS_VAL_ACCEPTED_CMDS        L"AcceptedCmds"
    #define KLPRSS_VAL_LOCAL_ONLY           L"LocalOnly"


	#define KLPRSS_VAL_INSTANCES			L"Instances"
	#define KLPRSS_VAL_LASTSTART			L"LastStartTime"
	#define KLPRSS_VAL_LASTSTOP				L"LastStopTime"
	#define KLPRSS_VAL_INSTANCEPORT			L"InstancePort"
    #define KLPRSS_VAL_CONNTYPE			    L"ConnectionType"
	#define KLPRSS_VAL_CONNECTIONNAME		L"ConnectionName"    
	#define KLPRSS_VAL_PID					L"Pid"

};


/*!
	\brief Информация о продукте.
*/
    
    //! Корневая папка продукта STRING_T
    #define KLPRSS_PRODVAL_INSTALLFOLDER    L"Folder"

    //! KLPRSS_PRODVAL_INSTALLTIME
    #define KLPRSS_PRODVAL_INSTALLTIME      KLPRSS_VAL_INSTALLTIME

    //! KLPRSS_PRODVAL_LASTUPDATETIME
    #define KLPRSS_PRODVAL_LASTUPDATETIME   L"LastUpdateTime"

    //! KLPRSS_PRODVAL_DISPLAYNAME
    #define KLPRSS_PRODVAL_DISPLAYNAME      L"DisplayName"

	//! Папка антивирусных баз продукта STRING_T
	#define KLPRSS_PRODVAL_BASEFOLDER		L"BaseFolder"

    //! Дата антивирусных баз, DATE_TIME_T.
    #define KLPRSS_PRODVAL_BASEDATE         L"BaseDate"
	
    //! Дата обновления антивирусных баз, DATE_TIME_T.
    #define KLPRSS_PRODVAL_BASEINSTALLDATE  L"BaseInstallDate"

    //! Количество записей в антивирусных базах, INT_T
    #define KLPRSS_PRODVAL_BASERECORDS      L"BaseRecords"

	//! Папка с ключами STRING_T
	#define KLPRSS_PRODVAL_KEYFOLDER		L"KeyFolder"
	
	//! Локализация продукта	STRING_T
	#define KLPRSS_PRODVAL_LOCID			L"LocID"

	//! Product full version in w.x.y.z format, STRING_T
	#define KLPRSS_PRODVAL_VERSION			L"ProdVersion"

	//! Connector full version in w.x.y.z format, STRING_T
	#define KLPRSS_CONNECTOR_VERSION        L"ConnectorVersion"

	//! Connector flags, INT_T
	#define KLPRSS_CONNECTOR_FLAGS          L"ConnectorFlags"

	//! Connector component name, STRING_T
	#define KLPRSS_CONNECTOR_COMPONENT_NAME L"ConnectorComponentName"

	//! Connector instance id, STRING_T
	#define KLPRSS_CONNECTOR_INSTANCE_ID    L"ConnectorInstanceId"

	//! Название ОС				STRING_T
	#define KLPRSS_PRODVAL_OS_NAME			L"OsName"

	//! Версия ОС				STRING_T
	#define KLPRSS_PRODVAL_OS_VERSION		L"OsVersion"

	//! Релиз ОС				STRING_T
	#define KLPRSS_PRODVAL_OS_RELEASE		L"OsRelease"


#endif // KLPRSS_SETTINGSSTORAGE_H
