#include "stdafx.h"
#include <CKAH/ckahum.h>
#include "../../windows/Hook/Plugins/Include/banomet_api.h"
#include "winsock.h"
#include "PluginUtils.h"
#include <CKAH/ckahiptoa.h>

CKAHUMMETHOD CKAHIDS::UnbanHost (CKAHUM::IP *pIp, CKAHUM::IP *pMask)
{
    if (!pIp || !pMask)
        return CKAHUM::srInvalidArg;

    if (pIp->Version != pMask->Version)
        return CKAHUM::srInvalidArg;

	log.WriteFormat (_T("[CKAHIDS::UnbanHost] Unbanning %s/%s"), PEL_INFO, CKAHUM::IPToA(*pIp),CKAHUM::IPMaskToA(*pMask));

	UNBAN_ITEM2 item;
    if (pIp->Isv4())
    {
        item.HostIp.IpVer = 4;
        item.HostIp.Ipv4  = pIp->v4;
        item.Mask.IpVer   = 4;
        item.Mask.Ipv4    = pMask->v4;
    }
    else if (pIp->Isv6())
    {
        item.HostIp.IpVer = 6;
        memcpy( item.HostIp.Ipv6_uchar, &pIp->v6.O, sizeof(item.HostIp.Ipv6_uchar));
        item.HostIp.Zone = pIp->v6.Zone;
        item.Mask.IpVer = 6;
        memcpy( item.Mask.Ipv6_uchar, &pMask->v6.O, sizeof(item.Mask.Ipv6_uchar));
        item.Mask.Zone = pMask->v6.Zone;
    }

	return SendPluginMessage (CCKAHPlugin::ptIDS, PLUG_IOCTL, BANOMET_PLUGIN_NAME, 
								MSG_UNBAN_HOST2, &item, sizeof (item), NULL, 0);
}


struct BannedList
{
    int                  size;
    CKAHIDS::BannedHost  list[];
};

CKAHUMMETHOD CKAHIDS::GetBannedList ( OUT CKAHIDS::HBANNEDLIST * phList )
{
    if ( ! phList )
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

    while ( ( res = SendPluginMessage(CCKAHPlugin::ptIDS, PLUG_IOCTL, BANOMET_PLUGIN_NAME, MSG_GET_BANNED_LIST2, NULL, 0, buf, bufsize, &retsize ) ) == CKAHUM::srInsufficientBuffer )
    {
        delete [] buf;
        bufsize *= 2;

        if ( ! ( buf = new unsigned char [ bufsize ] ) )
        {
            log.WriteFormat (_T("[CKAHIDS::GetBannedList] unable to allocate memory. size = %d, retsize = %d"), PEL_INFO, bufsize, retsize );
            return CKAHUM::srOpFailed;
        }
    }

    if ( res != CKAHUM::srOK )
    {
        log.WriteFormat (_T("[CKAHIDS::GetBannedList] error query list. err = 0x%x"), PEL_INFO, res );

        delete [] buf;
        return res;
    }

    int entries = retsize / sizeof(BAN_LIST_ITEM2);

    BannedList* list = (BannedList*) new unsigned char [ sizeof(int) + entries * sizeof(CKAHFW::OpenPort) ];

    list->size = entries;

    log.WriteFormat (_T("[CKAHIDS::GetBannedList] Got List. Count = %d"), PEL_INFO, list->size );

    for ( int i = 0; i < entries; ++i )
    {
        if (((BAN_LIST_ITEM2*)buf)[i].banned_ip.IpVer == 4)
        {
            list->list[i].IP.Setv4 ( ((BAN_LIST_ITEM2*)buf)[i].banned_ip.Ipv4 );
        }
        else if (((BAN_LIST_ITEM2*)buf)[i].banned_ip.IpVer == 6)
        {
            list->list[i].IP.Setv6 ( *(CKAHUM::OWord*)((BAN_LIST_ITEM2*)buf)[i].banned_ip.Ipv6_uchar,
                                     ((BAN_LIST_ITEM2*)buf)[i].banned_ip.Zone );
        }

        list->list[i].BanTime = ((BAN_LIST_ITEM2*)buf)[i].ban_from;
    }

    *phList = list;

    delete [] buf;
	
	return CKAHUM::srOK;
}

CKAHUMMETHOD CKAHIDS::BannedList_GetSize ( IN CKAHIDS::HBANNEDLIST hList, OUT int * psize )
{
    if ( ! hList || ! psize )
    {
        log.WriteFormat (_T("[CKAHIDS::BannedList_GetSize] Invalid arg"), PEL_INFO );

        return CKAHUM::srInvalidArg;
    }

    *psize = ((BannedList*)hList)->size;

    return CKAHUM::srOK;
}

CKAHUMMETHOD CKAHIDS::BannedList_GetItem ( IN CKAHIDS::HBANNEDLIST hList, IN int n, OUT CKAHIDS::BannedHost * pBannedHost )
{
    if ( ! hList || n >= ((BannedList*)hList)->size || ! pBannedHost )
    {
        log.WriteFormat (_T("[CKAHIDS::BannedList_GetItem] Invalid arg. n = %d, size = %d"), PEL_INFO, n , ((BannedList*)hList)->size );
        return CKAHUM::srInvalidArg;
    }

    *pBannedHost = ((BannedList*)hList)->list[n];
    return CKAHUM::srOK;
}

CKAHUMMETHOD CKAHIDS::BannedList_Delete ( IN CKAHIDS::HBANNEDLIST hList )
{
    if ( hList )
    {
        delete [] ( (unsigned char*) hList );
    }
	return CKAHUM::srOK;
}
