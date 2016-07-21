/*!
 * (C) 2002 "Kaspersky Lab"
 *
 * \file Params.h
 * \author Андрей Казачков
 * \date 2002
 * \brief Главный интерфейс модуля Params.
 *
 */
/*KLCSAK_PUBLIC_HEADER*/

#ifndef KLPRM_PARAMS_H
#define KLPRM_PARAMS_H

/*KLCSAK_BEGIN_PRIVATE*/
#include <string>
#include <vector>
/*KLCSAK_END_PRIVATE*/

#include <std/err/error.h>
#include <std/base/klbase.h>
#include "./errors.h"
#include "./value.h"

/*!
	\brief Проверка типа переменной. 
		\param _ptr ненулевой указатель на Value или на наследника Value
		\param _type элемент перечисления KLPAR::Value::Types (например, INT_T)
		\param _name указатель const wchar_t* на имя переменной
*/

#define KLPAR_CHKTYPE(_ptr, _type, _name)											\
	if((_ptr)->GetType()!=KLPAR::Value::_type)										\
		KLERR_THROW1(L"KLPAR", KLPAR::WRONG_VALUE_TYPE, (const wchar_t*)(_name));

#define KLPAR_CHKTYPE2(_ptr, _type, _name)											\
	if((_ptr)->GetType()!=_type)										\
		KLERR_THROW1(L"KLPAR", KLPAR::WRONG_VALUE_TYPE, (const wchar_t*)(_name));


namespace KLPAR {

    /*!
    * \brief Интерфейс контейнера Params (НЕ thread-safe !!!!!).
    *  
    *  Контейнер Params используется для единообразной работы с данными всех модулей
    *  CS Admin Kit.  Контейнер представляет собой набор пар имя - значение.  
    *  Имя представляет собой wchar_t строку, значение может быть либо простым, либо
    *  сложным типом.  Список поддерживаемых типов определен в перечислении Value::Types.
    *  Сложный тип может быть либо в свою очередь контейнером Params, либо массивом
    *  значений.
    *
    */
    
/*KLCSAK_BEGIN_PRIVATE*/
    //!obsolete
    typedef std::vector< std::wstring > ParamsNames;
    //!obsolete
    KLCSC_DECL void _GetParamsNames(const KLPAR::Params* p, ParamsNames& names);
/*KLCSAK_END_PRIVATE*/
    
    typedef KLSTD::AKWSTRARR names_t;

    class Params: public KLSTD::KLBaseQI
    {
    public:        
		/*!
		  \brief	Метод рекурсивно копирует всё дерево контейнеров и значений.
					Т.е. для каждого значения Value* в иерархия создаётся копия.
					Полученный контейнер и всё его содержимое становится
					полностью независимым от исходного контейнера.
		  \param	ppParams [out] указатель на переменную, в которую будет
					записан указатель на созданный контейнер Params
		*/
		virtual void Clone(Params** ppParams) const =0;

		/*!
		  \brief	Метод рекурсивно копирует всё дерево контейнеров и значений.
					В отличие от метода Clone, метод Duplicate осуществляет
					копирование только для переменных типа ParamsValue, для
					переменных других типов производится лишь копирование
					ссылок.
		  \param	ppParams [out] указатель на переменную, в которую будет
					записан указатель на созданный контейнер Params
		*/
		virtual void Duplicate(Params** ppParams) = 0;
   /*!
      \brief Сравнивает данное Params с pParams

      \return Тип значения:
			< 0 данное Params меньше pParams
			= 0 данное Params равно pParams
			> 0 данное Params больше pParams

    */
	  virtual int Compare(const Params* pParams) const =0;

    /*!
      \brief Добавляет новое значение к контейнеру.

      \param name [in] Имя добавляемого параметра.
      \param value [in] Значение добавленного параметра.
	  \param bReplace [in] Если параметр с именем name уже существует, то произойдёт ошибка ALREADY_EXIST.
    */

        virtual void AddValue(const wchar_t* name, Value * value) = 0;        

    /*!
      \brief Добавляет новое значение к контейнеру, если его не существует,
			замещает значение парамтра, если он уже существует.

      \param name [in] Имя добавляемого параметра.
      \param value [in] Значение добавленного параметра.
    */
        virtual void ReplaceValue(const wchar_t* name, Value * value) = 0;


    /*!
      \brief Удаляет значение из контейнера.

      \param name [in] Имя удаляемого параметра.
	  \param bThrow [in] Указывает, нужно ли выбрасывать исключение NOT_EXIST при отсутствии параметра
			или возвращать false.

    */

        virtual bool DeleteValue(const wchar_t* name, int bThrow = false) = 0;


    /*!
      \brief Устанавливает новое значение для параметра, существующего в контейнере.

      \param name [in] Имя изменяемого параметра.
      \param value [in] Новое значение параметра.

	    Если параметр с именем name не найден, то выбрасывается исключение NOT_EXIST.

    */
        virtual void SetValue(const wchar_t* name, Value * value) = 0;

    /*!
      \brief Возвращает указатель на сохраняемое в контейнере значение.

      \param name [in] Имя параметра для получения значения.
	  \param ppValue [out] указатель на объект Value, сохраняемый в контейнере
    */
		virtual void GetValue(const wchar_t* name, Value** ppValue) = 0;

    /*!
      \brief Возвращает указатель на сохраняемое в контейнере значение.


      \param name [in] Имя параметра для получения значения.
	  \param ppValue [out] указатель на объект Value, сохраняемый в контейнере
	  Если параметр с именем name не найден, то возвращается false, иначе true.

    */
		virtual bool GetValueNoThrow(const wchar_t* name, Value** ppValue) = 0;

    /*!
      \brief Возвращает имена всех сохраняемых в контейнере значений.
	  \param names [out] Массив всех сохраняемых в контейнере имен параметров.

    */
        virtual void GetNames(names_t &names) const  = 0;        

    /*!
      \brief Полностью очищает все содержимое контейнера Params.

 
    */
        virtual void Clear() = 0;

	/*!
	  \brief	Проверяет наличие параметра

	  \param	name [in] Имя параметра
	  \return	если параметр существует, возвращает true, иначе -- false
	*/
        virtual bool DoesExist(const wchar_t* name) const= 0;

    /*!
      \brief Копирует содержимое контейнера pSrc в данный контейнер.
		При этом существующие переменные данного контейнера замещаются 
		переменными контейнера pSrc.
     */
		virtual void CopyFrom(const Params* pSrc) = 0;

    /*!
      \brief Перемещает содержимое контейнера pSrc в данный контейнер.
		При этом существующие переменные данного контейнера замещаются 
		переменными контейнера pSrc.
     */
		virtual void MoveFrom(Params* pSrc) = 0;

    /*!
      \brief	Returns number of items 

      \return	number of items 
    */
        virtual size_t GetSize() const = 0;

    /*!
      \brief	Makes container and all its subcontainers read-only
    */
        virtual void MakeReadOnly() = 0;

/*KLCSAK_BEGIN_PRIVATE*/
    //! inlines
         void AddValue(const std::wstring& name, Value * value)
        {
            this->AddValue(name.c_str(), value);
        };

         void ReplaceValue(const std::wstring& name, Value * value)
        {
            this->ReplaceValue(name.c_str(), value);
        };

         bool DeleteValue(const std::wstring& name, int bThrow = false)
        {
            return this->DeleteValue(name.c_str(), bThrow);
        };

         void SetValue(const std::wstring& name, Value * value)
        {
            this->SetValue(name.c_str(), value);
        };

         void GetValue(const std::wstring& name, Value** ppValue)
        {
            this->GetValue(name.c_str(), ppValue);
        };

         bool GetValueNoThrow(const std::wstring& name, Value** ppValue)
        {
            return this->GetValueNoThrow(name.c_str(), ppValue);
        };

         bool DoesExist(const std::wstring& name) const
        {
            return this->DoesExist(name.c_str());
        };

         void GetNames(ParamsNames &names) const
         {
             _GetParamsNames(this, names);
         };

//! DEPRECATED !!!! DO NOT USE !!!
		const Value* GetValue2(const std::wstring& name, bool bThrow) const
        {
            KLSTD::CAutoPtr<Value> pValue;
            if(bThrow)
                const_cast<KLPAR::Params*>(this)->GetValue(name.c_str(), &pValue);
            else
                const_cast<KLPAR::Params*>(this)->GetValueNoThrow(name.c_str(), &pValue);
            return pValue;
        };

		const Value* GetValue(const std::wstring& name, int bThrow) const
        {
            return GetValue2(name, bThrow?true:false);
        };

		Value* GetValue2(const std::wstring& name, bool bThrow)
        {
            KLSTD::CAutoPtr<Value> pValue;
            if(bThrow)
                this->GetValue(name.c_str(), &pValue);
            else
                this->GetValueNoThrow(name.c_str(), &pValue);
            return pValue;
        };
/*KLCSAK_END_PRIVATE*/
    };
    
    typedef KLSTD::CAutoPtr<Params>  ParamsPtr;
}

#ifndef _HELPERS_H_11102C22_1458_4de9_8DD4_7A08981B188A
# include "../par/helpers.h"
#endif

/*!
   \brief Метод для создания контейнера Params. 
        Создает в heap новый объект класса Params и возвращает указатель на него.

   \return Указатель на вновь созданный объект.

*/
KLCSC_DECL void KLPAR_CreateParams(KLPAR::Params** ppParams) ;

#endif // KLPRM_PARAMS_H

// Local Variables:
// mode: C++
// End:
