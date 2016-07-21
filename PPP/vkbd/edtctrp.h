#ifndef __EDTCTRP_H
#define __EDTCTRP_H
//------------------------------------------------------------------------------------------------------------------------

BOOL ValidateWindowClass(HWND hWnd);
HRESULT PatchEditControl(HWND hWnd, PWCHAR wcPatchStr);
HRESULT PatchEditControl(HWND hWnd, WCHAR wc);

//------------------------------------------------------------------------------------------------------------------------
#endif