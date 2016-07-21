#include "idl/Context.hpp"
#include "CheckAttrs.hpp"

#include "BasicType.hpp"
#include "RootNameContext.hpp"
#include "idl/Keywords.hpp"
#include "idl/Lexeme.hpp"
#include "idl/IntNode.hpp"
#include "idl/AttributeNode.hpp"
#include "idl/ErrorFactory.hpp"
#include "idl/NameNode.hpp"
#include "idl/OperationNode.hpp"

#include "Namespace.hpp"

#include "traits.hpp"

#include <stack>
#include <vector>
#include <boost/intrusive_ptr.hpp>
#include <boost/format.hpp>

#pragma warning(push)
#pragma warning(disable:4245; disable:4244)
#include <boost/crc.hpp>
#pragma warning(pop)

#include <boost/algorithm/string.hpp>

namespace idl {

using boost::dynamic_pointer_cast;

using namespace parser;
using scaner::Lexeme;

/*static scaner::Int getHash(const NamePath& path) {
	std::string name = path.str();
	
	boost::crc_32_type crc;
	
	crc.process_bytes(&name[0], name.size());
	
	return static_cast<scaner::Int>(crc.checksum());
}*/

template <int bits>
static scaner::Int getCRC(const NamePath& path) {
	std::string name = path.str();
	return static_cast<scaner::Int> (boost::crc<bits, 0x1021, 0xFFFF, 0, false, false>(&name[0], name.size()));
}

struct AttrTools {
	template <Operations::Op op>
	static const AttributeNode::AttributeId* allowed(parser::OperationNode<op>*);
};

#define ATTR_ALLOWED_BEGIN(op)                                                                      	\
	template<>                                                                                          \
	const AttributeNode::AttributeId* AttrTools::allowed<parser::Operations::op>(parser::OperationNode<parser::Operations::op>*) {	\
		static AttributeNode::AttributeId _allowed[] = {
		
#define ATTR_ALLOWED_END                                                                      			\
		};                                                                                              \
		return _allowed;                                                                                \
	};

ATTR_ALLOWED_BEGIN(Interface)
		AttributeNode::Comment,
		AttributeNode::Comment_Ex,
		AttributeNode::Id,
		AttributeNode::Version,
		AttributeNode::Mnemonic,
		AttributeNode::C_Type_Name,
		AttributeNode::Cpp_Type_Name,
		AttributeNode::Output_Header_File,
		AttributeNode::Output_Source_File,
		AttributeNode::NotAnAttr,
ATTR_ALLOWED_END

ATTR_ALLOWED_BEGIN(Component)
		AttributeNode::Comment,
		AttributeNode::Comment_Ex,
		AttributeNode::Id,
		AttributeNode::Version,
		AttributeNode::Mnemonic,
		AttributeNode::Autostart,
		AttributeNode::Codepage,
		AttributeNode::Include_Files,
		AttributeNode::NotAnAttr,
ATTR_ALLOWED_END

ATTR_ALLOWED_BEGIN(Implementation)
		AttributeNode::Comment,
		AttributeNode::Comment_Ex,
		AttributeNode::Id,
		AttributeNode::Version,
		AttributeNode::Mnemonic,
		AttributeNode::C_Type_Name,
		AttributeNode::Cpp_Type_Name,
		AttributeNode::Output_Header_File,
		AttributeNode::Include_Files,
		AttributeNode::Output_Source_File,
		AttributeNode::UnknownPropGetter,
		AttributeNode::UnknownPropSetter,
		AttributeNode::NotAnAttr,
ATTR_ALLOWED_END

ATTR_ALLOWED_BEGIN(Property)
		AttributeNode::Comment,
		AttributeNode::Comment_Ex,
		AttributeNode::Id,
		AttributeNode::NotAnAttr,
ATTR_ALLOWED_END

ATTR_ALLOWED_BEGIN(PropertyPlugin)
		AttributeNode::Comment,
		AttributeNode::Comment_Ex,
		AttributeNode::Shared,
		AttributeNode::Access,
		AttributeNode::Getter,
		AttributeNode::Setter,
		AttributeNode::Member_Ref,
		AttributeNode::NotAnAttr,
ATTR_ALLOWED_END

ATTR_ALLOWED_BEGIN(PropertyPluginAdd)
		AttributeNode::Comment,
		AttributeNode::Comment_Ex,
		AttributeNode::Id,
		AttributeNode::Shared,
		AttributeNode::Access,
		AttributeNode::Getter,
		AttributeNode::Setter,
		AttributeNode::Member_Ref,
		AttributeNode::NotAnAttr,
ATTR_ALLOWED_END

ATTR_ALLOWED_BEGIN(ErrorDef)
		AttributeNode::Comment,
		AttributeNode::Comment_Ex,
		AttributeNode::Warning,
		AttributeNode::NotAnAttr,
ATTR_ALLOWED_END

ATTR_ALLOWED_BEGIN(Method)
		AttributeNode::Comment,
		AttributeNode::Comment_Ex,
		AttributeNode::NotAnAttr,
ATTR_ALLOWED_END

ATTR_ALLOWED_BEGIN(Message)
		AttributeNode::Comment,
		AttributeNode::Comment_Ex,
		AttributeNode::Id,
		AttributeNode::NotAnAttr,
ATTR_ALLOWED_END

ATTR_ALLOWED_BEGIN(MessageClass)
		AttributeNode::Comment,
		AttributeNode::Comment_Ex,
		AttributeNode::Id,
		AttributeNode::NotAnAttr,
ATTR_ALLOWED_END

ATTR_ALLOWED_BEGIN(MessageClassExtend)
		AttributeNode::Comment,
		AttributeNode::Comment_Ex,
		AttributeNode::NotAnAttr,
ATTR_ALLOWED_END

ATTR_ALLOWED_BEGIN(SysMethod)
		AttributeNode::Comment,
		AttributeNode::Comment_Ex,
		AttributeNode::NotAnAttr,
ATTR_ALLOWED_END

ATTR_ALLOWED_BEGIN(ExternMethod)
		AttributeNode::Comment,
		AttributeNode::Comment_Ex,
		AttributeNode::NotAnAttr,
ATTR_ALLOWED_END

ATTR_ALLOWED_BEGIN(Enum)
		AttributeNode::Comment,
		AttributeNode::Comment_Ex,
		AttributeNode::NotAnAttr,
ATTR_ALLOWED_END

ATTR_ALLOWED_BEGIN(Struct)
		AttributeNode::Comment,
		AttributeNode::Comment_Ex,
		AttributeNode::NotAnAttr,
ATTR_ALLOWED_END

ATTR_ALLOWED_BEGIN(Typedef)
		AttributeNode::Comment,
		AttributeNode::Comment_Ex,
		AttributeNode::NotAnAttr,
ATTR_ALLOWED_END

ATTR_ALLOWED_BEGIN(Const)
		AttributeNode::Comment,
		AttributeNode::Comment_Ex,
		AttributeNode::NotAnAttr,
ATTR_ALLOWED_END


using namespace parser;

using boost::dynamic_pointer_cast;

// collect all declarations
class AttrsChecker : public Visitor {
	RootNameContext::Ptr	_root;

	enum IdBits {
		InterfaceBits		=	16,
		PluginBits			=	16,
		ImplementationBits	=	16,
		PropertyBits		=	20,
		MessageBits			=	32,
		MessageClassBits	=	32,
		ExternMethodBits	=	32,
	};
	

  public:
	AttrsChecker(RootNameContext::Ptr root) : _root(root) {
		ASSERT(_root);
	}

	virtual ~AttrsChecker() {
	}
		
	RootNameContext::Ptr getRootNameContext() const {
		return _root;
	}

	static const Name& name(AttributeNode::Ptr attr) {
		return scaner::Keywords::getStr(static_cast<scaner::Lexeme::Id>(attr->getId()));
	}
	
	static AttributeNode::Ptr findAttr(Tuple::Ptr attrs, AttributeNode::AttributeId attrId) {
		ASSERT(attrs);
		
		for (size_t i = 0; i < attrs->getSize(); ++i) {
			AttributeNode::Ptr attr = dynamic_pointer_cast<AttributeNode>(attrs->getNode(i));
			ASSERT(attr);
			
			if (attr->getId() == attrId) {
				return attr;
			}
		}
		return NULL;
	}
	
	template <Operations::Op op>
	void ensureAllowed(parser::OperationNode<op>* node) {
		Tuple::Ptr attrs = OpTools::attrs(node);

		for (size_t i = 0; i < attrs->getSize(); ++i) {
			AttributeNode::Ptr attr = dynamic_pointer_cast<AttributeNode>(attrs->getNode(i));
			ASSERT(attr);
			
			bool allowed = false;
			const AttributeNode::AttributeId* attrId = AttrTools::allowed(node);
			for ( ; !allowed && (*attrId != AttributeNode::NotAnAttr); ++attrId )
				allowed = (attr->getId() == *attrId);
			if ( !allowed )
				ErrorFactory::instance().attributeNotAllowed(attr->getContext(), name(attr));
		}
	}
	
	template <Operations::Op op>
	void ensureNoDuplicates(parser::OperationNode<op>* node) {
		Tuple::Ptr attrs = OpTools::attrs(node);

		if (attrs->getSize() <= 1) {
			return;
		}
		
		size_t lastAttr = attrs->getSize() - 1;
		ASSERT(lastAttr > 0);
		
		for (size_t i = 0; i < lastAttr; ++i) {
			AttributeNode::Ptr attrI = dynamic_pointer_cast<AttributeNode>(attrs->getNode(i));
			ASSERT(attrI);
			for (size_t j = i + 1; j < attrs->getSize(); ++j) {
				ASSERT(i != j);
				AttributeNode::Ptr attrJ = dynamic_pointer_cast<AttributeNode>(attrs->getNode(j));
				ASSERT(attrJ);
				
				if (attrI->getId() == attrJ->getId()) {
					ErrorFactory::instance().attributeDuplicated(attrJ->getContext(), name(attrJ), attrI->getContext());
				}
			}
		}
		
	}


#define CHECK_ATTRS(op)	                                                                \
	virtual bool visitBefore(parser::OperationNode<parser::Operations::op>* node) {			\
		ensureNoDuplicates(node);                                                          	\
		ensureAllowed(node);                                                                \
		return true;                                                                        \
	}                                                                                       \
	virtual bool visitAfter(parser::OperationNode<parser::Operations::op>* ) {   			\
		return true;                                                                        \
	}

	template <Operations::Op op>
	void ensureCommon(parser::OperationNode<op>* node) {
		ensureNoDuplicates(node);
		ensureAllowed(node);
		ensureComment(OpTools::attrs(node));
		ensureCommentEx(OpTools::attrs(node));
	}
	
	
	template <class T>
	void ensureAttr(Tuple::Ptr attrs, AttributeNode::AttributeId attrId, const T& value) {
		AttributeNode::Ptr attr = findAttr(attrs, attrId);
		if (!attr) {
			// add missing attribute
			attrs->prepend(new AttributeNode(attrs->getContext(), attrId, value));
		}
	}

	
	void ensureMnemonic(Tuple::Ptr attrs, const Name& name) {
		std::string value = (boost::format("IID_%1%") % boost::to_upper_copy(name)).str();
		ensureAttr(attrs, AttributeNode::Mnemonic, value);
	}
	
	void ensureCTypeName(Tuple::Ptr attrs, const Name& name) {
		// c_type_name(hREGIONMGR)
		std::string value = (boost::format("h%1%") % boost::to_upper_copy(name)).str();
		ensureAttr(attrs, AttributeNode::Cpp_Type_Name, value);
	}
	
	void ensureCppTypeName(Tuple::Ptr attrs, const Name& name) {
		// cpp_type_name(cREGIONMGR)
		std::string value = (boost::format("c%1%") % boost::to_upper_copy(name)).str();
		ensureAttr(attrs, AttributeNode::C_Type_Name, value);
	}
	
	template <int bits>
	void ensureId(Tuple::Ptr attrs, const NamePath& name) {
		ensureAttr(attrs, AttributeNode::Id, getCRC<bits>(name));
	}
	
	void ensureVersion(Tuple::Ptr attrs) {
		ensureAttr(attrs, AttributeNode::Version, 1);
	}
	
	void ensureCodepage(Tuple::Ptr attrs) {
		ensureAttr(attrs, AttributeNode::Codepage, "ansi");
	}
	
	void ensureComment(Tuple::Ptr attrs) {
		ensureAttr(attrs, AttributeNode::Comment, "");
	}
	
	void ensureCommentEx(Tuple::Ptr attrs) {
		ensureAttr(attrs, AttributeNode::Comment_Ex, "");
	}
	
	void ensureAutostart(Tuple::Ptr attrs) {
		ensureAttr(attrs, AttributeNode::Autostart, 0);
	}
	
	void ensureWarning(Tuple::Ptr attrs) {
		ensureAttr(attrs, AttributeNode::Warning, 0);
	}
	
	void ensureShared(Tuple::Ptr attrs) {
		ensureAttr(attrs, AttributeNode::Shared, 0);
	}

	void ensureOutputSourceFile(Tuple::Ptr attrs, const Name& name) {
		ensureAttr(attrs, AttributeNode::Output_Source_File, name);
	}
	
	void ensureOutputHeaderFile(Tuple::Ptr attrs, const Name& name) {
		ensureAttr(attrs, AttributeNode::Output_Header_File, name);
	}
	
	void ensureAccess(Tuple::Ptr attrs) {
		Tuple::Ptr tuple = new Tuple(attrs->getContext());
		IntNode::Ptr access = new IntNode(attrs->getContext(), Lexeme::Ids::Read);
		tuple->prepend(access);
		ensureAttr(attrs, AttributeNode::Access, tuple);
	}
	
	virtual bool visitAfter(parser::OperationNode<parser::Operations::Interface>* node) {
		ensureCommon(node);
		
		Tuple::Ptr attrs = OpTools::attrs(node);
		ensureId<InterfaceBits>(attrs, OpTools::context(node)->getNamePath());
		ensureOutputSourceFile(attrs, OpTools::name(node));
		ensureOutputHeaderFile(attrs, OpTools::name(node));
		//ensureVersion(attrs, OpTools::name(node));
		return true;
	}
	
	virtual bool visitAfter(parser::OperationNode<parser::Operations::Component>* node) {
		ensureCommon(node);
		
		Tuple::Ptr attrs = OpTools::attrs(node);
		ensureId<PluginBits>(attrs, OpTools::context(node)->getNamePath());
		ensureCodepage(attrs);
		ensureVersion(attrs);
		ensureAutostart(attrs);
		return true;
	}
	
	virtual bool visitAfter(parser::OperationNode<parser::Operations::Implementation>* node) {
		ensureCommon(node);
		
		Tuple::Ptr attrs = OpTools::attrs(node);
		ensureId<ImplementationBits>(attrs, OpTools::context(node)->getNamePath());
		//ensureSystem(attrs);
		ensureVersion(attrs);
		ensureOutputSourceFile(attrs, OpTools::name(node));
		ensureOutputHeaderFile(attrs, OpTools::name(node));
		return true;
	}
	
	virtual bool visitAfter(parser::OperationNode<parser::Operations::ErrorDef>* node) {
		ensureCommon(node);
		
		Tuple::Ptr attrs = OpTools::attrs(node);
		ensureWarning(attrs);
		return true;
	}
	
	virtual bool visitAfter(parser::OperationNode<parser::Operations::Method>* node) {
		ensureCommon(node);
		
		Tuple::Ptr attrs = OpTools::attrs(node);
		return true;
	}
	
	virtual bool visitAfter(parser::OperationNode<parser::Operations::MessageClass>* node) {
		ensureCommon(node);
		
		Tuple::Ptr attrs = OpTools::attrs(node);
		ensureId<MessageClassBits>(attrs, OpTools::context(node)->getNamePath());
		return true;
	}
	
	virtual bool visitAfter(parser::OperationNode<parser::Operations::MessageClassExtend>* node) {
		ensureCommon(node);
		
		Tuple::Ptr attrs = OpTools::attrs(node);
		//ensureId(attrs, OpTools::context(node)->getNamePath());
		return true;
	}
	
	virtual bool visitAfter(parser::OperationNode<parser::Operations::Message>* node) {
		ensureCommon(node);
		
		Tuple::Ptr attrs = OpTools::attrs(node);
		ensureId<MessageBits>(attrs, OpTools::context(node)->getNamePath());
		return true;
	}
	
	virtual bool visitAfter(parser::OperationNode<parser::Operations::Property>* node) {
		ensureCommon(node);
		
		Tuple::Ptr attrs = OpTools::attrs(node);
		ensureId<PropertyBits>(attrs, OpTools::context(node)->getNamePath());
		return true;
	}

	
	template <Operations::Op op>
	void ensurePropertyPluginAttrs(parser::OperationNode<op>* node, Tuple::Ptr attrs) {
		const Context& context = node->getContext();
		
		ensureShared(attrs);
		ensureAccess(attrs);

		AttributeNode::Ptr accessAttr = findAttr(attrs, AttributeNode::Access);
		AttributeNode::Ptr setterAttr = findAttr(attrs, AttributeNode::Setter);
		AttributeNode::Ptr getterAttr = findAttr(attrs, AttributeNode::Getter);
		AttributeNode::Ptr memberAttr = findAttr(attrs, AttributeNode::Member_Ref);
		ASSERT(accessAttr);

		bool isShared = findAttr(attrs, AttributeNode::Shared)->getInt() != 0;
		
		if (isShared) {
			if (setterAttr || getterAttr || memberAttr) {
				AttributeNode::Ptr attr = (setterAttr ? setterAttr : (getterAttr ? getterAttr : memberAttr));
				
				ErrorFactory::instance().sharedPropertyHasGetterOrSetter(attr->getContext(), context);
			}
		} else {
			// check invalid combination: setter + getter + member_ref
			if (AccessTools::isReadOnly(accessAttr)) {
				if (setterAttr) {
					ErrorFactory::instance().readOnlyPropertyHasSetter(setterAttr->getContext(), context);
				}
				
				if (!getterAttr && !memberAttr) {
					ErrorFactory::instance().readOnlyPropertyHasNoAccessor(context);
				}
				
				if (getterAttr && memberAttr) {
					ErrorFactory::instance().propertyAccessorsOverspecified(context);
				}
			} 
			
			if (AccessTools::isWriteOnly(accessAttr)) {
				if(getterAttr) {
					ErrorFactory::instance().writeOnlyPropertyHasGetter(getterAttr->getContext(), context);
				}
				if(!setterAttr && !memberAttr) {
					ErrorFactory::instance().writeOnlyPropertyHasNoAccessor(context);
				}
				if(setterAttr && memberAttr) {
					ErrorFactory::instance().propertyAccessorsOverspecified(context);
				}
			}
			
			if (AccessTools::isReadWrite(accessAttr)) {
				if (setterAttr && getterAttr && memberAttr) {
					ErrorFactory::instance().propertyAccessorsOverspecified(context);
				} 
			}
					
			if (AccessTools::isEmpty(accessAttr)) {
				ErrorFactory::instance().emptyPropertyAccessList(context);
			}
		}
		
		Property::Ptr propertyContext = OpTools::context(node);
		if (isShared) {
			if (!propertyContext->getPerformer()) {
				ErrorFactory::instance().sharedPropertyHasNoValue(context, propertyContext->getNamePath());
			}
		} else {
			if (propertyContext->getPerformer()) {
				ErrorFactory::instance().localPropertyHasValue(context, propertyContext->getNamePath());
			}
		}
	}

	virtual bool visitAfter(parser::OperationNode<parser::Operations::PropertyPlugin>* node) {
		ensureCommon(node);
		
		Tuple::Ptr attrs = OpTools::attrs(node);
		ensurePropertyPluginAttrs(node, attrs);
		return true;
	}
	
	virtual bool visitAfter(parser::OperationNode<parser::Operations::PropertyPluginAdd>* node) {
		ensureCommon(node);
		
		Tuple::Ptr attrs = OpTools::attrs(node);
		ensureId<PropertyBits>(attrs, OpTools::context(node)->getNamePath());
		ensurePropertyPluginAttrs(node, attrs);
		return true;
	}
	
	virtual bool visitAfter(parser::OperationNode<parser::Operations::SysMethod>* node) {
		ensureCommon(node);
		return true;
	}
	
	virtual bool visitAfter(parser::OperationNode<parser::Operations::ExternMethod>* node) {
		ensureCommon(node);
		ensureId<ExternMethodBits>(OpTools::attrs(node), OpTools::context(node)->getNamePath());
		return true;
	}
	
	virtual bool visitAfter(parser::OperationNode<parser::Operations::Enum>* node) {
		ensureCommon(node);
		return true;
	}
	
	virtual bool visitAfter(parser::OperationNode<parser::Operations::Struct>* node) {
		ensureCommon(node);
		return true;
	}
	
	virtual bool visitAfter(parser::OperationNode<parser::Operations::Typedef>* node) {
		ensureCommon(node);
		return true;
	}
	
	virtual bool visitAfter(parser::OperationNode<parser::Operations::Const>* node) {
		ensureCommon(node);
		return true;
	}
	
};

void checkAttrs(RootNameContext::Ptr rootContext, NodeInterface::Ptr rootNode) {
	ASSERT(rootContext);
	ASSERT(rootNode);
	AttrsChecker checker(rootContext);
	rootNode->visit(checker);
}

} // namespace idl
