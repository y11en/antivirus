#ifndef Exception_hpp
#define Exception_hpp

#include <exception>
#include <boost/format.hpp>

#define IDL_EXCEPTION(x)        	idl::Exception(__FILE__, __LINE__, x)
#define THROW_EXCEPTION(class, x)   class(__FILE__, __LINE__, x)

namespace idl {

class Exception : public std::exception {
  public:
    Exception(const char* filename, int line, const std::string& format);
    Exception(const std::string& format);
    
    virtual ~Exception() throw();

    virtual const char* what() const throw();

    template <class E, class T>
    friend const E& operator% (const E& e, T t) {
        e._format % t;
        return e;
    }
        
  protected:
    void retrieveSystemError();

  protected:
    std::string         _context;
    std::string         _systemError;
    mutable std::string _output;

    mutable boost::format       _format;
};

} //namespace idl

#endif //Exception_hpp
