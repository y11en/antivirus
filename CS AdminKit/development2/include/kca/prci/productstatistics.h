/*!
 * (C) 2002 "Kaspersky Lab"
 *
 * \file PRCI/ProductStatistics.h
 * \author јндрей  азачков
 * \date 2002
 * \brief »нтерфейс дл€ предоставлени€ компонентом продукта статистики своей работы.
 *
 */

#ifndef KLPRCI_PRODUCTSTATISTICS_H
#define KLPRCI_PRODUCTSTATISTICS_H

#include <string>

#include "prci_const.h"
#include "std/err/error.h"
#include "std/par/params.h"

namespace KLPRCI {


    /*!
    * \brief »нтерфейс дл€ предоставлени€ компонентом продукта статистики своей работы.
    */


    class KLSTD_NOVTABLE ProductStatistics: public KLSTD::KLBaseQI{
    public:

    /*!
		\brief ”станавливает текущее значение статистики работы компонента.

        ѕосле вызова этого метода ответственность за управление переданным
        контейнером Params лежит на модуле ProductStatistics.   лиент не должен
        каким-либо образом модифицировать напр€мую содержимое контейнера 
        Params после вызова этого метода.

        \param statistics       [in]  —татистика работы компонента.
	*/

       virtual void SetStatisticsData( KLPAR::Params * statistics )  = 0;


    /*!
		\brief ќбновл€ет значени€ параметров статистики работы компонента.

        ≈сли в контейнере со статистикой работы компонента уже существуют
        такие параметры, то дл€ них измен€ютс€ значени€.  ≈сли же параметров с
        такими именами не существует, они добавл€ютс€ к контейнеру со статистикой
        работы компонента.

        \param statistics       [in]  Ќабор параметров с новыми значени€ми
                                      статистики работы компонента.
	*/

       virtual void ReplaceStatisticsData( KLPAR::Params * statistics ) = 0;


    /*!
      \brief ”дал€ет параметры статистики.

      ƒанный метод удал€ет параметры статистики с именами, переданными в 
      параметре statistics.  ≈сли параметров с такими именами не существует, 
      возвращаетс€ исключение NOT_EXIST.
	*/

	   virtual void DeleteStatisticsData(KLPAR::Params * statistics ) =0;

    };



}


#endif // KLPRCI_PRODUCTSTATISTICS_H
