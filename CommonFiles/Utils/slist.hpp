/*!
*		
*		
*		(C) 2003 Kaspersky Lab 
*		
*		\file	slist.hpp
*		\brief	простейший односвязанный список типов
*		
*		\author Владислав Овчарик
*		\date	11.05.2005 14:06:46
*		
*		
*/		
#if !defined(_SIMPLE_TYPE_LIST_INCLUDED_HPP_)
#define _SIMPLE_TYPE_LIST_INCLUDED_HPP_
///////////////////////////////////////////////////////////////////////////////
namespace util
{
///////////////////////////////////////////////////////////////////////////////
/**
 * \breif	- workaround for the "Error C2516. : is not a legal base class"
 */
template<class T, class U = int>
struct vc_base_workaround
		: public T
{
};
///////////////////////////////////////////////////////////////////////////////
/**
 * \breif	-	односвязанный список типов
 */
template <typename H, typename T>
struct slist
{
	typedef H head;
	typedef T tail;
};
//todo: НЕОБХОДИМО ДОДЕЛАТЬ ЕЩЕ БАЗОВЫЕ ОПЕРАЦИИ СО СПИСКОМ - ВЫЧИСЛЕНИЕ РАЗМЕРА
//		ВСТАВКА НОВОГО ЭЛЕМЕНТА, УДАЛЕНИЕ, ЗАМЕНА
///////////////////////////////////////////////////////////////////////////////
}//namespace util
///////////////////////////////////////////////////////////////////////////////
#endif//!defined(_SIMPLE_TYPE_LIST_INCLUDED_HPP_)