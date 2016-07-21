set ssuser=%user_release%
set sspwd=%user_release_pwd%
set ssdir=\\avp.ru\global\sourcesafe

rem *** labeling CommonFiles files
ss label "$/Release/Base Technologies 1.0/CommonFiles" -I-Y "-LUpdater SCM4Exchange 1.0.0.%kl_buildno%"

rem *** labeling KeepUp2Date
ss label "$/Release/Base Technologies 1.0/components/keepup2date" -I-Y "-LUpdater SCM4Exchange 1.0.0.%kl_buildno%"
