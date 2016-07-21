set ssuser=%user_release%
set sspwd=%user_release_pwd%
set ssdir=\\avp.ru\global\sourcesafe

rem *** labeling CommonFiles files
ss label "$/CommonFiles" -I-Y "-LLicensing 2.0.0.%kl_buildno%"
