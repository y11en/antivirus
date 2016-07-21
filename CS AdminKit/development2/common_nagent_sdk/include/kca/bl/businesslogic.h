
#ifndef KLBL_BUSINESSLOGIC_H
#define KLBL_BUSINESSLOGIC_H


namespace KLBL
{
    
    /*! Данное событие следует публиковать при завершении полного
        сканирования компьютера. Событие не имеет параметров.
    */
    const wchar_t c_szwFulscanCompletedEvent[]=L"KLBL_EV_FSCAN_COMPLETED";

    typedef enum
    {
        RtpState_Unknown = 0,
        RtpState_Stopped,
        RtpState_Suspended,
        RtpState_Starting,
        RtpState_Running,        
        RtpState_Running_MaxProtection,
        RtpState_Running_MaxSpeed,
        RtpState_Running_Recomended,
        RtpState_Running_Custom,
        RtpState_Failure
    }
    GlobalRptState;

};

#endif // KLBL_BUSINESSLOGIC_H
