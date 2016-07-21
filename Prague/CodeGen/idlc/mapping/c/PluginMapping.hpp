#ifndef mapping_PluginMapping_hpp
#define mapping_PluginMapping_hpp

#include "Redirector.hpp"

namespace idl {

class PluginMapping : public Redirector {
  public:
  	PluginMapping(Generator& generator);

	Component::Ptr getPluginContext() const;
  	  
	virtual void externMethodDecl(const Attributes& attrs, ExternMethod::Ptr context);
	virtual void componentOpen(const Attributes& attrs, Component::Ptr context);
	virtual void componentClose(const Attributes& attrs, Component::Ptr context);
	
  private:
	//std::string genPluginFileName(const Attributes& /*attrs*/, Component::Ptr context);
	
  	typedef std::vector<std::pair<Attributes, ExternMethod::Ptr> > ExternMethods;
    ExternMethods				_externMethods;
	Component::Ptr				_pluginContext;
};

} //namespace idl

#endif //mapping_PluginMapping_hpp
