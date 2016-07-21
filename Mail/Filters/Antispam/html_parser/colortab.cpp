// colortab.cpp
//
// $Revision: 1.3 $
// $Date: 2000/04/20 13:52:59 $
//

#include "htmparse.h"

////////////////////////////////////////////////////////////////
// utility: colorname-to-RGB converter

static struct _sym_clr_map {
	const char *name;
	const char *rgb;
} sym_clr_map[] = {
	{ "ALICEBLUE",		"#F0F8FF" },
	{ "ANTIQUEWHITE",	"#FAEBD7" },
	{ "AQUA",		"#00FFFF" },
	{ "AQUAMARINE",		"#7FFFD4" },
	{ "AZURE",		"#F0FFFF" },
	{ "BEIGE",		"#F5F5DC" },
	{ "BISQUE",		"#FFE4C4" },
	{ "BLACK",		"#000000" },  
	{ "BLANCHEDALMOND",	"#FFEBCD" },
	{ "BLUE",		"#0000FF" },
	{ "BLUEVIOLET",		"#8A2BE2" },
	{ "BROWN",		"#A52A2A" },
	{ "BURLYWOOD",		"#DEB887" },
	{ "CADETBLUE",		"#5F9EA0" },
	{ "CHARTREUSE",		"#7FFF00" },
	{ "CHOCOLATE",		"#D2691E" },
	{ "CORAL",		"#FF7F50" },
	{ "CORNFLOWER",		"#6495ED" },
	{ "CORNSILK",		"#FFF8DC" },
	{ "CRIMSON",		"#DC143C" },
	{ "CYAN",		"#00FFFF" },
	{ "DARKBLUE",		"#00008B" },
	{ "DARKCYAN",		"#008B8B" },
	{ "DARKGOLDENROD",	"#B8860B" },
	{ "DARKGRAY",		"#A9A9A9" },
	{ "DARKGREEN",		"#006400" },
	{ "DARKKHAKI",		"#BDB76B" },
	{ "DARKMAGENTA",	"#8B008B" },
	{ "DARKOLIVEGREEN",	"#556B2F" },
	{ "DARKORANGE",		"#FF8C00" },
	{ "DARKORCHID",		"#9932CC" },
	{ "DARKRED",		"#8B0000" },
	{ "DARKSALMON",		"#E9967A" },
	{ "DARKSEAGREEN",	"#8FBC8B" },
	{ "DARKSLATEBLUE",	"#483D8B" },
	{ "DARKSLATEGRAY",	"#2F4F4F" },
	{ "DARKTURQUOISE",	"#00CED1" },
	{ "DARKVIOLET",		"#9400D3" },
	{ "DEEPPINK",		"#FF1493" },
	{ "DEEPSKYBLUE",	"#00BFFF" },
	{ "DIMGRAY",		"#696969" },
	{ "DODGERBLUE",		"#1E90FF" },
	{ "FIREBRICK",		"#B22222" },
	{ "FLORALWHITE",	"#FFFAF0" },
	{ "FORESTGREEN",	"#228B22" },
	{ "FUCHSIA",		"#FF00FF" },
	{ "GAINSBORO",		"#DCDCDC" },
	{ "GHOSTWHITE",		"#F8F8FF" },
	{ "GOLD",		"#FFD700" },
	{ "GOLDENROD",		"#DAA520" },
	{ "GRAY",		"#808080" },
	{ "GREEN",		"#008000" },
	{ "GREENYELLOW",	"#ADFF2F" },
	{ "HONEYDEW",		"#F0FFF0" },
	{ "HOTPINK",		"#FF69B4" },
	{ "INDIANRED",		"#CD5C5C" },
	{ "INDIGO",		"#4B0082" },
	{ "IVORY",		"#FFFFF0" },
	{ "KHAKI",		"#F0E68C" },
	{ "LAVENDER",		"#E6E6FA" },
	{ "LAVENDERBLUSH",	"#FFF0F5" },
	{ "LAWNGREEN",		"#7CFC00" },
	{ "LEMONCHIFFON",	"#FFFACD" },
	{ "LIGHTBLUE",		"#ADD8E6" },
	{ "LIGHTCORAL",		"#F08080" },
	{ "LIGHTCYAN",		"#E0FFFF" },
	{ "LIGHTGOLDENRODYELLOW","#FAFAD2" },
	{ "LIGHTGREEN",		"#90EE90" },
	{ "LIGHTGREY",		"#D3D3D3" },
	{ "LIGHTPINK",		"#FFB6C1" },
	{ "LIGHTSALMON",	"#FFA07A" },
	{ "LIGHTSEAGREEN",	"#20B2AA" },
	{ "LIGHTSKYBLUE",	"#87CEFA" },
	{ "LIGHTSLATEGRAY",	"#778899" },
	{ "LIGHTSTEELBLUE",	"#B0C4DE" },
	{ "LIGHTYELLOW",	"#FFFFE0" },
	{ "LIME",		"#00FF00" },
	{ "LIMEGREEN",		"#32CD32" },
	{ "LINEN",		"#FAF0E6" },
	{ "MAGENTA",		"#FF00FF" },
	{ "MAROON",		"#800000" },
	{ "MEDIUMAQUAMARINE",	"#66CDAA" },
	{ "MEDIUMBLUE",		"#0000CD" },
	{ "MEDIUMORCHID",	"#BA55D3" },
	{ "MEDIUMPURPLE",	"#9370DB" },
	{ "MEDIUMSEAGREEN",	"#3CB371" },
	{ "MEDIUMSLATEBLUE",	"#7B68EE" },
	{ "MEDIUMSPRINGGREEN",	"#00FA9A" },
	{ "MEDIUMTURQUOISE",	"#48D1CC" },
	{ "MEDIUMVIOLETRED",	"#C71585" },
	{ "MIDNIGHTBLUE",	"#191970" },
	{ "MINTCREAM",		"#F5FFFA" },
	{ "MISTYROSE",		"#FFE4E1" },
	{ "MOCCASIN",		"#FFE4B5" },
	{ "NAVAJOWHITE",	"#FFDEAD" },
	{ "NAVY",		"#000080" },
	{ "OLDLACE",		"#FDF5E6" },
	{ "OLIVE",		"#808000" },
	{ "OLIVEDRAB",		"#6B8E23" },
	{ "ORANGE",		"#FFA500" },
	{ "ORANGERED",		"#FF4500" },
	{ "ORCHID",		"#DA70D6" },
	{ "PALEGOLDENROD",	"#EEE8AA" },
	{ "PALEGREEN",		"#98FB98" },
	{ "PALETURQUOISE",	"#AFEEEE" },
	{ "PALEVIOLETRED",	"#DB7093" },
	{ "PAPAYAWHIP",		"#FFEFD5" },
	{ "PEACHPUFF",		"#FFDAB9" },
	{ "PERU",		"#CD853F" },
	{ "PINK",		"#FFC0CB" },
	{ "PLUM",		"#DDA0DD" },
	{ "POWDERBLUE",		"#B0E0E6" }, 
	{ "PURPLE",		"#800080" },
	{ "RED",		"#FF0000" },
	{ "ROSYBROWN",		"#BC8F8F" },
	{ "ROYALBLUE",		"#4169E1" },
	{ "SADDLEBROWN",	"#8B4513" },
	{ "SALMON",		"#FA8072" },
	{ "SANDYBROWN",		"#F4A460" },
	{ "SEAGREEN",		"#2E8B57" },
	{ "SEASHELL",		"#FFF5EE" },
	{ "SIENNA",		"#A0522D" },
	{ "SILVER",		"#C0C0C0" },
	{ "SKYBLUE"		"#87CEEB" },
	{ "SLATEBLUE",		"#6A5ACD" },
	{ "SLATEGRAY",		"#708090" },
	{ "SNOW",		"#FFFAFA" },
	{ "SPRINGGREEN",	"#00FF7F" },
	{ "STEELBLUE",		"#4682B4" },
	{ "TAN",		"#D2B48C" },
	{ "TEAL",		"#008080" },
	{ "THISTLE",		"#D8BFD8" },
	{ "TOMATO",		"#FF6347" },
	{ "TURQUOISE",		"#40E0D0" },
	{ "VIOLET",		"#EE82EE" },
	{ "WHEAT",		"#F5DEB3" },
	{ "WHITE",		"#FFFFFF" },
	{ "WHITESMOKE",		"#F5F5F5" },
	{ "YELLOW",		"#FFFF00" },
	{ "YELLOWGREEN",	"#9ACD32" },
	{ NULL,	NULL }
};

static unsigned char a1h( char c )
{
 if( c>='A' ) return c-'A'+10;
 return c-'0';
}

inline unsigned char a2h( const char *s )
{
 return (a1h(s[0])<<4) + a1h(s[1]);
}

bool	html_color_parse( const char *s, unsigned long *p_rgb )
{
 if( !s ) return false;

 if( p_rgb!=NULL ) *p_rgb = 0;

 char buf[40];
 strncpy( buf,s,39 );
 buf[39] = 0;
 for( char *ps=buf; *ps!=0; ++ps ) { if(*ps>='a' && *ps<='z') *ps -= 'a'-'A'; }

 if( buf[0]!='#' ) {
	_sym_clr_map *p = sym_clr_map;
	for( ; p->name!=NULL; ++p ) {
		if( p->name[0]==buf[0] && strcmp( p->name, buf )==0 ) break;
	}
	if( p->name==NULL ) return false;
	if( p_rgb==NULL ) return true;
	strcpy( buf, p->rgb );
 }

 if( !isxdigit(buf[1]) || !isxdigit(buf[2]) || !isxdigit(buf[3]) ) return false;
 if( buf[4]==0 ) {
	// 3-digit RGB
	if( p_rgb!=NULL ) {
		unsigned char cr = a1h(buf[1])*17;
		unsigned char cg = a1h(buf[2])*17;
		unsigned char cb = a1h(buf[3])*17;
		*p_rgb = ((((unsigned long)cb << 8) + cg) << 8) + cr;
	}
 } else {
	if( !isxdigit(buf[4]) || !isxdigit(buf[5]) || !isxdigit(buf[6]) ) return false;
	if( p_rgb!=NULL ) {
		unsigned char cr = a2h(buf+1);
		unsigned char cg = a2h(buf+3);
		unsigned char cb = a2h(buf+5);
		*p_rgb = ((((unsigned long)cb << 8) + cg) << 8) + cr;
	}
 }

 return true;
}

void	html_color_format( unsigned long rgb, char *buf )
{
 int r = (int)(rgb & 0xFF);
 int g = (int)((rgb>>8) & 0xFF);
 int b = (int)((rgb>>16) & 0xFF);
 sprintf( buf, "#%.2X%.2X%.2X", r, g, b );
}


