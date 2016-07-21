#include "Scaner.hpp"

#include "ErrorFactory.hpp"

#include <fstream>
#include <sstream>
#include <iostream>

#include <boost/algorithm/string.hpp>
#include <boost/filesystem/operations.hpp>

namespace idl { namespace scaner {

std::string& check_final_slash( std::string& path ) {
	if ( !path.empty() ) {
		if ( boost::ends_with(path,"\"") )
			path.resize( path.length()-1 );
		if ( !boost::ends_with(path,"/") && !boost::ends_with(path,"\\") )
			path += '/';
	}
	return path;
}


// ---
std::string _normalize_fpath( std::string path ) {
	#if defined(_WIN32)
		char* spath = _strdup(path.c_str());
		strlwr( spath );
		std::string ret( spath );
		delete [] spath;
		return ret;
	#else
		return path;
	#endif
}



// ---
bool checkPrefix( std::string& str, const char* pfx ) {
	if ( pfx && *pfx && !boost::starts_with(str,pfx) ) {
		str = pfx + str;
		return true;
	}
	return false;
}



// ---
bool replPrefix( std::string& str, const char* new_pfx, const char* old_pfx ) {
	bool changed = false;
	if ( old_pfx && *old_pfx && boost::starts_with(str,old_pfx) ) {
		int len = strlen(old_pfx);
		str = str.substr(len);
		changed = true;
	}
	if ( new_pfx && *new_pfx ) {
		str = new_pfx + str;
		changed = true;
	}
	return changed;
}





Scaner::Scaner() : _currentContext() {
	static struct SetChecker {
		SetChecker() {
			boost::filesystem::path::default_name_check(&boost::filesystem::native);
		}
	} checker;
	_inImport = 0;
}

Scaner::~Scaner() {
	while (!_contextStack.empty()) {
		popContext();
	}
	ASSERT(!isInImport());
}

void Scaner::setIncludePaths(const IncludePaths& paths) {
	_includePaths = paths;
}

bool Scaner::findFile(const std::string& file, /*const std::string& basePath, const IncludePaths& paths,*/ std::string& result) const {
	using namespace boost::filesystem;
	
	std::string bp = _basePath;

	path fullPath;
	//fullPath = complete(file, _basePath);
	fullPath = check_final_slash(bp) + file;

	if (exists(fullPath)) {
		result = _normalize_fpath( fullPath.string() );
		return true;
	}
	
	BOOST_FOREACH(const std::string& includePath, _includePaths) {
		//path searchPath = complete(includePath, _basePath);
		//fullPath = complete(file, searchPath);
		fullPath = includePath + file;
		
		if (exists(fullPath)) {
			result = _normalize_fpath( fullPath.string() );
			return true;
		}
	}
	
	return false;
}

/*void Scaner::setForceInclude(const std::string& file) {
	_forceIncludeFile = file;
}*/

void Scaner::addForceInclude(const std::string& file) {
	_forceIncludeFiles.push_back(file);
}

void Scaner::addForceImport(const std::string& file) {
	_forceImportFiles.push_back(file);
}

static boost::shared_ptr<std::istream> makeForceIncludeStream(const Scaner::ForceFiles& included, const Scaner::ForceFiles& imported) {
	std::string data;
	
	boost::format includeFmt("#include \"%1%\"\n");
	BOOST_FOREACH(const std::string& file, included)  {
		data += (includeFmt % file).str();
	}
	
	boost::format importFmt("#import \"%1%\"\n");
	BOOST_FOREACH(const std::string& file, imported)  {
		data += (importFmt % file).str();
	}

	return boost::shared_ptr<std::istringstream>(new std::istringstream(data));
}

void Scaner::setSource(const std::string& file) {
	ASSERT(_contextStack.empty());
	ASSERT(!isInImport());

	_processedFiles.clear();

	boost::shared_ptr<std::ifstream> stream = boost::shared_ptr<std::ifstream>(new std::ifstream());
	stream->open(file.c_str());
        
	if (stream->fail()) {
		ErrorFactory::instance().failedToOpenFile(file);
    	throw IDL_EXCEPTION("fatal scaner error: failed to open file `%1%' for reading") % file;
	}

	boost::filesystem::path fullPath = boost::filesystem::complete(file);
	_processedFiles.insert(_normalize_fpath(fullPath.string()));
	
	_basePath = fullPath.branch_path().string();
	check_final_slash( _basePath );
	_stream = stream;
	_currentContext = Context(file, 1, 0, 1, 0);
	switch_streams(_stream.get(), 0);

	boost::shared_ptr<std::istream> forceIncludeStream = makeForceIncludeStream(_forceIncludeFiles, _forceImportFiles);

	pushContext("<internal>", _basePath, forceIncludeStream, false);

	//if (!_forceIncludeFile.empty()) {
	//	if (!processInclude(_forceIncludeFile, false)) {
	//	}
	//}
}


void Scaner::setSource(const std::string& name, boost::shared_ptr<std::istream> stream) {
	ASSERT(!isInImport());
	_stream = stream;
	_currentContext = Context(name, 1, 0, 1, 0);
	_basePath = boost::filesystem::initial_path().string();
	check_final_slash( _basePath );
	switch_streams(_stream.get(), 0);
}

const Context& Scaner::getCurrentContext() const {
	return _currentContext;
}


//void Scaner::initContext() {
//	_currentContext = Lexeme::Context("<unknown>", 1, 0);
//}
  
void Scaner::updateContext(const char* text, int size) {
	ASSERT(size > 0);

	static const int tabSize = 4;

	int line 	= _currentContext.getLastLine();
	int colon	= _currentContext.getLastColon();

	int firstLine = line;
	int firstColon = colon + 1;
	
	for (int i = 0; i < size; ++i) {
		switch (text[i]) {
			case '\r':
				break;
			case '\n' :
				line += 1;
				colon = 0;
				break;
			case '\t':
				colon += tabSize;
				break;
			default:
				colon += 1;
				break;
		}
	}

	if (firstLine == line)
		firstColon = std::min(firstColon, colon);
	
	_currentContext = Context(_currentContext.getSource(), firstLine, firstColon, line, colon, text);
	//std::cout << "Lexeme: " << _currentContext << ": " << text << std::endl;
}

bool Scaner::processInclude(const std::string& file, bool imported) {
	return pushContext(file, imported);
}
		
bool Scaner::isInImport() const {
	return _inImport > 0;
}

void Scaner::pushContext(const std::string& file, const std::string& basePath, IStreamPtr stream, bool inImport) {
	_contextStack.push(ScanerContext(yy_current_buffer, _basePath, _stream, getCurrentContext(), inImport));
	
	_basePath = basePath;
	check_final_slash( _basePath );
	_stream = stream;
	std::string ctx_file;
	if ( (file != "<internal>") && (file != "<initial>") && (std::string::npos == file.find('\\')) && (std::string::npos == file.find('/')) && !_basePath.empty() )
		ctx_file = _basePath + file;
	else
		ctx_file = file;
	_currentContext = Context(ctx_file, 1, 0, 1, 0);
	yy_switch_to_buffer(yy_create_buffer(_stream.get(), YY_BUF_SIZE));
}
		
bool Scaner::pushContext(const std::string& file, bool imported) {
	std::string fullPath;
	if (!findFile(file, fullPath)) {
		ErrorFactory::instance().failedToFindIncludeFile(file);
    	throw IDL_EXCEPTION("fatal scaner error: failed to find include file `%1%'") % file;
	}

	if ( _processedFiles.find(fullPath) != _processedFiles.end() ) {
		return false;
	}

	if (imported) {
		++_inImport;
	}
		
	_processedFiles.insert(fullPath);

	 IStreamPtr stream = IStreamPtr(new std::ifstream(fullPath.c_str()));
	//_file = boost::filesystem::/*system_*/complete(file).string();
	
	if (stream->fail()) {
		ErrorFactory::instance().failedToOpenFile(file);
    	throw IDL_EXCEPTION("fatal scaner error: failed to open file `%1%' for reading") % fullPath;
	}

	std::string basePath = boost::filesystem::path(fullPath).branch_path().string();
	pushContext(file, basePath, stream, imported);
	
	//_contextStack.push(ScanerContext(yy_current_buffer, _basePath, _stream, getCurrentContext(), imported));
	//
	//_basePath = boost::filesystem::path(fullPath).branch_path().string();
	//_stream = stream;
	//_currentContext = Context(file, 1, 0, 1, 0);
	//yy_switch_to_buffer(yy_create_buffer(_stream.get(), YY_BUF_SIZE));
	
	return true;
}

void Scaner::popContext() {
	if (_contextStack.top()._imported) {
		--_inImport;
	}
	
	yy_delete_buffer(yy_current_buffer);
	yy_switch_to_buffer(_contextStack.top()._buffer);
	_basePath = _contextStack.top()._basePath;
	_stream = _contextStack.top()._stream;
	_currentContext = _contextStack.top()._context;

	_contextStack.pop();
}

Scaner::ScanerContext::ScanerContext(yy_buffer_state* buffer, const std::string& basePath, IStreamPtr stream, const Context& context, bool imported) 
		: _buffer(buffer), _basePath(basePath), _stream(stream), _context(context), _imported(imported) {
}

const Lexeme& Scaner::getLexeme() const {
	return _lexeme;
}

void Scaner::fatalError(const char* msg, int line) {
	ErrorFactory::instance().fatalScanerError(line, msg);
    throw IDL_EXCEPTION("fatal scaner error at line %1%: %2%") % line % msg;
}

Int fromHexToInt(const std::string& hex) {
	Int result = 0;
	size_t cnt = hex.size();
	if ( cnt ) {
		char ch = hex[cnt-1];
		if ( (ch == 'l') || (ch == 'L') )
			--cnt;
		for ( size_t i=0; (i < cnt); ++i ) {
			ch = hex[i];
			
			result *= 0x10;
			if ( (ch >= '0') && (ch <= '9') )
				result += (ch - '0');
			else if ( (ch >= 'a') && (ch <= 'f') )
				result += 10 + (ch - 'a');
			else if ( (ch >= 'A') && (ch <= 'F') )
				result += 10 + (ch - 'A');
			else
				ASSERT( ("not hex digit!",false) );
		}
	}
	return result;
}

}} //namespace idl::scaner
