#if !defined(_LOCALTIME_H_)
#define _LOCALTIME_H_

#include <string>

namespace CLocalTime
{
	std::string getRfc822Date(time_t time);
    std::string getRfc822Date();
	int getGmtOff();
	std::string getTime();
	std::string getDate();
	int CalcGmtOffs(time_t time);
};

#endif //
