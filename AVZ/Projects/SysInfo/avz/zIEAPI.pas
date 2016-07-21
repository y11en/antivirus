unit zIEAPI;


interface

// Очиска кеша IE через API
function ClearIECache : boolean;

// Вычисление количества элементов в кеше IE через API
function GetIECacheItemsCount : integer;

implementation
uses windows, wininet;

// очиска кеша IE через API
function ClearIECache : boolean;
var
  lpEntryInfo: PInternetCacheEntryInfo;
  hCacheDir: LongWord;
  dwEntrySize: LongWord;
  dwLastError: LongWord;
begin
  Result := false;
  dwEntrySize := 0;
  FindFirstUrlCacheEntry(nil, TInternetCacheEntryInfo(nil^), dwEntrySize);
  GetMem(lpEntryInfo, dwEntrySize);
  hCacheDir := FindFirstUrlCacheEntry(nil, lpEntryInfo^, dwEntrySize);
  if (hCacheDir <> 0) then
    DeleteUrlCacheEntry(lpEntryInfo^.lpszSourceUrlName);
  FreeMem(lpEntryInfo);
  repeat
    dwEntrySize := 0;
    FindNextUrlCacheEntry(hCacheDir, TInternetCacheEntryInfo(nil^),
      dwEntrySize);
    dwLastError := GetLastError();
    if (GetLastError = ERROR_INSUFFICIENT_BUFFER) then
    begin
      GetMem(lpEntryInfo, dwEntrySize);
      if (FindNextUrlCacheEntry(hCacheDir, lpEntryInfo^, dwEntrySize)) then
        DeleteUrlCacheEntry(lpEntryInfo^.lpszSourceUrlName);
      FreeMem(lpEntryInfo);
    end;
  until (dwLastError = ERROR_NO_MORE_ITEMS);
  FindCloseUrlCache(hCacheDir);
  Result := true;
end;

// очиска кеша IE через API
function GetIECacheItemsCount : integer;
var
  lpEntryInfo: PInternetCacheEntryInfo;
  hCacheDir: LongWord;
  dwEntrySize: LongWord;
  dwLastError: LongWord;
begin
  Result := 0;
  dwEntrySize := 0;
  FindFirstUrlCacheEntry(nil, TInternetCacheEntryInfo(nil^), dwEntrySize);
  GetMem(lpEntryInfo, dwEntrySize);
  hCacheDir := FindFirstUrlCacheEntry(nil, lpEntryInfo^, dwEntrySize);
  if (hCacheDir <> 0) then
   inc(Result);
  FreeMem(lpEntryInfo);
  repeat
    dwEntrySize := 0;
    FindNextUrlCacheEntry(hCacheDir, TInternetCacheEntryInfo(nil^),  dwEntrySize);
    dwLastError := GetLastError();
    if (GetLastError = ERROR_INSUFFICIENT_BUFFER) then
    begin
      GetMem(lpEntryInfo, dwEntrySize);
      if (FindNextUrlCacheEntry(hCacheDir, lpEntryInfo^, dwEntrySize)) then
       inc(Result);
      FreeMem(lpEntryInfo);
    end;
  until (dwLastError = ERROR_NO_MORE_ITEMS);
  FindCloseUrlCache(hCacheDir);
end;

end.
