#include <windows.h>
#include <tchar.h>
#include <stuff/pathstr.h>
#include <stuff/pathstra.h>
#include <stuff/pathstrw.h>
#include <stuff/csystem.h>
#include <stuffio/xfile.h>
#include <stuffio/ioutil.h>


IOS_ERROR_CODE IOSFinishCopyingW( const WCHAR * pszFileName, WCHAR * pszBuffer, uint uicchBufferLen );
	
//---
IOS_ERROR_CODE IOSFullPathFromRelativePathW( const WCHAR * pszMainFileName, const WCHAR * pszRelFileName, WCHAR * pszBuffer, uint uicchBufferLen ) {
#if defined(_UNICODE)
	return IOSFullPathFromRelativePath( pszMainFileName, pszRelFileName, pszBuffer, uicchBufferLen );
#else
  if ( pszRelFileName && pszBuffer ) {
    // Превратим в полный путь
    CAPointer<WCHAR> mainFullPath = new WCHAR[_MAX_PATH];
    IOS_ERROR_CODE result = ::IOSMakeFullPathW( pszMainFileName, mainFullPath, _MAX_PATH );
    if ( result == IOS_ERR_OK ) {
      // Выделим чистый путь
      result = ::IOSDirExtractW( mainFullPath, mainFullPath, _MAX_PATH );
      if ( result == IOS_ERR_OK ) {
      // Выделим чистый путь
        CAPointer<WCHAR> relPath = new WCHAR[_MAX_PATH];
        result = ::IOSDirExtractW( pszRelFileName, relPath, _MAX_PATH );
        if ( result == IOS_ERR_OK ) {
					int n0 = _wcsnextc(_wcsninc(relPath, 0));
					int n1 = _wcsnextc(_wcsninc(relPath, 1));
          if ( !(n1 == L':' || (n0 == L'\\' && n1 == L'\\' ) ) ) { // если имя не содержит полный путь
            if ( n0 != L'\\' ) {
              // Если проверяетмый путь действительно относительный
              // Вычислим полную длину путей
              int mainCharCount = (int)wcslen( mainFullPath );
              int relCharCount  = (int)wcslen( relPath );

              // Разобьем пути на составляющие
              WCHAR *mainSubDir = wcstok( mainFullPath, L"\\" );
              while ( mainSubDir )
                mainSubDir = wcstok( NULL, L"\\" );

              WCHAR *relSubDir  = wcstok( relPath, L"\\" );
              while ( relSubDir )
                relSubDir = wcstok( NULL, L"\\" );

              // Отступаем от конца главного пути столько каталогов,
              // сколько обратных точек в проверяемом
              mainSubDir = _wcsninc(mainFullPath, mainCharCount);
              relSubDir  = relPath;
              int mainCount = 0;
              int relCount  = 0;
              while ( mainCount < mainCharCount && 
                      relCount  < relCharCount  &&
                      !_wcsicmp(relSubDir, L".." ) ) {
                mainSubDir = _wcsdec(mainFullPath, mainSubDir);
                while ( mainSubDir && _wcsnextc(mainSubDir) != L'\0' )
                  mainSubDir = _wcsdec(mainFullPath, mainSubDir);
								if ( !mainSubDir )
									mainSubDir = mainFullPath;
                int curLen = (int)wcslen( mainSubDir + 1 );
                mainCount  += curLen + 1;
                curLen = (int)wcslen( relSubDir );
                relCount  += curLen + 1;
								relSubDir = _wcsninc( relSubDir, curLen ) + 1;
              }        

              // Заводим новый буфер и наполняем его оставшейся частью
              // главного пути
              CAPointer<WCHAR> pszFinishPath = new WCHAR[_MAX_PATH];
              *pszFinishPath = L'\0';
              mainSubDir = mainFullPath;
              mainCharCount -= mainCount;
              mainCount     = 0;
              while ( mainCount < mainCharCount ) {
                wcscat( pszFinishPath, mainSubDir );
                wcscat( pszFinishPath, L"\\" );
                int curLen = (int)wcslen( mainSubDir );
                mainCount  += curLen + 1;
                mainSubDir = _wcsninc(mainSubDir, curLen) + 1;
              }        
              // Переносим в буфер оставшуюся часть проверяемого
              while ( relCount < relCharCount ) {
                wcscat( pszFinishPath, relSubDir );
                wcscat( pszFinishPath, L"\\" );
                int curLen = (int)wcslen( relSubDir );
                relCount  += curLen + 1;
                relSubDir = _wcsninc(relSubDir, curLen - 1) + 1;
              }        

              // Выделяем имя и дополняем им сформированный путь
              wcscpy( relPath, pszRelFileName );
              ::IOSFileExtractW( relPath, relPath, _MAX_PATH );
              wcscat( pszFinishPath, relPath );

              // Завершающее копирование с контролем размера буфера
              return ::IOSFinishCopyingW( pszFinishPath, pszBuffer, uicchBufferLen );
            }
            else {
              // Это корневой каталог главного пути
              CPathStrW pszFinishPath( _MAX_PATH );
              result = ::IOSRootDirExtractW( mainFullPath, pszFinishPath, _MAX_PATH );
              if (  result == IOS_ERR_OK ) {
                pszFinishPath = _wcsninc(relPath, 1);
                // Выделяем имя и дополняем им сформированный путь
                wcscpy( relPath, pszRelFileName );
                ::IOSFileExtractW( relPath, relPath, _MAX_PATH );
                pszFinishPath ^= relPath;

                // Завершающее копирование с контролем размера буфера
                return ::IOSFinishCopyingW( pszFinishPath, pszBuffer, uicchBufferLen );
              }
            } 
          }
          else {
            // Если проверяемый путь не относительный - просто копируем в выходной буфер
            result = ::IOSMakeFullPathW( pszRelFileName, relPath, _MAX_PATH );
            if ( result == IOS_ERR_OK ) 
              return ::IOSFinishCopyingW( relPath, pszBuffer, uicchBufferLen );
          }  
        }
      }
    }
    return result;
  }
  return IOS_ERR_INVALID_FILE_NAME;
#endif	
}
