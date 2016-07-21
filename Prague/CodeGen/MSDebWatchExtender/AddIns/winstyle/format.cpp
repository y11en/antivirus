#include <windows.h>
#include "common\custview.h"

static const tName WindowsStyles[] = {
	{ 0x00CF0000, "WS_OVERLAPPEDWINDOW " },
	{ 0x80880000, "WS_POPUPWINDOW "      },
	{ 0x00C00000, "WS_CAPTION "          },
	{ 0x00000000, "WS_OVERLAPPED "       },
	{ 0x80000000, "WS_POPUP "            },
	{ 0x40000000, "WS_CHILD "            },
	{ 0x20000000, "WS_MINIMIZE "         },
	{ 0x10000000, "WS_VISIBLE "          },
	{ 0x08000000, "WS_DISABLED "         },
	{ 0x04000000, "WS_CLIPSIBLINGS "     },
	{ 0x02000000, "WS_CLIPCHILDREN "     },
	{ 0x01000000, "WS_MAXIMIZE "         },
	{ 0x00800000, "WS_BORDER "           },
	{ 0x00400000, "WS_DLGFRAME "         },
	{ 0x00200000, "WS_VSCROLL "          },
	{ 0x00100000, "WS_HSCROLL "          },
	{ 0x00080000, "WS_SYSMENU "          },
	{ 0x00040000, "WS_THICKFRAME "       },
	{ 0x00020000, "WS_GROUP "            },
	{ 0x00010000, "WS_TABSTOP "          },
	{ 0x00020000, "WS_MINIMIZEBOX "      },
	{ 0x00010000, "WS_MAXIMIZEBOX "      },
};

// this function formats value (passed in dwValue and pEvalInfo parameters) into readable string
HRESULT WINAPI format_value( 
							DWORD dwValue,        // low 32-bits of address or value
							DEBUGHELPER *pHelper, // callback pointer to access helper functions
							int nBase,            // decimal or hex
							BOOL bIgnore,         // not used
							char *pResult,        // where the result needs to go
							size_t max,           // how large the above buffer is
							EVAL_INFO* pEvalInfo  // internal debugger data
							) 
{
	DWORD i;
	DWORD pos = 0;
	
	for (i=0; i<sizeof(WindowsStyles)/sizeof(WindowsStyles[0]);i++)
	{
		if (0 != (dwValue & WindowsStyles[i].id))
		{
			lstrcpyn(pResult+pos, WindowsStyles[i].name, max-pos);
			pos = lstrlen(pResult);
		}
	}
	if (pos == 0)
		lstrcpyn(pResult, "<unknown>", max);	
	return S_OK;
}

