/*!
* (C) 2005 "Kaspersky Lab"
*
* \file filesenderfactory.h
* \author Шияфетдинов Дамир
* \date 2005
* \brief Интерфейс фабрики объектов FileSender
*
*/

#ifndef KLSRVP_FILESENDERFACTORY_H
#define KLSRVP_FILESENDERFACTORY_H

#include <std/base/klbase.h>
#include <std/err/klerrors.h>

#include <std/base/klbaseqi_imp.h>

#include <kca/prcp/proxybase.h>
#include <kca/ft/filesender.h>

namespace KLFT {

	class KLSTD_NOVTABLE FileSenderFactory : public KLSTD::KLBaseQI
	{
	public:
		virtual void CreateFileSender( KLFT::FileSender **ppFileSender) = 0;

	};

	class FileSenderFactoryImp : public FileSenderFactory
	{
	public:
	    void Initialize(
                    KLSTD::KLBaseQI*    pOwner,
                    KLPRCP::CProxyBase* pOwnersProxy);

        KLSTD_INTERAFCE_MAP_REDIRECT(m_pOwner);
	protected:
        KLSTD::KLBaseQI*    m_pOwner;
        KLPRCP::CProxyBase* m_pOwnersProxy;
    public:
		void CreateFileSender( KLFT::FileSender **ppFileSender);
		
	};

} // end namespace KLFT

#endif // KLSRVP_FILESENDERFACTORY_H