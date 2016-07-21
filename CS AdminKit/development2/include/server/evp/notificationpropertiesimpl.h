/*!
 * (C) 2002 Kaspersky Lab 
 * 
 * \file	evp/notificationpropertiesimpl.h
 * \author	Mikhail Karmazine
 * \date	19:16 11.12.2002
 * \brief	Серверная имплементация интерфейса NotificationProperties
 */

#ifndef _NOTIFICATION_PROPS_IMPL_H__D5F367FB_8935_401a_944C_3A2352AC5E21__
#define _NOTIFICATION_PROPS_IMPL_H__D5F367FB_8935_401a_944C_3A2352AC5E21__

#include <server/evp/notificationpropertiessrv.h>
#include <srvp/pol/policies.h>
#include <server/tsk/commonsrvtasktools.h>
#include <server/evp/smtplist.h>
#include <server/srvinst/srvinstdata.h>
#include <server/srv_common.h>

namespace KLSRV
{
	class NotificationPropertiesImpl :
		public KLSTD::KLBaseImpl<KLSRV::NotificationPropertiesSrv>
	{
	public:

		NotificationPropertiesImpl();

		virtual ~NotificationPropertiesImpl();

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
			std::wstring & wstrEmailESMTPUserPassword);

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
			const std::wstring & wstrEmailESMTPUserPassword);

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
			bool bDenyDefaults = false);

		virtual void TestNetSend(
			KLDB::DbConnectionPtr pDbConn,
            const std::wstring & wstrNetSendParameter,
            const std::wstring & wstrMessageTemplate,
			bool bDenyDefaults = false);

		virtual void TestScript(
			KLDB::DbConnectionPtr pDbConn,
            const std::wstring & wstrScript,
            const std::wstring & wstrMessageTemplate,
			bool bDenyDefaults = false);

        virtual KLEVP::NotificationLimits GetNotificationLimits();
        
        virtual void SetNotificationLimits( 
			KLDB::DbConnectionPtr pDbConn,
			const KLEVP::NotificationLimits & nl );

		std::wstring GetEventLogMsgTemplate(
			KLDB::DbConnectionPtr pDbConn,
			const KLEVP::MessageArguments& msgArgs);

        std::wstring GetMessageText(
			KLDB::DbConnectionPtr pDbConn, 
			const std::wstring & wstrMsgTemplate, 
			const KLEVP::MessageArguments & msgArgs );

        void SendNotificationMessage(
			KLDB::DbConnectionPtr pDbConn,
            const KLEVP::notification_descr_t2 & nd,
            const KLEVP::notification_descr_t2 & ndDefault,
            const KLEVP::MessageArguments & msgArgs,
            const std::wstring & wstrSubject,
			bool bDenyDefaults,
			bool bSendMailAsinc = true,
			bool bThrow = true);

        void NotificationDescrFromParams(
            const KLSTD::CAutoPtr<KLPAR::Params> pParamsNotification,
            const std::wstring & wstrProductName,
            const std::wstring & wstrProductVersion,
            std::vector<KLEVP::notification_descr_t2>& vectNotificationDescr );

        long SetHostProductProperties(
			KLDB::DbConnectionPtr pDbConn,
            const std::wstring & wstrHostName,
            const std::wstring & wstrProductName,
            const std::wstring & wstrProductVersion,
            const std::vector<KLEVP::notification_descr_t2> & vectNotificationDescr );

        void SetTaskPropertiesEx(
			KLDB::DbConnectionPtr pDbConn,
			const std::wstring& wstrTaskId,
			const std::vector<KLEVP::notification_descr_t2>& vectNotificationDescr);

        void SetPolicyProperties(
			KLDB::DbConnectionPtr pDbConn,
            KLPOL::PolicyId policyId,
            const std::vector<KLEVP::notification_descr_t2>& vectNotificationDescr );

        KLSTD::CAutoPtr<KLSTD::MemoryChunk> EncryptSmtpPassword(const std::wstring& wstrPswd);
		
        std::wstring DecryptSmtpPassword(KLSTD::CAutoPtr<KLSTD::MemoryChunk> pMemoryChunk);
		
		void Initialize( KLSRV::SrvData* pSrvData,
                         CommonSrvGroupTaskTools * pCommonSrvTaskTools );
	private:
		KLSRV::SrvData* m_pSrvData;
        CommonSrvGroupTaskTools * m_pCommonSrvTaskTools;
		std::wstring m_wstrServerHostName;

        void TestNotifications(
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
			const std::wstring & wstrEmailESMTPUserPassword,
			bool bDenyDefaults);

        void NetSend( const std::wstring& wstrNetSend, const std::wstring& wstrDetailedInfo );

        void ExecScript(
            const std::wstring& wstrScriptTargets,
            const std::wstring& wstrScriptWorkingFolder,
            const KLEVP::MessageArguments & msgArgs );

        SMTPServers m_smtpServers;

        void SetPropertiesInternal(
			KLDB::DbConnectionPtr pDbConn,
	        KLPOL::PolicyId policyId,
            const std::wstring & wstrTaskId,
	        const std::vector<KLEVP::notification_descr_t2>& vectNotificationDescr);

        void ResetPolicyHosts(
			KLDB::DbConnectionPtr pDbConn,
			const KLPOL::PolicyId & policyId );

        void DeleteProperties(
			KLDB::DbConnectionPtr pDbConn,
	        const KLPOL::PolicyId & policyId,
            const std::wstring & wstrTaskId);

        void InsertRules(
			KLDB::DbConnectionPtr pDbConn,
            long nHeaderId,
            const std::vector<KLEVP::notification_descr_t2> & vectNotificationDescr );

        void GetPropertiesInternal(
			KLDB::DbConnectionPtr pDbConn,
	        KLPOL::PolicyId policyId,
            const std::wstring & wstrTaskId,
	        std::vector<KLEVP::notification_descr_t2>& vectNotificationDescr );

        void NotificationDescrFromParamsSeverity(
            const KLSTD::CAutoPtr<KLPAR::Params> pParamsNotification,
            const std::wstring & wstrProductName,
            const std::wstring & wstrProductVersion,
            std::vector<KLEVP::notification_descr_t2>& vectNotificationDescr,
            const std::wstring & wstrNFInfoDescrName,
            long nSeverity );

        //void GetSSPath( long nGroupID, std::wstring & wstrRelative, std::wstring & wstrFull );
	};
}

#endif // _NOTIFICATION_PROPS_IMPL_H__D5F367FB_8935_401a_944C_3A2352AC5E21__

// Local Variables:
// mode: C++
// End:
