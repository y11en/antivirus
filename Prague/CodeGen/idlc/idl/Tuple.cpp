#include "Context.hpp"
#include "Tuple.hpp"

#include "namecontext/Type.hpp"

#include <algorithm>
#include <boost/lambda/lambda.hpp>
#include <boost/lambda/bind.hpp>

using namespace boost::lambda;

namespace idl { namespace parser {

Tuple::Tuple(const Context& context) : NodeBase<Tuple>(context) {
}

bool Tuple::visitChildern(Visitor& visitor) {
	bool result = true;
	Nodes::reverse_iterator begin 	= _nodes.rbegin();
	Nodes::reverse_iterator end 	= _nodes.rend();
	for (; begin != end; ++begin) {
		NodeInterface* node = (*begin).get();
		if (!node->visit(visitor)) {
			result = false;
			break;
		}
	}
	//std::for_each(_nodes.rbegin(), _nodes.rend(), bind(&NodeInterface::visit, *_1, visitor));
	//std::for_each(_nodes.rbegin(), _nodes.rend(), (*_1)->visit(visitor));
	return result;
}

NodeInterface::Ptr Tuple::transform(Transformer& transformer) {
	return transformTuple(transformer);
}

Tuple::Ptr Tuple::transformTuple(Transformer& transformer) {
	ASSERT(canBeLocallyWrapped());
	
	Tuple::Ptr tuple = new Tuple(getContext());
	bool changed = false;
	for (size_t i = 0; i < _nodes.size(); ++i) {
		NodeInterface::Ptr node = _nodes[i]->transform(transformer);
		changed |= (node != _nodes[i]);
		if (node != NULL)
			tuple->prepend(node);
	}

	return changed ? tuple : this;
}

void Tuple::prepend(NodeInterface::Ptr node) {
	ASSERT(node);
	_nodes.push_back(node);
}

void Tuple::removeNode(size_t i) {
	ASSERT(i < _nodes.size());
	_nodes.erase(_nodes.begin() + i);
}

void Tuple::replaceNode(size_t i, NodeInterface::Ptr node) {
	ASSERT(i < _nodes.size());
	_nodes[i] = node;
}

NodeInterface::Ptr Tuple::getNode(size_t i) {
	ASSERT(i < _nodes.size());
	return _nodes[i];
}

size_t Tuple::getSize() const {
	return _nodes.size();
}

Tuple::Ptr Tuple::replaceNode(const Tuple& tuple, size_t i, NodeInterface::Ptr node) {
	Tuple::Ptr result = new Tuple(tuple);
	result->replaceNode(i, node);
	return result;
}

Tuple::Ptr Tuple::removeNode(const Tuple& tuple, size_t i) {
	Tuple::Ptr result = new Tuple(tuple);
	result->removeNode(i);
	return result;
}

}} //namespace idl::parser

#include <builder.hpp>

idl::parser::Tuple::Ptr make_tuple(idl::parser::Builder& builder, idl::parser::NodeInterface::Ptr v1, idl::parser::NodeInterface::Ptr v2, idl::parser::NodeInterface::Ptr v3, idl::parser::NodeInterface::Ptr v4) {
	idl::parser::Tuple::Ptr tuple = builder.createTuple();
	tuple->prepend(v1);
	tuple->prepend(v2);
	tuple->prepend(v3);
	tuple->prepend(v4);
	return tuple;
}


