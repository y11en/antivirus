#include <sys/time.h>
#include <time.h>

#include <iostream>
#include <iomanip>

#include <prague.h> 
#include <pr_loadr.h>
#include <iface/i_root.h>
#include <plugin/p_timer.h>

using namespace std;

template <class Ch, class Tr> 
basic_ostream <Ch,Tr>& formatted_hex ( basic_ostream<Ch,Tr>& aStream ) 
{ 
  aStream << "0x" <<  setw ( 8 ) << setfill ( '0' ) << uppercase << hex;
  return aStream; 
}

hROOT g_root = 0;

tERROR createTimer ( hTIMER& timer, tBOOL enable, tDWORD frequency ) 
{
  timer = 0;
  tERROR l_error = g_root->sysCreateObject((hOBJECT*)&timer, IID_TIMER, PID_TIMER);
  if ( PR_FAIL ( l_error ) ) {
    cerr << "Can't create a timer, error: " << formatted_hex << l_error << endl;
  }
  if ( PR_SUCC ( l_error ) ) {
     l_error = timer->set_pgFREQUENCY(frequency);
    if ( PR_FAIL ( l_error ) )
      cerr << "Can't set a frequency for the timer, error: " << formatted_hex << l_error << endl;
  }
  if ( PR_SUCC ( l_error ) ) {
    l_error = timer->set_pgENABLED(enable);
    if ( PR_FAIL ( l_error ) )    
      cerr << "Can't set the timer enabled, error: " << formatted_hex << l_error << endl;
  }
  if ( PR_SUCC ( l_error ) ) {
    l_error = timer->sysCreateObjectDone();
    if ( PR_FAIL ( l_error ) ) 
      cerr << "Can't complete a creation of the timer, error: " << formatted_hex << l_error << endl;
  }
  if ( PR_FAIL ( l_error ) && timer )
    timer->sysCloseObject();
  return l_error;
}


tERROR pr_call MsgReceiveRoot( hOBJECT _this, tDWORD msg_cls, tDWORD msg_id, hOBJECT send_point, hOBJECT ctx, hOBJECT receive_point, tPTR pbuff, tDWORD* blen ) 
{
  tERROR error = errOK;
  switch(msg_cls) {
  case pmc_TIMER: {
    struct timeval l_time;
    gettimeofday ( &l_time, 0 );
    char l_atime [100] = {0};
    ctime_r ((time_t*)&l_time.tv_sec, l_atime );
    cout << "Timer " << msg_id << " has been expired at " << l_atime << endl;
    break;
  }
  default:
    cerr << "Unexpected message" << endl;
  }
  return errOK;
}


int main()
{
  CPrague aPrague ( 0 );
  
  if ( !g_root ) {
    cerr << "Can't load Prague" << endl;
    return -1;
  }
  
  tERROR l_error = errOK;

  cMsgReceiver*       l_msg_rcv;
  if ( PR_SUCC ( l_error ) ) {
    l_error = g_root->sysCreateObject((hOBJECT*)&l_msg_rcv,IID_MSG_RECEIVER,PID_ANY,SUBTYPE_ANY);
    if ( PR_FAIL ( l_error ) )
      cerr << "Can't create a messages receiver, error: " << formatted_hex << l_error << endl;
  }
  if ( PR_SUCC ( l_error ) ) {
    l_error = l_msg_rcv->propSetDWord( pgRECEIVE_PROCEDURE, (tDWORD)MsgReceiveRoot );
    if ( PR_FAIL ( l_error ) )
      cerr << "Can't set a receive procedure, error: " << formatted_hex << l_error << endl;
  }
  if ( PR_SUCC ( l_error ) ) {
    l_error = l_msg_rcv->sysRegisterMsgHandler( pmc_TIMER, rmhLISTENER, g_root, IID_TIMER, PID_TIMER, IID_ANY, PID_ANY );
    if ( PR_FAIL ( l_error ) )
      cerr << "Can't register a message handler, error: " << formatted_hex << l_error << endl;
  }
  if ( PR_SUCC ( l_error ) ) l_error = l_msg_rcv->sysCreateObjectDone(); {
    if ( PR_FAIL ( l_error ) )
      cerr << "Can't complete a creation of the message receiver, error: " << formatted_hex << l_error << endl;
  }

  hTIMER l_timer1 = 0, l_timer2 = 0, l_timer3 = 0;
  if ( PR_SUCC ( l_error ) )
    l_error = createTimer ( l_timer1, cTRUE, 1000 ); 
  if ( PR_SUCC ( l_error ) )
    l_error = createTimer ( l_timer2, cTRUE, 10000 ); 
  if ( PR_SUCC ( l_error ) )
    l_error = createTimer ( l_timer3, cTRUE, 20000 ); 

  getchar ();
  
  if ( l_timer1 )
    l_timer1->sysCloseObject();

  if ( l_timer2 )
    l_timer2->sysCloseObject();

  if ( l_timer3 )
    l_timer3->sysCloseObject();

  return 0;
}
