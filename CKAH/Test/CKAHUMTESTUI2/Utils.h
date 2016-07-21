#if !defined(AFX_UTILS_H__629AD92C_4BC0_4EF6_99A1_A1278887F3B5__INCLUDED_)
#define AFX_UTILS_H__629AD92C_4BC0_4EF6_99A1_A1278887F3B5__INCLUDED_

#include <CKAH/ckahum.h>
#include <stuff/StdString.h>

void RefreshPortList (CListBox &ListCtrl, CKAHFW::HRULEPORTS hPorts);
void AddPort (CListBox &ListCtrl, CKAHFW::HRULEPORTS hPorts, CWnd *pParent);
void EditPort (CListBox &ListCtrl, CKAHFW::HRULEPORTS hPorts, CWnd *pParent);
void DeletePort (CListBox &ListCtrl, CKAHFW::HRULEPORTS hPorts);

void RefreshAddressList (CListBox &ListCtrl, CKAHFW::HRULEADDRESSES hAddresses);
void AddAddress (CListBox &ListCtrl, CKAHFW::HRULEADDRESSES hAddresses, CWnd *pParent);
void EditAddress (CListBox &ListCtrl, CKAHFW::HRULEADDRESSES hAddresses, CWnd *pParent);
void DeleteAddress (CListBox &ListCtrl, CKAHFW::HRULEADDRESSES hAddresses);

void PopupWindow (CWnd *pWnd, bool bMakeTopmostOnFail = true);

DWORD GetLongPathName (CStdStringW strShortPath, CStdStringW &strLongPath);

#endif // !defined(AFX_UTILS_H__629AD92C_4BC0_4EF6_99A1_A1278887F3B5__INCLUDED_)
