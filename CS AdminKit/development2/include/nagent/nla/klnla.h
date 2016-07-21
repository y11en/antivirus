/*!
 * (C) 2003 Kaspersky Lab 
 * 
 * \file	klnla.h
 * \author	Andrew Kazachkov
 * \date	03.09.2007 19:04:01
 * \brief	Network location awareness
 * 
 */

#ifndef __KLNLA_H__
#define __KLNLA_H__

namespace KLNLA
{
    /*! reserved name for so called inactive profile -- a profile with no 
        attempts to connect to the administration server
    */
    const wchar_t c_szwInactiveProfile[] = L".KLNLA_INACTIVE_PROFILE";

    /*!
        Section c_szwSectionNLA
    */
        //! section in policy and SS_SETTINGS
        const wchar_t c_szwSectionNLA[] = L"KLNAG_KLNLA_DATA";

        /*! Locations array. Order of location in array is important. 
            Nagent is switched to the profile that corresponds to the first 
            suitable location in the array. 
        */
        const wchar_t c_szwLocations[] = L"KLNLA_LOCATIONS"; //ARRAY_T|PARAMS_T

        /*
            Network location description
            Different attributes are connected with 'AND'. 

              For example, location with following attributes 
                c_szwDnsDomain = c_szwCOP=NC_ONE_OF, {'mycompany1.com', 'mycompany2.com'};
                c_szwDhcpServer = c_szwCOP=NC_NONE_OF, {'172.16.2.1', '172.16.2.2'};
      
              means c_szwDnsDomain is ONE OF 'mycompany1.com', 'mycompany2.com',
                AND c_szwDhcpServer is NONE OF '172.16.2.1', '172.16.2.2'
        */

        typedef enum
        {
            NC_ONE_OF = 0,  //one of
            NC_NONE_OF,     //none of
        }NLA_COP;

        //! Code of operation
        const wchar_t c_szwCOP[] = L"KLNLA_COP";      //INT_T_T

        //! value
        const wchar_t c_szwValue[] = L"KLNLA_value";

        //! Network location name -- string of any unicode printable symbols, length up to 255, cannot start with dot
        const wchar_t c_szwNetworkLocationName[]= L"KLNLA_LOCATION_NAME"; //STRING_T

        //! Network location Id -- location's unique id 
        const wchar_t c_szwNetworkLocationId[]= L"KLNLA_LOCATION_ID"; //STRING_T

        /*! Name of profile to switch to. May be c_szwInactiveProfile. 
            Location is ignored if specified profile does not exists. 
        */
        const wchar_t c_szwProfileToUse[] = L"KLNLA_PROFILE_TO_USE"; //STRING_T

        //! Location is enabled
        const wchar_t c_szwLocationEnabled[] = L"KLNLA_LOCATION_ENABLED"; //BOOL_T

        /*! Subnet
            +--c_szwSubnet
               +--c_szwCOP
               +--c_szwValue ARRAY_T|PARAMS_T
                  +--[]
                      +--c_szwSubnet_Address
                      +--c_szwSubnet_Mask
        */
        const wchar_t c_szwSubnet[] = L"KLNLA_SUBNET";
    
            //! Subnet address in network format
            const wchar_t c_szwSubnet_Address[] = L"KLNLA_SUBNET_ADDRESS";      //INT_T    
    
            //! Subnet mask in network format
            const wchar_t c_szwSubnet_Mask[] = L"KLNLA_SUBNET_MASK";            //INT_T

        /*! DNS domain
            +--c_szwDnsDomain
               +--c_szwCOP
               +--c_szwValue ARRAY_T|STRING_T
        */
        const wchar_t c_szwDnsDomain[] = L"KLNLA_DNSDOMAIN";

        /*! Default gateway
            +--c_szwDefaultGateway
               +--c_szwCOP
               +--c_szwValue ARRAY_T|INT_T
        */
        const wchar_t c_szwDefaultGateway[] = L"KLNLA_DEF_GATEWAY";

        /*! Default gateway
            +--c_szwDhcpServer
               +--c_szwCOP
               +--c_szwValue ARRAY_T|INT_T
        */
        const wchar_t c_szwDhcpServer[] = L"KLNLA_DHCP_SERVER";

        /*! DnsServer
            +--c_szwDnsServer
               +--c_szwCOP
               +--c_szwValue ARRAY_T|INT_T
        */
        const wchar_t c_szwDnsServer[] = L"KLNLA_DNS_SERVER";

        /*! WINS Server
            +--c_szwWinsServer
               +--c_szwCOP
               +--c_szwValue ARRAY_T|INT_T
        */
        const wchar_t c_szwWinsServer[] = L"KLNLA_WINS_SERVER";
        

        /*! Logged in domain
            +--c_szwLoggedInDomain
               +--c_szwCOP
               +--c_szwValue BOOL_T
        */
        const wchar_t c_szwLoggedInDomain[] = L"KLNLA_IN_DOMAIN";

        //! Profiles array. Order isn't important. 
        const wchar_t c_szwProfiles[] = L"KLNLA_PROFILES"; //ARRAY_T|PARAMS_T

        //! profile name -- unique string of symbols with codes 32-127, length up to 255, cannot start with dot
        const wchar_t c_szwProfileId[] = L"KLNLA_PROFILE_ID"; //STRING_T

        //! profile display name -- string of any unicode printable symbols, length up to 255
        const wchar_t c_szwProfileDisplayName[] = L"KLNLA_PROFILE_DISPLAY_NAME"; //STRING_T
        
        //! true, if nagent must be switched into roaming mode while using the profile
        const wchar_t c_szwRoamingMode[] = L"KLNLA_ROAMING_MODE"; //BOOL_T
        
        /*!
            this variable set true in profile means that administration 
            server address specified in will be used for downloading 
            updates only. 
        */
        const wchar_t c_szwUseServerAddressForUpdatesOnly[] = L"KLNLA_ADDRESS_FOR_UPDATES_ONLY"; //BOOL_T        

        /*
            Network agent profiles description

                c_szwProfileId
                c_szwProfileDisplayName
                c_szwRoamingMode
                c_szwUseServerAddressForUpdatesOnly
                KLNAG_SERVER_ADDRESS
                KLNAG_SERVER_PORTS
                KLNAG_SERVER_SSL_PORTS
                KLNAG_USE_SSL                
                KLNAG_SSL_CUSTOM_CERT       (optional)
                KLNAG_PROXYHOST_USE         (optional)
                KLNAG_PROXYHOST_LOCATION    (optional)
                KLNAG_PROXYHOST_LOGIN       (optional)
                KLNAG_PROXYHOST_PASSWORD    (optional)
        */

    /*!
        Section c_szwCurProfile in private settings
    */
        const wchar_t c_szwCurProfile[] = L"KLNLA_CUR_PROFILE";


        //! obsolete
        const wchar_t c_szwProfileName[] = L"KLNLA_PROFILE_ID";
};

#endif //__KLNLA_H__
