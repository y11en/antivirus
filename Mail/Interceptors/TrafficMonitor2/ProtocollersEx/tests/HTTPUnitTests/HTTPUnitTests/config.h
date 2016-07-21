#if !defined(__CONFIG_H)
#define __CONFIG_H

class Configure
{
public:

	static Configure& GetConfiguration()
	{
		static Configure config;
		return config;
	}

	bool ReconnectAfterEachPacket ()
	{
		return bReconnectAfterEachPacket;
	}

	void ReconnectAfterEachPacket (bool value)
	{
		bReconnectAfterEachPacket = value;
	}

	int MaximumChunkSize ()
	{
		return nMaximumChunkSize;
	}

	void MaximumChunkSize (int value)
	{
		nMaximumChunkSize = value;
	}

	int TimeoutBetweenChunks ()
	{
		return nTimeoutBetweenChunks;
	}

	void TimeoutBetweenChunks (int value)
	{
		nTimeoutBetweenChunks = value;
	}

private:
	Configure (): bReconnectAfterEachPacket (false), nMaximumChunkSize (80),  nTimeoutBetweenChunks(0) {};

	bool bReconnectAfterEachPacket;
	int nMaximumChunkSize;
	int nTimeoutBetweenChunks;
};














#endif