struct sAppSeverityRule
{
	cStrObj m_sAppPath;
	tDWORD m_nSeverity;
	tDWORD m_nAM;
};
typedef cVector<sAppSeverityRule> vAppSeverityRules;
class CNetRMSettings
{
public:
	CNetRMSettings();
	~CNetRMSettings();
	tDWORD SetHIPSSettings(CHipsSettings *);
	tDWORD SetNWsettings(cNWSettings *);
	tDWORD SetNWdata(cNWdata *);
	tTaskRequestState GetSettingsState();
	tBOOL  GetSettingsAndLock(CHipsSettings **,cNWSettings **,cNWdata **,vAppSeverityRules **);
	void   AddAppSeverityRule(wchar_t * l_pAppPath, DWORD l_Severity, DWORD l_AM);
	void   ClearAppSeverityRules(){ Lock();m_AppSeverityRules.clear(); Unlock ();};	

	void   Unlock ();
	tBOOL	Changed();

private:
	tBOOL				m_changed;	
	CRITICAL_SECTION    m_cs;
	void                Lock (); 
	CHipsSettings 		m_HIPSSettings_s;
	cNWSettings   		m_NWsettings_s;	//cNWSettings
	cNWdata      		m_NWdata_s;		//vZonesAddresses
	vAppSeverityRules   m_AppSeverityRules;		
};