#include "heur_delself.h"
#include "srv.h"

size_t parse_bat_line(char* buff, size_t size, size_t* pos, char* bat_args[], size_t max_bat_args)
{
	size_t p = *pos;
	for (size_t i=0; i<max_bat_args; i++)
		bat_args[i] = 0;
	if (0 == size)
		return 0;
	buff[size-1] = 0; // ensure zero-terminated
	while (p < size)
	{
		// parse line
		int start = 1;
		size_t bat_argc = 0;
		int str = 0;
		for (;p<size;p++)
		{
			char c = buff[p];
			if (c=='\r' || c=='\n')
			{
				buff[p++] = 0;
				break;
			}
			if (c>='A' && c<='Z')
				buff[p] = c = c|0x20; // lcase
			if (c <= ' ')
			{
				if (str)
					continue;
				buff[p] = 0;
				if (bat_argc == 1)
				{
					while (bat_args[0][0] == '@')
						bat_args[0]++;
					if (bat_args[0][0] == 0)
						bat_argc--;
				}
				start = 1;
			}
			else if (start)
			{
				if (c == '\"')
					str = 1;
				if (bat_argc < max_bat_args)
					bat_args[bat_argc++] = buff+p+str;
				start = 0;
			}
			else if (c == '\"' && str)
			{
				buff[p++] = 0;
				str = 0;
			}
		}
		if (!bat_argc)
			continue;
		*pos = p;
		return bat_argc;
	}
	return 0;
}

void cEventHandlerHeurDelSelf::iParseBat(cEvent& event, cFile& bat_file, tcstring cmd_line, bool bParseCmdLine)
{
	char buff[0x200];
	uint32_t size;

	if (bParseCmdLine)
	{
		if (!cmd_line)
			return;
		size = (uint32_t) tstrlen(cmd_line);
		if (size > countof(buff)-1)
			size = countof(buff)-1;
		tstrncpy(buff, cmd_line, size);
	}
	else
	{
		if (!bat_file.Read(0, buff, sizeof(buff)-2, &size))
			return;
	}
	buff[size++] = 0; // ensure zero-terminated
	char* bat_args[10];
	size_t bat_argc = 0;
	tcstring argv[10];
	size_t argc = 0;
	size_t pos = 0;
	tstring cmd_line_args = tstrdup(cmd_line);
	if (!cmd_line_args)
		return;
	argc = srvParseCmdLine(m_pEnvironmentHelper, cmd_line_args, NULL, argv, countof(argv));
	pos = 0;
	while (bat_argc = parse_bat_line(buff, size, &pos, bat_args, countof(bat_args)))
	{
		if (':' == bat_args[0][0]) // label
			continue;
		for (size_t i=0; i<bat_argc; i++)
		{
			if (bat_args[i][0]=='%' && bat_args[i][1]>='0' && bat_args[i][1]<='9')
			{
				size_t n = bat_args[i][1]-'0';
				if (argc > n)
					bat_args[i] = (char*)argv[n];
			}
		}
		if (bat_argc>1 && 0==tstrcmp(bat_args[0], _T("del")))
		{
			cFile del_file(m_pEnvironmentHelper, bat_args[1]);
			cProcess pProcess = event.GetProcess();
			if (!pProcess)
				break;
			if (0 == tstrcmp(del_file.getFull(), pProcess->m_image.getFull()))
			{
				cEvent new_event(event, pProcess->m_pid);
				pProcess.release();
				m_pEventMgr->OnDeleteSelf(new_event, del_file);
				break;
			}
		}
	}
	tstrfree(cmd_line_args);
	return;
};

void cEventHandlerHeurDelSelf::OnProcessCreatePre(cEvent& event, cFile& image_path, tcstring cmd_line, uint32_t creation_flags)
{
	if (0 == tstrcmp(image_path.getExtension(), _T("BAT"))
		|| 0 == tstrcmp(image_path.getExtension(), _T("CMD")))
	{
		return iParseBat(event, image_path, cmd_line, false);
	}
	if (cmd_line && 0 == tstrcmp(image_path.getFilename(), _T("CMD.EXE")))
	{
		tcstring ptr = tstrstr(cmd_line, _T("/c "));
		if (!ptr)
			ptr = tstrstr(cmd_line, _T("/C "));
		if (ptr)
			return iParseBat(event, image_path, tstrninc(ptr, 3), true);
	}
	return;
}

