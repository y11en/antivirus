#include "antispam_interface_imp2.h"

#include <Prague/prague.h>

#include <AntiPhishing/plugin/p_antiphishingtask.h>
#include <AntiPhishing/structs/s_antiphishing.h>

#include <Extract/plugin/p_mailmsg.h>

#include <Mail/common/GetAddressFromString.h>
#include <Mail/iface/i_antispamfilter.h>
#include <Mail/structs/s_spamtest.h>

#include "BWFind.h"

extern tPROPID g_propMessageIsIncoming;
extern tPROPID g_propMessageCheckOnly;
int ConvertOutlookRtf2Html(hIO in, hIO out);

#define CHECK_SPAMHAM_LETTERS(err)	\
CSFDBRecordData data;	\
{	\
	if ( m_pBayesFilter->GetData(">> SPAMHAM_LETTERS >>", data, NULL) )	\
	{	\
		if ( pProcessStat )	\
		{	\
		pProcessStat->m_dwSpamBaseSize = data.ulSpamCount;	\
		pProcessStat->m_dwHamBaseSize = data.ulHamCount;	\
		pProcessStat->m_bNeedMoreSpamToTrain =	\
			(data.ulSpamCount < m_settings.pAntispamSettings->m_dMinSpamLettersCount) ? m_settings.pAntispamSettings->m_dMinSpamLettersCount - data.ulSpamCount : 0;	\
		pProcessStat->m_bNeedMoreHamToTrain  =	\
			(data.ulHamCount  < m_settings.pAntispamSettings->m_dMinHamLettersCount)  ? m_settings.pAntispamSettings->m_dMinHamLettersCount  - data.ulHamCount  : 0;	\
		}	\
	}	\
	if (	\
		(data.ulSpamCount < m_settings.pAntispamSettings->m_dMinSpamLettersCount) ||	\
		(data.ulHamCount  < m_settings.pAntispamSettings->m_dMinHamLettersCount)	\
		)	\
		err = errOPERATION_CANCELED;	\
}


struct obj_t
{
	CAntispamCaller::CObjectToCheck_hObject* obj;
	CAntispamCaller::CObjectToCheck_hIO* objIO;
};

tERROR CAntispamImplementer::CKLAntispamFilter::GetStatistics( IN Statistics_t* p_Statistics )
{
	*p_Statistics = m_statistics;
	return errOK;
}

tERROR CAntispamImplementer::CKLAntispamFilter::FindAndFilterBody(
	IN  CAntispamCaller::CObjectSystemToCheck* pObject, 
	IN  ProcessParams_t* pProcessParams, 
	OUT long double* pResult,
	IN  tDWORD dwTimeOut,
	IN OUT ProcessStatistics_t* pProcessStat,
	IN TrainParams_t* pTrainParams
	)
{
	//Выбрать все тела письма и проверить одно из них (в порядке приоритета): RTFHTML, HTML, PlainText

	CAntispamCaller::CObjectToCheck_hOS* objOS2Check = (CAntispamCaller::CObjectToCheck_hOS*)pObject;
	tUINT objPtr = 0;
	tERROR err = errOK;
	typedef cVector<obj_t> ios_t;
	ios_t ios;
	err = pObject->PtrCreate(&objPtr);
	while ( PR_SUCC(err) && objPtr )
	{
		CAntispamCaller::CObjectToCheck_hObject* obj = NULL;
		err = pObject->ObjectCreate(objPtr, (CAntispamCaller::CObjectToCheck**)&obj);
		if ( PR_SUCC(err) ) 
		{
			CAntispamCaller::CObjectToCheck_hIO* objIO = new CAntispamCaller::CObjectToCheck_hIO(obj);
			if ( objIO ) 
			{
				hIO io = objIO->GetIO();
				if ( io && io->get_pgOBJECT_ORIGIN() == OID_MAIL_MSG_BODY ) // Фильтруем только тела письма
				{
					obj_t obj1 = {obj, objIO};
					ios.push_back(obj1);
				}
				else
				{
					delete objIO;
					pObject->ObjectClose(objPtr, obj);
				}
			}
		}
	}

	// Тела собрали - теперь расставляем их в порядке приоритета и проверяем одно тело с высшим приоритетом
	// Просто ищем тело с HTML. Если нашли - проверяем
	tUINT iCounter = 0;
	CAntispamCaller::CObjectToCheck_hIO* objIO2Check = NULL;
	if ( !objIO2Check )
	{
		for ( iCounter = 0; iCounter < ios.count(); iCounter++ )
		{
			CAntispamCaller::CObjectToCheck_hIO* objIO = ios.at(iCounter).objIO;
			if ( IsRTFHTML(objIO->GetIO()) ) 
			{
				objIO2Check = objIO;
				break;
			}
		}
	}

	if ( !objIO2Check )
	{
		// Если нет тела с RTFHTML проверяем HTML
		for ( iCounter = 0; iCounter < ios.count(); iCounter++ )
		{
			CAntispamCaller::CObjectToCheck_hIO* objIO = ios.at(iCounter).objIO;
			if ( IsHTML(objIO->GetIO()) ) 
			{
				objIO2Check = objIO;
				break;
			}
		}
	}
	
	if ( !objIO2Check )
	{
		// Если нет тела с HTML или RTFHTML проверяем PlainText
		for ( iCounter = 0; iCounter < ios.count(); iCounter++ )
		{
			CAntispamCaller::CObjectToCheck_hIO* objIO = ios.at(iCounter).objIO;
			if ( !IsHTML(objIO->GetIO()) && !IsRTFHTML(objIO->GetIO()) && !IsRTF(objIO->GetIO()) ) 
			{
				objIO2Check = objIO;
				break;
			}
		}
	}


	if ( objIO2Check || pProcessStat || objOS2Check )
	{
		// В случае процессирования на "белом" адресате - обучаемся как на Хаме => [(20 май 2005) только, если база антиспама недостаточна]
		// В случае тренировки на Хаме - запоминаем всех адресатов в белом листе
		// В случае тренировки на Спаме - удаляем адресатов из белого листа
		{
			hOS pOS = objOS2Check ? objOS2Check->GetOS() : NULL;
			hIO pIO = objIO2Check ? objIO2Check->GetIO() : NULL;
			cStringObj szAddress;
			cStringObj szSMTPAddress;
			
			cStringObj szUserName = pProcessStat && !pProcessStat->m_szUserName.empty() ? 
				pProcessStat->m_szUserName : "Default"; 
			{
				std::string sz;
				if ( pOS )
					err = GetStringData( (hOBJECT)pOS, pgMESSAGE_FROM, sz );
				else
					sz = pProcessStat->m_szLastMessageSender.c_str(cCP_ANSI);
				if ( PR_SUCC(err) && !sz.empty() )
				{
					GetAddressFromString(sz, sz);
					szAddress = sz.c_str();
				}

				std::string szTemp;
				if ( pOS )
					err = GetStringData( (hOBJECT)pOS, pgMESSAGE_FROM_SMTP, szTemp);
				if ( PR_SUCC(err) && !szTemp.empty())
					szSMTPAddress = szTemp.c_str();

			}

			if ( !pTrainParams )
			{
				// Проверяем по чернобелым спискам
				cStringObj szFoundAddr;
				tBOOL bNeedToMoreProcess = cTRUE;
				address_type_t address_type_found = at_not_found;
				{
					cAsBwListRule rule(szAddress.c_str(cCP_ANSI), cTRUE, cTRUE);
					cAsBwListRule ruleSMTP(szSMTPAddress.c_str(cCP_ANSI), cTRUE, cTRUE);
					cAsBwListRule mailDispatcher("Kaspersky MailDispatcher", cTRUE, cTRUE);
					if ( rule == mailDispatcher || ruleSMTP == mailDispatcher)
					{
						pProcessStat->m_dLastResult = 1;
						pProcessStat->m_dwLastReason = asdr_MailDispatcher;
						*pResult = 1; // Мы уверены, что это - Спам от МайлВошера
						bNeedToMoreProcess = cFALSE;
					}
				}
				if (
					bNeedToMoreProcess &&
					!szAddress.empty() &&
					(m_settings.pAntispamSettings->m_bUseWhiteAddresses ||
					m_settings.pAntispamSettings->m_bUseBlackAddresses)
					)
				{
					m_pAddressList->Find( szAddress.c_str(cCP_ANSI), szUserName.c_str(cCP_ANSI), address_type_found, 
						m_settings.pAntispamSettings->m_bUseWhiteAddresses, m_settings.pAntispamSettings->m_bUseBlackAddresses,
						&szFoundAddr
						);
					if (at_not_found == address_type_found && !szSMTPAddress.empty())
					{
						m_pAddressList->Find( szSMTPAddress.c_str(cCP_ANSI), szUserName.c_str(cCP_ANSI), address_type_found, 
							m_settings.pAntispamSettings->m_bUseWhiteAddresses, m_settings.pAntispamSettings->m_bUseBlackAddresses,
							&szFoundAddr
							);
					}
				}
				if ( bNeedToMoreProcess && pProcessStat && pResult )
				{
					if ( bNeedToMoreProcess && m_settings.pAntispamSettings->m_bUseWhiteAddresses )
					{
						if ( address_type_found == at_white )
						{
							pProcessStat->m_dLastResult = 0;
							pProcessStat->m_dwLastReason = asdr_WhiteAddress;
							pProcessStat->AddVerdict( asdr_WhiteAddress, szFoundAddr, 100);
							*pResult = 0; // Мы уверены, что это - Хам
							bNeedToMoreProcess = cFALSE;
						}
						else if ( !m_settings.pAntispamSettings->m_bUseBlackAddresses )
						{
							pProcessStat->m_dLastResult = 0;
							pProcessStat->m_dwLastReason = asdr_WhiteAddress_NOTFOUND;
							*pResult = 0; // Теоретически, это Спам - это мы выясним в конце всех проверок
						}
					}
					if ( bNeedToMoreProcess && m_settings.pAntispamSettings->m_bUseBlackAddresses )
					{
						if ( address_type_found == at_black )
						{
							pProcessStat->m_dLastResult = 1;
							pProcessStat->m_dwLastReason = asdr_BlackAddress;
							pProcessStat->AddVerdict( asdr_BlackAddress, szFoundAddr, 100);
							*pResult = 1; // Мы уверены, что это - Спам
							bNeedToMoreProcess = cFALSE;
						}
						else if ( !m_settings.pAntispamSettings->m_bUseWhiteAddresses )
						{
							pProcessStat->m_dLastResult = 0;
							pProcessStat->m_dwLastReason = asdr_NOTHING; //asdr_BlackAddress_NOTFOUND;
							*pResult = 0; // Теоретически, это Хам
						}
					}
				}
				//if ( m_settings.pAntispamSettings->m_bUseBayesian )
				{
					// Проверяем, достаточно ли записей в антиспам-базе для того, 
					// чтобы можно было работать
					CHECK_SPAMHAM_LETTERS(err);
					if ( PR_FAIL(err) )
					{
						tDOUBLE dLastResult = 0;
						err = errOK;
						// Если записей недостаточно, то применяем автообучение по черно-белым спискам
						if (
							m_settings.pAntispamSettings->m_bUseWhiteAddresses && 
							address_type_found == at_white 
							)
						{
							FilterBody(
								pOS,
								pIO, 
								true, 
								pResult,
								cAntispamTrainParams::TrainAsHam,
								pProcessStat
								);
							dLastResult = pProcessStat->m_dLastResult;
							pProcessStat->m_dLastResult = 0;
							pProcessStat->m_dwLastReason = asdr_WhiteAddress;
							if ( dLastResult != -2 )
							{
								// Обновляем статистику задачи
								cERROR l_err = errOK; CHECK_SPAMHAM_LETTERS(l_err);
								m_statistics += *((Statistics_t*)pProcessStat);
							}
							*pResult = 0;
						}
						else
						if ( 
							m_settings.pAntispamSettings->m_bUseBlackAddresses && 
							address_type_found == at_black 
							)
						{
							FilterBody(
								pOS,
								pIO, 
								true, 
								pResult,
								cAntispamTrainParams::TrainAsSpam,
								pProcessStat
								);
							dLastResult = pProcessStat->m_dLastResult;
							pProcessStat->m_dLastResult = 1;
							pProcessStat->m_dwLastReason = asdr_BlackAddress;
							if ( dLastResult != -2 )
							{
								// Обновляем статистику задачи
								cERROR l_err = errOK; CHECK_SPAMHAM_LETTERS(l_err);
								m_statistics += *((Statistics_t*)pProcessStat);
							}
							*pResult = 1;
						}
					}
				}

				if (
					bNeedToMoreProcess &&
					(
						m_settings.pAntispamSettings->m_bUseBayesian ||
						m_settings.pAntispamSettings->m_bUseGSG ||
						m_settings.pAntispamSettings->m_bUsePDB ||
						m_settings.pAntispamSettings->m_bUseRecentTerms ||
						m_settings.pAntispamSettings->m_bUseAntifishing ||
						m_settings.pAntispamSettings->m_bUseWhiteStrings ||
						m_settings.pAntispamSettings->m_bUseBlackStrings ||
						m_settings.pAntispamSettings->m_bUsePerUserSettings
					))
				{
					// Если записей в базе достаточно - обрабатываем письмо
					err =  FilterBody(
						pOS,
						pIO, 
						false, 
						pResult,
						cAntispamTrainParams::TrainAsSpam, // Этот параметр не будет использован при процессировании
						pProcessStat
						);
					if ( PR_SUCC(err) )
						m_statistics += *((Statistics_t*)pProcessStat);
				}
				else if ( 
					pResult && 
					pProcessStat && 
					pProcessStat->m_dwLastReason == asdr_WhiteAddress_NOTFOUND 
					) 
				{
					// Не прошедших белый список причисляем к спаму
					*pResult = 1;
					pProcessStat->m_dLastResult = 1;
					pProcessStat->AddVerdict( asdr_WhiteAddress, "", 0);
				}
					
			}
			else if ( pTrainParams ) 
			{
				if ( 
					/*(!szAddress.empty() || !szSMTPAddress.empty()) &&*/
					(m_settings.pAntispamSettings->m_bUseWhiteAddresses ||
					m_settings.pAntispamSettings->m_bUseBlackAddresses)
					) 
				{
					switch ( pTrainParams->Action ) 
					{
					case cAntispamTrainParams::TrainAsHam:
						
						if (!szAddress.empty())
							m_pAddressList->Update(szAddress.c_str(cCP_ANSI), szUserName.c_str(cCP_ANSI), at_white);
						if (!szSMTPAddress.empty())
							m_pAddressList->Update(szSMTPAddress.c_str(cCP_ANSI), szUserName.c_str(cCP_ANSI), at_white);

						if ( pOS && !pOS->propGetBool(g_propMessageIsIncoming) )
						{
							// Добавляем всех адресатов исходящего письма в "белый список"
							std::string s;
							std::string to, cc;
							if ( PR_SUCC(GetStringData( (hOBJECT)pOS, pgMESSAGE_TO, to )) )
								s = to;
							if ( PR_SUCC(GetStringData( (hOBJECT)pOS, pgMESSAGE_CC, cc )) )
							{
								if ( !s.empty() ) s += "; ";
								s += cc;
							}

							PutAddressesInList(s, szUserName, at_white, m_pAddressList);

						}
						if (pProcessStat && m_pAddressList->HasBeenChanged()) pProcessStat->m_dwLastReason = asdr_AddedToWhiteRecipient;
						break;
					case cAntispamTrainParams::RemoveFromSpam:
						m_pAddressList->Delete(szAddress.c_str(cCP_ANSI), szUserName.c_str(cCP_ANSI), at_black);
						m_pAddressList->Delete(szSMTPAddress.c_str(cCP_ANSI), szUserName.c_str(cCP_ANSI), at_black);
						break;
					// Не стоит автоматически вносить адреса в черный список
					//	case cAntispamTrainParams::TrainAsSpam:
					//		m_pAddressList->Update(szAddress.c_str(cCP_ANSI), szUserName.c_str(cCP_ANSI), at_black);
					//		break;
					case cAntispamTrainParams::TrainAsSpam:
					case cAntispamTrainParams::RemoveFromHam:
						m_pAddressList->Delete(szAddress.c_str(cCP_ANSI), szUserName.c_str(cCP_ANSI), at_white);
						m_pAddressList->Delete(szSMTPAddress.c_str(cCP_ANSI), szUserName.c_str(cCP_ANSI), at_white);
						break;
					}
				}
				switch ( pTrainParams->Action ) 
				{
				case cAntispamTrainParams::TrainAsHam:
				case cAntispamTrainParams::RemoveFromSpam:
					err =  FilterBody(
						pOS,
						pIO, 
						true, 
						pResult,
						pTrainParams->Action,
						pProcessStat
						);
					{
						cERROR err = errOK;
						CHECK_SPAMHAM_LETTERS(err);
						m_statistics += *((Statistics_t*)pProcessStat);
					}
					break;
				case cAntispamTrainParams::TrainAsSpam:
				case cAntispamTrainParams::RemoveFromHam:
					err =  FilterBody(
						pOS,
						pIO, 
						true, 
						pResult,
						pTrainParams->Action,
						pProcessStat
						);
					{
						cERROR err = errOK;
						CHECK_SPAMHAM_LETTERS(err);
						m_statistics += *((Statistics_t*)pProcessStat);
					}
					break;
				}
			}
		}
	}
	else if ( err == errEND_OF_THE_LIST )
		err = errOK; // Не считаем достижение конца списка ошибкой

	// Освобождаем все выделенные объекты
	for ( iCounter = 0; iCounter < ios.count(); iCounter++ )
	{
		pObject->ObjectClose(objPtr, ios.at(iCounter).obj);
		CAntispamCaller::CObjectToCheck_hIO* objIO = ios.at(iCounter).objIO;
		delete objIO;
		objIO = NULL;
	}
	pObject->PtrClose(objPtr);

	return err;
}

void CAntispamImplementer::CKLAntispamFilter::PutAddressesInList(const std::string& addresses, const cStringObj& userName, address_type_t type, CAddressList* pList)
{
	if (!pList || addresses.empty())
		return;

	static const std::string sep("\"\t;, <>\n\r");
	size_t lastpos = 0;
	size_t pos = 0;

	while (true)
	{
		pos = addresses.find(';', pos);
		std::string address = addresses.substr(lastpos, pos - lastpos);
		
		GetAddressFromString(address, address);

		// удаляем мусор слева и справа от адреса
		size_t posLeft = address.find_first_not_of(sep);
		size_t posRight = address.find_last_not_of(sep);

		if (std::string::npos == posLeft)
		{
			// нашли только мусор
			address.clear();
		}
		else
		{
			// вырезаем нормальный кусок
			address = address.substr(posLeft, posRight - posLeft + 1);
		}
		
		cStringObj addr = address.c_str();
		if (!addr.empty())
			pList->Update(addr.c_str(cCP_ANSI), userName.c_str(cCP_ANSI), type);		

		if (std::string::npos == pos)
			break;
		
		pos += 1;
		lastpos = pos;
	}


}

tERROR CAntispamImplementer::CKLAntispamFilter::Process( 
	IN  CAntispamCaller::CObjectSystemToCheck* pObject, 
	IN  ProcessParams_t* pProcessParams, 
	OUT long double* pResult,
	IN  tDWORD dwTimeOut,
	OUT ProcessStatistics_t* pProcessStat
	)
{
	tERROR err = errNOT_OK;
	try
	{
		err = FindAndFilterBody(pObject, pProcessParams, pResult, dwTimeOut, pProcessStat, NULL);
	}
	catch(std::exception &e)
	{
		PR_TRACE((m_settings.hAntispamTask, prtERROR, "Exception '%s' caught in CKLAntispamFilter::Process", e.what()));
		PR_ASSERT(0);
	}
	return err;
};
		
tERROR CAntispamImplementer::CKLAntispamFilter::Train(
	IN CAntispamCaller::CObjectSystemToCheck* pObject, 
	IN TrainParams_t* pTrainParams,
	IN  tDWORD dwTimeOut,
	IN OUT ProcessStatistics_t* pProcessStat
	)
{
	if ( pTrainParams && pTrainParams->Action == cAntispamTrainParams::ClearDatabase )
	{
		cERROR err = errOK;
//		if ( m_pAddressList )
//			err = m_pAddressList->ClearDB();
		if ( PR_SUCC(err) && m_pBayesFilter )
			err = m_pBayesFilter->ClearDB();
		return err;
	}
	if ( pTrainParams && pTrainParams->Action == cAntispamTrainParams::MergeDatabase )
	{
		cERROR err = errOK;
		// Не нужно мерджить адресные листы, т.к.  они все равно шарены между экземплярами фильтров
		//	if ( m_pAddressList && pTrainParams->pFilter )
		//		err = m_pAddressList->Merge(pTrainParams->pFilter->GetAddressList());
		if ( PR_SUCC(err) && m_pBayesFilter && pTrainParams->pFilter )
		{
			err = m_pBayesFilter->Merge(pTrainParams->pFilter->GetBayesFilter());
			if ( PR_SUCC(err) )
			{
				Statistics_t statistics;
				pTrainParams->pFilter->GetStatistics(&statistics);
				m_statistics += statistics;
				CSFDBRecordData data;	
				if ( m_pBayesFilter->GetData(">> SPAMHAM_LETTERS >>", data, NULL) )	
				{	
					m_statistics.m_dwSpamBaseSize = data.ulSpamCount;
					m_statistics.m_dwHamBaseSize = data.ulHamCount;
					m_statistics.m_bNeedMoreSpamToTrain =
						(data.ulSpamCount < m_settings.pAntispamSettings->m_dMinSpamLettersCount) ? 
						m_settings.pAntispamSettings->m_dMinSpamLettersCount - data.ulSpamCount : 0;
					m_statistics.m_bNeedMoreHamToTrain  =
						(data.ulHamCount  < m_settings.pAntispamSettings->m_dMinHamLettersCount)  ? 
						m_settings.pAntispamSettings->m_dMinHamLettersCount  - data.ulHamCount  : 0;
				}	
			}
		}
		return err;
	}
	if ( pTrainParams && pTrainParams->Action == cAntispamTrainParams::Train_End_Apply )
		return m_pBayesFilter->Save();

	tERROR err = errNOT_OK;
	try
	{
		err = FindAndFilterBody(pObject, NULL, NULL, dwTimeOut, pProcessStat, pTrainParams);
	}
	catch(std::exception &e)
	{
		PR_TRACE((m_settings.hAntispamTask, prtERROR, "Exception '%s' caught in CKLAntispamFilter::Train", e.what()));
		PR_ASSERT(0);
	}

	return err;
};

CAntispamImplementer::CKLAntispamFilter::CKLAntispamFilter():
	m_pHtmlFilter( 0 ),
	m_pDecoder( 0 ),
	m_pTextFilter( 0 ), 
	m_pBayesFilter( 0 ),
	m_pAddressList( 0 )
{
};
CAntispamImplementer::CKLAntispamFilter::~CKLAntispamFilter()
{
	if (m_pAddressList)  { delete m_pAddressList;   m_pAddressList = 0; }
	if (m_pBayesFilter) { delete m_pBayesFilter; m_pBayesFilter = 0; }
	if (m_pTextFilter)  { delete m_pTextFilter;  m_pTextFilter = 0; }
	if (m_pDecoder)     { delete m_pDecoder;     m_pDecoder = 0; }
	if (m_pHtmlFilter)  { delete m_pHtmlFilter;  m_pHtmlFilter = 0; }
};

tERROR CAntispamImplementer::CKLAntispamFilter::Init(CAntispamCaller::CMemoryAlloc* pMemoryAllocator, InitParams_t* pInitParams)
{
	tERROR err = errOK;
	m_settings = *pInitParams;
	if ( PR_SUCC(err) ) 
	{
		err = errNOT_OK;
		m_pHtmlFilter = new CHtmlFilter();
		m_pDecoder = new CCharsetDecoder();
		m_pTextFilter = new CTextFilter();
		m_pBayesFilter = new CBayesFilter();
		m_pAddressList = new CAddressList();
		if (
			m_pHtmlFilter &&
			m_pDecoder && 
			m_pTextFilter && 
			m_pBayesFilter &&
			m_pAddressList
			) 
		{
			// Инициализация данных
#ifdef USE_FISHER
			if ( PR_SUCC(err = m_pBayesFilter->Init(
				(hOBJECT)g_root, 
				pInitParams->pAntispamSettings->m_szBayesTableFileName_Original.c_str(cCP_ANSI),
				pInitParams->pAntispamSettings->m_szBayesTableFileName.c_str(cCP_ANSI),
				((double)pInitParams->pAntispamSettings->m_nFisherX)/100,
				((double)pInitParams->pAntispamSettings->m_nFisherS)/100,
				((double)pInitParams->pAntispamSettings->m_nFisherMinDev)/1000
				)))
#else
			if ( PR_SUCC(err = m_pBayesFilter->Init(
				(hOBJECT)g_root, 
				pInitParams->pAntispamSettings->m_szBayesTableFileName_Original.c_str(cCP_ANSI),
				pInitParams->pAntispamSettings->m_szBayesTableFileName.c_str(cCP_ANSI)
				)))
#endif
			{
				if ( PR_SUCC(err = m_pAddressList->Init(
					pInitParams->pAntispamSettings,
					pInitParams->hAntispamTask
					))) 
				{
					setlocale(LC_ALL, "");
					setlocale(LC_NUMERIC, "English");
					m_pDecoder->AddCharset("KOI8-R",		koi8_cp1251);
					m_pDecoder->AddCharset("WINDOWS-1251",	cp1251_cp1251);
					m_pDecoder->AddCharset("IBM866",		dos866_cp1251);
					m_pDecoder->AddCharset("ISO-8859-5",	iso_cp1251);
					m_pDecoder->AddCharset("MACCYRILLIC",   mac_cp1251);
					m_pDecoder->AddCharset("X-MAC-CYRILLIC",mac_cp1251);
				}
				if ( PR_SUCC(err) )
				{
					cERROR l_err = errOK;
					Statistics_t* pProcessStat = &m_statistics;
					CHECK_SPAMHAM_LETTERS(l_err);
				}
			}
		}
	}
	return err;
};

bool CAntispamImplementer::CKLAntispamFilter::IsHTML( hIO hBody )
{
	cStringObj sz;
	if ( 
		PR_SUCC(sz.assign(hBody, pgOBJECT_NAME)) &&
		sz == MSG_CONTENT_HTML_BODY
		)
		return true;
	if ( 
		PR_SUCC(sz.assign(hBody, pgMESSAGE_PART_CONTENT_TYPE)) &&
		cSTRING_COMP_EQ == sz.compare(0, cStringObj::whole, "TEXT/HTML", fSTRING_COMPARE_CASE_INSENSITIVE)
		)
		return true;
	return false;
}

bool CAntispamImplementer::CKLAntispamFilter::IsRTF( hIO hBody )
{
	cStringObj sz;
	if ( 
		PR_SUCC(sz.assign(hBody, pgOBJECT_NAME)) &&
		sz == MSG_CONTENT_RICH_BODY
		)
		return true;
	return false;
}

bool CAntispamImplementer::CKLAntispamFilter::IsRTFHTML( hIO hBody )
{
	cStringObj sz;
	if ( 
		PR_SUCC(sz.assign(hBody, pgOBJECT_NAME)) &&
		sz == MSG_CONTENT_RICH_BODY
		)
	{
		string szBody;
		CopyBodyToText(hBody, szBody, 128);
		if ( -1 != szBody.find("\\fromhtml") )
			return true;
	}
	return false;
}

bool CAntispamImplementer::CKLAntispamFilter::IsXMacCyrillic( hOS hMessage )
{
	cStringObj sz;
	if ( hMessage && PR_SUCC(sz.assign(hMessage, pgMESSAGE_INTERNET_HEADERS)))
	{
		sz.toupper();
		if ( cStringObj::npos != sz.find("CHARSET=\"X-MAC-CYRILLIC\"") )
			return true;
	}
	return false;
}

void CAntispamImplementer::CKLAntispamFilter::CopyBodyToText( hIO hBody, string& szBody, tQWORD qwSize )
{
	tDWORD dwRead = 0;
	tQWORD pos = 0;
	char c[1024];
	cERROR err = errOK;
	do 
	{
		if(PR_SUCC(err = hBody->SeekRead(&dwRead, pos, c, 1024)) && dwRead)
		{
			pos += dwRead;
			szBody += string(c, dwRead);
		}
	} 
	while(PR_SUCC(err) && dwRead && (qwSize == -1 || pos < qwSize));

}

#define PUSH_SYS_TOKEN(_target_, _results_, _name_, _szname_)	\
{	\
	char sz##_name_[]	= _szname_;	\
	unsigned long ul = 0;	\
	for ( ul = 0; ul < _results_.ul##_name_; ul++ )	\
		_target_.tokens.push_back(sz##_name_);	\
}

void CAntispamImplementer::CKLAntispamFilter::UpdateSysTokens(CHtmlFilter::results_t& results_html, CTextFilter::results_t& results_text)
{
	PUSH_SYS_TOKEN(results_html, results_html, UnknownDefsCount, ">> UnknownDefsCount >>");
	PUSH_SYS_TOKEN(results_html, results_html, SeemsColors,      ">> SeemsColors >>");
	PUSH_SYS_TOKEN(results_html, results_html, SmallText,        ">> SmallText >>");
	PUSH_SYS_TOKEN(results_html, results_html, ExternalObjects,  ">> ExternalObjects >>");
	PUSH_SYS_TOKEN(results_html, results_html, Scripts,          ">> Scripts >>");
	PUSH_SYS_TOKEN(results_html, results_html, HiddenElements,   ">> HiddenElements >>");

	PUSH_SYS_TOKEN(results_html, results_text, TextTricks,       ">> TextTricks >>");
	PUSH_SYS_TOKEN(results_html, results_text, RusLatTricks,     ">> RusLatTricks >>");
//	PUSH_SYS_TOKEN(results_text, SMTPAddress, ">> SMTPAddress >>");
//	PUSH_SYS_TOKEN(results_text, UpperChars, ">> UpperChars >>");
//	PUSH_SYS_TOKEN(results_text, LowerChars, ">> LowerChars >>");
}
tERROR CAntispamImplementer::CKLAntispamFilter::GetStringData(hOBJECT hMessage, tPROPID prop_id, cStringObj& szData)
{
	return szData.assign(hMessage, prop_id);
}

tERROR CAntispamImplementer::CKLAntispamFilter::GetStringData(hOBJECT hMessage, tPROPID prop_id, string& szData)
{
	tDWORD size = 0;
	tERROR err = errNOT_OK;
	if ( hMessage )
	{
		err = hMessage->propGetStr(&size, prop_id, NULL, 0);
		if( PR_SUCC(err) )
		{
			tBYTE  *ptr = new tBYTE[size];
			if ( ptr )
			{
				do 
				{
					err = hMessage->propGetStr(NULL, prop_id, ptr, size);
					if ( PR_SUCC(err) && size ) 
						szData = (char*)ptr;
					if ( err == errBUFFER_TOO_SMALL )
					{
						delete []ptr;
						size *= 2;
						ptr = new tBYTE[size];
					}
				} 
				while( err == errBUFFER_TOO_SMALL );
				delete []ptr;
			}
		}
	}
	return err;
}

// ---
// Function by Victor Matushenkov
//
tBOOL RemoveScripts( hIO pIStreamSrc, hIO pIStreamDest ) 
{
#if defined(REMOVE_SCRIPTS_UNICODE)
#define	DATATYPE WCHAR
#define _TXT(quote) L##quote
#define snicmp	_wcsnicmp
#else
#define	DATATYPE CHAR
#define _TXT(quote) quote
#define snicmp	_strnicmp
#endif


#define DATASIZE ((int)sizeof(DATATYPE))
#define BUF_SIZE  0x400
#define BUF_TAIL  0x10

	cERROR  hResult;
	tBOOL    bRetResult = cTRUE;

	tQWORD  liSeekPos = 0;
	tQWORD  liWritePos = 0;
	tQWORD  uliPos = 0;

	tDWORD dwReadPtr = 0;
	tDWORD dwWritePtr = 0;
	tDWORD cbRead = 0;
	tDWORD cbWritten;

	DATATYPE readdata[BUF_SIZE];
	DATATYPE writedata[BUF_SIZE];
	
	tDWORD dwNestLevel = 0;
	tDWORD cbSkip = 0;	

	BOOL bTagParam=FALSE;
	DATATYPE StringChar;

	enum tagBLOCKTYPE {
		Text,
		Comment,
		Script,
		ASPScript
	} curBlockType;

	curBlockType = Text;

	hResult = pIStreamDest->SetSize( uliPos );
	if ( PR_FAIL(hResult) )
		return cFALSE;

//	hResult = pIStreamSrc->SeekRead( liSeekPos, SEEK_SET, NULL );
//	if ( PR_FAIL(hResult) ) 
//		return cFALSE;

	do {
		if ( cbRead != 0 ) {
			liSeekPos += cbRead;
//			liSeekPos.QuadPart = -BUF_TAIL*DATASIZE;
//			hResult = pIStreamSrc->SeekRead( liSeekPos, SEEK_CUR, &uliPos );
//			if ( PR_FAIL(hResult) ) {
//				bRetResult = cFALSE;
//				break;
//			}
		}
		
		hResult = pIStreamSrc->SeekRead(&cbRead, liSeekPos, readdata, BUF_SIZE*DATASIZE );
		if ( PR_FAIL(hResult) ) {
			bRetResult = cFALSE;
			break;
		}

		cbRead = cbRead / DATASIZE;

		for (dwReadPtr = 0; ((dwReadPtr<cbRead) && (dwReadPtr<BUF_SIZE-BUF_TAIL)); dwReadPtr++)	{
			DATATYPE b;
			b = readdata[dwReadPtr];
			
			if ( bTagParam == FALSE )	{
				if ( curBlockType == Text )	{
					if (b == _TXT('<')) { // tag start
						if (dwNestLevel == 0)	{
							if ( snicmp(_TXT("SCRIPT"), readdata+dwReadPtr+1, 6) == 0 && (iswspace(readdata[dwReadPtr+7])!=0 || readdata[dwReadPtr+7] == _TXT('>')) )
								curBlockType = Script;
							if ( snicmp(_TXT("!--"), readdata+dwReadPtr+1, 3) == 0 )
								curBlockType = Comment;
							dwNestLevel++;
						}
					}

				} 
				if ( (dwNestLevel == 1) && ((b == _TXT('\"')) || (b == _TXT('\''))) && ((curBlockType == Text) || (curBlockType == Script))) { // tag parameter
					StringChar = b;
					bTagParam = cTRUE;
				}
			}

			if ( curBlockType != Script ) {
				if ( cbSkip == 0 )
					writedata[dwWritePtr++] = b;
				else
					cbSkip--;
			}

			if ( bTagParam == cFALSE ) {
				switch(curBlockType) {
					case Text:
						if (dwNestLevel > 0 && b == _TXT('>'))
							dwNestLevel--;
						break;
					case Comment:
						if (b == _TXT('-') && readdata[dwReadPtr+1] == _TXT('-') && readdata[dwReadPtr+2] == _TXT('>'))
							curBlockType = Text;
						break;
					case Script:
						if (dwNestLevel > 0 && b == _TXT('>'))
							dwNestLevel--;
						if ( b == _TXT('<') && readdata[dwReadPtr+1] == _TXT('/') && snicmp(_TXT("SCRIPT"), readdata+dwReadPtr+2, 6) == 0 && readdata[dwReadPtr+8] == _TXT('>') ) {
							curBlockType = Text;
							cbSkip = 8;
						}
						break;
/*					case ASPScript:
						if (b == _TXT('%') && readdata[dwReadPtr+1] == _TXT('>'))
							curBlockType = Text;
						break;
*/
				}
			}
			else {
				if ( StringChar == b )
					bTagParam = cFALSE;
			}

			if ( dwWritePtr == BUF_SIZE )	{
				hResult = pIStreamDest->SeekWrite( &cbWritten, liWritePos, writedata, BUF_SIZE*DATASIZE );
				if ( PR_FAIL(hResult) ) {
					bRetResult = cFALSE;
					break;
				}
				liWritePos += cbWritten;
				dwWritePtr=0;
			}
		}

	} while ( dwReadPtr == BUF_SIZE-BUF_TAIL );

	hResult = pIStreamDest->SeekWrite( &cbWritten, liWritePos, writedata, dwWritePtr*DATASIZE );
	if ( PR_FAIL(hResult) )
		bRetResult = FALSE;
	else {
//		LARGE_INTEGER lPos = {0, 0};
//		ULARGE_INTEGER ulSrcPos;
//		ULARGE_INTEGER ulDstPos;
//		pIStreamDest->SeekRead( lPos, STREAM_SEEK_END, &ulDstPos );
//		pIStreamSrc->SeekRead( lPos, STREAM_SEEK_END, &ulSrcPos );
		liWritePos += cbWritten;
		bRetResult = cTRUE;//(ulDstPos.QuadPart != ulSrcPos.QuadPart);
	}

	return bRetResult;
}

class COutlookHtmlBody
{
public:
	COutlookHtmlBody(IN hIO hRtfBody): m_hRtfBody(hRtfBody), m_hHtmlBody(NULL) {};
	virtual ~COutlookHtmlBody()
	{
		if ( m_hHtmlBody ) m_hHtmlBody->sysCloseObject();
	};
	hIO GetBody()
	{
		if ( !m_hHtmlBody )
		{
			hOBJECT hParent = m_hRtfBody->get_pgOBJECT_BASED_ON();
			cERROR err = hParent->sysCreateObjectQuick((hOBJECT*)&m_hHtmlBody, IID_IO, PID_TMPFILE);
			if ( PR_SUCC(err) )
			{
				cStringObj sz = MSG_CONTENT_HTML_BODY;
				sz.copy(m_hHtmlBody, pgOBJECT_NAME);
				m_hHtmlBody->propSetObj(pgOBJECT_BASED_ON, hParent);
				ConvertOutlookRtf2Html(m_hRtfBody, m_hHtmlBody);
			}
		}
		return m_hHtmlBody;
	};
private:
	hIO m_hRtfBody;
	hIO m_hHtmlBody;
};

tBOOL IsCleanX400Message(hOS hMessage)
{
	if ( hMessage && hMessage->propGetDWord(pgPLUGIN_ID) == PID_MAILMSG )
	{
		cStringObj sz; 
		cERROR err = sz.assign(hMessage, pgMESSAGE_INTERNET_HEADERS);
		if ( PR_FAIL(err) )
				return cTRUE;
		if ( PR_SUCC(err) && sz.empty() )
				return cTRUE;
		if ( PR_SUCC(err) && cSTRING_COMP_EQ == sz.compare(0, cStringObj::whole, "Received: from The Microsoft Outlook Team", fSTRING_COMPARE_CASE_INSENSITIVE) )
				return cTRUE;
	}
	return cFALSE;
}

tDWORD CAntispamImplementer::CKLAntispamFilter::AsBwListFindPattern(
	const cAsBwListRules &patterns,
	const char *text,
	tDWORD dwMaxLevel,
	ProcessStatistics_t* pProcessStat, tDWORD dwStatAlg)
{
	std::set<tSIZE_T> found_indices;
	tDWORD dwLevel = BWFind::AsBwListFindPattern(patterns, text, dwMaxLevel, found_indices);
	if(pProcessStat)
	{
		for(std::set<tSIZE_T>::const_iterator it = found_indices.begin(); it != found_indices.end(); ++it)
			pProcessStat->AddVerdict(dwStatAlg, patterns[*it].m_sRule, patterns[*it].m_dwLevel);
	}
	return dwLevel;
}

tERROR CAntispamImplementer::CKLAntispamFilter::FilterBody( 
	IN hOS hMessage, 
	IN hIO hOriginalBody, 
	IN bool bTrain, 
	IN long double* pS, 
	IN cAntispamTrainParams::Action_t TrainAction,
	IN OUT ProcessStatistics_t* pProcessStat
	)
{
	PR_TRACE(( m_settings.hAntispamTask, prtNOTIFY, "CKLAntispamFilter.FilterBody => begin"));
	cERROR err = errOK;

	hIO hBody = hOriginalBody;
	tBOOL bIsRTF = hBody ? IsRTFHTML(hBody) : cFALSE;
	COutlookHtmlBody hOutlookHtmlBody(hOriginalBody);

	// Если IsRTF(hOriginalBody) - заменяем hBody через ConvertOutlookRtf2Html
	if ( bIsRTF )
		hBody = hOutlookHtmlBody.GetBody();

	tBOOL bNeedToCheck = pProcessStat ? (pProcessStat->m_dLastResult == 0 && !bTrain) : !bTrain;
	tBOOL bIsHTML = hBody ? IsHTML(hBody) : cFALSE;
	cStringObj szUserName = 
		pProcessStat && !pProcessStat->m_szUserName.empty() ? 
		pProcessStat->m_szUserName : "Default"; 
	cAsBwList *pUserSettings = (cAsBwList *)m_settings.pAntispamSettings->settings_byuser(szUserName, true);
	tDWORD dwTriggerLevel = 0;
	CHtmlFilter::results_t results_html;
	CTextFilter::results_t results_text;
	std::string szDecodedText = pProcessStat && !pProcessStat->m_szLastMessageBody.empty() ? std::string(pProcessStat->m_szLastMessageBody.data(), pProcessStat->m_szLastMessageBody.size()) : "";
	cStringObj szSubject = pProcessStat ? pProcessStat->m_szLastMessageSubj   : ""; 
	cStringObj szFrom =    pProcessStat ? pProcessStat->m_szLastMessageSender : "";	   
	cStringObj szTo = "";	   
	cStringObj szCC = "";	   
	results_html.clear();
	results_text.clear();
	if ( hMessage )
	{
		if (szSubject.empty()) GetStringData((hOBJECT)hMessage, pgMESSAGE_SUBJECT, szSubject);
		if (szFrom.empty())    GetStringData((hOBJECT)hMessage, pgMESSAGE_FROM,	   szFrom);
		if (szTo.empty())      GetStringData((hOBJECT)hMessage, pgMESSAGE_TO,      szTo);
		if (szCC.empty())      GetStringData((hOBJECT)hMessage, pgMESSAGE_CC,      szCC);
	}

	if ( !pProcessStat || pProcessStat->m_szLastMessageBody.empty() )
	{
		if ( hBody )
		{
			// Узнаем кодировку письма из заголовка MessagePart
			GetStringData( (hOBJECT)hBody, pgMESSAGE_PART_CHARSET, results_text.szCharset );
			ToUpper( results_text.szCharset );
			if ( IsXMacCyrillic(hMessage) ) 
				results_text.szCharset = "WINDOWS-1251";
			if ( bIsHTML )
			{
				PR_TRACE(( m_settings.hAntispamTask, prtNOTIFY, "CKLAntispamFilter::FilterBody => bIsHTML "));
				m_pHtmlFilter->Process(hBody, results_html);
				if(m_pDecoder->Decode(results_html.szVisibleText.c_str(), szDecodedText,
						results_html.szCharset.size() ? results_html.szCharset.c_str() : results_text.szCharset.c_str()))
				{
					results_text.szCharset = "WINDOWS-1251";
				}
			}
			else
			{
				PR_TRACE(( m_settings.hAntispamTask, prtNOTIFY, "CKLAntispamFilter::FilterBody => !bIsHTML "));
				string szVisibleText = "";
				CopyBodyToText(hBody, szVisibleText);
				if(m_pDecoder->Decode(szVisibleText.c_str(), szDecodedText, results_text.szCharset.c_str()))
				{
					results_text.szCharset = "WINDOWS-1251";
				}
			}
		}
		if (pProcessStat)
		{
			size_t size = szDecodedText.size();
			pProcessStat->m_szLastMessageBody.resize(size);
			if(size)
				memcpy(pProcessStat->m_szLastMessageBody.data(), szDecodedText.c_str(), size);
		}
	}

	//////////////////////////////////////////////////////////////////////////
	//
	// Антиспамерский EICAR
	//
	
	if ( bNeedToCheck && pS && 0 == szDecodedText.find("_EICAR_") && szDecodedText.size() < 20 )
	{
		if ( szSubject == "_EICAR_SPAM_" ) 
		{
			if (pS) *pS = 1;
			if (pProcessStat) pProcessStat->m_dLastResult = 1;
			if (pProcessStat) pProcessStat->m_dwLastReason = asdr_Eicar;
			if (pProcessStat) pProcessStat->AddVerdict(asdr_Eicar, "", 100);
			PR_TRACE(( m_settings.hAntispamTask, prtNOTIFY, "CKLAntispamFilter.FilterBody => _EICAR_SPAM_"));
			return err;
		}
		else if ( szSubject == "_EICAR_PROBABLE_SPAM_" )
		{
			if (pS) *pS = 0.5;
			if (pProcessStat) pProcessStat->m_dLastResult = 0.5;
			if (pProcessStat) pProcessStat->m_dwLastReason = asdr_Eicar;
			if (pProcessStat) pProcessStat->AddVerdict(asdr_Eicar, "", 100);
			PR_TRACE(( m_settings.hAntispamTask, prtNOTIFY, "CKLAntispamFilter.FilterBody => _EICAR_PROBABLE_SPAM_"));
			return err;
		}
		else if ( szSubject == "_EICAR_HAM_" )
		{
			if (pS) *pS = 0;
			if (pProcessStat) pProcessStat->m_dLastResult = 0;
			if (pProcessStat) pProcessStat->m_dwLastReason = asdr_Eicar;
			if (pProcessStat) pProcessStat->AddVerdict(asdr_Eicar, "", 100);
			PR_TRACE(( m_settings.hAntispamTask, prtNOTIFY, "CKLAntispamFilter.FilterBody => _EICAR_HAM_"));
			return err;
		}
		if (pProcessStat) pProcessStat->AddVerdict(asdr_Eicar, "", 0);
		PR_TRACE(( m_settings.hAntispamTask, prtNOTIFY, "CKLAntispamFilter.FilterBody => _EICAR_ finish"));
	}
	//
	// Антиспамерский EICAR
	//
	//////////////////////////////////////////////////////////////////////////

	if ( bNeedToCheck && pUserSettings && pUserSettings->m_bUseCannotBeSpamValidate.m_bEnabled && m_settings.pAntispamSettings->m_bUsePerUserSettings)
	{
		if ( pUserSettings->m_bUseCannotBeSpamValidate.m_dwLevel >= 100 && IsCleanX400Message(hMessage) )
		{
			if (pS) *pS = 0;
			if (pProcessStat) pProcessStat->m_dLastResult = 0;
			if (pProcessStat) pProcessStat->m_dwLastReason = asdr_CannotBeSpam;
			if (pProcessStat) pProcessStat->AddVerdict(asdr_CannotBeSpam, "", pUserSettings->m_bUseCannotBeSpamValidate.m_dwLevel);
			PR_TRACE(( m_settings.hAntispamTask, prtNOTIFY, "CKLAntispamFilter.FilterBody => _CANNOT_BE_SPAM_"));
			return err;
		}
		else
			if (pProcessStat) pProcessStat->AddVerdict(asdr_CannotBeSpam, "", 0);
		PR_TRACE(( m_settings.hAntispamTask, prtNOTIFY, "CKLAntispamFilter.FilterBody => _CANNOT_BE_SPAM_ finish (%d)", dwTriggerLevel));
	}

	//////////////////////////////////////////////////////////////////////////
	//
	// Message For Me Validate
	//
	if ( bNeedToCheck && pUserSettings && pUserSettings->m_bUseNotForMeValidate.m_bEnabled && m_settings.pAntispamSettings->m_bUsePerUserSettings)
	{
		tBOOL bAddressFound = cFALSE;
		cAsBwListRule hTORule(szTo, cTRUE, cTRUE);
		cAsBwListRule hCCRule(szCC, cTRUE, cTRUE);
		for ( tDWORD i = 0; i < pUserSettings->m_aMyAddrs.size(); i++ ) 
		{
			if ( pUserSettings->m_aMyAddrs[i].m_bEnabled )
			{
				cAsBwListRule hRule(pUserSettings->m_aMyAddrs[i].m_sRule, cTRUE, cTRUE, cTRUE);
				if ( hRule == hTORule || hRule == hCCRule )
				{
					bAddressFound = cTRUE;
					break;
				}
			}
		}
		if ( !bAddressFound )
			dwTriggerLevel += pUserSettings->m_bUseNotForMeValidate.m_dwLevel;
		if ( dwTriggerLevel >= pUserSettings->m_dwTriggersLimit )
		{
			if (pS) *pS = 1;
			if (pProcessStat) pProcessStat->m_dLastResult = 1;
			if (pProcessStat) pProcessStat->m_dwLastReason = asdr_NotForMe;
			if (pProcessStat) pProcessStat->AddVerdict(asdr_NotForMe, "", pUserSettings->m_bUseNotForMeValidate.m_dwLevel);
			PR_TRACE(( m_settings.hAntispamTask, prtNOTIFY, "CKLAntispamFilter.FilterBody => _NOT_FOR_ME_"));
			return err;
		}
		else
		{
			if (pS) *pS = 0;
			if (pProcessStat) pProcessStat->m_dLastResult = 0;
			if (pProcessStat) pProcessStat->m_dwLastReason = asdr_NOTHING; //asdr_NotForMe_NOTFOUND;
			if (pProcessStat) pProcessStat->AddVerdict(asdr_NotForMe, "", 0);
		}
		PR_TRACE(( m_settings.hAntispamTask, prtNOTIFY, "CKLAntispamFilter.FilterBody => _NOT_FOR_ME_ finish (%d)", dwTriggerLevel));
	}
	//
	// Message For Me Validate
	//
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	//
	// Recipients Limit Validate
	//
	if ( bNeedToCheck && pUserSettings && pUserSettings->m_bUseRecipLimit.m_bEnabled && m_settings.pAntispamSettings->m_bUsePerUserSettings )
	{
		if ( pUserSettings && !szTo.empty() && !szCC.empty() )
		{
			tDWORD dwRecipCount = 0;
			tDWORD dwDelimPos = 0;
			while ( cStringObj::npos != (dwDelimPos = szTo.find_first_of(";", dwDelimPos)) )
				dwRecipCount++;
			while ( cStringObj::npos != (dwDelimPos = szCC.find_first_of(";", dwDelimPos)) )
				dwRecipCount++;
			if ( dwRecipCount >= pUserSettings->m_dwRecipLimit )
				dwTriggerLevel += pUserSettings->m_bUseRecipLimit.m_dwLevel;
			if ( dwTriggerLevel >= pUserSettings->m_dwTriggersLimit )
			{
				if (pS) *pS = 1;
				if (pProcessStat) pProcessStat->m_dLastResult = 1;
				if (pProcessStat) pProcessStat->m_dwLastReason = asdr_RecipLimit;
				if (pProcessStat) pProcessStat->AddVerdict(asdr_RecipLimit, "", pUserSettings->m_bUseRecipLimit.m_dwLevel);
				PR_TRACE(( m_settings.hAntispamTask, prtNOTIFY, "CKLAntispamFilter.FilterBody => _RECIPIENT_COUNT_"));
				return err;
			}
			else
			{
				if (pS) *pS = 0;
				if (pProcessStat) pProcessStat->m_dLastResult = 0;
				if (pProcessStat) pProcessStat->m_dwLastReason = asdr_NOTHING; //asdr_RecipLimit_NOTFOUND;
				if (pProcessStat) pProcessStat->AddVerdict(asdr_RecipLimit, "", 0);
			}
			PR_TRACE(( m_settings.hAntispamTask, prtNOTIFY, "CKLAntispamFilter.FilterBody => _RECIPIENT_COUNT_ finish (%d)", dwTriggerLevel));
		}
	}
	//
	// Recipients Limit Validate
	//
	//////////////////////////////////////////////////////////////////////////
	
	if ( bIsHTML )
	{
		//////////////////////////////////////////////////////////////////////////
		//
		// Remove Scripts
		//
		if ( bNeedToCheck && pUserSettings && pUserSettings->m_bRemoveScripts && m_settings.pAntispamSettings->m_bUsePerUserSettings )
		{
			if ( results_html.ulScripts )
			{
				PR_TRACE(( m_settings.hAntispamTask, prtNOTIFY, "CKLAntispamFilter.FilterBody => _REMOVE_SCRIPTS_ start"));
				hIO temp_io = NULL;
				cERROR err_io = hBody->sysCreateObjectQuick( (hOBJECT*)&temp_io, IID_IO, PID_TMPFILE);
				if ( PR_SUCC(err) )
				{
					if ( RemoveScripts(hBody, temp_io) && hBody->SetSize(0) )
					{
						tCHAR buf[1020+1];
						tQWORD qwRead = 0;
						tDWORD dwRead = 0;
						tQWORD qwWrite = 0;
						tDWORD dwWritten = 0;
						while ( PR_SUCC(err_io= temp_io->SeekRead(&dwRead, qwRead, buf, 1020)) && dwRead) 
						{
							qwRead += dwRead;
							if ( PR_SUCC(err_io= hBody->SeekWrite(&dwWritten, qwWrite, buf, dwRead)) && dwWritten)
								qwWrite += dwWritten;
						}
					}
					temp_io->sysCloseObject();
					temp_io = NULL;
				}
				PR_TRACE(( m_settings.hAntispamTask, prtNOTIFY, "CKLAntispamFilter.FilterBody => _REMOVE_SCRIPTS_ finish"));
			}
		}
		//
		// Remove Scripts
		//
		//////////////////////////////////////////////////////////////////////////
		
		//////////////////////////////////////////////////////////////////////////
		//
		// HTML Validate
		//

			//////////////////////////////////////////////////////////////////////////
			//
			//	#define CHECK_HTML_STAT(_stat_)
			//
			#define CHECK_HTML_STAT(_stat_)	\
				if (\
					pUserSettings->m_bUseHTMLValidate_##_stat_.m_bEnabled && \
					results_html.ul##_stat_	\
					)	\
					if (pProcessStat) pProcessStat->AddVerdict(asdr_InvalidHTML_##_stat_, "", pUserSettings->m_bUseHTMLValidate_##_stat_.m_dwLevel); \
				if (\
					pUserSettings->m_bUseHTMLValidate_##_stat_.m_bEnabled && \
					results_html.ul##_stat_ && \
					(dwTriggerLevel+=pUserSettings->m_bUseHTMLValidate_##_stat_.m_dwLevel) >= pUserSettings->m_dwTriggersLimit \
					)	\
				{	\
					if (pS) *pS = 1;	\
					if (pProcessStat) pProcessStat->m_dLastResult = 1;	\
					if (pProcessStat) pProcessStat->m_dwLastReason = asdr_InvalidHTML_##_stat_;	\
					if (pProcessStat) pProcessStat->AddVerdict(asdr_InvalidHTML_##_stat_, "", pUserSettings->m_bUseHTMLValidate_##_stat_.m_dwLevel); \
					PR_TRACE(( m_settings.hAntispamTask, prtNOTIFY, "CKLAntispamFilter.FilterBody => _CHECK_HTML_ ##_stat_"));	\
					return err;	\
				}	\
				else	\
				{	\
					if (pS) *pS = 0;	\
					if (pProcessStat) pProcessStat->m_dLastResult = 0;	\
					if (pProcessStat) pProcessStat->m_dwLastReason = asdr_NOTHING; \
				}	
			//
			//	#define CHECK_HTML_STAT(_stat_)
			//
			//////////////////////////////////////////////////////////////////////////
		
		if ( bNeedToCheck && pUserSettings && m_settings.pAntispamSettings->m_bUsePerUserSettings )
		{
			CHECK_HTML_STAT(UnknownDefsCount); 		// Количество несуществующих тегов
			CHECK_HTML_STAT(SeemsColors);			// Количество блоков текста с цветом фона
			CHECK_HTML_STAT(SmallText);				// Количество блоков с очень маленьким текстом
			CHECK_HTML_STAT(InvisibleCharCount);	// Количество невидимых символов
			CHECK_HTML_STAT(Scripts);				// Количество скриптов
			CHECK_HTML_STAT(HiddenElements);		// Количество скрытых элементов (style="visibility:hidden")
			PR_TRACE(( m_settings.hAntispamTask, prtNOTIFY, "CKLAntispamFilter.FilterBody => _CHECK_HTML_ finish (%d)", dwTriggerLevel));
		}
		#undef CHECK_HTML_STAT
		//
		// HTML Validate
		//
		//////////////////////////////////////////////////////////////////////////
		
		//////////////////////////////////////////////////////////////////////////
		//
		// Internal Objects Validate
		//
		if ( bNeedToCheck && pUserSettings && pUserSettings->m_bUseObjectsValidate.m_bEnabled && m_settings.pAntispamSettings->m_bUsePerUserSettings )
		{
			if (
				results_html.ulObjects &&				// Количество объектов в письме
				results_html.szVisibleText.empty()		// Без текста
				) 
				dwTriggerLevel += pUserSettings->m_bUseObjectsValidate.m_dwLevel;
			if ( dwTriggerLevel >= pUserSettings->m_dwTriggersLimit )
			{
				if (pS) *pS = 1;
				if (pProcessStat) pProcessStat->m_dLastResult = 1;
				if (pProcessStat) pProcessStat->m_dwLastReason = asdr_InternalObj;
				if (pProcessStat) pProcessStat->AddVerdict(asdr_InternalObj, "", 100);
				PR_TRACE(( m_settings.hAntispamTask, prtNOTIFY, "CKLAntispamFilter.FilterBody => _INTERNAL_OBJECTS_"));
				return err;
			}
			else
			{
				if (pS) *pS = 0;
				if (pProcessStat) pProcessStat->m_dLastResult = 0;
				if (pProcessStat) pProcessStat->m_dwLastReason = asdr_NOTHING; //asdr_InternalObj_NOTFOUND;
				if (pProcessStat) pProcessStat->AddVerdict(asdr_InternalObj, "", 0);
			}
			PR_TRACE(( m_settings.hAntispamTask, prtNOTIFY, "CKLAntispamFilter.FilterBody => _INTERNAL_OBJECTS_ finish (%d)", dwTriggerLevel));
		}
		//
		// Internal Objects Validate
		//
		//////////////////////////////////////////////////////////////////////////
		
		//////////////////////////////////////////////////////////////////////////
		//
		// External Objects Validate
		//
		if ( bNeedToCheck && pUserSettings && pUserSettings->m_bUseExternalObjectsValidate.m_bEnabled && m_settings.pAntispamSettings->m_bUsePerUserSettings )
		{
			if (
				results_html.ulExternalObjects			// Количество встроенных внешних объектов
				) 
				dwTriggerLevel += pUserSettings->m_bUseExternalObjectsValidate.m_dwLevel;
			if ( dwTriggerLevel >= pUserSettings->m_dwTriggersLimit )
			{
				if (pS) *pS = 1;
				if (pProcessStat) pProcessStat->m_dLastResult = 1;
				if (pProcessStat) pProcessStat->m_dwLastReason = asdr_ExternalObj;
				if (pProcessStat) pProcessStat->AddVerdict(asdr_ExternalObj, "", 100);
				PR_TRACE(( m_settings.hAntispamTask, prtNOTIFY, "CKLAntispamFilter.FilterBody => _EXTERNAL_OBJECTS_"));
				return err;
			}
			else
			{
				if (pS) *pS = 0;
				if (pProcessStat) pProcessStat->m_dLastResult = 0;
				if (pProcessStat) pProcessStat->m_dwLastReason = asdr_NOTHING; //asdr_ExternalObj_NOTFOUND;
				if (pProcessStat) pProcessStat->AddVerdict(asdr_ExternalObj, "", 0);
			}
			PR_TRACE(( m_settings.hAntispamTask, prtNOTIFY, "CKLAntispamFilter.FilterBody => _EXTERNAL_OBJECTS_ finish (%d)", dwTriggerLevel));
		}
		//
		// External Objects Validate
		//
		//////////////////////////////////////////////////////////////////////////
	}

	//////////////////////////////////////////////////////////////////////////
	//
	// Empty Message Validate
	//
	if ( bNeedToCheck && pUserSettings && pUserSettings->m_bUseEmptyValidate.m_bEnabled && m_settings.pAntispamSettings->m_bUsePerUserSettings )
	{
		if (
			szSubject.empty() &&		// Без темы 
			szDecodedText.empty()		// Без текста
			) 
			dwTriggerLevel += pUserSettings->m_bUseEmptyValidate.m_dwLevel;
		if ( dwTriggerLevel >= pUserSettings->m_dwTriggersLimit )
		{
			if (pS) *pS = 1;
			if (pProcessStat) pProcessStat->m_dLastResult = 1;
			if (pProcessStat) pProcessStat->m_dwLastReason = asdr_EmptyMessage;
			if (pProcessStat) pProcessStat->AddVerdict(asdr_EmptyMessage, "", 100);
			PR_TRACE(( m_settings.hAntispamTask, prtNOTIFY, "CKLAntispamFilter.FilterBody => _EMPTY_MESSAGE_"));
			return err;
		}
		else
		{
			if (pS) *pS = 0;
			if (pProcessStat) pProcessStat->m_dLastResult = 0;
			if (pProcessStat) pProcessStat->m_dwLastReason = asdr_NOTHING; //asdr_EmptyMessage_NOTFOUND;
			if (pProcessStat) pProcessStat->AddVerdict(asdr_EmptyMessage, "", 0);
		}
		PR_TRACE(( m_settings.hAntispamTask, prtNOTIFY, "CKLAntispamFilter.FilterBody => _EMPTY_MESSAGE_ finish (%d)", dwTriggerLevel));
	}
	//
	// Empty Message Validate
	//
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	//
	// Чернобелые строки (работаем по Теме и по Телу письма)
	//
	if ( 
		bNeedToCheck && 
		!szDecodedText.empty() &&
		pUserSettings &&
		(
			m_settings.pAntispamSettings->m_bUseWhiteStrings || 
			m_settings.pAntispamSettings->m_bUseBlackStrings
		))
	{
		cAsBwListRule hSubjectRule(szSubject, cTRUE, cTRUE); 
		if ( m_settings.pAntispamSettings->m_bUseWhiteStrings )
		{
			PR_TRACE(( m_settings.hAntispamTask, prtNOTIFY, "CKLAntispamFilter.FilterBody => _WHITE_STRING_ start search"));

			if (
				pUserSettings->m_aWhiteWords.count() && 
				( 
					AsBwListFindPattern(pUserSettings->m_aWhiteWords, hSubjectRule.m_sRule.c_str(cCP_ANSI), 0, pProcessStat, asdr_WhiteString) ||
					AsBwListFindPattern(pUserSettings->m_aWhiteWords, szDecodedText.c_str(), 0, pProcessStat, asdr_WhiteString)
				))
			{
				if (pS) *pS = 0;
				if (pProcessStat) pProcessStat->m_dLastResult = 0;
				if (pProcessStat) pProcessStat->m_dwLastReason = asdr_WhiteString;
				if (pProcessStat) pProcessStat->AddVerdict(asdr_WhiteString, "", 100);
				PR_TRACE(( m_settings.hAntispamTask, prtNOTIFY, "CKLAntispamFilter.FilterBody => _WHITE_STRING_"));
				return err;
			}
			else
			{
				if (pS) *pS = 0;
				if (pProcessStat) pProcessStat->m_dLastResult = 0;
				if (pProcessStat) pProcessStat->m_dwLastReason = asdr_WhiteString_NOTFOUND;
				if (pProcessStat) pProcessStat->AddVerdict(asdr_WhiteString, "", 0);
			}
			PR_TRACE(( m_settings.hAntispamTask, prtNOTIFY, "CKLAntispamFilter.FilterBody => _WHITE_STRING_ finish"));
		}
		if ( m_settings.pAntispamSettings->m_bUseBlackStrings )
		{
			tDWORD dwBlackLevel = 0;

			PR_TRACE(( m_settings.hAntispamTask, prtNOTIFY, "CKLAntispamFilter.FilterBody => _BLACK_STRING_ start search"));

			dwBlackLevel += AsBwListFindPattern(pUserSettings->m_aBlackWords, hSubjectRule.m_sRule.c_str(cCP_ANSI), pUserSettings->m_dwBlackWordsLimit, pProcessStat, asdr_BlackString);
			if(dwBlackLevel < pUserSettings->m_dwBlackWordsLimit)
				dwBlackLevel += AsBwListFindPattern(pUserSettings->m_aBlackWords, szDecodedText.c_str(), pUserSettings->m_dwBlackWordsLimit - dwBlackLevel, pProcessStat, asdr_BlackString);
			
			PR_TRACE(( m_settings.hAntispamTask, prtNOTIFY, "CKLAntispamFilter.FilterBody => _BLACK_STRING_ search finished (level %d)", dwBlackLevel));

			if ( dwBlackLevel >= pUserSettings->m_dwBlackWordsLimit )
			{
				if (pS) *pS = 1;
				if (pProcessStat) pProcessStat->m_dLastResult = 1;
				if (pProcessStat) pProcessStat->m_dwLastReason = asdr_BlackString;
				if (pProcessStat) pProcessStat->AddVerdict(asdr_BlackString, "", dwBlackLevel);
				PR_TRACE(( m_settings.hAntispamTask, prtNOTIFY, "CKLAntispamFilter.FilterBody => _BLACK_STRING_ limit exceeded"));
				return err;
			}

			if (pS) *pS = 0;
			if (pProcessStat) pProcessStat->m_dLastResult = 0;
			if (pProcessStat) pProcessStat->m_dwLastReason = asdr_NOTHING; //asdr_BlackString_NOTFOUND;
			if (pProcessStat) pProcessStat->AddVerdict(asdr_BlackString, "", dwBlackLevel);

			PR_TRACE(( m_settings.hAntispamTask, prtNOTIFY, "CKLAntispamFilter.FilterBody => _BLACK_STRING_ finish"));
		}
	}
	//
	// Чернобелые строки (работаем по Теме и по Телу письма)
	//
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	//
	// Проверки найденых в письме URL'ов
	//
	if ( 
		bIsHTML &&
		bNeedToCheck && 
		(m_settings.pAntispamSettings->m_bUseAntifishing ||
		(pUserSettings && pUserSettings->m_bUseURLChecking.m_bEnabled)
		))
	{
		for (CHtmlFilter::tokens_t::const_iterator 
			iTerator = results_html.tokens.end();
			iTerator != results_html.tokens.begin();
			)
		{
			string url = (char*)(--iTerator)->c_str();
			if ( 0 == url.find("http://") )
			{
				//////////////////////////////////////////////////////////////////////////
				//
				// Антифишинг
				//
				// Бросить этот URL в сторону задачи Антифишинга. Если ответят "AccessDenied", то вернуть
				if ( m_settings.pAntispamSettings->m_bUseAntifishing )
				{
					cAntiPhishingCheckRequest hAFParams;
					hAFParams.m_sURL = url.c_str();
					hAFParams.m_bInteractive = cFALSE;
					cERROR errAFish = m_settings.hAntispamTask->sysSendMsg(
						pmc_ANTI_PHISHING_TASK, 
						msg_ANTI_PHISHING_CHECK_URL,
						NULL,
						&hAFParams,
						SER_SENDMSG_PSIZE
						);
					if ( errACCESS_DENIED == errAFish )
					{
						if (pS) *pS = 1;
						if (pProcessStat) pProcessStat->m_dLastResult = 1;
						if (pProcessStat) pProcessStat->m_dwLastReason = asdr_AntiFishing;
						if (pProcessStat) pProcessStat->AddVerdict(asdr_AntiFishing, "", 100);
						PR_TRACE(( m_settings.hAntispamTask, prtNOTIFY, "CKLAntispamFilter.FilterBody => _ANTI_FISHING_"));
						return err;
					}
					else if ( (errOK_NO_DECIDERS != err) && !m_settings.pAntispamSettings->m_bUseBayesian )
					{
						if (pS) *pS = 0;
						if (pProcessStat) pProcessStat->m_dLastResult = 0;
						if (pProcessStat) pProcessStat->m_dwLastReason = asdr_NOTHING; //asdr_AntiFishing_NOTFOUND;
						if (pProcessStat) pProcessStat->AddVerdict(asdr_AntiFishing, "", 0);
					}
					PR_TRACE(( m_settings.hAntispamTask, prtNOTIFY, "CKLAntispamFilter.FilterBody => _ANTI_FISHING_ finish"));
				}
				//
				// Антифишинг
				//
				//////////////////////////////////////////////////////////////////////////

				//////////////////////////////////////////////////////////////////////////
				//
				// Рекламные URL'ы
				//
				if ( pUserSettings && pUserSettings->m_bUseURLChecking.m_bEnabled && m_settings.pAntispamSettings->m_bUsePerUserSettings )
				{
					CSFDBRecordData data;
					if ( 
						m_pBayesFilter->GetData((char*)url.c_str(), data) &&
						data.ulSpamCount > data.ulHamCount
						)
						dwTriggerLevel += pUserSettings->m_bUseURLChecking.m_dwLevel;
					if ( dwTriggerLevel >= pUserSettings->m_dwTriggersLimit )
					{
						if (pS) *pS = 1;
						if (pProcessStat) pProcessStat->m_dLastResult = 1;
						if (pProcessStat) pProcessStat->m_dwLastReason = asdr_Banner;
						if (pProcessStat) pProcessStat->AddVerdict(asdr_Banner, "", pUserSettings->m_bUseURLChecking.m_dwLevel);
						PR_TRACE(( m_settings.hAntispamTask, prtNOTIFY, "CKLAntispamFilter.FilterBody => _BANNER_"));
						return err;
					}
					else
					{
						if (pS) *pS = 0;
						if (pProcessStat) pProcessStat->m_dLastResult = 0;
						if (pProcessStat) pProcessStat->m_dwLastReason = asdr_NOTHING; //asdr_Banner_NOTFOUND;
						if (pProcessStat) pProcessStat->AddVerdict( asdr_Banner, "", 0);
					}
					PR_TRACE(( m_settings.hAntispamTask, prtNOTIFY, "CKLAntispamFilter.FilterBody => _BANNER_ finish"));
				}
				//
				// Рекламные URL'ы
				//
				//////////////////////////////////////////////////////////////////////////
			}
		}
	}
	//
	// Проверки найденых в письме URL'ов
	//
	//////////////////////////////////////////////////////////////////////////
	
	PR_TRACE(( m_settings.hAntispamTask, prtNOTIFY, "CKLAntispamFilter.FilterBody => (%d) %s", szDecodedText.size(), szDecodedText.c_str()));
	std::vector<char> TextToProcess(szDecodedText.c_str(), szDecodedText.c_str() + szDecodedText.size() + 1);
	m_pTextFilter->Process(
		&TextToProcess[0],
		results_text
		);
	PR_TRACE(( m_settings.hAntispamTask, prtNOTIFY, "CKLAntispamFilter.FilterBody => [%d] tokens", results_text.tokens.size()));

	//////////////////////////////////////////////////////////////////////////
	//
	// Неанглоязычные письма
	//
	if ( bNeedToCheck && pUserSettings && pUserSettings->m_bUseNonLatLimit.m_bEnabled && results_text.ulChars && m_settings.pAntispamSettings->m_bUsePerUserSettings )
	{
		tDWORD dwNonLatLevel = results_text.ulChars ? results_text.ulRusChars * 100 / results_text.ulChars : 0;
		if ( dwNonLatLevel >= pUserSettings->m_dwNonLatLimit )
			dwTriggerLevel += pUserSettings->m_bUseNonLatLimit.m_dwLevel;
		if ( dwTriggerLevel >= pUserSettings->m_dwTriggersLimit )
		{
			if (pS) *pS = 1;
			if (pProcessStat) pProcessStat->m_dLastResult = 1;
			if (pProcessStat) pProcessStat->m_dwLastReason = asdr_NotEnglish;
			if (pProcessStat) pProcessStat->AddVerdict( asdr_NotEnglish, "", pUserSettings->m_bUseNonLatLimit.m_dwLevel);
			PR_TRACE(( m_settings.hAntispamTask, prtNOTIFY, "CKLAntispamFilter.FilterBody => _NOT_ENGLISH_"));
			return err;
		}
		else
		{
			if (pS) *pS = 0;
			if (pProcessStat) pProcessStat->m_dLastResult = 0;
			if (pProcessStat) pProcessStat->m_dwLastReason = asdr_NOTHING; //asdr_NotEnglish_NOTFOUND;
			if (pProcessStat) pProcessStat->AddVerdict( asdr_NotEnglish, "", 0);
		}
		PR_TRACE(( m_settings.hAntispamTask, prtNOTIFY, "CKLAntispamFilter.FilterBody => _NOT_ENGLISH_ finish (%d)", dwTriggerLevel));
	}
	//
	// Неанглоязычные письма
	//
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	//
	// Spamtest
	//
	if ( bNeedToCheck && 
		(m_settings.pAntispamSettings->m_bUsePDB ||
			m_settings.pAntispamSettings->m_bUseGSG ||
			m_settings.pAntispamSettings->m_bUseRecentTerms) )
	{
		hOBJECT pSpamtest = NULL;
		hTASKMANAGER tm = (hTASKMANAGER)m_settings.hAntispamTask->propGetObj(pgTASK_TM);
		if ( tm && PR_SUCC(tm->GetService( TASKID_TM_ITSELF, m_settings.hAntispamTask, AVP_SERVICE_SPAMTEST, &pSpamtest, cREQUEST_SYNCHRONOUS)))
		{
			cSpamtestParams hSpamtestParams;
			hSpamtestParams.m_os = (hOBJECT)hMessage;
			hSpamtestParams.m_bUseGSG = m_settings.pAntispamSettings->m_bUseGSG;
			hSpamtestParams.m_bUsePDB = m_settings.pAntispamSettings->m_bUsePDB;
			hSpamtestParams.m_bUseRecentTerms = m_settings.pAntispamSettings->m_bUseRecentTerms;
			//if(hSpamtestParams.m_os)
			//	hSpamtestParams.m_os->propSetBool(pgPROXY_CREATE_REMOTE, cTRUE);
			cERROR errSpamtest = m_settings.hAntispamTask->sysSendMsg(
				pmc_SPAMTEST, 
				msg_ISSPAMMESSAGE,
				hSpamtestParams.m_os,
				&hSpamtestParams,
				SER_SENDMSG_PSIZE
				);
			if ( (errOK_DECIDED==errSpamtest) )
			{
				tDOUBLE res = 0;
				switch (hSpamtestParams.m_result) 
				{
				case 0: res = 0;   break;
				case 2: res = 0.56;break;
				case 1: res = 1;   break;
				}
				if (pS) *pS = res;
				if (pProcessStat) 
				{
					pProcessStat->m_dLastResult = res;
					switch (hSpamtestParams.m_facility) 
					{
					case 1: pProcessStat->m_dwLastReason = asdr_PDB;     break;
					case 2: pProcessStat->m_dwLastReason = asdr_GSG;     break;
					case 3: pProcessStat->m_dwLastReason = asdr_RecentTerms;     break;
					default:pProcessStat->m_dwLastReason = asdr_NOTHING; break;
					}
					PR_TRACE(( m_settings.hAntispamTask, prtNOTIFY, "CKLAntispamFilter.FilterBody => _SPAMTEST_ (%d)", hSpamtestParams.m_facility));
				}
				if ( res > (long double)m_settings.pAntispamSettings->m_dProbableSpamHighLevel/100 )
				{
					if (pS) *pS = res;
					if (pProcessStat) pProcessStat->AddVerdict( pProcessStat->m_dwLastReason, "", 100);
					tm->ReleaseService(TASKID_TM_ITSELF, pSpamtest);
					return err;
				}
				if (pProcessStat) pProcessStat->AddVerdict( pProcessStat->m_dwLastReason == asdr_NOTHING ? asdr_SPAMTEST : pProcessStat->m_dwLastReason, "", (tDWORD)(res * 100));
			}
			tm->ReleaseService(TASKID_TM_ITSELF, pSpamtest);
			PR_TRACE(( m_settings.hAntispamTask, prtNOTIFY, "CKLAntispamFilter.FilterBody => _SPAMTEST_ finish"));
		}
	}
	//
	// Spamtest
	//
	//////////////////////////////////////////////////////////////////////////
	
	cERROR err_bayes = errOK;
	CHECK_SPAMHAM_LETTERS(err_bayes);
	if ( bTrain ) 
	{
		tBOOL dwMessageIsOutgoing = hBody && !hBody->propGetBool(g_propMessageIsIncoming);
		if ( 
			hBody && (TrainAction==cAntispamTrainParams::TrainAsHam) && dwMessageIsOutgoing && 
			(m_statistics.m_dwHamBaseSize > m_settings.pAntispamSettings->m_dMinHamLettersCount) &&
			(m_statistics.m_dwHamBaseSize * 1.5 > m_statistics.m_dwSpamBaseSize)
			)
		{
			bool bAddedToWhiteRecipient = pProcessStat && (pProcessStat->m_dwLastReason == asdr_AddedToWhiteRecipient);
			if (pS) *pS = bAddedToWhiteRecipient ? -1 : -2;
			if (pProcessStat) pProcessStat->m_dLastResult = bAddedToWhiteRecipient ? -1 : -2;
			if (pProcessStat) pProcessStat->m_dwLastReason = bAddedToWhiteRecipient ? asdr_AddedToWhiteRecipient : asdr_Training;
			PR_TRACE(( m_settings.hAntispamTask, prtNOTIFY, "CKLAntispamFilter.FilterBody => _TRAIN_ finish (without iBayes)"));
			return err;
		}
		if ( !results_text.tokens.size() )
		{
			if (pS) *pS = -2;
			if (pProcessStat) pProcessStat->m_dLastResult = -2;
			if (pProcessStat) pProcessStat->m_dwLastReason = asdr_EmptyMessage;
			PR_TRACE(( m_settings.hAntispamTask, prtNOTIFY, "CKLAntispamFilter.FilterBody => _TRAIN_ denied: _EMPTY_MESSAGE_"));
			return err;
		}
		else
		{
			err = m_pBayesFilter->Update(
				results_text.tokens, 
				TrainAction
				);
			if (pS) *pS = -1;
			if (pProcessStat) pProcessStat->m_dLastResult = -1;
			if (pProcessStat) pProcessStat->m_dwLastReason = asdr_Training;
			PR_TRACE(( m_settings.hAntispamTask, prtNOTIFY, "CKLAntispamFilter.FilterBody => _TRAIN_ finish"));
		}
	}
	else if ( m_settings.pAntispamSettings->m_bUseBayesian )
	{
		//////////////////////////////////////////////////////////////////////////
		//
		// Байес
		//
		if ( PR_SUCC(err_bayes) )
		{
			if ( results_text.tokens.size() )
			{
				long double S = m_pBayesFilter->Process(results_text.tokens);
				if (pS) *pS = S;
				if (pProcessStat) pProcessStat->m_dLastResult = S;
				if (pProcessStat) pProcessStat->m_dwLastReason = asdr_Bayesian;
				if (pProcessStat) pProcessStat->AddVerdict( asdr_Bayesian, "", 100);
				PR_TRACE(( m_settings.hAntispamTask, prtNOTIFY, "CKLAntispamFilter.FilterBody => _iBAYES_"));
				return err;
			}
			else
			{
				if (pProcessStat) pProcessStat->AddVerdict( asdr_Bayesian, "", 0);
			}
		}
		PR_TRACE(( m_settings.hAntispamTask, prtNOTIFY, "CKLAntispamFilter.FilterBody => _iBAYES_ denied: need training (Spam: %d; Ham: %d)", data.ulSpamCount, data.ulHamCount));
		// 
		// Байес
		//
		//////////////////////////////////////////////////////////////////////////
	}

	if (
		!bTrain &&
		pProcessStat &&
		pProcessStat->m_dwLastReason == asdr_WhiteAddress_NOTFOUND ||
		pProcessStat->m_dwLastReason == asdr_WhiteString_NOTFOUND
		)
	{
		//////////////////////////////////////////////////////////////////////////
		//
		// Если письмо не прошло белые списки, 
		// и других причин считать его неспамом не нашлось - считаем письмо спамом
		//
		// (если письмо от майлДиспетчера не прошло WhiteString - не считаем его спамом)
		if ( !(
			hMessage && hMessage->propGetBool(g_propMessageCheckOnly) && 
			pProcessStat->m_dwLastReason == asdr_WhiteAddress_NOTFOUND &&
			m_settings.pAntispamSettings->m_bUseWhiteStrings &&
			pUserSettings &&
			!pUserSettings->m_aWhiteWords.empty()
			))
		{
			if (pS) *pS = 1;
			if (pProcessStat) pProcessStat->m_dLastResult = 1;
		}
		PR_TRACE(( m_settings.hAntispamTask, prtNOTIFY, "CKLAntispamFilter.FilterBody => _IS_NOT_WHITE_"));
		//
		// Если письмо не прошло белые списки, 
		// и других причин считать его неспамом не нашлось - считаем письмо спамом
		//
		//////////////////////////////////////////////////////////////////////////
	}

	PR_TRACE(( m_settings.hAntispamTask, prtNOTIFY, "CKLAntispamFilter.FilterBody => end (%terr)", err));

	return err;
}
