#ifndef idl_types_hpp
#define idl_types_hpp

#include <vector>
#include <string>

#include <boost/intrusive_ptr.hpp>
#include <boost/noncopyable.hpp>

#include "foreach.hpp"

#include "assert.hpp"

#define ITERATE(iter, container) for (iter = container.begin(); iter != container.end(); ++iter)

class RefCountBase;

struct null_deleter {
    void operator()(void const *) const {}
};

namespace boost {
    void intrusive_ptr_add_ref(RefCountBase* p);
    void intrusive_ptr_release(RefCountBase* p);
};

class RefCountBase : boost::noncopyable {
  private:
    int	_references;
    friend void ::boost::intrusive_ptr_add_ref(RefCountBase* p);
    friend void ::boost::intrusive_ptr_release(RefCountBase* p);

  protected:
    RefCountBase(const RefCountBase&) : _references(0) {}   // initialize references to 0
    RefCountBase() : _references(0) {}   					// initialize references to 0
    
    virtual ~RefCountBase() {}

  public:
    bool canBeLocallyWrapped() const { return (_references > 0); } // used to prevent from unexpected local destruction
};

// the two function overloads must be in the boost namespace on most compilers:
namespace boost {
	inline void intrusive_ptr_add_ref(RefCountBase* p) {
		// increment reference count of object *p
		++(p->_references);
	}

	inline void intrusive_ptr_release(RefCountBase * p) {
	   // decrement reference count, and delete object when reference count reaches 0
	   if (--(p->_references) == 0)
			delete p;
	} 

} // namespace boost

namespace idl { 

namespace scaner {

typedef std::string 	String;
typedef double			Float;
typedef long long		Int;


} //namespace idl::scaner

typedef std::string 		Name;

class NamePath {
  public:
  	std::string str() const;

  	static NamePath make(const std::string& str);
  	
  	const Name& last() const {
  		ASSERT(_path.size() > 0);
  		return *_path.rbegin();
  	}
  	
  	const Name& get(int i) const {
  		int index = (i < 0) ? _path.size() - i : i;
  		ASSERT((index >= 0) && (index < int(_path.size())));
  		return _path[index];
  	}

  	size_t size() const {
  		return _path.size();
  	}
  	  	
  	/*NamePath range(int i) const {
  		int index = (i < 0) ? _path.size() - i : i;
  		ASSERT(index > 0 && index < _path.size());
  		NamePath result(_path.begin() + index, _path.end());
  	
  		return _path[index];
  	}
  	
  	NamePath range(int from, int to) const {
  		int index = (i < 0) ? _path.size() - i : i;
  		ASSERT(index > 0 && index < _path.size());
  		return _path[index];
  	}*/
  	
  	void push_back(const Name& name) {
  		_path.push_back(name);
  	}
  	
  	bool operator < (const NamePath& b) const {
  		for (size_t i = 0; ; ++i) {
  			if (i == _path.size() || i == b._path.size()) {
  				return _path.size() < b._path.size();
  			}
  			
  			if (_path[i] == b._path[i])
  				continue;
			return _path[i] < b._path[i];
  		}
  	}
  	  	
  private:
  	typedef std::vector<Name>	Names;
  	Names	_path;
};

} //namespace idl

#endif //idl_types_hpp
