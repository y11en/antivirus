/*!
*		
*		
*		(C) 2003 Kaspersky Lab 
*		
*		\file	inherit_linearly.hpp
*		\brief	генератор линейной иерархии
*		
*		\author Владислав Овчарик
*		\date	11.05.2005 13:53:19
*		
*		
*/		
#if !defined(_INHERIT_LINIEARITY_INCLUDED_HPP_)
#define _INHERIT_LINIEARITY_INCLUDED_HPP_
///////////////////////////////////////////////////////////////////////////////
#include <boost/mpl/apply.hpp>
#include <boost/mpl/empty_base.hpp>
#include <boost/mpl/lambda.hpp>
///////////////////////////////////////////////////////////////////////////////
namespace util
{
///////////////////////////////////////////////////////////////////////////////
namespace mpl = boost::mpl;
///////////////////////////////////////////////////////////////////////////////
//fwd
template<typename tlist, typename lambdafn> struct inherit_linearly;
///////////////////////////////////////////////////////////////////////////////
namespace aux
{
///////////////////////////////////////////////////////////////////////////////
/**
 * \breif	-	реализациия генератора линейной иерархии
 */
template<bool C1>
struct inherit_linearly_impl
{
	template<typename L, typename F> struct result_
		: vc_base_workaround<typename mpl::apply1<typename mpl::lambda<F>::type, typename L::head>::type, typename L::tail>
		, inherit_linearly<typename L::tail, F>
	{
		typedef inherit_linearly<L, F> type_;
	};
};
///////////////////////////////////////////////////////////////////////////////
/**
 * \breif	-	для остановки рекурсии при генерации иерархии
 */
template<>
struct inherit_linearly_impl<true>
{
	template<typename L, typename F> struct result_
	{
		typedef L type_;
	};
};
///////////////////////////////////////////////////////////////////////////////
}//namespace aux
///////////////////////////////////////////////////////////////////////////////
/**
 * \breif	-	генератор линейной иерархии классов
 *				tlist - список типов slist<T1, T2, T3, ..., Tn>
 *				metafn - мета функция, возвращающая элемент иерархии
 *				type - результат генерации
 */
template<typename tlist, typename metafn>
struct inherit_linearly
		: aux::inherit_linearly_impl<mpl::is_empty_base<tlist>::value>::template result_<tlist, metafn>
{
	typedef typename inherit_linearly::type_ type;
};
///////////////////////////////////////////////////////////////////////////////
}//namespace util
#endif//!defined(_INHERIT_LINIEARITY_INCLUDED_HPP_)