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
	Configure (): nMaximumChunkSize (80),  nTimeoutBetweenChunks(0) {};

	int nMaximumChunkSize;
	int nTimeoutBetweenChunks;
};














#endif