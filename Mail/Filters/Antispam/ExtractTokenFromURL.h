#include "stdafx.h"
#include <string>
using namespace std;

typedef vector<string> tokens_t;

inline void ExtractTokenFromURL(const char* src, tokens_t& tokens)
{
	if ( src ) 
	{
		const char* pos = strstr(src, "http://");
		string domain = "";
		bool bNeedToFree = false;
		if ( pos ) 
		{
			const char* url = pos + strlen("http://");
			const char* pos_path = strstr(url, "/");
			domain = pos;
			// Засовываем целиком URL
			if ( !domain.empty() ) 
				tokens.push_back(domain);
			domain = url;
			// Ищем домен
			if ( pos_path )
				domain.erase(pos_path - url, domain.length()-(pos_path - url));
			const char* pos_dog = strstr(domain.c_str(), "@");
			if ( pos_dog ) 
				domain = pos_dog + sizeof(char);
			// Засовываем домен
			if ( !domain.empty() ) 
				tokens.push_back(domain);
		}
	}
}