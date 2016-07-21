Параметры запуска командной строки UnitTest.exe:
-ini		путь к файлу конфигурации
-log		путь к файлу логов классов апдейтера

Параметры файла конфигурации UnitTest.exe

[TESTCLASSES]		Секция с перечнем классов для тестирования
FileTransaction		FileTransaction
LocalFile		LocalFile
NetCore			NetCore, NetTransfer, NetFTP, netSMB
UpdaterTransaction	UpdaterTransaction

[COMMON]		Секция общих параметров
BaseFolder		Базовый каталог на локальной машине для работы тестов (все остальные пути относительные!)
DiffTool		Путь к каталогу утилиты бинарного сравнения каталогов


[FILETRANSACTION]	Секция параметров для тестирования FileTransaction и UpdaterTRansaction
Source          	Путь к каталогу источников обновления
SourceEtalon		Путь к каталогу эталонов источников обновления
Destination		Путь к каталогу для конечных файлов
DestinationEtalon	Путь к каталогу для эталонов конечных файлов
Backup			Путь к каталогу бэкапа
BackupEtalon		Путь к каталогу эталона бэкапа
FilesStorage		Путь к каталогу файлов для транзакций версии 1                                          	
FilesStorage2		Путь к каталогу файлов для транзакций версии 2
FilesStorage3		Путь к каталогу файлов для транзакций версии 3
DiffTool		Путь к каталогу с утилитой сравнения файлов
                                                                                

[LOCALFILE]		Секция параметров для тестирования LocalFile
Source          	Путь к каталогу источников обновления
SourceEtalon		Путь к каталогу эталонов источников обновления
Destination		Путь к каталогу для конечных файлов
DestinationEtalon	Путь к каталогу для эталонов конечных файлов
FilesStorage		Путь к каталогу файлов для транзакций версии 1                                          	

[NETCORE]		Секция параметров для тестирования NetCore, NetTransfer, NetFTP, NetSMB
Destination		Путь к каталогу для конечных файлов
DestinationEtalon	Путь к каталогу для эталонов конечных файлов
FilesStorage		Путь к каталогу файлов для транзакций версии 1                                          	
