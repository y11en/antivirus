// HTTPUnitTests.cpp : Defines the entry point for the console application.

#define PR_IMPEX_DEF

// AVP Prague stamp begin( Interface version,  )
#define Task_VERSION ((tVERSION)1)
// AVP Prague stamp end



#pragma warning(disable: 4786)

// AVP Prague stamp begin( Header includes,  )
#include <Prague/prague.h>
#include <Prague/pr_vtbl.h>
#include <Prague/iface/i_reg.h>
#include <Prague/iface/i_root.h>






#include <Windows.h>
#include <tchar.h>

#include "test.hpp"

/*
#include <memory>
#include <iosfwd>
#include <sstream>
#include <string>
#include <deque>
#include <prague.h>
#include <iface/i_io.h>
#include <pr_sys.h>
#include <avpsize_t.h>
#include <process.h>

#include <iostream>
#include <fstream>

#include <tchar.h>
#include "_aux.h"

#include <structs/s_httpscan.h>
#include <structs/s_urlflt.h>
#include <plugin/p_smtpprotocoller.h>

#include "config.h"
*/


hROOT g_root;
tPROPID  g_propVirtualName = 0;
tPROPID  g_propMailerPID = 0;
tPROPID  g_propMessageIsIncoming = 0;
tPROPID  g_propMsgDirection = 0;
tPROPID  g_propStartScan = 0;





template <EngineType T> void test_run ()
{

//---------------------------------------------------------------------------------------------------------------------//
//------------------------------------------------ PURE HTTP ----------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------//

/*	*/TestHTTP<T>::test_syntax("test01");
	/*TestHTTP<T>::test_syntax("test02");
	TestHTTP<T>::test_syntax("test03");//ошибка с bytes-range
	TestHTTP<T>::test_syntax("test04");
	TestHTTP<T>::test_syntax("test05");//check stream - подправить после фикса chunked в streaming
	TestHTTP<T>::test_syntax("test06");//check stream
	TestHTTP<T>::test_syntax("test07");//check stream
	TestHTTP<T>::test_syntax("test08");
	TestHTTP<T>::test_syntax("test09");//different behaviour
	TestHTTP<T>::test_syntax("test10");//check stream
	TestHTTP<T>::test_syntax("test14"); //передается пустой файл на проверку - не особый баг, но и смысла мало.
*/
	//TestHTTP<T>::test_syntax("test15");
	//TestHTTP<T>::test_syntax("test16");
	//TestHTTP<T>::test_syntax("test17");
	//TestHTTP<T>::test_syntax("test18");//mime, не работает
/*
*/
	//POST
	//TestHTTP<T>::test_syntax("test30");
	//TestHTTP<T>::test_syntax("test31"); // бага с chunked в post
	//добавить MIME-кодирование после фикса, щас все равно не работает
	//("test32"); - использование 100 Continue - отдельный тест (баг)
	//TestHTTP<T>::test_data_after_data("test32");

	//PUT
	//TestHTTP<T>::test_syntax("test35");
	//TestHTTP<T>::test_syntax("test36"); // бага с chunked в put
	//добавить MIME-кодирование после фикса, щас все равно не работает
	///("test37"); - использование 100 Continue - отдельный тест (баг)
	//TestHTTP<T>::test_data_after_data("test37");

	//special
	//TestHTTP<T>::test_one_byte("test38");
	// - длинный заголовок, травим по одному байту, смотрим, не пропадают ли символы перевода строки (ТТ 25658)


	//добавить использование 100 ОК
	//добавить некорректную длину chunk-а
	//добавить комменты
	//добавить неверные комменты

	//TestHTTP<T>::test_pipelining("test11"); // баг
	//сделать замес команды HEAD в pipelining (когда его починят), наверняка будет лажа
	//TestHTTP<T>::test_caching_response_chunks("test12");//stream - bug (воспроизводится также тулзой)

	//TestHTTP<T>::test_caching_response_normal("test13");//ok
	//TestHTTP<T>::test_cannot_detect_length("test18");//bug, сделать для стрима, должно работать

	//TestHTTP<T>::test_chunks_cases ("test40");//long chunk -тот же баг, что и предыдущий,
	//в стриме не отдает нулевой чанк
	//TestHTTP<T>::test_packet_after_packet ("test41");//incorrect chunk comment - зависает на разборе,
	//надо подумать

	//OPTIONS
	//TestHTTP<T>::test_syntax("test50");
	//TestHTTP<T>::test_packet_after_packet("test50");
	//TestHTTP<T>::test_syntax("test51");
	//TestHTTP<T>::test_packet_after_packet("test51");
	//TestHTTP<T>::test_syntax("test52");
	//TestHTTP<T>::test_packet_after_packet("test52");
	//TestHTTP<T>::test_syntax("test53");
	//TestHTTP<T>::test_packet_after_packet("test53");// бага с chunked в options
	//добавить MIME-кодирование после фикса, щас все равно не работает

	//HEAD
	//TestHTTP<T>::test_syntax("test101");
	//TestHTTP<T>::test_packet_after_packet("test101");
	//TestHTTP<T>::test_syntax("test102");
	//TestHTTP<T>::test_packet_after_packet("test102");

	//DELETE
	//TestHTTP<T>::test_syntax("test120"); TestHTTP<T>::test_packet_after_packet("test120");
	//TestHTTP<T>::test_syntax("test121"); TestHTTP<T>::test_packet_after_packet("test121");//check stream
	//TestHTTP<T>::test_syntax("test122"); TestHTTP<T>::test_packet_after_packet("test122");

	//TRACE
	//TestHTTP<T>::test_syntax("test130"); TestHTTP<T>::test_packet_after_packet("test130");




//------------------------------------------------------------------------------------------------------------------------//
//-------------------------------------- WEB DAV, others supported extensions --------------------------------------------//
//------------------------------------------------------------------------------------------------------------------------//

/*	
	TestHTTP<T>::test_method_supported ("UNLOCK");
	TestHTTP<T>::test_method_supported ("PROPFIND");
	TestHTTP<T>::test_method_supported ("PROPPATCH");
	TestHTTP<T>::test_method_supported ("MKCOL");
	TestHTTP<T>::test_method_supported ("COPY");//забыли добавить
	TestHTTP<T>::test_method_supported ("MOVE");
	TestHTTP<T>::test_method_supported ("LOCK");
	TestHTTP<T>::test_method_supported ("UNLOCK");
	TestHTTP<T>::test_method_supported ("BCOPY");
	TestHTTP<T>::test_method_supported ("BDELETE");
	TestHTTP<T>::test_method_supported ("BMOVE");
	TestHTTP<T>::test_method_supported ("BPROPFIND");
	TestHTTP<T>::test_method_supported ("BPROPPATCH");
	TestHTTP<T>::test_method_supported ("NOTIFY");
	TestHTTP<T>::test_method_supported ("POLL");
	TestHTTP<T>::test_method_supported ("SEARCH");
	TestHTTP<T>::test_method_supported ("SUBSCRIBE");
	TestHTTP<T>::test_method_supported ("UNSUBSCRIBE");
	TestHTTP<T>::test_method_supported ("X-MS-ENUMATTS");
	TestHTTP<T>::test_method_supported ("REPORT");
	TestHTTP<T>::test_method_supported ("CHECKOUT");
	TestHTTP<T>::test_method_supported ("CHECKIN");
	TestHTTP<T>::test_method_supported ("UNCHECKOUT");
	TestHTTP<T>::test_method_supported ("VERSION-CONTROL");
	TestHTTP<T>::test_method_supported ("MKWORKSPACE");//забыли добавить
	TestHTTP<T>::test_method_supported ("UPDATE");
	TestHTTP<T>::test_method_supported ("LABEL");
	TestHTTP<T>::test_method_supported ("MERGE");
	TestHTTP<T>::test_method_supported ("BASELINE-CONTROL");
	TestHTTP<T>::test_method_supported ("MKACTIVITY");


	TestHTTP<T>::test_method_unsupported_middle ("TEST_METHOD"); //глючит, отжирает последний символ

	//TestHTTP<T>::test_method_unsupported_start ("TEST_METHOD");
	//пока непонятно, как делать этот тест, сейчас анализатор говорит "не мое" и весь трафик идет мимо, баг.

	//TestHTTP<T>::test_litter ("litter"); //глючит, также как и с неизвестным методом
	

	//TestHTTP<T>::test_request_filters ("test210");
	//TestHTTP<T>::test_request_filters ("test211");
	//TestHTTP<T>::test_request_filters ("test212");
	//TestHTTP<T>::test_request_filters ("test213");
	//TestHTTP<T>::test_request_filters ("test214");
	//ips
	//TestHTTP<T>::test_request_filters ("test215");
	//TestHTTP<T>::test_request_filters ("test216");
	//TestHTTP<T>::test_request_filters ("test220");
	//TestHTTP<T>::test_request_filters ("test221");
	//TestHTTP<T>::test_request_filters ("test222");

	//commands, others than GET
	//TestHTTP<T>::test_request_filters ("test225");
	//TestHTTP<T>::test_request_filters ("test226");
	//TestHTTP<T>::test_request_filters ("test227");
	//TestHTTP<T>::test_request_filters ("test228");
	//TestHTTP<T>::test_request_filters ("test229");
	//TestHTTP<T>::test_request_filters ("test230"); //unknown method - пока не работает, соответственно, URI не выделит

	//TestHTTP<T>::test_request_filters_deny ("test202", cEngineBase::filter_WEBAV);
	//TestHTTP<T>::test_request_filters_deny ("test202", cEngineBase::filter_APHI);
	//TestHTTP<T>::test_request_filters_deny ("test202", cEngineBase::filter_PARC);
	//TestHTTP<T>::test_request_filters_deny ("test203", cEngineBase::filter_AD); //image
	//TestHTTP<T>::test_request_filters_deny ("test204", cEngineBase::filter_AD); //flash
	//добавит pipelining на антибаннер, когда починят, щас один хрен глючит, никакого смысла нет

	//проверки вердиктов ODS
	//TestHTTP<T>::test_request_avs_exchange ("test300");
	//TestHTTP<T>::test_request_avs_interrupt ("test301");
	//TestHTTP<T>::test_request_avs_interrupt_chunked ("test301");//stream - хрень с чанками

	//нужна проверка gzip - уже есть баг 24724, аналогично и с deflate, и с compress
	//на проверке с буферизацией все распаковки сделает третий движок и будет хорошо
	//на потоковой весь кодированный траффик будет литься в движок, а он, видимо, такие вещи делать не умеет пока
	//и, возможно, и не будет уметь

	//special cases
	//TestHTTP<T>::test_syntax("test400");
	//TestHTTP<T>::test_syntax("test401");
	//TestHTTP<T>::test_syntax("test402");//ошибка -  таб может использоваться в качестве разделителя
	//TestHTTP<T>::test_syntax("test403");//комменты в чанках
	//TestHTTP<T>::test_syntax("test404");//a single LF as a line terminator and ignore the leading CR
	//TestHTTP<T>::test_syntax("test405");//additional CRLF in POST request

	//TestHTTP<T>::test_skip_that_no_avs("test500");
	//TestHTTP<T>::test_skip_that_no_traffic_delay("test501");
	//TestHTTP<T>::test_skip_that_no_traffic_delay_chunked("test502");// - бага 26341 исправлено

	//TestHTTP<T>::test_skip_message_on_request("test505");//winamp
	
	//следующие 2 теста также проверяют отказ протоколлера от траффика и переход в режим детектирования
	//TestHTTP<T>::test_skip_message_on_response("test506");
	//TestHTTP<T>::test_skip_message_on_response_chunked("test506");

	//new functionality
	//TestHTTP<T>::test_async_data_send_runner("test13a");
//	TestHTTP<T>::test_async_long_message_normal_and_timeout();
//	TestHTTP<T>::test_async_long_message_chunked_and_timeout();

	//TestHTTP<T>::test_header_timeout();

	//TestHTTP<T>::test_connections();
*/
}

int _tmain(int argc, _TCHAR* argv[])
{

	srand(time(0));

	static char const* plugins[] = {"nfio.ppl", "prutil.ppl", "tempfile.ppl", /*"httpanlz.ppl", "thpimpl.ppl",*/ 0};
	prague::init_prague init(plugins);
	prague::pr_remote_api().GetRoot(&g_root);

	//test_run<eng_stream>();
	test_run<eng_third>();
}