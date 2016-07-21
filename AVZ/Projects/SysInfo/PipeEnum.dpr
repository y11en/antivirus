program PipeEnum;
{$APPTYPE CONSOLE}
uses
  Windows,
  SysUtils;

//
// The following are definitions for documented and undocumented native APIs
// and structures.
//

Const
  DIRBUFSIZE = 512;
  STATUS_NO_MORE_FILES = $80000006;

Type

  NTSTATUS = DWORD;
  USHORT = Word;

  FILE_INFORMATION_CLASS = (
    FileDirectoryInformation = 1,
    FileFullDirectoryInformation,
    FileBothDirectoryInformation,
    FileBasicInformation,
    FileStandardInformation,
    FileInternalInformation,
    FileEaInformation,
    FileAccessInformation,
    FileNameInformation,
    FileRenameInformation,
    FileLinkInformation,
    FileNamesInformation,
    FileDispositionInformation,
    FilePositionInformation,
    FileFullEaInformation,
    FileModeInformation,
    FileAlignmentInformation,
    FileAllInformation,
    FileAllocationInformation,
    FileEndOfFileInformation,
    FileAlternateNameInformation,
    FileStreamInformation,
    FilePipeInformation,
    FilePipeLocalInformation,
    FilePipeRemoteInformation,
    FileMailslotQueryInformation,
    FileMailslotSetInformation,
    FileCompressionInformation,
    FileCopyOnWriteInformation,
    FileCompletionInformation,
    FileMoveClusterInformation,
    FileOleClassIdInformation,
    FileOleStateBitsInformation,
    FileNetworkOpenInformation,
    FileObjectIdInformation,
    FileOleAllInformation,
    FileOleDirectoryInformation,
    FileContentIndexInformation,
    FileInheritContentIndexInformation,
    FileOleInformation,
    FileMaximumInformation
  );
  PFILE_INFORMATION_CLASS = ^FILE_INFORMATION_CLASS;

  IO_STATUS_BLOCK  = Record
    Status: NTSTATUS ;
    Information: ULONG
  End;
  PIO_STATUS_BLOCK = ^IO_STATUS_BLOCK;

  UNICODE_STRING = Record
    Length, MaximumLength: USHORT;
    Buffer: PWideChar;
  End;
  PUNICODE_STRING = ^UNICODE_STRING;

  PIO_APC_ROUTINE = Procedure(
    ApcContext: Pointer;
    IoStatusBlock: PIO_STATUS_BLOCK;
    Reserved: ULONG
  ); StdCall;

  FILE_DIRECTORY_INFORMATION = Record
    NextEntryOffset, FileIndex: ULONG;
    CreationTime, LastAccessTime, LastWriteTime,
    ChangeTime, EndOfFile, AllocationSize: LARGE_INTEGER;
    FileAttributes, FileNameLength: ULONG;
    FileName: WideChar
  End;
  PFILE_DIRECTORY_INFORMATION = ^FILE_DIRECTORY_INFORMATION;

  ENT_Error = Class(Exception);

//
// Native functions we use
//
Var
  NtQueryDirectoryFile: Function (
    FileHandle, Event: THANDLE;
    ApcRoutine: PIO_APC_ROUTINE;
    ApcContext:	Pointer;
    IoStatusBlock: PIO_STATUS_BLOCK;
    FileInformation: PFILE_DIRECTORY_INFORMATION;
    Length: DWORD;
    aFileInformationClass: FILE_INFORMATION_CLASS;
    ReturnSingleEntry: BOOLEAN;
    FileName: PUNICODE_STRING;
    RestartScan: BOOLEAN
  ): NTSTATUS; StdCall;

  RtlNtStatusToDosError: Function(Status: NTSTATUS): ULONG; StdCall;

  Function SysErrorMessage(ErrorCode: DWORD): String;
  Begin
    Result := SysUtils.SysErrorMessage(ErrorCode);
    AnsiToOEM(PChar(Result), PChar(Result));
  End;

  Procedure NT_Check(aStatus: NTSTATUS);
  Var aErr: Integer;
  Begin
    If (aStatus and $80000000 = $80000000) Or (aStatus = 0) Then Exit;
    aErr := RtlNtStatusToDosError(aStatus);
    Raise ENT_Error.Create(Format('Error (%d): "%s"', [aErr, SysErrorMessage(aErr)]));
  End;

  Function FoldString(Const aStr: String; aSize: Integer; Delimiter: Char = ' '): String;
  Var
    aLen: Integer;
    J: Integer;
  Begin
    aLen := Length(aStr);
    If aLen = aSize Then Exit;
    Result := aStr;
    SetLength(Result, aSize);
    If aLen > aSize Then Exit;

    For J := aLen + 1 To aSize Do
      Result[J] := Delimiter;
  End;

Var
  pipes: THANDLE;
  aNTStatus: NTSTATUS;
  dirInfo, curEntry: PFILE_DIRECTORY_INFORMATION;
  firstQuery: BOOLEAN = TRUE;
  ioStatus: IO_STATUS_BLOCK;
  aStr: String;
  Counter: DWORD;

begin
  Counter := 0;
  Try
    // Print the banner
    WriteLn('PipeList v1.01 by Mark Russinovich');
    WriteLn('http://www.sysinternals.com');
    WriteLn('Delphi ported by HandleX (alex_wh@mail.ru)');
    WriteLn;

    // Locate NTDLL entry points
    pipes := GetModuleHandle('ntdll');
    NtQueryDirectoryFile := GetProcAddress(pipes, 'NtQueryDirectoryFile');
    Win32Check(@NtQueryDirectoryFile <> Nil);

    RtlNtStatusToDosError := GetProcAddress(pipes, 'RtlNtStatusToDosError');
    Win32Check(@RtlNtStatusToDosError <> Nil);

    // Perform a directory listing of the existing pipe
    pipes := CreateFile('\\.\pipe\', GENERIC_READ,
      FILE_SHARE_READ or FILE_SHARE_WRITE or FILE_SHARE_DELETE,
      Nil, OPEN_EXISTING, 0, 0);
    Win32Check(pipes <> INVALID_HANDLE_VALUE);
    Try
      // Print the header
      WriteLn(FoldString('Pipe Name', 50), FoldString('Instances', 14), 'Max Instances');
      WriteLn(FoldString('---------', 50), FoldString('---------', 14), '-------------');

      // Query until no more files
      GetMem(dirInfo, DIRBUFSIZE);
      Repeat
        // Do a native directory listing
        antStatus := NtQueryDirectoryFile(pipes, 0, Nil, Nil, @ioStatus,
          dirInfo, DIRBUFSIZE, FileDirectoryInformation,
          False, Nil, firstQuery);
        if antStatus = STATUS_NO_MORE_FILES Then Break;
        NT_Check(antStatus);

        // Dump the information nicely formatted
        curEntry := dirInfo;
        Repeat
          Inc(Counter);
          aStr := WideCharLenToString(@curEntry.FileName, CurEntry.FileNameLength Div 2);
          Write(FoldString(aStr, 50));

          Write(FoldString(IntToStr(curEntry.EndOfFile.LowPart), 14));
          WriteLn(curEntry.AllocationSize.LowPart);

          If curEntry.NextEntryOffset = 0 Then Break
          Else curEntry := Pointer(DWORD(curEntry) + curEntry.NextEntryOffset);
        Until False;

        firstQuery := FALSE;
      Until False;
    Finally
      CloseHandle(pipes);
    End;
  Except
    WriteLn;
    WriteLn(Format('Exception raised class %s with message:'#$d#$a'%s', [ExceptObject.ClassName, Exception(ExceptObject).Message]));
    Write('Press Enter to terminate the application...');
    ReadLn;
  End;
  WriteLn(FoldString('', 74, '-'));
  WriteLn(Counter, ' pipes printed.');
end.

