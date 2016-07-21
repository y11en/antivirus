#if !defined(__QUERY_H)
#define __QUERY_H


template <tDWORD pos_rel_arg, class T1, class T2> class CommonQuery
{

public:

	CommonQuery (): bActive (false)
	{

	};

	virtual ~CommonQuery ()
	{

	}

	void Activate ()
	{
		hRequestReceived = CreateEvent (NULL, false, false, NULL);
		hPositiveEvent = CreateEvent (NULL, false, false, NULL);
		hNegativeEvent = CreateEvent (NULL, false, false, NULL);
		resEvents[0] = hPositiveEvent;
		resEvents[1] = hNegativeEvent;

		bActive = true;
	};

	void Deactivate ()
	{
		CloseHandle (hRequestReceived);
		CloseHandle (hPositiveEvent);
		CloseHandle (hNegativeEvent);

		bActive = false;
	};

	void GoOn (tDWORD err)
	{
		err == pos_rel_arg ? SetEvent (hPositiveEvent) : SetEvent(hNegativeEvent);
	};

	void GetRequest (T1& arg1, T2& arg2)
	{
		WaitForSingleObject(hRequestReceived, INFINITE);

		arg1 = m_arg1;
		arg2 = m_arg2;

	}

protected:

	bool bActive;
	HANDLE hRequestReceived;
	HANDLE hPositiveEvent;
	HANDLE hNegativeEvent;
	HANDLE resEvents[2];

	T1 m_arg1;
	T2 m_arg2;

};



#endif