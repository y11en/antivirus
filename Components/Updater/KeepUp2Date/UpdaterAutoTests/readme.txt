
Automatic Kaspersky Updater Component Testing

 Purpose: automates Updater functionality testing
 Usage:
   1. correct pathes in testCases/init.bat script
   2. run runAllTestCases.bat script
 Result: in case some test case fails then script is paused,
   failure reason is logger to trace file,
   and failed test case name is output



File and Folder documentation
/root
 |
 |- /baseSources/ 		source bases for automatic update tests
 |
 |- /configuration/		Updater configuration files
 |      |- /ss_storage.ini			IUpdater.exe configuration file
 |      |- /ss_storage.customSource		configuration file for custom source tests
 |      |- /ss_storage.customUpdateRetranslationOptions		configuration file for custom update/retranslation flags
 |
 |- /cygwin/			external tools to check automatic tests results
 |
 |- /etalonsToCompareResult/ 	etalon bases to automatically compare results
 |
 |- /testCases/
 |      |- /init.bat		helper script for initial initialization
 |      |
 |      |- /antiSpamDifference.bat	test if SQZE compression for Anti-Spam works
 |      |- /blackDate.bat	asserts that black list date validity is checked
 |      |- /checkByDate.bat	asserts that obsolete by date index is not downloaded and not replace local files
 |      |- /compression.bat	automatically performs recursive difference file apply and klz tests
 |      |- /differenceFormats.bat different difference format support
 |      |- /duplicates.bat	assert that filtering functionality works even if duplicate files are on source
 |      |- /knownReply.bat	perform known replies automatic tests
 |      |- /localPathChanged.bat assert that change of LocalPath XML tag does not damage update
 |      |- /optionalComponent.bat  asserts that missing optional component functionality works
 |      |- /returnCodes.Miscellaneous.bat 		test miscellaneous return codes
 |      |- /returnCodes.RESOLVE_CONNECT_FAIL.bat 	test invalid source name (not resolvable cases, not connectable)
 |      |- /rollback.bat	rollback automatic test case
 |      |- /sameFileNameDifferentRelativeURL.bat test case that file with the same name
              but located in different relative URL on source does not break retranslation
 |      |- /transportFTP_HTTP.bat	test if FTP and HTTP transport
 |      |- /UpdateFromPlainStructure.bat		test if update from plain structure folder passes
 |      |- /updateRetranslationFlagCombinations.bat 	update/retranslation combinations test case 
 |      |- /updateRetranslationSequences.bat		assert update/retranslation operations in different sequencies
 |      |- /xmlTags.bat		parse different XML tags
 |
 |
 |
 |- /clean.bat			cleans output
 |- /readme.txt			this file with documentation
 |
 |- /runAllTestCases.bat	runs all test cases



 