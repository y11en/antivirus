#ifndef __BLOB_H
#define __BLOB_H

#include "defs.h"
#include <string>

namespace KLUTIL
{

//! Обертка вокруг указателя на данные с размером.
class KLUTIL_API CBlockBase
{
private:
	char*  m_pData;
	size_t m_nDataSize;
	friend class CBlock;
public:
	CBlockBase(const CBlockBase& t) throw();
	//! "Оборачивает" эти данные.
	CBlockBase(const void* pData, size_t nDataSize) throw();
	//! Переводит бинарные данные в HEX представление (число цифр чётное).
	std::string	GetHex() const;
	//! Возвращает данные как строку.
	std::string   ToString() const;
	//! Возвращает указатель на начало данных.
	const void*	GetData() const throw() { return m_pData; }
	//! Возвращает размер данных.
	size_t	GetDataSize() const throw() { return m_nDataSize; }
};

//! Бинарный блок (является владельцем данных).
class KLUTIL_API CBlock: public CBlockBase
{
public:
	CBlock() throw();
	CBlock(const CBlockBase& blob);
	CBlock(const CBlock& blob);
	//! Копирует эти данные.
	CBlock(const void* pData, size_t nDataSize);
	//! Резервирует nReserveSize байтов, размер данных равен 0.
	CBlock(size_t nReserveSize);
	//! Переводит из Hex в бинарные данные.
	//!   \throw runtime_error в случае неверного HEX формата.
	CBlock(const std::string& szHexData);
	~CBlock();
	//! Обнуляет размер данных.
	void	Clear() throw();
	//! Обнуляет размер и данные.
	void	ClearSecure() throw();
	//! Деляет доступным не менее nSize байтов.
	void	MakeAvailable(size_t nSize);
	//! Резервирует после конца данных не менее nReserveSize байтов.
	void	MakeReserve(size_t nReserveSize);
	//! Устанавливает размер блока данных.
	void	SetSize(size_t nSize, char fill = 0);
	//! Возвращает указатель на начало данных.
	void*	GetData() throw();
	//! Возвращает указатель на начало данных.
	const void*	GetData() const throw() { return CBlockBase::GetData(); }
	//! Возвращает указатель на первый зарезервированный байт.
	void*	GetDataEnd() throw();
	//! Возвращает доступный размер после данных (применимый в AddDataSize).
	size_t	GetReserveSize() const throw();
	//! Возвращает доступный размер.
	size_t	GetAllocatedSize() const throw();
	void	AppendData(size_t nDataSize, const void* pData);
	//! Добавляет к размеру данных nAddSize.
	void	AddDataSize(int nAddSize) throw();
	//! Отнимает у rBlob его данные (rBlob.m_pData = NULL).
	void	operator <<(CBlock& rBlob) throw();
	//! Присваивает указатель на данные.
	void	AttachData(void* pData, size_t nDataSize) throw();
	//! Отнимает у this его данные (this.m_pData = NULL), нужен delete[] x;
	void*	DetachData() throw();
private:
	size_t m_nAllocatedSize;
};

//! Безопасный бинарный блок (очищает данные при разрушении).
class KLUTIL_API CSecBlock: public CBlock
{
public:
	CSecBlock() throw();
	CSecBlock(const CBlockBase& blob);
	CSecBlock(const void* pData, size_t nDataSize);
	CSecBlock(size_t nReserveSize);
	//! Переводит из Hex в бинарные данные.
	//!   \throw runtime_error в случае неверного HEX формата.
	CSecBlock(const std::string& szHexData);
	~CSecBlock();
};

} // namespace KLUTIL

#endif