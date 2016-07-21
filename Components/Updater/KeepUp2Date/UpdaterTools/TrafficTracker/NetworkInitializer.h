#if !defined(AFX_NETWORKINITIALIZER_H__EAA5082A_9346_4401_A10D_A98FCDB8776C__INCLUDED_)
#define AFX_NETWORKINITIALIZER_H__EAA5082A_9346_4401_A10D_A98FCDB8776C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


// this class automatically calls WSAStartup() and WSACleanup() and implements scoped
//  locking idiom to simplify cleanup of resources
class NetworkInitializer
{
public:
    // constructor calls WSAStartup()
	NetworkInitializer();
    // destructor calls WSACleanup()
	virtual ~NetworkInitializer();

    // returns true in case network is initialized
    bool initialized()const;

private:
    bool m_initialized;
};

#endif // !defined(AFX_NETWORKINITIALIZER_H__EAA5082A_9346_4401_A10D_A98FCDB8776C__INCLUDED_)
