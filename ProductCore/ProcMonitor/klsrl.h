#ifndef __KLSRL_PROTO_H
#define __KLSRL_PROTO_H

#include "events.h"
/*
    *****************************************************
    *     HIPS protocol description (to be discussed).  *
    *         (Document version 1.1  12.10.2007)        *
    *****************************************************


     -----------     Request      -----------
     |         |    -------->     |         |
     | Client  |      Reply       | Server  |   
     |         |    <-------      |         |
     -----------                  -----------


  The client/server communication in HIPS consists of a request packet sent by 
a client to a server, which replies with an exactly one response packet.
The protocol itself is stateless; all the information needed is contained in
a single packet. Both UDP and TCP transport can be used to transmit packets.
The HIPS servers listen for UDP and TCP requests on port 7024.

  Each HIPS packet consists of a 12 byte header followed by application data
and a trailing 2-byte checksum to provide data integrity.


 --------------------------------------------------------------
 |LEN |V |RN|   SN   |   HID  |        D a t a           |CRC | 
 --------------------------------------------------------------
   2   1  1     4         4        up to 498 bytes (UDP)   2
                                        1008 bytes (TCP)


  LEN - Packet length (2-byte unsigned integer, network order). It is needed 
        mostly by TCP protocol transport.
  V   - Version number (1-byte unsigned integer). V=1 for the current version
        This field, along with RN,SN and HID fields, is just copied into
        a response packet by a server. 
  RN  - Re-transmission number (1-byte unsigned integer). This value is 0 for
        the first request and incremented for each retransmission, while other 
        fields of the packet must stay untouched. Up to 256 retransmissions
        are allowed, but the recommended value is 3.
        Clients must issue all retransmissions within MAX_DELAY_SEC=24
        seconds. This matters for how long a server must remember old requests
        to recognize retransmissions. 
        The client UDP port number must be the same for each retransmission 
        attempt, while the source IP address may vary (multi-homed host).
        Clients need to know the RN field in responses to maintain RTTs 
        (Round Trip Time)
  SN  - Sequence number (4-byte unsigned integer, network order). This field
        is incremented for every request sent by a client. The initial value
        of this field is intended to be random.
  HID - Host ID (4-byte unsigned integer, network order). The value that
        uniquely identifies the client host. The server needs this value to
        maintain cache and to identify retransmissions. The server cannot 
        rely on the source IP address as client hosts can be multi-homed. 
        It is up to the client to choose the 'right' method to calculate 
        this number. Some recommendations are given below.
  Data- Application data (up to 498 bytes for UDP and up to 1008 bytes for TCP)  
  CRC - 2-byte CRC-16-CCITT checksum. Polynomial: x^16 + x^12 + x^5 + 1


  The main reason for including the HID field is that a server needs to 
 identify retransmissions. These must not change any aggregated data on
 the server side (such as MD5 counter described below), but still need
 responses to be sent back to a client. We cannot count on RN field since
 packets may arrive in reverse order. Server maintains a cache  of all 
 recently received requests. Each request is identified by the client port
 number, request sequence number (SN) and Host Id (HID). These fields
 remain unchanged during each retransmission. We cannot rely on the client
 source IP address since client host can be multi-homed. An objection
 against initialization the HID field with a client IP address is that
 such an address can be from a private network. One way for a client 
 to set up the host id is to use some hash of the host name. For example:

     for(i = 0, hid=0; i< strlen(hostname); i++) hid +=hostname[i]*i;


  Clients should use a strategy that provides an efficient retransmission
 policy. The recommended way to calculate timeout values between sending
 requests is to use Jacobson algorithm:

     rtt_av  - the smoothed RTT estimator
     rtt_var - the smoothed mean deviation estimator

     Initial values: rtt_av=0, rtt_var=3.0/4.0 (sec)

     delta = measured_RTT - rtt_av;
     rtt_av = rtt_av + (1.0/8.0)*delta
     rtt_var = rtt_var + (1.0/4.0)(|delta|-rtt_var)
     RTO = rtt_av + 4*rtt_var
     (RTO stands for retransmission timeout in seconds)

  When the retransmission timer expires, an exponential backoff should be used
  for the next timeout:
     Initial packet:        timeout=RTO
     First retransmission:  timeout=2*RTO
     Second retransmission: timeout=4*RTO

     


       ************   Application data format  ************


      ---------------------
      |SR|R |     SrD     |
      ---------------------
       1  1    variable

  SR  - Service/Operation/Response (1-byte unsigned integer)
        For requests:  SR=1   get information related to the MD5 hash.
                       SR=2   delete information related to the MD5 hash. 
                       SR=3   add information related to the MD5 hash. 
                       SR=11  get information related to the IP address
        For responses: SR=0   operation was successful
                       SR=128 operation was not successful
                       SR=129 critical error
                       SR=130 server is busy, try again later
   R  - Reserved (1 byte). Must be zero.
 SrD  - Service data depending on the value of SR field and whether
        it is a request or response.

         
   1a) Request, SR=1 - get DB entry related to the supplied MD5 hash 
                SR=2 - remove DB entry related to the supplied MD5 hash

      --------------------------
      |SR|R | C  |     MD5     |
      --------------------------
       1  1   2        16

  C   - Counter (2-byte unsigned integer, network order)-the number 
        of MD5 reports accumulated by frontend servers. Should be 1
        for end-users requests.
  MD5 - MD5 application hash (16 bytes)

    1b)  'Successful' response, SR=0 or request with SR=3 (add operation)

      -----------------------------------------------------------------------
      |SR|R | PVM  |     FLAGS      |   SD    |L |  PINF  |L |  VINF  | PAD |
      -----------------------------------------------------------------------
       1  1    4           16            8     1   < 256   1   < 256     <2


  PVM - Product Version Major (4-byte unsigned integer, network order)
 FLAGS- Flags (16 bytes)
   SD - Sign date (8-byte integer, network order)
    L - Length of the "Product Info" string in bytes (1 byte)
 PINF - "Product Info" string in UTF8 encoding (up to 256 bytes)
    L - Length of the "Vendor Info" string in bytes (1 byte)
 VINF - "Vendor Info" string in UTF8 encoding (up to 256 bytes)
  PAD - Padding byte to match 2-byte boundary (0 or 1 byte)

    1c)  'Not successful' response, SR=128   - MD5 sum not found
                                    SR=129   - critical error
                                    SR=130   - server is busy

      -------
      |SR|R |
      -------
       1  1 

    2a) Request, SR=11 - get information related to the supplied IPs

      ------------------------------
      |SR|N | IP1 | IP2 | .. | IPN |
      ------------------------------
       1  1   4      4          4 

   N  - The number of IP addresses in the request (1 byte, unsigned integer)
 IPi  - IP addresses, i=1,..,N 

    2b) Response //TODO//


     *********************************************************************
     *   Protocol for communication with the Backend server (Source DB)  *
     *********************************************************************


     Two asynchronous data streams are used to communicate data between
     the Frontend Master machine and the Backend server. Each stream is
     a sequence of data frames which have the format similar to those 
     described above for application data.


          --------------                  --------------
     1)   |  Frontend  |   TCP port 7040  |  Backend   |
          |   Master   |   ----------->   |(Source DB) | 
          |            |                  |            |
          --------------                  --------------


          MD5 hashes are reported to the Backend server

      
           |     frame 1        |      frame 2       |
           -------------------------------------------
           |SR|R | C  |   MD5   |SR|R | C  |   MD5   |  * * * *
           -------------------------------------------
            1  1   2      16       

   SR = 1 (always)
   R  = 0 (always)
   C  - Counter (2-byte unsigned integer, network order)
   MD5 - MD5 application hash (16 bytes)


          --------------                  --------------
     2)   |  Frontend  |   TCP port 7026  |  Backend   |
          |   Master   |  <------------   |(Source DB) | 
          |            |                  |            |
          --------------                  --------------


      MD5 related information is supplied to the Frontend master.
      There are two types of frame used.
    
     
      2a)  Frames that relay information related to MD5 hashes:

      -----------------------------------------------------
      |LEN|SR|R |  MD5  |PVM | FLAGS | SD |L |PINF|L |VINF|
      -----------------------------------------------------
        2  1  1    16     4     16     8   1  <256 1  <256

  SR = 3  - add information related to the MD5 hash
  R  = 0  - reserved field

      2b)  Frames that relay data removing instructions;

      -------------------
      |LEN|SR|R |  MD5  |
      -------------------
        2  1  1    16    

  SR = 2  - delete information related to the MD5 hash
  R  = 0  - reserved field
*/

#define KLSRL_PROTO_VERSION 1

#define MAX_PKT_SIZE_UDP 512
#define MAX_PKT_SIZE_TCP 1024

// client request code (SR)
#define OP_GET_MD5      1
#define OP_DEL_MD5      2
#define OP_ADD_MD5      3

#define OP_GET_IP      11

// server response code (SR)
#define RESP_OP_OK      0
#define RESP_OP_NOTOK 128
#define RESP_OP_CRIT  129
#define RESP_OP_BUSY  130

#include <kl_pushpack.h>
struct hips_hdr {
	uint16_t len; // packet length, network order
	uint8_t  ver; // protocol version
	uint8_t  rn;  // re-transition number
	uint32_t sn;  // sequence number, network order
	uint32_t hid; // host id, network order
	uint8_t  sr;  // operation/response
	uint8_t  r;   // reserved
};

/*
1a) Request, SR=1 - get DB entry related to the supplied MD5 hash 
SR=2 - remove DB entry related to the supplied MD5 hash
--------------------------
|SR|R | C  |     MD5     |
--------------------------
1  1   2        16
*/

struct klsrl_md5_query {
	struct hips_hdr hdr;
	uint16_t counter; // C   - Counter (2-byte unsigned integer, network order)-
	// the number of MD5 reports accumulated by front-end servers. 
	// Should be 1 for end-users requests.
	uint8_t  md5[16]; // MD5 - MD5 application hash (16 bytes)
};

/* 
1b) response, SR=0 or request with SR=3 (add operation)
-----------------------------------------------------------------------
|SR|R | PVM  |     FLAGS      |   SD    |L |  PINF  |L |  VINF  | PAD |
-----------------------------------------------------------------------
1  1    4           16            8     1   < 256   1   < 256     <2
*/

struct klsrl_md5_response {
	struct hips_hdr hdr;
	uint32_t prod_ver_mj;   // PVM - Product Version Major (4-byte unsigned integer, network order)
	uint8_t  flags[16];     // FLAGS- Flags (16 bytes)
	uint64_t sign_date;     // SD - Sign date (8-byte integer, network order)
	uint8_t  prod_name_len; // L - Length of the "Product Info" string in bytes (1 byte)
	uint8_t  prod_name[1];  // PINF - "Product Info" string in UTF8 encoding (up to 256 bytes)
	//uint8_t  ven_name_len; // L - Length of the "Vendor Info" string in bytes (1 byte)
	//uint8_t  ven_name[1];  // VINF - "Vendor Info" string in UTF8 encoding (up to 256 bytes)
	// PAD - Padding byte to match 2-byte boundary (0 or 1 byte)
};

#include <kl_poppack.h>

#endif __KLSRL_PROTO_H