#ifndef __S_AVZSCAN_H
#define __S_AVZSCAN_H

#include <plugin/p_avzscan.h>
#include <structs/s_taskmanager.h>
#include <structs/s_taskmanager.h>

// Пример структуры парсинга XML: cAVZScanPROCESS
// SerId этой сьруктуры нужно передать в cAVZScanSettings.m_dwXMLSerId, 
// а в cAVZScanSettings.m_dwXMLTag записать имя тега (ветки XML-файла)
struct cAVZScanPROCESS : public cSerializable
{
	cAVZScanPROCESS(){};
	DECLARE_IID( cAVZScanPROCESS, cSerializable, PID_AVZSCAN, 0 );

	tDWORD PID;
	cStringObj File;
	cStringObj Descr;		
	tDWORD Hidden;	
	cStringObj CmdLine;		
	tDWORD Size;	
	cStringObj Attr;		
	cStringObj CreateDate;		
	cStringObj ChageDate;	
	cStringObj MD5;	
};

IMPLEMENT_SERIALIZABLE_BEGIN( cAVZScanPROCESS, 0 )
	SER_VALUE( PID,       tid_DWORD,      "PID" )
	SER_VALUE( File,      tid_STRING_OBJ, "File" )
	SER_VALUE( Descr,     tid_STRING_OBJ, "Descr" )
	SER_VALUE( Hidden,    tid_DWORD,      "Hidden" )
	SER_VALUE( CmdLine,   tid_STRING_OBJ, "CmdLine" )
	SER_VALUE( Size,      tid_DWORD,      "Size" )
	SER_VALUE( Attr,      tid_STRING_OBJ, "Attr" )
	SER_VALUE( CreateDate,tid_STRING_OBJ, "CreateDate" )
	SER_VALUE( ChageDate, tid_STRING_OBJ, "ChageDate" )
	SER_VALUE( MD5,       tid_STRING_OBJ, "MD5" )
IMPLEMENT_SERIALIZABLE_END()

// Пример простого результата исполнения скрипта
struct cAVZScanResult : public cSerializable
{
	cAVZScanResult(): dwResult(0), bHasBackup(cFALSE) {};
	DECLARE_IID( cAVZScanResult, cSerializable, PID_AVZSCAN, 1 );

	tDWORD dwResult;
	cStringObj szResult;
	tBOOL bHasBackup;
};

IMPLEMENT_SERIALIZABLE_BEGIN( cAVZScanResult, 0 )
	SER_VALUE( szResult,   tid_STRING_OBJ, "szResult"   )
	SER_VALUE( dwResult,   tid_DWORD,      "dwResult"   )
	SER_VALUE( bHasBackup, tid_BOOL,       "bHasBackup" )
IMPLEMENT_SERIALIZABLE_END()

// Вектор Custom-скриптов для пакетного исполнения
struct cAVZCustomScript : public cSerializable
{
	cAVZCustomScript(): dwScriptID(0) {};
	DECLARE_IID( cAVZCustomScript, cSerializable, PID_AVZSCAN, 2 );

	cStringObj szScriptBase;
	tDWORD dwScriptID;
};
IMPLEMENT_SERIALIZABLE_BEGIN( cAVZCustomScript, 0 )
	SER_VALUE( szScriptBase, tid_STRING_OBJ, "ScriptBase"   )
	SER_VALUE( dwScriptID,   tid_DWORD,      "dwScriptID"   )
IMPLEMENT_SERIALIZABLE_END()
typedef cVector<cAVZCustomScript> cAVZCustomScripts;

struct cAVZScanSettings : public cTaskSettings
{
	cAVZScanSettings():
		m_szLogFileName("LOG\\avptool_syscheck.zip"),
		m_dwScriptMode(0),
		m_dwXMLSerId(0)
		//m_szXMLTag("AVZ\\PROCESS"),
		//m_dwXMLSerId(cAVZScanPROCESS::eIID)
		{};

	DECLARE_IID_SETTINGS( cAVZScanSettings, cTaskSettings, PID_AVZSCAN );

	// Для запуска можно скрипт в виде текста
	cStringObj m_szScript;	

	// ... либо из текстового файла
	cStringObj m_szScriptFile;

	// ... либо указать номера скриптов из соответствующей базы...
	cStringObj m_szScriptBase;      // Папка, где хранятся базы кастом-скриптов
	cStringObj m_szScriptBackupBase;// Папка для хранения бекапов
	tDWORD m_dwScriptMode;          // Тип запуска кастом-скриптов: 1=check, 2=fix
	cAVZCustomScripts m_aScriptID;  // Вектор кастом-скриптов

	// Результат работы скрипта можно сложить в файл
	cStringObj m_szLogFileName;	

	// XML-Результат работы скрипта можно преобразовать в вектор структур 
	cStringObj m_szXMLTag;
	tDWORD m_dwXMLSerId;
};

IMPLEMENT_SERIALIZABLE_BEGIN( cAVZScanSettings, 0 )
	SER_BASE( cTaskSettings,  0 )
	SER_VALUE( m_szScript,             tid_STRING_OBJ, "Script" )
	SER_VALUE( m_szScriptFile,         tid_STRING_OBJ, "ScriptFile" )
	SER_VALUE( m_szScriptBase,         tid_STRING_OBJ, "ScriptBase" )
	SER_VALUE( m_szScriptBackupBase,   tid_STRING_OBJ, "ScriptBackupBase" )
	SER_VALUE( m_dwScriptMode,         tid_DWORD,      "ScriptMode" )
	SER_VECTOR( m_aScriptID,   cAVZCustomScript::eIID, "ScriptID" )
	SER_VALUE( m_szLogFileName,        tid_STRING_OBJ, "LogFileName" )
	SER_VALUE( m_szXMLTag,             tid_STRING_OBJ, "XMLTag" )
	SER_VALUE( m_dwXMLSerId,           tid_DWORD,      "XMLSerId" )
IMPLEMENT_SERIALIZABLE_END()


typedef cVector<cSerializableObj, cSimpleMover<cSerializableObj>> cXMLRecords;
struct cAVZScanStatistics : public cTaskStatistics
{
	cAVZScanStatistics() : 
		m_bNeedReboot(cFALSE)
	{};

	DECLARE_IID_STATISTICS( cAVZScanStatistics, cTaskStatistics, PID_AVZSCAN );

	cStringObj m_szLastEvent;
	cXMLRecords m_aXMLRecords;
	tBOOL m_bNeedReboot;
};

IMPLEMENT_SERIALIZABLE_BEGIN( cAVZScanStatistics, 0 )
	SER_BASE( cTaskStatistics,  0 )
	SER_VALUE(         m_szLastEvent,   tid_STRING_OBJ,       "LastEvent"  )
	SER_VECTOR_OF_PTR( m_aXMLRecords,	cSerializable::eIID,  "XMLRecords" )
	SER_VALUE(         m_bNeedReboot,   tid_BOOL,             "NeedReboot" )
IMPLEMENT_SERIALIZABLE_END()

#endif //__S_AVZSCAN_H
