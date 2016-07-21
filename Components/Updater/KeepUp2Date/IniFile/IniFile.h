//
// The purpose of this class is to provide a simple, full featured means to
// store persistent data to a text file.  It uses a simple key/value paradigm
// to achieve this.  The class can read/write to standard Windows .ini files,
// and yet does not rely on any windows specific calls.  It should work as
// well in a linux environment (with some minor adjustments) as it does in
// a Windows one.
//
// Inspited by class Written by Gary McNickle <gary#sunstorm.net> and downloaded from:
// http://www.codeguru.com/cpp/data/data-misc/inifiles/article.php/c4583/#more//
//
// An example might look like this;
//
// [UserSettings]
// Name=Joe User
// Date of Birth=12/25/01
//
// ;
// ; Settings unique to this server
// ;
// [ServerSettings]
// Port=1200
// IP_Address=127.0.0.1
// MachineName=ADMIN
//


#ifndef __INI_FILE_H__INCLUDED_4411F0B1_6F7B_42f2_974F_B4A30E972C5B
#define __INI_FILE_H__INCLUDED_4411F0B1_6F7B_42f2_974F_B4A30E972C5B

#include "../Client/helper/stdinc.h"

namespace KLUPD {

class Log;

//////////////////////////////////////////////////////////////////////////////////
/// Globally defined structures, defines, & types

// When set, this define will cause SetValue() to create a new section, if the requested section does not already exist
#define AUTOCREATE_SECTIONS (1L<<1)

// When set, this define causes SetValue() to create a new key, if the requested key does not already exist
#define AUTOCREATE_KEYS (1L<<2)

class IniFile
{
public:
    // The Key structure stores the definition of a key. A key is a named identifier that is associated
    //  with a value. It may or may not have a comment.  All comments must PRECEDE the key on the line
    // in the configuration file
    struct Key
    {
	    Key(const NoCaseString &key = NoCaseString(), const NoCaseString &value = NoCaseString(), const NoCaseString &comment = NoCaseString());

	    NoCaseString m_key;
	    NoCaseString m_value;
	    NoCaseString m_comment;
    };

    // This structure stores the definition of a section. A section contains any number of keys,
    //  and may or may not have a comment. Like keys, all comments must precede the section
    struct Section
    {
        Section(const NoCaseString &name = NoCaseString(), const NoCaseString &comment = NoCaseString(), const std::vector<Key> & = std::vector<Key>());

	    NoCaseString m_name;
	    NoCaseString m_comment;
	    std::vector<Key> m_keys;
    };


    // load the file, it will do so and populate the section list with the values from the file
    IniFile(const KLUPD::Path &fileName, Log *, const long flags = AUTOCREATE_SECTIONS | AUTOCREATE_KEYS);
    // saves the file if any values have changed since the last save
    virtual ~IniFile();
    // removes all data
    void clear();

    KLUPD::Path fileName()const;

    /////////////////////////////////////////////////////////////////
    /// Data handling methods

    // Returns the key value as a string object.
    // A return value of NoCaseString() indicates that the key could not be found
    // Note that this returns keys specific to the given section only
    NoCaseString getValue(const NoCaseString &key, const NoCaseString &section)const;
    // Given a key, a value and a section, this function will attempt to locate the Key within the given section,
    // and if it finds it, change the keys value to the new value. If it does not locate the key, it will create
    // a new key with the proper value and place it in the section requested.
    // Will create the key if it is not found and AUTOCREATE_KEYS is active.
    bool setValue(const NoCaseString &key, const NoCaseString &value, const NoCaseString &section, const NoCaseString &comment = NoCaseString());

    // returns the key value as a NoCaseString object.
    // A return value of NoCaseString() indicates that the key could not be found.
    NoCaseString getString(const NoCaseString &key, const NoCaseString &section = NoCaseString())const;

    // returns the key value as an integer type. Returns INT_MIN if the key is not found
    int getInt(const NoCaseString &key, const NoCaseString &section)const;
    // Sets the value of a given key. Will create the key if it is not found and AUTOCREATE_KEYS is active
    bool setInt(const NoCaseString &key, const int value, const NoCaseString &section, const NoCaseString &comment = NoCaseString());

    // return the value as a bool
    bool getBool(const bool defaultValue, const NoCaseString &key, const NoCaseString &section)const;
    // sets the value of a given key. Will create the key if it is not found and AUTOCREATE_KEYS is active
    bool setBool(const NoCaseString &key, const bool value, const NoCaseString &comment = NoCaseString(), const NoCaseString &section = NoCaseString());


    // sets the comment for a given key
    // Returns false if the key is not found.
    bool setKeyComment(const NoCaseString &key, const NoCaseString &comment, const NoCaseString &section);
    // Sets the comment for a given section
    // Returns false if the section was not found
    bool setSectionComment(const NoCaseString &section, const NoCaseString &comment);


    /////////////////////////////////////////////////////////////////
    /// Key/Section handling methods

    // Given a key, a value and a section, this function will attempt to locate the Key within the given section,
    // and if it finds it, change the keys value to the new value. If it does not locate the key, it will create
    // a new key with the proper value and place it in the section requested.
    // The Section will be created if it does not exist and the AUTOCREATE_SECTIONS bit is set
    bool createKey(const NoCaseString &key, const NoCaseString &value, const NoCaseString &section, const NoCaseString &comment = NoCaseString());
    // Delete a specific key in a specific section.
    // Returns false if the key cannot be found or true when successfully deleted.
    bool deleteKey(const NoCaseString &key, const NoCaseString &fromSection = NoCaseString());

    // Given a section name, this function first checks to see if the given section already exists in
    //  the list or not, if not, it creates the new section and assigns it the comment given in comment.
    // The function returns true if successfully created, or false otherwise. 
    bool createSection(const NoCaseString &section, const NoCaseString &comment = NoCaseString());
    // Given a section name, this function first checks to see if the given section already exists in the list or not,
    // if not, it creates the new section and assigns it the comment given in comment.  The function returns true if
    // successfully created, or false otherwise. This version accepts a std::vector<Key> and sets up the newly created
    // Section with the keys in the list.
    bool createSection(const NoCaseString &section, const NoCaseString &comment, const std::vector<Key> &);
    // Deletes a given section
    bool deleteSection(const NoCaseString &section);


    // Given a key and section name, looks up the key and if found.
    // Returns a pointer to that key, otherwise returns 0
    Key *getKey(const NoCaseString &key, const NoCaseString &section)const;
    // Given a section name, locates that section in the list
    // Returns a pointer to section. If the section was not found, returns 0
    Section *getSection(const NoCaseString &section)const;


    /////////////////////////////////////////////////////////////////
    /// Utility Methods

    // returns the number of valid sections in the database
    int sectionCount();
    // returns the total number of keys, across all sections
    int keyCount();
    
    // current settings
	long m_flags;

private:
    // load and parse configuration file. If successful it will populate
    //  the Section list with the key/value pairs found in the file.
    // Note that comments are saved so that they can be rewritten to the file later.
    bool load();
    // save the Section list and keys to the file
    bool save();



    /////////////////////////////////////////////////////////////////////////////////
    /// General Purpose Utility Functions ///////////////////////////////////////////

    // Given a key +delimiter+ value string, pulls the key name from the string, deletes
    //  the delimiter and alters the original string to contain the remainder.  Returns the key
    static NoCaseString getNextWord(NoCaseString &commandLine);
    // parses a string into a proper comment token/comment
    static NoCaseString commentStr(const NoCaseString &comment);

	std::vector<Section> m_sections;

	KLUPD::Path m_fileName;
	bool m_dirty;

    Log *pLog;


    // This constant contains the characters that we check for to determine if a  line is a comment or not.
    // Note that the first character in this constant is the one used when writing comments to disk
    // (if the comment does not already contain an indicator)
    static const NoCaseString s_commentIndicators;
    // This constant contains the characters that we check against to determine if a line contains an assignment
    // ( key = value )
    // Note that changing these from their defaults ("=:") WILL affect the ability of IniFile to read/write
    // to .ini files.  Also, note that the first character in this constant is the one that is used when writing
    // the values to the file. (EqualIndicators[0])
    static const NoCaseString s_equalIndicators; 
};


}   // namespace KLUPD

#endif  // __INI_FILE_H__INCLUDED_4411F0B1_6F7B_42f2_974F_B4A30E972C5B
