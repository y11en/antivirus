#ifndef __KLNLAHANDLER_H__
#define __KLNLAHANDLER_H__

#include <nagent/nla/klnla.h>
#include <nagent/naginst/naginst.h>

namespace KLNLA
{
    class KLSTD_NOVTABLE NagNlaHandler
        :   public KLSTD::KLBaseQI
    {
    public:
        virtual void Destroy() = 0;

        //! returns true is there are some locations and profiles
        virtual bool IfNlaWorking() = 0;

        //! called by nagent on nagent start or if networks state changes
        virtual void OnNotifyAddrChange() = 0;

        //! returns true if nagent is in roaming mode forced by profile (for use by RoamingModeSwitcher)
        virtual bool IfForcedRoaming() = 0;
        
        //! returns true if nagent is in offline mode
        virtual bool IfOffline() = 0;
    };

    typedef KLSTD::CAutoPtr<NagNlaHandler> NagNlaHandlerPtr;
    

    void CreateNagNlaHandler(
                    KLNAG::NagentCallback*  pNagentCallback,
                    NagNlaHandler**         ppNagNlaHandler);


    /*!
      \brief	Loads nagent's settings to connect to the Administration Server:
                    KLNAG_SERVER_ADDRESS
                    KLNAG_SERVER_PORTS
                    KLNAG_SERVER_SSL_PORTS
                    KLNAG_USE_SSL
                    KLNAG_SSL_CUSTOM_CERT
                    KLNAG_PROXYHOST_USE
                    KLNAG_PROXYHOST_LOCATION
                    KLNAG_PROXYHOST_LOGIN
                    KLNAG_PROXYHOST_PASSWORD
                    

      \param    bForCommonUse specify 'false' if need server connection 
                data for downloading updates, specify 'true' otherwise
      \retval   bOffline returns true if using offline profile
      \retval   ppProfile profile or NULL if using roaming profile
    */
    void KLCSNAGT_DECL LoadNagentProfile(
                            bool            bForCommonUse,
                            bool&           bOffline, 
                            KLPAR::Params** ppProfile);

    

    /*!
      \brief	Cleans profiles. To use when reconnecting to another server.
    */
    void KLCSNAGT_DECL CleanNagentProfiles();
};

#endif //__KLNLAHANDLER_H__
