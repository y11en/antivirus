
InventorySCUMps.dll: dlldata.obj InventorySCUM_p.obj InventorySCUM_i.obj
	link /dll /out:InventorySCUMps.dll /def:InventorySCUMps.def /entry:DllMain dlldata.obj InventorySCUM_p.obj InventorySCUM_i.obj \
		kernel32.lib rpcndr.lib rpcns4.lib rpcrt4.lib oleaut32.lib uuid.lib \

.c.obj:
	cl /c /Ox /DWIN32 /D_WIN32_WINNT=0x0400 /DREGISTER_PROXY_DLL \
		$<

clean:
	@del InventorySCUMps.dll
	@del InventorySCUMps.lib
	@del InventorySCUMps.exp
	@del dlldata.obj
	@del InventorySCUM_p.obj
	@del InventorySCUM_i.obj
