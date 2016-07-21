#include "stdafx.h"
#include <CKAH/ckahum.h>
#include "CKAHMain.h"
#include "PluginUtils.h"
#include "../../windows/Hook/Plugins/Include/klstm_api.h"

CKAHUM::OpResult CKAHSTM::Start ()
{
	return CKAHUM_InternalStart (CCKAHPlugin::ptStealthMode, g_lpStorage);
}

CKAHUM::OpResult CKAHSTM::Stop ()
{
	return CKAHUM_InternalStop (CCKAHPlugin::ptStealthMode);
}

CKAHUM::OpResult CKAHSTM::Pause ()
{
	return CKAHUM_InternalPause (CCKAHPlugin::ptStealthMode);
}

CKAHUM::OpResult CKAHSTM::Resume ()
{
	return CKAHUM_InternalResume (CCKAHPlugin::ptStealthMode);
}

CKAHUM::OpResult CKAHSTM::SetStealth(char MacAddr[6], bool Mode)
{
	SET_STEALTH_STRUCT sss;
	memcpy ( sss.MacAddress, MacAddr, sizeof ( MacAddr ) );
	sss.isSet = Mode;

	CKAHUM::OpResult Result = SendPluginMessage(
							CCKAHPlugin::ptStealthMode, 
							PLUG_IOCTL, 
							STEALTH_MODE_PLUGIN_NAME, 
                            STEALTH_SET_STEALTH, 
							&sss, 
							sizeof ( sss ), 
							NULL, 
							0);


	return Result;
}

CKAHUM::OpResult CKAHSTM::AddNet( StmNet* pNet )
{
    if (!pNet)
        return CKAHUM::srInvalidArg;

    if (pNet->Net.Version != pNet->Mask.Version)
        return CKAHUM::srInvalidArg;

	STEALTH_NET2 sn;

    if (pNet->Net.Isv4())
    {
        sn.Net.IpVer = 4;
        sn.Net.Ipv4  = pNet->Net.v4;
        sn.Mask.IpVer = 4;
        sn.Mask.Ipv4  = pNet->Mask.v4;
    }
    else if (pNet->Net.Isv6())
    {
        sn.Net.IpVer = 6;
        memcpy (sn.Net.Ipv6_uchar, &pNet->Net.v6.O, sizeof(sn.Net.Ipv6_uchar));
        sn.Net.Zone = pNet->Net.v6.Zone;
        sn.Mask.IpVer = 6;
        memcpy (sn.Mask.Ipv6_uchar, &pNet->Mask.v6.O, sizeof(sn.Mask.Ipv6_uchar));
        sn.Mask.Zone = pNet->Mask.v6.Zone;
    }

	CKAHUM::OpResult Result = SendPluginMessage(
							CCKAHPlugin::ptStealthMode, 
							PLUG_IOCTL, 
							STEALTH_MODE_PLUGIN_NAME, 
                            STEALTH_ADD_NET2, 
							&sn, 
							sizeof ( sn ), 
							NULL, 
							0);

	return Result;
}

CKAHUM::OpResult CKAHSTM::RemoveNet( StmNet* pNet )
{
    if (!pNet)
        return CKAHUM::srInvalidArg;

    if (pNet->Net.Version != pNet->Mask.Version)
        return CKAHUM::srInvalidArg;

	STEALTH_NET2 sn;

    if (pNet->Net.Isv4())
    {
        sn.Net.IpVer = 4;
        sn.Net.Ipv4  = pNet->Net.v4;
        sn.Mask.IpVer = 4;
        sn.Mask.Ipv4  = pNet->Mask.v4;
    }
    else if (pNet->Net.Isv6())
    {
        sn.Net.IpVer = 6;
        memcpy (sn.Net.Ipv6_uchar, &pNet->Net.v6.O, sizeof(sn.Net.Ipv6_uchar));
        sn.Net.Zone = pNet->Net.v6.Zone;
        sn.Mask.IpVer = 6;
        memcpy (sn.Mask.Ipv6_uchar, &pNet->Mask.v6, sizeof(sn.Mask.Ipv6_uchar));
        sn.Mask.Zone = pNet->Mask.v6.Zone;
    }

	CKAHUM::OpResult Result = SendPluginMessage(
							CCKAHPlugin::ptStealthMode, 
							PLUG_IOCTL, 
							STEALTH_MODE_PLUGIN_NAME, 
                            STEALTH_REMOVE_NET2, 
							&sn, 
							sizeof ( sn ), 
							NULL, 
							0);

	return Result;
}

struct StmNetList
{
    int                  size;
    CKAHSTM::StmNet      list[];
};

CKAHUM::OpResult CKAHSTM::GetNetList(HSTMNETLIST* phList)
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
	
    while ( ( res = SendPluginMessage(
							CCKAHPlugin::ptStealthMode, 
							PLUG_IOCTL, 
							STEALTH_MODE_PLUGIN_NAME, 
							STEALTH_GET_NET_LIST2, 
							NULL, 
							0, 
							buf, 
							bufsize, 
							&retsize ) ) == CKAHUM::srInsufficientBuffer )
    {
        delete [] buf;
        bufsize *= 2;
		
        if ( ! ( buf = new unsigned char [ bufsize ] ) )
        {
            return CKAHUM::srOpFailed;
        }
    }

	if ( res != CKAHUM::srOK )
    {
        delete [] buf;
        return res;
    }

	int entries = PSTEALTH_NET_LIST2(buf)->Count;

	StmNetList* list = (StmNetList*) new unsigned char [ sizeof ( int ) + entries * sizeof ( CKAHSTM::StmNet ) ];

	list->size = entries;
	
	for ( int i = 0; i < entries; ++i )
    {
        if (((PSTEALTH_NET_LIST2)buf)->Net[i].Net.IpVer == 4)
            list->list[i].Net.Setv4 (((PSTEALTH_NET_LIST2)buf)->Net[i].Net.Ipv4);
        else if (((PSTEALTH_NET_LIST2)buf)->Net[i].Net.IpVer == 6)
            list->list[i].Net.Setv6 ( *(CKAHUM::OWord*)((PSTEALTH_NET_LIST2)buf)->Net[i].Net.Ipv6_uchar,
                                      ((PSTEALTH_NET_LIST2)buf)->Net[i].Net.Zone );

        if (((PSTEALTH_NET_LIST2)buf)->Net[i].Mask.IpVer == 4)
            list->list[i].Mask.Setv4 (((PSTEALTH_NET_LIST2)buf)->Net[i].Mask.Ipv4);
        else if (((PSTEALTH_NET_LIST2)buf)->Net[i].Mask.IpVer == 6)
            list->list[i].Mask.Setv6 ( *(CKAHUM::OWord*)((PSTEALTH_NET_LIST2)buf)->Net[i].Mask.Ipv6_uchar,
                                       ((PSTEALTH_NET_LIST2)buf)->Net[i].Mask.Zone );
    }

    *phList = list;

    delete [] buf;
	
	return CKAHUM::srOK;
}

CKAHUM::OpResult CKAHSTM::NetList_GetSize(HSTMNETLIST hList, int* psize)
{
	if ( ! hList || ! psize )
    {
        return CKAHUM::srInvalidArg;
    }

    *psize = ((StmNetList*)hList)->size;
    return CKAHUM::srOK;
}

CKAHUM::OpResult CKAHSTM::NetList_GetItem(HSTMNETLIST hList, int n, StmNet* pNet )
{
	if ( !hList || n >= ((StmNetList*)hList)->size || !pNet )
    {
        return CKAHUM::srInvalidArg;
    }

    *pNet = ((StmNetList*)hList)->list[n];

    return CKAHUM::srOK;
}

CKAHUM::OpResult CKAHSTM::NetList_Delete(HSTMNETLIST hList)
{
	if ( hList )
    {
        delete [] ( (unsigned char*) hList );
    }
	
	return CKAHUM::srOK;
}

CKAHUM::OpResult CKAHSTM::SetNetDefaultReaction( bool NetUsesStm )
{
	STEALTH_NET_REACTION snr = { NetUsesStm };

	CKAHUM::OpResult Result = SendPluginMessage(
							CCKAHPlugin::ptStealthMode, 
							PLUG_IOCTL, 
							STEALTH_MODE_PLUGIN_NAME, 
                            STEALTH_SET_NET_DEFAULT_REACTION, 
							&snr, 
							sizeof ( snr ), 
							NULL, 
							0);

	return Result;
}