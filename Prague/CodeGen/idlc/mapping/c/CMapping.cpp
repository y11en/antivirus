#include "Context.hpp"
#include "Scaner.hpp"
#include "namecontext/RefType.hpp"
#include "namecontext/BasicType.hpp"
#include "namecontext/RootNameContext.hpp"
#include "BasicTypeNode.hpp"

#include "mapping/Collector.hpp"
#include "mapping/Metadata.hpp"

#include "mapping/tools.hpp"

#include "Generator.hpp"
#include "Dispatcher.hpp"
#include "MappingTools.hpp"

#include <boost/format.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/tokenizer.hpp>

#include <errno.h>
#include <direct.h>
#include <windows.h>

#include <iostream>
#include <fstream>
#include <boost/program_options.hpp>

#define __DT__   __DATE__ ## " -- " ## __TIME__
#define __VER__  "0.1"

#if defined(_DEBUG)
# define __REL__ "(debug)"
#elif defined(NDEBUG)
# define __REL__ "(release)"
#else
# define __REL__ ""
#endif

#define __VERSION__ "Prague IDL compiler" __REL__ "; v:" __VER__ "; compiled at:" __DT__
#define __OPT_EXT  "opt"
#define __OPT_FILE "idlc." __OPT_EXT

// cmdline options
#define __HLP "help"
#define __IDL "idl"
#define __OPT "opt"
#define __INC "inc"
#define __OUT "out"
#define __IFC "ifc"
#define __PLG "plg"
#define __IMP "imp"
#define __MKD "mkd"
#define __CHK "chk"
#define __SLT "nvp"

using namespace idl;
using namespace idl::parser;

namespace po = boost::program_options;


#define PRIDL_INC_STR "PRIDL_INC"

// ---
static int  _idl_main( int argc, char* argv[] );
static bool _generate( const std::string& file, const std::string& data, bool md );
static bool _generate( const std::string& file, const std::vector<char>& data, bool md );
static bool _create_dir( const std::string& path );
static int  _printout_options( const po::options_description& dsc, const po::variables_map& vm, const std::string& cfg_name );
static bool _check_env( const po::options_description& dsc, po::variables_map& vm );
static bool _parse_cfg_file( const std::ifstream& ifs, const po::options_description& dsc, po::variables_map& vm );



namespace idl {

using parser::AttributeNode;
using parser::BasicTypeNode;
using boost::dynamic_pointer_cast;
using namespace boost::io;

std::string makeInterfaceSourceFile(const std::string idlFile) {
	return changeExt(idlFile, ".cpp");
}

using boost::to_upper_copy;
using boost::to_lower_copy;
using boost::format;

} //namespace idl




typedef std::vector<std::string> IncPaths;

// ---
static int _idl_main( int argc, char* argv[] ) {

	//std::string		interfaceHeaderFile;
	//std::string		interfaceSourceFile;

	bool        disableDefaultImports = false;
	bool        make_dir = false;
	std::string	idlFile, idlPath, def_out, if_out, pl_out, src_out, cfg_name;
	IncPaths    incPaths;
	
	try {

		po::typed_value<IncPaths>* incVal = po::value<IncPaths>( &incPaths );
		incVal->composing();

		// Declare the supported options.
		po::options_description dsc(
			"\nIt uses:\n"
			"  - command line options\n"
			"  - environment variable " PRIDL_INC_STR " for include paths (separated by \';\')\n"
			"  - options indicated in " __OPT_EXT "-file in format \"--swt = value\" (one per line)\n"
			"    " __OPT_EXT  "-file may be one of:\n"
			"      1. file pointed by command line option \"--" __OPT "\"\n"
			"      2. file with the same name as compiled one and extension \"." __OPT_EXT "\"\n"
			"      3. file situated in folder of the compiled file named \"" __OPT_FILE "\"\n"
			"      4. file situated in current folder named \"" __OPT_FILE "\"\n\n"
			"Allowed options"
		);
		dsc.add_options()
			( __HLP ",?",                                    "produce this help message")
			( __IDL ",s",  po::value<std::string>(&idlFile), "idl input file" )
			( __OPT ",@",  po::value<std::string>(&cfg_name),"options file" )
			( __INC ",I",  incVal,                           "add include path" )
			( __OUT ",o",  po::value<std::string>(&def_out), "common output directory (default = idl file path)" )
			( __IFC ",f",  po::value<std::string>(&if_out),  "iface header output directory (default = --" __OUT ")" )
			( __PLG ",p",  po::value<std::string>(&pl_out),  "plugin header output directory (default = --" __OUT ")" )
			( __IMP ",d",  po::value<std::string>(&src_out), "source file output directory (default = --" __OUT ")" )
			( __MKD ",m",                                    "make output directory if applicable (default = false)" )
			( __CHK ",c",                                    "just printout cumulative options" )
			( __SLT ",n",                                    "no version print" )
			//("no-default-import",   "disable default forced imports")
			//("interface-header",	po::value<std::string>(&interfaceHeaderFile), 	"set interface header output file")
			//("interface-source",	po::value<std::string>(&interfaceSourceFile), 	"set interface source output file")
		;

		po::positional_options_description p;
		p.add( __IDL, -1 );

		po::variables_map         vm;
		po::command_line_parser   cmd0( argc, argv );
		po::command_line_parser&  cmd0_1 = cmd0.options( dsc );
		po::command_line_parser&  cmd0_2 = cmd0_1.positional( p );

		bool not_parsed = false;
		try {
			po::store( cmd0_2.run(), vm );
			po::notify( vm );
		}
		catch( po::unknown_option& ) {
			not_parsed = true;
		}

		std::ifstream cfg_strm;
		if ( !cfg_name.empty() ) { // Load the option file and from cmd line
			cfg_strm.open( cfg_name.c_str() );
			if ( !cfg_strm ) {
				not_parsed = true;
				std::cout << "Could not open the " __OPT_EXT "-file: \"" << cfg_name << "\"" << std::endl;
			}
		}
		else {
			if ( vm.count(__IDL) ) {
				idlFile = vm[__IDL].as<std::string>();
				size_t slash_pos = idlFile.find_last_of( "\\/" );
				size_t dot_pos = idlFile.find_last_of( '.' );
				if ( slash_pos == std::string::npos )
					slash_pos	= 0;
				else
					idlPath = idlFile.substr( 0, slash_pos+1 );
				if ( (dot_pos == std::string::npos) || (dot_pos < slash_pos) )
					dot_pos = idlFile.length();
				cfg_name = idlFile.substr( 0, dot_pos ) + "." __OPT_EXT;
				cfg_strm.open( cfg_name.c_str() );
				if ( !cfg_strm ) {
					cfg_name = idlPath + __OPT_FILE;
					cfg_strm.clear();
					cfg_strm.open( cfg_name.c_str() );
				}
			}
			if ( !cfg_strm && (cfg_name != __OPT_FILE) ) {
				cfg_name = __OPT_FILE;
				cfg_strm.clear();
				cfg_strm.open( cfg_name.c_str() );
			}
		}
		if ( cfg_strm ) {
			if ( !_parse_cfg_file(cfg_strm,dsc,vm) ) {
				not_parsed = true;
				std::cout << "Could not parse the " __OPT_EXT "-file: \"" << cfg_name << "\"" << std::endl;
			}
			cfg_strm.close();
		}
		else
			cfg_name.clear();

		if ( vm.count(__HLP) ) {
			std::cout << dsc << std::endl;
			return 1;
		}

		_check_env( dsc, vm );

		IncPaths::iterator& end = incPaths.end();
		for( IncPaths::iterator& curr = incPaths.begin(); curr != end; ++curr ) {
			std::string& path = *curr;
			scaner::check_final_slash( path );
		}
		
		if ( !vm.count(__SLT) )
			std::cout << __VERSION__ << std::endl;

		if ( vm.count(__CHK) )
			return _printout_options( dsc, vm, cfg_name );

		if ( not_parsed || (1 != vm.count(__IDL)) ) {
			std::cout << dsc << std::endl;
			_printout_options( dsc, vm, cfg_name );
			return 1;
		}

		if ( idlFile.empty() ) {
			idlFile = vm[__IDL].as<std::string>();
			size_t pos = idlFile.find_last_of( "\\/" );
			if ( pos != std::string::npos )
				idlPath = idlFile.substr( pos+1 );
		}

		disableDefaultImports = !!vm.count("no-default-import");
		make_dir              = !!vm.count( __MKD );
				
		/*if (!vm.count("interface-header")) {
			interfaceHeaderFile = makeInterfaceHeaderFile(idlFile);
		}
		
		if (!vm.count("interface-source")) {
			interfaceSourceFile = makeInterfaceSourceFile(idlFile);
		}*/
		
	} catch( const std::exception& ex ) {
		std::cerr << "error: " << ex.what() << std::endl;
		return 1;
	}

	if ( def_out.empty() )
		def_out = idlPath;
	if ( if_out.empty() )
		if_out = def_out;
	if ( pl_out.empty() )
		pl_out = def_out;
	if ( src_out.empty() )
		src_out = def_out;
	scaner::check_final_slash( def_out );
	scaner::check_final_slash( if_out );
	scaner::check_final_slash( pl_out );
	scaner::check_final_slash( src_out );

	bool result = true;
	if ( result ) {
		Metadata metadata;
		std::cout << "--: " << idlFile << std::endl;
		result = compileIDL( idlFile, incPaths, disableDefaultImports, metadata );
		if ( result )	{
			std::vector<char> data;
			metadata.dump( data );
			if ( !data.empty() )
				result &= _generate( src_out + changeExt(basename(idlFile), ".metadata"), data, make_dir );
		}
	}
		
	if ( result ) {
		Generator generator;
		Dispatcher dispatcher( generator );
		
		dispatcher.start();
		result = compileIDL( idlFile, incPaths, disableDefaultImports, dispatcher );
		dispatcher.stop();
		
		if (result)	{
			generator.gen( changeExt(basename(idlFile),"") );

			const OutputContexts& contexts = generator.getOutputContexts();
			OutputContexts::const_iterator i = contexts.begin();
			for( ; i != contexts.end(); ++i ) {
				const OutputContext& context = *i;
				std::string path;
				switch( context._type ) {
					case OutputContext::iface  : path = if_out + context._file;  break;
					case OutputContext::plugin : path = pl_out + context._file;  break;
					case OutputContext::src    : path = src_out + context._file; break;
					default                    : path = def_out + context._file; break;
				}
				result &= _generate( path, context._data, make_dir );
			}
		}
	}
	return result ? 0 : 1;
}


// ---
static bool _generate( const std::string& file, const std::string& data, bool md ) {
	std::ofstream stream( file.c_str() );
	if ( stream.fail() && md && _create_dir(file) )
		stream.open( file.c_str() );
	if ( stream.fail() ) {
		std::cerr << "error: failed to open file " << file << std::endl;
		return false;
	}
	stream << data << std::endl;
	if (stream.fail()) {
		std::cerr << "error: failed to write file " << file << std::endl;
		return false;
	}
	return true;
}



// ---
static bool _generate( const std::string& file, const std::vector<char>& data, bool md ) {
	std::fstream stream;
	stream.open( file.c_str(), std::ios::out | std::ios::binary );
	if ( stream.fail() && md && _create_dir(file) )
		stream.open( file.c_str(), std::ios::out | std::ios::binary );
	if ( stream.fail() ) {
		std::cerr << "error: failed to open file " << file << std::endl;
		return false;
	}
	stream.write(&data[0], sizeof(data[0])*data.size());
	if ( stream.fail() ) {
		std::cerr << "error: failed to write file " << file << std::endl;
		return false;
	}
	return true;
}


static bool _create_dir( const std::string& path ) {
	bool would_create = false;
	int pos = path.find_first_of( "\\/" );
	while( pos != std::string::npos ) {
		would_create = true;
		std::string sect = path.substr( 0, ++pos );
		BOOL created = ::CreateDirectory( sect.c_str(), 0 );
		if ( !created ) {
			DWORD err = ::GetLastError();
			if ( err == ERROR_ACCESS_DENIED ) {
				int l = sect.length();
				if ( !(((l == 3) && (':' == sect.at(1))) || ((l == 1) && (pos == 1))) )
					return false;
			}
			else if ( err != ERROR_ALREADY_EXISTS )
				return false;
		}
		pos = path.find_first_of( "\\/", pos );
	}
	return would_create;
}



// ---
static int _printout_options( const po::options_description& dsc, const po::variables_map& vm, const std::string& cfg_name ) {
	
	std::cout << "cumulative options (include " __OPT_EXT "-file & environment " PRIDL_INC_STR "): " << std::endl;

	typedef boost::shared_ptr<po::option_description> option_p;
	typedef std::vector< option_p > options_t;
	const options_t& opts = dsc.options();
	options_t::const_iterator it = opts.begin();
	for( ; it != opts.end(); ++it ) {
		option_p opt = *it;
		std::string str;
		const std::string& key = opt->long_name();
		if ( key == __OPT )
			str = cfg_name;
		else if ( vm.count(key) ) {
			const po::variable_value& vv = vm[key];
			if ( key == __INC ) {
				const IncPaths& arr = vv.as<IncPaths>();
				for( IncPaths::const_iterator& i = arr.begin(); i != arr.end(); ++i ) {
					str += *i;
					str += ";";
				}
			}
			else if ( key == __CHK )
				str = "true";
			else if ( key == __MKD )
				str = "true";
			else if ( key == __SLT )
				str = "true";
			else
				str = vm[key].as<std::string>();
		}
		if ( key != __HLP )
			std::cout << "  --" << key << " = " << str << std::endl;
	}
	std::cout << std::endl;
	return 0;
}


// ---
static std::string _trim( std::string& s ) {
	int c = 0;
	int l = s.length();

	while( l>0 && (s[0] == '\"') && (s[l-1] == '\"') ) {
		s.erase( l-1, 1 );
		s.erase( 0, 1 );
		l -= 2;
	}

	for( int i=l-1; 0<i; --i ) {
		if ( (s[i] != ' ') && (s[i] != '\t') )
			break;
		++c;
	}
	if ( c ) {
		l -= c;
		s.erase( l, c );
	}
	for( c=0; (s[c] == ' ') || (s[c] == '\t'); ++c )
		;
	if ( c ) {
		l -= c;
		s.erase( 0, c );
	}

	while( l>0 && (s[0] == '\"') && (s[l-1] == '\"') ) {
		s.erase( l-1, 1 );
		s.erase( 0, 1 );
		l -= 2;
	}

	return s;
}





// ---
static bool _check_env( const po::options_description& dsc, po::variables_map& vm ) {
	char buff[0x1000];
	DWORD size = ::GetEnvironmentVariable( PRIDL_INC_STR, buff, sizeof(buff) );
	if ( size ) {
		boost::char_separator<char> sep(";");
		typedef boost::tokenizer<boost::char_separator<char> > tokenizer;
		std::string cont(buff);
		tokenizer tok( cont, sep );

		std::vector<std::string> paths;
		tokenizer::iterator beg = tok.begin();
		tokenizer::iterator end = tok.end();
		copy( beg, end, back_inserter(paths) );

		std::vector<std::string> args;
		for( size_t i = 0; i < paths.size(); ++i ) {
			const std::string& val = paths[i];
			if ( !val.empty() ) {
				args.push_back( "--" __INC );
				args.push_back( val );
			}
		}

		bool parsed = true;
		if ( args.size() ) {
			// Parse the file and store the options
			po::command_line_parser  cmd2( args );
			po::command_line_parser& cmd2_1 = cmd2.options( dsc );

			try {
				po::store( cmd2_1.run(), vm );
				po::notify( vm );
			}
			catch( po::unknown_option& ) {
				parsed = false;
			}
		}
		return parsed;
	}
	return true;
}




// ---
static bool _parse_cfg_file( const std::ifstream& ifs, const po::options_description& dsc, po::variables_map& vm ) {

	// Read the whole file into a string
	std::stringstream ss;
	ss << ifs.rdbuf();

	// Split the file content
	boost::char_separator<char> sep("\n\r");
	typedef boost::tokenizer<boost::char_separator<char> > tokenizer;
	std::string cont = ss.str();
	tokenizer tok( cont, sep );

	std::vector<std::string> lines;
	tokenizer::iterator beg = tok.begin();
	tokenizer::iterator end = tok.end();
	copy( beg, end, back_inserter(lines) );

	std::vector<std::string> args;
	for( size_t i = 0; i < lines.size(); ++i ) {
		std::string  key, val;
		std::string& line = lines[i];
		size_t pos = line.find( '=' );
		if ( pos == std::string::npos )
			key = _trim(line);
		else {
			key = _trim(line.substr(0,pos));
			val = _trim(line.substr(pos+1));
		}
		args.push_back( key );
		if ( !val.empty() )
			args.push_back( val );
	}

	// Parse the file and store the options
	po::command_line_parser  cmd2( args );
	po::command_line_parser& cmd2_1 = cmd2.options( dsc );

	bool parsed = true;
	try {
		po::store( cmd2_1.run(), vm );
		po::notify( vm );
	}
	catch( po::unknown_option& ) {
		parsed = false;
	}
	return parsed;
}



extern void CreateMiniDump( EXCEPTION_POINTERS* pep ); 
extern void saveSEH();
extern void restoreSEH();

int main( int argc, char* argv[] ) {
	saveSEH();
	__try {
		return _idl_main(argc, argv); 
	} 
	__except( ::CreateMiniDump(GetExceptionInformation()), EXCEPTION_EXECUTE_HANDLER ) {
	}

	return 1; 
}


