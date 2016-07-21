#if !defined(AFX_INDEXFILEXMLPARSER_H__22D7E101_0233_42EB_957C_3492E15902E9__INCLUDED_)
#define AFX_INDEXFILEXMLPARSER_H__22D7E101_0233_42EB_957C_3492E15902E9__INCLUDED_

#if _MSC_VER > 1000
    #pragma once
#endif // _MSC_VER > 1000

#include "stdinc.h"

namespace KLUPD {


// IndexFileXMLParser - parse index files (master.xml, kavset.xml and so on)
class IndexFileXMLParser
{
public:
    enum Tag
    {
        updateDate,
        blackDate,
    };

    IndexFileXMLParser( const NoCaseString& strFilename,
                        FileVector &,
                        Signature6Checker &,
                        const Path &parentRelativeURLPath,
                        const bool retranslationMode,
                        CallbackInterface &,
                        Log * );

    // parse index file for specified tag
    static NoCaseString getTagFromLocalFile(const Tag &, const Path &fullFileName, CallbackInterface &, Log *);

    // return set of values to write into KCA product settings storage obtained
    //  while parsing index XML files from "UpdateSettings" attributes
    std::vector<UpdatedSettings> updatedSettings()const;

    // parse XML file. Fill with values parameters from constructor
    bool parseXMLRecursively(XmlReader &, int levelGuard);

    NoCaseString& GetUpdateDate(NoCaseString& strUpdateDate) const { return strUpdateDate = m_dateFromFile; }
    NoCaseString GetUpdateDate() const { NoCaseString strUpdateDate; return GetUpdateDate(strUpdateDate); }

    NoCaseString& GetBlackDate(NoCaseString& strBlackDate) const { return strBlackDate = m_blackDateFromFile; }
    NoCaseString GetBlackDate() const { NoCaseString strBlackDate; return GetBlackDate(strBlackDate); }

private:
    // disable copy operations
    IndexFileXMLParser &operator=(const IndexFileXMLParser &);
    IndexFileXMLParser(const IndexFileXMLParser &);


    void onStartElement(const NoCaseString &Name, const XmlAttrMap &);
    void onEndElement();
	
    FileInfo::ApplicationFilter parseBuild(const NoCaseString &applicationStringToParse)const;
    void parseUpdateSection(const XmlAttrMap &);
    void parseSettingsSection(const XmlAttrMap &);
    void parseFileOrRegistrySection(const XmlAttrMap &);
    void parseLanguageSection(const XmlAttrMap &);
    void parseFromAppVersionSection(const XmlAttrMap &);
    void parseOsSection(const XmlAttrMap &);

    // helper functions to expand files, see the expandFiles() function
    FileVector expandItem(const NoCaseString &variableName, const std::vector<NoCaseString> &variableValues, const FileInfo &)const;
    FileVector expandVariableSubstitution(const NoCaseString &variableName, const std::vector<NoCaseString> &variableValues, const FileVector &)const;

    // return a set of files corresponding to current entity
    // Sample in retranslation mode:
     // input string "Localization=[en,ru,de];Foo=[foo,bar]"
     // output is 6 files with next substitutions
     // 1: en,foo   2: en,bar
     // 1: ru,foo   2: ru,bar
     // 1: de,foo   2: de,bar
    // Sample in update mode: the single file description (most appropriate for applicaiton) is composed
    FileVector expandFiles(const NoCaseString &substituteEnvironment, const FileInfo &)const;

    NoCaseString m_strFilename;

    std::stack<NoCaseString> m_sectionHistory;
    NoCaseString m_currentSectionName;
    NoCaseString m_dateFromFile;
    NoCaseString m_blackDateFromFile;

    FileVector &m_files;
    size_t m_currentFileStartIndex;
    size_t m_currentFileEndIndex;
    Signature6Checker &m_signature6Checker;
    const Path m_parentRelativeURLPath;

    size_t m_skipSectionLevel;
    std::vector<UpdatedSettings> m_updatedSettings;

    // files are parsed differently for update and retranslation operations
     // retranslation: single XML FileDescription entity may be expanded to several files
     // update: XML FileDescription is processed depend on current localization,
     //  different files may be downloaded depends on environment
    const bool m_retranslationMode;

    CallbackInterface &m_callbacks;
    Log *pLog;
};

}   // namespace KLUPD


#endif // !defined(AFX_INDEXFILEXMLPARSER_H__22D7E101_0233_42EB_957C_3492E15902E9__INCLUDED_)
