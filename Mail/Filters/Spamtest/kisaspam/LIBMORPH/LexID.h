/* Copyright (C) 2001-2003 Ashmanov & Partners company, Moscow, Russia
 * All rigts reserved.
 *
 * PROJECT: LINGUISTIC PROCESSOR
 *//*!
  \file  LexID.h
  \brief The structure of lexeme ID
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
#ifndef __LexID_H
#define __LexID_H

#ifdef __cplusplus
extern "C" {
#endif

/*************** TYPES DEFINITION ******************************************/
typedef unsigned long LEXID;
typedef LEXID* PLEXID;
typedef const LEXID* PCLEXID;

#define LEXNONE 0
#define LEXINVALID 0xFFFFFFFF

/*! \struct _LexemeStrict
    \brief  Structure of strict LexID: FYI only, actually - DWORD
*/
struct _LexemeStrict {
    unsigned m:4;       /*!< morphology number */
    unsigned c:2;       /*!< capitalization sheme */
    unsigned f:8;       /*!< form */
    unsigned id:18;     /*!< lexeme id: 262 143 total */
};

/*! \struct _LexemeFuzzy
    \brief  Structure of fuzzy LexID: FYI only, actually - DWORD
*/
struct _LexemeFuzzy {
    unsigned m:4;       /*!< morphology number */
    unsigned c:2;       /*!< capitalization sheme */
    unsigned f:8;       /*!< form */
    unsigned id:18;     /*!< lexeme id: 262 143 total */
};

/*! \struct _LexemeHier
    \brief  Structure of hieroglyph LexID: FYI only, actually - DWORD
*/
struct _LexemeHier {
    unsigned m:4;       /*!< morphology number */
    unsigned c:2;       /*!< capitalization sheme */
    unsigned id:26;     /*!< lexeme id: 67 108 863 total, actually - half */
};

/*! \struct _LexemeControl
    \brief  Structure of service LexID: FYI only, actually - DWORD
*/
struct _LexemeControl {
    unsigned m:4;       /*!< morphology number: 0xF */
    unsigned t:4;       /*!< control type */
    unsigned r:14;      /*!< reserved */
    unsigned hno:10;    /*!< numeral info: homonym amount, markup tag etc. */
};

#define LEX_CTRL_MASK    0xFF000000
#define LEX_CTRL_HOMO    0xF0000000
#define LEX_CTRL_MARKUP  0xF1000000

/*! \note The structure of LEXID */
#define LEX_MORPH     0xF0000000
#define LEX_CAPS      0x0C000000
#define LEX_IDNORM    0x0003FFFF
#define LEX_FORM      (~(LEX_MORPH|LEX_CAPS|LEX_IDNORM))
#define LEX_IDHIER    (LEX_FORM|LEX_IDNORM)
#define LEX_NOHOMO    0x000003FF

#define CAP_NOCASE    0x00000000
#define CAP_LOWERCASE 0x04000000
#define CAP_UPPERCASE 0x08000000
#define CAP_TITLECASE (CAP_LOWERCASE|CAP_UPPERCASE)

#define PUNCTSPACE_LEFT  CAP_UPPERCASE
#define PUNCTSPACE_RIGHT CAP_LOWERCASE
#define PUNCTSPACE_BOTH  CAP_TITLECASE
#define PUNCTSPACE_NONE  CAP_NOCASE

/*! \note Reserved Morphology numbers */
#define MORPHNO_BASE  0x0
#define MORPHNO_PUNCT 0x1
#define MORPHNO_NUM   0x2
#define MORPHNO_CTRL  0xF


/*************** FUNCTION PROTOTYPES ***************************************/

#ifdef __cplusplus
} /* extern "C" */

/*! \note Lexid helpers */
inline unsigned short LexNoMrph( LEXID lex ) { return (unsigned short)((lex&LEX_MORPH)>>28); }
inline LEXID LexIdNorm( LEXID lex ) { return lex&LEX_IDNORM; }
inline unsigned char  LexIdForm( LEXID lex ) { return LexNoMrph(lex) > 2 ? (unsigned char)((lex&LEX_FORM)>>18) : (unsigned char)0; }
inline LEXID LexIdHier( LEXID lex ) { return lex&LEX_IDHIER; }
inline LEXID LexId( LEXID lex )     { return (LexNoMrph(lex)>2) ? (lex&LEX_IDNORM) : (lex&LEX_IDHIER); }

inline bool  LexIsNum( LEXID lex )   { return (lex&LEX_MORPH)==(MORPHNO_NUM<<28); }
inline bool  LexIsPunct( LEXID lex ) { return (lex&LEX_MORPH)==(MORPHNO_PUNCT<<28); }
inline bool  LexIsBase( LEXID lex )  { return (lex&LEX_MORPH)==(MORPHNO_BASE<<28); }
inline bool  LexNone( LEXID lex )    { return LexIsNum(lex) ? false : LexIdHier(lex) == LEXNONE; }

inline bool  LexIsUpper( LEXID lex )  { return LexIsPunct(lex) ? false: (lex&LEX_CAPS)==CAP_UPPERCASE; }
inline bool  LexIsLower( LEXID lex )  { return LexIsPunct(lex) ? false: (lex&LEX_CAPS)==CAP_LOWERCASE; }
inline bool  LexIsTitle( LEXID lex )  { return LexIsPunct(lex) ? false: (lex&LEX_CAPS)==CAP_TITLECASE; }
inline bool  LexIsNoCase( LEXID lex ) { return LexIsPunct(lex) ? true: (lex&LEX_CAPS)==CAP_NOCASE; }

inline bool  LexIsCtrl( LEXID lex )  { return lex != LEXINVALID && LEXID(lex&LEX_MORPH)==LEXID(MORPHNO_CTRL<<28); }
inline bool  LexIsCtrlHomo( LEXID lex )   { return (lex&LEX_CTRL_MASK)==LEX_CTRL_HOMO; }
inline bool  LexIsCtrlMarkup( LEXID lex ) { return (lex&LEX_CTRL_MASK)==LEX_CTRL_MARKUP; }

inline unsigned short LexNoHomo( LEXID lex )  { return LexIsCtrlHomo(lex) ? ((unsigned short)(lex&LEX_NOHOMO)) : (unsigned short)0; }

inline unsigned short LexMarkupTag( LEXID lex )  { return LexIsCtrlMarkup(lex) ? (unsigned short)(((unsigned short)lex)&0x7FFF) : (unsigned short)0; }
inline bool  LexIsMarkupOpening( LEXID lex ) { return LexIsCtrlMarkup(lex) ? ((((unsigned short)lex)&0x8000)==0) : false; }
inline bool  LexIsMarkupClosing( LEXID lex ) { return LexIsCtrlMarkup(lex) ? ((((unsigned short)lex)&0x8000)!=0) : false; }

inline unsigned char  LexPunctSpace( LEXID lex )  { return LexIsPunct(lex) ? ((unsigned char)((lex&LEX_CAPS)>>26)): (unsigned char)0; }
inline bool  LexPunctSpaceRight( LEXID lex )      { return LexIsPunct(lex) ? ((lex&PUNCTSPACE_RIGHT)!=0) : false; }
inline bool  LexPunctSpaceLeft( LEXID lex )       { return LexIsPunct(lex) ? ((lex&PUNCTSPACE_LEFT)!=0)  : false; }
inline bool  LexPunctSpaceBoth( LEXID lex )       { return LexIsPunct(lex) ? ((lex&PUNCTSPACE_BOTH)==PUNCTSPACE_BOTH) : false; }

inline LEXID LexSetUpper( LEXID lex )  { return (LexIsNum(lex)||LexIsPunct(lex)) ? lex : (lex&(~LEX_CAPS))|CAP_UPPERCASE; }
inline LEXID LexSetLower( LEXID lex )  { return (LexIsNum(lex)||LexIsPunct(lex)) ? lex : (lex&(~LEX_CAPS))|CAP_LOWERCASE; }
inline LEXID LexSetTitle( LEXID lex )  { return (LexIsNum(lex)||LexIsPunct(lex)) ? lex : (lex&(~LEX_CAPS))|CAP_TITLECASE; }
inline LEXID LexSetNoCase( LEXID lex ) { return (lex&(~LEX_CAPS)); }

inline bool  LexEq( LEXID lex1, LEXID lex2 ) { lex1&=(~LEX_CAPS); lex2&=(~LEX_CAPS); return (LexNoMrph(lex1)>2 && LexNoMrph(lex2)>2) ? (lex1&(~LEX_FORM))==(lex2&(~LEX_FORM)) : (lex1==lex2); }

inline LEXID LexSetNoMrph( LEXID lex, unsigned char no ) { lex &= (~LEX_MORPH); return lex | (LEXID(no&0xF)<<28); }
inline LEXID LexSetForm( LEXID lex, unsigned char f ) { if (LexNoMrph(lex)<=2) return lex; lex &= (~LEX_FORM); return lex | (LEXID(f)<<18); }
inline LEXID LexNormalForm( LEXID lex ) { return LexNoMrph(lex) > 2 ? (lex&(~LEX_FORM)) : lex; }
inline LEXID LexNormalize( LEXID lex ) { return LexSetNoCase(LexNormalForm(lex)); }

#else /* __cplusplus */

#define LexNoMrph(lex) ((unsigned short)(((lex)&LEX_MORPH)>>28))
#define LexIdNorm(lex) ((lex)&LEX_IDNORM)
#define LexIdForm(lex) ((unsigned char)(LexNoMrph(lex) > 2 ? (unsigned char)(((lex)&LEX_FORM)>>18) : 0))
#define LexIdHier(lex) ((lex)&LEX_IDHIER)
#define LexId(lex)     ((LexNoMrph(lex)>2) ? ((lex)&LEX_IDNORM) : ((lex)&LEX_IDHIER))

#define LexIsNum(lex)   (((lex)&LEX_MORPH)==(MORPHNO_NUM<<28))
#define LexIsPunct(lex) (((lex)&LEX_MORPH)==(MORPHNO_PUNCT<<28))
#define LexIsBase(lex)  (((lex)&LEX_MORPH)==(MORPHNO_BASE<<28))
#define LexNone(lex)    (LexIsNum(lex) ? 0 : LexIdHier(lex) == LEXNONE)

#define LexIsUpper(lex)  (LexIsPunct(lex) ? 0 : ((lex)&LEX_CAPS)==CAP_UPPERCASE)
#define LexIsLower(lex)  (LexIsPunct(lex) ? 0 : ((lex)&LEX_CAPS)==CAP_LOWERCASE)
#define LexIsTitle(lex)  (LexIsPunct(lex) ? 0 : ((lex)&LEX_CAPS)==CAP_TITLECASE)
#define LexIsNoCase(lex) (LexIsPunct(lex) ? 1 : ((lex)&LEX_CAPS)==CAP_NOCASE)

#define LexIsCtrl(lex)  ((lex) != LEXINVALID && LEXID((lex)&LEX_MORPH)==LEXID(MORPHNO_CTRL<<28))
#define LexIsCtrlHomo(lex)   (((lex)&LEX_CTRL_MASK)==LEX_CTRL_HOMO)
#define LexIsCtrlMarkup(lex) (((lex)&LEX_CTRL_MASK)==LEX_CTRL_MARKUP)

#define LexNoHomo(lex)  (LexIsCtrlHomo(lex) ? ((unsigned short)((lex)&LEX_NOHOMO)) : 0)

#define LexMarkupTag(lex)  (LexIsCtrlMarkup(lex) ? (((unsigned short)lex)&0x7FFF) : 0)
#define LexIsMarkupOpening(lex) (LexIsCtrlMarkup(lex) ? ((((unsigned short)(lex))&0x8000)==0) : 0)
#define LexIsMarkupClosing(lex) (LexIsCtrlMarkup(lex) ? ((((unsigned short)(lex))&0x8000)!=0) : 0)

#define LexPunctSpace(lex)  (LexIsPunct(lex) ? ((unsigned char)(((lex)&LEX_CAPS)>>26)): 0)
#define LexPunctSpaceRight(lex)      (LexIsPunct(lex) ? (((lex)&PUNCTSPACE_RIGHT)!=0) : 0)
#define LexPunctSpaceLeft(lex)       (LexIsPunct(lex) ? (((lex)&PUNCTSPACE_LEFT)!=0)  : 0)
#define LexPunctSpaceBoth(lex)       (LexIsPunct(lex) ? (((lex)&PUNCTSPACE_BOTH)==PUNCTSPACE_BOTH) : 0)

#define LexSetUpper(lex)  ((LexIsNum(lex)||LexIsPunct(lex)) ? (lex) : ((lex)&(~LEX_CAPS))|CAP_UPPERCASE)
#define LexSetLower(lex)  ((LexIsNum(lex)||LexIsPunct(lex)) ? (lex) : ((lex)&(~LEX_CAPS))|CAP_LOWERCASE)
#define LexSetTitle(lex)  ((LexIsNum(lex)||LexIsPunct(lex)) ? (lex) : ((lex)&(~LEX_CAPS))|CAP_TITLECASE)
#define LexSetNoCase(lex) ((lex)&(~LEX_CAPS))

#if 0
inline bool  LexEq( LEXID lex1, LEXID lex2 ) { lex1&=(~LEX_CAPS); lex2&=(~LEX_CAPS); return (LexNoMrph(lex1)>2 && LexNoMrph(lex2)>2) ? (lex1&(~LEX_FORM))==(lex2&(~LEX_FORM)) : (lex1==lex2); }

inline LEXID LexSetNoMrph( LEXID lex, unsigned char no ) { lex &= (~LEX_MORPH); return lex | (LEXID(no&0xF)<<28); }
inline LEXID LexSetForm( LEXID lex, unsigned char f ) { if (LexNoMrph(lex)<=2) return lex; lex &= (~LEX_FORM); return lex | (LEXID(f)<<18); }
#endif

#define LexNormalForm(lex) (LexNoMrph(lex) > 2 ? ((lex)&(~LEX_FORM)) : (lex))
#define LexNormalize(lex) (LexSetNoCase(LexNormalForm(lex)))

#endif /*__cplusplus*/

#endif /*__LexID_H*/
