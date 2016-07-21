// ComAvpGui.h: interface for the CGUIControler class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GUICONTROLER_H__9DD1E27C_F8EA_46F1_B23E_DB64FF4A6DC1__INCLUDED_)
#define AFX_GUICONTROLER_H__9DD1E27C_F8EA_46F1_B23E_DB64FF4A6DC1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

bool GUIControllerInit(tPTR hInstance);
bool GUIControllerRegister();
void GUIControllerUnregister();
void GUIControllerDone();
void GUIControllerEvent(hOBJECT hObj, CRootItem * pRoot, cGuiEvent * pParams);

void GUIControllerAddRoot(CRootItem * pRoot, LPCWSTR strObject);
void GUIControllerRemoveRoot(CRootItem * pRoot);
CRootItem * GUIControllerGetRoot(LPCWSTR strObject);

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

#endif // !defined(AFX_GUICONTROLER_H__9DD1E27C_F8EA_46F1_B23E_DB64FF4A6DC1__INCLUDED_)
