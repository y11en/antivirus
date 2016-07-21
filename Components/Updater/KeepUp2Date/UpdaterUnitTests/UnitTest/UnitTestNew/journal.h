#ifndef JOURNAL_H
#define JOURNAL_H

#include "include/journal_iface.h"

class TestJournalInterface: public KLUPD::JournalInterface
{
public:
	TestJournalInterface():
		retr_result (KLUPD::EVENT_DNS_NAME_RESOLVED),
		upd_result (KLUPD::EVENT_DNS_NAME_RESOLVED),
		rollback_result (KLUPD::EVENT_DNS_NAME_RESOLVED)
	  {

	  }
	virtual void publishMessage(const KLUPD::CoreError &, const NoCaseString &parameter = NoCaseString(), const NoCaseString &parameter2 = NoCaseString())
	{

	};

	virtual void publishRetranslationResult(const KLUPD::CoreError & r)
    {
		retr_result = r;
    }

    virtual void publishUpdateResult(const KLUPD::CoreError & r)
    {
		upd_result = r;
    }

    virtual void publishRollbackResult(const KLUPD::CoreError & r)
    {
		rollback_result = r;
    }

	KLUPD::CoreError GetRetranslationResult ()
	{
		return retr_result;
	}

	KLUPD::CoreError GetUpdateResult ()
	{
		return upd_result;
	}

	KLUPD::CoreError GetRollbackResult ()
	{
		return rollback_result;
	}


private:

	KLUPD::CoreError retr_result;
	KLUPD::CoreError upd_result;
	KLUPD::CoreError rollback_result;

};









#endif