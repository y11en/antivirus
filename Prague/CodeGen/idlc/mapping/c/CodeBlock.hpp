#ifndef mapping_CodeBlock_hpp
#define mapping_CodeBlock_hpp

#include <string>
#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>

namespace idl {

class CodeBlock : boost::noncopyable {
public:
	typedef boost::shared_ptr<CodeBlock> Ptr;

	typedef std::string Name;

	enum Language {
		CLang		=	0x01,
		CppLang		=	0x02,
		CommonLang	=	CLang | CppLang,
	};

	CodeBlock(const Name& name, Language lang = CommonLang);

	const Name& name() const;
	Language    lang() const;
	std::string langName() const;

	void add(const std::string& code);
	const std::string&	data() const;

	void indent();
	void unindent();

private:
	Language	_lang;
	Name		_name;
	std::string	_data;
	std::string	_indent;
};


// ---
inline std::string CodeBlock::langName() const {
	switch( _lang ) {
		case CLang      : return "c";
		case CppLang    : return "c++";
		case CommonLang : return "comm";
		default         : return "unk";
	}
}

} //namespace idl


#endif //mapping_CodeBlock_hpp
