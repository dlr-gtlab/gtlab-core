net use g: \\kpfiler1.intra.dlr.de\AT-KP
copy features\gitlab_windows_local_settings512.pri local_settings.pri

set path=%path%;%qtdir%\bin;C:\Program Files (x86)\Microsoft Visual Studio\2017\BuildTools\VC\Tools\MSVC\14.10.25017\bin\HostX64\x64;C:\Program Files (x86)\Windows Kits\10\bin\x64
set LIB=%LIB%;C:\Program Files (x86)\Microsoft Visual Studio\2017\BuildTools\VC\Tools\MSVC\14.10.25017\lib\x64;C:\Program Files (x86)\Windows Kits\10\lib\10.0.14393.0\ucrt\x64;C:\Program Files (x86)\Windows Kits\10\lib\10.0.14393.0\um\x64;
set LIBPATH=%LIBPATH%;C:\Program Files (x86)\Microsoft Visual Studio\2017\BuildTools\VC\Tools\MSVC\14.10.25017\lib\x64;C:\Program Files (x86)\Windows Kits\10\lib\10.0.14393.0\ucrt\x64;C:\Program Files (x86)\Windows Kits\10\lib\10.0.14393.0\um\x64;
set INCLUDE=%INCLUDE%;C:\Program Files (x86)\Microsoft Visual Studio\2017\BuildTools\VC\Tools\MSVC\14.10.25017\include;C:\Program Files (x86)\Windows Kits\10\include\10.0.14393.0\ucrt;C:\Program Files (x86)\Windows Kits\10\include\10.0.14393.0\shared;C:\Program Files (x86)\Windows Kits\10\include\10.0.14393.0\um;C:\Program Files (x86)\Windows Kits\10\include\10.0.14393.0\winrt;

set LIB=%LIB%;C:\Qt\5.12.6\msvc2017_64\lib
set LIBPATH=%LIBPATH%;C:\Qt\5.12.6\msvc2017_64\lib
set INCLUDE=%INCLUDE%;C:\Qt\5.12.6\msvc2017_64\include;C:\Qt\5.12.6\msvc2017_64\include\QtTest

C:\\Qt\\5.12.6\\msvc2017_64\\bin\\qmake.exe gtlab.pro -spec win32-msvc "CONFIG+=debug"
C:\Qt\Tools\QtCreator\bin\jom.exe -f Makefile qmake_all
C:\Qt\Tools\QtCreator\bin\jom.exe
