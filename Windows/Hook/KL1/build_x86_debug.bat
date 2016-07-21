copy sources_i386_debug sources
build -cZg
del sources

copy .\objchk_w2k_x86\i386\kl1sup.lib ..\KLSDK\LIB\i386\debug\kl1sup.lib