@if exist %2%3 del /F /Q %2%3
@if not exist %2 mkdir %2
copy /b %1 %2%3
