copy sources_amd64_debug sources
build -cZg
del sources

copy .\objchk_wnet_amd64\amd64\kl1sup.lib ..\KLSDK\LIB\amd64\debug\kl1sup.lib