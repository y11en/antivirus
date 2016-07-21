#ifndef _ERR_CODES_
#define _ERR_CODES_
/*Error codes format*/

//
//  Values are 32 bit values layed out as follows:
//
//   3 3 2 2 2 2 2 2 2 2 2 2 1 1 1 1 1 1 1 1 1 1
//   1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0
//  +---+-+-+-----------------------+-------------------------------+
//  |Sev|C|R|     Facility          |               Code            |
//  +---+-+-+-----------------------+-------------------------------+
//
//  where
//
//      Sev - is the severity code
//
//          00 - Success
//          01 - Informational
//          10 - Warning
//          11 - Error
//
//      C - is the Customer code flag
//
//      R - is a reserved bit
//
//      Facility - is the facility code
//
//      Code - is the facility's status code
//
//

#define CUSTOMER_FLAG	(0x20000000)

//severity codes
#define NET_SUCCESS				(0x0) | CUSTOMER_FLAG
//errors
#define NET_CRITICAL_ERROR		(((0x3)<<30) | (0x1<<28) | CUSTOMER_FLAG) & 0xF0000000
#define NET_ERROR				(0x3<<30) | CUSTOMER_FLAG
//warnings
#define NET_DANGEROUS			(0x2<<30) | (0x1<<28) | CUSTOMER_FLAG
#define NET_WARNING				(0x2<<30) | CUSTOMER_FLAG
#define NET_IMPLEMENTATION		(0x1<<30) | (0x1<<28) | CUSTOMER_FLAG

//Notification
#define NET_NOTIFICATION		(0x1<<30) | CUSTOMER_FLAG




#endif //_ERR_CODES_