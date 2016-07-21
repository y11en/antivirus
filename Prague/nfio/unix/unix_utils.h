#if !defined (_UNIX_UTILS_H_)
#define _UNIX_UTILS_H_

tERROR pr_call systemErrors2PragueErrors ( tDWORD unixErr );

tERROR pr_call _getFileAttributes( const tCHAR* aFileName, tDWORD& anAttributes, tDWORD& aSystemError );

tERROR pr_call _getFileAttributes( const tCHAR* aFilePath, const tCHAR* aFileName, tDWORD& anAttributes, tDWORD& aSystemError );

tERROR pr_call _setFileAttributes( const tCHAR* aFileName, tDWORD anAttributes, tDWORD& aSystemError );

tERROR pr_call _setFileAttributes( const tCHAR* aFilePath, const tCHAR* aFileName, tDWORD anAttributes, tDWORD& aSystemError );


tERROR pr_call _getFileSize ( const tCHAR* aFileName, tQWORD& aFileSize, tDWORD& aSystemError );

tERROR pr_call _getFileSize ( const tCHAR* aFilePath, const tCHAR* aFileName, tQWORD& aFileSize, tDWORD& aSystemError );

tERROR pr_call _setFileSize ( const tCHAR* aFileName, tQWORD aFileSize , tDWORD& aSystemError );

tERROR pr_call _setFileSize ( const tCHAR* aFilePath, const tCHAR* aFileName, tQWORD aFileSize , tDWORD& aSystemError );


tERROR pr_call _getFileTime ( const tCHAR* aFileName, tPROPID aNeededTimeStamp, tDATETIME& aDateTime, tDWORD& aSystemError );

tERROR pr_call _getFileTime ( const tCHAR* aFilePath, const tCHAR* aFileName, tPROPID aNeededTimeStamp, tDATETIME& aDateTime, tDWORD& aSystemError );

tERROR pr_call _setFileTime ( const tCHAR* aFileName, tPROPID aNeededTimeStamp, tDATETIME aDateTime, tDWORD& aSystemError );

tERROR pr_call _setFileTime ( const tCHAR* aFilePath, const tCHAR* aFileName, tPROPID aNeededTimeStamp, tDATETIME aDateTime, tDWORD& aSystemError );

tERROR pr_call _isFolder ( const tCHAR* aFilePath, const tCHAR* aFileName, tBOOL& aResult, tDWORD& aSystemError );


tERROR pr_call _copy ( const tCHAR* aSource, const tCHAR* aDestination, tBOOL anOverwrite, tDWORD& aSystemError );

tERROR pr_call _rename ( const tCHAR* aSource, const tCHAR* aDestination, tBOOL anOverwrite, tDWORD& aSystemError );

tERROR pr_call _rename (  const tCHAR* aSourcePath, const tCHAR* aSourceName, 
                          const tCHAR* aDestinationPath, const tCHAR* aDestinationName, tBOOL anOverwrite, tDWORD& aSystemError );

tERROR pr_call _delete ( const tCHAR* aFileName, tDWORD& aSystemError );

tERROR _absolutePath ( const tCHAR* aPath, tDWORD aSize, tCHAR* aBuffer, tDWORD aBufferSize );

tERROR _dirName ( const tCHAR* aPath, tDWORD aSize, tCHAR* aBuffer, tDWORD aBufferSize, tDWORD& anOutSize );

tERROR _fileName ( const tCHAR* aPath, tDWORD aSize, tCHAR* aBuffer, tDWORD aBufferSize, tDWORD& anOutSize );

#endif // _UNIX_UTILS_H_
