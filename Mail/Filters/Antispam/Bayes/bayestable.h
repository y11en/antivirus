/*!
*		
*		
*		(C) 2002 Kaspersky Lab 
*		
*		\file	BayesTable.h
*		\brief	Определения функций BayesTable
*		
*		\author Vitaly DVi Denisov
*		\date	15.07.2004 
*		
*		Example:
		{
			CBayesTable hBayesTable;
			tERROR err = errOK;
			hOBJECT parent; // todo: init parent
			if ( PR_FAIL(err = hBayesTable.Init(parent, "c:\sfdb.dat")) ) 
				return err;

			CSFDBRecordKey key; // todo: init key
			CSFDBRecordData data; // todo: init data
			tBYTE hKey[16];
			if ( hBayesTable.HashKey(key, hKey) )
			{
			}
		}
*		
*/		

#ifndef _BAYESTABLE_H_
#define _BAYESTABLE_H_


#include "../sfdb_table.h"

#include <Prague/prague.h>

#include <memory>
#include <mapidefs.h>
#include <string>
#include <math.h>


#ifndef IN
#define IN
#endif
#ifndef OUT
#define OUT
#endif

	//////////////////////////////////////////////////////////////////////////
	//
	//	Структура данных, по которым будет рассчитан ключ для записи в SFDB
	//
	struct CSFDBRecordKey
	{
		//////////////////////////////////////////////////////////////////////////
		// 
		// Открытые члены структуры
		//
		tBYTE* pToken1;
		tULONG dwToken1Size;
		tBYTE* pToken2;
		tULONG dwToken2Size;
		// 
		// Открытые члены структуры
		//
		//////////////////////////////////////////////////////////////////////////
		
		
		///////////////////////////////////////////////////////////////////////////
		//! \fn				: GetBuffer
		//! \brief			:	Создает плоский буфер и укладывает в него все свои свойства, 
		//!						возвращает размер этого буфера.
		tERROR GetBuffer(OUT tBYTE** pBufData, OUT tULONG& dwBufSize)
		{
			tERROR err = errOK;
			dwBufSize = dwToken1Size + dwToken2Size + 2;
			if ( pBufData && !*pBufData ) 
			{
				*pBufData = new tBYTE[dwBufSize];
				if ( *pBufData ) 
				{
					memset(*pBufData, 0, dwBufSize);
					memcpy(*pBufData, pToken1, dwToken1Size);
					if ( dwToken2Size )
					{
						(*pBufData)[dwToken1Size] = ' '; // поставим разделитель, чтобы было удобнее смотреть
						memcpy(*pBufData +1+ dwToken1Size, pToken2, dwToken2Size);
						(*pBufData)[1+ dwToken1Size +dwToken2Size] = 0; // поставим разделитель, чтобы было удобнее смотреть
					}
				}
				else
					err = errWRITE_PROTECT;
			}
			else
				err = errWRITE_PROTECT;
			return err;
		};

		bool operator==(const CSFDBRecordKey& key) const
		{
			if ( 
				pToken1 && key.pToken1 && 
				dwToken1Size && 
				(dwToken1Size == key.dwToken1Size) &&
				0 == memcmp(pToken1, key.pToken1, dwToken1Size) &&
				(
					!pToken2 ||
					(
						key.pToken2 && 
						dwToken2Size && 
						(dwToken2Size == key.dwToken2Size) &&
						0 == memcmp(pToken2, key.pToken2, dwToken2Size)
					)
				)
				) 
				return true;
			return false;
		}

		CSFDBRecordKey():
			pToken1(0),
			dwToken1Size(0),
			pToken2(0),
			dwToken2Size(0)
		{
		}
	};
	//
	//	Структура данных, по которым будет рассчитан ключ для записи в SFDB
	//
	//////////////////////////////////////////////////////////////////////////


	//////////////////////////////////////////////////////////////////////////
	//
	//	Структура данных, хранимых в SFDB
	//
	struct CSFDBRecordData
	{
		unsigned long ulSpamCount;
		unsigned long ulHamCount;
		
		CSFDBRecordData():
			ulSpamCount(0),
			ulHamCount(0)
			{
			};
		
		void merge(CSFDBRecordData* p) 
		{
			ulSpamCount += p->ulSpamCount;
			ulHamCount  += p->ulHamCount;
		}

		///////////////////////////////////////////////////////////////////////////
		//! \fn				: UpdateCount
		//! \brief			:	Апдейтит данные
		//! \return			: bool 
		//! \param          : bool bIsSpam
		bool IncrementCount(bool bIsSpam)
		{
			if ( bIsSpam )
				ulSpamCount++;
			else
				ulHamCount++;
			return true;
		};
		bool DecrementCount(bool bIsSpam)
		{
			if ( bIsSpam )
				ulSpamCount--;
			else
				ulHamCount--;
			return true;
		};

		long double GetSpamP(unsigned long ulSpamCountAll)
		{
			return
				ulSpamCountAll ? 
				(long double)ulSpamCount/(long double)ulSpamCountAll : 0.4
				;
		}
		long double GetHamP(unsigned long ulHamCountAll)
		{
			return
				ulHamCountAll ?
				(long double)ulHamCount/(long double)ulHamCountAll : 0.6
				;
		}

		///////////////////////////////////////////////////////////////////////////
		//! \fn				: GetWeight
		//! \brief			:	Возвращает вес
		//! \return			: long double 
		long double GetWeight(CSFDBRecordData& dataCountAll)
		{
			long double dWeight = 0.4; // Вес токена
			long double _ulSpamCount = 
				dataCountAll.ulSpamCount ? 
				(long double)ulSpamCount/(long double)dataCountAll.ulSpamCount : 0.4
				;
			long double _ulHamCount = 
				dataCountAll.ulHamCount ?
				(long double)ulHamCount/(long double)dataCountAll.ulHamCount : 0.6
				;
			if ( max(_ulSpamCount, _ulHamCount) ) 
			{
//				if ( (ulSpamCount + ulHamCount) < 5 )
//					dWeight = 0.5;
				if ( !ulSpamCount || !ulHamCount )
//					dWeight =  
//						(sin(
//							(long double)((long double)_ulSpamCount - (long double)_ulHamCount) / 
//							(long double)max(_ulSpamCount, _ulHamCount)
//							) 
//						+ 1)/2 
//						;
					dWeight =  (long double)(ulSpamCount + 1)/(ulSpamCount + ulHamCount + 2);
				else 
					dWeight = 
						(long double)(_ulSpamCount) / 
						(long double)((long double)_ulSpamCount + (long double)_ulHamCount);
			}
			else
				dWeight = 0.4;
			return dWeight;
		};
	};
	//
	//	Структура данных, хранимых в SFDB
	//
	//////////////////////////////////////////////////////////////////////////

typedef CSFDBTable<CSFDBRecordKey, CSFDBRecordData> CSFDBTableForBayes;
class CBayesTable : public CSFDBTableForBayes
{
public:
	CBayesTable();
	~CBayesTable();

	///////////////////////////////////////////////////////////////////////////
	//! \fn				: Init
	//! \brief			:	Инициализирует базу SFDB
	//! \return			: tERROR 
	//! \param          : IN const hOBJECT parent
	//! \param          : IN tCHAR * sfdb_path
	tERROR Init(IN const hOBJECT parent, IN tCHAR * sfdb_path, IN tCHAR* sfdb_save_to);

private:
	hMSG_RECEIVER m_hMsgReceiver; //< Объект, предоставляющий функцию MsgReceive

};



#endif//_BAYESTABLE_H_
