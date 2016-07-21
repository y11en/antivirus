#ifndef _TCP_
#define _TCP_
#include "..\StdAfx.h"

typedef struct tcphdr {
	__u16	source;
	__u16	dest;
	__u32	seq;
	__u32	ack_seq;
#if defined(__LITTLE_ENDIAN_BITFIELD)
	union{
		struct{
			__u16	res1:4,
					doff:4,
					fin:1,
					syn:1,
					rst:1,
					psh:1,
					ack:1,
					urg:1,
					ece:1,
					cwr:1;
		};
		struct{
			__u16	res1:4,
					doff:4,
					flag:6,
					res2:2;
		}BRIEF;
	};
#elif defined(__BIG_ENDIAN_BITFIELD)
	__u16	doff:4,
		res1:4,
		cwr:1,
		ece:1,
		urg:1,
		ack:1,
		psh:1,
		rst:1,
		syn:1,
		fin:1;
#else
#error	"Adjust your <asm/byteorder.h> defines"
#endif	
	__u16	window;
	__u16	check;
	__u16	urg_ptr;
}tcphdr;


enum {
  TCP_ESTABLISHED = 1,
  TCP_SYN_SENT,
  TCP_SYN_RECV,
  TCP_FIN_WAIT1,
  TCP_FIN_WAIT2,
  TCP_TIME_WAIT,
  TCP_CLOSE,
  TCP_CLOSE_WAIT,
  TCP_LAST_ACK,
  TCP_LISTEN,
  TCP_CLOSING,	 /* now a valid state */

  TCP_MAX_STATES /* Leave at the end! */
};

#define TCP_STATE_MASK	0xF
#define TCP_ACTION_FIN	(1 << 7)

enum {
  TCPF_ESTABLISHED	= (1 << 1),
  TCPF_SYN_SENT		= (1 << 2),
  TCPF_SYN_RECV		= (1 << 3),
  TCPF_FIN_WAIT1	= (1 << 4),
  TCPF_FIN_WAIT2	= (1 << 5),
  TCPF_TIME_WAIT	= (1 << 6),
  TCPF_CLOSE		= (1 << 7),
  TCPF_CLOSE_WAIT	= (1 << 8),
  TCPF_LAST_ACK		= (1 << 9),
  TCPF_LISTEN		= (1 << 10),
  TCPF_CLOSING		= (1 << 11) 
};

/*
 *	The union cast uses a gcc extension to avoid aliasing problems
 *  (union is compatible to any of its members)
 *  This means this part of the code is -fstrict-aliasing safe now.
 */
union tcp_word_hdr { 
	struct tcphdr hdr;
	__u32 		  words[5];
}; 

#define tcp_flag_word(tp) ( ((union tcp_word_hdr *)(tp))->words [3]) 
/*
enum { 
	TCP_FLAG_CWR = __constant_htonl(0x00800000), 
	TCP_FLAG_ECE = __constant_htonl(0x00400000), 
	TCP_FLAG_URG = __constant_htonl(0x00200000), 
	TCP_FLAG_ACK = __constant_htonl(0x00100000), 
	TCP_FLAG_PSH = __constant_htonl(0x00080000), 
	TCP_FLAG_RST = __constant_htonl(0x00040000), 
	TCP_FLAG_SYN = __constant_htonl(0x00020000), 
	TCP_FLAG_FIN = __constant_htonl(0x00010000),
	TCP_RESERVED_BITS = __constant_htonl(0x0FC00000),
	TCP_DATA_OFFSET = __constant_htonl(0xF0000000)
}; 
*/
/* TCP socket options */
#define TCP_NODELAY			1	/* Turn off Nagle's algorithm. */
#define TCP_MAXSEG			2	/* Limit MSS */
#define TCP_CORK			3	/* Never send partially complete segments */
#define TCP_KEEPIDLE		4	/* Start keeplives after this period */
#define TCP_KEEPINTVL		5	/* Interval between keepalives */
#define TCP_KEEPCNT			6	/* Number of keepalives before death */
#define TCP_SYNCNT			7	/* Number of SYN retransmits */
#define TCP_LINGER2			8	/* Life time of orphaned FIN-WAIT-2 state */
#define TCP_DEFER_ACCEPT	9	/* Wake up listener only when data arrive */
#define TCP_WINDOW_CLAMP	10	/* Bound advertised window */
#define TCP_INFO			11	/* Information about this connection. */

#define TCPI_OPT_TIMESTAMPS	1
#define TCPI_OPT_SACK		2
#define TCPI_OPT_WSCALE		4
#define TCPI_OPT_ECN		8

enum tcp_ca_state
{
	TCP_CA_Open = 0,
#define TCPF_CA_Open	(1<<TCP_CA_Open)
	TCP_CA_Disorder = 1,
#define TCPF_CA_Disorder (1<<TCP_CA_Disorder)
	TCP_CA_CWR = 2,
#define TCPF_CA_CWR	(1<<TCP_CA_CWR)
	TCP_CA_Recovery = 3,
#define TCPF_CA_Recovery (1<<TCP_CA_Recovery)
	TCP_CA_Loss = 4
#define TCPF_CA_Loss	(1<<TCP_CA_Loss)
};

struct tcp_info
{
	__u8	tcpi_state;
	__u8	tcpi_ca_state;
	__u8	tcpi_retransmits;
	__u8	tcpi_probes;
	__u8	tcpi_backoff;
	__u8	tcpi_options;
	__u8	tcpi_snd_wscale : 4, tcpi_rcv_wscale : 4;

	__u32	tcpi_rto;
	__u32	tcpi_ato;
	__u32	tcpi_snd_mss;
	__u32	tcpi_rcv_mss;

	__u32	tcpi_unacked;
	__u32	tcpi_sacked;
	__u32	tcpi_lost;
	__u32	tcpi_retrans;
	__u32	tcpi_fackets;

	/* Times. */
	__u32	tcpi_last_data_sent;
	__u32	tcpi_last_ack_sent;     /* Not remembered, sorry. */
	__u32	tcpi_last_data_recv;
	__u32	tcpi_last_ack_recv;

	/* Metrics. */
	__u32	tcpi_pmtu;
	__u32	tcpi_rcv_ssthresh;
	__u32	tcpi_rtt;
	__u32	tcpi_rttvar;
	__u32	tcpi_snd_ssthresh;
	__u32	tcpi_snd_cwnd;
	__u32	tcpi_advmss;
	__u32	tcpi_reordering;
};
DEF(TCP)
DEF(TCP_SES)


#if defined(__LITTLE_ENDIAN_BITFIELD)
#define TCP_FLAG(urg, ack, psh, rst, syn, fin)	(BYTE)((0x01 & urg) | (0x01 & ack)<<1 | (0x01 & psh)<<2 | (0x01 & rst<<3) | (0x01 & syn)<<4 | (0x01 & fin)<<5)
#elif defined(__BIG_ENDIAN_BITFIELD)
#define TCP_FLAG(urg, ack, psh, rst, syn, fin)	(BYTE)((0x01 & urg)<<5 | (0x01 & ack)<<4 | (0x01 & psh)<<3 | (0x01 & rst<<2) | (0x01 & syn)<<1 | (0x01 & fin))
#endif //__LITTLE_ENDIAN_BITFIELD

#endif	/* TCP_ */
