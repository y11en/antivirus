#include "map_wrapper.h"

#include <sstream>


KLUPD::PathSubstitutionFinderByKey::PathSubstitutionFinderByKey(const NoCaseString &key)
 : m_key(key)
{
}

bool KLUPD::PathSubstitutionFinderByKey::operator()(const std::pair<NoCaseString, NoCaseString> &right)const
{
    return m_key == right.first;
}


//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

void KLUPD::MapStringVStringWrapper::operator=(const MapStringVStringWrapper &source)
{
	m_data.assign(source.m_data.begin(), source.m_data.end());
}
void KLUPD::MapStringVStringWrapper::Clear()
{
	m_data.clear();
}

KLUPD::NoCaseString KLUPD::MapStringVStringWrapper::toString()const
{
    std::ostringstream stream;
    stream.imbue(std::locale::classic());

    for(ConstIterator keyIter = m_data.begin(); keyIter != m_data.end(); ++keyIter)
    {
        // trace key
        stream << keyIter->first.toAscii();
        if(!keyIter->second.empty())
        {
            // trace values
            stream << "(";
            for(std::vector<NoCaseString>::const_iterator valueIter = keyIter->second.begin(); valueIter != keyIter->second.end(); ++valueIter)
            {
                stream << valueIter->toAscii();
                if(valueIter + 1 != keyIter->second.end())
                    stream << ", ";
            }
            stream << ")";
        }

        if(keyIter + 1 != m_data.end())
            stream << ", ";
    }

    return asciiToWideChar(stream.str());
}

long KLUPD::MapStringVStringWrapper::GetSize()
{
	return m_data.size();
}

void KLUPD::MapStringVStringWrapper::AddPair(const NoCaseString &key, const std::vector<NoCaseString> &value)
{
    m_data.push_back(std::make_pair(key, value));
}

void KLUPD::MapStringVStringWrapper::AddPairs(const std::vector<std::pair<NoCaseString, NoCaseString> > &source)
{
	for(std::vector<std::pair<NoCaseString, NoCaseString> >::const_iterator
							iter = source.begin(); iter != source.end(); ++iter)
	{
        std::vector<NoCaseString> stringVector;
		stringVector.push_back(iter->second);
		m_data.push_back(std::make_pair(iter->first, stringVector));
	}
}

void KLUPD::MapStringVStringWrapper::remove(const NoCaseString &key)
{
    for(Iterator iter = m_data.begin(); iter != m_data.end();)
    {
        if(iter->first == key)
		{
            m_data.erase(iter);
            iter = m_data.begin();
        }
        else
            ++iter;
    }
}

bool KLUPD::MapStringVStringWrapper::Find(const NoCaseString &key, std::vector<NoCaseString> &value)const
{
    for(ConstIterator iter = m_data.begin(); iter != m_data.end(); ++iter)
    {
        if(iter->first == key)
		{
            value = iter->second;
            return true;
        }
    }
    return false;
}
