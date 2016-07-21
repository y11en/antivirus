// htmunits.h
//
// $Revision: 1.3 $
// $Date: 2000/04/20 13:52:59 $
//

#ifndef	_HTMUNITS_H_
#define	_HTMUNITS_H_

enum {
	CSS_MU_PX,	// pixels
	CSS_MU_EM,	// relative to font height
	CSS_MU_EX,	// height of letter 'x'
	CSS_MU_MM,	// millimeters
	CSS_MU_CM,	// centimeters
	CSS_MU_IN,	// inches, 2.54 cm
	CSS_MU_PT,	// points, 1/72 in
	CSS_MU_PC,	// picas, 12 pt
	CSS_MU_PR,	// percent
	// sentinel
	CSS_MU_LAST
};

bool	css_mu_translate( int ids, double v, int idd, double& d );
bool	css_mu_to_metric( double v, int id, double& d );
bool	css_mu_from_metric( double v, int id, double& d );

const char * css_mu_name( int id );
bool	css_mu_compare_name( const char *s, int id );
bool	css_mu_parse( const char *s, double *pval, int *pmu, int *pmupos );

// font utilites

#ifdef _WIN32
bool	css_font_to_logfont( const char *name, LOGFONT& lf );
bool	css_font_from_logfont( const LOGFONT& lf, string& buf );
#endif	// _WIN32

#endif	// _HTMUNITS_H_

