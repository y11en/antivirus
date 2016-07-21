/*!
 * (C) 2003 Kaspersky Lab 
 * 
 * \file	klcspwd_inst.h
 * \author	Andrew Kazachkov
 * \date	27.10.2004 16:04:13
 * \brief	
 * 
 */


#ifndef __KLCSPWD_INST_H__
#define __KLCSPWD_INST_H__

#include "./klcspwd_common.h"

namespace KLCSPWD
{
    /*!
      \brief	Initializes data protection. This call must be made from 
                the installer while installing the first product with 
                data protection required.

      \return                        non-zero in case of error
    */
    KLCSPWD_DECL int InstallDataProtection();

    /*!
      \brief	Deinitializes data protection. This call must be made from 
                the installer if there is no products with data 
                protection required.

      \return                        non-zero in case of error
    */
    KLCSPWD_DECL int UninstallDataProtection();
};

#endif //__KLCSPWD_INST_H__
