/*-----------------02.05.99 09:31-------------------
 * Project Prague                                 *
 * Subproject FAT Interface                       *
 * Author Andrew Andy Petrovitch                  *
 * Copyright (c) Kaspersky Lab                    *
 * Purpose Prague API                             *
 *                                                *
--------------------------------------------------*/

#ifndef __I_IOFAT_H
#define __I_IOFAT_H

#define prIOFAT_ORIGIN                mPROPERTY_MAKE_REQUIRED( pTYPE_OBJECT,  0x00000001 )
#define prIOFAT_ALLOC_OFFSET          mPROPERTY_MAKE_REQUIRED( pTYPE_DWORD,   0x00000002 )
#define prIOFAT_ALLOC_SIZE            mPROPERTY_MAKE_REQUIRED( pTYPE_DWORD,   0x00000003 )
#define prIOFAT_CLUSTER_SIZE          mPROPERTY_MAKE_REQUIRED( pTYPE_DWORD,   0x00000004 )
#define prIOFAT_START_CLUSTER         mPROPERTY_MAKE_REQUIRED( pTYPE_DWORD,   0x00000005 )
#define prIOFAT_END_CLUSTER           mPROPERTY_MAKE_REQUIRED( pTYPE_DWORD,   0x00000006 )
#define prIOFAT_FAT_OBJECT            mPROPERTY_MAKE_REQUIRED( pTYPE_OBJECT,  0x00000007 )
#define prIOFAT_OBJECT_SIZE           mPROPERTY_MAKE_REQUIRED( pTYPE_DWORD,   0x00000008 )
#define prIOFAT_OBJECT_FIRST_CLUSTER  mPROPERTY_MAKE_REQUIRED( pTYPE_DWORD,   0x00000009 )

#define PID_FAT_WRAPPER      ((tPID)(200))

#endif
