/*!
 * (C) 2002 "Kaspersky Lab"
 *
 * \file FilesList.h
 * \author Дамир Шияфетдинов
 * \date 2004
 * \brief Интерфейс списка рабочих файлов
 * 
 */

#ifndef KLFT_FILESLIST_H
#define KLFT_FILESLIST_H

#include <std/base/klbase.h>
#include <kca/prci/componentid.h>

namespace KLFT {

	/*!
    * \brief Список рабочих файлов подсистемы FT.
    *  
    */

	class KLSTD_NOVTABLE FilesList : public KLSTD::KLBase 
	{			
	public:

		/*!
			\brief Итератор по файлам
		*/
		virtual void Reset() = 0;

		/*!
			\brief Итератор по файлам
		*/
		virtual bool Next() = 0;


		/*!
			\brief Возвращет информацию по текущему файлу
		*/
		virtual const FileDescription &Next() = 0;

	};

} // end namespace KLFT

#endif // !defined(KLFT_FILESLIST_H)
