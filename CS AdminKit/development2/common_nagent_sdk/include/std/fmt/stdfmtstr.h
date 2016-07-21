#ifndef __KLSTDFMTSTR_H__
#define __KLSTDFMTSTR_H__

#include <climits>

namespace KLSTD
{
    const size_t c_nStdAutoArgs = UINT_MAX;
};

/*!
  \brief	Formats message

    Within the message text, several escape sequences are supported for 
    dynamically formatting the message. These escape sequences and their 
    meanings are shown in the following table. All escape sequences start 
    with the percent character (%). 
    Escape Sequence	Meaning
    %0  Terminates a message text line without a trailing new line 
        character. This escape sequence can be used to build up long 
        lines or to terminate the message itself without a 
        trailing new line character. It is useful for prompt messages.
    %n[!s!] Identifies an insert. The value of n can be in the range 1 
        through 99. 
    Any other nondigit character following a percent character is 
    formatted in the output message without the percent character. 

    Following are some special escape sequences: 
    Format string	Resulting output
    %%  A single percent sign in the formatted message text.
    %n  A hard line break when the format string occurs at the end of a
        line. This format string is useful when DoFormatMessage is 
        supplying regular line breaks so the message fits in a certain 
        width.
    %space  A space in the formatted message text. This format string can 
        be used to ensure the appropriate number of trailing spaces in a 
        message text line.
    %.  A single period in the formatted message text. This format string 
        can be used to include a single period at the beginning of a line 
        without terminating the message text definition.
    %!  A single exclamation point in the formatted message text. This 
        format string can be used to include an exclamation point 
        immediately after an insert without its being mistaken for the 
        beginning of a printf format string.

  \param	szwTemplate template string (see above)
  \param	pszwArgs    arguments
  \param	nArgs       number of arguments. If c_nStdAutoArgs then 
                        array of pszwArgs must be NULL-ended
  \param	wstrResult  resulting formatted string
  \return	true if success
*/
KLCSC_DECL bool KLSTD_FormatMessage(
                const wchar_t*  szwTemplate,
                const wchar_t** pszwArgs,
                size_t          nArgs,
                KLSTD::AKWSTR&   wstrResult);

#endif //__KLSTDFMTSTR_H__
