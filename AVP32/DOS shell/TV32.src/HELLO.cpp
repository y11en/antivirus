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
#define Uses_TScreen
#include "include\tv.h"
#include <stdio.h>
#include <stdlib.h>

const GreetThemCmd = 100;
const FileBoxCmd = 101;

class THelloApp : public TApplication
{

public:

    THelloApp();

    virtual void handleEvent( TEvent& event );
    static TMenuBar *initMenuBar( TRect );
    static TStatusLine *initStatusLine( TRect );
    void greetingBox();

private:

    void fileBox();
};

THelloApp::THelloApp() :
    TProgInit( THelloApp::initStatusLine,
               THelloApp::initMenuBar,
               THelloApp::initDeskTop
             )
{
}

void THelloApp::greetingBox()
{
    TRect r(25,5,55,16);
//    r.move(-30,-7);
    TDialog *d = new TDialog(r, "Hello, World!" );

    d->insert( new TStaticText( TRect( 3, 5, 15, 6 ), "How are you?" ) );
    d->insert( new TButton( TRect( 16, 2, 28, 4 ), "Terrific", cmCancel, bfNormal ) );
    d->insert( new TButton( TRect( 16, 4, 28, 6 ), "Ok", cmCancel, bfNormal ) );
    d->insert( new TButton( TRect( 16, 6, 28, 8 ), "Lousy", cmCancel, bfNormal ) );
    d->insert( new TButton( TRect( 16, 8, 28, 10 ), "Cancel", cmCancel, bfNormal ) );

    deskTop->execView( d );
    destroy(d);
}

void THelloApp::fileBox() {
  TFileDialog *dialog = new TFileDialog("*.CPP","File","Select file",
                                fdOKButton|fdHelpButton,
                                5);
  ushort c = TProgram::deskTop->execView(dialog);
  char buf[256];
  dialog->getFileName(buf);
  if ( c != cmCancel ) printf("File: '%s'\n",buf);
  TObject::destroy( dialog );
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
            case FileBoxCmd:
                fileBox();
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
     ( *new TSubMenu( "~ð~", kbAltSpace ) +
        *new TMenuItem( "~G~reeting...", GreetThemCmd, kbAltG )) +
     ( *new TSubMenu( "~H~ello", kbAltH ) +
        *new TMenuItem( "~G~reeting...", GreetThemCmd, kbAltG ) +
        *new TMenuItem( "~F~ilebox...", FileBoxCmd, kbAltB ) +
         newLine() +
        *new TMenuItem( "E~x~it", cmQuit, cmQuit, hcNoContext, "Alt-X" ))
        );

}

TStatusLine *THelloApp::initStatusLine( TRect r )
{
    r.a.y = r.b.y-1;
    return new TStatusLine( r,
        *new TStatusDef( 0, 0xFFFF ) +
            *new TStatusItem( "~Ctrl-Ins~ Filebox", kbCtrlIns, FileBoxCmd ) +
            *new TStatusItem( "~Alt-X~ Exit", kbAltX, cmQuit ) +
            *new TStatusItem( 0, kbF10, cmMenu )
            );
}

#include <time.h>
int main(int argc,char *argv[])
{
#ifndef __MSDOS__
    int mode = 0;
    if ( argc > 2 ) mode = atoi(argv[1]) + (atoi(argv[2]) << 8);
                        //  row                 col
#endif
    THelloApp helloWorld;
#ifndef __MSDOS__
    helloWorld.setScreenMode(mode);
#endif
    helloWorld.run();
    return 0;
}
