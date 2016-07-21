set initScriptPath=testCases/

@echo *******************************************************************
call testCases/antiSpamDifference.bat

@echo *******************************************************************
call testCases/blackDate.bat

@echo *******************************************************************
call testCases/checkByDate.bat

@echo *******************************************************************
call testCases/compression.bat

@echo *******************************************************************
call testCases/differenceFormats.bat

@echo *******************************************************************
call testCases/duplicates.bat

@echo *******************************************************************
call testCases/knownReply.bat

@echo *******************************************************************
call testCases/localPathAndRelativeUrlChange.bat

@echo *******************************************************************
call testCases/optionalComponent.bat

@echo *******************************************************************
call testCases/returnCodes.Miscellaneous.bat

@echo *******************************************************************
call testCases/returnCodes.RESOLVE_CONNECT_FAIL.bat

@echo *******************************************************************
call testCases/rollback.bat

@echo *******************************************************************
call testCases/sameFileNameDifferentRelativeURL.bat

@echo *******************************************************************
call testCases/transportFTP_HTTP.bat

@echo *******************************************************************
call testCases/UpdateFromPlainStructure.bat

@echo *******************************************************************
call testCases/updateRetranslationFlagCombinations.bat

@echo *******************************************************************
call testCases/updateRetranslationSequences.bat

@echo *******************************************************************
call testCases/xmlTags.bat

echo all tests performed
pause