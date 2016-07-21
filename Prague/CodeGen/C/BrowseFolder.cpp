#include "stdafx.h"
#include <windows.h>
#include <shlobj.h>

// ---
//int CALLBACK BrowseCallbackProc( HWND hwnd, UINT uMsg, LPARAM lParam, LPARAM lpData ) {
//
//  switch( uMsg ) {
//
//    case BFFM_INITIALIZED :
//      if ( lpData && *(const char*)lpData )
//        ::SendMessage( hwnd, BFFM_SETSELECTION, 1, lpData );
//      return 0;
//
//    case BFFM_SELCHANGED :
//      //::SHGetPathFromIDList( (LPITEMIDLIST)lParam, CDistrApp::work_buffer );
//      //lParam = ::CheckExist(CDistrApp::work_buffer) == 0;
//      //::SendMessage( hwnd, BFFM_ENABLEOK, 0, lParam );
//      //::SendMessage( hwnd, BFFM_SETSTATUSTEXT, 0, (LPARAM)"Победа" );
//      return 0;
//
//    case BFFM_VALIDATEFAILED :
//      //::MessageBox( hwnd, (const char*)lParam, "Заголовок", MB_OK );
//      return 0;
//  }
//  return 0;
//}


//// ---
//HRESULT ChooseFolder( HWND owner, const char* title, char* buffer, BFFCALLBACK bff_call_back, void* param ) {
//
//  HRESULT       hr;
//  IShellFolder* folder;
//  LPITEMIDLIST  browse = 0;    // PIDL selected by user  
//
//  if ( SUCCEEDED(hr=::SHGetDesktopFolder(&folder)) ) {
//    LPMALLOC malloc;
//    /*
//    ULONG parsed;
//    int len = ::MultiByteToWideChar( CP_ACP, MB_COMPOSITE, CDistrApp::work_buffer, -1, 0, 0 );
//    BSTR  dir = ::SysAllocStringLen( 0, len );
//    ::MultiByteToWideChar( CP_ACP, MB_COMPOSITE, CDistrApp::work_buffer, -1, dir, len );
//    if ( !SUCCEEDED(folder->ParseDisplayName(m_hWnd,0,dir,&parsed,&item,0)) ) {
//      folder->Release();
//      malloc->Release();
//      return;
//    }
//    folder->Release();
//    ::SysFreeString( dir );
//    */
//
//    if ( SUCCEEDED(hr=::SHGetMalloc(&malloc)) ) {
//      LPITEMIDLIST item;      // PIDL for folder 
//  
//      // Get the PIDL for the Programs folder. 
//      // CSIDL_COMMON_DESKTOPDIRECTORY 
//      // CSIDL_DRIVES
//      if ( SUCCEEDED(hr=::SHGetSpecialFolderLocation(owner,CSIDL_DESKTOP,&item)) ) {
//
//        char local_title[100];
//        BROWSEINFO bi;     
//        bi.hwndOwner      = owner;
//        bi.pidlRoot       = item;
//        bi.pszDisplayName = buffer;
//
//        if ( !title || !*title ) {
//          ::GetWindowText( owner, local_title, sizeof(local_title) );
//          title = local_title;
//        }
//
//        bi.lpszTitle = title;
//
//        if ( bff_call_back ) {
//          bi.lpfn = bff_call_back;
//          bi.lParam = (LPARAM)param;
//        }
//        else {
//          bi.lpfn = ::BrowseCallbackProc;
//          if ( param )
//            bi.lParam = (LPARAM)param;
//          else
//            bi.lParam = (LPARAM)buffer;
//        }
//
//
//        bi.ulFlags = 0
//          //| BIF_STATUSTEXT
//          //| BIF_BROWSEINCLUDEFILES
//          //| ( bff_call_back ? BIF_EDITBOX : 0 )
//          //| ( bff_call_back ? BIF_VALIDATE : 0 )
//          //| BIF_RETURNFSANCESTORS
//          | BIF_RETURNONLYFSDIRS;
//
//
//        // Browse for a folder and return its PIDL. 
//        browse = ::SHBrowseForFolder( &bi );
//
//        // Fill in the BROWSEINFO structure.     
//        if (  browse != NULL ) {
//          ::SHGetPathFromIDList( browse, buffer );
//          malloc->Free( browse );
//        }      
//        malloc->Free( item ); 
//      }
//      malloc->Release();
//    }
//    folder->Release();
//  }
//  return browse ? S_OK : E_FAIL;
//}



