#ifndef __KLCONNAPP_CONNAPP_H__
#define __KLCONNAPP_CONNAPP_H__

#include "appinst.h"
#include <common/reporter.h>

namespace KLCONNAPP
{
    class KLSTD_NOVTABLE ConnectorStorage
        :   public KLSTD::KLBaseQI
    {
    public:
        /*!
          \brief	Adds product to list

          \param	szwProduct - product
          \param	szwVersion - version
          \return	true if added, false if product is already in list
          \exception KLERR:::Error* in case of error
        */
        virtual bool Connect(
                        const wchar_t*              szwProduct,
                        const wchar_t*              szwVersion) = 0;

        /*!
          \brief	Removes product from list

          \param	szwProduct - product
          \param	szwVersion - version
          \return	true if removed, false if product is not in list
        */
        KLSTD_NOTHROW virtual bool Disconnect(
                        const wchar_t*              szwProduct,
                        const wchar_t*              szwVersion) throw() = 0;

        /*!
          \brief	Retrieves a product from list

          \param	szwProduct - product
          \param	szwVersion - version
          \return	true if product in list, false if product is not in list
          \exception KLERR:::Error* in case of error
        */
        virtual bool Get(
                        const wchar_t*              szwProduct,
                        const wchar_t*              szwVersion,
                        KLCONNAPP::AppInst**        ppAppInst) = 0;

        /*!
          \brief	Rereads products list, extra connectors are removed                    

          \param	szwProduct - product
          \param	szwVersion - version
          \param	vecProducts [out] vector of products
          \exception KLERR:::Error* in case of error
        */
        virtual void ReConnectAll(
                std::vector<KLPRSS::product_version_t>& vecProducts) = 0;
        
        virtual void Destroy() = 0;
        
        virtual void NotifyFwPortsMayChange() = 0;
    };
};

void KLCONNAPP_CreateConnectorStorage(
                    KLSTD::Reporter*                pReporter,
                    KLPRCI::ComponentInstance*      pInst,
                    long                            lDefTimeout,
                    KLSTD::KLBaseQI*                pNagent,
                    KLCONNAPP::ConnectorStorage**   ppConnectorStorage);

#endif //__KLCONNAPP_CONNAPP_H__
