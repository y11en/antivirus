/*-----------------02.05.99 12:57-------------------
 * Project Prague                                 *
 * Interface internal functions definition        *
 * Author Andrew Andy Petrovitch                  *
 * Copyright (c) Kaspersky Lab                    *
 * Purpose Prague API                             *
 * revision 2                                     *
--------------------------------------------------*/

#ifndef __PR_INT_H
#define __PR_INT_H

/*
 * plugin internal function definitions. any
 * functions in table are optional.
 *
 * if no functions are present it is recommended
 * to drop whole table
 */

typedef tERROR  (pr_call *tIntFnRecognize     )( hOBJECT object, tDWORD type );
typedef tERROR  (pr_call *tIntFnObjectNew     )( hOBJECT object, tBOOL construct );
typedef tERROR  (pr_call *tIntFnObjectInit    )( hOBJECT object );
typedef tERROR  (pr_call *tIntFnObjectInitDone)( hOBJECT object );
typedef tERROR  (pr_call *tIntFnObjectCheck   )( hOBJECT object );
typedef tERROR  (pr_call *tIntFnObjectPreClose)( hOBJECT object );
typedef tERROR  (pr_call *tIntFnObjectClose   )( hOBJECT object );
typedef tERROR  (pr_call *tIntFnChildNew      )( hOBJECT object, hOBJECT* new_obj, tIID iid, tPID pid, tDWORD subtype );
typedef tERROR  (pr_call *tIntFnChildInitDone )( hOBJECT object, hOBJECT child );
typedef tERROR  (pr_call *tIntFnChildClose    )( hOBJECT object, hOBJECT child );
typedef tERROR  (pr_call *tIntFnMsgReceive    )( hOBJECT object, tDWORD msg_cls, tDWORD msg_id, hOBJECT send_point, hOBJECT ctx, hOBJECT recieve_point, tPTR pbuff, tDWORD* blen );
typedef tERROR  (pr_call *tIntFnIFaceTransfer )( hOBJECT object, tDWORD sub_type, tDWORD op_code );
typedef tERROR  (pr_call *tIntFnPropIO        )( hOBJECT object, tDWORD* out_size, tPROPID prop, tPTR buffer, tDWORD count );


// ---
typedef struct siINTERNAL {
  tIntFnRecognize       Recognize;
  tIntFnObjectNew       ObjectNew;
  tIntFnObjectInit      ObjectInit;
  tIntFnObjectInitDone  ObjectInitDone;
  tIntFnObjectCheck     ObjectCheck;
  tIntFnObjectPreClose  ObjectPreClose;
  tIntFnObjectClose     ObjectClose;
  tIntFnChildNew        ChildNew;
  tIntFnChildInitDone   ChildInitDone;
  tIntFnChildClose      ChildClose;
  tIntFnMsgReceive      MsgReceive;
  tIntFnIFaceTransfer   IFaceTransfer;
} iINTERNAL;


// IFaceTransfer function
// 
// Kernel IFaceTransfer call sequence:
// 
// op_code parameter is IFACE_TRANSFER_QUERY
// IFaceTransfer func has to answer if is it possible to convert internal data from one interface subtype to another
// kernel checks returned error code
//   -- for old interface with new subtype 
//   -- for new interface with old subtype
//
// op_code parameter is IFACE_TRANSFER_PERFORM
// IFaceTransfer function has to convert internal data from one interface subtype to another
// kernel doesn't check returned error code
//   -- for old interface with new subtype 
//   -- for new interface with old subtype
// 
// IFaceTransfer operation codes ( op_code parameter )
#define IFACE_TRANSFER_QUERY    (1) // query about possibility to transfrer to/from another subtype
#define IFACE_TRANSFER_PERFORM  (2) // perform transfrer operation

#endif


