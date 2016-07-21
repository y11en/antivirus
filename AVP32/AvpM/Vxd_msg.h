////////////////////////////////////////////////////////////////////
//
//  VXD_MSG.H
//  AVPM - AVP95.vxd communication
//  Project AVP
//  Alexey de Mont de Rique [graf@avp.ru], Kaspersky Labs. 1999
//  Copyright (c) Kaspersky Labs.
//
////////////////////////////////////////////////////////////////////

#ifndef __VXD_MSG_H
#define __VXD_MSG_H

#define AVP_VXD_MSGSTRING "AVPVXD95 Control Messages"

#define LOAD_VXD			0x0301
#define UNLOAD_VXD			0x0302
#define OPTIONS_CHENGED		0x0303

#define CLOSE				0x0309
#define ARE_YOU_HERE		0x0311

#define VXD_AVP_MSGSTRING "VXD AVP Messages"

#define VXD_EXE_LOADED		0x0310
#define VXD_LOADED  		0x0303
#define VXD_UNLOADED	    0x0304

#define VXD_GK_LOAD_ERR			0x0305
#define VXD_GK_UNLOAD_ERR    	0x0306
#define VXD_AVP_LOAD_ERR		0x0307
#define VXD_AVP_UNLOAD_ERR		0x0308

#define VXD_CHECK_PRESENTS      0x0330

#endif