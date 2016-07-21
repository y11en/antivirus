#ifndef __ENFORCEMENT_H_
#define __ENFORCEMENT_H_

#include <exception>
#include <stdexcept>

#define STRINGIZE(something) STRINGIZE_HELPER(something) 
#define STRINGIZE_HELPER(something) #something
#define ENFORCE(exp) \
    MakeEnforcer((exp), "Expression '" #exp "' failed in '" \
    __FILE__ "', line: "STRINGIZE(__LINE__)).Enforce()

//NB (see BUG: Expansion of __LINE__ Macro Is Incorrect When Using /ZI in MSDN)

// Macro trick to quote a number:
template <class Ref>
class Enforcer
{
    Ref obj_;
	const char* locus_;
public:
    Enforcer(Ref obj, const char* locus) : obj_(obj), locus_(locus) {}
    Ref Enforce()
    {
        if (!obj_) throw std::runtime_error(locus_);
        return obj_;
    }
};

template <typename T>
inline Enforcer<const T&>
MakeEnforcer(const T& obj,  const char* locus)
{
    return Enforcer<const T&>(obj, locus);
}



#endif //__ENFORCEMENT_H_
