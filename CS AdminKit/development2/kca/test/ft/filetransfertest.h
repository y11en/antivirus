/*!
 * (C) 2004 "Kaspersky Lab"
 *
 * \file ft/test/FileTransferTest.h
 * \author
 * \date 2004
 * \brief 
 *
*/

#ifndef __KL_FT_FILE_TRANSFER_TEST_H
#define __KL_FT_FILE_TRANSFER_TEST_H

#include <build/klcsc.h>

#include "testmodule.h"

#include "../../ft/multicastsender.h"

class CUploadTest :
public KLSTD::KLBaseImpl<KLTST2::Test2>
{
public:
    IMPLEMENT_TEST2_INSTANTIATE(CUploadTest)
		
	void Initialize(KLTST2::TestData2* pTestData);
	KLSTD_NOTHROW void Deinitialize() throw();
	long Run();
	
private:
	KLTST2::TestData2 *m_pTestData;
};

class CFileTransferServer :
public KLSTD::KLBaseImpl<KLTST2::Test2>
{
public:
    IMPLEMENT_TEST2_INSTANTIATE(CFileTransferServer)
		
	void Initialize(KLTST2::TestData2* pTestData);
	KLSTD_NOTHROW void Deinitialize() throw();
	long Run();
	
private:
	KLTST2::TestData2 *m_pTestData;
};

class CFileTransferFolderSync :
public KLSTD::KLBaseImpl<KLTST2::Test2>
{
public:
    IMPLEMENT_TEST2_INSTANTIATE(CFileTransferFolderSync)
		
	void Initialize(KLTST2::TestData2* pTestData);
	KLSTD_NOTHROW void Deinitialize() throw();
	long Run();
	
private:
	KLTST2::TestData2 *m_pTestData;
};


class CFileTransferMulticastBase : public KLTST2::Test2, public KLFT::MulticastSender::FileProvider
{
};

class CFileTransferMulticast :
public KLSTD::KLBaseImpl<CFileTransferMulticastBase>
{
public:

	// KLFT::MulticastSender::FileProvider
	virtual int GetFileInfo( const std::wstring &fileId, 
			const std::wstring &fileName,
			unsigned long &fullSize,
			std::wstring &folderName );

	virtual int GetFileChunk( const std::wstring &fileId, 
			const std::wstring &fileName,
			unsigned long pos,
			void *buff, unsigned long buffSize, 
			bool &endOfFile );

	virtual int OnFileSent( const std::wstring &fileId,
			const std::wstring &fileName, 
			const unsigned long sentSize );
	
	/////////////////////////////////////////////

    IMPLEMENT_TEST2_INSTANTIATE(CFileTransferMulticast)
		
	void Initialize(KLTST2::TestData2* pTestData);
	KLSTD_NOTHROW void Deinitialize() throw();
	long Run();
	
private:
	KLTST2::TestData2 *m_pTestData;

	KLSTD::CAutoPtr<KLSTD::Semaphore>	m_pStopSem;

	KLSTD::CAutoPtr<KLSTD::File>		m_pFileSend;
	unsigned long						m_lFileSize;
};


class CFileTransferBridge :
public KLSTD::KLBaseImpl<KLTST2::Test2>
{
public:

    IMPLEMENT_TEST2_INSTANTIATE(CFileTransferBridge)
		
	void Initialize(KLTST2::TestData2* pTestData);
	KLSTD_NOTHROW void Deinitialize() throw();
	long Run();
	
private:
	KLTST2::TestData2 *m_pTestData;
};


#endif // __KL_FT_FILE_TRANSFER_TEST_H


