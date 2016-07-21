// Debug Levels
enum DEBUG_LEVEL
{
    LEVEL_CRITICAL_ERROR = 0,
    LEVEL_ERROR,
    LEVEL_IMPORTANT,
    LEVEL_NOTIFY,
    LEVEL_INTERESTING,
    LEVEL_FLOOD
};

// ------------------------------------
// ---- Common Debug Bits          ----
// ------------------------------------

#define BIT_INIT						0x00000001
#define BIT_DISPATCH					0x00000002
#define BIT_SHOW_INFO					0x00000004
#define BIT_PLUGINS	    				0x00000008


// ------------------------------------
// ---- Debug Bits for KLIN driver ----
// ------------------------------------

#define BIT_TDI_ACCEPT					0x00000010
#define BIT_TDI_ACTION					0x00000020
#define BIT_TDI_ASSOCIATE_ADDRESS		0x00000040
#define BIT_TDI_CONNECT					0x00000080
#define BIT_TDI_DISASSOCIATE_ADDRESS	0x00000100
#define BIT_TDI_DISCONNECT				0x00000200
#define BIT_TDI_LISTEN					0x00000400
#define BIT_TDI_QUERY_INFORMATION		0x00000800
#define BIT_TDI_RECEIVE					0x00001000
#define	BIT_TDI_RECEIVE_DATAGRAM		0x00002000
#define BIT_TDI_SEND					0x00004000
#define BIT_TDI_SEND_DATAGRAM			0x00008000
#define BIT_TDI_SET_EVENT_HANDLER		0x00010000
#define BIT_TDI_SET_INFORMATION			0x00020000
#define BIT_OPEN_OBJECT					0x00040000
#define BIT_CLOSE_OBJECT				0x00080000
#define BIT_SHUTDOWN					0x00100000
#define BIT_IP_FILTER					0x00200000
#define BIT_AFD_FILTER					0x00400000
#define BIT_MD5_HASH					0x00800000
#define BIT_PROCESS                     0x01000000
#define BIT_TRACK_IRP                   0x02000000
#define BIT_TRACK_MEM                   0x04000000

// -------------------------------------
// ---- Debug Bits for KLICK driver ----
// -------------------------------------

#define BIT_SEND                        0x00000010
#define BIT_RECEIVE                     0x00000020
#define BIT_RECEIVE_COMPLETE            0x00000040
#define BIT_REQUEST                     0x00000080
#define BIT_STATUS                      0x00000100
#define BIT_STATUS_COMPLETE             0x00000200
#define BIT_TRANSFER_COMPLETE           0x00000400
#define BIT_TRANSFER                    0x00000800
#define BIT_SEND_COMPLETE               0x00001000
#define BIT_OPEN_ADAPTER_COMPLETE       0x00002000
#define BIT_CLOSE_ADAPTER_COMPLETE      0x00004000
#define BIT_RESET_COMPLETE              0x00008000

#define BIT_SEND_PACKETS                0x00010000
#define BIT_RESET                       0x00020000

#define BIT_STREAM                      0x00040000


#define BIT_PROTOCOL                    BIT_RECEIVE                 |   \
                                        BIT_STATUS                  |   \
                                        BIT_RECEIVE_COMPLETE        |   \
                                        BIT_STATUS                  |   \
                                        BIT_STATUS_COMPLETE         |   \
                                        BIT_OPEN_ADAPTER_COMPLETE   |   \
                                        BIT_CLOSE_ADAPTER_COMPLETE  |   \
                                        BIT_RESET_COMPLETE          |   \
                                        BIT_TRANSFER_COMPLETE


#define BIT_MINIPORT                    BIT_SEND                    |   \
                                        BIT_REQUEST                 |   \
                                        BIT_SEND_PACKETS            |   \
                                        BIT_RESET                   |   \
                                        BIT_REQUEST                 |   \
                                        BIT_TRANSFER
                                        


#define BIT_ALL							0xFFFFFFFF