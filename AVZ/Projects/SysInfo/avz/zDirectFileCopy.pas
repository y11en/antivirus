unit zDirectFileCopy;

interface

function DirectCopyFile(AFileName, ANewName : String) : boolean;

implementation
uses Windows, Messages, SysUtils, Variants, Classes, Graphics, Controls, RxVerInf,
     Registry, Dialogs, zSharedFunctions, math, ntdll;

type

 // STARTING_VCN_INPUT_BUFFER
 STARTING_VCN_INPUT_BUFFER = packed record
   StartingVcn : LARGE_INTEGER;
 end;

 TExtent = packed record
   NextVcn     : LARGE_INTEGER;
   Lcn         : LARGE_INTEGER;
 end;

 RETRIEVAL_POINTERS_BUFFER = packed record
   ExtentCount : DWORD;
   StartingVcn : LARGE_INTEGER;
   Extents     : packed array [0..0] of TExtent;
 end;

 TClusterInfo = record
  StartingLcn  : LARGE_INTEGER;
  ClustLen     : DWORD;
 end;

function LargeIntToInt64(LI : LARGE_INTEGER) : int64;
begin
 Result := LI.LowPart shl 32;
 Result := Result + LI.HighPart;
end;

function Int64ToLargeInt(I64 : int64): LARGE_INTEGER;
begin
 Result.HighPart := I64 and $0FFFFFFFF;
 Result.LowPart  := (I64 shr 32) and $0FFFFFFFF;
end;

function CtlCode(DeviceType, Funcshun, Method, Access: DWORD): DWORD;
begin
  Result := (DeviceType shl 16) or (Access shl 14) or (Funcshun shl 2) or Method;
end;

function DirectCopyFile(AFileName, ANewName : String) : boolean;
Const
  FILE_DEVICE_FILE_SYSTEM = $00000009;
  METHOD_NEITHER = 3;
  FILE_ANY_ACCESS = 0;
var
  hFile  : THandle;
  InputBuf : STARTING_VCN_INPUT_BUFFER;
  OutBuf     : Pointer;
  OutBufSize, Bytes, FileSize : DWord;
  FSCTL_GET_RETRIEVAL_POINTERS: DWord;
  lpSectorsPerCluster, lpBytesPerSector, lpNumberOfFreeClusters, lpTotalNumberOfClusters, ClusterSize, ClusterCount  : DWORD;
  Clusters : array of TClusterInfo;
  Buf      : packed array of byte;
  i,j : integer;
  FS : TFileStream;
  Offset : LARGE_INTEGER;
  CopySize : DWORD;
  lpMaximumComponentLength, lpFileSystemFlags: DWORD;
  FileSystemType : array[0..32] of char;
  FileSystemS : string;
  DataRel : dword;

  maxRootEntries  : WORD;
  reservedSectors : WORD;
	FATcount        : BYTE;
  sectorsPerFAT, rootDirSectors, firstDataSector   : DWORD;
begin
  Result := false;
  if not(ISNT) then exit;
  DataRel := 0;
  hFile := CreateFile(pchar('\\.\'+AFileName),1,
                 FILE_SHARE_READ or FILE_SHARE_WRITE or FILE_SHARE_DELETE,
                 Nil, Open_Existing,0,0);
  // Попытка открыть файл провалилась - попробуем усилить права доступа
  if hFile = INVALID_HANDLE_VALUE then
   hFile := CreateFile(PChar(AFileName),
                     READ_CONTROL, 0, nil,
                     OPEN_EXISTING,  FILE_FLAG_BACKUP_SEMANTICS, 0);
  if hFile = INVALID_HANDLE_VALUE then begin
      exit;
  end;
  // Запрос размера файла
  FileSize   := GetFileSize(hFile, nil);
  // Файлы размером до 4096 игнорируем
  if FileSize < 4096 then begin
   CloseHandle(hFile);
   exit;
  end;
  // Файлы размером > 20 мб игнорируем
  if FileSize > 20*1024*1024 then begin
   CloseHandle(hFile);
   exit;
  end;
  // Определение данных о диске
  GetDiskFreeSpace(PChar(copy(AFileName, 1, 3)), lpSectorsPerCluster, lpBytesPerSector, lpNumberOfFreeClusters, lpTotalNumberOfClusters);
  // Вычисление размера кластера и объема буфера для цепочки кластеров
  ClusterSize := lpSectorsPerCluster *  lpBytesPerSector;
  OutBufSize := sizeof(RETRIEVAL_POINTERS_BUFFER) + (FileSize div ClusterSize + 1) * SizeOf(TExtent)+1024;
  // Выделение памяти
  GetMem(OutBuf, OutBufSize);
  ZeroMemory(OutBuf, OutBufSize);
  FSCTL_Get_Retrieval_Pointers := CTLCODE(FILE_DEVICE_FILE_SYSTEM, 28, METHOD_NEITHER, FILE_ANY_ACCESS);
  // Запрос цепочки кластеров
  InputBuf.StartingVcn.QuadPart := 0;
  InputBuf.StartingVcn.LowPart  := 0;
  if DeviceIoControl(hFile, FSCTL_GET_RETRIEVAL_POINTERS,
                     @InputBuf, SizeOF(InputBuf),
                     OutBuf, OutBufSize,Bytes, Nil)  then
  begin
   ClusterCount := (FileSize + ClusterSize - 1) div ClusterSize;
   SetLength(Clusters, RETRIEVAL_POINTERS_BUFFER(OutBuf^).ExtentCount);
   Clusters[0].ClustLen                := LargeIntToInt64(RETRIEVAL_POINTERS_BUFFER(OutBuf^).Extents[0].NextVcn) - LargeIntToInt64(InputBuf.StartingVcn);
   Clusters[0].StartingLcn.QuadPart    := LargeIntToInt64(RETRIEVAL_POINTERS_BUFFER(OutBuf^).Extents[0].Lcn);
   if RETRIEVAL_POINTERS_BUFFER(OutBuf^).ExtentCount > 1 then
    for i := 1 to RETRIEVAL_POINTERS_BUFFER(OutBuf^).ExtentCount-1 do begin
     {$R-}
     Clusters[i].ClustLen               := LargeIntToInt64(RETRIEVAL_POINTERS_BUFFER(OutBuf^).Extents[i].NextVcn) - LargeIntToInt64(RETRIEVAL_POINTERS_BUFFER(OutBuf^).Extents[i-1].NextVcn);
     Clusters[i].StartingLcn.QuadPart   := LargeIntToInt64(RETRIEVAL_POINTERS_BUFFER(OutBuf^).Extents[i].Lcn);
     {$R+}
    end;
  end;
  FreeMem(OutBuf, OutBufSize);
  CloseHandle(hFile);
  // **** Чтение файла ***
  // Определение данных о диске
  windows.GetVolumeInformation(PChar(copy(AFileName, 1, 3)), nil, 0,
                       nil,
                       lpMaximumComponentLength, lpFileSystemFlags,
                       FileSystemType, 32);
  FileSystemS := UpperCase(Trim(FileSystemType));
  // Открытие диска
  hFile := CreateFile(pchar('\\.\'+copy(AFileName, 1, 2)), GENERIC_READ,
                     FILE_SHARE_READ or FILE_SHARE_WRITE, nil, OPEN_EXISTING, 0, 0);
  if hFile = INVALID_HANDLE_VALUE then exit;
  if (FileSystemS = 'FAT') or (FileSystemS = 'FAT32') then begin
   // Позиционирование на сектор 0
   Offset.QuadPart := 0;
   SetFilePointer(hFile, Longint(Offset.LowPart), @Offset.HighPart, FILE_BEGIN);
   // Чтение сектора 0
   SetLength(Buf, ClusterSize);
   ReadFile(hFile, Buf[0], ClusterSize, Bytes, 0);
   // Извлечение из Boot сектора maxRootEntries, reservedSectors и FATcount
   CopyMemory(@maxRootEntries,  pointer(dword(@Buf[0]) + 17), sizeof(WORD));
 	 CopyMemory(@reservedSectors,  pointer(dword(@Buf[0]) + 14), sizeof(WORD));
   CopyMemory(@FATcount,  pointer(dword(@Buf[0]) + 16), sizeof(BYTE));
   // Определение количества секторов на FAT (в зависимоти от типа FAT или FAT32)
   if (FileSystemS = 'FAT') then
    CopyMemory(@sectorsPerFAT,  pointer(dword(@Buf[0]) + 22), sizeof(WORD))
   else
    CopyMemory(@sectorsPerFAT,  pointer(dword(@Buf[0]) + 36), sizeof(DWORD));
   // Вычисление количества секторов в RootDirectory
   rootDirSectors  := ((maxRootEntries * 32) + (lpBytesPerSector - 1)) div lpBytesPerSector;
   // Вычисление метоположения первого сектора данных
   firstDataSector := reservedSectors + (FATcount * sectorsPerFAT) + rootDirSectors;
   // Рассчет смещения в байтах
   DataRel := firstDataSector * lpBytesPerSector;
  end;
  // Создание файлового потока
  FS := TFileStream.Create(ANewName, fmCreate);
  try
    SetLength(Buf, ClusterSize);
    CopySize := 0;
    for i := 0 to length(Clusters)-1 do begin
     Offset.QuadPart := DataRel + ClusterSize * Clusters[i].StartingLcn.QuadPart;
     SetFilePointer(hFile, Longint(Offset.LowPart), @Offset.HighPart, FILE_BEGIN);
     for j := 1 to Clusters[i].ClustLen do begin
      ReadFile(hFile, Buf[0], ClusterSize, Bytes, 0);
      bytes := min(Bytes, FileSize - CopySize);
      inc(CopySize, ClusterSize);
      FS.Write(Buf[0], Bytes)
     end;
    end;
    Result := true;
  finally
   FS.Free;
  end;
end;


end.
