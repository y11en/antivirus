#include "kl_stackdata.h"

void
CheckFiFo()
{
    char    Buffer1[100];
    CKl_Fifo    fifo;

    // fifo.PushData("Hello ", strlen("Hello ") );
    // fifo.PushData("World ", strlen("World ") );
    fifo.PushData("H", 1);
    fifo.PushData("e", 1);
    fifo.PushData("l", 1);
    fifo.PushData("l", 1);
    fifo.PushData("0", 1);
    fifo.PushData(" ", 1);
    fifo.PushData("w", 1);
    fifo.PushData("o", 1);
    fifo.PushData("r", 1);
    fifo.PushData("l", 1);
    fifo.PushData("d", 1);
    fifo.PushData("_", 1);
    /*
    fifo.PopData(Buffer1, 6);
    Buffer1[6] = 0;
    printf ("%s\n",Buffer1);
    */

    Buffer1[fifo.PopData(Buffer1, 100)] = 0;
    printf ("%s\n",Buffer1);
/*    
    while ( fifo.PopData(Buffer1, 1) )
    {
        Buffer1[1] = 0;
        printf ("%s\n",Buffer1);
    }
*/
}