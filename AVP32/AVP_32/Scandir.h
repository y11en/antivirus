
class CScanDir{
public:
	CScanDir(const char* dirName);
	~CScanDir();
	LRESULT Dispatch(CWnd* pWnd, int* killScan, DWORD MFlags, BOOL recurse);
protected:
	CString Path;
	CPtrList DirList;
};
