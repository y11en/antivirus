/*!
 * (C) 2003 Kaspersky Lab 
 * 
 * \file	klsecurity/exports.h
 * \author	Mikhail Karmazine
 * \date	28.02.2003 12:44:11
 * \brief	Экспортируемые функции, которые должны поддерживать все реализации
 *              библиотеки klsecurity.
 *          Каждая такая реализация не должна зависеть от других библиотек (от KLCSC,
 *          KLCSS, и т.д.), чтобы не было циклических зависимостей библиотек 
 *          друг от друга (т.к. функциональность klsecurity может требоваться 
 *          в любой такой библиотеке).
 *
 *          !!!!!!!!!!!!  ОЧЕНЬ ВАЖНО:   !!!!!!!!!!!!!
 *          После деплоймента в данный файл функции могут только добавляться,
 *          заголовки старых функций не должны меняться. При добавлении новых 
 *          функций надо не забыть внести их в файл Exports.def, проставив  
 *          для новых функций новые ordinals, не трогая ordinals для старых 
 *          функций.
 */

#ifndef __KLSECURITY_EXPORTS_H__
#define __KLSECURITY_EXPORTS_H__

#include <string>
#include "std/base/kldefs.h"
#include "kca/prci/componentid.h"
#include "transport/avt/acedecl.h"
#include "transport/klsecurity/common.h"

#ifdef KLSECURITY_EXPORTS
# define KLSECURITY_DECL     KLSTD_DLLEXPORT
#else
# define KLSECURITY_DECL     KLSTD_DLLIMPORT
#endif

/*!
  \brief	KLSEC_GetDLLType - возвращает тип DLL

  \return	KLSEC::DLLType 
*/
KLSECURITY_DECL KLSEC::DLLType KLSEC_GetDLLType();
typedef KLSEC::DLLType (*KLSEC_PGetDLLType)();


/*!
  \brief	KLSEC_GetDLLVersion - возвращает версию DLL

  \return	KLSEC::DLLVersion 
*/
KLSECURITY_DECL KLSEC::DLLVersion KLSEC_GetDLLVersion();
typedef KLSEC::DLLVersion (*KLSEC_PGetDLLVersion)();



/*!
  \brief	KLSEC_GetACLDeclForComponent
    Возвращает массив из структур KLAVT::ACE_DECLARATION и размер этого массива.
    (Этот массив затем можно передать в функицю KLAVT_CreateACL для получения
    объекта, инкапсулирующего собственно ACL) Каких-либо операций по удалению
    полученного массива выполнять не нужно.

    Какой именно список будет получен в результате, в общем случае зависит от
        ID компоненты и действия, которое мы хотим над ней произвести.

    !!!!!!! Функция не выбрасывает исключений, а возвращает результат true/false !!!!!

  \param	LPRCI::ComponentId compId - ID компоненты.
  \param	const KLAVT::ACE_DECLARATION** ppAceDeclList - массив с описанием ACE
  \param	int* pnAceCount [out] - размер массива
  \return	bool - true если все хорошо, false в противном случае.
*/

KLSECURITY_DECL bool KLSEC_GetACLDeclForComponent(
							const KLPRCI::ComponentId&			compId,
							const KLAVT::ACE_DECLARATION**		ppAceDeclList,
							long*								pnAceCount);

typedef bool (*KLSEC_PGetACLDeclForComponent)(
							const KLPRCI::ComponentId&			compId,
							const KLAVT::ACE_DECLARATION**		ppAceDeclList,
							long*								pnAceCount);


KLSECURITY_DECL bool KLSEC_GetACLDeclForComponentTask(
							const KLPRCI::ComponentId&			compId,
							const std::wstring&					wstrTaskName,
							const KLAVT::ACE_DECLARATION**		ppAceDeclList,
							long*								pnAceCount);

typedef bool (*KLSEC_PGetACLDeclForComponentTask)(
							const KLPRCI::ComponentId&			compId,
							const std::wstring&					wstrTaskName,
							const KLAVT::ACE_DECLARATION**		ppAceDeclList,
							long*								pnAceCount);

typedef struct
{
    const wchar_t*  szwType;
    long            lType;
    long            lSubType;
}KLSEC_SECTION_DESC, *PKLSEC_SECTION_DESC;


KLSECURITY_DECL bool KLSEC_GetACLDeclForSection(
                            const KLSEC_SECTION_DESC&           type,
                            const wchar_t*                      szwProduct,
                            const wchar_t*                      szwVersion,
                            const wchar_t*                      szwSection,
							const KLAVT::ACE_DECLARATION**		ppAceDeclList,
							long*								pnAceCount);

typedef bool (*KLSEC_PGetACLDeclForSection)(
                            const KLSEC_SECTION_DESC&           type,
                            const wchar_t*                      szwProduct,
                            const wchar_t*                      szwVersion,
                            const wchar_t*                      szwSection,
							const KLAVT::ACE_DECLARATION**		ppAceDeclList,
							long*								pnAceCount);

KLSECURITY_DECL bool KLSEC_GetACLDeclForAction(
							AVP_dword							dwActionGroupID,
							const KLAVT::ACE_DECLARATION**		ppAceDeclList,
							long*								pnAceCount);

typedef bool (*KLSEC_PGetACLDeclForAction)(
							AVP_dword							dwActionGroupID,
							const KLAVT::ACE_DECLARATION**		ppAceDeclList,
							long*								pnAceCount);


/*!
  \brief	Освобождает память, занимаемую ACL
*/
KLSECURITY_DECL void KLSEC_FreeACL(
							const KLAVT::ACE_DECLARATION*		pAceDeclList);

typedef void (*KLSEC_PFreeACL)(
							const KLAVT::ACE_DECLARATION*		pAceDeclList);


/*!
  \brief	Вычисляет длину ACL
  \param	pAceDeclList [in]
  \return	-1 в случае ошибки, иначе длина
*/
KLSECURITY_DECL long KLSEC_GetACLLen(
							const KLAVT::ACE_DECLARATION* pAceDeclList);

typedef long (*KLSEC_PGetACLLen)(
							const KLAVT::ACE_DECLARATION* pAceDeclList);


#endif //__KLSECURITY_EXPORTS_H__

// Local Variables:
// mode: C++
// End:
