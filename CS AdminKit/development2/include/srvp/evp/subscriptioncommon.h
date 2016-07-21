/*!
 * (C) 2002 "Kaspersky Lab"
 *
 * \file evp/subscriptioncommon.h
 * \author Михаил Кармазин
 * \date 17:54 09.12.2002
 * \brief Общий файл для подписки на события от агента. Содержит константы-имена
	 в контейнере Params, содержащем данные о том, на какие события от агента 
	 подписывается сервер.
 */

#ifndef _EVP_SUBSCRIPTION_COMMON_H_FBC25167_F922_4fc7_92ED_EE2B48F1E582__
#define _EVP_SUBSCRIPTION_COMMON_H_FBC25167_F922_4fc7_92ED_EE2B48F1E582__

/* Note that E2S_SUBSCRIPTION_PREFIX must not be a part 
   of E2S_TEMP_SUBSCRIPTION_PREFIX !!!. For example, such combination is INVALID:
E2S_SUBSCRIPTION_PREFIX	     is "e2s_sbscr_"
E2S_TEMP_SUBSCRIPTION_PREFIX is "e2s_sbscr_temp_"  */
#define E2S_SUBSCRIPTION_PREFIX	        L"e2s_sbscr_"
#define E2S_TEMP_SUBSCRIPTION_PREFIX	L"e2s_temp_sbscr_" 

#define E2S_POLICY                      L"policy"

//! Подписка на события от задачи/политики
#define E2S_SUBSCRIPTION_ID(item_id, sev) \
     (std::wstring(E2S_SUBSCRIPTION_PREFIX)+ \
      std::wstring(sev) + \
      std::wstring(item_id) )

namespace KLEVP
{
    const wchar_t c_szSubscriptionDataFile[]=L"e2s_subscription.xml";

    const wchar_t c_spE2SSubscrSection[]=L"SubscriptionData";
    const wchar_t c_spE2SFakeSubscrSection[]=L"FakeSubscriptionData";
    
    const wchar_t c_spE2S_LSTask[]=L"TASK_ID";       
	const wchar_t c_spE2S_LSTaskResultCode[]=L"TASK_ID_RSLT_CODE";
	const wchar_t c_spE2S_LSTaskResultDescr[]=L"TASK_ID_RSLT_DESCR";
	const wchar_t c_spE2S_LSTaskTmpFileToDeleteOnRestart[]=L"TASK_ID_DEL_ON_RESTART";
    const wchar_t c_spE2S_LSTaskDistributivePath[]=L"TASK_ID_DISTRIB_PATH";       
    const wchar_t c_spE2S_LSTaskDeadline[]=L"TASK_ID_DEADLINE";       
    
    const wchar_t c_spE2S_SSNFChecksum[]=L"SSNFChecksum"; // BOOL_T

    // этот параметр пишется в KLPRSS_PRODUCT_CORE / KLPRSS_VERSION_INDEPENDENT / c_spE2SSubscrSection
    const wchar_t c_spLocalWritableFolder[]=L"LocalWritableFolder";	//STRING_T

    // ! Параметры подписки для задачи или политики
    const wchar_t c_spE2SSubscrToStore[]=L"SubscrToStore";          //STRING_T
    const wchar_t c_spE2SSubscrNotToStore[]=L"SubscrNotToStore";    //STRING_T
    const wchar_t c_spE2SEventsForEL[]=L"EventsForEL";    //STRING_T
    const wchar_t c_spE2SFinalEventId[]=L"FinalEventId";    //INT_T
    const wchar_t c_spE2SFinalNonstoredEventId[]=L"FinalNonstoredEventId";    //INT_T
    const wchar_t c_spE2SFinalLoggedEventId[]=L"FinalLoggedEventId";    //INT_T
    const wchar_t c_spE2SFinalLoggedNonstoredEventId[]=L"FinalLoggedNonstoredEventId";    //INT_T
    const wchar_t c_spE2SNotificationSource[]=L"NotificationSource";          //PARAMS_T
    const wchar_t c_spE2SNFSourceProcessed[]=L"NFSourceProcessed";          //BOOL_T
    const wchar_t c_spE2SCounterIdToStore[]=L"CounterIdToStore";          //STRING_T
    const wchar_t c_spE2SCounterIdNotToStore[]=L"CounterIdNotToStore";    //STRING_T

	/*!
        Событие c_EventE2SDataChanged публикуется при изменении информации о подписках на события, которые надо отправлять на  сервер.
        Параметры отсутствуют.
    */
	const wchar_t c_EventE2SDataChanged[]=L"KLEVP_E2SDataChanged";
}

#endif // _EVP_SUBSCRIPTION_COMMON_H_FBC25167_F922_4fc7_92ED_EE2B48F1E582__

// Local Variables:
// mode: C++
// End:
