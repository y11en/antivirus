// UniRoot.cpp: implementation of the CUniRoot class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "UniRoot.h"
#include "SysSink.h"
//#include <pthread.h>

//////////////////////////////////////////////////////////////////////

#ifdef _DEBUG
	#pragma comment (lib, "QtGuid4.lib")
	#pragma comment (lib, "QtCored4.lib")
#else // _DEBUG
	#pragma comment (lib, "QtGui4.lib")
	#pragma comment (lib, "QtCore4.lib")
#endif // _DEBUG

//////////////////////////////////////////////////////////////////////

#define tMAKEWORD(a, b)      ((tWORD)(((tBYTE)(a)) | ((tWORD)((tBYTE)(b))) << 8))
#define tMAKELONG(a, b)      ((tLONG)(((tWORD)(a)) | ((tDWORD)((tWORD)(b))) << 16))
#define tLOWORD(l)           ((tWORD)(l))
#define tHIWORD(l)           ((tWORD)(((tDWORD)(l) >> 16) & 0xFFFF))
#define tLOBYTE(w)           ((tBYTE)(w))
#define tHIBYTE(w)           ((tBYTE)(((tWORD)(w) >> 8) & 0xFF))

//////////////////////////////////////////////////////////////////////
// CUniRoot

SINK_MAP_BEGINEX(CUniRoot)
	SINK_DYNAMIC_EX("shelltree",  GUI_ITEMT_TREE,     CShellTreeSink())
SINK_MAP_END(CRootItem)

//////////////////////////////////////////////////////////////////////

#define UM_ONEVENT        (QEvent::Type)(QEvent::User + 100)
#define UM_ONEVENTQUEUE   (QEvent::Type)(QEvent::User + 101)
#define UM_ONACTION       (QEvent::Type)(QEvent::User + 102)

//////////////////////////////////////////////////////////////////////

CUniRoot::CUniRoot(tDWORD nFlags, CRootSink * pSink) :
	CRootItem(nFlags, pSink),
	m_nTimerSpin(0),
	m_pDesktopDC(NULL),
	m_pPenGrayed(NULL),
	m_pPenHotlight(NULL),
	m_pIconProvider(NULL)
{
}

CUniRoot::~CUniRoot()
{
}

bool CUniRoot::ActivateData(bool bActive)
{
	if( bActive )
	{
		static int     g_argc = 0;
		static char *  g_argv[] = {""};
		
		// TEMP !!!!!!!  QCoreApplication uses static data !!!!!!!!
		m_app = new QApplication(g_argc, g_argv);
		moveToThread(m_app->thread());

		m_pDesktopDC = new QPainter(QApplication::desktop());
		m_pPenGrayed = new QPen(QColor(128, 128, 128));
		m_pPenHotlight = new QPen(QColor(248,151,48));
		m_pIconProvider = new QFileIconProvider();
		
		m_idTimerRefresh = startTimer(TIMER_REFRESH_INT);
		
		OnActivate(this, true);
		return true;
	}

	OnActivate(this, false);

	CRootItem::Clear();

	killTimer(m_idTimerRefresh);
	
	if( m_pIconProvider ) delete m_pIconProvider, m_pIconProvider = NULL;
	if( m_pPenHotlight )  delete m_pPenHotlight,  m_pPenHotlight = NULL;
	if( m_pPenGrayed )    delete m_pPenGrayed,    m_pPenGrayed = NULL;
	if( m_pDesktopDC )    delete m_pDesktopDC,    m_pDesktopDC = NULL;
	
	moveToThread(NULL);
	delete m_app;

	return true;
}

bool CUniRoot::IsOwnThread()
{
	QThread * pThr = thread();
	if( !pThr )
		return false;
	return QThread::currentThread() == pThr;
}

void CUniRoot::MessageLoop(CItemBase * pItem)
{
	QEventLoop eventLoop;

	while( m_bActive )
	{ 
		eventLoop.processEvents(QEventLoop::WaitForMoreEvents|QEventLoop::ProcessEventsFlag(QEventLoop::DeferredDeletion));

		if( m_bRefreshRequest )
		{
			if( !pItem )
			{
				RefreshDialog(NULL);
				m_bRefreshRequest = false;
			}
			else if( pItem->m_nFlags & STYLE_TOP_ITEM )
				((CDialogItem *)pItem)->Close();
		}

		if( pItem && (!pItem->m_pRoot) )
			break;
	}
}

//////////////////////////////////////////////////////////////////////

class CUniEvent : public QEvent
{
public:
	CUniEvent(Type type, QSemaphore * pSync = NULL, tPTR wParam = NULL, tPTR lParam = NULL) :
		QEvent(type), m_pSync(pSync), m_wParam(wParam), m_lParam(lParam)
	{
	}

	tPTR         m_wParam;
	tPTR         m_lParam;
	QSemaphore * m_pSync;
};

void CUniRoot::SendEvent(tSyncEventData * pEvent)
{
	if( !thread() )
		return;

	if( !pEvent->m_bOwnThread && (pEvent->nFlags & EVENT_SEND_SYNC) && !pEvent->nTimeout )
	{
		QSemaphore _waiter(1); _waiter.acquire();
		QCoreApplication::postEvent(this, new CUniEvent(UM_ONEVENT, &_waiter, (tPTR)pEvent));
		_waiter.acquire();
	}
	else
		CRootItem::SendEvent(pEvent);
}

void CUniRoot::PostProcessEventQueue()
{
	QSemaphore _waiter(1); _waiter.acquire();
	QCoreApplication::postEvent(this, new CUniEvent(UM_ONEVENTQUEUE, &_waiter));
	_waiter.acquire();
}

void CUniRoot::PostAction(tWORD nActionId, CItemBase * pItem, void * pParam, bool bSync)
{
	bool bItem = !!pItem;
	if( bItem && !bSync )
		pItem->AddRef();
	
	tPTR wParam = (tPTR)tMAKELONG(nActionId, tMAKEWORD(bItem, bSync));
	tPTR lParam = bItem ? (tPTR)pItem : (tPTR)pParam;
	
	if( bSync )
	{
		if( IsOwnThread() )
			QCoreApplication::sendEvent(this, &CUniEvent(UM_ONACTION, NULL, wParam, lParam));
		else
		{
			QSemaphore _waiter(1); _waiter.acquire();
			QCoreApplication::postEvent(this, new CUniEvent(UM_ONACTION, &_waiter, wParam, lParam));
			_waiter.acquire();
		}
	}
	else
		QCoreApplication::postEvent(this, new CUniEvent(UM_ONACTION, NULL, wParam, lParam));
}

bool CUniRoot::SetAnimationTimer(tDWORD nTimeout)
{
	if( nTimeout )
		m_idTimerAnimation = startTimer(nTimeout);
	else
		killTimer(m_idTimerAnimation);
	return true;
}

//////////////////////////////////////////////////////////////////////

bool CUniRoot::event(QEvent * e)
{
	CUniEvent * pUniEvt = (CUniEvent *)e;
	
	switch(e->type())
	{
	case UM_ONEVENT:
		OnEventHandler(pUniEvt->m_wParam, pUniEvt->m_lParam);
		if( pUniEvt->m_pSync )
			pUniEvt->m_pSync->release();
		break;

	case UM_ONEVENTQUEUE:
		ProcessEventQueue();
		break;
	
	case UM_ONACTION:
		OnActionHandler(pUniEvt->m_wParam, pUniEvt->m_lParam);
		if( pUniEvt->m_pSync )
			pUniEvt->m_pSync->release();
		break;

	default:
		return QObject::event(e);
	}
	
	return true;
}

void CUniRoot::OnEventHandler(tPTR wParam, tPTR lParam)
{
	CRootItem::ProcessEvent((tSyncEventData*)wParam);
}

void CUniRoot::OnActionHandler(tPTR wParam, tPTR lParam)
{
	bool bItem = !!tLOBYTE(tHIWORD(wParam));
	bool bSync = !!tHIBYTE(tHIWORD(wParam));
	tWORD nActionId = (tDWORD)(wParam);
	CItemBase * pItem = bItem ? (CItemBase *)lParam : NULL;
	void * pParam = bItem ? NULL : (void *)lParam;
	
	if( !pItem || pItem->m_pRoot )
		OnAction((tDWORD)nActionId, pItem, pParam);
	
	if( pItem && !bSync )
		pItem->Release();
}

void CUniRoot::timerEvent(QTimerEvent * e)
{
	if( e->timerId() == m_idTimerRefresh || e->timerId() == m_idTimerAnimation )
		OnTimer(e->timerId());
	else
		QObject::timerEvent(e);
}

//////////////////////////////////////////////////////////////////////

void CUniRoot::OnGetSysDefFont(tString& strFont)
{
	strFont = "Tahoma";
}

void CUniRoot::OnGetModulePath(tObjPath& strModulePath)
{
//	const QString& sqAppPath = QCoreApplication::applicationFilePath();
	// MBI !!!!!!!!!!!!
}

HDC CUniRoot::GetDesktopDC()
{
	return (HDC)m_pDesktopDC;
}

bool CUniRoot::BrowseFile(CItemBase * pParent, LPCSTR strTitle, LPCSTR strFileType, tObjPath& strPath, bool bSave)
{
	QString qsTitle; _2QS(strTitle, qsTitle);
	QString qsFileName; _2QS(strPath, qsFileName);
	
	QWidget * qParent = GetParentQW(pParent);
	if( !qParent )
		qParent = QApplication::desktop();
	
	// "All Files (*);;Text Files (*.txt)"
	if( bSave )
		qsFileName = QFileDialog::getSaveFileName(qParent, qsTitle, qsFileName, tr("All Files (*);;"));
	else
		qsFileName = QFileDialog::getOpenFileName(qParent, qsTitle, qsFileName, tr("All Files (*);;"));

	if( qsFileName.isEmpty() )
		return false;

	_2PS(qsFileName, strPath);
	return true;
}

bool CUniRoot::BrowseApp(CItemBase * pItem, tObjPath& strAppPath)
{
	return BrowseFile(pItem, LoadLocString(TOR_tString, "BrowseForExecutable"), "exe", strAppPath);
}

bool CUniRoot::GetObjectInfo(CObjectInfo * pObjInfo)
{
	if( CRootItem::GetObjectInfo(pObjInfo) )
		return true;

	tDWORD& nMask = pObjInfo->m_nQueryMask;
	tDWORD& nObjType = pObjInfo->m_nType;

	QString qs; _2QS(pObjInfo->m_strName, qs);
	QFileInfo _fileInfo(qs);
		
	if( nObjType == SHELL_OBJTYPE_UNKNOWN )
	{
		if( _fileInfo.isRoot() )
			nObjType = SHELL_OBJTYPE_DRIVE;
		else if( _fileInfo.isFile() )
			nObjType = SHELL_OBJTYPE_FILE;
		else if( _fileInfo.isDir() )
			nObjType = SHELL_OBJTYPE_FOLDER;
	}
	
	_2PS(_fileInfo.fileName(), pObjInfo->m_strShortName);

	if( (nMask & OBJINFO_ICON) && m_pIconProvider )
	{
		if( !pObjInfo->m_strName.compare(0, 10, "%personal%") )
			pObjInfo->m_pIcon = CreateIcon((HICON)&m_pIconProvider->icon(QFileIconProvider::Desktop));
		else if( nObjType == SHELL_OBJTYPE_MYCOMPUTER )
			pObjInfo->m_pIcon = CreateIcon((HICON)&m_pIconProvider->icon(QFileIconProvider::Computer));
		else
			pObjInfo->m_pIcon = CreateIcon((HICON)&m_pIconProvider->icon(_fileInfo));
	}
	
	pObjInfo->m_strDisplayName = pObjInfo->m_strName;
	return true;
}

void CUniRoot::_InitDrivesList()
{
	m_setDrives = QDir::drives();
}

bool CUniRoot::EnumDrives(tString& strDrive, tDWORD * pCookie)
{
	cAutoCS locker(m_accessor, true);

	tDWORD pos = *pCookie;
	if( !pos )
		_InitDrivesList();

	if( pos >= m_setDrives.size() )
		return false;

	(*pCookie)++;

	const QFileInfo& pFileInfo = m_setDrives[pos];
	if( pFileInfo.isSymLink() )
		return EnumDrives(strDrive, pCookie);
	
	_2PS(pFileInfo.path(), strDrive);
	return true;
}

//////////////////////////////////////////////////////////////////////

void CRootItem::InitInstance(HINSTANCE hInstance) {}
void CRootItem::ExitInstance() {}

struct BLOB
{
	BLOB () : cbSize (0), pBlobData(0) {}
	~BLOB() { if (cbSize && pBlobData) {free(pBlobData); pBlobData=0; cbSize=0;}}
	int cbSize;
	BYTE* pBlobData;
};

static pthread_key_t g_TlsBuff;
static pthread_once_t theKeyOnce = PTHREAD_ONCE_INIT;

void makeKey()
{
  pthread_key_create ( &g_TlsBuff, NULL );
}

inline BYTE* GetBuff(DWORD len)
{
	pthread_once(&theKeyOnce, makeKey );
	BLOB* pData = (BLOB*)pthread_getspecific ( g_TlsBuff );
	if( !pData ) {
		pData = new BLOB;
		if( !pData )
			return NULL;
		pthread_setspecific(g_TlsBuff, pData);
		
	}
	
	if( pData->cbSize < len ) 
		pData->pBlobData = (BYTE*)realloc(pData->pBlobData, pData->cbSize = len);
	
	return pData->pBlobData;
}



void CRootItem::ThreadDetach() 
{
	pthread_once(&theKeyOnce, makeKey );
	BLOB* pData = (BLOB*)pthread_getspecific(g_TlsBuff);
	if(!pData)
		return;
	delete pData;
	pthread_setspecific(g_TlsBuff, 0);
}

void CRootItem::AdjustMetrics(CItemBase* pItem) {}

//////////////////////////////////////////////////////////////////////

CRootItem * CRootItem::CreateInstance(tDWORD nFlags, CRootSink * pSink)
{
	CRootItem * pRoot = new CUniRoot(nFlags, pSink);
	if( !pRoot )
		return NULL;
	pRoot->OnInit();
	return pRoot;
}

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
