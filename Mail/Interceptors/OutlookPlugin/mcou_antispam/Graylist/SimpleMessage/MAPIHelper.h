#ifndef _MAPIHELPER_H_
#define _MAPIHELPER_H_

#include "../../AntispamOUSupportClass.h"

HRESULT HrCreateSearchKey(              // RETURNS: return code
    IN LPVOID lpObject,                 // pointer to object
    IN LPTSTR lpszAddressType,          // pointer to address type
    IN LPTSTR lpszAddress,              // pointer to address
    OUT LPTSTR *lppszSearchKey);        // pointer to search key

HRESULT HrExpandMessageAddressList(     // RETURNS: return code
    IN LPMESSAGE  lpMessage,            // pointer to message
    OUT ULONG*    lpcMesgAddr,          // count of message addresses
    OUT ULONG*    lpcReplyAddr,         // count of reply recipient addresses
    OUT ULONG*    lpcRecipAddr,         // count of recipient addresses
    OUT LPADRLIST *lppRecipList,        // pointer to recipient address list
    OUT LPADRLIST *lppAdrList);         // pointer to address list

#endif // _MAPIHELPER_H_
