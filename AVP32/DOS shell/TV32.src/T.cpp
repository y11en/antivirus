/*---------------------------------------------------------*/
/*                                                         */
/*   Turbo Vision 1.0                                      */
/*   Copyright (c) 1991 by Borland International           */
/*                                                         */
/*   Turbo Vision Hello World Demo Source File             */
/*---------------------------------------------------------*/

#define Uses_TKeys
#define Uses_TApplication
#define Uses_TEvent
#define Uses_TRect
#define Uses_TDialog
#define Uses_TStaticText
#define Uses_TButton
#define Uses_TMenuBar
#define Uses_TSubMenu
#define Uses_TMenuItem
#define Uses_TStatusLine
#define Uses_TStatusItem
#define Uses_TStatusDef
#define Uses_TDeskTop
#define Uses_TFileDialog
#include "include\tv.h"

const GreetThemCmd = 100;

class THelloApp : public TApplication
{

public:

    THelloApp();

    virtual void handleEvent( TEvent& event );
    static TMenuBar *initMenuBar( TRect );
    static TStatusLine *initStatusLine( TRect );

private:

    void greetingBox();
};

THelloApp::THelloApp() :
    TProgInit( &THelloApp::initStatusLine,
               &THelloApp::initMenuBar,
               &THelloApp::initDeskTop
             )
{
}

void THelloApp::greetingBox()
{
   TFileDialog *dialog = new TFileDialog("*.*","File","Enter file name",
                                fdOKButton | fdHelpButton,33);
  if ( validView(dialog) ) {
    static char buf[65+1];
    dialog->helpCtx = 0;
    ushort c = TProgram::deskTop->execView(dialog);
    dialog->getFileName(buf);
    TObject::destroy( dialog );
  }
}

void THelloApp::handleEvent( TEvent& event )
{
    TApplication::handleEvent( event );
    if( event.what == evCommand )
        {
        switch( event.message.command )
            {
            case GreetThemCmd:
                greetingBox();
                clearEvent( event );
                break;
            default:
                break;
            }
        }
}

TMenuBar *THelloApp::initMenuBar( TRect r )
{

    r.b.y = r.a.y+1;

    return new TMenuBar( r,
      *new TSubMenu( "~H~ello", kbAltH ) +
        *new TMenuItem( "~G~reeting...", GreetThemCmd, kbAltG ) +
         newLine() +
        *new TMenuItem( "E~x~it", cmQuit, cmQuit, hcNoContext, "Alt-X" )
        );

}

TStatusLine *THelloApp::initStatusLine( TRect r )
{
    r.a.y = r.b.y-1;
    return new TStatusLine( r,
        *new TStatusDef( 0, 0xFFFF ) +
            *new TStatusItem( "~Alt-X~ Exit", kbAltX, cmQuit ) +
            *new TStatusItem( 0, kbF10, cmMenu )
            );
}

int main()
{
    THelloApp helloWorld;
    helloWorld.run();
    return 0;
}
