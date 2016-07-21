#if !defined(AFX_CKAHSTORAGE_H__A5A05CC4_95EB_430E_80A3_487E55CB561B__INCLUDED_)
#define AFX_CKAHSTORAGE_H__A5A05CC4_95EB_430E_80A3_487E55CB561B__INCLUDED_

#include <CKAH/ckahumdefs.h>

// интерфейс дл€ работы с persistent storage
class CCKAHStorage
{
public:
	//
	// имена параметров имеют вид:
	//
	// Name1\Name2\...\NameN
	//
	// тип параметров - нуль-терминированна€ строка
	//

	// прочитать параметр szParamName в буфер szParamValue длины nParamValueBufferLength
	// возвращает размер буфера в байтах, требуемый дл€ размещени€ параметра, включа€ последний '\0'
	// 0 - в случае ошибки
	// можно узнать необходимую длину выходного буфера, передав в 2 и 3 параметры 0 и NULL соответственно
	virtual DWORD GetParam (LPCWSTR IN szParamName, DWORD IN nParamValueBufferLength, LPWSTR OUT szParamValue) = 0;

	// записать параметр szParamName со значением szParamValue
	// возвращает true в случае успеха
	virtual bool SetParam (LPCWSTR IN szParamName, LPCWSTR IN szParamValue) = 0;

	// очищает ¬—≈ параметры (поддерево параметров), имеющих начало szParamValue
	virtual bool ClearParams (LPCWSTR IN szParamName) = 0;

    // очищает ¬—ё persistent storage (на деинсталл€ции)
    virtual bool Cleanup() = 0;
};

typedef CCKAHStorage *LPCKAHSTORAGE;

//////////////////////////////////////////////////////////////////////////

// экспортируема€ реализаци€ интерфейса CCKAHStorage через реестр
class CKAHUMEXPORT CCKAHRegStorageImpl : public CCKAHStorage
{
public:
	// конструктор
	// hKey, szSubKey определ€ют корень ветки реестра, в которой будет производитьс€
	// работа с параметрами
	CCKAHRegStorageImpl (HKEY hKey, LPCWSTR szSubKey);

	virtual ~CCKAHRegStorageImpl ();

    virtual void SetPath (HKEY hKey, LPCWSTR szSubKey);

	virtual DWORD GetParam (LPCWSTR IN szParamName, DWORD IN nParamValueBufferLength, LPWSTR OUT szParamValue);
	virtual bool SetParam (LPCWSTR IN szParamName, LPCWSTR IN szParamValue);
	virtual bool ClearParams (LPCWSTR IN szParamName);
    virtual bool Cleanup ();

protected:
	HKEY m_hKey;
	LPCWSTR m_szSubKey;
};

#endif // !defined(AFX_CKAHSTORAGE_H__A5A05CC4_95EB_430E_80A3_487E55CB561B__INCLUDED_)
