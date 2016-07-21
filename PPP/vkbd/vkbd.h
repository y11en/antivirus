 #ifndef __VKBD_H__
#define __VKBD_H__

#define MOD_CAPITAL 0x0010
#define VKBD_INIT_FUNC                extern "C"  bool         __stdcall Init()
#define VKBD_DEINIT_FUNC              extern "C"  bool         __stdcall Deinit()
#define VKBD_PROCESS_VKBD_EVENT_FUNC  extern "C"  bool         __stdcall ProcessVkbdEvent(DWORD nVK, bool bKeyDown)
#define VKBD_GET_KEY_NAME_FUNC        extern "C"  const char * __stdcall GetKeyName(DWORD nVK, const void *hLayout)
#define VKBD_GET_KEYBOARD_LAYOUT_FUNC extern "C"  const void * __stdcall GetKeboardLayout()

struct VkbdApi
{
	VkbdApi() : m_pInit(NULL), m_pDeinit(NULL), m_pProcessVkbdEvent(NULL), m_pGetKeyName(NULL), m_pGetKeboardLayout(NULL) {}
	
	typedef bool         (__stdcall *tInit)();
	typedef bool         (__stdcall *tDeinit)();
	typedef bool         (__stdcall *tProcessVkbdEvent)(unsigned nVK, bool bKeyDown);
	typedef const char * (__stdcall *tGetKeyName)(unsigned nVK, const void *hLayout);
	typedef const void * (__stdcall *tGetKeboardLayout)();

	tInit             m_pInit;
	tDeinit           m_pDeinit;
	tProcessVkbdEvent m_pProcessVkbdEvent;
	tGetKeyName       m_pGetKeyName;
	tGetKeboardLayout m_pGetKeboardLayout;
};

#endif // __VKBD_H__
