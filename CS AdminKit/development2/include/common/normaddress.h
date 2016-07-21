/*!
 * (C) 2003 Kaspersky Lab 
 * 
 * \file	normaddress.h
 * \author	Andrew Kazachkov
 * \date	12.10.2004 16:29:08
 * \brief	
 * 
 */

#ifndef __KLNORMADDRESS_H__
#define __KLNORMADDRESS_H__

namespace KLADDR
{
    /*!
        checks and converts address from <host>:<port> into 
        http://<host>:<port>,  0 < port < 65536
    */
    KLCSKCA_DECL std::wstring NormalizeAddress(
                        const wchar_t* szwAddress, 
                        bool bPortRequired = true);

    KLCSKCA_DECL std::wstring NormalizeAddress2(
                        const wchar_t* szwAddress, 
                        bool bPortRequired = true, 
                        int nDefPort = 0);
}

#endif //__KLNORMADDRESS_H__
