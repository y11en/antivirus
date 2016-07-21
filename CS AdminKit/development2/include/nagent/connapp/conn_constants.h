#ifndef __CONN_CONSTANTS_H__
#define __CONN_CONSTANTS_H__

//!
#define KLCONN_GUI_CALL_PARAMS  L"KLCONN_GUI_CALL_PARAMS"   //PARAMS_T
#define KLCONN_GUI_CALL_RESULTS L"KLCONN_GUI_CALL_RESULTS"   //PARAMS_T
#define KLCONN_GUI_CALL_RESCODE L"KLCONN_GUI_CALL_RESCODE"   //INT_T

//! Connector statistics

//! Base check violations
#define KLCONN_STP_REPL_CHECKS_BASE     L"KLCONN_STP_REPL_CHECKS_BASE"  //INT_T

//! Compatibility check violations
#define KLCONN_STP_REPL_CHECKS_COMPAT   L"KLCONN_STP_REPL_CHECKS_COMPAT"  //INT_T

//! Replication errors
#define KLCONN_STP_REPL_ERRORS          L"KLCONN_STP_REPL_ERRORS"  //INT_T

//! Last replication error
#define KLCONN_STP_REPL_LASTERROR       L"KLCONN_STP_REPL_LASTERROR"  //PARAMS_T

//! Settings replications
#define KLCONN_STP_NREPL_SETTINGS       L"KLCONN_STP_NREPL_SETTINGS"  //INT_T

//! Policy replications
#define KLCONN_STP_NREPL_POLICY         L"KLCONN_STP_NREPL_POLICY"  //INT_T

//! Task settings replications
#define KLCONN_STP_NREPL_TSKSETTINGS    L"KLCONN_STP_NREPL_TSKSETTINGS"  //INT_T

//! Groups task settings replications
#define KLCONN_STP_NREPL_GTSKSETTINGS   L"KLCONN_STP_NREPL_GTSKSETTINGS"  //INT_T

//! Task state replications
#define KLCONN_STP_NREPL_TSKSTATE       L"KLCONN_STP_NREPL_TSK"  //INT_T

//! Application state replications
#define KLCONN_STP_NREPL_APPSTATE       L"KLCONN_STP_NREPL_APPSTATE"  //INT_T

//! Application props replications
#define KLCONN_STP_NREPL_APPPROPS       L"KLCONN_STP_NREPL_APPPROPS"  //INT_T

//! Replication debug flags
#define KLCONN_STP_REPL_DF              L"KLCONN_STP_REPL_DF"  //INT_T

#endif //__CONN_CONSTANTS_H__
