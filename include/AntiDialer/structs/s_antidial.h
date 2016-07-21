#ifndef _antidial_task_structs_
#define _antidial_task_structs_

#include <Prague/pr_cpp.h>
#include <Prague/pr_vector.h>

#include <AntiDialer/plugin/p_antidial.h>
#include <ProductCore/structs/s_taskmanager.h>


typedef enum
{
                e_cAntiDialRequest = 1,
} _antidial_structs_ids;


//-----------------------------------------------------------------------------
//! Settings

struct cAntiDialSettings : public cTaskSettings
{
	cAntiDialSettings(): cTaskSettings()
	{}

	DECLARE_IID_SETTINGS(cAntiDialSettings, cTaskSettings, PID_ANTIDIAL);
	cVector<cEnabledStrItem>     m_vAllowPhoneNumbersList;
};

IMPLEMENT_SERIALIZABLE_BEGIN( cAntiDialSettings, 0 )
        SER_BASE( cTaskSettings, 0 )
        SER_VECTOR_M(vAllowPhoneNumbersList,         cEnabledStrItem::eIID)
IMPLEMENT_SERIALIZABLE_END()


//-----------------------------------------------------------------------------
//! Statistics

struct cAntiDialStatistics : public cTaskStatistics
{
        cAntiDialStatistics(): cTaskStatistics(),
                m_nDialsChecked(0),
                m_nDialsBlocked(0)
        {}

        DECLARE_IID_STATISTICS(cAntiDialStatistics, cTaskStatistics, PID_ANTIDIAL);

        tUINT             m_nDialsChecked;                    // Number of dials checked
        tUINT             m_nDialsBlocked;                    // Number of dials blocked
        tUINT             m_nAllowPhoneNumbersListCount;      // Number of phone numbers in allow list
};

IMPLEMENT_SERIALIZABLE_BEGIN( cAntiDialStatistics, 0 )
        SER_BASE( cTaskStatistics, 0 )
        SER_VALUE_M( nDialsChecked,         tid_UINT )
        SER_VALUE_M( nDialsBlocked,         tid_UINT )
        SER_VALUE_M( nAllowPhoneNumbersListCount, tid_UINT )
IMPLEMENT_SERIALIZABLE_END()

//-----------------------------------------------------------------------------
//! Check Request


struct cAntiDialCheckRequest : public cSerializable
{
        cAntiDialCheckRequest(): cSerializable(),
                m_sProcessName(),
                m_sPhoneNumber(),
                m_sEntry(),
				m_bOverride(cFALSE),
				m_nPID(0)
        {}

        DECLARE_IID(cAntiDialCheckRequest, cSerializable, PID_ANTIDIAL, e_cAntiDialRequest);

		tDWORD            m_nPID;          // dialing process id
        cStringObj        m_sProcessName;  // dialing process name
        cStringObj        m_sPhoneNumber;  // phone number to dial          
        cStringObj        m_sEntry;        // entry name
		tBOOL             m_bOverride;     // override phone number specified
};

IMPLEMENT_SERIALIZABLE_BEGIN( cAntiDialCheckRequest, 0 )
        SER_VALUE_M( sProcessName,       tid_STRING_OBJ)
        SER_VALUE_M( nPID,               tid_DWORD)
        SER_VALUE_M( sPhoneNumber,       tid_STRING_OBJ)
        SER_VALUE_M( sEntry,             tid_STRING_OBJ )
        SER_VALUE_M( bOverride,          tid_BOOL)
IMPLEMENT_SERIALIZABLE_END()

#endif // _antidial_task_structs_

