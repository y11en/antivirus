/*!
*		
*		
*		\file	antispam_interface.h
*		\brief	Интерфейс модуля Антиспам
*		
*		\author Vitaly DVi Denisov
*		
		При реализации антиспам-фильтра необходимо:
		1.	Определить типы данных инициализации, процессирования, обучения, 
			а также выдаваемую пользователю статистику
		2.	Для работы с памятью использовать методы класса CMemoryAlloc
		3.	Для получения указателя на часть письма (в методах Process и Train) использовать 
			CObjectSystemToCheck::PtrCreate
		4.	Для получения очередной части письма в виде объекта использовать метод
			CObjectSystemToCheck::ObjectCreate
		5.	Для получения данных, ассоциированных с объектом, создать объект CObjectIOToCheck на основе 
			CObjectToCheck и использовать методы CObjectToIOCheck::SeekRead и CObjectIOToCheck::GetSize
		6.	Объект типа CObjectSystemToCheck также можно создать на основе CObjectToCheck
		7.	Для освобождения ресурсов, занятых объектом-частью письма, использовать
			CObjectSystemToCheck::ObjectClose
		8.	Для освобождения указателя использовать
			CObjectSystemToCheck::PtrClose
		9.	Для получения свойств объекта (таких, как имя отправителя, имя получателя и т.д.)
			использовать метод CObjectToCheck::PropertyGet
			Доступные свойства представлены ниже константами с префиксами pgMESSAGE_.
			Все свойства являются строковыми (char*).
			Константы с префиксами pgMESSAGE_ доступны для письма.
			Константы с префиксами pgMESSAGE_PART_ доступны для части письма.
		10.	Для оценки кода ошибки пользоваться макросами PR_SUCC и PR_FAIL. 
		11.	Для соблюдения таймаута, а также корректного завершения обработки объекта
			по желанию пользователя, использовать методы CObjectToCheck::GetTickCount для
			засечки начала обработки объекта и CObjectToCheck::CheckTimeOut для опроса
			необходимости завершения обработки.
			Метод CObjectToCheck::CheckTimeOut следует вызывать до и после наиболее 
			затратных действий. Возвращенный код PR_FAIL сигнализирует
			о необходимости немедленной остановки обработки.
*		
*		
*		
*/		

#ifndef _ANTISPAM_INTERFACE2_H_
#define _ANTISPAM_INTERFACE2_H_

#ifndef IN
#define IN   // Вспомогательный дефайн: входной параметр функции
#endif
#ifndef OUT
#define OUT  // Вспомогательный дефайн: выходной параметр функции
#endif

#ifndef PRAGUE_DEFINED
	//////////////////////////////////////////////////////////////////////////
	//
	//	Используемые типы данных (взято из Prague)
	//
	typedef long tERROR;
	#define PR_SUCC(e) (((tERROR)(e)) >= 0)
	#define PR_FAIL(e) (((tERROR)(e)) < 0)
	typedef unsigned int tDWORD;
	typedef double tQWORD;
	typedef unsigned int tPROPID;
	typedef void* tPTR;
	typedef unsigned int tBOOL;
	typedef unsigned int tUINT;
	//
	//	Используемые типы данных
	//
	//////////////////////////////////////////////////////////////////////////

	#define errOK 0 
	#define errNOT_IMPLEMENTED  -1 

	/* global Mail Message properties */

	#define pgMESSAGE_FROM						0x00001000 
	#define pgMESSAGE_ORIGINAL_FROM				0x00001001 
	#define pgMESSAGE_DISPLAY_FROM				0x00001002 
	#define pgMESSAGE_ORIGINAL_DISPLAY_FROM		0x00001003 
	#define pgMESSAGE_TO						0x00001004 
	#define pgMESSAGE_DISPLAY_TO				0x00001005 
	#define pgMESSAGE_CC						0x00001006 
	#define pgMESSAGE_DISPLAY_CC				0x00001007 
	#define pgMESSAGE_SUBJECT					0x00001008 
	#define pgMESSAGE_DATE						0x00001009 
	#define pgMESSAGE_MAILID					0x0000100a 
	#define pgMESSAGE_INTERNET_HEADERS			0x0000100b 
	#define pgMESSAGE_MIME_VERSION				0x0000100c 
	#define pgMESSAGE_RETURN_PATH				0x0000100d 


	/* global Mail Message Part properties */

	#define pgMESSAGE_PART_CONTENT_TYPE			0x00001020 
	#define pgMESSAGE_PART_TRANSFER_ENCODING	0x00001021 
	#define pgMESSAGE_PART_FILE_NAME			0x00001022 
	#define pgMESSAGE_PART_DISPLAY_NAME			0x00001023 
	#define pgMESSAGE_PART_CONTENT_ID			0x00001024 
	#define pgMESSAGE_PART_CONTENT_DESCRIPTION	0x00001025 
	#define pgMESSAGE_PART_CHARSET				0x00001026 
	#define pgMESSAGE_PART_CONTENT_LENGTH		0x00001027 

#endif//PRAGUE_DEFINED

//////////////////////////////////////////////////////////////////////////
//
//	Реализация этих типов - на плечах пользователя антиспам-фильтра
//
namespace CAntispamCaller
{
	// Класс, предоставляющий методы работы с памятью
	struct CMemoryAlloc 
	{
		///////////////////////////////////////////////////////////////////////////
		//! \fn				: Allocate
		//! \brief			:	Метод выделения памяти.
		//!						В случае ошибки возвращает 0
		//! \return			: tPTR				- Указатель на буфер памяти
		//! \param          : tQWORD qwSize		- Требуемый размер буфера
		virtual tPTR Allocate(tQWORD qwSize);

		///////////////////////////////////////////////////////////////////////////
		//! \fn				: ReAllocate
		//! \brief			:	Метод перераспределения памяти
		//!						В случае ошибки возвращает 0
		//! \return			: tPTR				- Указатель на буфер памяти
		//! \param          : tPTR pObject		- Исходный буфер
		//! \param          : tQWORD qwNewSize	- Новый размер
		virtual tPTR ReAllocate(tPTR pObject, tQWORD qwNewSize);

		///////////////////////////////////////////////////////////////////////////
		//! \fn				: Free
		//! \brief			:	Метод освобождения памяти
		//! \return			: void 
		//! \param          : tPTR pObject
		virtual void Free(tPTR pObject);
	};

	// Класс, предоставляющий методы работы с объектом
	struct CObjectToCheck
	{
		///////////////////////////////////////////////////////////////////////////
		//! \fn				: PropertyGet
		//! \brief			:	Метод получения любого доступного свойства из объекта
		//!						При корректном исполнении метода возвращается неошибочный
		//!						код ошибки и ненулевой размер прочитанного буфера
		//! \return			: tERROR				- Код ошибки
		//! \param          : OUT tDWORD* out_size	- Размер скопированных данных
		//! \param          : IN tPROPID prop_id	- Идентификатор свойства
		//! \param          : IN tPTR buffer		- Указатель на буфер для копирования
		//! \param          : IN tDWORD size		- Доступный размер буфера
		virtual tERROR PropertyGet( 
			OUT tDWORD* out_size, 
			IN tPROPID prop_id, 
			IN tPTR buffer, 
			IN tDWORD size 
			)
		{return errNOT_IMPLEMENTED;};

		///////////////////////////////////////////////////////////////////////////
		//! \fn				: GetTickCount
		//! \brief			:	Возвращает количество мс, прошедшее с момента старта 
		//!						операционной системы. Используется для засечки момента начала 
		//!						обработки объекта и передается в функцию CheckTimeout
		//! \return			: tDWORD 
		virtual tDWORD GetTickCount()
		{
			return GetTickCount();
		};

		///////////////////////////////////////////////////////////////////////////
		//! \fn				: CheckTimeout
		//! \brief			:	Проверка окончания таймаута.
		//! \return			: tERROR 
		//! \param          : tDWORD dwTimeOut		- оцениваемый таймаут
		//! \param          : tDWORD dwProcessBegin	- значение GetTickCount, сохраненное 
		//!											в момент начала обработки
		virtual tERROR CheckTimeout(tDWORD dwTimeOut, tDWORD dwProcessBegin)
		{
			return errOK;
		};
	};

	// Класс, предоставляющий методы работы с объектной системой
	struct CObjectSystemToCheck : public CObjectToCheck
	{

		///////////////////////////////////////////////////////////////////////////
		//! \fn				: CObjectSystemToCheck
		//! \brief			:	Конструктор, создающий объект CObjectSystemToCheck из CObjectToCheck
		//! \param          : CObjectToCheck* pObject
		CObjectSystemToCheck(CObjectToCheck* pObject){};
		CObjectSystemToCheck(){};

		///////////////////////////////////////////////////////////////////////////
		//! \fn				: PtrCreate
		//! \brief			:	Получение указателя на объект в системе
		//! \return			: tERROR 
		//! \param          : OUT tUINT* pObjPtr
		virtual tERROR PtrCreate(OUT tUINT* pObjPtr)
		{return errNOT_IMPLEMENTED;};

		///////////////////////////////////////////////////////////////////////////
		//! \fn				: GetNextObject
		//! \brief			:	Получение следующего объекта по данному указателю
		//!						Объект может быть освобожден только методом ObjectClose
		//! \return			: tERROR 
		//! \param          : IN tUINT pObjPtr
		//! \param          : OUT CObjectToCheck** ppObject
		virtual tERROR ObjectCreate(IN tUINT pObjPtr, OUT CObjectToCheck** ppObject)
		{return errNOT_IMPLEMENTED;};

		///////////////////////////////////////////////////////////////////////////
		//! \fn				: ObjectClose
		//! \brief			:	Освобождение объекта по указателю pObjPtr
		//! \return			: tERROR 
		//! \param          : IN tUINT pObjPtr
		//! \param          : IN CObjectToCheck* pObject
		virtual tERROR ObjectClose(IN tUINT pObjPtr, IN CObjectToCheck* pObject)
		{return errNOT_IMPLEMENTED;};

		///////////////////////////////////////////////////////////////////////////
		//! \fn				: PtrClose
		//! \brief			:	Освобождение указателя на объект в системе
		//! \return			: tERROR 
		//! \param          : IN tUINT pObjPtr
		virtual tERROR PtrClose(IN tUINT pObjPtr)
		{return errNOT_IMPLEMENTED;};
	};

	struct CObjectIOToCheck : public CObjectToCheck
	{
		
		///////////////////////////////////////////////////////////////////////////
		//! \fn				: CObjectSystemToCheck
		//! \brief			:	Конструктор, создающий объект CObjectIOToCheck из CObjectToCheck
		//! \param          : CObjectToCheck* pObject
		CObjectIOToCheck(CObjectToCheck* pObject){};
		CObjectIOToCheck(){};

		///////////////////////////////////////////////////////////////////////////
		//! \fn				: SeekRead
		//! \brief			:	Метод получения данных из объекта
		//! \return			: tERROR				- Код ошибки
		//! \param          : OUT tDWORD* out_size	- Размер скопированных данных
		//! \param          : IN tQWORD offset		- Текущая позиция (в байтах)
		//! \param          : IN tPTR buffer		- Указатель на буфер для копирования
		//! \param          : IN tDWORD size		- Доступный размер буфера
		virtual tERROR SeekRead( 
			OUT tDWORD* out_size, 
			IN tQWORD offset, 
			IN tPTR buffer, 
			IN tDWORD size 
			)
		{return errNOT_IMPLEMENTED;};

		///////////////////////////////////////////////////////////////////////////
		//! \fn				: GetSize
		//! \brief			:	Получение размера объекта
		//! \return			: tERROR				- Код ошибки
		//! \param          : OUT tQWORD* out_size	- Размер объекта
		virtual tERROR GetSize( OUT tQWORD* out_size )
		{return errNOT_IMPLEMENTED;};
	};
}
//
//	Реализация этих типов - на плечах пользователя антиспам-фильтра
//
//////////////////////////////////////////////////////////////////////////



//////////////////////////////////////////////////////////////////////////
//
//	Реализация этих типов - на плечах разработчиков антиспам-фильтра
//
namespace CAntispamImplementer
{
	
	class CAntispamFilter
	{
	public:
		CAntispamFilter(){};
		virtual ~CAntispamFilter(){};
		
		//TODO: Здесь необходимо определить, какие данные нужны антиспам-фильтру для инициализации
		struct InitParams_t;
		//TODO: Здесь необходимо определить, какие данные нужны антиспам-фильтру для процессирования
		struct ProcessParams_t;
		//TODO: Здесь необходимо определить, какие данные антиспам-фильтр может вернуть в качестве статистики
		struct Statistics_t;
		//TODO: Здесь необходимо определить, какие данные антиспам-фильтр может вернуть в качестве статистики при обработке объекта
		struct ProcessStatistics_t;
		//TODO: Здесь необходимо определить, какие данные нужны антиспам-фильтру для обучения
		struct TrainParams_t;
		
		///////////////////////////////////////////////////////////////////////////
		//! \fn				: Init
		//! \brief			:	Инициализирует антиспам-фильтр данными, указанными в pInitData.
		//!						Устанавливает методы выделения и освобождения памяти.
		//! \return			: tERROR 
		//! \param          : CAntispamCaller::CMemoryAlloc* pMemoryAllocator
		//! \param          : InitParams_t* pInitParams
		virtual tERROR Init(
			CAntispamCaller::CMemoryAlloc* pMemoryAllocator, 
			InitParams_t* pInitParams
			)
			{return errNOT_IMPLEMENTED;};

		///////////////////////////////////////////////////////////////////////////
		//! \fn				: GetStatistics
		//! \brief			:	Выдача накопленной статистики 
		//! \return			: virtual tERROR 
		//! \param          : IN Statistics_t* p_Statistics
		virtual tERROR GetStatistics( IN Statistics_t* p_Statistics )
			{return errNOT_IMPLEMENTED;};

		///////////////////////////////////////////////////////////////////////////
		//! \fn				: Process
		//! \brief			:	Обработка объекта pObject и выдача результата pdwResult
		//! \return			: tERROR								- Код ошибки
		//! \param          : IN  CObjectSystemToCheck* pObject		- Письмо для процессирования
		//! \param          : IN  ProcessParams_t* pProcessParams	- Параметры процессирования
		//! \param          : OUT long double* pdwResult			- Результат процессирования {0..1}
		//! \param          : IN  tDWORD dwTimeOut					- Максимальное время для процессирования (мс)
		//! \param          : OUT ProcessStatistics_t* pProcessStat - Статистика процессирования 
		virtual tERROR Process( 
			IN  CAntispamCaller::CObjectSystemToCheck* pObject, 
			IN  ProcessParams_t* pProcessParams, 
			OUT long double* pResult,
			IN  tDWORD dwTimeOut,
			OUT ProcessStatistics_t* pProcessStat = 0
			)
			{return errNOT_IMPLEMENTED;};

		///////////////////////////////////////////////////////////////////////////
		//! \fn				: Train
		//! \brief			:	Обучение фильтра на объекте pObject. 
		//!						Обязательно указывается принадлежность объекта к спаму 
		//!						флагом bIsSpamObject
		//! \return			: tERROR						- Код ошибки
		//! \param          : IN CObjectSystemToCheck* pObject	- Письмо для обучения
		//! \param          : IN TrainParams_t* pTrainParams - Параметры обучения
		//! \param          : IN  tDWORD dwTimeOut			- Максимальное время для обучения (мс)
		virtual tERROR Train(
			IN CAntispamCaller::CObjectSystemToCheck* pObject, 
			IN TrainParams_t* pTrainParams,
			IN  tDWORD dwTimeOut
			)
			{return errNOT_IMPLEMENTED;};
	};
}
//
//	Реализация этих типов - на плечах разработчиков антиспам-фильтра
//
//////////////////////////////////////////////////////////////////////////

#endif//_ANTISPAM_INTERFACE2_H_


