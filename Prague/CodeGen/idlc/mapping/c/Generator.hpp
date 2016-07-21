#include <vector>
#include <stack>

#include "mapping/Collector.hpp"

#include "CodeBlock.hpp"

#include <boost/format.hpp>

namespace idl {

struct OutputContext {
	typedef enum ftype { unk, iface, plugin, src } ftype;

	ftype         _type;
	std::string		_file;
	std::string		_data;
	
	//OutputContext() : _type(unk) {}
	OutputContext( ftype type, const std::string& file ) : _type(type), _file(file) {}
};

typedef std::deque<OutputContext>	OutputContexts;

class Generator {
public:
	Generator();
	virtual ~Generator();

	void add( const CodeBlock::Name& name, CodeBlock::Language lang, std::string code );
	void indent( const CodeBlock::Name& name, CodeBlock::Language lang );
	void unindent( const CodeBlock::Name& name, CodeBlock::Language lang );

	const OutputContexts& getOutputContexts() const;

public:
	void gen( const std::string& basename );

private:
	CodeBlock::Ptr block(const CodeBlock::Name& name, CodeBlock::Language lang);
	CodeBlock::Ptr findBlock(const CodeBlock::Name& name, CodeBlock::Language lang) const;
	int  countBlocks(const std::string& pattern, CodeBlock::Language lang) const;
	void matchBlocks(const std::string& pattern, CodeBlock::Language lang, std::vector<std::string>& names) const;
	void consolidate(const std::string& pattern, CodeBlock::Language lang, std::string& data) const;

private:
	typedef std::vector<CodeBlock::Ptr> CodeBlocks;

	CodeBlocks      _codeBlocks;
	OutputContexts  _outputContexts;
};

} //namespace idl

