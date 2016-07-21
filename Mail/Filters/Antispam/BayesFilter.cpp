// BayesFilter.cpp: implementation of the CBayesFilter class.
//
//////////////////////////////////////////////////////////////////////

#pragma warning(disable: 4786)

#include "stdafx.h"
#include "BayesFilter.h"
#include <stdlib.h>
#include <map>
#include <vector>
#include <algorithm>
#include <limits>
#include <iostream>
#include "fill_stop_words.h"
#include "stl_port.h"
#include "toupper.h"

//#define _USE_PAIR
#ifdef _DEBUG
	#define _DMP_DOUBLE_TOKENS
//	#define _DMP_TOKENS
#endif // _DEBUG

#define ABS(X) ((X>0) ? (X) : (-(X)))
#define BAYES(_a,_b) (_a)/((_a)+(_b))

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBayesFilter::CBayesFilter():
	m_pBayesTable(NULL)
{
	FILL_STOP_WORDS(m_stop_words);
}

CBayesFilter::~CBayesFilter()
{
	if ( m_pBayesTable ) 
	{
		delete m_pBayesTable;
		m_pBayesTable = NULL;
	}
}

tERROR CBayesFilter::Init(IN const hOBJECT parent, IN tCHAR * sfdb_path, IN tCHAR* sfdb_save_to)
{
	tERROR err = errOK;
	if ( !m_pBayesTable ) 
	{
		m_pBayesTable = new CBayesTable();
		if ( m_pBayesTable )
		{
			err = m_pBayesTable->Init((hOBJECT)parent, sfdb_path, sfdb_save_to);
			if ( PR_FAIL(err) )
			{
				delete m_pBayesTable;
				m_pBayesTable = NULL;
			}
		}
	}
	if ( !m_pBayesTable ) 
		err = errNOT_OK;
	return err;
}

void CBayesFilter::DeleteTwice(IN OUT tokens_t& tokens)
{
	// Исключаем спаренные токены, сохраняя их порядок
	typedef map<string, CTextFilter::token_t*> tokens_map_t;
	tokens_t tokens2 = tokens;
	int i = tokens.size();
	int j = 0;
	tokens_map_t tokens_map;
	tokens.clear();
	for (tokens_t::iterator 
		iTerator = tokens2.begin();
		iTerator != tokens2.end();
		++iTerator
		)
		{
			j++;
			string str = iTerator->token1;
			if ( iTerator->token2 )
			{
				str += " ";
				str += iTerator->token2;
			}
			string str_lower = str;
			ToLower(str_lower);
			if (
//				m_stop_words.find(str_lower) == m_stop_words.end() &&
				tokens_map.find(str) == tokens_map.end()
				)
			{
				tokens.push_back(*iTerator);
				tokens_map[str] = &(*iTerator);
			}
		}
}

tERROR CBayesFilter::UpdateSpamHam(IN /*bool bUpdateAsSpam*/ cAntispamTrainParams::Action_t update_action, unsigned long ulCount)
{
	tERROR err = errNOT_OK;
	char spamham[] = ">> SPAMHAM >>";
	unsigned __int64 hKey = 0L;
	CSFDBRecordKey key;
	key.pToken1 = (tBYTE*)spamham;
	key.dwToken1Size = strlen(spamham);
	if ( m_pBayesTable->HashKey(key, hKey) )
	{
		CSFDBRecordData data;
		m_pBayesTable->GetData(hKey, data);
		while (ulCount-- > 0)
		{
			switch (update_action) 
			{
			case cAntispamTrainParams::TrainAsSpam:    data.IncrementCount(true);  break;
			case cAntispamTrainParams::TrainAsHam:     data.IncrementCount(false); break;
			case cAntispamTrainParams::RemoveFromSpam: data.DecrementCount(true);  break;
			case cAntispamTrainParams::RemoveFromHam:  data.DecrementCount(false); break;
			}
		}
		err = m_pBayesTable->Update(hKey, data);
	}
	return err;
}

tERROR CBayesFilter::UpdateSpamHamLetters(IN /*bool bUpdateAsSpam*/ cAntispamTrainParams::Action_t update_action)
{
	tERROR err = errNOT_OK;
	char spamham[] = ">> SPAMHAM_LETTERS >>";
	unsigned __int64 hKey = 0L;
	CSFDBRecordKey key;
	key.pToken1 = (tBYTE*)spamham;
	key.dwToken1Size = strlen(spamham);
	if ( m_pBayesTable->HashKey(key, hKey) )
	{
		CSFDBRecordData data;
		m_pBayesTable->GetData(hKey, data);
		switch (update_action) 
		{
		case cAntispamTrainParams::TrainAsSpam:    data.IncrementCount(true);  break;
		case cAntispamTrainParams::TrainAsHam:     data.IncrementCount(false); break;
		case cAntispamTrainParams::RemoveFromSpam: data.DecrementCount(true);  break;
		case cAntispamTrainParams::RemoveFromHam:  data.DecrementCount(false); break;
		}
		err = m_pBayesTable->Update(hKey, data);
	}
	return err;
}

//bool CBayesFilter::GetSpamHam(
//	OUT CSFDBRecordData& data, 
//	OUT OPTIONAL unsigned __int64* phKey 
//	)
//{
//	return GetSpamHam( data, m_pBayesTable, NULL, phKey );
//}
//bool CBayesFilter::GetSpamHam(
//	OUT CSFDBRecordData& data, 
//	IN OPTIONAL CBayesTable* pBayesTable, 
//	IN OPTIONAL hSECUREFILEDB hDataBase,
//	OUT OPTIONAL unsigned __int64* phKey
//	)
//{
//	return GetData( ">> SPAMHAM >>", data, pBayesTable, NULL, phKey );
//}
bool CBayesFilter::GetData(
	IN char* pszRecordName,
	OUT CSFDBRecordData& data, 
	OUT OPTIONAL unsigned __int64* phKey 
	)
{
	return GetData( pszRecordName, data, m_pBayesTable, NULL, phKey );
}
bool CBayesFilter::GetData(
	IN char* pszRecordName,
	OUT CSFDBRecordData& data, 
	IN OPTIONAL CBayesTable* pBayesTable, 
	IN OPTIONAL hSECUREFILEDB hDataBase,
	OUT OPTIONAL unsigned __int64* phKey
	)
{
	bool bRes = false;
	if ( !(pBayesTable||hDataBase) )
		return bRes;
	
	unsigned __int64 hKey = 0L;
	unsigned __int64* _phKey = phKey ? phKey : &hKey;
	CSFDBRecordKey key;
	key.pToken1 = (tBYTE*)pszRecordName;
	key.dwToken1Size = strlen(pszRecordName);
	if ( CBayesTable::HashKey(key, *_phKey) )
	{
		if ( pBayesTable )
			bRes = pBayesTable->GetData(*_phKey, data);
		else if ( hDataBase )
			bRes = CBayesTable::GetData(hDataBase, *_phKey, data);
	}
	return bRes;
}

tERROR CBayesFilter::Update(IN const tokens_t& tokens, IN /*bool bUpdateAsSpam*/ cAntispamTrainParams::Action_t update_action)
{
	cERROR err = errOK;
	if ( m_pBayesTable )
	{
		unsigned long i = 0;
		for (tokens_t::const_iterator 
			iTerator = tokens.begin();
			iTerator != tokens.end();
			++iTerator
			)
			{
				unsigned __int64 hKey = 0L;
				CSFDBRecordKey key;
				key.pToken1 = (tBYTE*)(iTerator->token1);
				key.dwToken1Size = stl_port::strlen(iTerator->token1);
				if ( m_pBayesTable->HashKey(key, hKey) )
				{
					CSFDBRecordData data;
					m_pBayesTable->GetData(hKey, data);
					switch (update_action) 
					{
					case cAntispamTrainParams::TrainAsSpam:    data.IncrementCount(true);  break;
					case cAntispamTrainParams::TrainAsHam:     data.IncrementCount(false); break;
					case cAntispamTrainParams::RemoveFromSpam: data.DecrementCount(true);  break;
					case cAntispamTrainParams::RemoveFromHam:  data.DecrementCount(false); break;
					}
					if ( PR_FAIL(err = m_pBayesTable->Update(hKey, data)) )
						break;
					i++;
				}
#ifdef _USE_PAIR
				// Добавляем инфу по спаренным токенам
				if ( iTerator != tokens.begin() )
				{
					tokens_t::const_iterator iiTerator = iTerator;
					iiTerator--;
					key.pToken1 = (tBYTE*)(iiTerator->token1);
					key.dwToken1Size = stl_port::strlen(iiTerator->token1);
					key.pToken2 = (tBYTE*)(iTerator->token1);
					key.dwToken2Size = stl_port::strlen(iTerator->token1);
					if ( m_pBayesTable->HashKey(key, hKey) )
					{
						CSFDBRecordData data;
						m_pBayesTable->GetData(hKey, data);
						switch (update_action) 
						{
						case cAntispamTrainParams::TrainAsSpam:    data.IncrementCount(true);  break;
						case cAntispamTrainParams::TrainAsHam:     data.IncrementCount(false); break;
						case cAntispamTrainParams::RemoveFromSpam: data.DecrementCount(true);  break;
						case cAntispamTrainParams::RemoveFromHam:  data.DecrementCount(false); break;
						}
						if ( PR_FAIL(err = m_pBayesTable->Update(hKey, data)) )
							break;
						i++;
					}
				}
#endif // _USE_PAIR
			}
		if ( i )
		{
			UpdateSpamHam(update_action, i);
			UpdateSpamHamLetters(update_action);
		}
	}
	return err;
}

#define UPDATEBAYESPARAMETERS(_a, _b, _w,  _K)	\
	long double w = 0.5 + (_w - 0.5) / _K;	\
	_a = _a * w ;	\
	_b = _b * (1 - w);	\
	if ( (_a < 0.00001) && (_b < 0.00001) ) \
	{	\
		_a *= 100;	\
		_b *= 100;	\
	}	

long double CBayesFilter::Process(IN const tokens_t& tokens)
{
	long double a = 1;
	long double b = 1;
	long double S = 0.5;
	long double P = 1;
	long double Q = 1;
	int i = 0;
	int iInteresting = 0;
	// Оцениваем не больше 1000 токенов
	const int max_tokens_count = 1000;
	// Оцениваем не больше 15 интересных токенов
	const int max_interesting_tokens_count = 15;
	// Флаг, поднимающийся, если хватило интересных токенов
	bool bInteresting = false;
	// Узнаем количество записей в базе
	CSFDBRecordData dataCountAll;
	// Векстор весов токенов
	typedef vector<long double> weights_t;
	weights_t weights;	// Вектор одиночных токенов
	long double sum_weights = 0;

	div_t div_result = div( tokens.size(), max_tokens_count );
	int j = div_result.quot;
	int k = 0;
	int n = 0;
	long double c = 1;
	long double KCrossed = 1;

	if ( m_pBayesTable )
	{
		GetData(">> SPAMHAM >>", dataCountAll);
		c = (long double)dataCountAll.ulSpamCount/ (long double)( dataCountAll.ulSpamCount+ dataCountAll.ulHamCount );
		for (tokens_t::const_iterator 
			iTerator = tokens.begin();
			iTerator != tokens.end();
			++iTerator
			)
			{
				if ( i > max_tokens_count ) 
					break;
				
				if ( i == k ) // Следует ли оценивать данный токен?
				{
					unsigned __int64 hKey = 0L;
					CSFDBRecordKey key;
					key.pToken1 = (tBYTE*)(iTerator->token1);
					key.dwToken1Size = stl_port::strlen(iTerator->token1);
					key.pToken2 = (tBYTE*)(iTerator->token2);
					key.dwToken2Size = stl_port::strlen(iTerator->token2);
					if ( m_pBayesTable->HashKey(key, hKey) )
					{
						CSFDBRecordData data;
						bool bFound = m_pBayesTable->GetData(hKey, data);
						long double weight = data.GetWeight(dataCountAll);
						weights.push_back(weight);
						sum_weights += weight;
#ifdef _DMP_TOKENS
						{
							cout << key.pToken1;
							cout << "; ";
							cout << weight;
							cout << "\n";
						}
#endif // _DEBUG
					}

#ifdef _USE_PAIR
					//////////////////////////////////////////////////////////////////////////
					//
					// Добавляем веса спаренных токенов
					//
					{
						tokens_t::const_iterator iiTerator = iTerator;
						int iStep = 0;
						iiTerator++;
						const int nMaxSteps = 1;
						while ( 
							(iiTerator != tokens.end()) &&
							(iStep < nMaxSteps)
							)
						{
							iStep++;
							key.pToken1 = (tBYTE*)(iTerator->token1);
							key.dwToken1Size = stl_port::strlen(iTerator->token1);
							key.pToken2 = (tBYTE*)(iiTerator->token1);
							key.dwToken2Size = stl_port::strlen(iiTerator->token1);
							if ( m_pBayesTable->HashKey(key, hKey) )
							{
								CSFDBRecordData data;
								bool bFound = m_pBayesTable->GetData(hKey, data);
								if ( bFound ) 
								{
									long double weight2 = data.GetWeight(dataCountAll);
									
									//weight2 = 0.5 + (weight2 - 0.5) * (nMaxSteps - iStep);
									weights.push_back(weight2);
									sum_weights += weight2;
#ifdef _DMP_DOUBLE_TOKENS
										{
											cout << key.pToken1;
											cout << " ";
											cout << key.pToken2;
											cout << " [" << iStep+1 << "] ";
											cout << "; ";
											cout << weight2;
											cout << "\n";
										}
#endif // _DMP_DOUBLE_TOKENS
								}
							}
							iiTerator++;
						}
					}
					//
					// Добавляем веса спаренных токенов
					//
					//////////////////////////////////////////////////////////////////////////
#endif // _USE_PAIR

					i++;
					k = i+j; // Вычисляем, какой следующий токен будем оценивать
					n++;     // Подсчитаем, сколько токенов мы всего оценили
				}
				else
					i++;
			}

		{
			a = 1;
			b = 1;
			sort(weights.begin(), weights.end());
			unsigned long ulTokensCount = weights.size();
			long double K = ABS(ulTokensCount * (sum_weights/ulTokensCount - 0.5)) + 1;
			if ( ulTokensCount ) 
			{
				unsigned long i = 0;
				for ( i = 0; i < ulTokensCount; i++ )
				{
					UPDATEBAYESPARAMETERS(a, b, weights[i], K);
					if ( KCrossed == 1 )
					{
						long double S1 = BAYES(a,b);
						// Ищем границу пересечения с 0,5 (это - начальное значение S)
						if ( S1 > S )
							KCrossed = (long double)ulTokensCount / (ulTokensCount - i);
					}
				} 

				// Вычисляем вероятность по Байесу
				S = BAYES(a,b);

				// Если график не пересек границу в 0,5 - стоит поискать пересечение с S
				if ( KCrossed == 1 ) 
				{
					a = 1;
					b = 1;
					for ( i = 0; i < ulTokensCount; i++ )
					{
						UPDATEBAYESPARAMETERS(a, b, weights[i], K);
						long double S1 = BAYES(a,b);
						if ( S1 < S )
						{
							KCrossed = (long double)ulTokensCount / (i+1);
							break;
						}
					} 
				}
			}
		}
	}

	S = BAYES(a,b);

	if ( KCrossed != 1 ) 
		S = (S - 0.5) / KCrossed + 0.5;

	return S;
}

