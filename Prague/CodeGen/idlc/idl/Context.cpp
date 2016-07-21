#include "Context.hpp"

#include "ASSERT.hpp"
#include <boost/format.hpp>

namespace idl { namespace parser {

Context::Context() 
		: _firstLine(0), _firstColon(0), _lastLine(0), _lastColon(0) {
}

Context::Context(
		const std::string& source, 
		int firstLine, 
		int firstColon, 
		int lastLine, 
		int lastColon,
		const std::string& text) 
				: _source(source), _firstLine(firstLine), _firstColon(firstColon), _lastLine(lastLine), _lastColon(lastColon), _text(text) {
	ASSERT(_firstLine <= _lastLine);
	ASSERT(_firstLine < _lastLine || (_firstLine == _lastLine && _firstColon <= _lastColon));
}
		
int	Context::getFirstLine() const {
	return _firstLine;
}

int	Context::getFirstColon() const {
	return _firstColon;
}

int	Context::getLastLine() const {
	return _lastLine;
}

int	Context::getLastColon() const {
	return _lastColon;
}

/*template <class T>
void join(Context& result, T first, T last) {
	std::string text	= first->getText();
	std::string source	= first->getSource();
	
	int	firstLine 		= first->getFirstLine();
	int	firstColon 		= first->getFirstColon();
	int	lastLine 		= last->getLastLine();
	int	lastColon 		= last->getLastColon();
	
	for (; first != last; ++first) {
		if (!first->getText().empty()) {
			text += ' ';
			text += first->getText();
		}
	}
	
	result = Context(source, firstLine, firstColon, lastLine, lastColon, text);
}*/

const std::string& Context::getSource() const {
	return _source;
}

const std::string& Context::getText() const {
	return _text;
}

std::string Context::fullContextInfo() const {
	std::string lineInfo;
	
	if (getFirstLine() == getLastLine()) {
		lineInfo = (boost::format("%1%.%2%-%3%") % getFirstLine() % getFirstColon() % getLastColon()).str();
	} else {
		lineInfo = (boost::format("%1%.%2%-%3%.%4%") % getFirstLine() % getFirstColon() % getLastLine() % getLastColon()).str();
	}

	return (boost::format("[%1%: %2% `%3%']") % getSource() % lineInfo % getText()).str();
}

std::string	Context::shortContextInfo() const {
	std::string lineInfo;
	
	if (getFirstLine() == getLastLine()) {
		lineInfo = (boost::format("%1%.%2%-%3%") % getFirstLine() % getFirstColon() % getLastColon()).str();
	} else {
		lineInfo = (boost::format("%1%.%2%-%3%.%4%") % getFirstLine() % getFirstColon() % getLastLine() % getLastColon()).str();
	}

	return (boost::format("%1%(%2%)") % getSource() % lineInfo).str();
}

std::string Context::mediumContextInfo() const {
	std::string lineInfo;

	std::string text = getText();
	
	const size_t MaxInfoLength = 16;
	if (text.length() > MaxInfoLength) {
		text = text.substr(0, MaxInfoLength) + "...";
	}
		
	if (getFirstLine() == getLastLine()) {
		lineInfo = (boost::format("%1%.%2%-%3%") % getFirstLine() % getFirstColon() % getLastColon()).str();
	} else {
		lineInfo = (boost::format("%1%.%2%-%3%.%4%") % getFirstLine() % getFirstColon() % getLastLine() % getLastColon()).str();
	}

	return (boost::format("[%1%: %2%] `%3%'") % getSource() % lineInfo % text).str();
}

std::ostream& operator << (std::ostream& stream, const Context& context) {
	stream << context.mediumContextInfo();
	return stream;
}

}} //namespace idl::parser
