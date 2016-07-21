#ifndef __NN_Lang__
#define __NN_Lang__

#ifdef __cplusplus
#define NN_INLINE inline
#else
#define NN_INLINE
#endif // __cplusplus

#ifdef __cplusplus
extern "C" {
#endif

// Resource
HGLOBAL
WINAPI
LoadResourceLangA(
    HMODULE hModule,
    LPCSTR lpType,
    LPCSTR lpName,
    WORD wLanguage);
HGLOBAL
WINAPI
LoadResourceLangW(
    HMODULE hModule,
    LPCWSTR lpType,
    LPCWSTR lpName,
    WORD wLanguage);
#ifdef UNICODE
#define LoadResourceLang LoadResourceLangW
#else
#define LoadResourceLang LoadResourceLangA
#endif // !UNICODE

// String
int
WINAPI
LoadStringLangA(
    HINSTANCE hInstance,
    UINT uID,
    LPSTR lpBuffer,
    int nBufferMax,
    WORD wLanguage);
int
WINAPI
LoadStringLangW(
    HINSTANCE hInstance,
    UINT uID,
    LPWSTR lpBuffer,
    int nBufferMax,
    WORD wLanguage);
#ifdef UNICODE
#define LoadStringLang LoadStringLangW
#else
#define LoadStringLang LoadStringLangA
#endif // !UNICODE

// Menu
HMENU
WINAPI
LoadMenuLangA(
    HINSTANCE hInstance,
    LPCSTR lpMenuName,
    WORD wLanguage);
HMENU
WINAPI
LoadMenuLangW(
    HINSTANCE hInstance,
    LPCWSTR lpMenuName,
    WORD wLanguage);
#ifdef UNICODE
#define LoadMenuLang LoadMenuLangW
#else
#define LoadMenuLang LoadMenuLangA
#endif // !UNICODE

// Icon - Cursor
HICON
WINAPI
LoadIconCursorLangA(
    HINSTANCE hInstance,
    LPCSTR lpIconCursorName,
    WORD wLanguage,
    BOOL bIcon);
HICON
WINAPI
LoadIconCursorLangW(
    HINSTANCE hInstance,
    LPCWSTR lpIconCursorName,
    WORD wLanguage,
    BOOL bIcon);
#ifdef UNICODE
#define LoadIconCursorLang LoadIconCursorLangW
#else
#define LoadIconCursorLang LoadIconCursorLangA
#endif // UNICODE

// Icon
HICON
WINAPI
LoadIconLangA(
    HINSTANCE hInstance,
    LPCSTR lpIconName,
    WORD wLanguage);
HICON
WINAPI
LoadIconLangW(
    HINSTANCE hInstance,
    LPCWSTR lpIconName,
    WORD wLanguage);
#ifdef UNICODE
#define LoadIconLang LoadIconLangW
#else
#define LoadIconLang LoadIconLangA
#endif // !UNICODE

// Cursor
HICON
WINAPI
LoadCursorLangA(
    HINSTANCE hInstance,
    LPCSTR lpCursorName,
    WORD wLanguage);
HICON
WINAPI
LoadCursorLangW(
    HINSTANCE hInstance,
    LPCWSTR lpCursorName,
    WORD wLanguage);
#ifdef UNICODE
#define LoadCurosrLang LoadCursorLangW
#else
#define LoadCursorLang LoadCursorLangA
#endif // !UNICODE

// Dialog
HWND
WINAPI
CreateDialogParamLangA(
    HINSTANCE hInstance,
    LPCSTR lpTemplateName,
    HWND hWndParent,
    DLGPROC lpDialogFunc,
    LPARAM dwInitParam,
    WORD wLanguage);
HWND
WINAPI
CreateDialogParamLangW(
    HINSTANCE hInstance,
    LPCWSTR lpTemplateName,
    HWND hWndParent,
    DLGPROC lpDialogFunc,
    LPARAM dwInitParam,
    WORD wLanguage);
#ifdef UNICODE
#define CreateDialogParamLang  CreateDialogParamLangW
#else
#define CreateDialogParamLang  CreateDialogParamLangA
#endif // !UNICODE

#ifdef UNICODE
#define CreateDialogIndirectParamLang  CreateDialogIndirectParamLangW
#else
#define CreateDialogIndirectParamLang  CreateDialogIndirectParamLangA
#endif // !UNICODE

#define CreateDialogLangA(hInstance, lpName, hWndParent, lpDialogFunc, wLanguage) \
CreateDialogParamLangA(hInstance, lpName, hWndParent, lpDialogFunc, 0L, wLanguage)
#define CreateDialogLangW(hInstance, lpName, hWndParent, lpDialogFunc, wLanguage) \
CreateDialogParamLangW(hInstance, lpName, hWndParent, lpDialogFunc, 0L, wLanguage)
#ifdef UNICODE
#define CreateDialogLang  CreateDialogLangW
#else
#define CreateDialogLang  CreateDialogLangA
#endif // !UNICODE

INT_PTR
WINAPI
DialogBoxParamLangA(
    HINSTANCE hInstance,
    LPCSTR lpTemplateName,
    HWND hWndParent,
    DLGPROC lpDialogFunc,
    LPARAM dwInitParam,
    WORD wLanguage);
INT_PTR
WINAPI
DialogBoxParamLangW(
    HINSTANCE hInstance,
    LPCWSTR lpTemplateName,
    HWND hWndParent,
    DLGPROC lpDialogFunc,
    LPARAM dwInitParam,
    WORD wLanguage);
#ifdef UNICODE
#define DialogBoxParamLang  DialogBoxParamLangW
#else
#define DialogBoxParamLang  DialogBoxParamLangA
#endif // !UNICODE

#define DialogBoxLangA(hInstance, lpTemplate, hWndParent, lpDialogFunc, wLanguage) \
DialogBoxParamLangA(hInstance, lpTemplate, hWndParent, lpDialogFunc, 0L, wLanguage)
#define DialogBoxLangW(hInstance, lpTemplate, hWndParent, lpDialogFunc, wLanguage) \
DialogBoxParamLangW(hInstance, lpTemplate, hWndParent, lpDialogFunc, 0L, wLanguage)
#ifdef UNICODE
#define DialogBoxLang  DialogBoxLangW
#else
#define DialogBoxLang  DialogBoxLangA
#endif // !UNICODE

// Functions
// Resource
NN_INLINE
HRSRC
FindResourceExExA(
    HMODULE hModule,
    LPCSTR lpType,
    LPCSTR lpName,
    WORD wLanguage)
{
	if( !wLanguage )
		wLanguage = GetUserDefaultLangID();

	HRSRC hrRes = FindResourceExA(hModule, lpType, lpName, wLanguage);

	if( !hrRes && wLanguage != LANG_USER_DEFAULT )
		hrRes = FindResourceExA(hModule, lpType, lpName , LANG_USER_DEFAULT);

	if( !hrRes )
		hrRes = FindResourceExA(hModule, lpType, lpName , MAKELANGID(LANG_NEUTRAL,SUBLANG_NEUTRAL));

	return hrRes;
}

NN_INLINE
HRSRC
FindResourceExExW(
    HMODULE hModule,
    LPCWSTR lpType,
    LPCWSTR lpName,
    WORD wLanguage)
{
	if( !wLanguage )
		wLanguage = GetUserDefaultLangID();

	HRSRC hrRes = FindResourceExW(hModule, lpType, lpName, wLanguage);

	if( !hrRes && wLanguage != LANG_USER_DEFAULT )
		hrRes = FindResourceExW(hModule, lpType, lpName , LANG_USER_DEFAULT);

	if( !hrRes )
		hrRes = FindResourceExW(hModule, lpType, lpName , MAKELANGID(LANG_NEUTRAL,SUBLANG_NEUTRAL));

	return hrRes;
}

NN_INLINE
HGLOBAL
WINAPI
LoadResourceLangA(
    HMODULE hModule,
    LPCSTR lpType,
    LPCSTR lpName,
    WORD wLanguage)
{
    HRSRC hrRes=FindResourceExExA(hModule, lpType, lpName, wLanguage);
    
    return LoadResource(hModule, hrRes);
}

NN_INLINE
HGLOBAL
WINAPI
LoadResourceLangW(
    HMODULE hModule,
    LPCWSTR lpType,
    LPCWSTR lpName,
    WORD wLanguage)
{
    HRSRC hrRes=FindResourceExExW(hModule, lpType, lpName, wLanguage);
    
    return LoadResource(hModule, hrRes);
}

// String
NN_INLINE
int
WINAPI
LoadStringLangA(
    HINSTANCE hInstance,
    UINT uID,
    LPSTR lpBuffer,
    int nBufferMax,
    WORD wLanguage)
{
    LPCWSTR str;
    WORD strPos;

    str=(LPCWSTR)
        LoadResourceLangA(
            hInstance,
            (LPCSTR)RT_STRING,
            MAKEINTRESOURCEA(1+(uID >> 4)),
            wLanguage);
    if(!str)
        return 0;

    for(strPos=0;strPos<(uID & 0x000F);strPos++)
        str+=*str+1;

    if(!nBufferMax)
        return *str;
    
    if(!lpBuffer)
        return 0;
    
    WideCharToMultiByte(
        CP_ACP,
        0,
        str+1,
        *str+1,
        lpBuffer,
        nBufferMax,
        NULL,
        NULL);

    lpBuffer[min(nBufferMax-1, *str)]='\0';
    return min(nBufferMax, *str);
}

NN_INLINE
int
WINAPI
LoadStringLangW(
    HINSTANCE hInstance,
    UINT uID,
    LPWSTR lpBuffer,
    int nBufferMax,
    WORD wLanguage)
{
    LPCWSTR str;
    WORD strPos;

    str=(LPCWSTR)LoadResourceLangW(
            hInstance,
            (LPCWSTR)RT_STRING,
            MAKEINTRESOURCEW(1+(uID >> 4)),
            wLanguage);
    if(!str)
        return 0;

    for(strPos=0;strPos<(uID & 0x000F);strPos++)
        str+=*str+1;

    if(!nBufferMax)
        return *str;
    
    if(!lpBuffer)
        return 0;
    
    lstrcpynW(lpBuffer, str+1, min(nBufferMax, *str+1));

    lpBuffer[min(nBufferMax-1, *str)]=L'\0';
    return min(nBufferMax, *str);
}

// Menu
NN_INLINE
HMENU
WINAPI
LoadMenuLangA(
    HINSTANCE hInstance,
    LPCSTR lpMenuName,
    WORD wLanguage)
{
    return LoadMenuIndirectA(
        (CONST MENUTEMPLATEA*)
        LoadResourceLangA(hInstance,(LPCSTR)RT_MENU,lpMenuName,wLanguage));
}

NN_INLINE
HMENU
WINAPI
LoadMenuLangW(
    HINSTANCE hInstance,
    LPCWSTR lpMenuName,
    WORD wLanguage)
{
    return LoadMenuIndirectW(
        (CONST MENUTEMPLATEW*)
        LoadResourceLangW(hInstance,(LPCWSTR)RT_MENU,lpMenuName,wLanguage));
}

// LoadIconCursorLang
NN_INLINE
HICON
WINAPI
LoadIconCursorLangA(
    HINSTANCE hInstance,
    LPCSTR lpIconCursorName,
    WORD wLanguage,
    BOOL bIcon)
{
    HRSRC hrIcon;

    int iid=LookupIconIdFromDirectory(
                (BYTE*)LoadResourceLangA(
                    hInstance,
                    bIcon?
                        (LPCSTR)RT_GROUP_ICON:
                        (LPCSTR)RT_GROUP_CURSOR,
                    lpIconCursorName,
                    wLanguage),
                bIcon);
    
    if(!iid)
        return NULL;

    hrIcon=FindResourceExExA(hInstance,
		bIcon ? (LPCSTR)RT_ICON : (LPCSTR)RT_CURSOR,
		MAKEINTRESOURCEA(iid),
		wLanguage);

    return CreateIconFromResource(
        (BYTE*)LoadResource(hInstance, hrIcon),
        SizeofResource(hInstance, hrIcon),
        bIcon, 0x00030000);
}

NN_INLINE
HICON
WINAPI
LoadIconCursorLangW(
    HINSTANCE hInstance,
    LPCWSTR lpIconCursorName,
    WORD wLanguage,
    BOOL bIcon)
{
    HRSRC hrIcon;

    int iid=LookupIconIdFromDirectory(
                (BYTE*)LoadResourceLangW(
                    hInstance,
                    bIcon?
                    (LPCWSTR)RT_GROUP_ICON:
                    (LPCWSTR)RT_GROUP_CURSOR,
                    lpIconCursorName,
                    wLanguage),
                bIcon);
    
    if(!iid)
        return NULL;

    hrIcon=FindResourceExExW(hInstance,
		bIcon ? (LPCWSTR)RT_ICON : (LPCWSTR)RT_CURSOR,
		MAKEINTRESOURCEW(iid),
		wLanguage);

    return CreateIconFromResource(
        (BYTE*)LoadResource(hInstance, hrIcon),
        SizeofResource(hInstance, hrIcon),
        bIcon, 0x00030000);
}

// Icon
NN_INLINE
HICON
WINAPI
LoadIconLangA(
    HINSTANCE hInstance,
    LPCSTR lpIconName,
    WORD wLanguage)
{
    return LoadIconCursorLangA(hInstance,lpIconName,wLanguage,TRUE);
}

NN_INLINE
HICON
WINAPI
LoadIconLangW(
    HINSTANCE hInstance,
    LPCWSTR lpIconName,
    WORD wLanguage)
{
    return LoadIconCursorLangW(hInstance,lpIconName,wLanguage,TRUE);
}

// Cursor
NN_INLINE
HICON
WINAPI
LoadCursorLangA(
    HINSTANCE hInstance,
    LPCSTR lpCursorName,
    WORD wLanguage)
{
    return LoadIconCursorLangA(hInstance,lpCursorName,wLanguage,FALSE);
}

NN_INLINE
HICON
WINAPI
LoadCursorLangW(
    HINSTANCE hInstance,
    LPCWSTR lpCursorName,
    WORD wLanguage)
{
    return LoadIconCursorLangW(hInstance,lpCursorName,wLanguage,FALSE);
}

// Dialog
NN_INLINE
HWND
WINAPI
CreateDialogParamLangA(
    HINSTANCE hInstance,
    LPCSTR lpTemplateName,
    HWND hWndParent,
    DLGPROC lpDialogFunc,
    LPARAM dwInitParam,
    WORD wLanguage)
{
    CreateDialogIndirectParamA(
        hInstance,
        (LPDLGTEMPLATEA)LoadResourceLangA(
            hInstance,
            (LPCSTR)RT_DIALOG,
            lpTemplateName,
            wLanguage),
        hWndParent,
        lpDialogFunc,
        dwInitParam);
}

NN_INLINE
HWND
WINAPI
CreateDialogParamLangW(
    HINSTANCE hInstance,
    LPCWSTR lpTemplateName,
    HWND hWndParent,
    DLGPROC lpDialogFunc,
    LPARAM dwInitParam,
    WORD wLanguage)
{
    return CreateDialogIndirectParamW(
        hInstance,
        (LPDLGTEMPLATEW)LoadResourceLangW(
            hInstance,
            (LPCWSTR)RT_DIALOG,
            lpTemplateName,
            wLanguage),
        hWndParent,
        lpDialogFunc,
        dwInitParam);
}

NN_INLINE
INT_PTR
WINAPI
DialogBoxParamLangA(
    HINSTANCE hInstance,
    LPCSTR lpTemplateName,
    HWND hWndParent,
    DLGPROC lpDialogFunc,
    LPARAM dwInitParam,
    WORD wLanguage)
{
    return DialogBoxIndirectParamA(
        hInstance,
        (LPDLGTEMPLATEA)LoadResourceLangA(
            hInstance,
            (LPCSTR)RT_DIALOG,
            lpTemplateName,
            wLanguage),
        hWndParent,
        lpDialogFunc,
        dwInitParam);
}

NN_INLINE
INT_PTR
WINAPI
DialogBoxParamLangW(
    HINSTANCE hInstance,
    LPCWSTR lpTemplateName,
    HWND hWndParent,
    DLGPROC lpDialogFunc,
    LPARAM dwInitParam,
    WORD wLanguage)
{
    return DialogBoxIndirectParamW(
        hInstance,
        (LPDLGTEMPLATEW)LoadResourceLangW(
            hInstance,
            (LPCWSTR)RT_DIALOG,
            lpTemplateName,
            wLanguage),
        hWndParent,
        lpDialogFunc,
        dwInitParam);
}

// LangID
#ifdef NN_LANG_ID

// String
int
WINAPI
LoadStringRA(
    HINSTANCE hInstance,
    UINT uID,
    LPSTR lpBuffer,
    int nBufferMax);
int
WINAPI
LoadStringRW(
    HINSTANCE hInstance,
    UINT uID,
    LPWSTR lpBuffer,
    int nBufferMax);
#ifdef UNICODE
#define LoadStringR LoadStringRW
#else
#define LoadStringR LoadStringRA
#endif // !UNICODE

// Menu
HMENU
WINAPI
LoadMenuRA(
    HINSTANCE hInstance,
    LPCSTR lpMenuName);
HMENU
WINAPI
LoadMenuRW(
    HINSTANCE hInstance,
    LPCWSTR lpMenuName);
#ifdef UNICODE
#define LoadMenuR LoadMenuRW
#else
#define LoadMenuR LoadMenuRA
#endif // !UNICODE

// Icon
HICON
WINAPI
LoadIconRA(
    HINSTANCE hInstance,
    LPCSTR lpIconName);
HICON
WINAPI
LoadIconRW(
    HINSTANCE hInstance,
    LPCWSTR lpIconName);
#ifdef UNICODE
#define LoadIconR LoadIconRW
#else
#define LoadIconR LoadIconRA
#endif // !UNICODE

// Cursor
HICON
WINAPI
LoadCursorRA(
    HINSTANCE hInstance,
    LPCSTR lpCursorName);
HICON
WINAPI
LoadCursorRW(
    HINSTANCE hInstance,
    LPCWSTR lpCursorName);
#ifdef UNICODE
#define LoadCurosrR LoadCursorRW
#else
#define LoadCursorR LoadCursorRA
#endif // !UNICODE

// Dialog
HWND
WINAPI
CreateDialogParamRA(
    HINSTANCE hInstance,
    LPCSTR lpTemplateName,
    HWND hWndParent,
    DLGPROC lpDialogFunc,
    LPARAM dwInitParam);
HWND
WINAPI
CreateDialogParamRW(
    HINSTANCE hInstance,
    LPCWSTR lpTemplateName,
    HWND hWndParent,
    DLGPROC lpDialogFunc,
    LPARAM dwInitParam);
#ifdef UNICODE
#define CreateDialogParamR  CreateDialogParamRW
#else
#define CreateDialogParamR  CreateDialogParamRA
#endif // !UNICODE

#ifdef UNICODE
#define CreateDialogIndirectParamR  CreateDialogIndirectParamRW
#else
#define CreateDialogIndirectParamR  CreateDialogIndirectParamRA
#endif // !UNICODE

#define CreateDialogRA(hInstance, lpName, hWndParent, lpDialogFunc) \
CreateDialogParamRA(hInstance, lpName, hWndParent, lpDialogFunc, 0L)
#define CreateDialogRW(hInstance, lpName, hWndParent, lpDialogFunc) \
CreateDialogParamRW(hInstance, lpName, hWndParent, lpDialogFunc, 0L)
#ifdef UNICODE
#define CreateDialogR  CreateDialogRW
#else
#define CreateDialogR  CreateDialogRA
#endif // !UNICODE

INT_PTR
WINAPI
DialogBoxParamRA(
    HINSTANCE hInstance,
    LPCSTR lpTemplateName,
    HWND hWndParent,
    DLGPROC lpDialogFunc,
    LPARAM dwInitParam);
INT_PTR
WINAPI
DialogBoxParamRW(
    HINSTANCE hInstance,
    LPCWSTR lpTemplateName,
    HWND hWndParent,
    DLGPROC lpDialogFunc,
    LPARAM dwInitParam);
#ifdef UNICODE
#define DialogBoxParamR  DialogBoxParamRW
#else
#define DialogBoxParamR  DialogBoxParamRA
#endif // !UNICODE

#define DialogBoxRA(hInstance, lpTemplate, hWndParent, lpDialogFunc) \
DialogBoxParamRA(hInstance, lpTemplate, hWndParent, lpDialogFunc, 0L)
#define DialogBoxRW(hInstance, lpTemplate, hWndParent, lpDialogFunc) \
DialogBoxParamRW(hInstance, lpTemplate, hWndParent, lpDialogFunc, 0L)
#ifdef UNICODE
#define DialogBoxR  DialogBoxRW
#else
#define DialogBoxR  DialogBoxRA
#endif // !UNICODE

// Functions
NN_INLINE
int
WINAPI
LoadStringRA(
    HINSTANCE hInstance,
    UINT uID,
    LPSTR lpBuffer,
    int nBufferMax)
{
    return LoadStringLangA(
        hInstance,
        uID,
        lpBuffer,
        nBufferMax,
        NN_LANG_ID);
}

NN_INLINE
int
WINAPI
LoadStringRW(
    HINSTANCE hInstance,
    UINT uID,
    LPWSTR lpBuffer,
    int nBufferMax)
{
    return LoadStringLangW(
        hInstance,
        uID,
        lpBuffer,
        nBufferMax,
        NN_LANG_ID);
}

// Menu
NN_INLINE
HMENU
WINAPI
LoadMenuRA(
    HINSTANCE hInstance,
    LPCSTR lpMenuName)
{
    return LoadMenuLangA(
        hInstance,
        lpMenuName,
        NN_LANG_ID);
}

NN_INLINE
HMENU
WINAPI
LoadMenuRW(
    HINSTANCE hInstance,
    LPCWSTR lpMenuName)
{
    return LoadMenuLangW(
        hInstance,
        lpMenuName,
        NN_LANG_ID);
}

// Icon
NN_INLINE
HICON
WINAPI
LoadIconRA(
    HINSTANCE hInstance,
    LPCSTR lpIconName)
{
    return LoadIconLangA(
        hInstance,
        lpIconName,
        NN_LANG_ID);
}

NN_INLINE
HICON
WINAPI
LoadIconRW(
    HINSTANCE hInstance,
    LPCWSTR lpIconName)
{
    return LoadIconLangW(
        hInstance,
        lpIconName,
        NN_LANG_ID);
}

// Cursor
NN_INLINE
HICON
WINAPI
LoadCursorRA(
    HINSTANCE hInstance,
    LPCSTR lpCursorName)
{
    return LoadCursorLangA(
        hInstance,
        lpCursorName,
        NN_LANG_ID);
}

NN_INLINE
HICON
WINAPI
LoadCursorRW(
    HINSTANCE hInstance,
    LPCWSTR lpCursorName)
{
    return LoadCursorLangW(
        hInstance,
        lpCursorName,
        NN_LANG_ID);
}

// Dialog
NN_INLINE
HWND
WINAPI
CreateDialogParamRA(
    HINSTANCE hInstance,
    LPCSTR lpTemplateName,
    HWND hWndParent,
    DLGPROC lpDialogFunc,
    LPARAM dwInitParam)
{
    return CreateDialogParamLangA(
        hInstance,
        lpTemplateName,
        hWndParent,
        lpDialogFunc,
        dwInitParam,
        NN_LANG_ID);
}

NN_INLINE
HWND
WINAPI
CreateDialogParamRW(
    HINSTANCE hInstance,
    LPCWSTR lpTemplateName,
    HWND hWndParent,
    DLGPROC lpDialogFunc,
    LPARAM dwInitParam)
{
    return CreateDialogParamLangW(
        hInstance,
        lpTemplateName,
        hWndParent,
        lpDialogFunc,
        dwInitParam,
        NN_LANG_ID);
}


NN_INLINE
INT_PTR
WINAPI
DialogBoxParamRA(
    HINSTANCE hInstance,
    LPCSTR lpTemplateName,
    HWND hWndParent,
    DLGPROC lpDialogFunc,
    LPARAM dwInitParam)
{
    return DialogBoxParamLangA(
        hInstance,
        lpTemplateName,
        hWndParent,
        lpDialogFunc,
        dwInitParam,
        NN_LANG_ID);
}

NN_INLINE
INT_PTR
WINAPI
DialogBoxParamRW(
    HINSTANCE hInstance,
    LPCWSTR lpTemplateName,
    HWND hWndParent,
    DLGPROC lpDialogFunc,
    LPARAM dwInitParam)
{
    return DialogBoxParamLangW(
        hInstance,
        lpTemplateName,
        hWndParent,
        lpDialogFunc,
        dwInitParam,
        NN_LANG_ID);
}


// ReDefines
#ifdef NN_LANG_R
#ifndef NN_LANG_ID
#error You must define NN_LANG_ID
#endif // NN_LANG_ID

// String
#undef LoadStringA
#define LoadStringA LoadStringRA
#undef LoadStringW
#define LoadStringW LoadStringRW
#undef LoadString
#define LoadString LoadStringR

// Menu
#undef LoadMenuA
#define LoadMenuA LoadMenuRA
#undef LoadMenuW
#define LoadMenuW LoadMenuRW
#undef LoadMenu
#define LoadMenu LoadMenuR

// Icon
#undef LoadIconA
#define LoadIconA LoadIconRA
#undef LoadIconW
#define LoadIconW LoadIconRW
#undef LoadIcon
#define LoadIcon LoadIconR

// Cursor
#undef LoadCursorA
#define LoadCursorA LoadCursorRA
#undef LoadCursorW
#define LoadCursorW LoadCursorRW
#undef LoadCursor
#define LoadCursor LoadCursorR

// Dialog
#undef CreateDialogParamA
#define CreateDialogParamA CreateDialogParamRA
#undef CreateDialogParamW
#define CreateDialogParamW CreateDialogParamRW
#undef CreateDialogParam
#define CreateDialogParam CreateDialogParam

#undef DialogBoxParamA
#define DialogBoxParamA DialogBoxParamRA
#undef DialogBoxParamW
#define DialogBoxParamW DialogBoxParamRW
#undef DialogBoxParam
#define DialogBoxParam DialogBoxParamR
#endif // NN_LANG_R

#endif // NN_LANG_ID


#ifdef __cplusplus
}
#endif

#endif // __NN_Lang__