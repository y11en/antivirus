/*!
 * (C) 2005 Kaspersky Lab 
 * 
 * \file	server/evp/notificationpropertiessrv.h
 * \author	Andrew Lashchenkov
 * \date	19:16 11.12.2002
 * \brief	Серверная интерфейс NotificationProperties
 */

#ifndef _NOTIFICATION_PROPS_SRV_H__D5F367FB_8935_401a_944C_3A2352AC5E21__
#define _NOTIFICATION_PROPS_SRV_H__D5F367FB_8935_401a_944C_3A2352AC5E21__

#include <srvp/evp/notificationproperties.h>
#include <srvp/pol/policies.h>
#include <server/tsk/commonsrvtasktools.h>
#include <server/evp/smtplist.h>
#include <server/db/dbconnection.h>

namespace KLSRV
{
	class NotificationPropertiesSrv : public KLSTD::KLBase{
	public:

		virtual void GetDefaultSettings(
			KLDB::DbConnectionPtr pDbConn,
            std::wstring & wstrSMTPServer,
            long & nSMTPPort,
		    std::wstring & wstrEMail,
            bool & bUseEmail,
		    std::wstring & wstrNetSendParameter,
            bool & bUseNetSend,
		    std::wstring& wstrScript,
            bool & bUseScript,
            std::wstring & wstrMessageTemplate,
			std::wstring & wstrEmailSubject,
			std::wstring & wstrEmailFrom,
			std::wstring & wstrEmailESMTPUserName,
			std::wstring & wstrEmailESMTPUserPassword) = 0;

		virtual void SetDefaultSettings(
			KLDB::DbConnectionPtr pDbConn,
            const std::wstring & wstrSMTPServer,
            long nSMTPPort,
		    const std::wstring & wstrEMail,
            bool bUseEmail,
		    const std::wstring & wstrNetSendParameter,
            bool bUseNetSend,
		    const std::wstring & wstrScript,
            bool bUseScript,
            const std::wstring & wstrMessageTemplate,
			const std::wstring & wstrEmailSubject,
			const std::wstring & wstrEmailFrom,
			const std::wstring & wstrEmailESMTPUserName,
			const std::wstring & wstrEmailESMTPUserPassword) = 0;

		virtual void TestEmail(
			KLDB::DbConnectionPtr pDbConn,
            const std::wstring & wstrSMTPServer,
            long nSMTPPort,
		    const std::wstring & wstrEMail,
            const std::wstring & wstrMessageTemplate,
			const std::wstring & wstrEmailSubject,
			const std::wstring & wstrEmailFrom,
			const std::wstring & wstrEmailESMTPUserName,
			const std::wstring & wstrEmailESMTPUserPassword,
			bool bDenyDefaults = false) = 0;

		virtual void TestNetSend(
			KLDB::DbConnectionPtr pDbConn,
            const std::wstring & wstrNetSendParameter,
            const std::wstring & wstrMessageTemplate,
			bool bDenyDefaults = false) = 0;

		virtual void TestScript(
			KLDB::DbConnectionPtr pDbConn,
            const std::wstring & wstrScript,
            const std::wstring & wstrMessageTemplate,
			bool bDenyDefaults = false) = 0;

        virtual KLEVP::NotificationLimits GetNotificationLimits() = 0;
        
        virtual void SetNotificationLimits( 
			KLDB::DbConnectionPtr pDbConn,
			const KLEVP::NotificationLimits & nl ) = 0;

		virtual std::wstring GetEventLogMsgTemplate(
			KLDB::DbConnectionPtr pDbConn,
			const KLEVP::MessageArguments& msgArgs) = 0;

        virtual std::wstring GetMessageText( 
			KLDB::DbConnectionPtr pDbConn,
			const std::wstring & wstrMsgTemplate, 
			const KLEVP::MessageArguments & msgArgs ) = 0;

        virtual void SendNotificationMessage(
			KLDB::DbConnectionPtr pDbConn,
            const KLEVP::notification_descr_t2 & nd,
            const KLEVP::notification_descr_t2 & ndDefault,
            const KLEVP::MessageArguments & msgArgs,
            const std::wstring & wstrSubject,
			bool bDenyDefaults,
			bool bSendMailAsinc = true,
			bool bThrow = true) = 0;

        virtual void NotificationDescrFromParams(
            const KLSTD::CAutoPtr<KLPAR::Params> pParamsNotification,
            const std::wstring & wstrProductName,
            const std::wstring & wstrProductVersion,
            std::vector<KLEVP::notification_descr_t2>& vectNotificationDescr ) = 0;

        virtual long SetHostProductProperties(
			KLDB::DbConnectionPtr pDbConn,
            const std::wstring & wstrHostName,
            const std::wstring & wstrProductName,
            const std::wstring & wstrProductVersion,
            const std::vector<KLEVP::notification_descr_t2> & vectNotificationDescr ) = 0;

        virtual void SetTaskPropertiesEx(
			KLDB::DbConnectionPtr pDbConn,
			const std::wstring& wstrTaskId,
			const std::vector<KLEVP::notification_descr_t2>& vectNotificationDescr) = 0;

        virtual void SetPolicyProperties(
			KLDB::DbConnectionPtr pDbConn,
            KLPOL::PolicyId policyId,
            const std::vector<KLEVP::notification_descr_t2>& vectNotificationDescr ) = 0;
		
        virtual std::wstring DecryptSmtpPassword(
			KLSTD::CAutoPtr<KLSTD::MemoryChunk> pMemoryChunk ) = 0;

	};
}

#endif // _NOTIFICATION_PROPS_SRV_H__D5F367FB_8935_401a_944C_3A2352AC5E21__

// Local Variables:
// mode: C++
// End:
