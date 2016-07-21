/*!
 * (C) 2002 "Kaspersky Lab"
 *
 * \file SubscriptionIterator.h
 * \author Дамир Шияфетдинов
 * \date 2002
 * \brief Интерфейс класса итератора подписок
 * 
 */

#ifndef KLEV_SUBSCIRPTION_ITERATOR_H
#define KLEV_SUBSCIRPTION_ITERATOR_H

namespace KLEV {

	class Subscription;

	class SubscriptionIterator
	{
		public:

			virtual ~SubscriptionIterator() {}
			
			/*!
				\brief Устанавливает итератор на начало списка
			*/
			virtual void Reset() = 0;

			/*!
				\brief Возвращает текущую подписку и переводит итератор на следующую 
					в списке подписку
			*/
			virtual Subscription *Next() = 0;

	};

} // namespace KLEV


#endif // KLEV_SUBSCIRPTION_ITERATOR_H
