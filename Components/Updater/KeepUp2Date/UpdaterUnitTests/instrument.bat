rem "H:\Microsoft Visual Studio 8\Team Tools\Performance Tools\vsinstr.exe" H:\Updater-dev\out_win32\debug\Updater.dll /coverage
del H:\Updater-dev\out_win32\debug\Updater.dll
rename H:\Updater-dev\out_win32\debug\Updater.dll.orig Updater.dll
rem "H:\Microsoft Visual Studio 8\Team Tools\Performance Tools\vsinstr.exe" H:\Updater-dev\out_win32\debug\Updater.dll /exclude:DSKMCheckSign;DSKMFindSign;DSKMHashBuffer;DSKMHashEnd;DSKMHashInit;DSKM_CheckCurrObject;DSKM_CheckFile;DSKM_PrepareObjectHash;KLUPD::Signature6Checker::Implementation::DskmListWrapper::checkSignature
rem "H:\Microsoft Visual Studio 8\Team Tools\Performance Tools\vsinstr.exe" H:\Updater-dev\out_win32\debug\Updater.dll /coverage
"H:\Microsoft Visual Studio 8\Team Tools\Performance Tools\vsinstr.exe" H:\Updater-dev\out_win32\debug\Updater.dll /coverage /exclude:DSKM*