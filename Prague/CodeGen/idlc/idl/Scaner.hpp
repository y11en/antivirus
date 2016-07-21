#ifndef Scaner_hpp
#define Scaner_hpp

#include "FlexLexer.h"

#include <string>
#include <stack>
#include <vector>
#include <set>
#include <boost/shared_ptr.hpp>

#include "Lexeme.hpp"
#include "Exception.hpp"

#define YY_BUF_SIZE 			(64*1024)

namespace idl { namespace scaner {

std::string& check_final_slash( std::string& path );
std::string  _normalize_fpath( std::string path );
bool         checkPrefix( std::string& str, const char* pfx );
bool         replPrefix( std::string& str, const char* new_pfx, const char* old_pfx );

class Scaner : public yyFlexLexer {
  public:
  	typedef std::vector<std::string>	ForceFiles;
  	typedef std::vector<std::string>	IncludePaths;
  
  	Scaner();
  	~Scaner();

	void addForceInclude(const std::string& file);
	void addForceImport(const std::string& file);
  	  
	void setIncludePaths(const IncludePaths& paths);
	void setSource(const std::string& file);
	void setSource(const std::string& name, boost::shared_ptr<std::istream> stream);
	
    virtual int yylex();
    
    const Lexeme& getLexeme() const;
    
    static void fatalError(const char* msg, int line);
    
  private:
  	typedef boost::shared_ptr<std::istream> IStreamPtr;
  
  private:
	bool findFile(const std::string& file, /*const std::string& basePath, const IncludePaths& paths,*/ std::string& result) const;
  	const Context& getCurrentContext() const;

  	//void initContext();
  	  
    void updateContext(const char* text, int size);

	void pushContext(const std::string& file, const std::string& basePath, IStreamPtr stream, bool inImport);
    bool pushContext(const std::string& file, bool imported);
    void popContext();
        
	bool processInclude(const std::string& file, bool imported);
	bool isInImport() const;
	
  private:
  	struct ScanerContext {
  		yy_buffer_state*	_buffer;
	    std::string			_basePath;
  		IStreamPtr			_stream;
  		Context				_context;
  		bool				_imported;
  		
  		ScanerContext(yy_buffer_state* buffer, const std::string& basePath, IStreamPtr stream, const Context& context, bool imported);
  	};

    typedef std::stack<ScanerContext>	ContextStack;
    typedef std::set<std::string>		ProcessedFiles;

    ForceFiles		_forceIncludeFiles;
    ForceFiles		_forceImportFiles;
        
    ProcessedFiles	_processedFiles;
    
    IncludePaths	_includePaths;
    
    ContextStack	_contextStack;

    std::string		_stringBuffer;
    std::string		_basePath;
  	IStreamPtr		_stream;

  	std::string		_forceIncludeFile;
  	  
  	int				_inImport;
  	
    Lexeme 			_lexeme;
    Context			_currentContext;
};

extern Int fromHexToInt(const std::string& hex);

}} //namespace idl::scaner

#endif //Scaner_hpp
