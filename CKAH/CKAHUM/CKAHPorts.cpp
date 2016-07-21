#include "stdafx.h"
#include <CKAH/ckahum.h>
#include "CKAHMain.h"
#include "PluginUtils.h"
#include "Plugins/Include/fw_ev_api.h"
#include "hook/klick_api.h"

// ports
#pragma pack(push, 1)

struct OpenPortList
{
    int                 size;
    CKAHFW::OpenPort    list[];
};

#pragma pack(pop)

CKAHUM::OpResult CKAHFW::GetOpenPortList ( OUT CKAHFW::HOPENPORTLIST * phlist )
{
    if ( ! phlist )
    {
        return CKAHUM::srInvalidArg;
    }

    unsigned char * buf;
    ULONG bufsize = 65536;
    ULONG retsize = 0;

    if ( ! ( buf = new unsigned char [ bufsize ] ) )
    {
        return CKAHUM::srOpFailed;
    }

    CKAHUM::OpResult res;

    // while ( ( res = SendPluginMessage(CCKAHPlugin::ptFirewall, PLUG_IOCTL, FIREWALL_PLUGIN_NAME, MSG_GET_PORT_LIST3, NULL, 0, buf, bufsize, &retsize ) ) == CKAHUM::srInsufficientBuffer )
	while ( bufsize < sizeof( FW_PORT_LIST_ITEM4 ) * 500 )	
    {
		res = SendPluginMessage(CCKAHPlugin::ptFirewall, PLUG_IOCTL, FIREWALL_PLUGIN_NAME, MSG_GET_PORT_LIST4, NULL, 0, buf, bufsize, &retsize );

		if ( res == CKAHUM::srOK )
			break;
		
		delete [] buf;
		bufsize *= 2;
		
		if ( ! ( buf = new unsigned char [ bufsize ] ) )
		{
			log.WriteFormat (_T("[CKAHFW::GetOpenPortList] Unable to allocate memory(%d bytes for PortList"), PEL_INFO, bufsize);
			return CKAHUM::srOpFailed;
		}		
    }
/*
    if ( res != CKAHUM::srOK )
    {
        delete [] buf;
        return res;
    }
*/

    int entries = retsize / sizeof(FW_PORT_LIST_ITEM4);

    OpenPortList* port_list = (OpenPortList*) new unsigned char [ sizeof(int) + entries * sizeof(CKAHFW::OpenPort) ];

    port_list->size = entries;

    for ( int i = 0; i < entries; ++i )
    {
        const FW_PORT_LIST_ITEM4& src = ((FW_PORT_LIST_ITEM4*)buf)[i];
        CKAHFW::OpenPort& dst = port_list->list[i];

        dst.Proto       = src.Protocol;

        if      (src.LocalIp.IpVer == 4) 
            dst.LocalIP.Setv4 ( src.LocalIp.Ipv4 );
        else if (src.LocalIp.IpVer == 6) 
            dst.LocalIP.Setv6 ( *(CKAHUM::OWord*)src.LocalIp.Ipv6_uchar, src.LocalIp.Zone);

        dst.LocalPort   = src.LocalPort;
        dst.PID         = src.PID;
        wcsncpy (dst.AppName, src.UserAppName, APPNAMELEN);
        dst.AppName[APPNAMELEN - 1] = 0;
        dst.CmdLine[0] = 0;
        wcsncpy (dst.CmdLine, src.CommandLine, CMDLINELEN);
        dst.CmdLine[CMDLINELEN - 1] = 0;
        dst.ActiveTime  = src.ActiveTime;
        dst.InBytes     = src.inBytes;
        dst.OutBytes    = src.outBytes;
    }

    *phlist = port_list;

    delete [] buf;
	
	return CKAHUM::srOK;
}

CKAHUM::OpResult CKAHFW::OpenPortList_GetSize ( IN CKAHFW::HOPENPORTLIST hlist, OUT int * psize )
{
    if ( ! hlist || ! psize )
    {
        return CKAHUM::srInvalidArg;
    }

    *psize = ((OpenPortList*)hlist)->size;
    return CKAHUM::srOK;
}

CKAHUM::OpResult CKAHFW::OpenPortList_GetItem ( IN CKAHFW::HOPENPORTLIST hlist, IN int n, OUT CKAHFW::OpenPort * pport)
{
    if ( ! hlist || n >= ((OpenPortList*)hlist)->size || ! pport )
    {
        return CKAHUM::srInvalidArg;
    }

    *pport = ((OpenPortList*)hlist)->list[n];
    return CKAHUM::srOK;
}


CKAHUM::OpResult CKAHFW::OpenPortList_Delete ( IN CKAHFW::HOPENPORTLIST hlist )
{
    if ( hlist )
    {
        delete [] ( (unsigned char*) hlist );
    }
	return CKAHUM::srOK;
}

// connections

#pragma pack(push, 1)

struct ConnectionList
{
    int                 size;
    CKAHFW::Connection  list[];
};

#pragma pack(pop)

CKAHUM::OpResult CKAHFW::GetConnectionList ( OUT CKAHFW::HCONNLIST * phlist )
{
    if ( ! phlist )
    {
        return CKAHUM::srInvalidArg;
    }

    unsigned char * buf;
    ULONG bufsize = 65536;
    ULONG retsize = 0;

    if ( ! ( buf = new unsigned char [ bufsize ] ) )
    {
        return CKAHUM::srOpFailed;
    }

    CKAHUM::OpResult res;

    while ( ( res = SendPluginMessage(CCKAHPlugin::ptFirewall, PLUG_IOCTL, FIREWALL_PLUGIN_NAME, MSG_GET_CONN_LIST4, NULL, 0, buf, bufsize, &retsize ) ) == CKAHUM::srInsufficientBuffer )
    {
        delete [] buf;
        bufsize *= 2;

        if ( ! ( buf = new unsigned char [ bufsize ] ) )
        {
			log.WriteFormat (_T("[CKAHFW::GetConnectionList] Unable to allocate memory (%d bytes) for ConnList"), PEL_INFO, bufsize);
            return CKAHUM::srOpFailed;
        }
    }

    if ( res != CKAHUM::srOK )
    {
        delete [] buf;
        return res;
    }

    int entries = retsize / sizeof(FW_CONN_LIST_ITEM4);

    ConnectionList* list = (ConnectionList*) new unsigned char [ sizeof(int) + entries * sizeof(CKAHFW::Connection) ];

    list->size = entries;

    for ( int i = 0; i < entries; ++i )
    {
        const FW_CONN_LIST_ITEM4& src = ((FW_CONN_LIST_ITEM4*)buf)[i];
        CKAHFW::Connection& dst = list->list[i];

        dst.Proto       = (UCHAR)src.Protocol;

        if      (src.LocalIp.IpVer == 4)
            dst.LocalIP.Setv4 ( src.LocalIp.Ipv4 );
        else if (src.LocalIp.IpVer == 6)
            dst.LocalIP.Setv6 ( *(CKAHUM::OWord*)src.LocalIp.Ipv6_uchar, src.LocalIp.Zone );

        if      (src.RemoteIp.IpVer == 4)
            dst.RemoteIP.Setv4 ( src.RemoteIp.Ipv4 );
        else if (src.RemoteIp.IpVer == 6)
            dst.RemoteIP.Setv6 ( *(CKAHUM::OWord*)src.RemoteIp.Ipv6_uchar, src.RemoteIp.Zone );

        dst.LocalPort   = src.LocalPort;
        dst.RemotePort  = src.RemotePort;
        dst.PID         = src.PID;
        dst.IsIncoming  = src.isIncoming;
        wcsncpy (dst.AppName, src.UserAppName, APPNAMELEN);
        dst.AppName[APPNAMELEN - 1] = 0;
        dst.CmdLine[0] = 0;
        wcsncpy (dst.CmdLine, src.CommandLine, CMDLINELEN);
        dst.CmdLine[CMDLINELEN - 1] = 0;
        dst.ActiveTime  = src.ActiveTime;
        dst.InBytes     = src.inBytes;
        dst.OutBytes    = src.outBytes;
    }

    *phlist = list;
	
    delete [] buf;

	return CKAHUM::srOK;
}

CKAHUM::OpResult CKAHFW::ConnectionList_GetSize ( IN CKAHFW::HCONNLIST hlist, OUT int * psize )
{
    if ( ! hlist || ! psize )
    {
        return CKAHUM::srInvalidArg;
    }

    *psize = ((ConnectionList*)hlist)->size;
    return CKAHUM::srOK;
}

CKAHUM::OpResult CKAHFW::ConnectionList_GetItem ( IN CKAHFW::HCONNLIST hlist, IN int n, OUT CKAHFW::Connection * pconn )
{
    if ( ! hlist || n >= ((ConnectionList*)hlist)->size || ! pconn )
    {
        return CKAHUM::srInvalidArg;
    }

    *pconn = ((ConnectionList*)hlist)->list[n];
    return CKAHUM::srOK;
}

CKAHUM::OpResult CKAHFW::ConnectionList_Delete ( IN CKAHFW::HCONNLIST hlist )
{
    if ( hlist )
    {
        delete [] ( (unsigned char*) hlist );
    }
	return CKAHUM::srOK;
}                        

CKAHUM::OpResult CKAHFW::BreakConnection ( UCHAR Proto, CKAHUM::IP *pLocalIP, USHORT LocalPort, CKAHUM::IP *pRemoteIP, USHORT RemotePort )
{
    if (!pLocalIP || !pRemoteIP)
        return CKAHUM::srInvalidArg;

    FW_BREAK_CONN_ITEM2 breakconn;

    breakconn.Protocol   = Proto;
    if (pLocalIP->Isv4())
    {
        breakconn.LocalIp.IpVer = 4;
        breakconn.LocalIp.Ipv4  = pLocalIP->v4;
    }
    else
    {
        breakconn.LocalIp.IpVer = 6;
        memcpy(breakconn.LocalIp.Ipv6_uchar, &pLocalIP->v6.O, sizeof(breakconn.LocalIp.Ipv6_uchar));
        breakconn.LocalIp.Zone = pLocalIP->v6.Zone;
    }

    if (pRemoteIP->Isv4())
    {
        breakconn.RemoteIp.IpVer = 4;
        breakconn.RemoteIp.Ipv4  = pRemoteIP->v4;
    }
    else
    {
        breakconn.RemoteIp.IpVer = 6;
        memcpy(breakconn.RemoteIp.Ipv6_uchar, &pRemoteIP->v6.O, sizeof(breakconn.RemoteIp.Ipv6_uchar));
        breakconn.RemoteIp.Zone = pRemoteIP->v6.Zone;
    }

    breakconn.LocalPort  = LocalPort;
    breakconn.RemotePort = RemotePort;
    
    return SendPluginMessage(CCKAHPlugin::ptFirewall, PLUG_IOCTL, FIREWALL_PLUGIN_NAME, MSG_BREAK_CONN2, &breakconn, sizeof(breakconn), NULL, 0, 0 );
}


// ports
#pragma pack(push, 1)
struct HostStatList
{
    ULONG                   ItemCount;
    CKAHFW::HostStatItem    Item[0];
};
#pragma pack(pop)


CKAHUM::OpResult CKAHFW::GetHostStatistics( OUT HHOSTSTATLIST * phlist )
{
    if ( ! phlist )
    {
        return CKAHUM::srInvalidArg;
    }
    
    unsigned char * buf;
    ULONG bufsize = 65536;
    ULONG retsize = 0;
    
    if ( ! ( buf = new unsigned char [ bufsize ] ) )
    {
        return CKAHUM::srOpFailed;
    }

    RtlZeroMemory ( buf, bufsize );
    
    CKAHUM::OpResult res;
    
    while ( ( res = SendPluginMessage(CCKAHPlugin::ptFirewall, PLUG_IOCTL, FIREWALL_PLUGIN_NAME, MSG_GET_HOST_STAT2, NULL, 0, buf, bufsize, &retsize ) ) == CKAHUM::srInsufficientBuffer )
    {
        delete [] buf;
        bufsize *= 2;
        
        if ( ! ( buf = new unsigned char [ bufsize ] ) )
        {
            return CKAHUM::srOpFailed;
        }

        RtlZeroMemory ( buf, bufsize );
    }

    if ( res != CKAHUM::srOK )
    {
        delete [] buf;
        return res;
    }

    unsigned int count = ((HostStatTable2*)buf)->ItemCount;
    HostStatList*   hList = (HostStatList*) new unsigned char [ sizeof(HostStatList) + count*sizeof(CKAHFW::HostStatItem) ];
    
    if ( !hList )
    {
        delete [] buf;
        return CKAHUM::srOpFailed;
    }

    ULONG       ItemCount = 0;
    
    for ( ULONG i = 0; i < ((HostStatTable2*)buf)->ItemCount; i++ )
    {
		const HostStatItem2& src = ((HostStatTable2*)buf)->Item[i];
		CKAHFW::HostStatItem& dst = hList->Item[ItemCount];

        if (src.HostIp.IpVer == 4)
            dst.HostIp.Setv4 ( src.HostIp.Ipv4 );
        else if (src.HostIp.IpVer == 6)
            dst.HostIp.Setv6 ( *(CKAHUM::OWord*)src.HostIp.Ipv6_uchar, src.HostIp.Zone);
        else
            continue;

        dst.TotalStat.inBytes      = src.TotalStat.inBytes;
        dst.TotalStat.outBytes     = src.TotalStat.outBytes;
        dst.TcpStat.inBytes        = src.TcpStat.inBytes;
        dst.TcpStat.outBytes       = src.TcpStat.outBytes;
        dst.UdpStat.inBytes        = src.UdpStat.inBytes;
        dst.UdpStat.outBytes       = src.UdpStat.outBytes;
        dst.BroadcastStat.inBytes  = src.BroadcastStat.inBytes;
        dst.BroadcastStat.outBytes = src.BroadcastStat.outBytes;

        ItemCount++;
    }

    hList->ItemCount = ItemCount;

    delete [] buf;
    *phlist = hList;
    return res;
}

CKAHUM::OpResult CKAHFW::HostStatistics_GetSize(IN HHOSTSTATLIST hlist, OUT int * psize )
{
    if ( ! hlist || ! psize )
    {
        return CKAHUM::srInvalidArg;
    }

    *psize = ((HostStatList*)hlist)->ItemCount;

    return CKAHUM::srOK;
}

CKAHUM::OpResult CKAHFW::HostStatistics_GetItem(IN HHOSTSTATLIST hlist, IN int n, OUT CKAHFW::HostStatItem * phsi )
{
    if ( ! hlist || (ULONG)n >= ((HostStatList*)hlist)->ItemCount || ! phsi )
    {
        return CKAHUM::srInvalidArg;
    }
    struct CKAHFW::HostStatItem* item = &((HostStatList*)hlist)->Item[n];

    *phsi = *item;

    return CKAHUM::srOK;
}

CKAHUM::OpResult CKAHFW::HostStatistics_Delete(IN HHOSTSTATLIST hlist )
{
    if ( hlist )
    {
        delete [] ( (unsigned char*) hlist );
    }
	return CKAHUM::srOK;
}