// BayesFilter.h: interface for the CBayesFilter class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BAYESFILTER_H__3A93A1A9_2B99_4DD1_8F7D_5A936D4F9446__INCLUDED_)
#define AFX_BAYESFILTER_H__3A93A1A9_2B99_4DD1_8F7D_5A936D4F9446__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Bayes/bayestable.h"
#include "textfilter.h"

#include <Mail/structs/s_antispam.h>

#include <vector>
#include <string>
#include <map>

using namespace std;

class CBayesFilter  
{
public:
	CBayesFilter();
	virtual ~CBayesFilter();
	
	typedef vector<CTextFilter::token_t> tokens_t;


	///////////////////////////////////////////////////////////////////////////
	//! \fn				: Process
	//! \brief			:	Оценивает токены. Возвращает вероятность спама {0..1}
	//! \return			: long double 
	//! \param          : IN const tokens_t& tokens
	long double Process(IN const tokens_t& tokens);

	///////////////////////////////////////////////////////////////////////////
	//! \fn				: Update
	//! \brief			:	Пересчитывает весовые коэффициенты. 
	//!						Если bUpdateAsSpam - то в сторону увеличения, 
	//!						иначе - наоборот
	//! \return			: tERROR 
	//! \param          : IN const tokens_t& tokens
	//! \param          : IN bool bUpdateAsSpam
	tERROR Update(IN const tokens_t& tokens, IN /*bool bUpdateAsSpam*/ cAntispamTrainParams::Action_t update_action);

	///////////////////////////////////////////////////////////////////////////
	//! \fn				: Add
	//! \brief			:	Добавляет неизвестные токены как новые в базу 
	//!						(использовать только для первичного обучения 
	//!						на точно определенном как спам/не_спам письме).
	//!						Для автообучения по результатам сканирования 
	//!						следует использовать метод Update
	//! \return			: inline tERROR 
	//! \param          : IN const tokens_t& tokens
	//! \param          : IN bool bUpdateAsSpam
	inline tERROR Add(IN const tokens_t& tokens, IN /*bool bUpdateAsSpam*/ cAntispamTrainParams::Action_t update_action)
	{ return Update(tokens, update_action); };

	inline tERROR ClearDB()
	{ return m_pBayesTable->ClearDB(); };

	inline tERROR Merge(CBayesFilter* pFilter)
	{ return m_pBayesTable->Merge(pFilter->GetBayesTable()); };

	inline tERROR Save()
	{ return m_pBayesTable->SaveTo(NULL); };
	///////////////////////////////////////////////////////////////////////////
	//! \fn				: DeleteTwice
	//! \brief			:	Удаляет сдвоенные токены из вектора
	//! \return			: void 
	//! \param          : IN OUT tokens_t& tokens
	void DeleteTwice(IN OUT tokens_t& tokens);

	///////////////////////////////////////////////////////////////////////////
	//! \fn				: Init
	//! \brief			:	Инициализирует базу SFDB
	//! \return			: tERROR 
	//! \param          : IN const hOBJECT parent
	//! \param          : IN tCHAR * sfdb_path
	//! \param          : IN tCHAR * sfdb_save_to
	tERROR Init(IN const hOBJECT parent, IN tCHAR * sfdb_path, IN tCHAR* sfdb_save_to);

	///////////////////////////////////////////////////////////////////////////
	//! \fn				: GetSpamHam
	//! \brief			:	Возвращает объем базы спама и базы не-спама в структуре CSFDBRecordData
	//!						Для работы используется либо pBayesTable, либо hDataBase
	//!						Если задан phKey, то он тоже возвращается
	//! \return			: static bool 
	//! \param          : IN CBayesTable* pBayesTable
	//! \param          : OUT CSFDBRecordData& data
	//! \param          : OUT OPTIONAL CBayesTable::CSFDBRecordKey* key
	//! \param          : IN OPTIONAL CBayesTable* pBayesTable
	//! \param          : IN OPTIONAL unsigned __int64* phKey = NULL
//	static bool GetSpamHam(
//		OUT CSFDBRecordData& data, 
//		IN OPTIONAL CBayesTable* pBayesTable, 
//		IN OPTIONAL hSECUREFILEDB hDataBase,
//		OUT OPTIONAL unsigned __int64* phKey = NULL
//		);
	
	bool GetData(
		IN char* pszRecordName,
		OUT CSFDBRecordData& data, 
		OUT OPTIONAL unsigned __int64* phKey = NULL
		);
	static bool CBayesFilter::GetData(
		IN char* pszRecordName,
		OUT CSFDBRecordData& data, 
		IN OPTIONAL CBayesTable* pBayesTable, 
		IN OPTIONAL hSECUREFILEDB hDataBase,
		OUT OPTIONAL unsigned __int64* phKey = NULL
		);

	inline CBayesTable* GetBayesTable() const { return m_pBayesTable; }

private:
	CBayesTable* m_pBayesTable;
	tERROR UpdateSpamHamLetters(IN /*bool bUpdateAsSpamt*/ cAntispamTrainParams::Action_t update_action);
	tERROR UpdateSpamHam(IN /*bool bUpdateAsSpam*/ cAntispamTrainParams::Action_t update_action, unsigned long ulCount);
	typedef map<string, int> stop_words_t;
	stop_words_t m_stop_words;
};

#endif // !defined(AFX_BAYESFILTER_H__3A93A1A9_2B99_4DD1_8F7D_5A936D4F9446__INCLUDED_)
