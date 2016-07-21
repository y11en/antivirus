@if not "%_BUILDARCH%"=="AMD64" goto Exit

@md obj%BUILD_ALT_DIR%
ml64 /c /Fo obj%BUILD_ALT_DIR%\osspec_asm.obj osspec_asm.asm

:Exit

build
