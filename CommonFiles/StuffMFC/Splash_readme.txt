Follow these steps to add a splash screen to your dialog based application: 

Copy the Splash.cpp and Splash.h files from this example project to your project,
 and add the following code to your applications InitInstance() method in the 
CWinApp derived class: 

#include "Splash.h"

BOOL CDialogsplApp::InitInstance()
{
  // CG: The following block was added by the Splash Screen component.
  {
     CCommandLineInfo cmdInfo;
     ParseCommandLine(cmdInfo);

     CSplashWnd::EnableSplashScreen(cmdInfo.m_bShowSplash);
  }
  
  ...
}

Next, use class wizard to add the OnCreate method to the CDialog derived class .cpp file,
 and add the following code to it: 

#include "Splash.h"

int CDialogsplDlg::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
   if (CDialog::OnCreate(lpCreateStruct) == -1)
      return -1;
   
   // CG: The following line was added by the Splash Screen component.
   CSplashWnd::ShowSplashScreen(this);

   return 0;
}

 