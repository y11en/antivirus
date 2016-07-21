#ifndef _KAVSET_XML_H_8860B430_9117_4d7a_B50E_2352FF3C08D3
#define _KAVSET_XML_H_8860B430_9117_4d7a_B50E_2352FF3C08D3

// section names
#define section_UpdateFiles       L"UpdateFiles"
#define section_FileDescription   L"FileDescription"
#define section_OS                    L"OS"
#define keyw_OsName               "Name"
#define section_OsVer               "Version"

#define section_FromAppVersion    L"FromAppVersion"
#define keyw_ToAppVersion           L"ToAppVersion"
#define keyw_Version                     "Version"
#define section_LocLang                L"LocLang"
#define keyw_Lang                        "Lang"

// signature 6 section
#define section_Registry                L"Registry"
// signature 6 tag
#define keyw_RegistryBody            L"Body"



// Generic section keywords
#define keyw_UpdDate                 "UpdateDate"
#define keyw_BlackListDate           "BlackDate"
#define keyw_ObjectType              "ObjectType"

// Section valid keywords
#define keyw_ComponentID            L"ComponentID"
#define keyw_ApplicationID            L"ApplicationID"
#define keyw_Build                        L"Build"
#define keyw_UpdateType              L"UpdateType"
#define keyw_Filename                  L"Filename"
// path on update source relative to index file
#define keyw_ServerFolder             L"ServerFolder"
// absolute path on update source
#define keyw_RelativeSrvPath        L"RelativeSrvPath"
#define keyw_LocalPath                 L"LocalPath"
#define keyw_ProductVersion         L"ProductVersion"
#define keyw_LocLang                   L"LocLang"
#define keyw_FileSign                    L"FileSign"
#define kayw_MD5                         L"MD5"
#define keyw_FileSize                    L"FileSize"
#define keyw_FileDate                   L"FileDate"
#define keyw_Comment                 L"Comment"
// XML element which allows to specify variable and value for substitutions in format "Localization=[en,ru,de];Class=[foo,bar]"
#define keyw_SubstituteEnvironment  L"SubEnvrnmnt"
// fix name for patch to be passed to the product
#define keyw_FixName                       L"FixName"
#define keyw_BlackListSolvePeriodDays L"SolvePeriod"
#define keyw_ReplaceMode                L"ReplaceMode"
#define keyw_ReplaceModeSuffix       L"suffix"
#define keyw_ReplaceModeOrdinary   L"ordinary"



//// update types in index XML files
// bases type identifier string type in index XML files
#define UpdateTypeBase                L"base"
// patch type identifier string type in index XML files
#define UpdateTypePatch               L"patch"
// patch type identifier string type in index XML files (synonym patch; introduced for 6th index)
#define UpdateTypeExecutable       L"executable"
// index type identifier string type in index XML files
#define UpdateTypeDesc                L"desc"
// black list type identifier string type in index XML files
#define UpdateTypeBlst                  L"blst"

// signature 6 registry
#define UpdateTypeSignature6Registry    L"REGISTRY"
// signature 6 "operable" key signed by public key
#define UpdateTypeSignature6PublicKey L"KLKEY"

#define keyw_UpdateObligation       L"UpdateObligation"

// Valid Obligation keywords
#define UpdateObligMandatory        L"mandatory"
#define UpdateObligOptional           L"optional"

#define section_UpdateSettings      L"UpdateSettings"

#define keyw_Name                   "Name"
#define keyw_Type                    "Type"
#define keyw_Value                   "Value"


#define keyw_UpdateDate     L"update_date"
#define keyw_AVRecords      L"av_records"


#endif   // _KAVSET_XML_H_8860B430_9117_4d7a_B50E_2352FF3C08D3
