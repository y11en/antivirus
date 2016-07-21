/*KLCSAK_PUBLIC_HEADER*/
/*KLCSAK_PUBLIC_HEADER*/

#ifndef KLPRM_VALUE_H
#define KLPRM_VALUE_H

#include <std/base/klbase.h>
#include <std/err/error.h>
#include <avp/avp_data.h>
#include <std/time/kltime.h>

namespace KLPAR {

    class StringValue;
    class BoolValue;
    class IntValue;
    class LongValue;
    class DateTimeValue;
    class DateValue;
    class BinaryValue;
    class FloatValue;
    class DoubleValue;
    class ParamsValue;
    class ArrayValue;
    class Params;


   /*!
    * \brief Интерфейс для создания объектов класса Value.
    *
    */ 
    class KLSTD_NOVTABLE ValuesFactory: public KLSTD::KLBase{
	public:
        /*!
        \brief Создает в heap новый объект класса StringValue 
                и возвращает указатель на него.

        \return Указатель на вновь созданный объект.

        */
        virtual void CreateStringValue(StringValue** ppVal) = 0;
        virtual void CreateBoolValue(BoolValue** ppVal)  = 0;
        virtual void CreateIntValue(IntValue** ppVal)  = 0;
        virtual void CreateLongValue(LongValue** ppVal)  = 0;
        virtual void CreateDateTimeValue(DateTimeValue** ppVal)  = 0;
        virtual void CreateDateValue(DateValue** ppVal)  = 0;
        virtual void CreateBinaryValue(BinaryValue** ppVal)  = 0;
        virtual void CreateFloatValue(FloatValue** ppVal)  = 0;
        virtual void CreateDoubleValue(DoubleValue** ppVal) = 0;
        virtual void CreateParamsValue(ParamsValue** ppVal)  = 0;
        virtual void CreateArrayValue(ArrayValue** ppVal)  = 0;
    };

    typedef KLSTD::CAutoPtr<KLPAR::ValuesFactory> ValuesFactoryPtr;


    /*!
    * \brief Базовый интерфейс для значений, сохраняемых в контейнере Params.
    *  
    */ 

    class KLSTD_NOVTABLE Value: public KLSTD::KLBase{
	public:
        //! Список типов значений, сохраняемых в Params. Примитивные типы
        //  совпадают с типами, определенными в XML Schema.
        enum Types {
            EMPTY_T,  /*!< пустое значение (значение не определено */
            STRING_T, /*!< wchar_t строка */  
            BOOL_T,   /*!< boolean значение */
            INT_T,    /*!< 32-bit integer */
            LONG_T,   /*!< 64-bit integer */
            DATE_TIME_T,  /*!< time_t */
            DATE_T,   /*!< строка в формате ССYY-MM-DD */
            BINARY_T, /*!< бинарная последовательность */
            FLOAT_T,  /*!< 32-bit IEEE float */
            DOUBLE_T, /*!< 64-bit IEEE float */
            PARAMS_T, /*!< Вложенный контейнер Params */
            ARRAY_T   /*!< Вложенный контейнер Array */
        };

 
   /*!
      \brief Возвращает тип значения.

      \return Тип значения.

    */

      virtual Types	GetType() const = 0;
	  virtual void	Clone(Value** ppValue) const =0;

   /*!
      \brief Сравнивает данное Value с pValue

      \return Тип значения:
			< 0 данное Value меньше pValue
			= 0 данное Value равно pValue
			> 0 данное Value больше pValue

    */
	  virtual int Compare(const Value* pValue) const =0;
	protected:
    };

    typedef KLSTD::CAutoPtr<Value>  ValuePtr;

   /*!
    * \brief Значения типа STRING_T, сохраняемые в контейнере Params.
    *  
    */ 
    class KLSTD_NOVTABLE StringValue : public Value {
    public:

        /*!
            \brief Устанавливает значение. Строка копируется во внутренний буфер.
            В деструкторе класса буфер освобождается.

            \param value [in] Строка со значением для StringValue 

        */
        virtual void SetValue( const wchar_t * value ) = 0;

        /*!
            \brief Устанавливает значение по ссылке. 
            
            Строка не копируется во внутренний буфер, вместо этого используется
            переданный указатель.  Время жизни переданного указателя должно быть
            больше, чем время жизни StringValue.  В деструкторе класса никаких
            действий с переданным указателем не происходит.

            \param value [in] Строка со значением для StringValue 

        */
        virtual void SetValueRef( const wchar_t * value ) = 0;

        /*!
            \brief Возвращает указатель на строку, переданную в SetValue или
                    SetValueRef.

            \return Значение, сохраняемое в StringValue

        */
        virtual const wchar_t * GetValue() const = 0;
    };
    
    typedef KLSTD::CAutoPtr<KLPAR::StringValue> StringValuePtr;


   /*!
    * \brief Значения типа BOOL_T, сохраняемые в контейнере Params.
    *  
    */ 
    class KLSTD_NOVTABLE BoolValue : public Value {
    public:
        virtual void SetValue( bool value ) = 0;
        virtual bool GetValue() const = 0;
    };

    typedef KLSTD::CAutoPtr<KLPAR::BoolValue> BoolValuePtr;

   /*!
    * \brief Значения типа INT_T, сохраняемые в контейнере Params.
    *  
    */ 
    class KLSTD_NOVTABLE IntValue : public Value {
    public:
        virtual void SetValue( long value ) = 0;
        virtual long GetValue() const = 0;
    };

    typedef KLSTD::CAutoPtr<KLPAR::IntValue> IntValuePtr;


   /*!
    * \brief Значения типа LONG_T, сохраняемые в контейнере Params.
    *  
    */ 
    class KLSTD_NOVTABLE LongValue : public Value {
    public:
        virtual void SetValue( AVP_longlong value ) = 0;
        virtual AVP_longlong GetValue() const = 0;
    };

    typedef KLSTD::CAutoPtr<KLPAR::LongValue> LongValuePtr;

   /*!
    * \brief Значения типа DATETIME_T, сохраняемые в контейнере Params.
    *  
    */ 
    class KLSTD_NOVTABLE DateTimeValue : public Value {
    public:
        virtual void SetValue( KLSTD::klstd_old_time_t value ) = 0;
        virtual KLSTD::klstd_old_time_t GetValue() const = 0;
    };

    typedef KLSTD::CAutoPtr<KLPAR::DateTimeValue> DateTimeValuePtr;


   /*!
    * \brief Значения типа DATE_T, сохраняемые в контейнере Params.
    *  
    */ 
    class KLSTD_NOVTABLE DateValue : public Value {
    public:
        virtual void SetValue( const char * value ) = 0;
        virtual const char * GetValue() const = 0;
    };

    typedef KLSTD::CAutoPtr<KLPAR::DateValue> DateValuePtr;

    /*!
		\brief Callback - функция для освобождения памяти, переданной в BinaryValue

		\param buff [in]  Адрес буфера для освобождения.
		\param context [in] Контекст, связанный с этим буфером.
	*/
    typedef void (*FreeBufferCallback)(void * buff, void * context );


   /*!
    * \brief Значения типа BINARY_T, сохраняемые в контейнере Params.
    *  
    */ 
    class KLSTD_NOVTABLE BinaryValue : public Value {
    public:

        /*!
            \brief Устанавливает значение для BinaryValue.
            
            Передаваемый указатель должен быть корректным все время жизни BinaryValue.
            После вызова этого метода ответственность за управление переданным
            буфером лежит на BinaryValue.  Буфер освобождается в деструкторе
            BinaryValue путем вызова метода free либо в момент 
            вызова последующего метода SetValue.

            \param value [in]  Буфер для использования в BinaryValue.
            \param bytes [in]  Размер буфера в байтах.

        */
        virtual void SetValue( void * value, size_t bytes ) = 0;

        /*!
            \brief Устанавливает значение для BinaryValue.
            
            Передаваемый указатель должен быть корректным все время жизни BinaryValue.
            После вызова этого метода ответственность за управление переданным
            буфером лежит на BinaryValue.  Буфер освобождается в деструкторе
            BinaryValue путем вызова переданной callback - функции с контекстом,
            переданным для данного буфера либо в момент вызова последующего 
            метода SetValue.

            \param value [in]  Буфер для использования в BinaryValue.
            \param bytes [in]  Размер буфера в байтах.
            \param callback [in]  Callback - функция для освобождения заведенной памяти.
            \param context [in] Контекст для вызова Callback - функции.

        */
        virtual void SetValue( void * value, 
                               size_t bytes,
                               FreeBufferCallback callback,
                               void * context ) = 0;

        /*!
            \brief Возвращает указатель на буфер BinaryValue.  В случае, если
            буфер не установлен, возвращает 0.
        */

        virtual void * GetValue() const = 0;

        /*!
            \brief Возвращает размер буфера BinaryValue в байтах.  В случае, если
            буфер не установлен, возвращает 0.
        */

        virtual size_t GetSize() const = 0;
    };

    typedef KLSTD::CAutoPtr<KLPAR::BinaryValue> BinaryValuePtr;

   /*!
    * \brief Значения типа FLOAT_T, сохраняемые в контейнере Params.
    *  
    */ 
    class KLSTD_NOVTABLE FloatValue : public Value {
    public:
        virtual void SetValue( float value ) = 0;
        virtual float GetValue() const = 0;
    };

    typedef KLSTD::CAutoPtr<KLPAR::FloatValue> FloatValuePtr;


   /*!
    * \brief Значения типа DOUBLE_T, сохраняемые в контейнере Params.
    *  
    */ 
    class KLSTD_NOVTABLE DoubleValue : public Value {
    public:
        virtual void SetValue( double value ) = 0;
        virtual double GetValue() const = 0;
    };

    typedef KLSTD::CAutoPtr<KLPAR::DoubleValue> DoubleValuePtr;


   /*!
    * \brief Значения типа PARAMS_T, сохраняемые в контейнере Params.
    *  
    */ 
    class KLSTD_NOVTABLE ParamsValue : public Value {
    public:

        /*!
            \brief Устанавливает значение для ParamsValue.
            
            Передаваемый указатель должен быть корректным все время жизни ParamsValue.
            После вызова этого метода ответственность за управление переданным
            объектом Params лежит на ParamsValue.  Переданный объект Params
            освобождается в деструкторе ParamsValue путем вызова оператора delete
            либо в момент вызова последующего метода SetValue.

            \param value [in]  Буфер для использования в BinaryValue.
            \param bytes [in]  Размер буфера в байтах.

        */
        virtual void SetValue( Params * value ) = 0;


        /*!
            \brief Возвращает указатель на сохраняемый объект Params.  В случае, если
            объект Params не установлен, возвращает 0.
        */

        virtual Params * GetValue() const = 0;
    };

    typedef KLSTD::CAutoPtr<KLPAR::ParamsValue> ParamsValuePtr;


   /*!
    * \brief Значения типа ARRAY_T, сохраняемые в контейнере Params.
    *  
    */ 
    class KLSTD_NOVTABLE ArrayValue : public Value {
    public:

        /*!
            \brief Позволяет установить размер массива
        */
		virtual void SetSize(size_t size)=0;

        /*!
            \brief Позволяет получить размер массива
        */
		virtual size_t GetSize() const =0;

        /*!
            \brief Позволяет записать nIndex-й элемент массива.
        */
		virtual void SetAt(size_t nIndex, Value* value)=0;

        /*!
            \brief Позволяет олучить nIndex-й элемент массива.
        */
        virtual void GetAt(size_t nIndex, Value** value) = 0;
		virtual const Value* GetAt(size_t nIndex) const = 0;
    };

    typedef KLSTD::CAutoPtr<KLPAR::ArrayValue> ArrayValuePtr;
}

/*!
   \brief Создание фабрики объектов Value. 
        Создает в heap новый объект класса ValuesFactory и возвращает указатель на него.

   \return Указатель на вновь созданный объект.

*/

KLCSC_DECL void KLPAR_CreateValuesFactory(KLPAR::ValuesFactory**);


#endif // KLPRM_VALUE_H
