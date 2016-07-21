#include "stdafx.h"
#include <CKAH/ckahum.h>
#include "CKAHMain.h"
#include "PluginUtils.h"
#include "../../windows/Hook/Plugins/Include/cr_api.h"

CKAHUM::OpResult CKAHCR::Start ()
{
	return CKAHUM_InternalStart (CCKAHPlugin::ptRedirector, g_lpStorage);
}

CKAHUM::OpResult CKAHCR::Stop ()
{
	return CKAHUM_InternalStop (CCKAHPlugin::ptRedirector);
}

CKAHUM::OpResult CKAHCR::Pause ()
{
	return CKAHUM_InternalPause (CCKAHPlugin::ptRedirector);
}

CKAHUM::OpResult CKAHCR::Resume ()
{
	return CKAHUM_InternalResume (CCKAHPlugin::ptRedirector);
}

CKAHUM::OpResult CKAHCR::SetWatchdogTimeout (DWORD timeout_sec)
{
    return SendPluginMessage(CCKAHPlugin::ptRedirector, PLUG_COMMON_IOCTL, REDIRECTOR_PLUGIN_NAME, 
                             MSG_PLUG_SET_WD_TIMEOUT, &timeout_sec, sizeof (timeout_sec), NULL, 0);
}

// регистрирует прокси
CKAHUM::OpResult CKAHCR::RegisterProxy(USHORT port, DWORD pid, OUT HPROXY * phlistener)
{
    if (phlistener) *phlistener = 0;

    CrInitInfo    in;
    CrInitInfoRet out;

    in.m_ListenPort = port;
    in.m_ProxyPID = pid;

    unsigned long ret;

    CKAHUM::OpResult or = SendPluginMessage (CCKAHPlugin::ptRedirector, PLUG_IOCTL, REDIRECTOR_PLUGIN_NAME, 
                                             MSG_CR_INITIALIZE, &in, sizeof (in), &out, sizeof(out), &ret);

    if (or != CKAHUM::srOK) return or;
    if (ret < sizeof(out) || out.m_Status != 0) return CKAHUM::srOpFailed;

    if (phlistener) *phlistener = (HPROXY)out.m_Handle;

    return CKAHUM::srOK;
}

// регистрирует прокси2
CKAHUM::OpResult CKAHCR::RegisterProxy2(USHORT port, USHORT serv_port, DWORD pid, OUT HPROXY * phlistener)
{
    if (phlistener) *phlistener = 0;

    CrInitInfo2    in;
    CrInitInfoRet out;

    in.m_ListenPort = port;
    in.m_ServPort = serv_port;
    in.m_ProxyPID = pid;

    unsigned long ret;

    CKAHUM::OpResult or = SendPluginMessage (CCKAHPlugin::ptRedirector, PLUG_IOCTL, REDIRECTOR_PLUGIN_NAME, 
        MSG_CR_INITIALIZE2, &in, sizeof (in), &out, sizeof(out), &ret);

    if (or != CKAHUM::srOK) return or;
    if (ret < sizeof(out) || out.m_Status != 0) return CKAHUM::srOpFailed;

    if (phlistener) *phlistener = (HPROXY)out.m_Handle;

    return CKAHUM::srOK;
}

// разрегистрирует прокси
CKAHUM::OpResult CKAHCR::Proxy_Unregister(IN HPROXY hproxy)
{
    return SendPluginMessage(CCKAHPlugin::ptRedirector, PLUG_IOCTL, REDIRECTOR_PLUGIN_NAME, 
                             MSG_CR_DONE, &hproxy, sizeof (hproxy), NULL, 0);
}

// добавляет порт в список портов перенаправления
CKAHUM::OpResult CKAHCR::Proxy_AddPort (IN HPROXY hproxy, USHORT port)
{
    CrPortInfo in;

    in.m_Handle = (ULONG)hproxy;
    in.m_PortValue = port;

    return SendPluginMessage(CCKAHPlugin::ptRedirector, PLUG_IOCTL, REDIRECTOR_PLUGIN_NAME, 
                             MSG_CR_ADD_PORT, &in, sizeof (in), NULL, 0);
}

// удаляет порт из списка порта перенаправления
CKAHUM::OpResult CKAHCR::Proxy_RemovePort (IN HPROXY hproxy, USHORT port)
{
    CrPortInfo in;

    in.m_Handle = (ULONG)hproxy;
    in.m_PortValue = port;

    return SendPluginMessage(CCKAHPlugin::ptRedirector, PLUG_IOCTL, REDIRECTOR_PLUGIN_NAME, 
                             MSG_CR_REMOVE_PORT, &in, sizeof (in), NULL, 0);
}

// получить текущий список портов перенаправления
CKAHUM::OpResult CKAHCR::Proxy_GetPortList ( IN HPROXY hproxy, OUT HPORTLIST * phlist )
{
    CrPortListInfo out;

    *phlist = 0;

    unsigned long ret;
    CKAHUM::OpResult or = SendPluginMessage(CCKAHPlugin::ptRedirector, PLUG_IOCTL, REDIRECTOR_PLUGIN_NAME, 
                                            MSG_CR_GET_PORTS, &hproxy, sizeof(hproxy), &out, sizeof (out), &ret);

    if (or != CKAHUM::srOK) return or;
    if (ret < sizeof(out)) return CKAHUM::srOpFailed;

    unsigned long size = sizeof(CrPortListInfo) + sizeof(USHORT) * out.m_TotalPortCount;
    unsigned char * buf;

    if ( ! ( buf = new unsigned char [ size ] ) )
    {
        return CKAHUM::srOpFailed;
    }

    or = SendPluginMessage(CCKAHPlugin::ptRedirector, PLUG_IOCTL, REDIRECTOR_PLUGIN_NAME, 
                           MSG_CR_GET_PORTS, &hproxy, sizeof(hproxy), buf, size, &ret);

    if (or != CKAHUM::srOK) return or;
    if (ret < size) return CKAHUM::srOpFailed;

    *phlist = (HPORTLIST) buf;

    return CKAHUM::srOK;
}

// получить размер списка портов перенаправления
CKAHUM::OpResult CKAHCR::PortList_GetSize ( IN HPORTLIST hlist, OUT int * psize )
{
    if (!hlist) return CKAHUM::srInvalidArg;

    *psize = ((CrPortListInfo*) hlist)->m_PortCount;
    return CKAHUM::srOK;
}

// получить элемент списка портов перенаправления
CKAHUM::OpResult CKAHCR::PortList_GetItem ( IN HPORTLIST hlist, IN int n, OUT USHORT * pport )
{
    if (!hlist) return CKAHUM::srInvalidArg;

    if (n < 0 || (unsigned long)n >= ((CrPortListInfo*) hlist)->m_PortCount) return CKAHUM::srInvalidArg;

    *pport = ((CrPortListInfo*) hlist)->m_Port[n];

    return CKAHUM::srOK;
}

// удалить хэндл списка портов перенаправления
CKAHUM::OpResult CKAHCR::PortList_Delete ( IN HPORTLIST hlist )
{
    delete [] (unsigned char*) hlist;

    return CKAHUM::srOK;
}

CKAHUM::OpResult CKAHCR::Proxy_AddExclude(IN CKAHUM::IP* host, IN USHORT port, IN wchar_t* ImagePath, IN ULONG ImagePathSize, OUT HEXCLUDE* phex)
{    
    if ( ImagePathSize == 0 || ImagePath == NULL )
        return CKAHUM::srOpFailed;
    
    CKAHUM::OpResult or = CKAHUM::srOK;
    unsigned long ret;    

    ULONG          exi_size = sizeof ( CrExcludeInfo2 );
    CrExcludeInfo2* pexi = (CrExcludeInfo2*)malloc ( exi_size ) ;

    if ( pexi )
    {        
        pexi->m_Port  = port;        

        if (host->Isv4())
        {
            pexi->m_Host.IpVer = 4;
            pexi->m_Host.Ipv4 = host->v4;
        }
        else if (host->Isv6())
        {
            pexi->m_Host.IpVer = 6;
            memcpy( pexi->m_Host.Ipv6_uchar, &host->v6.O, sizeof(pexi->m_Host.Ipv6_uchar));
            pexi->m_Host.Zone = host->v6.Zone;
        }
        
        RtlZeroMemory ( pexi->m_Hash, sizeof ( pexi->m_Hash ) );
        
        if ( ImagePath[0] != L'*' )
        {
            or = CKAHFW::GetApplicationChecksum(ImagePath, pexi->m_Hash );
        }

        if ( or == CKAHUM::srOK)
        {
            or = SendPluginMessage(CCKAHPlugin::ptRedirector, PLUG_IOCTL, REDIRECTOR_PLUGIN_NAME, 
                MSG_ADD_EXCLUDE2, pexi, exi_size, phex, sizeof (HEXCLUDE), &ret);
        }
        else
        {
            log.WriteFormat (_T("Unable to get checksum"), PEL_ERROR );
        }

        free ( pexi );
    }    
    
    return or;
}

CKAHUM::OpResult CKAHCR::Proxy_RemoveExclude(IN HEXCLUDE hex )
{
    CKAHUM::OpResult or;
    unsigned long ret;

    or = SendPluginMessage(CCKAHPlugin::ptRedirector, PLUG_IOCTL, REDIRECTOR_PLUGIN_NAME, 
            MSG_REMOVE_EXCLUDE, &hex, sizeof (HEXCLUDE), NULL, 0, &ret);

    return or;
}

CKAHUM::OpResult CKAHCR::Associate_Conn(
                            IN ULONG lIp1, IN USHORT lPort1, IN ULONG rIp1, IN USHORT rPort1, 
                            IN ULONG lIp2, IN USHORT lPort2, IN ULONG rIp2, IN USHORT rPort2 )
{
    CKAHUM::OpResult or;
    unsigned long ret;

    CrAssociateStruct CrStruct;

    CrStruct.lIp1   = lIp1;
    CrStruct.lPort1 = lPort1;
    CrStruct.rIp1   = rIp1;
    CrStruct.rPort1 = rPort1;

    CrStruct.lIp2   = lIp2;
    CrStruct.lPort2 = lPort2;
    CrStruct.rIp2   = rIp2;
    CrStruct.rPort2 = rPort2;

    or = SendPluginMessage(CCKAHPlugin::ptRedirector, PLUG_IOCTL, REDIRECTOR_PLUGIN_NAME, 
        MSG_CR_ASSOCIATE_CONN, &CrStruct, sizeof (CrStruct), NULL, 0, &ret);

    return or;
}

CKAHUM::OpResult CKAHCR::Drv_GetFlags( ULONG* flags )
{
    CKAHUM::OpResult or;
    unsigned long ret;
    ULONG   fl;

    or = SendPluginMessage(CCKAHPlugin::ptRedirector, PLUG_IOCTL, REDIRECTOR_PLUGIN_NAME, 
        MSG_CR_FLAGS, NULL, 0, &fl, sizeof(ULONG), &ret);

    if ( flags )
        *flags = fl;

    return or;
}

CKAHUM::OpResult CKAHCR::Drv_SetQuitEvent()
{
    CKAHUM::OpResult or;
    unsigned long ret;
    or = SendPluginMessage(CCKAHPlugin::ptRedirector, PLUG_IOCTL, REDIRECTOR_PLUGIN_NAME, 
        MSG_PREPARE_EXIT, NULL, 0, NULL, 0, &ret);

    return or;
}
