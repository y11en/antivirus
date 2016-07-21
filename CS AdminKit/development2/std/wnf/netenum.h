/*!
*		(C) 2002 Kaspersky Lab 
*		
*		\file	netenum.h
*		\brief	
*		
*		\author Андрей Брыксин
*		\date	21.10.2002 17:02:14
*		
*		Example:	
*/		


#ifndef _NET_ENUM_H_
#define _NET_ENUM_H_

#include <winnetwk.h>

#pragma message("network library is linking with \"mpr.lib\"")
#pragma comment(lib, "mpr.lib")

class CNetEnumBase 
{
public:		// constants
	#define _BIT(n)		(1<<n)

	enum {
		GLOBALNET	= _BIT(0),		// search the entire network
		CONNECTED	= _BIT(1),		// search only currently connected resources
		REMEMBERED	= _BIT(2),		// search only "persistent" connections
		CONTEXT		= _BIT(6),		// Enumerate only resources in the network context of the caller

		TYPE_ANY	= _BIT(3),		// search all types of resources
		TYPE_DISK	= _BIT(4),		// search all disk resources
		TYPE_PRINT	= _BIT(5),		// search all print resources

		SEARCHDEFAULT	= _BIT(0) | _BIT(3)
	} ;
	#undef _BIT

protected:		// construction/destruction
	CNetEnumBase(bool bEnumServers) ;		// make this class a base-class.

public:		// attributes
	// The following functions can be used in your overridden
	// "OnHitResource()" member-function (see below), to
	// determine the type of the resource.
	
	// NOTE: Only one of these functions can return true.
	bool			IsServer( NETRESOURCE & ) const ;
	bool			IsDomain( NETRESOURCE & ) const ;
	bool			IsShare( NETRESOURCE & ) const ;
	bool			IsGeneric( NETRESOURCE & ) const ;
	
	// The following functions will simplify the LPNETRESOURCE access:
	const wchar_t*	GetLocalName( NETRESOURCE & ) const ;
	const wchar_t*	GetRemoteName( NETRESOURCE & ) const ;
	const wchar_t*	GetComment( NETRESOURCE & ) const ;
	const wchar_t*	GetProvider( NETRESOURCE & ) const ;
	bool			IsConnectable( NETRESOURCE & ) const ;


public:		// operations
		// Enumerate the network. See OnHitResource() below.
	void		Enumerate(LPNETRESOURCE lpnr = NULL, DWORD dwFlags = CNetEnumBase::SEARCHDEFAULT, DWORD dwUsage = 0);

		// Add a connection to the network.
		// If you want to connect to a local drive ("H:" for instance),
		// you have to fill out the "lpLocalName" of <NetRC> (if this
		// member is NULL or empty).
		// <dwFlags> can be set to "CONNECT_UPDATE_PROFILE" to make the
		// connection persistent (i.e. reconnect when the user logs on).
		// if <UserName> is NULL, it defaults to the current user.
		// if <Password> is NULL, it defaults to the <Username>'s password.
		// This function calls NetError() (see below) and returns false
		// if the connection fails; true on success.
		// See WNetAddConnection2() in the online-help for more details
	void		AddConnection(
					NETRESOURCE & NetRC,
					DWORD Flags = 0,
					wchar_t* UserName = 0,
					wchar_t* Password = 0
				);

	
		// Cancel a network-connection. Returns true on success; false on
		// failure. The NetError() method (see below) will be called on
		// failure. For further information see WNetCancelConnection2()
		// in the online-help.
	void		CancelConnection(
					wchar_t* szName,
					DWORD dwFlags = CONNECT_UPDATE_PROFILE,
					bool ForceDisconnect = false
				);
	void		CancelConnection(
					NETRESOURCE & NetRC,
					DWORD dwFlags = CONNECT_UPDATE_PROFILE,
					bool ForceDisconnect = false
				);

protected:	// overridables
	bool m_bEnumServers;

	// OnHitResource will be called whenever the enumerator finds
	// a net resource. See "Enumerate()" above.
	virtual bool OnHitResource( NETRESOURCE & ) = 0;
	virtual void ThrowNetError(DWORD dwErrorCode);
};


//////////////////////////////////////////////////////////////////////////////
// CNetEnumBase -- inlining ...

inline void CNetEnumBase :: CancelConnection(NETRESOURCE & rNetRC, DWORD dwFlags, bool bForce) {
	CancelConnection(rNetRC.lpLocalName, dwFlags, bForce);
}

inline bool CNetEnumBase :: IsServer( NETRESOURCE & rNetRC_p ) const {
	return (rNetRC_p.dwDisplayType == RESOURCEDISPLAYTYPE_SERVER)
			? true : false ;
}
inline bool CNetEnumBase :: IsDomain( NETRESOURCE & rNetRC_p ) const {
	return (rNetRC_p.dwDisplayType == RESOURCEDISPLAYTYPE_DOMAIN)
			? true : false ;
}

inline bool CNetEnumBase :: IsShare( NETRESOURCE & rNetRC_p ) const {
	return (rNetRC_p.dwDisplayType == RESOURCEDISPLAYTYPE_SHARE)
			? true : false ;
}

inline bool CNetEnumBase :: IsGeneric( NETRESOURCE & rNetRC_p ) const {
	return (rNetRC_p.dwDisplayType == RESOURCEDISPLAYTYPE_GENERIC)
			? true : false ;
}

inline const wchar_t* CNetEnumBase :: GetLocalName( NETRESOURCE & rNetRC_p ) const {
	return rNetRC_p.lpLocalName ;
}

inline const wchar_t* CNetEnumBase :: GetRemoteName( NETRESOURCE & rNetRC_p ) const {
	return rNetRC_p.lpRemoteName ;
}

inline const wchar_t* CNetEnumBase :: GetComment( NETRESOURCE & rNetRC_p ) const {
	return rNetRC_p.lpComment ;
}

inline const wchar_t* CNetEnumBase :: GetProvider( NETRESOURCE & rNetRC_p ) const {
	return rNetRC_p.lpProvider ;
}

inline bool CNetEnumBase :: IsConnectable( NETRESOURCE & rNetRC_p ) const {
	return (
		(rNetRC_p.dwScope == RESOURCE_GLOBALNET)
				&&
		(RESOURCEUSAGE_CONNECTABLE == (rNetRC_p.dwUsage & RESOURCEUSAGE_CONNECTABLE))
	) ? true : false ;
}

//////////////////////////////////////////////////////////////////////////////

template <class T>
class CNetEnum : public CNetEnumBase 
{
	typedef bool (T::*NsFnc)(CNetEnum<T>*, NETRESOURCE &, DWORD);
	
public:
	CNetEnum(bool bEnumServers):
		CNetEnumBase(bEnumServers),
		m_pTheObject(0), 
		m_NsFnc(0) 
	{
	}

    void Create(T * pObj, NsFnc fnc, DWORD data) 
	{
		m_pTheObject = pObj;
		m_NsFnc = fnc;
		Data = data;
    }
protected: // overridables
	T *m_pTheObject;
	NsFnc m_NsFnc;
	DWORD Data;

    virtual bool OnHitResource(NETRESOURCE & rNetRC) {
		return (m_pTheObject->*m_NsFnc)(this,rNetRC,Data);
    }
};

#endif	// Network_h