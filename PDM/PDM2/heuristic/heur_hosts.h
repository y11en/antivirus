#ifndef _cEventHandlerHeurHosts__H_
#define _cEventHandlerHeurHosts__H_

#include "../eventmgr/include/eventhandler.h"
#include "../eventmgr/include/lock.h"


class cEventHandlerHeurHosts : public cEventHandler
{
public:
	cEventHandlerHeurHosts();
	virtual ~cEventHandlerHeurHosts();

	bool Init();

	_IMPLEMENT( OnFileCloseModifiedPost );
	_IMPLEMENT( OnHostsAdded );

private:
	cLock  m_lock;

	char*  m_pHosts;
	size_t m_nHostsSize;
	char*  m_pHostsParsed;
	size_t m_nHostsSizeParsed;

	bool GatherHostsFile(char** ppHosts, size_t* pHostsSize, char** ppHostsParsed, size_t* pHostsSizeParsed);
	bool ParseHostsFile(char* pBuffer, size_t dwBufferSize, char* pParsedBuffer, size_t* pdwParseSize);
};

#endif // _EventHandlerPDM__H_
