#include "net_header.h"
#include "kl_object.h"

#define FRAME_IP_PACKET         0x1
#define FRAME_ARP_PACKET        0x2
#define FRAME_IPX_PACKET        0x3

class CKl_Frame : public CKl_Object
{
public:
    CKl_Frame();
    virtual ~CKl_Frame();

    virtual void GetPacket(CKl_PlainPacket& Packet) = 0;
};

class CKl_EthFrame : public CKl_Frame
{
    PETH_HEADER m_EthHeader;
    void*       m_Data;
public:
    CKl_EthFrame();
    virtual ~CKl_EthFrame();

    void GetPacket(CKl_PlainPacket& Packet);
};

class CKl_PlainPacket   : public CKl_Object
{
    void*           m_packet;
    CKl_Frame*      m_frame;
public:
    CKl_PlainPacket();
    virtual ~CKl_PlainPacket() {};

    virtual int FrameType() = 0;
    virtual void GetPacket()
};

class CKl_IpPacket : public CKl_EthFrame
{
public:
    CKl_IpPacket();
    virtual ~CKl_IpPacket();
};