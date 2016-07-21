#if !defined(_SKIP_APPLICATION_INCLUDED_H_)
#define _SKIP_APPLICATION_INCLUDED_H_
#pragma warning(disable:4786)
#include "action.h"
#include <boost/utility.hpp>
///////////////////////////////////////////////////////////////////////////////
namespace http
{
///////////////////////////////////////////////////////////////////////////////
/**
 * \breif	-	îñóùåñòâëÿåò ïğîâåğêó íà ñïåöèôè÷åñêèå ïğèëîæåíèÿ
 *				ÎÃĞÀÍÈ×ÅÍÈÅ ÄÀÍÍÛÉ ÊËÀÑÑ ÍÅ ÌÎÆÅÒ ÁÛÒÜ ÁÀÇÎÂÛÌ, ÒÀÊ ÊÀÊ
 *				Â ÊÎÍÑÒĞÓÊÒÎĞÅ ÂÛÇÛÂÀÅÒÑß ÂÈĞÓÀËÜÍÛÅ ÌÅÒÎÄÛ, ×ÅĞÅÇ ÌÅÕÀÍÈÇÌ
 *				ÄÂÎÉÍÎÉ ÄÈÑÏÅÒ×ÅĞÅÇÀÖÈÈ
 */
class skip_application
		: private boost::noncopyable
		, public action_hierarchy<action_list>
{
public:
	explicit skip_application(message& msg);
	virtual void do_action(http::response_message&);
	virtual void do_action(http::request_message&);
	DEFINE_DEFAULT_ACTION(message)
	bool get() const { return skipped_; }
private:
	typedef bool (skip_application::*unspecified_bool_type)() const;

public:
	operator unspecified_bool_type() const
	{
		return skipped_ ? &skip_application::get: false;
	}
private:
	bool	skipped_;
};
///////////////////////////////////////////////////////////////////////////////
}//namespace http
///////////////////////////////////////////////////////////////////////////////
#endif//!defined(_SKIP_APPLICATION_INCLUDED_H_)