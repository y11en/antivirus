////////////////////////////////////////////////////////////////////
//
//  AVP_ALER.H
//  AVP CC Alerts
//  Project AVP
//  Alexey de Mont de Rique [graf@avp.ru], Kaspersky Labs. 1999
//  Copyright (c) Kaspersky Labs.
//
////////////////////////////////////////////////////////////////////

#ifndef __AVP_ALER_H
#define __AVP_ALER_H

#define AVP_ALERT_OBJECT_DETECT      0x0202 // ScanObject*
#define AVP_ALERT_OBJECT_CURE        0x0203 // ScanObject*
#define AVP_ALERT_OBJECT_WARNING     0x0204 // ScanObject*
#define AVP_ALERT_OBJECT_SUSP        0x0205 // ScanObject*
#define AVP_ALERT_OBJECT_DELETE      0x0208 // ScanObject*
#define AVP_ALERT_ERROR_EXT          0x0400 // char* filename
#define AVP_ALERT_ERROR_INT          0x0401 // char* filename

#endif