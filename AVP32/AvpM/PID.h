////////////////////////////////////////////////////////////////////
//
//  PID.H
//  AVPM Policy IDs
//  Project AVP
//  Alexey de Mont de Rique [graf@avp.ru], Kaspersky Labs. 1999
//  Copyright (c) Kaspersky Labs.
//
////////////////////////////////////////////////////////////////////

#ifndef __PID_H
#define __PID_H

#define PID_AVPM_BASE 8000

#define PID_AVPM PID_AVPM_BASE

/////////////////////////////////////////////////////////////////////////////////////
// AVP32 Property IDs

// Actions
#define PROPID_DW_DENY		1100
#define PROPID_DW_CURE		1101
#define PROPID_DW_DELETE	1102
#define PROPID_DW_MBALERT	1103
#define PROPID_DW_ENABLE	1104

// Objects

#define PROPID_ST_MASKINC		1201
#define PROPID_DW_LIMITCOMP		1202

#define PROPID_DW_SCANPACK		1203
#define PROPID_DW_SCANARCH		1204
#define PROPID_DW_SCANMAIL		1205
#define PROPID_DW_SCANMAILPLAIN	1206

#define PROPID_DW_MASKSMART		1207
#define PROPID_DW_MASKPROG		1208
#define PROPID_DW_MASKALL		1209
#define PROPID_DW_MASKUSER		1210
#define PROPID_ST_LIMITCOMPOUNDSIZE	1211
#define PROPID_ST_MASKEXC		1212
#define PROPID_DW_MASKEXC		1213

// Options

#define PROPID_DW_WARNINGS		1312
#define PROPID_DW_CA			1313
#define PROPID_DW_REPORT		1314
#define PROPID_DW_LIMIT			1315
#define PROPID_ST_REPORTFILE	1316
#define PROPID_ST_LIMITSIZE		1317
#define PROPID_DW_REF_BYTIMER	1340
#define PROPID_DW_REF_BYEVENT	1341
#define PROPID_DW_EVENT			1342
#define PROPID_DW_REF_INTERVAL	1343

#endif