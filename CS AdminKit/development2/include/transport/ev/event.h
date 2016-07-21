/*!
 * (C) 2002 "Kaspersky Lab"
 *
 * \file Event.h
 * \author Дамир Шияфетдинов
 * \date 2002
 * \brief Интерфейс класса событие, который является основной информационной единицей 
 *			библиотеки Eventing
 * 
 */


#ifndef KLEV_EVENT_H
#define KLEV_EVENT_H

#include <string>

#include "std/base/klbase.h"
#include "std/par/params.h"

#include "kca/prci/componentid.h"

#include "transport/ev/common.h"

namespace KLEV {

	class Event : public KLSTD::KLBase
	{
		public:

			virtual ~Event() {}

			/*!
				\brief Функция инициализации события.
		
				\param publisher [in] Наименование публикатора 
				\param type [in] Тип события
				\param body [in] Параметры события
				\param birthTime [in] Время возниконовении события 
							( если равно 0 то в событие будет проставленно текущее время )
				\param lifeTime [in] Время жизни события в секундах
			*/
			virtual void Init( const KLPRCI::ComponentId &publisher, const std::wstring &type, 
				KLPAR::Params *body, const time_t birthTime = 0, 
				const int lifeTime = 0 ) = 0;

			/*!
				\brief Возвращает имя публикатора события
			*/
			virtual const KLPRCI::ComponentId &GetPublisher( ) const = 0;

			/*!
				\brief Устанавливает имя публикатора события

				\param newPublisher [out] Новое имя публикатора 
			*/
			virtual void SetPublisher( const KLPRCI::ComponentId &newPublisher ) = 0;

			/*!
				\brief Возвращает тип события
			*/
			virtual const std::wstring &GetType() const = 0;

			/*!
				\brief Устанавливает тип события

				\param eventType [in] Новый тип события
			*/
			virtual void SetType( const std::wstring &eventType ) = 0;

			/*!
				\brief Возвращает параметры события

					Функция возвращает простой указатель на внутренние Params класса.
					Возвращенные данной функцией Params изменеться не должны. При возвращении
					указателя AddRef не вызывается.
			*/
			virtual KLPAR::Params *GetParams() const = 0;

			/*!
				\brief Устанавливает параметры события

				\param eventParams [in] Новые параметры события
			*/
			virtual void SetParams( KLPAR::Params *eventParams ) = 0;

			/*!
				\brief Возвращает время возникновения события
			*/
			virtual time_t GetBirthTime() const = 0;

			/*!
				\brief Устанавливает время возникновения события

				\param eventBirthTime [in] Новое время возникновения события
			*/
			virtual void SetBirthTime( const time_t eventBirthTime ) = 0;

			/*!
				\brief Возвращает время жизни события в секундах				
			*/
			virtual int GetLifeTime() const = 0;

			/*!
				\brief Устанавливает время жизни события в секундах

				\param eventLifeTime [in] Новое время жизни события
			*/
			virtual void SetLifeTime( const int eventLifeTime ) = 0;
	};

} // namespace KLEV

/*!
   \brief Метод для создания события Event. 
        Созадает Event и инициализирует его переданными параметрами.

   \return Указатель на вновь созданный объект.
*/
KLCSTR_DECL void KLEV_CreateEvent( KLEV::Event **ppEvent, const KLPRCI::ComponentId &publisher, 
				const std::wstring &type, KLPAR::Params *body, 
				const time_t birthTime = 0, int lifeTime = 0 );

#endif // KLEV_EVENT_H

// Local Variables:
// mode: C++
// End:
