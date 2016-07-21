#if !defined(_CKAHNETWORK_H__INCLUDED_)
#define _CKAHNETWORK_H__INCLUDED_

// Firewall
namespace CKAHFW
{
    const int ADAPTERNAMELEN = 256;
    const int ADAPTERDESCLEN = 256;
    const int PHYSLEN = 8;

    struct Network
    {
        BOOL            HasIPv4;                            // наличие IPv4
        DWORD           IPv4;                               // IPv4-адрес сети
        DWORD           Maskv4;                             // IPv4-маска сети
        BOOL            HasIPv6;                            // наличие IPv6
        CKAHUM::IPv6    IPv6;                               // IPv6-адрес сети
        CKAHUM::IPv6    Maskv6;                             // IPv6-маска сети
        DWORD           Zones[CKAHUM::IPv6::MaxScopes];     // IPv6 zone indices для всех scopes
        CHAR            AdapterName[ADAPTERNAMELEN];        // Имя адаптера
        CHAR            AdapterDesc[ADAPTERDESCLEN];        // Описание адаптера
        DWORD           PhysLength;
        BYTE            Phys[PHYSLEN];                      // MAC-адрес адаптера
    };

    typedef LPVOID HNETWORKLIST;

	// callback-функция уведомления об изменении конфигурации сети
	typedef VOID (CALLBACK *NETWORKCHANGEPROC) (LPVOID lpContext);

    // конфигурация сети

    // установить callback для уведомления об изменении конфигурации сети
	CKAHUMMETHOD SetNetworkChangeCallback ( NETWORKCHANGEPROC NetworkChangeCallback, LPVOID lpContext );

    // получить список сетей
	CKAHUMMETHOD GetNetworkList ( HNETWORKLIST * phlist );

    // получить размер списка сетей
    CKAHUMMETHOD NetworkList_GetSize ( HNETWORKLIST hlist, int * psize );

    // получить элемент списока сетей
    CKAHUMMETHOD NetworkList_GetItem ( HNETWORKLIST hlist, int n, Network * pnet );

    // удалить список сетей
    CKAHUMMETHOD NetworkList_Delete ( HNETWORKLIST hlist );
};

#endif // !defined(_CKAHNETWORK_H__INCLUDED_)
