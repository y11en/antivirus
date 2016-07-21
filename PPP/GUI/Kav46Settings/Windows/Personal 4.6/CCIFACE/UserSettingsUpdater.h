#pragma once

#include "DEFS.H"
#include "BaseMarshal.h"
#include "UserSettingsSchedule.h"
#include "UserSettingsLogon.h"
#include "../release_type.h"

#pragma warning(disable : 4251) // 'identifier' : class 'type' needs to have dll-interface to be used by clients of class 'type2'
#pragma warning(disable : 4275) // non – DLL-interface classkey 'identifier' used as base for DLL-interface classkey 'identifier'
#pragma warning(disable : 4786) // 'identifier' : identifier was truncated to 'number' characters in the debug information

namespace Private
{
	//******************************************************************************
	// CProxySettings_v1
	//******************************************************************************
	struct CCIFACEEXPORT CProxySettings_v1 : public CCBaseMarshal<PID_PROXY_SETTINGS>
	{
		enum eProxyProtocol
		{
			eHTTP,
			eFTP,
			eSOCKS4
		};

		// detect proxy setting automatically
		bool m_bAutoDetect;
		// proxy server address (in case no automatic detection)
		std::string m_szHost;
		// proxy server port number (in case no automatic detection)
		USHORT m_sPort;

		CProxySettings_v1()
			: m_sPort(3128), m_bUseAuthenticatonDeprecated(false), m_bAutoDetect(true) {}
		
		virtual PoliType& MarshalImpl(PoliType& pt)
		{
			eProxyProtocol deprecated(eHTTP);

			return BaseMarshal::MarshalImpl(pt)
				<< m_bAutoDetect
				<< m_szHost
				<< m_sPort
				<< m_bUseAuthenticatonDeprecated
				<< m_szUserNameDeprecated
				<< m_szPasswordDeprecated
				<< SIMPLE_DATA_SER ( deprecated );
		}

	protected:
		std::string m_szUserNameDeprecated;
		bool m_bUseAuthenticatonDeprecated;
		std::string m_szPasswordDeprecated;
	};
	
	//******************************************************************************
	// CProxySettings_v2
	//******************************************************************************
	struct CCIFACEEXPORT CProxySettings_v2 : public DeclareNextVersion<CProxySettings_v1>
	{
		bool m_bUsePassiveFTP;
		int	 m_nTimeout;

		CProxySettings_v2 () : m_bUsePassiveFTP(true), m_nTimeout(60){}

		virtual PoliType& MarshalImpl(PoliType &pt)
		{
			bool m_bHttpProxyEnableDeprecated = true;
			return BaseMarshal::MarshalImpl(pt) << m_bHttpProxyEnableDeprecated << m_bUsePassiveFTP << m_nTimeout;
		}
	};

	//******************************************************************************
	// CProxySettings_v3
	//******************************************************************************
	struct CCIFACEEXPORT CProxySettings_v3 : public DeclareNextVersion<CProxySettings_v2>
	{
		CLogonCredentials m_LogonCredentials;

		virtual PoliType& MarshalImpl(PoliType& pt)
		{
			BaseMarshal::MarshalImpl(pt);
			if (pt.is_read_from_pol_() && pt.error_() == PoliType::eNoErrorOldDataVersion)
			{
				m_LogonCredentials.m_bUseCredentials = m_bUseAuthenticatonDeprecated;
				if (m_bUseAuthenticatonDeprecated)
				{
					// Пароль будет зашифрован в CAVPAgentApp::UpgradeSettings()
					m_LogonCredentials.m_ePasswordChanged = CLogonCredentials::ePasswordChanged;
					m_LogonCredentials.m_sPassword = m_szPasswordDeprecated;
					m_LogonCredentials.m_UserName = m_szUserNameDeprecated;
					m_szPasswordDeprecated = "";
					m_szUserNameDeprecated = "";
				}
			}
			else
				pt << m_LogonCredentials;
			return pt;
		}
	};
}

typedef DeclareExternalName<Private::CProxySettings_v3> CProxySettings;

namespace Private
{
	//******************************************************************************
	// CCUpdateSource_v1
	//******************************************************************************
	struct CCIFACEEXPORT CCUpdateSource_v1 : public CCBaseMarshal<PID_UPDATER_SOURCE>
	{
		// Дублируется здесь, чтобы не тащить зависимость от KeepUp2Date/Client/include/update_source.h
		/// the EUpdateSourceType enumerates different source types
		enum EUpdateSourceType
		{
			UST_None = 0,
			/// update using AdminKit transport
			UST_AKServer,
			/// Update from Kaspersky Laboratory source. Kaspersky Laboratory server
			///  addresses are loaded dynamically from configuration files
			UST_KLServer,
			/// updater from user specified source
			UST_UserURL,
			/// update using AdminKit transport from AdminKit Master server
			UST_AKMServer,
				
			UST_ENUM_SIZE
		};

		bool m_bActive;
		EUpdateSourceType m_SourceType;
		std::string m_URL;
		
		virtual PoliType &MarshalImpl(PoliType &pt)
		{
			return BaseMarshal::MarshalImpl(pt) << m_bActive << SIMPLE_DATA_SER(m_SourceType) << m_URL;
		}
	};

	//******************************************************************************
	// CCUpdateSource_v2
	//******************************************************************************
	struct CCIFACEEXPORT CCUpdateSource_v2 : public DeclareNextVersion<CCUpdateSource_v1>
	{
		CCUpdateSource_v2() :
			m_bInetServer(true),
			m_bDontUseProxy(false)
		{		
		}

		// TODO CHECK IF DEPRECATED
		bool m_bInetServer;

		bool m_bDontUseProxy;

		virtual PoliType &MarshalImpl(PoliType &pt)
		{
			return BaseMarshal::MarshalImpl(pt) << m_bInetServer << m_bDontUseProxy;
		}
	};
}
typedef DeclareExternalName<Private::CCUpdateSource_v2> CCUpdateSource;


namespace Private
{
	//******************************************************************************
	//	UpdateModeOptions_v1
	//******************************************************************************
	struct UpdateModeOptions_v1 : public CCBaseMarshal < PID_UPDATE_OPTIONS >
	{
		UpdateModeOptions_v1 ()
        :   m_downloadBases ( true ),
            m_downloadModules ( true ),
            m_downloadModulesOption ( downloadUrgent )
        {
        }

        virtual PoliType & MarshalImpl ( PoliType & pt )
        {
            return BaseMarshal::MarshalImpl ( pt ) << m_downloadBases << m_downloadModules << SIMPLE_DATA_SER ( m_downloadModulesOption );
        }

		enum UpdateModulesOptions
		{
			downloadUrgent,
			downloadAll
		};
		
		bool m_downloadBases;							// download bases
		bool m_downloadModules;							// download modules update
		UpdateModulesOptions m_downloadModulesOption;	// download modules set
	};
}
typedef DeclareExternalName<Private::UpdateModeOptions_v1> UpdateModeOptions;

namespace Private
{
	//******************************************************************************
	//	RetranslationModeOptions_v1
	//******************************************************************************

	struct RetranslationModeOptions_v1 : public CCBaseMarshal < PID_UPDATE_RETRANSLATION >
	{
		RetranslationModeOptions_v1 ()
        :   m_enabled ( false ),
            m_filter ( true )
        {
            m_updateModeOptions.m_downloadModulesOption = UpdateModeOptions::downloadAll;
        }

        virtual PoliType & MarshalImpl ( PoliType & pt )
        {
            return BaseMarshal::MarshalImpl ( pt ) << m_enabled << m_updateModeOptions << m_folder << m_filter;
        }

		bool m_enabled;
        UpdateModeOptions m_updateModeOptions;
		std::string m_folder;

		// the m_filter flag indicates that components and applications should be filtered for retranslation
		bool m_filter;
	};
}

typedef DeclareExternalName<Private::RetranslationModeOptions_v1> RetranslationModeOptions;

namespace Private
{
	//******************************************************************************
	// CUserSettingsUpdater_v1
	//******************************************************************************
	class CCIFACEEXPORT CUserSettingsUpdater_v1 : public CCBaseMarshal<PID_USER_SETTINGS_UPDATER>
	{
	public:
			
		enum EUpdateInterval
		{
			eEveryHour,
			eEvery3Hour,
			eEvery6Hour,
			eEvery12Hour,
			eEvery24Hour,
			eEvery2Days,
			eEvery3Days,
			eEvery7Days
		};

		enum EUpdateType
		{
			eNormal,            // 26.01.2005 (Тимур): смысл этих значений (eNormal, eExtended, eParanoid) изменился,
			eExtended,          // теперь любое из них означает, что обновляться надо из Интернета.
			eParanoid,          // Тип подгружаемых баз теперь задается в структуре CAVSettings в поле m_AVDBType
			eLocalFolder
		};
    
		bool m_bUseNetDetect;
		bool m_bUpdateOn;
		bool m_bAskUser;
		EUpdateInterval m_eUpdateInterval;
		EUpdateType m_TypeDeprecated;
		// m_bPatch is DEPRECATED field, see UpdateModeOptions in CUserSettingsUpdater_v6
		// bool m_bPatch;
		bool m_bOkInfoEventsOff;
		std::vector<std::string> m_URLs;
		std::string m_LocalFolderPath;
    
		CUserSettingsUpdater_v1 ();
		
		virtual PoliType & MarshalImpl ( PoliType & pt )
		{
			bool bPatchFake = false;
			return BaseMarshal::MarshalImpl ( pt )
							<< m_bUseNetDetect
							<< m_bUpdateOn
							<< m_bAskUser
							<< SIMPLE_DATA_SER(m_eUpdateInterval)
							<< bPatchFake
							<< SIMPLE_DATA_SER(m_TypeDeprecated)
							<< m_bOkInfoEventsOff
							<< m_LocalFolderPath
							;
		}
	};

	//******************************************************************************
	// CUserSettingsUpdater_v2
	//******************************************************************************
	class CCIFACEEXPORT CUserSettingsUpdater_v2 : public DeclareNextVersion<CUserSettingsUpdater_v1>
	{
	public:
		CUserSettingsUpdater_v2 ();
		PoliType & MarshalImpl(PoliType& pt)
		{
			//сериализуем только базовую часть
			BaseMarshal::MarshalImpl  (pt);
			if(!(pt.is_read_from_pol_ () && GetDataVersion() < CurrentVersion))
				pt << std::vector<std::string>() << std::vector<std::string>();
			return pt;
		}

		std::vector<std::string> m_PreferredURLs;
		std::vector<std::string> m_ReservedURLs;
	};

	//******************************************************************************
	// CUserSettingsUpdater_v3
	//******************************************************************************
	class CCIFACEEXPORT CUserSettingsUpdater_v3 : public DeclareNextVersion<CUserSettingsUpdater_v2>
	{
	public:	
		CProxySettings m_ProxySettings;
		virtual PoliType & MarshalImpl(PoliType& pt)
		{
			BaseMarshal::MarshalImpl  (pt) << m_ProxySettings;
			m_bUseNetDetect = true;
			return pt;
		}
	};

	//******************************************************************************
	// CUserSettingsUpdater_v4
	//******************************************************************************
	class CCIFACEEXPORT CUserSettingsUpdater_v4 : public DeclareNextVersion<CUserSettingsUpdater_v3>
	{
	public:
		bool m_bLocalFolder;

		typedef std::vector<CCUpdateSource> CCUpdateSources;
		CCUpdateSources m_UpdateSources;

		
		CUserSettingsUpdater_v4 ()
		{
			m_bLocalFolder = false;

            CCUpdateSource UpdateSource;
            UpdateSource.m_bActive = true;

#if defined(__WKS)
            UpdateSource.m_SourceType = CCUpdateSource::UST_AKServer;
            m_UpdateSources.push_back ( UpdateSource );
#endif

            UpdateSource.m_SourceType = CCUpdateSource::UST_KLServer;
            m_UpdateSources.push_back ( UpdateSource );
		}

		//!!!must be called after MarshalImpl
		void ProcessRegistry ();
		
		virtual PoliType & MarshalImpl ( PoliType & pt )
		{
			BaseMarshal::MarshalImpl ( pt );

			if  ( pt.is_read_from_pol_ () && GetDataVersion () < CurrentVersion )
			{
				#if !defined(__PERSONAL_PRO) && !defined(__WKS)
					//don't remove UST_KLServer
					// m_UpdateSources.clear ();
					m_bLocalFolder = (m_TypeDeprecated == eLocalFolder);
				#endif
			}
			else
            {
                bool bPPRODeprecated = false;
                pt << m_bLocalFolder << m_UpdateSources << bPPRODeprecated;
            }
			return pt;
		}
	};


	//******************************************************************************
	// CUserSettingsUpdater_v5
	//******************************************************************************
	class CCIFACEEXPORT CUserSettingsUpdater_v5 : public DeclareNextVersion<CUserSettingsUpdater_v4>
	{
	public:
		CScheduleSettings m_TaskSchedule;
		CLogonCredentials m_LogonInfo;
		std::string m_RegionDeprecated;
		
		CUserSettingsUpdater_v5()
		{
			m_TaskSchedule.m_bUseSchedule = true;

			KLUTIL::CLocTime::GetCurrentTime().CopyTo(m_TaskSchedule.m_stLocalStartTime);

			m_TaskSchedule.m_eScanInterval = CScheduleSettings::eEveryHour;
			m_TaskSchedule.m_nFrequent = 3;
			AdjustScheduleUseNetDetect();
		}

		void AdjustScheduleUseNetDetect()
		{
			m_TaskSchedule.m_bUseNetDetect = false;
			if (!m_bLocalFolder)
			{
				for (CCUpdateSources::const_iterator it = m_UpdateSources.begin()
					, end = m_UpdateSources.end(); it != end; ++it)
				{
					if (!it->m_bActive)
						continue;
					
					// Т.к. первым в списке стоит сервер администирования или папка,
					// то NetDetect не нужен.
					if (it->m_SourceType == CCUpdateSource::UST_AKServer
						|| it->m_SourceType == CCUpdateSource::UST_UserURL && !it->m_bInetServer)
						return;

					if (it->m_SourceType == CCUpdateSource::UST_KLServer
						|| it->m_SourceType == CCUpdateSource::UST_UserURL && it->m_bInetServer)
					{
						m_TaskSchedule.m_bUseNetDetect = true;
						return;
					}
				}
			}
		}

		virtual PoliType & MarshalImpl ( PoliType & pt )
		{
			return BaseMarshal::MarshalImpl(pt) << m_TaskSchedule << m_LogonInfo << m_RegionDeprecated;
		}
	};


	//******************************************************************************
	//	CUserSettingsUpdater_v6
	//******************************************************************************
	class CCIFACEEXPORT CUserSettingsUpdater_v6 : public DeclareNextVersion<CUserSettingsUpdater_v5>
	{
	public:
		CUserSettingsUpdater_v6()
			: m_askUserBeforePatchInstall(true)
			, m_bTryInetValueFromOldWKSPolicy(true)
        {
			m_TaskSchedule.m_bDeferredSchedule = true; // для Updater m_bDeferredSchedule по умолчанию true
        }

        virtual PoliType &MarshalImpl(PoliType &pt)
        {
			BaseMarshal::MarshalImpl(pt);
			// В 6-й версии появился CScheduleSettings_v5 в котором m_bDeferredSchedule по умолчанию false
			// для Updater это неверно
			// также в CScheduleSettings_v5 добавился свой флаг m_bAskUser
			if (pt.is_read_from_pol_() && pt.error_() == PoliType::eNoErrorOldDataVersion)
			{
				m_TaskSchedule.m_bDeferredSchedule = true;
				m_TaskSchedule.m_bAskUser = m_bAskUser;
				return pt;
			}
            return pt << m_updateModeOptions << m_retranslationModeOptions << m_askUserBeforePatchInstall 
					<< m_CountryCode << m_LastDetectedCountryCode << m_bTryInetValueFromOldWKSPolicy;
        }
		
		// download options
		UpdateModeOptions m_updateModeOptions;

        // options related to retranslation
		RetranslationModeOptions m_retranslationModeOptions;

		bool m_askUserBeforePatchInstall;	// whether to ask user before patch install
											// used in service only (do not use in SettingsConverterUpdater)

		std::string m_CountryCode;
		std::string m_LastDetectedCountryCode;
		
		// используется только в AK Connector как хранилище настройки от старого формата политики
		// не использовать в сервисе и GUI
		bool m_bTryInetValueFromOldWKSPolicy;
	};
}
typedef DeclareExternalName<Private::CUserSettingsUpdater_v6> CUserSettingsUpdater;