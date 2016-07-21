#ifndef mapping_Redirector_hpp
#define mapping_Redirector_hpp

#include "mapping/CheckedCollector.hpp"

#include <boost/format.hpp>

#include "CodeBlock.hpp"

namespace idl {

class Generator;

class Redirector : public CheckedCollector {
	public:
		Redirector(Generator& generator);

		void setBlock(const CodeBlock::Name& blockName, CodeBlock::Language lang);

	protected:
		void add(const std::string& data);
		void add(const boost::format& fmt);

	public:
		void indent();
		void unindent();

	private:
		CodeBlock::Name		  _blockName;
		CodeBlock::Language _lang;

		Generator& 			    _generator;
};

} // namespace idl

#endif //mapping_Redirector_hpp
