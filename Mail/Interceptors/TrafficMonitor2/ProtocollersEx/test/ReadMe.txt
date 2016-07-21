#include <structs/s_mc.h>
#if 0
			cMCProcessParams params1;
			tERROR error(service_.request(pmc_MAILTASK_PROCESS, proxy_->client_id(), content, &url_[0], &params1));
			if((params1.p_MailCheckerVerdicts & mcv_MESSAGE_DELETED) == mcv_MESSAGE_DELETED)
			{
				prague::IO<char> err_io(prague::create_temp_io());
				vector_ptr html(make_default_html(&url_[0], params1.p_szVirusName.c_str(cCP_ANSI)));
				char const* begin = &(*html)[0];
				char const* end = begin + html->size();
				err_io.insert(err_io.end(), begin, end);
				msg->replace(err_io.release());
			}
			else if(params1.p_MailCheckerVerdicts & (mcv_MESSAGE_AV_INFECTED | mcv_MESSAGE_AV_WARNING))
			{
				prague::IO<char> err_io(prague::create_temp_io());
				vector_ptr html(make_default_html(&url_[0], params1.p_szVirusName.c_str(cCP_ANSI)));
				char const* begin = &(*html)[0];
				char const* end = begin + html->size();
				err_io.insert(err_io.end(), begin, end);
				prague::IO<char>(msg->replace(err_io.release()));
			}
#endif