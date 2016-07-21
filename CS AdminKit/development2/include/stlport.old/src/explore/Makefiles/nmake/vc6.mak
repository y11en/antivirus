!ifndef MSVC_DIR
MSVC_DIR = c:\Program Files\Microsoft Visual Studio\VC98
!endif

CFLAGS_COMMON = /nologo /W3 /GR /GX /Zi
CXXFLAGS_COMMON = /nologo /W3 /GR /GX /Zi

!include $(RULESBASE)/$(USE_MAKE)/vc-common.mak

