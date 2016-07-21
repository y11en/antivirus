#include "windows.h"
#include "console.h"
#include "cio.h"

#define ARGS_NUM 10

int get_command(char* cmd, char** args, int* pargc);
int parse_command(char* cmd, char** args, int* pargc);

int console(int do_command(char* cmd, char** args, int argc))
{
	char cmd[0x100];
	char* args[ARGS_NUM];
	int argc;

	do 
	{
		get_command(cmd, args, &argc);
	} while (do_command(cmd, args, argc));
	
	return 0;
}

int get_command(char* cmd, char** args, int* pargc)
{
	_printf("-");
	_gets(cmd);
	if (*cmd == 0)
		return 0;
	parse_command(cmd, args, pargc);
	return 1;
}

int parse_command(char* cmd, char** args, int* pargc)
{
	int i;
	int j;
/*
	for (i=0; i<ARGS_NUM; i++)
	{
		args[i] = "";
	}
*/
	i=0;
	j=0;
	while (cmd[i] == ' ' || cmd[i] == '\t')
		i++;
	while(cmd[i] != 0)
	{
		if (cmd[i] == '\t')
			cmd[i] = ' ';
		if (cmd[i] >= ' ')
		{
			cmd[j] = cmd[i];
			j++;
		}
		i++;
	}
	cmd[j] = 0;

	*pargc = 0;
	args[0] = cmd;
	i=0;
	while (cmd[i] != 0)
	{
		if (cmd[i] == ' '/* || cmd[i] == '='*/)
		{
			cmd[i] = 0;
			if (args[*pargc][0] != 0)
				(*pargc)++;
			args[*pargc] = cmd+i+1;
		}
		i++;
	}
	if (args[*pargc][0] == 0)
		(*pargc)--;

	for (i=*pargc+1; i<ARGS_NUM; i++)
		args[i] = "";
	
	return 0;
}

