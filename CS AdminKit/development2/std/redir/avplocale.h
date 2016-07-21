// * Gettext support for Avp modules
// * by Vicente Aguilar <vaguilar@retemail.es>
 
#ifndef _AVP_LOCALE_H_
#define _AVP_LOCALE_H_

#include <locale.h>

#ifdef ENABLE_NLS
// Define to the name of the distribution.  
#define PACKAGE "avbase"

// Full path to rpm locale directory (usually /usr/share/locale) 
#ifndef LOCALEDIR
 #define LOCALEDIR "/usr/share/locale"
#endif

    #include <libintl.h>

    #define _(String) gettext (String)
    #ifdef gettext_noop
	#define N_(String) gettext_noop (String)
    #else
	#define N_(String) (String)
    #endif
#else
// Stubs that do something close enough. 
    #define textdomain(String) (String)
    #define gettext(String) (String)
    #define dgettext(Domain,Message) (Message)
    #define dcgettext(Domain,Message,Type) (Message)
    #define bindtextdomain(Domain,Directory) (Domain)
    #define _(String) (String)
    #define N_(String) (String)
#endif

#endif
