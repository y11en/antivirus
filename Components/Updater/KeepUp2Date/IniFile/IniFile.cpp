#include "IniFile.h"

// TODO: when all moved to VS 2005 use new secure API, and fix secure bug
#ifdef WIN32
    // 4996 'function': was declared deprecated
    #pragma warning( disable : 4996 )
#endif

#include "../Client/helper/updaterStaff.h"

const KLUPD::NoCaseString KLUPD::IniFile::s_commentIndicators = L";#";
const KLUPD::NoCaseString KLUPD::IniFile::s_equalIndicators = L"=:";

KLUPD::IniFile::Key::Key(const NoCaseString &key, const NoCaseString &value, const NoCaseString &comment)
 : m_key(key),
   m_value(value),
   m_comment(comment)
{
}

KLUPD::IniFile::Section::Section(const NoCaseString &name, const NoCaseString &comment, const std::vector<Key> &keys)
 : m_name(name),
   m_comment(comment),
   m_keys(keys)
{
}

KLUPD::IniFile::IniFile(const KLUPD::Path &fileName, Log *log, const long flags)
 : m_flags(flags),
   m_fileName(fileName),
   m_dirty(false),
   pLog(log)
{
    m_sections.push_back(Section());
    load();
    m_dirty = false;    // TODO: some better means not to set dirty flag on load() ?
}

KLUPD::IniFile::~IniFile()
{
    if(m_dirty)
        save();
}

void KLUPD::IniFile::clear()
{
    m_dirty = true;
    m_sections.clear();
}

KLUPD::Path KLUPD::IniFile::fileName()const
{
    return m_fileName;
}

KLUPD::NoCaseString KLUPD::IniFile::getValue(const NoCaseString &keyName, const NoCaseString &section)const 
{
    const Key *key = getKey(keyName, section);
    return key ? key->m_value : NoCaseString();
}

bool KLUPD::IniFile::setValue(const NoCaseString &keyName, const NoCaseString &value, const NoCaseString &sectionName, const NoCaseString &comment)
{
    if(keyName.empty())
        return false;

    Section *section = getSection(sectionName);
    if(!section)
    {
        if(!(m_flags & AUTOCREATE_SECTIONS) || !createSection(sectionName, L""))
            return false;

        section = getSection(sectionName);
    }

    // sanity check
    if(!section)
        return false;

    // if the key does not exist in that section, and the value passed is not NoCaseString(L"") then add the new key
    Key *pKey = getKey(keyName, sectionName);
    if(!pKey && (m_flags & AUTOCREATE_KEYS))
    {
        m_dirty = true;
        section->m_keys.push_back(Key(keyName, value, comment));
        return true;
    }

    if(pKey)
    {
        pKey->m_value = value;
        pKey->m_comment = comment;
        m_dirty = true;
        return true;
    }

    return false;
}

KLUPD::NoCaseString KLUPD::IniFile::getString(const NoCaseString &keyName, const NoCaseString &sectionName)const
{
    return getValue(keyName, sectionName);
}

int	KLUPD::IniFile::getInt(const NoCaseString &keyName, const NoCaseString &sectionName)const
{
    const NoCaseString value = getValue(keyName, sectionName);
    if(value.empty())
        return INT_MIN;
    size_t result = 0;
    value.toLong(result);
    return result;
}
bool KLUPD::IniFile::setInt(const NoCaseString &keyName, const int value, const NoCaseString &sectionName, const NoCaseString &comment)
{
    char buffer[64];
    snprintf(buffer, 64, "%d", value);
    return setValue(keyName, asciiToWideChar(buffer).c_str(), sectionName, comment);
}

bool KLUPD::IniFile::getBool(const bool defaultValue, const NoCaseString &key, const NoCaseString &section)const
{
    const NoCaseString value = getValue(key, section);
    if(value.empty())
        return defaultValue;

    return value[0] == L'1'
        || (value == L"true") 
        || (value == L"yes");
}
bool KLUPD::IniFile::setBool(const NoCaseString &keyName, const bool value, const NoCaseString &comment, const NoCaseString &sectionName)
{
    return setValue(keyName, value ?  L"True" : L"False", sectionName, comment);
}

bool KLUPD::IniFile::setKeyComment(const NoCaseString &keyName, const NoCaseString &comment, const NoCaseString &sectionName)
{
    Section *section = getSection(sectionName);
    if(!section)
        return false;
    if(keyName.empty())
        return false;

    for(std::vector<Key>::iterator keyIter = section->m_keys.begin(); keyIter != section->m_keys.end(); ++keyIter)
    {
        if(keyIter->m_key == keyName)
        {
            keyIter->m_comment = comment;
            m_dirty = true;
            return true;
        }
    }
    return false;
}

bool KLUPD::IniFile::setSectionComment(const NoCaseString &sectionName, const NoCaseString &comment)
{
    for(std::vector<Section>::iterator sectionIter = m_sections.begin(); sectionIter != m_sections.end(); ++sectionIter)
    {
        if(sectionIter->m_name == sectionName)
        {
            sectionIter->m_comment = comment;
            m_dirty = true;
            return true;
        }
    }
    return false;
}

bool KLUPD::IniFile::createKey(const NoCaseString &keyName, const NoCaseString &value,
                               const NoCaseString &sectionName, const NoCaseString &comment)
{
    const bool autoKey = (m_flags & AUTOCREATE_KEYS) == AUTOCREATE_KEYS;
    m_flags |= AUTOCREATE_KEYS;

    const bool bReturn = setValue(keyName, value, sectionName, comment);

    if(!autoKey)
        m_flags &= ~AUTOCREATE_KEYS;

    return bReturn;
}
bool KLUPD::IniFile::deleteKey(const NoCaseString &keyName, const NoCaseString &fromSection)
{
    Section *section = getSection(fromSection);
    if(!section)
        return false;

    for(std::vector<Key>::iterator keyIter = section->m_keys.begin(); keyIter != section->m_keys.end(); ++keyIter)
    {
        if(keyIter->m_key == keyName)
        {
            section->m_keys.erase(keyIter);
            return true;
        }
    }

    return false;
}

bool KLUPD::IniFile::createSection(const NoCaseString &sectionName, const NoCaseString &comment)
{
    if(getSection(sectionName))
        return false;

    m_sections.push_back(Section(sectionName, comment));
    m_dirty = true;
    return true;
}
bool KLUPD::IniFile::createSection(const NoCaseString &sectionName, const NoCaseString &comment, const std::vector<Key> &keys)
{
    if(!createSection(sectionName, comment))
        return false;

    Section *section = getSection(sectionName);
    if(!section)
        return false;

    section->m_name = sectionName;
    for(std::vector<Key>::const_iterator keyIter = keys.begin(); keyIter != keys.end(); ++keyIter)
    {
        if(!keyIter->m_key.empty())
            section->m_keys.push_back(Key(keyIter->m_key, keyIter->m_value, keyIter->m_comment));
    }

    m_dirty = true;
    return true;
}
bool KLUPD::IniFile::deleteSection(const NoCaseString &sectionName)
{
    for(std::vector<Section>::iterator sectionIter = m_sections.begin(); sectionIter != m_sections.end(); ++sectionIter)
    {
        if(sectionIter->m_name == sectionName)
        {
            m_sections.erase(sectionIter);
            return true;
        }
    }

    return false;
}


KLUPD::IniFile::Key *KLUPD::IniFile::getKey(const NoCaseString &keyName, const NoCaseString &sectionName)const
{
    // Since our default section has a name value of NoCaseString("") this should
    // always return a valid section, wether or not it has any keys in it is another matter
    Section *section = getSection(sectionName);
    if(!section)
        return 0;

    for(std::vector<Key>::const_iterator keyIter = section->m_keys.begin(); keyIter != section->m_keys.end(); ++keyIter)
    {
        if(keyIter->m_key == keyName)
            return (Key *)&(*keyIter);
    }
    return 0;
}

KLUPD::IniFile::Section *KLUPD::IniFile::getSection(const NoCaseString &sectionName)const
{
    for(std::vector<Section>::const_iterator sectionIter = m_sections.begin(); sectionIter != m_sections.end(); ++sectionIter)
    {
        if(sectionIter->m_name == sectionName)
            return (Section*)&(*sectionIter);
    }
    return 0;
}

int KLUPD::IniFile::sectionCount() 
{
    return m_sections.size();
}

int KLUPD::IniFile::keyCount()
{
    int counter = 0;
    for(std::vector<Section>::const_iterator sectionIter = m_sections.begin(); sectionIter != m_sections.end(); ++sectionIter)
        counter += sectionIter->m_keys.size();
    return counter;
}


bool KLUPD::IniFile::load()
{
    if(!LocalFile(m_fileName, pLog).exists())
    {
        TRACE_MESSAGE2("File does not exist '%S'", m_fileName.toWideChar());
        return false;
    }

    // do not create a new file here.  If it doesn't exist, just return false and report the failure.
    FileStream file(pLog);
    if(!file.open(m_fileName, std::ios::in))
    {
        TRACE_MESSAGE2("Failed to load ini configuration file, because failed to open file '%S'",
            m_fileName.toWideChar());
        return false;
    }

    const bool autoKey = (m_flags & AUTOCREATE_KEYS) == AUTOCREATE_KEYS;
    const bool autoSec = (m_flags & AUTOCREATE_SECTIONS) == AUTOCREATE_SECTIONS;

    NoCaseString comment;
    Section *section = getSection(L"");

    // needs to be set to restore the original values later
    m_flags |= AUTOCREATE_KEYS | AUTOCREATE_SECTIONS;

    bool doneWithFile = false;
    while(!doneWithFile)
    {
        NoCaseString line;
        file.getLine(line);
        line.trim();

        doneWithFile = file.done();

        if(line.find_first_of(s_commentIndicators.toWideChar()) == 0)
        {
            comment += L"\n";
            comment += line;
        }
        // new section
        else if(line.find_first_of("[") == 0)
        {
            line.erase(0, 1);
            const size_t lastOffset = line.find_last_of("]");
            if(lastOffset != NoCaseStringImplementation::npos)
            {
                line.erase(lastOffset, 1);

                createSection(line, comment);
                section = getSection(line);
                comment = NoCaseString();
            }
        }
        // we have a key, add this key/value pair
        else if(!line.empty())
        {
            NoCaseString keyName = getNextWord(line);
            NoCaseString value = line;

            if(!keyName.empty())
            {
                setValue(keyName, value, section ? section->m_name : NoCaseString(), comment);
                comment = NoCaseString(L"");
            }
        }
    }

    // restore the original flag values.
    if(!autoKey)
        m_flags &= ~AUTOCREATE_KEYS;
    if(!autoSec)
        m_flags &= ~AUTOCREATE_SECTIONS;
    return true;
}

bool KLUPD::IniFile::save()
{
    if(m_fileName.empty())
    {
        TRACE_MESSAGE("Failed to save ini file, because no file name is specified");
        return false;
    }

    FileStream file(pLog);
    if(!file.open(m_fileName, std::ios::out | std::ios::trunc))
    {
        TRACE_MESSAGE2("Unable to save ini configuration file, because unable to open file '%S'", m_fileName.toWideChar());
        return false;
    }

    for(std::vector<Section>::const_iterator sectionIter = m_sections.begin(); sectionIter != m_sections.end(); ++sectionIter)
    {
        bool wroteComment = false;
        if(!sectionIter->m_comment.empty())
        {
            wroteComment = true;
            file.writeLine(NoCaseString(L"\n") + commentStr(sectionIter->m_comment));
        }

        if(!sectionIter->m_name.empty())
        {
            NoCaseString buffer = wroteComment ? L"\n" : L"";
            buffer += NoCaseString(L"[") + sectionIter->m_name + L"]";
            file.writeLine(buffer);
        }

        for(std::vector<Key>::const_iterator keyIter = sectionIter->m_keys.begin(); keyIter != sectionIter->m_keys.end(); ++keyIter)
        {
            if(keyIter->m_key.empty())
                continue;

            NoCaseString buffer = keyIter->m_comment.empty() ? L"" : L"\n";
            buffer += commentStr(keyIter->m_comment);
            buffer += keyIter->m_comment.empty() ? L"" : L"\n";
            buffer += keyIter->m_key.toWideChar();
            buffer += s_equalIndicators[0];
            buffer += keyIter->m_value;

            file.writeLine(buffer);
        }
    }

    m_dirty = false;
    return true;
}


KLUPD::NoCaseString KLUPD::IniFile::getNextWord(NoCaseString &commandLine)
{
    const size_t equalIndicatorOffset = commandLine.find_first_of(s_equalIndicators.toWideChar());
    NoCaseString word = L"";
    if(equalIndicatorOffset == NoCaseStringImplementation::npos)
    {
        word = commandLine;
        commandLine = L"";
    }
    else
    {
        word = commandLine.substr(0, equalIndicatorOffset);
        commandLine.erase(0, equalIndicatorOffset + 1);
    }

    word.trim();
    return word;
}
KLUPD::NoCaseString KLUPD::IniFile::commentStr(const NoCaseString &commentIn)
{
    NoCaseString newString = L"";
    NoCaseString comment = commentIn;
    comment.trim();
    if(comment.empty())
        return comment;

    if(comment.find_first_of(s_commentIndicators.toWideChar()) != 0)
    {
        newString = s_commentIndicators[0];
        newString += L" ";
    }
    newString += comment;
    return newString;
}
