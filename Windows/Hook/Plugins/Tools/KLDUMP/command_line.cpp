// command_line.cpp: implementation of the command_line class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "command_line.h"
#include "pcre\pcre.h"
#include "RangeTypes.h"

extern bool g_bProtoAll;
extern bool g_bProtoTCP;
extern bool g_bProtoUDP;
extern bool g_bProtoICMP;
extern int  g_Proto;

extern bool g_bBcasts;

extern std::vector<Address> g_vecORRemoteList;
extern std::vector<Address> g_vecNEGRemoteList;

extern std::vector<Address> g_vecORLocalList;
extern std::vector<Address> g_vecNEGLocalList;

extern char* g_szFile2Store;


struct ParseString
{
	explicit ParseString (char* szBaseString) : m_szString(szBaseString) {}
	
	std::string Get(int offset_begin, int offset_end)
	{
		if (offset_begin!=-1 && offset_end!=-1 && offset_begin>=0 && offset_begin<strlen(m_szString))
			return std::string(m_szString+offset_begin, m_szString+offset_end);
		
		return "";
	}
	
	char* m_szString;
};

//									port_start				-						port_end
static bool make_ports( std::string& start_port, std::string& range_symbol, std::string& end_port, OUT Range<USHORT>& out_port )
{
	if (start_port.empty ())
		return false;
	else
	{
		int port_start_int = atoi(start_port.c_str ());

		USHORT port_start;
		if (port_start_int>=0 && port_start_int<=USHRT_MAX)
			port_start = (USHORT)port_start_int;
		else
			return false;

		if (range_symbol.empty ())
		{
			out_port.type = RangeType::eSingle;
			out_port.val = port_start;
			out_port.bval_any = false;
		}
		else if (range_symbol == "-")
		{
			out_port.type = RangeType::eRange;
			out_port.start = port_start;
			out_port.bval_any = false;
			out_port.bend_any = true;
		}
		else
		{
			int port_end_int = atoi(end_port.c_str ());
			
			USHORT port_end;
			if (port_end_int>=0 && port_end_int<=USHRT_MAX && port_end_int>port_start_int)
				port_end = (USHORT)port_end_int;
			else
				return false;

			out_port.type = RangeType::eRange;
			out_port.start = port_start;
			out_port.end = port_end;
			out_port.bval_any = false;
			out_port.bend_any = false;
		}
	}
	
	return true;
}

static bool make_ip (std::string& part1, std::string& part2, std::string& part3, std::string& part4, OUT ULONG& out_ip)
{
	if(part1.empty () || part2.empty () || part3.empty () || part4.empty ())
		return false;

	int part1_int = atoi(part1.c_str());
	int part2_int = atoi(part2.c_str());
	int part3_int = atoi(part3.c_str());
	int part4_int = atoi(part4.c_str());

	if (part1_int>=0 && part1_int<=255 &&
		part2_int>=0 && part2_int<=255 &&
		part3_int>=0 && part3_int<=255 &&
		part4_int>=0 && part4_int<=255)
	{
		out_ip = (part1_int<<24)|(part2_int<<16)|(part3_int<<8)|part4_int;
		return true;
	}

	return false;
}


static bool parse_address(char* szAddr, Address& addr)
{
	const char* pszFullPattern = "^(!)?(\\d{1,3})\\.(\\d{1,3})\\.(\\d{1,3})\\.(\\d{1,3})(-((\\d{1,3})\\.(\\d{1,3})\\.(\\d{1,3})\\.(\\d{1,3}))?)?(:(\\d+)(-(\\d+)?)?)?$";
	const char* pszPortPattern = "^(!)?(\\d+)(-(\\d+)?)?$";

	pcre *f;				// переменная для хранения преобразованного шаблона
    const char *errstr;		// буфер для сообщения об ошибке
    int errchar;			// номер символа
    int vector[100] = {-1}; // массив для результатов, размер = (количество скобок "(" ")" + 2)*3/2
    int vecsize=100;		// размер массива
    int pairs;				// количество найденных пар
	

	ParseString ps(szAddr);

    if( (f=pcre_compile(pszFullPattern,PCRE_CASELESS,&errstr,&errchar,NULL)) && 
		((pairs=pcre_exec(f, NULL, szAddr, strlen(szAddr), 0, PCRE_NOTEMPTY, vector, vecsize))>0) )
	{
		Range<ULONG> ip;
		if (!make_ip(ps.Get (vector[4], vector[5]), ps.Get (vector[6], vector[7]), ps.Get (vector[8], vector[9]), ps.Get (vector[10], vector[11]), ip.start))
		{
			printf("Invalid ip address format: %s\n", szAddr);
			return false;
		}
		else
		{
			std::string range = ps.Get (vector[12], vector[13]);
			if(range.empty ())
			{
				//single address (ip_start)
				ip.type = RangeType::eSingle;
				ip.bval_any = false;
				ip.bend_any = false;
			}
			else if (range == "-")
			{
				//ip_start - any
				ip.type = RangeType::eRange;
				ip.bval_any = false;
				ip.bend_any = true;
			}
			else
			{
				if (!make_ip(ps.Get (vector[16], vector[17]), ps.Get (vector[18], vector[19]), ps.Get (vector[20], vector[21]), ps.Get (vector[22], vector[23]), ip.end))
				{
					printf("Invalid ip address format: %s\n", szAddr);
					return false;
				}

				//range (ip_start - ip_end)
				ip.type = RangeType::eRange;
				ip.bval_any = false;
				ip.bend_any = false;

			}
		}

		Range<USHORT> port;

		if (ps.Get (vector[26], vector[27]).empty ())
		{
			port.type = RangeType::eSingle;
			port.bval_any = true;
		}
		else
		{
			//						port_start							-								port_end
			if (!make_ports( ps.Get (vector[26], vector[27]), ps.Get (vector[28], vector[29]), ps.Get (vector[30], vector[31]), port ))
			{
				printf("Invalid port format: %s\n", szAddr);
				return false;
			}
		}

		addr.ip = ip;
		addr.port = port;

	}
	else if( (f=pcre_compile(pszPortPattern,PCRE_CASELESS,&errstr,&errchar,NULL)) && 
			((pairs=pcre_exec(f, NULL, szAddr, strlen(szAddr), 0, PCRE_NOTEMPTY, vector, vecsize))>0) )
	{
		Range<USHORT> port;
		//					port_start							-								port_end
		if (!make_ports( ps.Get (vector[4], vector[5]), ps.Get (vector[6], vector[7]), ps.Get (vector[8], vector[9]), port ))
		{
			printf("Invalid port format: %s\n", szAddr);
			return false;
		}

		addr.ip.type = RangeType::eSingle;
		addr.ip.bval_any = true;

		addr.port = port;
	}
	else
	{
		printf("Invalid address format: %s\n", szAddr);
		return false;
	}

	return true;
}


static void ShowError()
{
	printf("Command line error. Execution stopped.\n");
}

static void ShowUsage()
{
    printf("Usage: kldump <options>\n\n"

           "options:\n"
           "-f <filename>   - output file\n"
           "-r <ip>:<port>  - remote address(es) and/or port(s)\n"
           "-l <ip>:<port>  - local address(es) and/or port(s)\n"
           "-p <proto>      - IP protocol: tcp, udp, icmp or numeric.\n"
           "-b              - capture broadcasts as well.\n"
          );
}

bool parse_commandline(int argc, char* argv[])
{
	int i = 1;
    if (argc < 2)
    {
        ShowUsage();
        return false;
    }
	while ( i<argc )
	{
		if (argv[i][0] == '/' || argv[i][0] == '-')
		{
			if (strlen(argv[i]) != 2)
			{
				ShowError ();
				return false;
			}
			
			switch(argv[i][1])
			{
            case '?':
                ShowUsage();
                return false;

			case 'r':
			case 'l':
				if (i+1<argc)
				{
					Address adr;
					if (parse_address(argv[i+1], adr))
					{
						if (argv[i+1][0] == '!')
						{
							argv[i][1]=='r'?g_vecNEGRemoteList.push_back(adr):g_vecNEGLocalList.push_back(adr);
						}
						else
						{
							argv[i][1]=='r'?g_vecORRemoteList.push_back(adr):g_vecORLocalList.push_back(adr);
						}
					}
					else
						return false;
					
					i+=2;
				}
				else
				{
					ShowError ();
					return false;
				}
				break;
				
			case 'f':
				if (i+1<argc)
				{
					g_szFile2Store = argv[i+1];
					i+=2;
				}
				else
				{
					ShowError ();
					return false;
				}
				break;

            case 'p':
				if (i+1<argc)
				{
                    if      (stricmp(argv[i+1], "tcp") == 0)  g_bProtoTCP = true, g_bProtoAll = false;
                    else if (stricmp(argv[i+1], "udp") == 0)  g_bProtoUDP = true, g_bProtoAll = false;
                    else if (stricmp(argv[i+1], "icmp") == 0) g_bProtoICMP = true, g_bProtoAll = false;
                    else 
                        sscanf(argv[i+1], "%d", &g_Proto), g_bProtoAll = false;
					
					i+=2;
				}
				else
				{
					ShowError ();
					return false;
				}
                break;

            case 'b':
                g_bBcasts = true;
                ++i;
                break;

            default:
                ShowUsage ();
                return false;
			}
		}
		else
		{
			ShowError ();
			return false;
		}
	}

	return true;
}
