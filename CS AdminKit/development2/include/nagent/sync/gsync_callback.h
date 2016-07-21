#ifndef __GSYNC_CALLBACK_H__
#define __GSYNC_CALLBACK_H__

namespace KLGSYN
{
    class KLSTD_NOVTABLE GSynCallback_ServerTime
    {
    public:
        /*!
          \brief	Called before starting gsyn. Callback is called BEFORE gsyn is started so 
                    long processing in the callback may cause timeout error on the administration server

          \param	szwLocation     IN gsyn location
          \param	tmServerTime    IN server time, KLSTD::c_tmInvalidTime if server hasn't sent time
        */
        virtual void OnGsynStarting_ServerTime(
            const wchar_t*  szwLocation,
            time_t          tmServerTime) = 0;
    };

    void Set_GSynCallback(GSynCallback_ServerTime*  pCallback);
};

#endif //__GSYNC_CALLBACK_H__
