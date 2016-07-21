#if !defined(GENERIC_STRATEGY_H_INCLUDED)
#define GENERIC_STRATEGY_H_INCLUDED
#include "proc_strategy.h"
#include <vector>
///////////////////////////////////////////////////////////////////////////////
namespace http
{
///////////////////////////////////////////////////////////////////////////////
struct preprocessing;
///////////////////////////////////////////////////////////////////////////////
class generic_strategy
{
public:
	explicit generic_strategy(preprocessing*);

	message* extract_message(analyzer& anlz, msg_direction direction, bool is_ssl, bool force);
	preprocessing* pre_process();
	void pre_process(preprocessing*);
	message* replace(std::auto_ptr<message>);
	void reprocess();
	bool is_forced() const;
	bool need_skip() const;
	message const* current_message() const;
	generic_strategy(generic_strategy const&);
	~generic_strategy();
private:
	preprocessing*				preprocess_;//! preprocessing strategy
	std::auto_ptr<message>		msg_;		//! current processing message
	std::vector<char>			url_;		//! last requested URI
	process_state				state_;		//! processing state
	bool						forced_;	//! occure forced processing
};
///////////////////////////////////////////////////////////////////////////////
}//namesapce http
///////////////////////////////////////////////////////////////////////////////
#endif//!defined(GENERIC_STRATEGY_H_INCLUDED)