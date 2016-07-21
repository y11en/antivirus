#include "idl/Context.hpp"
#include "ConstValue.hpp"

namespace idl {

void ConstValue::setPerformer(Performer::Ptr performer) {
	_performer = performer;
	
	/*// clamping to type
	BasicNode::TypeId typeId = _performer->getTypeId();
	
	Int mask = BasicType::getMask(typeId);
	Int value = asInt();
	
	if ((mask & value) == value) {
	}
	*/
}

Performer::Ptr ConstValue::getPerformer() const {
	return _performer;
}

} //namespace idl
