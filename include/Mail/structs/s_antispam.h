/*!
*		
*		
*		(C) 2002 Kaspersky Lab 
*		
*		\file	trafficmonitor_types.h
*		\brief	Здесь задаются типы настроек перехватчика траффик-монитора
*		
*		\author Vitaly DVi Denisov
*		\date	11.03.2004 17:32:41
*		
*		Example:	
*		
*		
*		
*/		

#ifndef _S_ANTISPAM_H_
#define _S_ANTISPAM_H_

#include <Mail/plugin/p_antispamtask.h>
#include <ProductCore/common/MatchStrings.h>
#include <ProductCore/structs/s_taskmanager.h>
#ifdef __unix__
#include <cctype>
#endif

typedef enum 
{
	asstruct_AS_TRAIN_PARAMS = 1,
	asstruct_AS_REPORT,
	asstruct_AS_PERSISTANCE,
	asstruct_AS_ASK,
	asstruct_AS_STATISTICS_EX,
	asstruct_AS_TRAIN_PARAMS_EX,
	asstruct_AS_ADDRESSLIST,
	asstruct_AS_ADDRESSLISTS,
	asstruct_cAsBwListRule,
	asstruct_cAsBwList,
	asstruct_cAsBwLists,
	asstruct_AS_ASK_PLUGIN,
	asstruct_cAsTrigger,
	asstruct_cAsVerdict,
	asstruct_LAST
} asstructs_t;

struct cAsBwListRule : public cSerializable
{
	cAsBwListRule() :
		cSerializable(),
		m_bEnabled(cTRUE),
		m_dwLevel(100),
		m_bNew(cFALSE)
		{}
		
	cAsBwListRule(cStringObj& sRule, tBOOL bEnabled, tBOOL bNew, tBOOL bIsSubst=cFALSE) :
		cSerializable(),
		m_sRule(bIsSubst ? "" : sRule),
		m_bEnabled(bEnabled),
		m_dwLevel(100),
		m_bNew(bNew)
		{
			if ( bIsSubst )
			{
				m_sRule = "*";
				m_sRule += sRule;
				m_sRule += "*";
			}
		}
		
	cAsBwListRule(tSTRING sRule, tBOOL bEnabled, tBOOL bNew, tBOOL bIsSubst=cFALSE) :
		cSerializable(),
		m_sRule(bIsSubst ? "" : sRule),
		m_bEnabled(bEnabled),
		m_dwLevel(100),
		m_bNew(bNew)
		{
			if ( bIsSubst )
			{
				m_sRule = "*";
				m_sRule += sRule;
				m_sRule += "*";
			}
		}
	
	DECLARE_IID( cAsBwListRule, cSerializable, PID_ANTISPAMTASK, asstruct_cAsBwListRule );

	tBOOL      m_bEnabled;   // Правило применяется?
	cStringObj m_sRule;      // Значение правила
	tDWORD     m_dwLevel;    // Уровень значимости правила (в процентах)
	tBOOL      m_bNew;       // Эта запись была добавлена внутри задачи. (не сериализуется)

	bool operator ==(const cAsBwListRule & o) 
	{ 
		if (m_bEnabled != o.m_bEnabled)
			return false;
		cStringObj sRule1 = o.m_sRule; sRule1.toupper();
		cStringObj sRule2 = m_sRule;   sRule2.toupper();
//		bool b = patmat(sRule1.c_str(cCP_ANSI), sRule2.c_str(cCP_ANSI));
		cStringPtr pRule1(sRule1);
		cStringPtr pRule2(sRule2);
		bool b = MatchStrings(pRule1, pRule2);
		return b;
	}
};

IMPLEMENT_SERIALIZABLE_BEGIN( cAsBwListRule, 0 )
	SER_VALUE(  m_bEnabled,   tid_BOOL,       "Enabled" )
	SER_VALUE(  m_sRule,      tid_STRING_OBJ, "Rule" )
	SER_VALUE(  m_dwLevel,    tid_DWORD,      "Level" )
IMPLEMENT_SERIALIZABLE_END();

typedef cVector<cAsBwListRule> cAsBwListRules;

//-----------------------------------------------------------------------------
//

struct cAsTrigger : public cSerializable
{
	cAsTrigger(tDWORD dwLevel = 80, tBOOL bEnabled = cFALSE) :
		cSerializable(),
		m_bEnabled(bEnabled),
		m_dwLevel(dwLevel)
		{}
		
	DECLARE_IID( cAsTrigger, cSerializable, PID_ANTISPAMTASK, asstruct_cAsTrigger );

	tBOOL      m_bEnabled;   // Правило применяется?
	tDWORD     m_dwLevel;    // Уровень значимости правила (в процентах)
};

IMPLEMENT_SERIALIZABLE_BEGIN( cAsTrigger, 0 )
	SER_VALUE(  m_bEnabled,   tid_BOOL,       "Enabled" )
	SER_VALUE(  m_dwLevel,    tid_DWORD,      "Level" )
IMPLEMENT_SERIALIZABLE_END();

//-----------------------------------------------------------------------------
//

struct cAsBwList : public cTaskSettings
{
	cAsBwList() :
		cTaskSettings(),
		m_dwBlackWordsLimit(100),
		m_dwRecipLimit(100),
		m_dwNonLatLimit(50),
		m_bRemoveScripts(cFALSE),
		m_dwTriggersLimit(100),
		m_bShowMailwasher(cTRUE),
		m_bUseEmptyValidate(100),
		m_bUseNotForMeValidate(100),
		m_bUseCannotBeSpamValidate(100, cTRUE)
	{
		m_bUseEmptyValidate.m_dwLevel = 100;
		m_bUseNotForMeValidate.m_dwLevel = 100;
	}
		
	DECLARE_IID( cAsBwList, cTaskSettings, PID_ANTISPAMTASK, asstruct_cAsBwList );
	
	cAsBwListRules m_aMyAddrs;
	cAsBwListRules m_aWhiteAddrs;
	cAsBwListRules m_aWhiteWords;
	cAsBwListRules m_aBlackAddrs;
	cAsBwListRules m_aBlackWords;
	tDWORD         m_dwBlackWordsLimit;   // Порог признания письма спамом по черным фразам (в процентах. Процентов м.б. > 100)
	tDWORD         m_dwNonLatLimit;       // Максимальное соотношение латинских и других символов
	tDWORD         m_dwRecipLimit;        // Максимальное количество получателей для неспамового письма (считаются по полям To и Cc)
	tBOOL          m_bRemoveScripts;				    // Удалять все скрипты из писем

	// triggers
	cAsTrigger     m_bUseURLChecking;            // Проверка рекламных URL'ов (опознавать их как 100% спам)
//	cAsTrigger     m_bUseHTMLValidate;           // Проверка HTML на соответствие (несоответствием считается присутствие незнакомых тегов, скрытого текста, комментариев и т.д.)
	cAsTrigger     m_bUseHTMLValidate_UnknownDefsCount;   // Количество несуществующих тегов
	cAsTrigger     m_bUseHTMLValidate_SeemsColors;        // Количество блоков текста с цветом фона
	cAsTrigger     m_bUseHTMLValidate_SmallText;          // Количество блоков с очень маленьким текстом
	cAsTrigger     m_bUseHTMLValidate_InvisibleCharCount; // Количество невидимых символов
	cAsTrigger     m_bUseHTMLValidate_Scripts;            // Количество скриптов
	cAsTrigger     m_bUseHTMLValidate_HiddenElements;     // Количество скрытых элементов (style="visibility:hidden")
	cAsTrigger     m_bUseObjectsValidate;        // Проверка на наличие объектов при отсутствии текста в теле письма 
	cAsTrigger     m_bUseExternalObjectsValidate;// Проверка на наличие внешних объектов в теле письма
	cAsTrigger     m_bUseEmptyValidate;          // Письмо без темы и без содержимого считается спамом
	cAsTrigger     m_bUseNotForMeValidate;       // Поиск моих адресов в полях To и Cc
	cAsTrigger     m_bUseNonLatLimit;			 // Считать спамом письма с нелатинскими символами
	cAsTrigger     m_bUseRecipLimit;			 // Считать спамом письма с количеством получателей > m_dwRecipLimit
	cAsTrigger     m_bUseCannotBeSpamValidate;   // Письмо, не прошедшее через SMTP-сервер (X400, например), не может быть спамом
	tDWORD         m_dwTriggersLimit;			 // Порог признания письма спамом по триггерам (в процентах. Процентов м.б. > 100)

	tBOOL          m_bShowMailwasher;
};

IMPLEMENT_SERIALIZABLE_BEGIN( cAsBwList, 0 )
	SER_BASE( cTaskSettings, 0 )
	SER_VECTOR( m_aMyAddrs,                   cAsBwListRule::eIID,    "MyAddrs" )
	SER_VECTOR( m_aWhiteAddrs,                cAsBwListRule::eIID,    "WhiteAddrs" )
	SER_VECTOR( m_aWhiteWords,                cAsBwListRule::eIID,    "WhiteWords" )
	SER_VECTOR( m_aBlackAddrs,                cAsBwListRule::eIID,    "BlackAddrs" )
	SER_VECTOR( m_aBlackWords,		          cAsBwListRule::eIID,    "BlackWords" )
	SER_VALUE(  m_dwBlackWordsLimit,          tid_DWORD,              "BlackWordsLimit" )
	SER_VALUE(  m_dwNonLatLimit,              tid_DWORD,              "NonLatLimit" )
	SER_VALUE(  m_dwRecipLimit,               tid_DWORD,              "RecipLimit" )
	SER_VALUE( m_bRemoveScripts,              tid_BOOL,               "RemoveScripts" )
	SER_VALUE( m_bUseURLChecking,             cAsTrigger::eIID,       "UseURLChecking" )
	SER_VALUE( m_bUseHTMLValidate_UnknownDefsCount,  cAsTrigger::eIID,"UseHTMLValidate_UnknownDefsCount" )
	SER_VALUE( m_bUseHTMLValidate_SeemsColors,       cAsTrigger::eIID,"UseHTMLValidate_SeemsColors" )
	SER_VALUE( m_bUseHTMLValidate_SmallText,         cAsTrigger::eIID,"UseHTMLValidate_SmallText" )
	SER_VALUE( m_bUseHTMLValidate_InvisibleCharCount,cAsTrigger::eIID,"UseHTMLValidate_InvisibleCharCount" )
	SER_VALUE( m_bUseHTMLValidate_Scripts,           cAsTrigger::eIID,"UseHTMLValidate_Scripts" )
	SER_VALUE( m_bUseHTMLValidate_HiddenElements,    cAsTrigger::eIID,"UseHTMLValidate_HiddenElements" )
	SER_VALUE( m_bUseObjectsValidate,         cAsTrigger::eIID,       "UseObjectsValidate" )
	SER_VALUE( m_bUseExternalObjectsValidate, cAsTrigger::eIID,       "UseExternalObjectsValidate" )
	SER_VALUE( m_bUseEmptyValidate,           cAsTrigger::eIID,       "UseEmptyValidate" )
	SER_VALUE( m_bUseNotForMeValidate,        cAsTrigger::eIID,       "UseNotForMeValidate" )
	SER_VALUE( m_bUseNonLatLimit,             cAsTrigger::eIID,       "UseNonLatLimit" )
	SER_VALUE( m_bUseRecipLimit,              cAsTrigger::eIID,       "UseRecipLimit" )
	SER_VALUE( m_bUseCannotBeSpamValidate,    cAsTrigger::eIID,       "UseCannotBeSpamValidate" )
	SER_VALUE( m_dwTriggersLimit,             tid_DWORD,              "TriggersLimit" )
	SER_VALUE( m_bShowMailwasher,             tid_BOOL,               "ShowMailwasher" )
IMPLEMENT_SERIALIZABLE_END( );

//-----------------------------------------------------------------------------

struct cAntispamSettings : public cTaskSettingsPerUser
{
	cAntispamSettings():
		cTaskSettingsPerUser(),
		m_szBayesTableFileName_Original("%Data%\\antispam_original.sfdb"),
		m_szBayesTableFileName("%Data%\\antispam.sfdb"),
		m_bUseWhiteListHeuristic(cTRUE),
		m_szWhiteListFileName(""),
		m_dProbableSpamLowLevel(50),
		m_dProbableSpamHighLevel(50),
		m_dMinSpamLettersCount(50),
		m_dMinHamLettersCount(50),
		m_bCheckTrafficMonitorMessages(cTRUE),
		m_bCheckExternalPluginMessages(cTRUE),
		m_bCheckIncomingMessagesOnly(cTRUE),
		m_bUseFisher(cFALSE),
		m_nFisherX(50),
		m_nFisherS(100),
		m_nFisherMinDev(375),
		m_bFirstRun(cFALSE),
		m_bUseWhiteAddresses(cTRUE),
		m_bUseBlackAddresses(cTRUE),
		m_bUseWhiteStrings(cFALSE),
		m_bUseBlackStrings(cFALSE),
		m_bUseAntifishing(cTRUE),
		m_bUseBayesian(cTRUE),
		m_bUseGSG(cTRUE),
		m_bUsePDB(cTRUE),
		m_bUsePerUserSettings(cTRUE),
		m_bUseRecentTerms(cTRUE)
	{}
	
	DECLARE_IID_SETTINGS( cAntispamSettings, cTaskSettingsPerUser, PID_ANTISPAMTASK );

	tDWORD settings_byuser_getid() { return cAsBwList::eIID; }

	cStringObj 	m_szBayesTableFileName_Original;     // Database file installed originally
	cStringObj 	m_szBayesTableFileName;              // Current used database file
	tBOOL		m_bUseWhiteListHeuristic;            // Use white-list heuristic?
	cStringObj	m_szWhiteListFileName;               // Database file for white-list
	tDWORD		m_dProbableSpamLowLevel;             
	tDWORD		m_dProbableSpamHighLevel;
	tDWORD		m_dMinSpamLettersCount;              // How many spam letters needed for correct processing
	tDWORD		m_dMinHamLettersCount;               // How many ham letters needed for correct processing
	tBOOL		m_bCheckTrafficMonitorMessages;      // Check data from trafficMonitor?
	tBOOL		m_bCheckExternalPluginMessages;      // Check data from external plugins?
	tBOOL		m_bCheckIncomingMessagesOnly;        // Check incoming messages only or all messages?
	tBOOL       m_bUseFisher;
	tDWORD      m_nFisherX;
	tDWORD      m_nFisherS;
	tDWORD      m_nFisherMinDev;
	tBOOL       m_bFirstRun;                         // Is the first run of the antispam task?

	// Triggers
	tBOOL       m_bUseWhiteAddresses;
	tBOOL       m_bUseBlackAddresses;
	tBOOL       m_bUseWhiteStrings;
	tBOOL       m_bUseBlackStrings;
	tBOOL       m_bUseAntifishing;
	tBOOL       m_bUseBayesian;
	tBOOL       m_bUseGSG;
	tBOOL       m_bUsePDB;
	tBOOL       m_bUsePerUserSettings;
	tBOOL       m_bUseRecentTerms;
};

IMPLEMENT_SERIALIZABLE_BEGIN( cAntispamSettings, 0 )
	SER_BASE( cTaskSettingsPerUser,  0 )
	SER_VALUE( m_szBayesTableFileName_Original,tid_STRING_OBJ, "BayesTableFileName_Original"  )
	SER_VALUE( m_szBayesTableFileName,         tid_STRING_OBJ, "BayesTableFileName"  )
	SER_VALUE( m_bUseWhiteListHeuristic,       tid_BOOL,       "UseWhiteListHeuristic"  )
	SER_VALUE( m_szWhiteListFileName,          tid_STRING_OBJ, "WhiteListFileName"  )
	SER_VALUE( m_dProbableSpamLowLevel,        tid_DWORD,      "ProbableSpamLowLevel"  )
	SER_VALUE( m_dProbableSpamHighLevel,       tid_DWORD,      "ProbableSpamHighLevel"  )
	SER_VALUE( m_dMinSpamLettersCount,         tid_DWORD,      "MinSpamLettersCount"  )
	SER_VALUE( m_dMinHamLettersCount,          tid_DWORD,      "MinHamLettersCount"  )
	SER_VALUE( m_bCheckTrafficMonitorMessages, tid_BOOL,       "CheckTrafficMonitorMessages"  )
	SER_VALUE( m_bCheckExternalPluginMessages, tid_BOOL,       "CheckExternalPluginMessages"  )
	SER_VALUE( m_bCheckIncomingMessagesOnly,   tid_BOOL,       "CheckIncomingMessagesOnly"  )
	SER_VALUE( m_bUseFisher,                   tid_BOOL,       "UseFisher" )
	SER_VALUE( m_nFisherX,                     tid_DWORD,      "FisherX" )
	SER_VALUE( m_nFisherS,                     tid_DWORD,      "FisherS" )
	SER_VALUE( m_nFisherMinDev,                tid_DWORD,      "FisherMinDev" )
	SER_VALUE( m_bFirstRun,                    tid_BOOL,       "FirstRun" )
	SER_VALUE( m_bUseWhiteAddresses,           tid_BOOL,       "UseWhiteAddresses" )
	SER_VALUE( m_bUseBlackAddresses,           tid_BOOL,       "UseBlackAddresses" )
	SER_VALUE( m_bUseWhiteStrings,             tid_BOOL,       "UseWhiteWords" )
	SER_VALUE( m_bUseBlackStrings,             tid_BOOL,       "UseBlackWords" )
	SER_VALUE( m_bUseAntifishing,              tid_BOOL,       "UseAntifishing" )
	SER_VALUE( m_bUseBayesian,                 tid_BOOL,       "UseBayesian" )
	SER_VALUE( m_bUseGSG,                      tid_BOOL,       "UseGSG" )
	SER_VALUE( m_bUsePDB,                      tid_BOOL,       "UsePDB" )
	SER_VALUE( m_bUsePerUserSettings,          tid_BOOL,       "UsePerUserSettings" )
	SER_VALUE( m_bUseRecentTerms,              tid_BOOL,       "UseRecentTerms" )
IMPLEMENT_SERIALIZABLE_END( );

//-----------------------------------------------------------------------------

struct cAntispamStatistics: public cTaskStatistics 
{
	cAntispamStatistics():
		m_dwMailCount(0),
		m_dwMailSpamCount(0),
		m_bNeedMoreSpamToTrain(50),
		m_bNeedMoreHamToTrain(50),
		m_dwSpamBaseSize(0),
		m_dwHamBaseSize(0)
		{};

	DECLARE_IID_STATISTICS( cAntispamStatistics, cTaskStatistics, PID_ANTISPAMTASK );

	cAntispamStatistics& operator+=(const cAntispamStatistics& o)
	{
		if ( (-1 == m_timeStart) || (m_timeStart > o.m_timeStart) ) 
			m_timeStart = o.m_timeStart;
		if ( (-1 == m_timeFinish) || (m_timeFinish < o.m_timeFinish) ) 
			m_timeFinish = o.m_timeFinish;
		m_dwMailCount += o.m_dwMailCount;
		m_dwMailSpamCount += o.m_dwMailSpamCount;
		m_bNeedMoreSpamToTrain = o.m_bNeedMoreSpamToTrain;
		m_bNeedMoreHamToTrain = o.m_bNeedMoreHamToTrain;
		m_dwSpamBaseSize = o.m_dwSpamBaseSize;
		m_dwHamBaseSize = o.m_dwHamBaseSize;
		return *this;
	}

	tDWORD	m_dwMailCount;			// Количество обработанных сообщений
	tDWORD	m_dwMailSpamCount;		// Количество спамных обработанных сообщений
	tDWORD  m_bNeedMoreSpamToTrain;	// Требуется дообучить на m_bNeedMoreSpamToTrain письмах.
	tDWORD  m_bNeedMoreHamToTrain;	// Требуется дообучить на m_bNeedMoreHamToTrain.
	tDWORD	m_dwSpamBaseSize;		// Количество спамовых писем в базе
	tDWORD	m_dwHamBaseSize;		// Количество хамовых писем в базе
};


IMPLEMENT_SERIALIZABLE_BEGIN( cAntispamStatistics, 0 )
	SER_BASE( cTaskStatistics,  0 )
	SER_VALUE( m_dwMailCount,          tid_DWORD,      "MailCount" )
	SER_VALUE( m_dwMailSpamCount,      tid_DWORD,      "MailSpamCount" )
	SER_VALUE( m_bNeedMoreSpamToTrain, tid_DWORD,      "NeedMoreSpamToTrain" )
	SER_VALUE( m_bNeedMoreHamToTrain,  tid_DWORD,      "NeedMoreHamToTrain" )
	SER_VALUE( m_dwSpamBaseSize,       tid_DWORD,      "SpamBaseSize" )
	SER_VALUE( m_dwHamBaseSize,        tid_DWORD,      "HamBaseSize" )
IMPLEMENT_SERIALIZABLE_END( );


//-----------------------------------------------------------------------------

typedef enum 
{
	asdr_NOTHING                = -100,
	asdr_AddedToWhiteRecipient  = -3,
	asdr_HasBeenTrained         = -2,
	asdr_Training               = -1,
	asdr_NeedTraining           = 0,
	asdr_Bayesian               = 1,
	asdr_WhiteAddress           = 2,
	asdr_BlackAddress           = 3,
	asdr_WhiteString            = 4,
	asdr_BlackString            = 5,
	asdr_AntiFishing            = 6,
	asdr_WhiteAddress_NOTFOUND  = 7,
	asdr_BlackAddress_NOTFOUND  = 8,
	asdr_WhiteString_NOTFOUND   = 9,
	asdr_BlackString_NOTFOUND   = 10,
	asdr_AntiFishing_NOTFOUND   = 11,
	asdr_MailDispatcher         = 12,
	asdr_Eicar                  = 13,
	asdr_Banner                 = 14,
	asdr_Banner_NOTFOUND        = 15,
	asdr_InvalidHTML            = 16,
	asdr_InvalidHTML_NOTFOUND   = 17,
	asdr_ExternalObj            = 18,
	asdr_ExternalObj_NOTFOUND   = 19,
	asdr_InternalObj            = 20,
	asdr_InternalObj_NOTFOUND   = 21,
	asdr_EmptyMessage           = 22,
	asdr_EmptyMessage_NOTFOUND  = 23,
	asdr_NotForMe               = 24,
	asdr_NotForMe_NOTFOUND      = 25,
	asdr_NotEnglish             = 26,
	asdr_NotEnglish_NOTFOUND    = 27,
	asdr_RecipLimit             = 28,
	asdr_RecipLimit_NOTFOUND    = 29,
	asdr_GSG                    = 30,
	asdr_PDB                    = 31,
	asdr_InvalidHTML_UnknownDefsCount 	 = 32,
	asdr_InvalidHTML_SeemsColors		 = 33,
	asdr_InvalidHTML_SmallText			 = 34,
	asdr_InvalidHTML_InvisibleCharCount	 = 35,
	asdr_InvalidHTML_Scripts			 = 36,
	asdr_InvalidHTML_HiddenElements		 = 37,
	asdr_CannotBeSpam					 = 38,
	asdr_SPAMTEST               = 39,
	asdr_RecentTerms			= 40,
	asdr_LAST,
} asDetectReason_t;

//-----------------------------------------------------------------------------
struct cAntispamVerdict : public cSerializable
{
	cAntispamVerdict():
		m_dwAlgorithm(asdr_NOTHING),
		m_dwLevel(100)
		{};
		
	cAntispamVerdict( tDWORD dwAlg, const cStringObj& szDescr, tDWORD dwLevel):
		m_dwAlgorithm(dwAlg),
		m_szDescription(szDescr),
		m_dwLevel(dwLevel)
		{};

	DECLARE_IID( cAntispamVerdict, cSerializable, PID_ANTISPAMTASK, asstruct_cAsVerdict );

	tDWORD     m_dwAlgorithm;		// Алгоритм (asDetectReason_t)
	cStringObj m_szDescription;		// Описание (нужно для черно-белых списков)
	tDWORD     m_dwLevel;			// Штраф за срабатывание алгоритма
};

IMPLEMENT_SERIALIZABLE_BEGIN( cAntispamVerdict, 0 )
	SER_VALUE(  m_dwAlgorithm,   tid_DWORD,       "Algorithm" )
	SER_VALUE(  m_szDescription, tid_STRING_OBJ,  "Description"  )
	SER_VALUE(  m_dwLevel,       tid_DWORD,       "Level" )
IMPLEMENT_SERIALIZABLE_END();

//-----------------------------------------------------------------------------
typedef cVector<cAntispamVerdict> cAntispamVerdicts;

struct cAntispamStatisticsEx: public cAntispamStatistics 
{
	enum MessageCategory_t
	{
		IsDeleted = -2,
		IsUnknown = -1,
		IsHam = 0,
		IsProbableSpam,
		IsSpam
	};

	cAntispamStatisticsEx():
		m_dLastResult(0),
		m_dwLastMessageCategory(IsUnknown),
		m_dwLastReason(asdr_NeedTraining),
		m_bSettingsChanged(cFALSE)
		{};

	DECLARE_IID( cAntispamStatisticsEx, cAntispamStatistics, PID_ANTISPAMTASK, asstruct_AS_STATISTICS_EX );

	cAntispamStatisticsEx& operator+=(const cAntispamStatisticsEx& o)
	{
		*((cAntispamStatistics*)this) = o;
		if ( !m_szUserName.empty() && m_szUserName != o.m_szUserName )
			return *this;
		m_dLastResult = o.m_dLastResult;
		m_dwLastMessageCategory = o.m_dwLastMessageCategory;
		m_szLastObjectName = o.m_szLastObjectName;
		m_szLastMessageBody = o.m_szLastMessageBody;
		m_szLastMessageSubj = o.m_szLastMessageSubj;
		m_bSettingsChanged = o.m_bSettingsChanged;
		return *this;
	};

	void AddVerdict(tDWORD dwAlg, const cStringObj& szDescr, tDWORD dwLevel) { m_aVerdicts.push_back(cAntispamVerdict(dwAlg,szDescr,dwLevel)); };
	void AddVerdict(tDWORD dwAlg, char* szDescr, tDWORD dwLevel)       { m_aVerdicts.push_back(cAntispamVerdict(dwAlg,cStringObj(szDescr),dwLevel)); };

	tDWORD     m_dwPID;				    // PID почтового клиента
	cStringObj m_szUserName;			// Имя пользователя Windows, для которого накоплена эта статистика
	tDOUBLE    m_dLastResult;			// Результат последнего проверенного письма (при обучении = -1)
	tDWORD     m_dwLastMessageCategory;	// [MessageCategory_t] Категория, присвоенная сообщению
	cStringObj m_szLastObjectName;		// Имя последнего поверенного письма
	cStringObj m_szLastMessageSender;	// Отправитель последнего письма
	cStringObj m_szLastMessageSubj;		// Тема последнего письма
	cStringObj m_szLastMessageBody_Deprecated;
	tDWORD     m_dwLastReason;          // Причина (asDetectReason_t)
	tBOOL      m_bSettingsChanged;		// Были ли изменены настройки в процессе обучения?
	cAntispamVerdicts  m_aVerdicts;		// Вердикты, выданные на это письмо
	cStringObj m_szLastMessageDate;		// Дата последнего письма
	cVector<tCHAR> m_szLastMessageBody;  // Тело последнего письма
};


IMPLEMENT_SERIALIZABLE_BEGIN( cAntispamStatisticsEx, 0 )
	SER_BASE( cAntispamStatistics,  0 )
	SER_VALUE( m_dwPID,                tid_DWORD,                  "PID" )
	SER_VALUE( m_szUserName,           tid_STRING_OBJ,             "UserName"  )
	SER_VALUE( m_dLastResult,          tid_DOUBLE,                 "Result" )
	SER_VALUE( m_dwLastMessageCategory,tid_DWORD,                  "MessageCategory" )
	SER_VALUE( m_szLastObjectName,     tid_STRING_OBJ,             "ObjectName"  )
	SER_VALUE( m_szLastMessageSender,  tid_STRING_OBJ,             "MessageSender"  )
	SER_VALUE( m_szLastMessageSubj,    tid_STRING_OBJ,             "MessageSubj"  )
	SER_VALUE( m_szLastMessageBody_Deprecated, tid_STRING_OBJ,     "MBD"  )
	SER_VALUE( m_dwLastReason,         tid_DWORD,                  "Reason" )
	SER_VALUE( m_bSettingsChanged,     tid_BOOL,                   "SettingsChanged" )
	SER_VECTOR( m_aVerdicts,           cAntispamVerdict::eIID,     "Verdicts" )
	SER_VALUE( m_szLastMessageDate,    tid_STRING_OBJ,             "MessageDate"  )
	SER_VECTOR(m_szLastMessageBody,    tid_CHAR,				   "MessageBody" )
IMPLEMENT_SERIALIZABLE_END( );


//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Параметры пражского сообщения (для эмуляции функции Train)
struct cAntispamTrainParams : public cSerializable
{
	cAntispamTrainParams():
		cSerializable(),
		p_dwTimeOut(-1),
		p_Action(0),
		p_dwPID(0),
		p_pStat(NULL)
		{}
			
	DECLARE_IID( cAntispamTrainParams, cSerializable, PID_ANTISPAMTASK, asstruct_AS_TRAIN_PARAMS );
		
	enum Action_t				// из CAntispamImplementer::CKLAntispamFilter::TrainParams_t::Action_t
	{ 
		MergeDatabase = -6,		// Слить две базы в одну
		GetBaseInfo = -5,		// Получить информацию по анстипам-базе (возвращается в статистике функции Train)
		Train_Begin = -4,		// Начать полное переобучение оригинальной базы
		Train_End_Cancel = -3,	// Закончить полное переобучение оригинальной базы и отменить изменения
		Train_End_Apply = -2,	// Применить переобученную базу
		ClearDatabase = -1,		// Очистить статистическую базу 
		TrainAsSpam = 0,		// Это - спам
		TrainAsHam,				// Это - не спам
		RemoveFromSpam,			// Удалить это письмо из базы спама
		RemoveFromHam			// Удалить это письмо из базы не_спама
	};
	tDWORD p_dwTimeOut;			// таймаут, входной параметр
	tDWORD p_Action;			// Action_t
	tDWORD p_dwPID;				// PID процесса, запустившего тренировку
	cSerObj<cAntispamStatisticsEx> p_pStat;// [OUT] статистика
};

IMPLEMENT_SERIALIZABLE_BEGIN( cAntispamTrainParams, 0 )
	SER_VALUE( p_dwTimeOut,  tid_DWORD, "TimeOut" )
	SER_VALUE( p_Action,     tid_DWORD, "Action" )
	SER_VALUE( p_dwPID,      tid_DWORD, "PID" )
	SER_VALUE_PTR_NO_NAMESPACE( p_pStat,  cAntispamStatisticsEx::eIID, "stat" )

IMPLEMENT_SERIALIZABLE_END()


//-----------------------------------------------------------------------------
// Параметры пражского сообщения (отчеты)
typedef cAntispamStatisticsEx cAntispamReport;

//-----------------------------------------------------------------------------
// Параметры пражского сообщения (для эмуляции функции Train, запущенной из окна отчетов)
struct cAntispamTrainParamsEx : public cSerializable
{
	cAntispamTrainParamsEx() 
		{};

	DECLARE_IID( cAntispamTrainParamsEx, cSerializable, PID_ANTISPAMTASK, asstruct_AS_TRAIN_PARAMS_EX );

	cAntispamTrainParams m_AntispamTrainParams;
	cAntispamReport      m_AntispamReport;
};

IMPLEMENT_SERIALIZABLE_BEGIN( cAntispamTrainParamsEx, 0 )
	SER_VALUE( m_AntispamTrainParams, cAntispamTrainParams::eIID, "AntispamTrainParams" )
	SER_VALUE( m_AntispamReport,      cAntispamReport::eIID,      "AntispamReport" )
IMPLEMENT_SERIALIZABLE_END()

//-----------------------------------------------------------------------------
// Сохраненные persistant-данные задачи антиспама
struct cAntispamPersistance : public cSerializable
{
	cAntispamPersistance():
		cSerializable(),
		m_bJustTrained(cFALSE)
		{}
			
	DECLARE_IID( cAntispamPersistance, cSerializable, PID_ANTISPAMTASK, asstruct_AS_PERSISTANCE );
		
	tBOOL m_bJustTrained;	// Этому пользователю уже предлагалось обучение
};

IMPLEMENT_SERIALIZABLE_BEGIN( cAntispamPersistance, 0 )
	SER_VALUE( m_bJustTrained, tid_BOOL, "JustTrained"  )
IMPLEMENT_SERIALIZABLE_END()

//-----------------------------------------------------------------------------
// Структура, сопровождающая сообщение pmcRUN_TRAINWIZARD
struct cAntispamAskTrain : public cSerializable
{
	cAntispamAskTrain():
		cSerializable(),
		m_dwSessionId(0)
		{}
			
	DECLARE_IID( cAntispamAskTrain, cSerializable, PID_ANTISPAMTASK, asstruct_AS_ASK );
		
	tDWORD m_dwSessionId;	// Этому пользователю уже предлагалось обучение
};

IMPLEMENT_SERIALIZABLE_BEGIN( cAntispamAskTrain, 0 )
	SER_VALUE( m_dwSessionId, tid_DWORD, "SessionId"  )
IMPLEMENT_SERIALIZABLE_END()

//-----------------------------------------------------------------------------
// Структура, сопровождающая сообщение pmc_ANTISPAM_ASK_PLUGIN (опрос антиспам-плагинов)
struct cAntispamAskPlugin : public cSerializable
{
	cAntispamAskPlugin():
		cSerializable(),
		m_dwProcessID(0),
		m_dwProtocolType(0)
		{}
			
	DECLARE_IID( cAntispamAskPlugin, cSerializable, PID_ANTISPAMTASK, asstruct_AS_ASK_PLUGIN );
		
	tDWORD m_dwProcessID;	    // PID процесса, вытягивающего письмо
	tDWORD m_dwProtocolType;	// тип протокола, по которому получено письмо от трафикМонитора (TrafficProtocoller::protocoller_order_t)
};

IMPLEMENT_SERIALIZABLE_BEGIN( cAntispamAskPlugin, 0 )
	SER_VALUE( m_dwProcessID,    tid_DWORD, "ProcessID"  )
	SER_VALUE( m_dwProtocolType, tid_DWORD, "ProtocolType"  )
IMPLEMENT_SERIALIZABLE_END()




#endif//_S_ANTISPAM_H_
