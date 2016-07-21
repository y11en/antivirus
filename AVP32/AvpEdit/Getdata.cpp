#include "stdafx.h"
#include "resource.h"


typedef struct GetDataTag {
  const char* file;
        const char*     prompt;
        char*                           data;
        int                                     max_len;
        int                                     id;
} GetDataStruct;

char buff[512], buff2[512];


// ---
static BOOL CALLBACK GetDataDlgProc( HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam ) {
        HDC sdc;
        int sw, sh;
        RECT rect;
        GetDataStruct* pd;
        HWND edit, header;
  static flash = 0;

        switch ( uMsg ) {
                case WM_INITDIALOG :
      flash = 0;
                        pd = (GetDataStruct*)lParam;
                        SetWindowLong( hwndDlg, DWL_USER, lParam );
      header = GetDlgItem( hwndDlg, IDC_HEADER );
      if ( header ) {
        GetDlgItemText( hwndDlg, IDC_HEADER, buff, sizeof(buff) );
        wsprintf( buff2, buff, pd->file ? pd->file : "" );
        SetDlgItemText( hwndDlg, IDC_HEADER, buff2 );
      }
                        edit = GetDlgItem( hwndDlg, pd->id );
                        if ( edit ) {
                                ShowWindow( edit, SW_SHOWNORMAL );
                                if ( pd->max_len > 0 )
                                        SendMessage( edit, EM_SETLIMITTEXT, pd->max_len, 0 );
                                else
                                        pd->max_len = 256;
                                if ( pd->data && *pd->data )
                                        SendMessage( edit, WM_SETTEXT, 0, (LPARAM)pd->data );
                        }
                        sdc = GetDC( GetDesktopWindow() );
                        sw = GetDeviceCaps( sdc, HORZRES );
                        sh = GetDeviceCaps( sdc, VERTRES );
                        ReleaseDC( GetDesktopWindow(), sdc );
                        GetWindowRect( hwndDlg, &rect );
                        SetWindowPos( hwndDlg, HWND_TOPMOST, (sw - (rect.right-rect.left))/2, (sh - (rect.bottom-rect.top))/2, 0, 0, SWP_NOSIZE );
                        SetDlgItemText( hwndDlg, IDC_PROMPT, (LPCTSTR)pd->prompt );
      SetTimer( hwndDlg, 1, 15000, 0 );
                        return TRUE;
    case WM_TIMER :
      {
        /*
        HWND focus = ::GetFocus();
        if ( hwndDlg == focus || IsChild(hwndDlg,focus) ) {
          if ( flash ) {
            flash = 0;
            FlashWindow( hwndDlg, FALSE );
          }
          KillTimer( hwndDlg, 1 );
          MessageBeep( -1 );
        }
        else {
        */
          FlashWindow( hwndDlg, TRUE );
          if ( !flash ) {
            flash = 1;
            KillTimer( hwndDlg, 1 );
            SetTimer( hwndDlg, 1, 500, 0 );
          }
        //}
      }
                        return TRUE;
                case WM_COMMAND :
                        switch ( LOWORD(wParam) ) {
                                case IDOK2 :
                                        pd = (GetDataStruct*)GetWindowLong( hwndDlg, DWL_USER );
                                        GetDlgItemText( hwndDlg, pd->id, (LPTSTR)pd->data, pd->max_len );
                                        EndDialog( hwndDlg, IDOK );
                                        break;
                                case IDCANCEL2 :
                                        pd = (GetDataStruct*)GetWindowLong( hwndDlg, DWL_USER );
                                        pd->data[0] = 0;
                                        EndDialog( hwndDlg, IDCANCEL );
                                        break;
                        }
        }
        return FALSE;
}

// ---
int GetData( HWND dad, int secure, const char* file, const char* prompt, char* data, int max_len ) {
        HINSTANCE hInst = (HINSTANCE)GetModuleHandle(0);
        GetDataStruct pd;
  pd.file = file;
        pd.prompt = prompt;
        pd.data = data;
        pd.max_len = max_len;
        pd.id = secure ? IDC_PASSWORD : IDC_DATA;

        return DialogBoxParam( hInst, MAKEINTRESOURCE(IDD_DATA), dad, (DLGPROC)GetDataDlgProc, (LPARAM)&pd );
}

