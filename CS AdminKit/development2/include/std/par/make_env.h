#ifndef __KLPAR_MAKE_ENV_H__
#define __KLPAR_MAKE_ENV_H__

namespace KLPAR
{
	const wchar_t c_szwEnvName_Empty[]=L"empty-c2UGO0JKp7ahjqgpOmzvT1";

	/*!
	  \brief	ѕреобразование блока окружени€ в контейнер Params

	  \param	pEnvironment
	  \param	ppEnv
	*/
#if defined(UNICODE) && defined(_WIN32)
	KLCSC_DECL void SplitEnvironmentW_(const void* pEnvironment, KLPAR::Params** ppEnv);

	KLSTD_INLINEONLY void SplitEnvironment(const void* pEnvironment, KLPAR::Params** ppEnv)
	{
		SplitEnvironmentW_(pEnvironment, ppEnv);
	};
#else
	KLCSC_DECL void SplitEnvironmentA_(const void* pEnvironment, KLPAR::Params** ppEnv);

	KLSTD_INLINEONLY void SplitEnvironment(const void* pEnvironment, KLPAR::Params** ppEnv)
	{
		SplitEnvironmentA_(pEnvironment, ppEnv);
	};
#endif
	/*!
	  \brief	ѕреобразование контейнера Params а блок окружени€

	  \param	pEnvironment
	  \param	ppChunk
	*/
#if defined(UNICODE) && defined(_WIN32)
	KLCSC_DECL void MakeEnvironmentW_(KLPAR::Params* pEnvironment, KLSTD::MemoryChunk** ppChunk);
	KLSTD_INLINEONLY void MakeEnvironment(KLPAR::Params* pEnvironment, KLSTD::MemoryChunk** ppChunk)
	{
		MakeEnvironmentW_(pEnvironment, ppChunk);
	};
#else
	KLCSC_DECL void MakeEnvironmentA_(KLPAR::Params* pEnvironment, KLSTD::MemoryChunk** ppChunk);	
	KLSTD_INLINEONLY void MakeEnvironment(KLPAR::Params* pEnvironment, KLSTD::MemoryChunk** ppChunk)
	{
		MakeEnvironmentA_(pEnvironment, ppChunk);
	};
#endif
};
#endif //__KLPAR_MAKE_ENV_H__
