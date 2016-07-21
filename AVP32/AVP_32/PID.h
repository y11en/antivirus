////////////////////////////////////////////////////////////////////
//
//  PID.H
//  AVP32 Policy IDs
//  Project AVP
//  Alexey de Mont de Rique [graf@avp.ru], Kaspersky Labs. 1999
//  Copyright (c) Kaspersky Labs.
//
////////////////////////////////////////////////////////////////////

#ifndef __PID_H
#define __PID_H


#define PID_AVP32_BASE 7000

#define PID_AVP32_LOC PID_AVP32_BASE
#define PID_AVP32_NET PID_AVP32_BASE+1

/////////////////////////////////////////////////////////////////////////////////////
// AVP32 Property IDs

// Locations
#define PROPID_STR_DISKS		1000
#define PROPID_DW_LOCALDISKS	1001
#define PROPID_DW_FLOPPYDISKS	1002
#define PROPID_DW_NETDISKS		1003

// Actions
#define PROPID_DW_REPONLY		1100
#define PROPID_DW_DISDLG		1101
#define PROPID_DW_DISAUTO		1102
#define PROPID_DW_DELETE		1103
#define PROPID_DW_COPYINF		1104
#define PROPID_DW_COPYSUS		1105
#define PROPID_ST_INFFOLDER		1106
#define PROPID_ST_SUSFOLDER		1107

// Objects

#define PROPID_DW_SCANMEM		1200
#define PROPID_DW_SCANSECT		1201
#define PROPID_DW_SCANFILES		1202
#define PROPID_DW_SCANPACK		1203
#define PROPID_DW_SCANARCH		1204

#define PROPID_DW_SCANMAIL		1205
#define PROPID_DW_SCANMAILPLAIN	1206
#define PROPID_DW_MASKSMART		1207
#define PROPID_DW_MASKPROG		1208
#define PROPID_DW_MASKALL		1209
#define PROPID_DW_MASKUSER		1210
#define PROPID_ST_MASKINC		1211
#define PROPID_ST_MASKEXC		1212
#define PROPID_DW_MASKEXC		1213

// options

#define PROPID_DW_WARNINGS		1300
#define PROPID_DW_CA			1301
#define PROPID_DW_REDUNDANT		1302
#define PROPID_DW_SOUND			1303
#define PROPID_DW_TRACK			1304
#define PROPID_DW_SHOWOK		1305
#define PROPID_DW_SHOWPACK		1306
#define PROPID_DW_REPORT		1314
#define PROPID_DW_LIMIT			1315
#define PROPID_DW_APPEND		1307
#define PROPID_ST_REPORTFILE	1316
#define PROPID_ST_LIMITSIZE		1317
#define PROPID_DW_PRIORITY		1318
#define PROPID_DW_REF_BYTIMER	1340
#define PROPID_DW_REF_BYEVENT	1341
#define PROPID_DW_EVENT			1342
#define PROPID_DW_REF_INTERVAL	1343

#endif