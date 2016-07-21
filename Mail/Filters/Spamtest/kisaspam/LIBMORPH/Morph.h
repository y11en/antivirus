/* Copyright (C) 2001-2003 Ashmanov & Partners company, Moscow, Russia
 * All rigts reserved.
 *
 * PROJECT: LINGUISTIC PROCESSOR
 * NAME:    Generic Morphology
 *//*!
  \file  Morph.h
  \brief Generic Morphology
 *//*
 *
 * COMPILER: MS VC++ 6.0, GCC     TARGET: Win32,FreeBSD-x86,Linux-x86
 *  LIB: none
 * XREF: none
 *==========================================================================
 * REVISION:
 * 15.10.01: v 1.00 created by AlexIV
 *==========================================================================
 */
#ifndef __Morph_H
#define __Morph_H

#include <wintypes.h>
#include "LexID.h"

#ifdef __cplusplus
extern "C" {
#endif

/*************** GENERAL DEFINES *******************************************/
#if !defined(APIENTRY)
#if defined(_MSC_VER)
    #define  APIENTRY __stdcall
#else
    #define  APIENTRY
#endif
#endif

/*! \def MORPHID_INVALID 
    \def MORPHID_BASE
    \brief Global morphology identifiers
*/
#define MORPHID_INVALID 0xFFFF
#define MORPHID_BASE    0

#if !defined(WORDLEN_MAX)
#define WORDLEN_MAX 1024
#endif

/*! \defgroup ERRORS Error codes
    \see <errno.h> 
@{*/
#ifndef ELING_OK
#define ELING_OK        (0)   /*!< no error */
#define ELING_ENOTIMPL  (-1)  /*!< not implemented */
#define ELING_CONTINUE  (-3)  /*!< continue iteration */
#define ELING_BREAK     (-4)  /*!< break iteration */
#define ELING_EFAULT    (-14) /*!< unknown fault */
#define ELING_EIO       (-5)  /*!< I/O error */
#define ELING_E2BIG     (-7)  /*!< Argument list too long */
#define ELING_EACCES    (-13) /*!< Permission denied */
#define ELING_EAGAIN    (-11) /*!< No more processes */
#define ELING_ECHILD    (-10) /*!< No spawned processes */
#define ELING_EDEADLOCK (-36) /*!< Resource deadlock would occur */
#define ELING_EEXIST    (-17) /*!< File exists */
#define ELING_EINVAL    (-22) /*!< Invalid argument */
#define ELING_EMFILE    (-24) /*!< Too many open files */
#define ELING_ENOENT    (-2)  /*!< No such file or directory */
#define ELING_ENOMEM    (-12) /*!< Not enough memory */
#define ELING_ENOSPC    (-28) /*!< No space left on device */
#define ELING_ERANGE    (-34) /*!< Result too large */
#define ELING_EMPTY     (-35) /*!< Dataset is empty */
#endif
/*@}*/ 

/*************** TYPES DEFINITION ******************************************/
#if !defined(__HMORPH_defined__)
#define  __HMORPH_defined__

typedef struct tag_HMORPH { int unused; } *HMORPH;
typedef HMORPH* PHMORPH;

typedef short (APIENTRY* TEnumWords)( LEXID idLex, void* pv );

#define MORPH_TYPE(wType) ((wType)&0x000F)
#define MORPH_HIER     0x0001
#define MORPH_FUZZY    0x0002
#define MORPH_STRICT   0x0004
#define MORPH_GENERIC  0x0008
#define MORPH_NUM      (MORPH_GENERIC|MORPH_HIER)
#define MORPH_PUNCT    (MORPH_GENERIC|MORPH_STRICT)
#define MORPH_CTRL     0x000F

/*! \defgroup MORPHMODES Morphology Open Modes
    \note 0 means "default": in-memory nonpersistent read-write.
@{*/
#ifndef MORPH_MEM
#define MORPH_MEM      0x8000 /*!< In-memory: non-persistent dynamic morphology */
#define MORPH_READONLY 0x4000 /*!< Read only: does not change morphology file, but can append */
#define MORPH_NOAPPEND 0x2000 /*!< No append: readonly without words append */
#define MORPH_CREATE   0x1000 /*!< Create: truncate file if it exists, try to create if it doesn't, error if fails */
#define MORPH_KEEP     0x0800 /*!< Keep: leave unchanged modes in previous state */
#endif
/*@}*/ 

/*! \struct MorphDictInfo
    \brief  Morphology information structure
*/
typedef struct _MorphDictInfo {
    unsigned short wSize;       /*!< size of this struct in bytes */
	unsigned short wID;         /*!< dictionary ID (globally unique) */
	unsigned short wType;       /*!< type - bit mask of constants */
	unsigned short wLang;       /*!< language, see LG_* in language.h */
	unsigned short wCodePage;   /*!< codepage, see CP_* in language.h */
	unsigned short wWordLenMax; /*!< the largest word size in this dict */
	unsigned long  nLex;        /*!< amount of lexemes (approximation) */
	unsigned long  dwVer;	    /*!< dictionary version, see VERSIONINFO */
	unsigned long  dwVerPrivate;/*!< the same */
/*! \note fields, that are not persistent */
	const char*    szFileName;  /*!< dictionary file name */
    unsigned long  dwNo;        /*!< dictionary internal no */
} MorphDictInfo;

#define MorphDictInfoSize() (sizeof(unsigned short)*6+sizeof(unsigned long)*3)

#endif /*__HMORPH_defined__*/

/*! \struct _SimpleMorph
    \brief  Simple morphology access methods
*/
struct _SimpleMorph {
/* private: */
    HMORPH hMorph;
    unsigned long nRef;    /*!< Refcounter for C++ */
/* public: */
    short (APIENTRY *lemmatize)( HMORPH h, unsigned short options,
                        const char* pWord, unsigned short ccWord,
                        PLEXID pLex, unsigned short cdwLex );

    LEXID (APIENTRY *append)( HMORPH h, unsigned short options,
                        const char* pWord, unsigned short ccWord, 
                        unsigned long  dwLexInfo );

    unsigned long (APIENTRY *getlex)( HMORPH h, LEXID idLex );

    short (APIENTRY *gettext)( HMORPH h, LEXID idLex, 
                        char* szWord, unsigned short ccWord );

    short (APIENTRY *getforms)( HMORPH h, unsigned short options,
                        LEXID idLex, unsigned long dwLexInfo/*0-All*/, 
                        unsigned char* pForms, unsigned short ccForms );

    MorphDictInfo* (APIENTRY *info)( HMORPH h );

    short (APIENTRY *flush)( HMORPH h );
    short (APIENTRY *close)( HMORPH h );
};

/*************** FUNCTION PROTOTYPES ***************************************/
/*! \brief Create SimpleMorph object
    \param nMode - use MORPH_READONLY | MORPH_MEM
*/
short  APIENTRY apMorphCreate( struct _SimpleMorph* pDest, const char* szPath, unsigned short nMode, unsigned char No );

HMORPH APIENTRY apMorphOpen( const char* szPath, unsigned short nMode, unsigned char No );

short  APIENTRY apMorphEnum( HMORPH h, TEnumWords pFun, void *pv );
  
short  APIENTRY apMorphLemmatize( HMORPH h, unsigned short options,
  const char* pWord, unsigned short ccWord,
  PLEXID pLex, unsigned short cdwLex );

unsigned long APIENTRY apMorphGetLex( HMORPH h, LEXID idLex );

short  APIENTRY apMorphGetText( HMORPH h, LEXID idLex, 
  char* szWord, unsigned short ccWord );

short  APIENTRY apMorphGetForms( HMORPH h, unsigned short options,
  LEXID idLex, unsigned long dwLexInfo/*0-All*/, 
  unsigned char* pForms, unsigned short ccForms );

LEXID  APIENTRY apMorphAppend( HMORPH h, unsigned short options,
  const char* pWord, unsigned short ccWord,
  unsigned long  dwLexInfo );

short  APIENTRY apMorphRemove( HMORPH h, LEXID idLex );

const  MorphDictInfo* APIENTRY apMorphGetInfo( HMORPH h );
short  APIENTRY apMorphSetInfo( HMORPH h, const MorphDictInfo *pInfo );

short  APIENTRY apMorphSetCharSet( HMORPH h, void* pCharSet );
       /* Dirty trick - send pointer to C++ class as void* */

short  APIENTRY apMorphFlush( HMORPH h );
short  APIENTRY apMorphClose( HMORPH h );

#ifdef __cplusplus
} /* extern "C" */

/*! \class SimpleMorph
    \brief _SimpleMorph encapsulation
 */
class SimpleMorph
{
protected: 
  _SimpleMorph* m_pMorph;
public:
  bool m_bEnabled;
// Construction
public:
  SimpleMorph( const SimpleMorph& src ) : m_pMorph(0), m_bEnabled(false)
    { operator=(src); }
  SimpleMorph( const struct _SimpleMorph& src ) : m_pMorph(0), m_bEnabled(false)
    { operator=(src); }
  SimpleMorph( const struct _SimpleMorph* pSrc = NULL ) : m_pMorph(0), m_bEnabled(false)
    { if (pSrc) operator=(*pSrc); }
  SimpleMorph& operator=( const SimpleMorph& src );
  SimpleMorph& operator=( const struct _SimpleMorph& src );
  SimpleMorph& operator=( const struct _SimpleMorph* pSrc );
  DWORD AddRef();
  DWORD Release();
  ~SimpleMorph()  { Release(); }

// Operations:
  operator HMORPH() const { return m_pMorph ? m_pMorph->hMorph : NULL; }
  short Lemmatize( PLEXID pLex, WORD cdwLex,
                   LPCSTR szWord, WORD ccWord = MAXWORD, WORD options = 0 ) const
    { return (m_pMorph->lemmatize)(m_pMorph->hMorph,options,szWord,ccWord,pLex,cdwLex); }

  LEXID Append( DWORD dwLexInfo, 
                LPCSTR szWord, WORD ccWord = MAXWORD, WORD options = 0 )
    { return (m_pMorph->append)(m_pMorph->hMorph,options,szWord,ccWord,dwLexInfo); }

  DWORD GetLex( LEXID idLex ) const
    { return (m_pMorph->getlex)(m_pMorph->hMorph,idLex); }

  short GetText( LEXID idLex, LPSTR szWord, WORD ccWord = MAXWORD) const
    { return (m_pMorph->gettext)(m_pMorph->hMorph,idLex,szWord,ccWord); }

  short GetForms( LEXID idLex, unsigned char* pForms, unsigned short ccForms,
                  unsigned long dwLexInfo = 0, unsigned short options = 0 )
    { return (m_pMorph->getforms)(m_pMorph->hMorph,options,idLex,dwLexInfo,pForms,ccForms); }

  short Flush() const { return m_pMorph ? (m_pMorph->flush)(m_pMorph->hMorph) : (short)ELING_EFAULT; }

// Information:
  MorphDictInfo* Info() { return (m_pMorph->info)(m_pMorph->hMorph); }
  WORD  GetNo() const { return WORD(LexNoMrph(((m_pMorph->info)(m_pMorph->hMorph))->dwNo)); }
  void  SetNo( WORD No ) const
  {
   ((m_pMorph->info)(m_pMorph->hMorph))->dwNo = LexSetNoMrph(0,BYTE(No));
  }
  WORD  ID() const { return (m_pMorph->info)(m_pMorph->hMorph)->wID; }
  WORD  Type() const { return (m_pMorph->info)(m_pMorph->hMorph)->wType; }
  WORD  Lang() const { return (m_pMorph->info)(m_pMorph->hMorph)->wLang; }
  WORD  CP() const { return (m_pMorph->info)(m_pMorph->hMorph)->wCodePage; }
  WORD  WordLenMax() const { return (m_pMorph->info)(m_pMorph->hMorph)->wWordLenMax; }
  DWORD size() const { return (m_pMorph->info)(m_pMorph->hMorph)->nLex; }
  LPCSTR File() const { return (m_pMorph->info)(m_pMorph->hMorph)->szFileName; }
  bool  Enabled() const { return (this && m_pMorph) ? m_bEnabled : false; }
  bool  Enable() { return m_pMorph ? (m_bEnabled=true) : (m_bEnabled=false); }
  bool  Disable() { return (m_bEnabled=false); }
  bool  IsHier()   const { return MORPH_TYPE(Type()) == MORPH_HIER;  }
  bool  IsFuzzy()  const { return MORPH_TYPE(Type()) == MORPH_FUZZY; }
  bool  IsStrict() const { return MORPH_TYPE(Type()) == MORPH_STRICT;}
  bool  IsRO() const { return (Type()&MORPH_READONLY)!=0; }
  bool  SetRO( bool bRO = true );
};

// Implementation:
inline DWORD SimpleMorph::AddRef()  
{ 
  return m_pMorph ? ++(m_pMorph->nRef) : 0; 
}
inline DWORD SimpleMorph::Release() 
{ 
  if ( !m_pMorph ) return 0;
  if ( (--(m_pMorph->nRef)) > 0 ) return m_pMorph->nRef; 
  (m_pMorph->close)(m_pMorph->hMorph); 
  delete m_pMorph;
  m_pMorph = 0;
  m_bEnabled = false;
  return 0; 
}
inline SimpleMorph& SimpleMorph::operator=( const SimpleMorph& src )
{
  Release();
  m_pMorph = src.m_pMorph;
  if ( AddRef() ) m_bEnabled = true;
  return *this;
}
inline SimpleMorph& SimpleMorph::operator=( const struct _SimpleMorph& src )
{
  Release();
  m_pMorph = const_cast<_SimpleMorph*>(&src);
  if ( AddRef() ) m_bEnabled = true;
  return *this;
}
inline SimpleMorph& SimpleMorph::operator=( const struct _SimpleMorph* pSrc )
{
  Release();
  m_pMorph = const_cast<_SimpleMorph*>(pSrc);
  if ( AddRef() ) m_bEnabled = true;
  return *this;
}
inline bool SimpleMorph::SetRO( bool bRO/*=true*/ )
{ 
  bool bWas = IsRO(); 
  if (bRO) Info()->wType|=MORPH_READONLY;
  else     Info()->wType&=(~WORD(MORPH_READONLY));
  return bWas; 
}


#endif /*__cplusplus*/

#endif /*__Morph_H*/
