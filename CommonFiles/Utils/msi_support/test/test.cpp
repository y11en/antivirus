// test.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>

#pragma warning(disable : 4786)
#include "../common/ca_data_parser.h"

//////////////////////////////////////////////////////////////////////////
void test_cmd_parser()
{
	using namespace MsiSupport::Utility;
	using namespace std;
	CADataParser<char> parser;
//	std::string s("aaaa=    vvvvv    ;dddd=\"aaaaa''assds\"  ; a='g'");
	std::string s("a=b");
	if (parser.parse(s))
	{
		const CADataParser<char>::values_map_t& values = parser.values();
		for (CADataParser<char>::values_map_t::const_iterator it = values.begin();
				it != values.end();
				++it)
		{
			cout << it->first << " : " << it->second << endl;
		}
	}
	char ch;
	cin >> ch;
}

//////////////////////////////////////////////////////////////////////////
void test_remove_on_restart(const char* file)
{
	::MoveFileExA(file, NULL, MOVEFILE_DELAY_UNTIL_REBOOT);
}

//////////////////////////////////////////////////////////////////////////
int main(int argc, char* argv[])
{
	if (argc < 1 || std::string(argv[1]) == "cmd_parser")
	{
		test_cmd_parser();
	}
	else if (argc >= 2 || std::string(argv[1]) == "remove_on_reboot")
	{
		test_remove_on_restart(argv[2]);
	}
	return 0;
}

