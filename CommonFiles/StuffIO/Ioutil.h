#ifndef __IOUTIL_H
#define __IOUTIL_H

#include <windows.h>
#include <tchar.h>
#include "../stuff/stddefs.h"
#include "xerrors.h"


// Стуктуры для функции IOSGetFileInfo
//--
typedef struct {
  uint uiSecond;
  uint uiMinute;
  uint uiHour;
  uint uiDay;
  uint uiDayOfWeek;
  uint uiMonth;
  uint uiYear;
} IOSTIME, *PIOSTIME;

//--
typedef struct {
  uint32 dwcbSize;
  IOSTIME fioTime;
} IOSFILEINFO, *PIOSFILEINFO; 

// Получить информацию о файле
// pszFileName - полное имя файла
// pfi         - указатель на информационную структуру
IOS_ERROR_CODE IOSGetFileInfo(const TCHAR * pszFileName, PIOSFILEINFO pfi);
IOS_ERROR_CODE IOSGetFileInfoW(const WCHAR * pszFileName, PIOSFILEINFO pfi);


// Копировать файл
// pszSrc        - полное имя исходного файла
// pszDst        - полное имя выходного файлф
// fFailIfExists - true - если нельзя выполнять копирование на существующий файл
IOS_ERROR_CODE IOSCopyFile( const TCHAR * pszSrc, const TCHAR * pszDst, bool fFailIfExists = false );
IOS_ERROR_CODE IOSCopyFileW( const WCHAR * pszSrc, const WCHAR * pszDst, bool fFailIfExists = false );


// Удалить файл
// pszFileName - полное имя файла
IOS_ERROR_CODE IOSDeleteFile( const TCHAR * pszFileName );
IOS_ERROR_CODE IOSDeleteFileW( const WCHAR * pszFileName );


// Переименовать файл
// pszOldFileName  - полное имя исходного файла
// pszNewFileName  - полное имя выходного файла
IOS_ERROR_CODE IOSRenameFile( const TCHAR * pszOldFileName, const TCHAR * pszNewFileName );


// Создать каталог
// pszDirName - полное имя каталога
IOS_ERROR_CODE IOSCreateDirectory( const TCHAR * pszDirName );
IOS_ERROR_CODE IOSCreateDirectoryW( const WCHAR * pszDirName );


// Удалить каталог
// pszDirName - полное имя каталога
IOS_ERROR_CODE IOSDeleteDirectory( const TCHAR * pszDirName );


// Получить текущий каталог задачи
// pszDirBuf    - буфер для имени каталога
// uicchBufSize - размер буфера
IOS_ERROR_CODE IOSGetCurrentDirectory( TCHAR * pszDirBuf, uint uicchBufSize, ulong *puicchBuffSizeNeeded = NULL );
IOS_ERROR_CODE IOSGetCurrentDirectoryW( wchar_t * pszDirBuf, uint uicchBufSize, ulong *puicchBuffSizeNeeded = NULL );


// Установить текущий каталог задачи
// pszDirName - имя каталога
IOS_ERROR_CODE IOSSetCurrentDirectory( const TCHAR * pszDirName );
IOS_ERROR_CODE IOSSetCurrentDirectoryW( const wchar_t * pszDirName );


// Проверить возможность записи в указанный файл
// pszFileName     - полное имя файла
// fCheckDirectory - true - если нужно проверять возможность записи в каталог
//                          размещения файла           
IOS_ERROR_CODE IOSCheckWritableFile( const TCHAR * pszFileName, bool fCheckDirectory = true );


// Проверить возможность записи в указанный каталог
// pszDirName - имя каталога
IOS_ERROR_CODE IOSCheckWritableDir( const TCHAR * pszDirName );
IOS_ERROR_CODE IOSCheckWritableDirW( const WCHAR * pszDirName );


// Проверить - является ли данное имя именем каталога
// pszFileName - проверяемое имя
IOS_ERROR_CODE IOSGetIsDirectory( const TCHAR * pszFileName );
IOS_ERROR_CODE IOSGetIsDirectoryW( const WCHAR * pszFileName );


// Выдать размер файла в файтах
// pszFileName      - имя файла
// pdwcbyFileLength - выходное значение - размер файла
IOS_ERROR_CODE IOSFileLengthName( const TCHAR * pszFileName, uint32 * pdwcbyFileLength );
IOS_ERROR_CODE IOSFileLengthNameW( const WCHAR * pszFileName, uint32 * pdwcbyFileLength );


// Поверить наличие файла с заданным именем
// pszFileName - полное имя файла
IOS_ERROR_CODE IOSFileExists( const TCHAR * pszFileName );
IOS_ERROR_CODE IOSFileExistsW( const WCHAR * pszFileName );


// Построить полное имя файла
// pszRelativePath - относительное имя файла
// pszBuffer       - буфер под полное имя
// uicchBufferLen  - размер буфера
IOS_ERROR_CODE IOSMakeFullPath( const TCHAR * pszRelativePath, TCHAR * pszBuffer, uint uicchBufferLen );
IOS_ERROR_CODE IOSMakeFullPathW( const WCHAR * pszRelativePath, WCHAR * pszBuffer, uint uicchBufferLen );


// Извлечь путь из полного имени файла
// pszFileName    - полное имя файла
// pszDirName     - буфер под выходное имя каталога
// uicchBufferLen - размер буфера
IOS_ERROR_CODE IOSDirExtract( const TCHAR * pszFileName, TCHAR * pszDirName, uint uicchBufferLen );
IOS_ERROR_CODE IOSDirExtractW( const WCHAR * pszFileName, WCHAR * pszDirName, uint uicchBufferLen );


// Извлечь имя корневого каталога из полного имени файла
// pszFileName    - полное имя файла
// pszDirName     - буфер под выходное имя каталога
// uicchBufferLen - размер буфера
IOS_ERROR_CODE IOSRootDirExtract( const TCHAR * pszFileName, TCHAR * pszDirName, uint uicchBufferLen );
IOS_ERROR_CODE IOSRootDirExtractW( const WCHAR * pszFileName, WCHAR * pszDirName, uint uicchBufferLen );


// Извлечь имя файла с расширением из полного имени файла
// pszFullFileName  - полное имя файла
// pszFileName      - буфер под имя файла
// uicchBufferLen   - размер буфера
IOS_ERROR_CODE IOSFileExtract( const TCHAR * pszFullFileName, TCHAR * pszFileName, uint uicchBufferLen );
IOS_ERROR_CODE IOSFileExtractW( const WCHAR * pszFullFileName, WCHAR * pszFileName, uint uicchBufferLen );


// Извлечь имя файла без расширения из полного имени файла
// pszFullFileName  - полное имя файла
// pszFileName      - буфер под имя файла
// uicchBufferLen   - размер буфера
IOS_ERROR_CODE IOSPureFileExtract( const TCHAR * pszFullFileName, TCHAR * pszFileName, uint uicchBufferLen );


// Найти позицию расширения в имени файла
// pszFileName - имя файла
// realExt     - true - если нужно выдать позицию перевого символа в расширении
//                      иначе выдается позиция '.'
TCHAR 				 * IOSFindFileExtention( const TCHAR *pszFileName, bool realExt = false );
WCHAR 				 * IOSFindFileExtentionW( const WCHAR *pszFileName, bool realExt = false );


// Построить относительное имя файла относительно заданного файла
// pszMainFileName  - имя задающего файла
// pszRelFileName   - имя обрабатываемого файла
// pszBuffer        - буфер под выходное имя
// uicchBufferLen   - размер буфера
IOS_ERROR_CODE IOSRelativePathFromFullPath( const TCHAR * pszMainFileName, const TCHAR * pszRelFileName, TCHAR * pszBuffer, uint uicchBufferLen );



// Построить полное имя файла из относительного
// pszMainFileName  - имя задающего файла
// pszRelFileName   - имя обрабатываемого файла
// pszBuffer        - буфер под выходное имя
// uicchBufferLen   - размер буфера
IOS_ERROR_CODE IOSFullPathFromRelativePath( const TCHAR * pszMainFileName, const TCHAR * pszRelFileName, TCHAR * pszBuffer, uint uicchBufferLen );
IOS_ERROR_CODE IOSFullPathFromRelativePathW( const WCHAR * pszMainFileName, const WCHAR * pszRelFileName, WCHAR * pszBuffer, uint uicchBufferLen );


// Преобразовать (если возможно) локальное имя в имя в формате UNC
// pszLocalPath - исходное локальное имя
// pszUNCPath   - буфер под выходное имя в формате UNC
// uicchUNCLen  - размер буфера
IOS_ERROR_CODE IOSLocalPathToUNC( const TCHAR *pszLocalPath, TCHAR *pszUNCPath, uint uicchUNCLen );
IOS_ERROR_CODE IOSLocalPathToUNCW( const WCHAR *pszLocalPath, WCHAR *pszUNCPath, uint uicchUNCLen );


// Преобразовать короткое имя в длинное
// pszFileName    - имя файла
// pszBuffer      - выходной буфер
// uicchBufferLen - размер буфера
IOS_ERROR_CODE IOSGetLongPathName( const TCHAR *pszFileName, TCHAR *szBuffer, uint uicchBufferLen );


// Преобразовать имя файла в базированный синоним
// pszSourcePath  - исходное имя файла
// pszBasePath    - базовый путь
// pszAlias       - синоним дла замены пути
// pszBuffer      - выходной буфер
// uicchBufferLen - размер буфера
// Пример :
// pszSourcePath  - "с:\myfiles\myprojects\firstproject\shelf.cdw"
// pszBasePath    - "с:\myfiles\myprojects"
// pszAlias       - "allprojects"
// pszBuffer      - "<allprojects>\firstproject\shelf.cdw"
IOS_ERROR_CODE IOSFullPathToBased( const TCHAR *pszSourcePath, const TCHAR *pszBasePath, const TCHAR *pszAlias, TCHAR *pszBuffer, uint uicchBufferLen );



// Преобразовать базированный синоним в полное имя файла
// pszSourcePath  - исходное имя файла
// pszBasePath    - базовый путь
// pszAlias       - синоним дла замены пути
// pszBuffer      - выходной буфер
// uicchBufferLen - размер буфера
// Пример :
// pszSourcePath  - "<allprojects>\firstproject\shelf.cdw"
// pszBasePath    - "с:\myfiles\myprojects"
// pszAlias       - "allprojects"
// pszBuffer      - "с:\myfiles\myprojects\firstproject\shelf.cdw"
IOS_ERROR_CODE IOSBasedPathToFull( const TCHAR *pszSourcePath, const TCHAR *pszBasePath, const TCHAR *pszAlias, TCHAR *pszBuffer, uint uicchBufferLen );


// Искать файл по заданной схеме поиска
// pszFileName    - имя файла
// szBuffer       - буфер под выходное имя
// uicchBufferLen - размер буфера
// pszFindPattern - шаблон схемы поиска

// Схема поиска
// 1 - каталог запуска задачи
// 2 - текущий каталог
// 3 - каталог Windows
// 4 - каталог Windows\System или WindowsNT/System32
// 5 - каталог WindowsNT\System16
// 6 - каталоги Path 

IOS_ERROR_CODE IOSFindFile( const TCHAR *pszFileName, TCHAR *szBuffer, uint uicchBufferLen, const TCHAR *pszFindPattern = _T("21346"));

// Преобразовать имя файла в отображаемое имя
// pszFileName    - имя файла
// szBuffer       - буфер под выходное имя
// uicchBufferLen - размер буфера
IOS_ERROR_CODE IOSGetFileTitle( const TCHAR *pszFileName, TCHAR *szBuffer, uint uicchBufferLen );

// Проверить возможность создания/записи в файл с заданным именем
// pszFileName    - имя файла
IOS_ERROR_CODE IOSCheckCreatableFile( const TCHAR *pszFileName );

// Запросить тип устройства
UINT IOSGetDriveType( const TCHAR *pszDriveName );
UINT IOSGetDriveTypeW( const WCHAR *pszDriveName );

// Запросить атрибуты файла
DWORD IOSGetFileAttributes( const TCHAR *pszFileName );
DWORD IOSGetFileAttributesW( const WCHAR *pszFileName );

IOS_ERROR_CODE IOSGetFileAttributesEx( const TCHAR *pszFileName, GET_FILEEX_INFO_LEVELS fInfoLevelId, LPVOID lpFileInformation );
IOS_ERROR_CODE IOSGetFileAttributesExW( const WCHAR *pszFileName, GET_FILEEX_INFO_LEVELS fInfoLevelId, LPVOID lpFileInformation );


// Установить атрибуты файла
BOOL IOSSetFileAttributes( const TCHAR *pszFileName, DWORD dwFileAttributes );
BOOL IOSSetFileAttributesW( const WCHAR *pszFileName, DWORD dwFileAttributes );

// Получить короткое имя файла 
DWORD IOSGetShortPathName( const TCHAR *lpszLongPath, TCHAR *lpszShortPath, DWORD cchBuffer );
DWORD IOSGetShortPathNameW( const WCHAR *lpszLongPath, WCHAR *lpszShortPath, DWORD cchBuffer );

// Получить каталог временных файлов 
DWORD IOSGetTempPath( DWORD cchBuffer, TCHAR *pchBuffer );
DWORD IOSGetTempPathW( DWORD cchBuffer, WCHAR *pchBuffer );

// Получить путь на временный файл
UINT IOSGetTempFileName( const TCHAR *lpPathName, const TCHAR *lpPrefixString, UINT uUnique, TCHAR *lpTempFileName );
UINT IOSGetTempFileNameW( const WCHAR *lpPathName, const WCHAR *lpPrefixString, UINT uUnique, WCHAR *lpTempFileName );

// Получить информацию об устройстве
//  LPCTSTR lpRootPathName,           // root directory
//  LPTSTR	lpVolumeNameBuffer,        // volume name buffer
//  DWORD		nVolumeNameSize,            // length of name buffer
//  LPDWORD lpVolumeSerialNumber,     // volume serial number
//  LPDWORD lpMaximumComponentLength, // maximum file name length
//  LPDWORD lpFileSystemFlags,        // file system options
//  LPTSTR	lpFileSystemNameBuffer,    // file system name buffer
//  DWORD		nFileSystemNameSize         // length of file system name buffer
BOOL IOSGetVolumeInformation( LPCTSTR lpRootPathName, LPTSTR lpVolumeNameBuffer, DWORD nVolumeNameSize, LPDWORD lpVolumeSerialNumber, LPDWORD lpMaximumComponentLength, LPDWORD lpFileSystemFlags, LPTSTR lpFileSystemNameBuffer, DWORD nFileSystemNameSize );
BOOL IOSGetVolumeInformationW( LPCWSTR lpRootPathName, LPWSTR lpVolumeNameBuffer, DWORD nVolumeNameSize, LPDWORD lpVolumeSerialNumber, LPDWORD lpMaximumComponentLength, LPDWORD lpFileSystemFlags, LPWSTR lpFileSystemNameBuffer, DWORD nFileSystemNameSize );


// Копировать/удалить файл на перезагрузке
// pszSrcName - source file full name
// pszDestName - destignation file fule name (might be NUL - means file should be deleted on reboot)
IOS_ERROR_CODE IOSMoveFileOnReboot( LPCTSTR pszSrcName, LPCTSTR pszDestName );
IOS_ERROR_CODE IOSMoveFileOnRebootW( LPCWSTR pszSrcName, LPCWSTR pszDestName );
IOS_ERROR_CODE IOSMoveFileOnReboot9x( LPCTSTR pszSrcName, LPCTSTR pszDestName );


// Проверить - не является ли устройство subst-drive
IOS_ERROR_CODE IOSIsSubstDrive( TCHAR caDrive );
IOS_ERROR_CODE IOSIsSubstDriveW( WCHAR cDrive );

// Получить имя DOS Device
// lpDeviceName - MS-DOS device name string
// lpTargetPath - query results buffer
// ucchMax      - maximum size of buffer
DWORD IOSQueryDosDevice( LPCTSTR lpDeviceName, LPTSTR lpTargetPath, DWORD ucchMax );
DWORD IOSQueryDosDeviceW( LPCWSTR lpDeviceName, LPWSTR lpTargetPath, DWORD ucchMax );

// Получить список устройств в системе
// nBufferLength  - size of buffer
// lpBuffer       - drive strings buffer
DWORD IOSGetLogicalDriveStrings( DWORD nBufferLength,  LPTSTR lpBuffer );
DWORD IOSGetLogicalDriveStringsW( DWORD nBufferLength,  LPWSTR lpBuffer );


#endif 
