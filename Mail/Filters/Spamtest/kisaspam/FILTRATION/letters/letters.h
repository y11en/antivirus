/*******************************************************************************
 * Proj: Content Filtration                                                    *
 * --------------------------------------------------------------------------- *
 * File: letters.h                                                             *
 * Created: Sun Nov 11 12:36:21 2001                                           *
 * Desc: Different letters formats parsing (outlook, thebat, mailbox, ... )    *
 * --------------------------------------------------------------------------- *
 * Andrey L. Kalinin, andrey@kalinin.ru                                        *
 *******************************************************************************/

/**
 * \file  letters.h
 * \brief Different letters formats parsing (outlook, thebat, mailbox, ... )
 */

#ifndef __letters_h__
#define __letters_h__

#ifdef __cplusplus
extern "C" 
{
#endif

  typedef struct __letter letter;

  struct __letter
  {
    struct
    {
      const char* begin;
      size_t      count;
    } subject;

    struct
    {
      const char* begin;
      size_t      count;
    } body;
  };

  /*
   * NOTE. Only LETTER_OUTLOOK is used now.
   */

#define LETTER_OUTLOOK 0
#define LETTER_THEBAT  1
#define LETTER_HTML    2
#define LETTER_XML     3
#define LETTER_MIME    4
#define LETTER_PLAIN   5
#define LETTER_UNIXBOX 6

  /**
   * \brief Parse text to letter struct. 
   *
   * \param txt --- input text;
   * \param type --- type of input text, see LETTER_* consts.
   * \param l --- letter struct to be filled in.
   */
  letter* letter_parse(const char* txt, unsigned int type, letter* l);
  letter* letter_create(const char* subj, const char* body, letter* l);

#ifdef __cplusplus
}
#endif

#endif // __letters_h__

/*
 * <eof letters.h>
 */
