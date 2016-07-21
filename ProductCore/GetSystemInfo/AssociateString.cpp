#include "stdafx.h"
#include "SysInfoDefines.h"
#include <MmReg.h>
#include "AssociatedString.h"

extern HMODULE		hKernel;
void GetAudioDevCompanyName(int wCompany,char** stCompany)
{
	__try
	{
		switch (wCompany) {
			case MM_MICROSOFT:
				strcpy(*stCompany,"Microsoft Corporation");
				break;
			case MM_CREATIVE:
				strcpy(*stCompany,"Creative Labs, Inc");
				break;
			case MM_MEDIAVISION:
				strcpy(*stCompany,"Media Vision, Inc.");
				break;
			case MM_FUJITSU:
				strcpy(*stCompany,"Fujitsu Corp.");
				break;
			case MM_ARTISOFT:
				strcpy(*stCompany,"Artisoft, Inc.");
				break;
			case MM_TURTLE_BEACH:
				strcpy(*stCompany,"Turtle Beach, Inc.");
				break;
			case MM_IBM:
				strcpy(*stCompany,"IBM Corporation");
				break;
			case MM_VOCALTEC:
				strcpy(*stCompany,"VocalTec, Inc.");
				break;
			case MM_ROLAND:
				strcpy(*stCompany,"Roland Corporation");
				break;
			case MM_DSP_SOLUTIONS:
				strcpy(*stCompany,"DSP Solutions, Inc.");
				break;
			case MM_NEC:
				strcpy(*stCompany,"NEC Corporation");
				break;
			case MM_ATI:
				strcpy(*stCompany,"ATI Technologies, Inc.");
				break;
			case MM_WANGLABS:
				strcpy(*stCompany,"Wang Laboratories, Inc");
				break;
			case MM_TANDY:
				strcpy(*stCompany,"Tandy Corporation");
				break;
			case MM_VOYETRA:
				strcpy(*stCompany,"Voyetra Technologies");
				break;
			case MM_ANTEX:
				strcpy(*stCompany,"Antex Electronics Corporation");
				break;
			case MM_ICL_PS:
				strcpy(*stCompany,"ICL Personal Systems");
				break;
			case MM_INTEL:
				strcpy(*stCompany,"Intel Corporation");
				break;
			case MM_GRAVIS:
				strcpy(*stCompany,"Advanced Gravis Computer Technology, Ltd.");
				break;
			case MM_VAL:
				strcpy(*stCompany,"Video Associates Labs, Inc.");
				break;
			case MM_INTERACTIVE:
				strcpy(*stCompany,"InterActive Inc");
				break;
			case MM_YAMAHA:
				strcpy(*stCompany,"Yamaha Corporation of America");
				break;
			case MM_EVEREX:
				strcpy(*stCompany,"Everex Systems, Inc");
				break;
			case MM_ECHO:
				strcpy(*stCompany,"Echo Speech Corporation");
				break;
			case MM_SIERRA:
				strcpy(*stCompany,"Sierra Semiconductor Corporation");
				break;
			case MM_CAT:
				strcpy(*stCompany,"Computer Aided Technology, Inc.");
				break;
			case MM_APPS:
				strcpy(*stCompany,"APPS Software");
				break;
			case MM_DSP_GROUP:
				strcpy(*stCompany,"DSP Group, Inc");
				break;
			case MM_MELABS:
				strcpy(*stCompany,"microEngineering Labs");
				break;
			case MM_COMPUTER_FRIENDS:
				strcpy(*stCompany,"Computer Friends, Inc.");
				break;
			case MM_ESS:
				strcpy(*stCompany,"ESS Technology, Inc.");
				break;
			case MM_AUDIOFILE:
				strcpy(*stCompany,"Audio, Inc.");
				break;
			case MM_MOTOROLA:
				strcpy(*stCompany,"Motorola, Inc.");
				break;
			case MM_CANOPUS:
				strcpy(*stCompany,"Canopus, co., Ltd.");
				break;
			case MM_EPSON:
				strcpy(*stCompany,"Seiko Epson Corporation, Inc.");
				break;
			case MM_TRUEVISION:
				strcpy(*stCompany,"Truevision, Inc.");
				break;
			case MM_AZTECH:
				strcpy(*stCompany,"Aztech Labs, Inc.");
				break;
			case MM_VIDEOLOGIC:
				strcpy(*stCompany,"Videologic, Inc.");
				break;
			case MM_SCALACS:
				strcpy(*stCompany,"SCALACS");
				break;
			case MM_KORG:
				strcpy(*stCompany,"Toshihiko Okuhura, Korg, Inc.");
				break;
			case MM_APT:
				strcpy(*stCompany,"Audio Processing Technology");
				break;
			case MM_ICS:
				strcpy(*stCompany,"Integrated Circuit Systems, Inc.");
				break;
			case MM_ITERATEDSYS:
				strcpy(*stCompany,"Iterated Systems, Inc.");
				break;
			case MM_METHEUS:
				strcpy(*stCompany,"Metheus Corporation");
				break;
			case MM_LOGITECH:
				strcpy(*stCompany,"Logitech, Inc.");
				break;
			case MM_WINNOV:
				strcpy(*stCompany,"Winnov");
				break;
			case MM_NCR:
				strcpy(*stCompany,"NCR Corporation");
				break;
			case MM_EXAN:
				strcpy(*stCompany,"EXAN, Ltd.");
				break;
			case MM_AST:
				strcpy(*stCompany,"AST Research, Inc.");
				break;
			case MM_WILLOWPOND:
				strcpy(*stCompany,"Willow Pond Corporation");
				break;
			case MM_SONICFOUNDRY:
				strcpy(*stCompany,"Sonic Foundry");
				break;
			case MM_VITEC:
				strcpy(*stCompany,"Visual Information Technologies, Inc.");
				break;
			case MM_MOSCOM:
				strcpy(*stCompany,"MOSCOM Corporation");
				break;
			case MM_SILICONSOFT:
				strcpy(*stCompany,"Silicon Soft, Inc.");
				break;
			case MM_SUPERMAC:
				strcpy(*stCompany,"Supermac Technology, Inc.");
				break;
			case MM_AUDIOPT:
				strcpy(*stCompany,"Audio Processing Technology");
				break;
			case MM_SPEECHCOMP:
				strcpy(*stCompany,"Speech Compression");
				break;
			case MM_AHEAD:
				strcpy(*stCompany,"Ahead, Inc.");
				break;
			case MM_DOLBY:
				strcpy(*stCompany,"Dolby Laboratories, Inc.");
				break;
			case MM_OKI:
				strcpy(*stCompany,"OKI");
				break;
			case MM_AURAVISION:
				strcpy(*stCompany,"Auravision Corporation");
				break;
			case MM_OLIVETTI:
				strcpy(*stCompany,"Ing C. Olivetti & C., S.p.A.");
				break;
			case MM_IOMAGIC:
				strcpy(*stCompany,"I/O Magic Corporation");
				break;
			case MM_MATSUSHITA:
				strcpy(*stCompany,"Matsushita Electric Corporation of America");
				break;
			case MM_CONTROLRES:
				strcpy(*stCompany,"Control Resources Corporation");
				break;
			case MM_XEBEC:
				strcpy(*stCompany,"Xebec Multimedia Solutions Limited");
				break;
			case MM_NEWMEDIA:
				strcpy(*stCompany,"New Media Corporation");
				break;
			case MM_NMS:
				strcpy(*stCompany,"Natural MicroSystems Corporation");
				break;
			case MM_LYRRUS:
				strcpy(*stCompany,"Lyrrus, Inc.");
				break;
			case MM_COMPUSIC:
				strcpy(*stCompany,"Compusic");
				break;
			case MM_OPTI:
				strcpy(*stCompany,"OPTi, Inc.");
				break;
			case MM_ADLACC:
				strcpy(*stCompany,"Adlib Accessories Inc.");
				break;
			case MM_COMPAQ:
				strcpy(*stCompany,"Compaq Computer Corp.");
				break;
			case MM_DIALOGIC:
				strcpy(*stCompany,"Dialogic Corporation");
				break;
			case MM_INSOFT:
				strcpy(*stCompany,"InSoft, Inc.");
				break;
			case MM_MPTUS:
				strcpy(*stCompany,"M.P. Technologies, Inc.");
				break;
			case MM_WEITEK:
				strcpy(*stCompany,"Weitek");
				break;
			case MM_LERNOUT_AND_HAUSPIE:
				strcpy(*stCompany,"Lernout & Hauspie");
				break;
			case MM_QCIAR:
				strcpy(*stCompany,"Quanta Computer Inc.");
				break;
			case MM_APPLE:
				strcpy(*stCompany,"Apple Computer, Inc.");
				break;
			case MM_DIGITAL:
				strcpy(*stCompany,"Digital Equipment Corporation");
				break;
			case MM_MOTU:
				strcpy(*stCompany,"Mark of the Unicorn");
				break;
			case MM_WORKBIT:
				strcpy(*stCompany,"Workbit Corporation");
				break;
			case MM_OSITECH:
				strcpy(*stCompany,"Ositech Communications Inc.");
				break;
			case MM_MIRO:
				strcpy(*stCompany,"miro Computer Products AG");
				break;
			case MM_CIRRUSLOGIC:
				strcpy(*stCompany,"Cirrus Logic");
				break;
			case MM_ISOLUTION:
				strcpy(*stCompany,"ISOLUTION  B.V.");
				break;
			case MM_HORIZONS:
				strcpy(*stCompany,"Horizons Technology, Inc");
				break;
			case MM_CONCEPTS:
				strcpy(*stCompany,"Computer Concepts Ltd");
				break;
			case MM_VTG:
				strcpy(*stCompany,"Voice Technologies Group, Inc.");
				break;
			case MM_RADIUS:
				strcpy(*stCompany,"Radius");
				break;
			case MM_ROCKWELL:
				strcpy(*stCompany,"Rockwell International");
				break;
			case MM_OPCODE:
				strcpy(*stCompany,"Opcode Systems");
				break;
			case MM_VOXWARE:
				strcpy(*stCompany,"Voxware Inc");
				break;
			case MM_NORTHERN_TELECOM:
				strcpy(*stCompany,"Northern Telecom Limited");
				break;
			case MM_APICOM:
				strcpy(*stCompany,"APICOM");
				break;
			case MM_GRANDE:
				strcpy(*stCompany,"Grande Software");
				break;
			case MM_ADDX:
				strcpy(*stCompany,"ADDX");
				break;
			case MM_WILDCAT:
				strcpy(*stCompany,"Wildcat Canyon Software");
				break;
			case MM_RHETOREX:
				strcpy(*stCompany,"Rhetorex Inc");
				break;
			case MM_BROOKTREE:
				strcpy(*stCompany,"Brooktree Corporation");
				break;
			case MM_ENSONIQ:
				strcpy(*stCompany,"ENSONIQ Corporation");
				break;
			case MM_FAST:
				strcpy(*stCompany,"FAST Multimedia AG");
				break;
			case MM_NVIDIA:
				strcpy(*stCompany,"NVidia Corporation");
				break;
			case MM_OKSORI:
				strcpy(*stCompany,"OKSORI Co., Ltd.");
				break;
			case MM_DIACOUSTICS:
				strcpy(*stCompany,"DiAcoustics, Inc.");
				break;
			case MM_GULBRANSEN:
				strcpy(*stCompany,"Gulbransen, Inc.");
				break;
			case MM_KAY_ELEMETRICS:
				strcpy(*stCompany,"Kay Elemetrics, Inc.");
				break;
			case MM_CRYSTAL:
				strcpy(*stCompany,"Crystal Semiconductor Corporation");
				break;
			case MM_SPLASH_STUDIOS:
				strcpy(*stCompany,"Splash Studios");
				break;
			case MM_QUARTERDECK:
				strcpy(*stCompany,"Quarterdeck Corporation");
				break;
			case MM_TDK:
				strcpy(*stCompany,"TDK Corporation");
				break;
			case MM_DIGITAL_AUDIO_LABS:
				strcpy(*stCompany,"Digital Audio Labs, Inc.");
				break;
			case MM_SEERSYS:
				strcpy(*stCompany,"Seer Systems, Inc.");
				break;
			case MM_PICTURETEL:
				strcpy(*stCompany,"PictureTel Corporation");
				break;
			case MM_ATT_MICROELECTRONICS:
				strcpy(*stCompany,"AT&T Microelectronics");
				break;
			case MM_OSPREY:
				strcpy(*stCompany,"Osprey Technologies, Inc.");
				break;
			case MM_MEDIATRIX:
				strcpy(*stCompany,"Mediatrix Peripherals");
				break;
			case MM_SOUNDESIGNS:
				strcpy(*stCompany,"SounDesignS M.C.S. Ltd.");
				break;
			case MM_ALDIGITAL:
				strcpy(*stCompany,"A.L. Digital Ltd.");
				break;
			case MM_SPECTRUM_SIGNAL_PROCESSING:
				strcpy(*stCompany,"Spectrum Signal Processing, Inc.");
				break;
			case MM_ECS:
				strcpy(*stCompany,"Electronic Courseware Systems, Inc.");
				break;
			case MM_AMD:
				strcpy(*stCompany,"AMD");
				break;
			case MM_COREDYNAMICS:
				strcpy(*stCompany,"Core Dynamics");
				break;
			case MM_CANAM:
				strcpy(*stCompany,"CANAM Computers");
				break;
			case MM_SOFTSOUND:
				strcpy(*stCompany,"Softsound, Ltd.");
				break;
			case MM_NORRIS:
				strcpy(*stCompany,"Norris Communications, Inc.");
				break;
			case MM_DDD:
				strcpy(*stCompany,"Danka Data Devices");
				break;
			case MM_EUPHONICS:
				strcpy(*stCompany,"EuPhonics");
				break;
			case MM_PRECEPT:
				strcpy(*stCompany,"Precept Software, Inc.");
				break;
			case MM_CRYSTAL_NET:
				strcpy(*stCompany,"Crystal Net Corporation");
				break;
			case MM_CHROMATIC:
				strcpy(*stCompany,"Chromatic Research, Inc");
				break;
			case MM_VOICEINFO:
				strcpy(*stCompany,"Voice Information Systems, Inc");
				break;
			case MM_VIENNASYS:
				strcpy(*stCompany,"Vienna Systems");
				break;
			case MM_CONNECTIX:
				strcpy(*stCompany,"Connectix Corporation");
				break;
			case MM_GADGETLABS:
				strcpy(*stCompany,"Gadget Labs LLC");
				break;
			case MM_FRONTIER:
				strcpy(*stCompany,"Frontier Design Group LLC");
				break;
			case MM_VIONA:
				strcpy(*stCompany,"Viona Development GmbH");
				break;
			case MM_CASIO:
				strcpy(*stCompany,"Casio Computer Co., LTD");
				break;
			case MM_DIAMONDMM:
				strcpy(*stCompany,"Diamond Multimedia");
				break;
			case MM_S3:
				strcpy(*stCompany,"S3");
				break;
			case MM_FRAUNHOFER_IIS:
				strcpy(*stCompany,"Fraunhofer");
				break;
			case MM_PRAGMATRAX:
				strcpy(*stCompany,"PRAGMATRAX Software");
				break;
			case MM_CYRIX:
				strcpy(*stCompany,"Cyrix Corporation");
				break;
			case MM_PHILIPS_SPEECH_PROCESSING:
				strcpy(*stCompany,"Philips Speech Processing");
				break;
			case MM_NETXL:
				strcpy(*stCompany,"NetXL, Inc.");
				break;
			case MM_BECUBED:
				strcpy(*stCompany,"BeCubed Software Inc.");
				break;
			case MM_AARDVARK:
				strcpy(*stCompany,"Aardvark Computer Systems, Inc.");
				break;
			case MM_BINTEC:
				strcpy(*stCompany,"Bin Tec Communications GmbH");
				break;
			case MM_HEWLETT_PACKARD:
				strcpy(*stCompany,"Hewlett-Packard Company");
				break;
			case MM_ACULAB:
				strcpy(*stCompany,"Aculab plc");
				break;
			case MM_FAITH:
				strcpy(*stCompany,"Faith,Inc.");
				break;
			case MM_MITEL:
				strcpy(*stCompany,"Mitel Corporation");
				break;
			case MM_QUANTUM3D:
				strcpy(*stCompany,"Quantum3D, Inc.");
				break;
			case MM_SNI:
				strcpy(*stCompany,"Siemens-Nixdorf");
				break;
			case MM_EMU:
				strcpy(*stCompany,"E-mu Systems, Inc.");
				break;
			case MM_TERRATEC:
				strcpy(*stCompany,"TerraTec Electronic GmbH");
				break;
			case MM_MEDIASONIC:
				strcpy(*stCompany,"MediaSonic Ltd.");
				break;
			case MM_SANYO:
				strcpy(*stCompany,"SANYO Electric Co., Ltd.");
				break;
			case MM_NOGATECH:
				strcpy(*stCompany,"NOGATECH Ltd.");
				break;
			case MM_XYZ:
				strcpy(*stCompany,"Co. XYZ for testing");
				break;
			case MM_DVISION:
				strcpy(*stCompany,"D-Vision Systems, Inc.");
				break;
			case MM_NETSCAPE:
				strcpy(*stCompany,"Netscape Communications");
				break;
			case MM_SOUNDSPACE:
				strcpy(*stCompany,"Soundspace Audio");
				break;
			case MM_VANKOEVERING:
				strcpy(*stCompany,"VanKoevering Company");
				break;
			case MM_QTEAM:
				strcpy(*stCompany,"Q-Team");
				break;
			case MM_ZEFIRO:
				strcpy(*stCompany,"Zefiro Acoustics");
				break;
			case MM_STUDER:
				strcpy(*stCompany,"Studer Professional Audio AG");
				break;
			case MM_QUICKNET:
				strcpy(*stCompany,"Quicknet Technologies");
				break;
			case MM_ALARIS:
				strcpy(*stCompany,"Alaris, Inc.");
				break;
			case MM_SICRESOURCE:
				strcpy(*stCompany,"SIC Resource Inc.");
				break;
			case MM_NEOMAGIC:
				strcpy(*stCompany,"NeoMagic Corporation");
				break;
			case MM_MERGING_TECHNOLOGIES:
				strcpy(*stCompany,"Merging Technologies S.A.");
				break;
			case MM_XIRLINK:
				strcpy(*stCompany,"Xirlink, Inc.");
				break;
			case MM_COLORGRAPH:
				strcpy(*stCompany,"Colorgraph (UK) Ltd");
				break;
			case MM_OTI:
				strcpy(*stCompany,"Oak Technology, Inc.");
				break;
			case MM_AUREAL:
				strcpy(*stCompany,"Aureal Semiconductor");
				break;
			case MM_VIVO:
				strcpy(*stCompany,"Vivo Software");
				break;
			case MM_SHARP:
				strcpy(*stCompany,"Sharp");
				break;
			case MM_LUCENT:
				strcpy(*stCompany,"Lucent Technologies");
				break;
			case MM_ATT:
				strcpy(*stCompany,"AT&T Labs, Inc.");
				break;
			case MM_SUNCOM:
				strcpy(*stCompany,"Sun Communications, Inc.");
				break;
			case MM_SORVIS:
				strcpy(*stCompany,"Sorenson Vision");
				break;
			case MM_INVISION:
				strcpy(*stCompany,"InVision Interactive");
				break;
			case MM_BERKOM:
				strcpy(*stCompany,"Deutsche Telekom Berkom GmbH");
				break;
			case MM_MARIAN:
				strcpy(*stCompany,"Marian GbR Leipzig");
				break;
			case MM_DPSINC:
				strcpy(*stCompany,"Digital Processing Systems, Inc.");
				break;
			case MM_BCB:
				strcpy(*stCompany,"BCB Holdings Inc.");
				break;
			case MM_MOTIONPIXELS:
				strcpy(*stCompany,"Motion Pixels");
				break;
			case MM_QDESIGN:
				strcpy(*stCompany,"QDesign Corporation");
				break;
			case MM_NMP:
				strcpy(*stCompany,"Nokia Mobile Phones");
				break;
			case MM_DATAFUSION:
				strcpy(*stCompany,"DataFusion Systems (Pty) (Ltd)");
				break;
			case MM_DUCK:
				strcpy(*stCompany,"The Duck Corporation");
				break;
			case MM_FTR:
				strcpy(*stCompany,"Future Technology Resources Pty Ltd");
				break;
			case MM_BERCOS:
				strcpy(*stCompany,"BERCOS GmbH");
				break;
			case MM_ONLIVE:
				strcpy(*stCompany,"OnLive! Technologies, Inc.");
				break;
			case MM_SIEMENS_SBC:
				strcpy(*stCompany,"Siemens Business Communications Systems");
				break;
			case MM_TERALOGIC:
				strcpy(*stCompany,"TeraLogic, Inc.");
				break;
			case MM_PHONET:
				strcpy(*stCompany,"PhoNet Communications Ltd.");
				break;
			case MM_WINBOND:
				strcpy(*stCompany,"Winbond Electronics Corp");
				break;
			case MM_VIRTUALMUSIC:
				strcpy(*stCompany,"Virtual Music, Inc.");
				break;
			case MM_ENET:
				strcpy(*stCompany,"e-Net, Inc.");
				break;
			case MM_GUILLEMOT:
				strcpy(*stCompany,"Guillemot International");
				break;
			case MM_EMAGIC:
				strcpy(*stCompany,"Emagic Soft- und Hardware GmbH");
				break;
			case MM_MWM:
				strcpy(*stCompany,"MWM Acoustics LLC");
				break;
			case MM_PACIFICRESEARCH:
				strcpy(*stCompany,"Pacific Research and Engineering Corporation");
				break;
			case MM_SIPROLAB:
				strcpy(*stCompany,"Sipro Lab Telecom Inc.");
				break;
			case MM_LYNX:
				strcpy(*stCompany,"Lynx Studio Technology, Inc.");
				break;
			case MM_SPECTRUM_PRODUCTIONS:
				strcpy(*stCompany,"Spectrum Productions");
				break;
			case MM_DICTAPHONE:
				strcpy(*stCompany,"Dictaphone Corporation");
				break;
			case MM_QUALCOMM:
				strcpy(*stCompany,"QUALCOMM, Inc.");
				break;
			case MM_RZS:
				strcpy(*stCompany,"Ring Zero Systems, Inc");
				break;
			case MM_AUDIOSCIENCE:
				strcpy(*stCompany,"AudioScience Inc.");
				break;
			case MM_PINNACLE:
				strcpy(*stCompany,"Pinnacle Systems, Inc.");
				break;
			case MM_EES:
				strcpy(*stCompany,"EES Technik für Musik GmbH");
				break;
			case MM_HAFTMANN:
				strcpy(*stCompany,"haftmann#software");
				break;
			case MM_LUCID:
				strcpy(*stCompany,"Lucid Technology, Symetrix Inc.");
				break;
			case MM_HEADSPACE:
				strcpy(*stCompany,"Headspace, Inc");
				break;
			case MM_UNISYS:
				strcpy(*stCompany,"UNISYS CORPORATION");
				break;
			case MM_LUMINOSITI:
				strcpy(*stCompany,"Luminositi, Inc.");
				break;
			case MM_ACTIVEVOICE:
				strcpy(*stCompany,"ACTIVE VOICE CORPORATION");
				break;
			case MM_DTS:
				strcpy(*stCompany,"Digital Theater Systems, Inc.");
				break;
			case MM_DIGIGRAM:
				strcpy(*stCompany,"DIGIGRAM");
				break;
			case MM_SOFTLAB_NSK:
				strcpy(*stCompany,"Softlab-Nsk");
				break;
			case MM_FORTEMEDIA:
				strcpy(*stCompany,"ForteMedia, Inc");
				break;
			case MM_SONORUS:
				strcpy(*stCompany,"Sonorus, Inc.");
				break;
			case MM_ARRAY:
				strcpy(*stCompany,"Array Microsystems, Inc.");
				break;
			case MM_DATARAN:
				strcpy(*stCompany,"Data Translation, Inc.");
				break;
			case MM_I_LINK:
				strcpy(*stCompany,"I-link Worldwide");
				break;
			case MM_SELSIUS_SYSTEMS:
				strcpy(*stCompany,"Selsius Systems Inc.");
				break;
			case MM_ADMOS:
				strcpy(*stCompany,"AdMOS Technology, Inc.");
				break;
			case MM_LEXICON:
				strcpy(*stCompany,"Lexicon Inc.");
				break;
			case MM_SGI:
				strcpy(*stCompany,"Silicon Graphics Inc.");
				break;
			case MM_IPI:
				strcpy(*stCompany,"Interactive Product Inc.");
				break;
			case MM_ICE:
				strcpy(*stCompany,"IC Ensemble, Inc.");
				break;
			case MM_VQST:
				strcpy(*stCompany,"ViewQuest Technologies Inc.");
				break;
			case MM_ETEK:
				strcpy(*stCompany,"eTEK Labs Inc.");
				break;
			case MM_CS:
				strcpy(*stCompany,"Consistent Software");
				break;
			case MM_ALESIS:
				strcpy(*stCompany,"Alesis Studio Electronics");
				break;
			case MM_INTERNET:
				strcpy(*stCompany,"INTERNET Corporation");
				break;
			case MM_SONY:
				strcpy(*stCompany,"Sony Corporation");
				break;
			case MM_HYPERACTIVE:
				strcpy(*stCompany,"Hyperactive Audio Systems, Inc.");
				break;
			case MM_UHER_INFORMATIC:
				strcpy(*stCompany,"UHER informatic GmbH");
				break;
			case MM_SYDEC_NV:
				strcpy(*stCompany,"Sydec NV");
				break;
			case MM_FLEXION:
				strcpy(*stCompany,"Flexion Systems Ltd.");
				break;
			case MM_VIA:
				strcpy(*stCompany,"Via Technologies, Inc.");
				break;
			case MM_MICRONAS:
				strcpy(*stCompany,"Micronas Semiconductors, Inc.");
				break;
			case MM_ANALOGDEVICES:
				strcpy(*stCompany,"Analog Devices, Inc.");
				break;
			case MM_HP:
				strcpy(*stCompany,"Hewlett Packard Company");
				break;
			case MM_MATROX_DIV:
				strcpy(*stCompany,"Matrox");
				break;
			case MM_QUICKAUDIO:
				strcpy(*stCompany,"Quick Audio, GbR");
				break;
			case MM_YOUCOM:
				strcpy(*stCompany,"You/Com Audiocommunicatie BV");
				break;
			case MM_RICHMOND:
				strcpy(*stCompany,"Richmond Sound Design Ltd.");
				break;
			case MM_IODD:
				strcpy(*stCompany,"I-O Data Device, Inc.");
				break;
			case MM_ICCC:
				strcpy(*stCompany,"ICCC A/S");
				break;
			case MM_3COM:
				strcpy(*stCompany,"3COM Corporation");
				break;
			case MM_MALDEN:
				strcpy(*stCompany,"Malden Electronics Ltd.");
				break;
			default:
				strcpy(*stCompany,"Unknown");
				break;
		}
		return;
	}
	__finally
	{
	}
}

void GetCodePageName(UINT wCodePage,char** stCodePageName)
{
	CPINFOEX		lpCPInfoEx;
	GetCPInfoExPtr	m_hGetCPInfoEx=NULL;
	__try
	{
		m_hGetCPInfoEx=(GetCPInfoExPtr)GetProcAddress(hKernel,"GetCPInfoExA");
		if (m_hGetCPInfoEx!=NULL)
		{
			if (m_hGetCPInfoEx(wCodePage,0,&lpCPInfoEx)!=0)
			{
				strcpy(*stCodePageName,lpCPInfoEx.CodePageName);
				return;
			}
		}
		switch (wCodePage)
		{
		case 037:
			strcpy(*stCodePageName,"EBCDIC");
			break;
		case 290:
			strcpy(*stCodePageName,"IBM Extended English Katakana");
			break;
		case 300:
			strcpy(*stCodePageName,"IBM Japanese Character Sets");
			break;
		case 437:
			strcpy(*stCodePageName,"MS-DOS United States");
			break;
		case 500:
			strcpy(*stCodePageName,"EBCDIC \"500V1\"");
			break;
		case 708:
			strcpy(*stCodePageName,"Arabic (ASMO 708)");
			break;
		case 709:
			strcpy(*stCodePageName,"Arabic (ASMO 449+, BCON V4)");
			break;
		case 710:
			strcpy(*stCodePageName,"Arabic (Transparent Arabic)");
			break;
		case 711:
			strcpy(*stCodePageName,"Arabic (Nafitha Enhanced)");
			break;
		case 720:
			strcpy(*stCodePageName,"Arabic (Transparent ASMO)");
			break;
		case 737:
			strcpy(*stCodePageName,"Greek (formerly 437G)");
			break;
		case 775:
			strcpy(*stCodePageName,"Baltic");
			break;
		case 833:
			strcpy(*stCodePageName,"IBM Hangul Extended Single-Byte");
			break;
		case 834:
			strcpy(*stCodePageName,"Korean Host Double-byte");
			break;
		case 835:
			strcpy(*stCodePageName,"IBM Traditional Chinese Character Sets");
			break;
		case 836:
			strcpy(*stCodePageName,"Simplified Chinese Single-Byte");
			break;
		case 837:
			strcpy(*stCodePageName,"IBM Simplified Chinese Character Sets");
			break;
		case 850:
			strcpy(*stCodePageName,"MS-DOS Multilingual (Latin I)");
			break;
		case 852:
			strcpy(*stCodePageName,"MS-DOS Slavic (Latin II)");
			break;
		case 855:
			strcpy(*stCodePageName,"IBM Cyrillic (primarily Russian)");
			break;
		case 857:
			strcpy(*stCodePageName,"IBM Turkish");
			break;
		case 860:
			strcpy(*stCodePageName,"MS-DOS Portuguese");
			break;
		case 861:
			strcpy(*stCodePageName,"MS-DOS Icelandic");
			break;
		case 862:
			strcpy(*stCodePageName,"Hebrew");
			break;
		case 863:
			strcpy(*stCodePageName,"MS-DOS Canadian-French");
			break;
		case 864:
			strcpy(*stCodePageName,"Arabic");
			break;
		case 865:
			strcpy(*stCodePageName,"MS-DOS Nordic");
			break;
		case 866:
			strcpy(*stCodePageName,"MS-DOS Russian");
			break;
		case 869:
			strcpy(*stCodePageName,"IBM Modern Greek");
			break;
		case 870:
			strcpy(*stCodePageName,"IBM EBCDIC Latin-2");
			break;
		case 874:
			strcpy(*stCodePageName,"Thai");
			break;
		case 875:
			strcpy(*stCodePageName,"EBCDIC");
			break;
		case 932:
			strcpy(*stCodePageName,"Japanese");
			break;
		case 936:
			strcpy(*stCodePageName,"Chinese (PRC, Singapore)");
			break;
		case 949:
			strcpy(*stCodePageName,"Korean");
			break;
		case 950:
			strcpy(*stCodePageName,"Chinese (Taiwan; Hong Kong SAR, PRC)");
			break;
		case 1026:
			strcpy(*stCodePageName,"EBCDIC");
			break;
		case 1027:
			strcpy(*stCodePageName,"IBM Extended Lowercase English");
			break;
		case 1200:
			strcpy(*stCodePageName,"Unicode (BMP of ISO 10646)");
			break;
		case 1201:
			strcpy(*stCodePageName,"Universal Alphabet (Unicode) (ISO-10646-UCS-2)");
			break;
		case 1250:
			strcpy(*stCodePageName,"Windows Eastern European");
			break;
		case 1251:
			strcpy(*stCodePageName,"Windows Cyrillic");
			break;
		case 1252:
			strcpy(*stCodePageName,"Windows US (ANSI)");
			break;
		case 1253:
			strcpy(*stCodePageName,"Windows Greek");
			break;
		case 1254:
			strcpy(*stCodePageName,"Windows Turkish");
			break;
		case 1255:
			strcpy(*stCodePageName,"Windows Hebrew");
			break;
		case 1256:
			strcpy(*stCodePageName,"Windows Arabic");
			break;
		case 1257:
			strcpy(*stCodePageName,"Windows Baltic");
			break;
		case 1258:
			strcpy(*stCodePageName,"Windows Vietnamese windows-1258");
			break;
		case 1361:
			strcpy(*stCodePageName,"Korean (Johab)");
			break;
		case 10000:
			strcpy(*stCodePageName,"Macintosh Roman");
			break;
		case 10001:
			strcpy(*stCodePageName,"Macintosh Japanese");
			break;
		case 10002:
			strcpy(*stCodePageName,"Macintosh Chinese");
			break;
		case 10003:
			strcpy(*stCodePageName,"Macintosh Korean");
			break;
		case 10004:
			strcpy(*stCodePageName,"Macintosh Arabic");
			break;
		case 10005:
			strcpy(*stCodePageName,"Macintosh Hebrew");
			break;
		case 10006:
			strcpy(*stCodePageName,"Macintosh Greek I");
			break;
		case 10007:
			strcpy(*stCodePageName,"Macintosh Cyrillic");
			break;
		case 10008:
			strcpy(*stCodePageName,"Macintosh");
			break;
		case 10010:
			strcpy(*stCodePageName,"Macintosh Romanian");
			break;
		case 10017:
			strcpy(*stCodePageName,"Macintosh Ukrainian");
			break;
		case 10029:
			strcpy(*stCodePageName,"Macintosh Latin 2");
			break;
		case 10079:
			strcpy(*stCodePageName,"Macintosh Icelandic");
			break;
		case 10081:
			strcpy(*stCodePageName,"Macintosh Turkish");
			break;
		case 10082:
			strcpy(*stCodePageName,"Macintosh Croatian");
			break;
		case 11009:
			strcpy(*stCodePageName,"Canadian Standard");
			break;
		case 20105:
			strcpy(*stCodePageName,"IA5 IRV");
			break;
		case 20106:
			strcpy(*stCodePageName,"IA6 (German)");
			break;
		case 20107:
			strcpy(*stCodePageName,"IA6 (Swedish)");
			break;
		case 20108:
			strcpy(*stCodePageName,"IA6 (Norwegian)");
			break;
		case 20261:
			strcpy(*stCodePageName,"T.61-8bit");
			break;
		case 20269:
			strcpy(*stCodePageName,"ISO-6937");
			break;
		case 20273:
			strcpy(*stCodePageName,"IBM EBCDIC Germany");
			break;
		case 20277:
			strcpy(*stCodePageName,"IBM EBCDIC Denmark/Norway");
			break;
		case 20278:
			strcpy(*stCodePageName,"IBM EBCDIC Finland/Sweden");
			break;
		case 20280:
			strcpy(*stCodePageName,"IBM EBCDIC Italy");
			break;
		case 20284:
			strcpy(*stCodePageName,"IBM EBCDIC Latin America/Spain");
			break;
		case 20285:
			strcpy(*stCodePageName,"IBM EBCDIC United Kingdom");
			break;
		case 20290:
			strcpy(*stCodePageName,"IBM EBCDIC Japanese IBM290");
			break;
		case 20297:
			strcpy(*stCodePageName,"IBM EBCDIC France");
			break;
		case 20405:
			strcpy(*stCodePageName,"Czech (Programmers)");
			break;
		case 20408:
			strcpy(*stCodePageName,"Greek Latin IBM 220");
			break;
		case 20409:
			strcpy(*stCodePageName,"United States-Dvorak");
			break;
		case 20420:
			strcpy(*stCodePageName,"IBM EBCDIC Arabic");
			break;
		case 20423:
			strcpy(*stCodePageName,"IBM EBCDIC Greek");
			break;
		case 20833:
			strcpy(*stCodePageName,"Korean (IBM EBCDIC)");
			break;
		case 20838:
			strcpy(*stCodePageName,"IBM EBCDIC Thai");
			break;
		case 20866:
			strcpy(*stCodePageName,"Russian - KOI8-R");
			break;
		case 20871:
			strcpy(*stCodePageName,"IBM EBCDIC Icelandic");
			break;
		case 20880:
			strcpy(*stCodePageName,"IBM EBCDIC Cyrillic");
			break;
		case 20905:
			strcpy(*stCodePageName,"IBM EBCDIC Turkish");
			break;
		case 21025:
			strcpy(*stCodePageName,"IBM EBCDIC Cyrillic");
			break;
		case 21027:
			strcpy(*stCodePageName,"Japanese");
			break;
		case 21866:
			strcpy(*stCodePageName,"Ukranian - KOI8-RU");
			break;
		case 21591:
			strcpy(*stCodePageName,"ISO 8859-1 Western");
			break;
		case 21592:
			strcpy(*stCodePageName,"ISO 8859-2 Eastern Europe");
			break;
		case 28593:
			strcpy(*stCodePageName,"ISO 8859-3 Turkish");
			break;
		case 28594:
			strcpy(*stCodePageName,"ISO 8859-4 Baltic");
			break;
		case 28595:
			strcpy(*stCodePageName,"ISO 8859-5 Cyrillic");
			break;
		case 28596:
			strcpy(*stCodePageName,"ISO 8859-6 Arabic");
			break;
		case 28597:
			strcpy(*stCodePageName,"ISO 8859-7 Greek");
			break;
		case 28598:
			strcpy(*stCodePageName,"ISO 8859-8 Hebrew");
			break;
		case 28599:
			strcpy(*stCodePageName,"ISO 8859-9");
			break;
		case 30408:
			strcpy(*stCodePageName,"Greek IBM 319");
			break;
		case 30409:
			strcpy(*stCodePageName,"United States-LH Dvorak");
			break;
		case 40408:
			strcpy(*stCodePageName,"Greek Latin IBM 319");
			break;
		case 40409:
			strcpy(*stCodePageName,"United States-RH Dvorak");
			break;
		case 50220:
			strcpy(*stCodePageName,"Japanese (JIS)");
			break;
		case 50221:
			strcpy(*stCodePageName,"Japanese (JIS)");
			break;
		case 50222:
			strcpy(*stCodePageName,"Japanese (JIS)");
			break;
		case 50225:
			strcpy(*stCodePageName,"Korean");
			break;
		case 50408:
			strcpy(*stCodePageName,"Greek Latin");
			break;
		case 50932:
			strcpy(*stCodePageName,"Japanese");
			break;
		case 50949:
			strcpy(*stCodePageName,"Korean");
			break;
		case 51932:
			strcpy(*stCodePageName,"Japanese");
			break;
		case 51949:
			strcpy(*stCodePageName,"Korean");
			break;
		case 52936:
			strcpy(*stCodePageName,"Simplified Chinese");
			break;
		case 65000:
			strcpy(*stCodePageName,"Unicode UTF-7");
			break;
		case 65001:
			strcpy(*stCodePageName,"Unicode UTF-8");
			break;
		default:
			strcpy(*stCodePageName,"");
			break;
		}
		return;
	}
	__finally
	{
	}
}


