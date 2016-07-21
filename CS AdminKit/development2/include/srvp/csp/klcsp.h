/*!
 * (C) 2003 Kaspersky Lab 
 * 
 * \file	klcsp.h
 * \author	Andrew Kazachkov
 * \date	16.06.2003 19:34:25
 * \brief	Common server proxy classes
 * 
 */

#ifndef __KL__CSP_H__
#define __KL__CSP_H__

#define KLCSP_ITERATOR_ARRAY    L"KLCSP_ITERATOR_ARRAY" //ARRAY_T

namespace KLPAR{
	class Params;
};

namespace KLCSP
{
    //! Структура, содержащая идентификатор удаленного компьютера.
	typedef struct {
		//! Имя домена, в который входит хост.
		std::wstring domain;
		//! Имя компьютера в домене.
		std::wstring host;
	}host_id_t;
    
	//! Описание сортировки по одному полю 
	typedef struct 
	{
		std::wstring wstrFieldName;	// поле, по которому осуществляеться сортировка
		bool bOrder;				// если true, то сортировка прямая, если false - то обратная.
	} field_order_t;
    
    typedef std::vector<field_order_t>   vec_field_order_t;

    class KLSTD_NOVTABLE ForwardIterator : public KLSTD::KLBase
	{
	public:

	/*!
	  \brief	GetNextItems

	  \param	lCount [in, out] Количество записей, которое необходимо считать.
									Сюда же запишется количество реально считанных записей.
	  \param	ppData [out] - содержит массив элементов
	  \return	bool - false, если достигнут конец списка.
	*/
		virtual bool GetNextItems(long& lCount, KLPAR::Params** ppData)=0;
	};

    class KLSTD_NOVTABLE ChunkAccessor : public KLSTD::KLBase
	{
	public:

	/*!
	  \brief	GetRecordsCount

	  \return	long - общее количество записей.
	*/
		virtual long GetRecordsCount()=0;

	/*!
	  \brief	GetChunk

	  \param	lStart [in] Номер первой записи, которую необходимо считать.
	  \param	lCount [in] Количество записей, которое необходимо считать.
	  \param	ppData [out] - содержит массив элементов.
	  \return	long - количество реально считанных записей.
	*/
		virtual long GetChunk(long lStart, long lCount, KLPAR::Params** ppData)=0;
	};
};

#endif //__KL__CSP_H__

// Local Variables:
// mode: C++
// End:
