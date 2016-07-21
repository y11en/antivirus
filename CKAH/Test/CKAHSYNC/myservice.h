// myservice.h

#include "ntservice.h"

class CMyService : public CNTService
{
public:
	CMyService();
	~CMyService();
	virtual BOOL OnInit();
    virtual void Run();
    virtual void OnStop();

	void PerformBasesMerge ();

protected:
	HANDLE m_hStopEvent;
};
