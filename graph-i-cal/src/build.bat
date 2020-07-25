IF NOT EXIST ..\build MKDIR ..\build
PUSHD ..\build
IF NOT EXIST .\win32 MKDIR win32
PUSHD win32
dir
cl /MD /Zi /FC /nologo ..\..\src\main.cpp /I ..\..\src\include  /link /ignore:4099 /LTCG ..\..\src\libs\win32\msvc\raylib.lib ..\..\src\libs\win32\msvc\glfw3.lib ..\..\src\libs\win32\msvc\OpenAL32.lib gdi32.lib user32.lib shell32.lib winmm.lib

IF NOT EXIST .\assets mkdir .\assets
XCOPY /S /I /Y ..\..\assets .\assets

IF NOT EXIST .\styles mkdir .\styles
XCOPY /S /I /Y ..\..\styles .\styles

COPY /Y ..\..\src\libs\win32\msvc\OpenAL32.dll .\

PUSHD ..\..\src
