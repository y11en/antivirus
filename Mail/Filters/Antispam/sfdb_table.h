// whitelist.h: interface for the CWhiteList class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SFDB_TABLE_H__7DDA834B_DF85_4ED1_A831_EB6B30D23500__INCLUDED_)
#define AFX_SFDB_TABLE_H__7DDA834B_DF85_4ED1_A831_EB6B30D23500__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <Prague/prague.h>
#include <Prague/pr_cpp.h>

#include <ProductCore/iface/i_sfdb.h>
#include <ProductCore/plugin/p_sfdb.h>

#include <string>

#ifndef IN
#define IN
#endif
#ifndef OUT
#define OUT
#endif

extern void HashData(tULONG ulData, const tBYTE* lpData, tBYTE pHashData[8]);

template<class K, class D> class CSFDBTable
{
public:
	CSFDBTable():
		m_hDataBase(NULL)
	{};
	virtual ~CSFDBTable()
	{
		if( m_hDataBase )
		{
			SaveTo( (tCHAR*)m_szFileName.c_str(cCP_ANSI) );
			m_hDataBase->sysCloseObject();
		}
		m_hDataBase = NULL;
	};

	///////////////////////////////////////////////////////////////////////////
	//! \fn				: HashKey
	//! \brief			:	Создает хеш структуры CSFDBRecordKey.
	//!						Этот хеш используется в качестве ключа записи SFDB
	//! \return			: static bool 
	//! \param          : IN K& key
	//! \param          : OUT unsigned __int64& hKey
	static bool HashKey(IN K& key, OUT unsigned __int64& hKey)
	{
		tBYTE* pBufData = NULL;
		tULONG dwBufSize = 0;
		if ( PR_FAIL(key.GetBuffer(&pBufData, dwBufSize)) )
			return false;
		HashData(dwBufSize, pBufData, (tBYTE*)&hKey);
		delete []pBufData;
		return true;
	};

	///////////////////////////////////////////////////////////////////////////
	//! \fn				: Init
	//! \brief			:	Инициализирует базу SFDB
	//! \return			: tERROR 
	//! \param          : IN const hOBJECT parent
	//! \param          : IN tCHAR * sfdb_path
	//! \param          : IN tCHAR * sfdb_path_save_to
	tERROR Init(IN const hOBJECT parent, IN tCHAR * sfdb_path, IN tCHAR * sfdb_save_to)
	{
		tERROR error = errOK;

		if ( !m_hDataBase ) 
		{
			PR_TRACE_A0( this, "Enter CSFDBTable::Init" );

			
			cStrObj str_sfdb_save_to(sfdb_save_to);
			cStrObj str_sfdb_path(sfdb_path);
			parent->sysSendMsg(pmc_PRODUCT_ENVIRONMENT, pm_EXPAND_ENVIRONMENT_STRING, (hOBJECT)cAutoString(str_sfdb_save_to), 0, 0);
			parent->sysSendMsg(pmc_PRODUCT_ENVIRONMENT, pm_EXPAND_ENVIRONMENT_STRING, (hOBJECT)cAutoString(str_sfdb_path), 0, 0);
			if ( sfdb_save_to )
				m_szFileName = str_sfdb_save_to;
			else if ( sfdb_path )
				m_szFileName = str_sfdb_path;

			error = parent->sysCreateObjectQuick( 
				(hOBJECT*)&m_hDataBase, 
				IID_SECUREFILEDB, 
				PID_SFDB, 
				SUBTYPE_ANY 
				);
			if ( PR_SUCC(error) )
				error = m_hDataBase->propSetDWord( 
					plSERIALIZE_OPTIONS, 
					0
					);
			if( PR_SUCC(error) )
				error = m_hDataBase->propSetDWord( 
					plDATABASE_SIZE_LIMIT, 0x2000000 // 32 Mb
					);
			
			if ( PR_SUCC(error) && !str_sfdb_path.empty() )
				error = m_hDataBase->Load( str_sfdb_path.c_str(cCP_ANSI) );

			if ( PR_FAIL(error) && m_hDataBase )
				error = m_hDataBase->InitNew( 
					1, 
					8, 
					sizeof(D), 
					0
					);

			if( PR_FAIL(error) )
			{
				if( m_hDataBase )
					m_hDataBase->sysCloseObject();
				m_hDataBase = NULL;
			}
		}
		
		PR_TRACE_A0( this, "Leave CSFDBTable::Init" );
		return error;
	};

	///////////////////////////////////////////////////////////////////////////
	//! \fn				: GetData
	//! \brief			:	Проверяет наличие записи в SFDB по ключу hKey 
	//!						и возвращает найденные данные
	//! \return			: bool 
	//! \param          : IN unsigned __int64& hKey
	//! \param          : IN D& hData
	bool GetData(IN unsigned __int64 hKey, OUT D& hData)
	{ return GetData( m_hDataBase, hKey, hData ); };
	static bool GetData(IN hSECUREFILEDB hDataBase, IN unsigned __int64 hKey, OUT D& hData)
	{
		tERROR error = hDataBase->GetRecord((tVOID*)&hKey, (tVOID*)&hData);
		if ( PR_SUCC(error) )
		{
			// Такая же запись есть в базе. 
			return true;
		}
		else
			return false;
	};

	///////////////////////////////////////////////////////////////////////////
	//! \fn				: Update
	//! \brief			:	Добавляет/Изменяет запись по ключу hKey
	//! \return			: void 
	//! \param          : IN unsigned __int64& hKey
	//! \param          : IN CSFDBRecordData& hData
	tERROR Update(IN unsigned __int64& hKey, IN D& hData)
	{ return m_hDataBase->UpdateRecord((tVOID*)&hKey, &hData); };

	///////////////////////////////////////////////////////////////////////////
	//! \fn				: Delete
	//! \brief			:	Удаляет запись по ключу hKey
	//! \return			: tERROR 
	//! \param          : IN unsigned __int64& hKey
	tERROR Delete(IN unsigned __int64& hKey)
	{ return m_hDataBase->DeleteRecord((tVOID*)&hKey); };

	///////////////////////////////////////////////////////////////////////////
	//! \fn				: ClearDB
	//! \brief			:	Полная очистка базы
	//! \return			: tERROR 
	tERROR ClearDB()
	{ return m_hDataBase->CleanDB(); };
	
	///////////////////////////////////////////////////////////////////////////
	//! \fn				: SaveTo
	//! \brief			:	Сохраняет базу в указанный файл
	//! \return			: tERROR 
	//! \param          : tCHAR* sfdb_path
	tERROR SaveTo( tCHAR* sfdb_path )
	{ 
		cStrObj str_sfdb_path(sfdb_path ? sfdb_path : m_szFileName);
		m_hDataBase->sysSendMsg(pmc_PRODUCT_ENVIRONMENT, pm_EXPAND_ENVIRONMENT_STRING, (hOBJECT)cAutoString(str_sfdb_path), 0, 0);
		return m_hDataBase->Save(str_sfdb_path.c_str(cCP_ANSI)); 
	};

	///////////////////////////////////////////////////////////////////////////
	//! \fn				: GetRecord
	//! \brief			:	Возвращает содержимое записи по ее порядковому номеру.
	//!						Функцию имеет смысл использовать только для енумерации записей, 
	//!						и производить это в критической секции, не позволяя делать 
	//!						Update и Delete, чтобы не допустить смещения записей.
	//!						Если запись не найдена, возвращает errNOT_FOUND
	//! \return			: tERROR 
	//! \param          : IN tDWORD nRecordNum
	//! \param          : OUT unsigned __int64& hKey
	//! \param          : OUT D& hData
	tERROR GetRecord(IN tDWORD nRecordNum, OUT unsigned __int64& hKey, OUT D& hData )
	{ return m_hDataBase->EnumRecords( nRecordNum, &hKey, &hData ); };

	tERROR Merge(CSFDBTable<K,D>* pDataToMerge)
	{
		cERROR err = errOK;
		tDWORD dwRecordNum = 0;
		unsigned __int64 hSourceKey = 0;
		D  hSourceData;
		while ( PR_SUCC(err= pDataToMerge->GetRecord(dwRecordNum++, hSourceKey, hSourceData) ))
		{
			cERROR err = errOK;
			unsigned __int64 hTargetKey = hSourceKey;
			D  hTargetData;
			GetData( hTargetKey, hTargetData );
			hTargetData.merge(&hSourceData);
			Update( hTargetKey, hTargetData );
		} 
		if ( err == errNOT_FOUND )
			err = errOK;
		return err;
	}

protected:
	hSECUREFILEDB m_hDataBase;
	cStringObj m_szFileName;
};

#endif // !defined(AFX_SFDB_TABLE_H__7DDA834B_DF85_4ED1_A831_EB6B30D23500__INCLUDED_)
