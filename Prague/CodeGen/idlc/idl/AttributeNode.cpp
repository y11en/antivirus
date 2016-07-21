#include "Context.hpp"
#include "AttributeNode.hpp"
#include "Lexeme.hpp"
#include "IntNode.hpp"
#include "namecontext/NameContext.hpp"

namespace idl { namespace parser {

AttributeNode::AttributeNode(const Context& context, AttributeId id, const idl::scaner::String& value) : NodeBase<AttributeNode>(context), _stringValue(value), _id(id) {
	ensureType(TypeString);
}

AttributeNode::AttributeNode(const Context& context, AttributeId id, const idl::scaner::Int& value) : NodeBase<AttributeNode>(context), _intValue(value), _id(id) {
	ensureType(TypeInt);
}

AttributeNode::AttributeNode(const Context& context, AttributeId id, Tuple::Ptr value) : NodeBase<AttributeNode>(context), _tupleValue(value), _id(id) {
	ensureType(TypeTuple);
}

AttributeNode::AttributeId AttributeNode::getId() const {
	return _id;
}

const idl::scaner::String& AttributeNode::getString() const {
	return _stringValue;
}

const idl::scaner::Int& AttributeNode::getInt() const {
	return _intValue;
}

Tuple::Ptr AttributeNode::getTuple() const {
	return _tupleValue;
}

void AttributeNode::ensureType(TypeId /*typeId*/) {
	// todo: check type here
}

}


using boost::dynamic_pointer_cast;
using scaner::Lexeme;
using namespace parser;

bool AccessTools::has(parser::AttributeNode::Ptr attr, int right) {
	ASSERT(attr);
	ASSERT(right == Lexeme::Ids::Read || right == Lexeme::Ids::Write || right == Lexeme::Ids::Write_On_Init);
	
	Tuple::Ptr tuple = attr->getTuple();
	for (size_t i = 0; i < tuple->getSize(); ++i) {
		ASSERT(tuple->getNode(i));
		IntNode::Ptr intNode = dynamic_pointer_cast<IntNode>(tuple->getNode(i));
		ASSERT(intNode);
		if (intNode->getValue() == right) {
			return true;
		}
	}
	return false;
}

bool AccessTools::hasRead(parser::AttributeNode::Ptr attr) {
	ASSERT(attr);
	return has(attr, Lexeme::Ids::Read);
}

bool AccessTools::hasWrite(parser::AttributeNode::Ptr attr) {
	ASSERT(attr);
	return AccessTools::has(attr, Lexeme::Ids::Write);
}

bool AccessTools::hasWriteOnInit(parser::AttributeNode::Ptr attr) {
	ASSERT(attr);
	return has(attr, Lexeme::Ids::Write_On_Init);
}

bool AccessTools::isReadOnly(parser::AttributeNode::Ptr attr) {
	ASSERT(attr);
	return hasRead(attr) && !hasWrite(attr) && !hasWriteOnInit(attr);
}

bool AccessTools::isWriteOnly(parser::AttributeNode::Ptr attr) {
	ASSERT(attr);
	return !hasRead(attr) && (hasWrite(attr) || hasWriteOnInit(attr));
}

bool AccessTools::isReadWrite(parser::AttributeNode::Ptr attr) {
	ASSERT(attr);
	return hasRead(attr) && (hasWrite(attr) || hasWriteOnInit(attr));
}

bool AccessTools::isEmpty(parser::AttributeNode::Ptr attr) {
	ASSERT(attr);
	return attr->getTuple()->getSize() == 0;
}


} //namespace idl::parser
