#ifndef mapping_CommonMapping_hpp
#define mapping_CommonMapping_hpp

#include "Context.hpp"
#include "mapping/Collector.hpp"

#include "Redirector.hpp"

#include <vector>

#define PRAGUE_STD_HEADERS                 \
				"#include <Prague/prague.h>\n"     \
				"#include <Prague/pr_vtbl.h>\n"    \
				"#include <Prague/pr_idl.h>\n"

namespace idl {

class CommonMapping : public Redirector {
  public:
  	CommonMapping(Generator& generator);

  public:
	virtual std::string fullNameSimple(NameContext::Ptr context) const = 0;
	virtual std::string shortNameSimple(NameContext::Ptr context) const = 0;
	virtual std::string fullPropNameSimple(NameContext::Ptr context) const = 0;
	
	virtual std::string fullName(NameContext::Ptr context) const;
	virtual std::string shortName(NameContext::Ptr context) const;
	
	static std::string cName(const NamePath& path);
	static std::string cName(NameContext::Ptr context);
	static std::string macroName(NameContext::Ptr context);
	static std::string methodCName(Interface::Ptr interface, Method::Ptr method);
	static std::string methodCName(Interface::Ptr interface, const std::string& method);
  	
  public:
	//virtual void constDecl(const Attributes& /*attrs*/, ConstValue::Ptr context);
	virtual void structDecl(const Attributes& /*attrs*/, Struct::Ptr context);
	virtual void enumDecl(const Attributes& /*attrs*/, Enum::Ptr context);
	
  private:
	std::string name(NameContext::Ptr context, std::string (CommonMapping::*functor)(NameContext::Ptr context) const) const;
};

class CommonMappingC : public CommonMapping {
  public:
  	CommonMappingC(Generator& generator);

  public:
  	using CommonMapping::fullName;
  	using CommonMapping::shortName;
  	
	virtual std::string fullNameSimple(NameContext::Ptr context) const;
	virtual std::string shortNameSimple(NameContext::Ptr context) const;
	virtual std::string fullPropNameSimple(NameContext::Ptr context) const { return fullNameSimple(context); }
	virtual void forwardIFace( NameContext::Ptr context );
	virtual void forwardStruct( NameContext::Ptr context );
	virtual void typecast( NameContext::Ptr context );
	virtual void typedefDecl(const Attributes& /*attrs*/, Typedef::Ptr context);
};

class CommonMappingCpp : public CommonMapping {
  public:
  	CommonMappingCpp(Generator& generator);

  public:
  	using CommonMapping::fullName;
  	using CommonMapping::shortName;
  	
	virtual std::string fullNameSimple(NameContext::Ptr context) const;
	virtual std::string shortNameSimple(NameContext::Ptr context) const;
	virtual std::string fullPropNameSimple(NameContext::Ptr context) const;
	virtual void forwardIFace( NameContext::Ptr context );
	virtual void forwardStruct( NameContext::Ptr context );
	virtual void typecast( NameContext::Ptr context );
	virtual void typedefDecl(const Attributes& /*attrs*/, Typedef::Ptr context);
};

} //namespace idl

#endif //mapping_CommonMapping_hpp
