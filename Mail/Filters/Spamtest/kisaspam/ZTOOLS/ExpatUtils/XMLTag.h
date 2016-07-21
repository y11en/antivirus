#ifndef __XMLTag_h__
#define __XMLTag_h__
//======================================================================//
#ifdef _MSC_VER
#pragma warning(error:4706) // assignment within conditional expression
#pragma warning (disable: 4786)
#pragma warning (disable: 4514) // unreferenced inline function has been removed
#endif
//======================================================================//
#include <string>
#include <map>
#include <vector>

#include "XMLStruct.h"
//======================================================================//
class XMLTag
{
public:
  XMLTag();
  virtual ~XMLTag();

  // access to tag data
  const char* Name() const { return m_name.c_str(); }
  const _STL::string& NameStr() const { return m_name; }
  
  const char* Data() const { return m_data.c_str(); }
  size_t DataSize() const { return m_data.size(); }
  const char* AttrValue(const char* attrname) const; // NULL if attrname doesn't exist
  const char* AttrValueStr(const char* attrname) const; // "" if attrname doesn't exist
  const XMLTag* Subtag(const char* name) const; // NULL if no such subtag
  const XMLTag* Subtag(size_t n) const; // NULL if no such subtag
  size_t SubtagsNumber() const { return subtags_vector.size(); }

  bool HasSubtags() const
    { return (subtags_map.size() || subtags_vector.size()); }
  bool IsEmpty() const
    { return (!m_data.size() && !subtags_map.size() && !subtags_vector.size()); }
  
  int SrcCodePage() const { return m_SrcCodePage; }
  int DestCodePage() const  { return m_DestCodePage; }
  const _TagDescription* Descr() const { return m_descr; }
  bool Parsed() const  { return m_parsed; }

  // iterators
  typedef _STL::map<_STL::string, _STL::string> AttrsMap;
  typedef AttrsMap::const_iterator AttrsIterator;
  typedef _STL::map<_STL::string, XMLTag> SubTagsMap;
  typedef SubTagsMap::const_iterator UniqueSubtagsIterator;
  typedef _STL::vector<XMLTag> SubTagsVector;
  typedef SubTagsVector::const_iterator SubtagsIterator;

  AttrsIterator AttrsBegin() const { return attrs.begin(); }
  AttrsIterator AttrsEnd() const { return attrs.end(); }
  UniqueSubtagsIterator UniqueSubtagsBegin() const { return subtags_map.begin(); }
  UniqueSubtagsIterator UniqueSubtagsEnd() const { return subtags_map.end(); }
  SubtagsIterator SubtagsBegin() const { return subtags_vector.begin(); }
  SubtagsIterator SubtagsEnd() const { return subtags_vector.end(); }

  // modification
  void Clear();
  void SetName(const char* name) { m_name = name; }
  void SetDescr(const _TagDescription* descr) { m_descr = descr; }
  bool FindDescr(const _TagDescription* parent_descr);
  
  void SetSrcCodePage(const int cp) { m_SrcCodePage = cp; }
  void SetDestCodePage(const int cp) { m_DestCodePage = cp; }

  void SetParsed(bool parsed) { m_parsed = parsed; }

  void AppendData(const char* data) { m_data += data; }
  void SetData(const char* data) { m_data = data; }

  XMLTag* AddSubtag(const char* name, const char** atts = NULL, bool unique = false);
  XMLTag* XMLTag::AddSubtag(const XMLTag& tag, bool unique = false);
  
  void AddAttrs(const char** atts);
  bool AddAttr(const char* name, const char* value); 
  void SetAttr(const char* name, const char* value);
  void DeleteAttr(const char* name);
  
  void DeleteSubtag(size_t n);

  // non constant pointers and references
  SubTagsVector& GetSubtags() { return subtags_vector; }
  XMLTag* GetSubtag(const char* name); // NULL if no such subtag
  XMLTag* GetSubtag(size_t n); // NULL if no such subtag

protected:
  //XMLTag* m_parent;
  
  _STL::string m_name;
  _STL::string m_data;

  AttrsMap attrs;
  SubTagsMap subtags_map;
  SubTagsVector subtags_vector;

  const _TagDescription* m_descr;

  bool m_parsed;

  int m_SrcCodePage;
  int m_DestCodePage;
};
//======================================================================//
#endif
