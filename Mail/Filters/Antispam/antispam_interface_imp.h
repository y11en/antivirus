#ifndef _ANTISPAM_INTERFACE_IMP_H_
#define _ANTISPAM_INTERFACE_IMP_H_

#include "htmlfilter.h"
#include "charsetdecoder.h"
#include "encodingtables.h"
#include "textfilter.h"
#ifndef USE_FISHER
	#include "bayesfilter.h"
#else
	#include "bfilter.h"
#endif//USE_FISHER
#include "toupper.h"

#include <pr_loadr.h>
#include <pr_remote.h>
#include <iface/i_io.h>
#include <iface/i_os.h>
#include <iface/i_mailmsg.h>
#include <plugin/p_msoe.h>
#include <pr_oid.h>

#define PRAGUE_DEFINED
#include "antispam_interface.h"

namespace CAntispamCaller
{
	class CIOObjectToCheck : public CObjectToCheck
	{
	public:
		CIOObjectToCheck(hIO io): m_io(io)
		{
		};
		virtual ~CIOObjectToCheck()
		{
		};
		tERROR PropertyGet( 
			OUT tDWORD* out_size, 
			IN tPROPID prop_id, 
			IN tPTR buffer, 
			IN tDWORD size 
			)
		{
			return m_io->propGet(out_size, prop_id, buffer, size);
		};
		tERROR SeekRead( 
			OUT tDWORD* out_size, 
			IN tQWORD offset, 
			IN tPTR buffer, 
			IN tDWORD size 
			)
		{
			return m_io->SeekRead(out_size, offset, buffer, size);
		};
		tERROR GetSize( OUT tQWORD* out_size )
		{
			return m_io->GetSize(out_size, IO_SIZE_TYPE_EXPLICIT);
		};
		hIO GetIO()
		{
			return m_io;
		};

	private:
		hIO m_io;
	};
};

hROOT g_root = NULL;
PRRemoteAPI g_RP_API = {0};
HMODULE g_hModuleRemotePrague = NULL;

tERROR PragueInit(HINSTANCE hModule)
{
	tCHAR szLoadPath[MAX_PATH] = PR_REMOTE_MODULE_NAME;
	tERROR err = errOK;

	g_hModuleRemotePrague = LoadLibraryEx(szLoadPath, NULL, LOAD_WITH_ALTERED_SEARCH_PATH);
	
	if (!g_hModuleRemotePrague)
		err = errMODULE_CANNOT_BE_LOADED;
	else
	{
		tERROR ( *hGetAPI )(PRRemoteAPI *api) = NULL;

		*(void**)&hGetAPI = GetProcAddress(g_hModuleRemotePrague, "PRGetAPI");
		if( hGetAPI )
			err = hGetAPI(&g_RP_API);

		if( PR_SUCC(err) )
			err = g_RP_API.Initialize(PR_LOADER_FLAG_FORCE_PLUGIN_SEARCH);

		if (PR_SUCC(err))
			err = g_RP_API.GetRoot(&g_root);
	}
	return err;
};

tERROR PragueTerm()
{
	if ( g_RP_API.Deinitialize )
		return g_RP_API.Deinitialize();
	else
		return errOK;
};


namespace CAntispamImplementer
{
	class CKLAntispamFilter : public CAntispamFilter
	{
	public:
		CKLAntispamFilter():
			m_pHtmlFilter( 0 ),
			m_pDecoder( 0 ),
			m_pTextFilter( 0 ), 
			m_pBayesFilter( 0 )
		{
		};
		~CKLAntispamFilter()
		{
			if (m_pBayesFilter) { delete m_pBayesFilter; m_pBayesFilter = 0; }
			if (m_pTextFilter) { delete m_pTextFilter;  m_pTextFilter = 0; }
			if (m_pDecoder) { delete m_pDecoder;     m_pDecoder = 0; }
			if (m_pHtmlFilter) { delete m_pHtmlFilter;  m_pHtmlFilter = 0; }
			PragueTerm();
		};

		tERROR Init(CAntispamCaller::CMemoryAlloc* pMemoryAllocator, InitParams_t* pInitParams)
		{
			tERROR err = PragueInit(NULL);
			if ( PR_SUCC(err) ) 
			{
				m_pHtmlFilter = new CHtmlFilter();
				m_pDecoder = new CCharsetDecoder();
				m_pTextFilter = new CTextFilter();
				m_pBayesFilter = new CBayesFilter();
				if (
					m_pHtmlFilter &&
					m_pDecoder && 
					m_pTextFilter && 
					m_pBayesFilter
					) 
				{
					// Инициализация данных
					if ( PR_SUCC(err = m_pBayesFilter->Init((hOBJECT)g_root, "o:\\bayestable.sfdb", 0)) )
					{
						setlocale(LC_ALL, "");
						m_pDecoder->AddCharset("KOI8-R",		koi8_cp1251);
						m_pDecoder->AddCharset("WINDOWS-1251",	cp1251_cp1251);
						m_pDecoder->AddCharset("IBM866",		dos866_cp1251);
						m_pDecoder->AddCharset("ISO-8859-5",		iso_cp1251);
						m_pDecoder->AddCharset("X-MAC-CYRILLIC",mac_cp1251);
					}
				}
			}
			return err;
		};
		
		tERROR Process( 
			IN  CAntispamCaller::CObjectToCheck* pObject, 
			IN  ProcessParams_t* pProcessParams, 
			OUT long double* pResult,
			IN  tDWORD dwTimeOut,
			OUT ProcessStatistics_t* pProcessStat
			)
		{
			return FilterBody(
				((CAntispamCaller::CIOObjectToCheck*)pObject)->GetIO(), 
				false, 
				pResult,
				false
				);
		};
		
		tERROR Train(
			IN CAntispamCaller::CObjectToCheck* pObject, 
			IN TrainParams_t* pTrainParams,
			IN  tDWORD dwTimeOut
			)
		{
			long double Result = 0;
			return FilterBody(
				((CAntispamCaller::CIOObjectToCheck*)pObject)->GetIO(),
				true, 
				&Result,
				pTrainParams->bIsSpam
				);
		};
	
	private:
		CHtmlFilter* m_pHtmlFilter;
		CCharsetDecoder* m_pDecoder;
		CTextFilter* m_pTextFilter;
		CBayesFilter* m_pBayesFilter;
		bool IsHTML( hIO hBody )
		{
			bool bResult = false;
			tDWORD size = 0;
			tERROR err = hBody->propGetStr(&size, pgMESSAGE_PART_CONTENT_TYPE, NULL, 0);
			if( PR_SUCC(err) )
			{
				tBYTE  *ptr = new tBYTE[size];
				err = hBody->propGetStr(NULL, pgMESSAGE_PART_CONTENT_TYPE, ptr, size);
				if ( PR_SUCC(err) && size ) 
				{
					ToUpper((char*)ptr);
					string szptr = (char*)ptr;
					if ( szptr == "TEXT/HTML" )
						bResult = true;
				}
				delete ptr;
			}
			return bResult;

		}

		void CopyBodyToText( hIO hBody, string& szBody )
		{
			tDWORD dwRead = 0;
			tQWORD pos = 0;
			char c = 0;
			cERROR err = errOK;
			do 
			{
				if ( PR_SUCC(err= hBody->SeekRead(&dwRead, pos, &c, sizeof(char))) && dwRead )
				{
					pos++;
					szBody += c;
				}
			} 
			while( PR_SUCC(err) && dwRead  );

		}

		#define PUSH_SYS_TOKEN(_target_, _results_, _name_, _szname_)	\
		{	\
			char sz##_name_[]	= _szname_;	\
			unsigned long ul = 0;	\
			for ( ul = 0; ul < _results_.ul##_name_; ul++ )	\
				_target_.tokens.push_back(sz##_name_);	\
		}

		void UpdateSysTokens(CHtmlFilter::results_t& results_html, CTextFilter::results_t& results_text)
		{
			PUSH_SYS_TOKEN(results_html, results_html, UnknownDefsCount, ">> UnknownDefsCount >>");
			PUSH_SYS_TOKEN(results_html, results_html, SeemsColors, ">> SeemsColors >>");
			PUSH_SYS_TOKEN(results_html, results_html, SmallText, ">> SmallText >>");
			PUSH_SYS_TOKEN(results_html, results_html, ExternalObjects, ">> ExternalObjects >>");
			PUSH_SYS_TOKEN(results_html, results_html, Scripts, ">> Scripts >>");
			PUSH_SYS_TOKEN(results_html, results_html, HiddenElements, ">> HiddenElements >>");

			PUSH_SYS_TOKEN(results_html, results_text, TextTricks, ">> TextTricks >>");
			PUSH_SYS_TOKEN(results_html, results_text, RusLatTricks, ">> RusLatTricks >>");
//			PUSH_SYS_TOKEN(results_text, SMTPAddress, ">> SMTPAddress >>");
//			PUSH_SYS_TOKEN(results_text, UpperChars, ">> UpperChars >>");
//			PUSH_SYS_TOKEN(results_text, LowerChars, ">> LowerChars >>");
		}
		tERROR GetStringData(hOBJECT hMessage, tPROPID prop_id, string& szData)
		{
			tDWORD size = 0;
			tERROR err = errNOT_OK;
			if ( hMessage )
			{
				err = hMessage->propGetStr(&size, prop_id, NULL, 0);
				if( PR_SUCC(err) )
				{
					tBYTE  *ptr = new tBYTE[size];
					do 
					{
						err = hMessage->propGetStr(NULL, prop_id, ptr, size);
						if ( PR_SUCC(err) && size ) 
							szData = (char*)ptr;
						if ( err == errBUFFER_TOO_SMALL )
						{
							delete ptr;
							size *= 2;
							ptr = new tBYTE[size];
						}
					} 
					while( err == errBUFFER_TOO_SMALL );
					delete ptr;
				}
			}
			return err;
		}
		tERROR FilterBody( hIO hBody, bool bTrain, long double* pS, bool bIsSpam )
		{
			cERROR err = errOK;
			CHtmlFilter::results_t results_html;
			CTextFilter::results_t results_text;
			string szDecodedText = "";
			results_html.clear();
			results_text.clear();

			string szSubject = "";
			GetStringData(hBody->get_pgOBJECT_BASED_ON(), pgMESSAGE_SUBJECT, szSubject);
			string szFrom ="";	GetStringData(hBody->get_pgOBJECT_BASED_ON(), pgMESSAGE_FROM, szFrom);
			string szTo ="";	GetStringData(hBody->get_pgOBJECT_BASED_ON(), pgMESSAGE_TO, szTo);
			string szCC ="";	GetStringData(hBody->get_pgOBJECT_BASED_ON(), pgMESSAGE_CC, szCC);
			string szAddresses = szFrom + "; " + szTo + "; " + szCC;
			
			{
				if ( IsHTML(hBody) )
				{
					m_pHtmlFilter->Process(hBody, results_html);
					m_pDecoder->Decode(
						results_html.szVisibleText.c_str(),
						szDecodedText,
						results_html.szCharset.c_str()
						);
#ifdef _DEBUG
//					std::cout << endl;
//					std::cout << "===================< data >=====================" << endl;
//					{
//						tERROR err = errOK;
//						tQWORD pos_in_io = 0;
//						tDWORD dwRead = 0;
//						char buf[33];
//						tDWORD size = 33;
//						memset(buf, 0, size);
//						while ( PR_SUCC(err= hBody->SeekRead(&dwRead, pos_in_io, buf, size-1) ) && dwRead )
//						{
//							std::cout << buf;
//							pos_in_io += dwRead;
//						};
//					}
//
//					std::cout << endl;
//					std::cout << "================< end of data >=================" << endl;
	//					std::cout << "================< visible text >================" << endl;
	//					std::cout << szDecodedText;
	//					std::cout << endl;
	//					std::cout << "=============< end of visible text >============" << endl;
	//					std::cout << endl;
#endif // _DEBUG
				}
				else
					CopyBodyToText(hBody, szDecodedText);

				szDecodedText += "\r\n" + szSubject + "\r\n" + szAddresses;

				m_pTextFilter->Process(
					(char*)(szDecodedText.c_str()), 
					results_text
					);

//				m_pBayesFilter->DeleteTwice(results_text.tokens);
//				
//				UpdateSysTokens(results_html, results_text);
//				{
//					for (CHtmlFilter::tokens_t::const_iterator 
//						iTerator = results_html.tokens.end();
//						iTerator != results_html.tokens.begin();
//						)
//						{
//							results_text.tokens.insert(
//								results_text.tokens.begin(),
//								CTextFilter::token_t(
//									(char*)(--iTerator)->c_str()
//									)
//								);
//						}
//				}




//#ifdef _DEBUG
//				cout << "==================================================================\n";
//				for (CTextFilter::tokens_t::const_iterator 
//					iTerator = results_text.tokens.begin();
//					iTerator != results_text.tokens.end();
//					++iTerator
//					)
//					{
//						//if ( iTerator->token1[0] == '>' )
//						{
//							cout << iTerator->token1;
//							cout << "\n";
//						}
//					}
//#endif // _DEBUG

				if ( bTrain ) 
					err = m_pBayesFilter->Update(results_text.tokens, bIsSpam);
				else
					if (pS) *pS = m_pBayesFilter->Process(results_text.tokens);
			}
			return err;
		}
	};



	CAntispamFilter::CAntispamFilter(){};
	CAntispamFilter::~CAntispamFilter(){};
	tERROR CAntispamFilter::Init(
		CAntispamCaller::CMemoryAlloc* pMemoryAllocator, 
		InitParams_t* pInitParams
		)
	{return errNOT_IMPLEMENTED;};
	tERROR CAntispamFilter::Process( 
		IN  CAntispamCaller::CObjectToCheck* pObject, 
		IN  ProcessParams_t* pProcessParams, 
		OUT long double* pResult,
		IN  tDWORD dwTimeOut,
		OUT ProcessStatistics_t* pProcessStat
		)
	{return errNOT_IMPLEMENTED;};
	tERROR CAntispamFilter::Train(
		IN CAntispamCaller::CObjectToCheck* pObject, 
		IN TrainParams_t* pTrainParams,
		IN  tDWORD dwTimeOut
		)
	{return errNOT_IMPLEMENTED;};


}

#endif//_ANTISPAM_INTERFACE_IMP_H_