# perl script gets text from HTTP, converts to WIN codepage and prints out to console
# nik@kaspersky.com
# 
require Encode;

use LWP;
use LWP::UserAgent;

sub wintodos {
  my $pvdcoderdos = shift;
  $pvdcoderdos =~
tr/\xC0\xC1\xC2\xC3\xC4\xC5\xC6\xC7\xC8\xC9\xCA\xCB\xCC\xCD\xCE\xCF\xD0\xD1\
xD2\xD3\xD4\xD5\xD6\xD7\xD8\xD9\xDA\xDB\xDC\xDD\xDE\xDF\xE0\xE1\xE2\xE3\xE4\
xE5\xE6\xE7\xE8\xE9\xEA\xEB\xEC\xED\xEE\xEF\xF0\xF1\xF2\xF3\xF4\xF5\xF6\xF7\
xF8\xF9\xFA\xFB\xFC\xFD\xFE\xFF/\x80\x81\x82\x83\x84\x85\x86\x87\x88\x89\x8A
\x8B\x8C\x8D\x8E\x8F\x90\x91\x92\x93\x94\x95\x96\x97\x98\x99\x9A\x9B\x9C\x9D
\x9E\x9F\xA0\xA1\xA2\xA3\xA4\xA5\xA6\xA7\xA8\xA9\xAA\xAB\xAC\xAD\xAE\xAF\xE0
\xE1\xE2\xE3\xE4\xE5\xE6\xE7\xE8\xE9\xEA\xEB\xEC\xED\xEE\xEF/;
  return $pvdcoderdos;
}


sub koitowin {
  my $pvdcoderwin = shift;
  $pvdcoderwin =~
tr/\xE1\xE2\xF7\xE7\xE4\xE5\xF6\xFA\xE9\xEA\xEB\xEC\xED\xEE\xEF\xF0\xF2\xF3\
xF4\xF5\xE6\xE8\xE3\xFE\xFB\xFD\xFF\xF9\xF8\xFC\xE0\xF1\xC1\xC2\xD7\xC7\xC4\
xC5\xD6\xDA\xC9\xCA\xCB\xCC\xCD\xCE\xCF\xD0\xD2\xD3\xD4\xD5\xC6\xC8\xC3\xDE\
xDB\xDD\xDF\xD9\xD8\xDC\xC0\xD1/\xC0\xC1\xC2\xC3\xC4\xC5\xC6\xC7\xC8\xC9\xCA
\xCB\xCC\xCD\xCE\xCF\xD0\xD1\xD2\xD3\xD4\xD5\xD6\xD7\xD8\xD9\xDA\xDB\xDC\xDD
\xDE\xDF\xE0\xE1\xE2\xE3\xE4\xE5\xE6\xE7\xE8\xE9\xEA\xEB\xEC\xED\xEE\xEF\xF0
\xF1\xF2\xF3\xF4\xF5\xF6\xF7\xF8\xF9\xFA\xFB\xFC\xFD\xFE\xFF/;
  return $pvdcoderwin;
}

if (length($ARGV[0]) == 0 || length($ARGV[1]) == 0 )
{
 print"\n\nusage example get_http_content.pl Param1 Param2 [[Param3] [Param4]]\nwhere Param1 - product abbreviation such as 'kav'\n      Param2 - product build number '6.0.0.1'\n      Param3 - tt login\n      Param4 - tt password";
}
else
{
 my $product_name = $ARGV[0];
 my $product_build= $ARGV[1];

 my $user = $ARGV[2];
 my $pasw = $ARGV[3];
 
# retreive content from bt.kaspersky.com
# Create a user agent object


use HTTP::Cookies;
$cookie_jar = HTTP::Cookies->new( file => "lwp_cookies.dat", autosave => 1, ignore_discard => 1);#

#$cookie_jar->set_cookie( 0, "tt_login", $user, "/tmtrack" , "bt.kaspersky.com");#, 443, False, True, 86400, False );
#$cookie_jar->set_cookie( 0, "tt_passw", $pasw, "/tmtrack" , "bt.kaspersky.com");#, 443, False, True, 86400, False );
#$cookie_jar->set_cookie( 0, "tt_autologin", "1", "/tmtrack","bt.kaspersky.com");#, 443, False, True, 86400, False );


# retreive content from bt.kaspersky.com
# Create a user agent object
$ua = LWP::UserAgent->new;
#$ua->agent("Mozilla/4.0 (compatible; MSIE 6.0; Windows NT 5.1)");#MyAgent/1.0

$ua->cookie_jar($cookie_jar); # $ua->cookie_jar( {} ); # default temporary cookie jar

# Create a request to open script page

my $req_uri = 'https://bt.kaspersky.com/tmtrack/tmtrack.dll?ScriptPage';
my $req_form = '&scriptName=show_fixes_in_build&abr='.$product_name.'&build='.$product_build;


$req_form .= '&ttAuthUID='.$user;
if(length($pasw) != 0){
	$req_form .= '&ttAuthPWD='.$pasw;
}

$req_uri .= $req_form;



#print "\n\nTrying to open scriptPage\nURI:$req_uri\nFORM:$req_form\n";

my $req = HTTP::Request->new(GET => $req_uri);
$req->content_type('application/x-www-form-urlencoded');
#$req->content($req_form);

# Pass request to the user agent and get a response back
my $res = $ua->request($req);

# Check the outcome of the response

if ($res->is_success) {
	#print Encode::encode('cp866', Encode::decode("koi8r", $res->content));
	print Encode::encode('cp1251', Encode::decode("koi8r", $res->content));
	
}
else {
	print $res->status_line, "\n";
}


# output all data to default console

}

