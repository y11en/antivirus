#include "stdafx.h"
#include "util.h"

std::string GetBinBufferTraceString(const BYTE *pDataBuffer, DWORD dwDataBufferSize)
{
#ifdef _TURN_OFF_BIN_TRACES
	return "<skipped>";
#else
	std::string str;

	const int chars_per_line = 0x20;
	const int max_line_count = 0x10;
	const int last_line_count = 2;

	BYTE b;
	char line[chars_per_line * 3 + 2 + chars_per_line * 2 + 1], *pline, *pcline;
	bool last_lines = false;
	for(int line_count = 0; dwDataBufferSize; ++line_count)
	{
		if(last_lines || line_count < max_line_count)
		{
			memset(line, ' ', sizeof(line));
			line[sizeof(line) - 1] = '\0';
			pline = line;
			pcline = line + chars_per_line * 3 + 2;
			for(int i = 0; i < chars_per_line; ++i)
			{
				if(dwDataBufferSize)
				{
					--dwDataBufferSize;
					b = *pDataBuffer++;
					pline += _snprintf(pline, 3, "%02x ", b);
					//					if(b == '%')
					//						pcline += _snprintf(pcline, 2, "%%%%");
					//					else
					pcline += _snprintf(pcline, 1, "%c", (isprint(b) && !iscntrl(b)) ? b : '.');
				}
			}
			*pcline = '\0';
			if(!str.empty())
				str += "\r\n";
			str += line;
		}
		else
		{
			str += "\r\n";
			str += "...";
			while(dwDataBufferSize > last_line_count * chars_per_line)
			{
				dwDataBufferSize -= chars_per_line;
				pDataBuffer += chars_per_line;
			}
			last_lines = true;
		}
	}

	return str;
#endif // _TURN_OFF_BIN_TRACES
}

std::string GetBinBufferTraceStringShort(const BYTE *pDataBuffer, DWORD dwDataBufferSize)
{
#ifdef _TURN_OFF_BIN_TRACES
	return "<skipped>";
#else
	std::string str;
	const size_t max_len = 0x20;

	size_t len = min(max_len, dwDataBufferSize);

	unsigned char ch;
	str.resize(len);
	for(size_t i = 0; i < len; ++i)
		str[i] = isprint((ch = *pDataBuffer++)) ? ch : '.';
	if(len < dwDataBufferSize)
		str += "...";
	return str;
#endif // _TURN_OFF_BIN_TRACES
}
